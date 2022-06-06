#!/usr/bin/env python3
import os
import numpy as np
import argparse
import torch
import threading
import time
import sys
import traceback

from threading import Lock
from queue import Queue
from concurrent.futures import ThreadPoolExecutor

from utils.fio import load_gz, coco_to_seg
from utils.gpu import get_gpu

from abyss_fabric.asset_parts import func_dy_overlap_plus_cylinder_v2

DESCRIPTION = """
Group assets by types and output to different asset files, each type/asset_id per output.
Asset types: 
    #0,unlabelled
    1,pressure vessel
    2,line
    3,flange
    4,pipe support
    #5,drain
    #6,bridle
    #7,tank pump
    #8,sandjet
    #9,tankfill
"""

mutex = Lock()
mutex_gpu = Lock()
file_locks = {}
annotations = {}
flanges = {}
pipe_supports = {}
asset_types = {
    #0: 'unlabelled',
    1: 'pressure-vessel',
    2: 'line',
    3: 'flange',
    4: 'pipe-support',
    #5: 'drain',
    #6: 'bridle',
    #7: 'tank-pump',
    #8: 'sandjet',
    #9: 'tankfill'
}

consumer_queue = Queue()
def thread_consumer(base: str, index: int):
    while True:
        item = consumer_queue.get()

        content, scan_idx, face_id, asset_id, type_id = item
        if asset_id == -1:
            break

        the_id = '{}-{}'.format(asset_id, type_id)
        if the_id not in file_locks:
            file_locks[the_id] = Lock()

        file_locks[the_id].acquire()
        #if id_and_type not in file_handles:

        # keep flanges and supports in ram
        if type_id == 3:
            if asset_id not in flanges:
                flanges[asset_id] = content
            else:
                flanges[asset_id] = np.concatenate((flanges[asset_id], content), axis = 0)
            file_locks[the_id].release() 
            continue

        if type_id == 4:
            if asset_id not in pipe_supports:
                pipe_supports[asset_id] = content
            else:
                pipe_supports[asset_id] = np.concatenate((pipe_supports[asset_id], content), axis = 0)
            file_locks[the_id].release() 
            continue

        type_dir = base + '/parts/' + asset_types[type_id]
        f = open(type_dir + '/' + str(asset_id) + '.bin', 'ab')
        f.write(content)
        f.close()

        file_locks[the_id].release() 

    print("Consumer finished: ", index)
    consumer_queue.task_done()

def thread_get_parts(f_lines, f_types, p_scan, gpu = '', percentage = 1, scan_idx = 0):
    global annotations
    try:
        thread_id = threading.get_ident()
        device = torch.device(get_gpu(thread_id, gpu))
        print (scan_idx, thread_id, device, len(annotations))

        # annotation_id,asset_id
        lines = coco_to_seg(f_lines, 'annotation_id,category_id')
        lines = torch.tensor(lines).to(device)

        # type_id
        types = coco_to_seg(f_types, 'category_id')
        types = torch.tensor(types).to(device)

        # stack lines and types and free memory
        lines = torch.cat((lines, types), axis = 3)

        face_id = -1
        for face in os.listdir(p_scan):
            filename = p_scan + face
            face_id = face_id + 1
            dface = load_gz(filename, (-1, 9)) # fijxyzdlp
            dface = torch.tensor(dface).to(device)
            if not dface.shape[0]: # what do we do?
                continue
            
            dface = dface[torch.randint(dface.shape[0], (int(dface.shape[0] * percentage / 100), ))]    # down sample it

            t1 = lines[dface[:,0].type(torch.LongTensor),dface[:,2].type(torch.LongTensor),dface[:,1].type(torch.LongTensor)]   # son of a bitch, ji ij who comes first?
            asset_ids = t1[torch.logical_and(~torch.eq(t1[:,1], -1.0), ~torch.eq(t1[:,2], -1.0))]  # both type id and asset id are valid
            if not asset_ids.shape[0]:
                continue
            
            unique_asset_ids = torch.unique(asset_ids[:, :3], dim=0)
            for annotation_id, asset_id, type_id in np.array(unique_asset_ids.cpu()):
                if type_id not in asset_types:
                    print('!!! Warning !!! Unknown type ', type_id)
                    continue

                mask = torch.logical_and(torch.eq(t1[:,0], annotation_id), torch.logical_and(torch.eq(t1[:,1], asset_id), torch.eq(t1[:,2], type_id)))
                curr = dface[mask]
                ones = torch.ones((curr.shape[0], 1), device = device)
                
                # exclusive access to queue
                mutex.acquire()

                center = np.mean(np.frombuffer(np.array(curr[:,[3,4,5]].cpu()), dtype=np.float32).reshape((-1,3)), axis=0)
                annotations[annotation_id] = center

                #curr = torch.cat((curr, ones * type_id, ones * scan_idx * 1000000 + curr[:, -1:]), dim = 1)
                curr = torch.cat((curr, ones * type_id, curr[:, -1:]), dim = 1) # fijxyzdlptp
                curr = np.array(curr.cpu(), dtype = np.int32)
                consumer_queue.put((curr, scan_idx, face_id, asset_id, type_id))
                mutex.release() 
    except:
        print("**************** Unexpected error on gpu: ", get_gpu(threading.get_ident(), gpu))
        print(sys.exc_info())
        traceback.print_exc()
        #_thread.interrupt_main()

def merge_clusters(v, size = 1.0, precision = 0.02):
    device = 'cuda:0'
    device1 = 'cuda:0'
    if torch.cuda.device_count() > 1:
        device1 = 'cuda:1'

    v = torch.tensor(v).to(device)
    polygons = torch.unique(v[:, -1:])

    #indexes = np.in1d(np.array(list(annotations.keys())), polygons.cpu().numpy()).nonzero()
    #annotation_centers = np.array(list(annotations.values())).reshape((-1,3))[indexes]
    #centers = torch.tensor(np.array(list(annotation_centers))).to(device)
    #dist = torch.cdist(centers, centers)
    #argwhere = (dist < size).nonzero()
    #argwhere = argwhere[argwhere[:,0] != argwhere[:,1]]
    #argwhere, _ = torch.sort(argwhere, dim = 1)
    #pairs = torch.unique(argwhere, dim = 0).reshape((-1, 2))
    if polygons.shape[0] < 2:
        return np.array(v.cpu(), dtype=np.uint32)

    print('num of polygons ', polygons.shape)
    pairs = torch.tensor(np.array([]).reshape((-1, 2)), dtype = torch.int32)
    for i in range(polygons.shape[0] - 1):
        for j in range(i, polygons.shape[0]):
            pairs = torch.cat((pairs, torch.tensor(np.array([i,j]).reshape((-1,2)))), dim=0)
    #print(pairs, polygons.shape)

    if pairs.shape[0] > 0 and pairs.shape[1] == 2:
        for ii in range(polygons.shape[0]):
            for i in range(pairs.shape[0]):
                poly_id1, poly_id2 = polygons[pairs[i]]
                if poly_id1 == poly_id2: #merged ones are the same
                    continue

                idx1 = torch.eq(v[:, -1], poly_id1)
                idx2 = torch.eq(v[:, -1], poly_id2)

                # so awkward
                pts1 = torch.tensor(np.frombuffer(np.array(v[idx1, 3:6].cpu()), dtype=np.float32)).reshape((-1, 3)).to(device1)
                pts2 = torch.tensor(np.frombuffer(np.array(v[idx2, 3:6].cpu()), dtype=np.float32)).reshape((-1, 3)).to(device1)
                if (pts1.shape[0] == 0) or (pts2.shape[0] == 0): #nothing to merge
                    continue

                #print(pts1[0], v[0])
                d = torch.cdist(pts1, pts2)
                overlap = torch.less(d, precision).nonzero()
                #overlap = torch.less(overlap[:,0], overlap[:,1])
                #print(pts1, pts2, d, precision)
                if (overlap.shape[0] / (pts1.shape[0] * pts2.shape[0]) > 0.0004):
                    #print(overlap.shape, pts1.shape, pts2.shape, poly_id1, poly_id2)
                    v[idx2, -1] = poly_id1
                    pairs[i][1] = pairs[i][0]
                    pairs[torch.eq(pairs[:,0], pairs[i][1]), 0:1] = pairs[i][0].clone()    # poly_id2 no longer exist
            old_pairs = torch.unique(pairs, dim = 0)
            if (old_pairs.shape[0] == pairs.shape[0]) and torch.all(torch.eq(old_pairs, pairs)):
                break
            pairs = old_pairs

    return np.array(v.cpu(), dtype=np.uint32)

def asset_parts(base: str, deck: str = 'PD', gpu: str = '', \
    worker: int = 2, max_scan: int = -1, consumer: int = 4, percentage: int = 1):

    basedir_asset_types = '{}/domain/assets/workflow/0.polygons/2.labelled.coco/{}/'.format(base, deck)
    basedir_asset_lines = '{}/domain/assets/workflow/1.assets/1.assets.fixing/{}/'.format(base, deck)
    basedir_lines       = '{}/domain/assets/spatial/lines/{}/'.format(base, deck)

    scan_idx = 0

    # create folders for different asset types
    for type_id in asset_types:
        type_dir = base + '/parts/' + asset_types[type_id]
        if asset_types[type_id] not in os.listdir(base + '/parts'):
            os.mkdir(type_dir)
            
    pool_consumer = ThreadPoolExecutor(max_workers = consumer)
    for i in range(consumer):
        pool_consumer.submit(thread_consumer, base, i)

    try:
        num_gpu = len(gpu.split(',')) if gpu else torch.cuda.device_count()
        pool = ThreadPoolExecutor(max_workers = worker * num_gpu)
        scan_idx = -1
        for fn in os.listdir(basedir_asset_types):
            if not max_scan:
                break

            max_scan -= 1
            scan_idx = scan_idx + 1
            # annotation_id,asset_id
            f_lines = basedir_asset_lines + fn + '.json'

            # type_id
            f_types = basedir_asset_types + fn + '/asset-types.json'

            # path of the scan
            p_scan = basedir_lines + fn + '/'

            #thread_get_parts(f_lines, f_types, p_scan, gpu, scan_idx)
            pool.submit(thread_get_parts, f_lines, f_types, p_scan, gpu, percentage, scan_idx)
    
        pool.shutdown(wait=True)
    except:
        print("**************** Unexpected error *****************")
        print(sys.exc_info())
        #sys.exit()


    print('annotations ', len(annotations))
    print('centers ', np.unique(np.array(list(annotations.values())), axis = 0).shape)

    print('Producer finished, waiting for consumers...')
    for i in range(consumer):
        consumer_queue.put((None, -1, -1, -1, -1))

    pool_consumer.shutdown(wait=True)
    
    #print('Merging flanges and supports ...')
    #for k, v in flanges.items():
    #    v = merge_clusters(v)
    #    with open(base + '/parts/flange/' + str(k) + '.bin', 'wb') as f:
    #        f.write(v)
    #
    #for k, v in pipe_supports.items():
    #    v = merge_clusters(v)
    #    with open(base + '/parts/pipe-support/' + str(k) + '.bin', 'wb') as f:
    #        f.write(v)

    print('Congrats! All work completed.')

def get_args():
    parser = argparse.ArgumentParser(description = DESCRIPTION, formatter_class = argparse.RawTextHelpFormatter)
    parser.add_argument("base", help = "The base dir of the data model.")
    parser.add_argument("--deck", type = str, default = 'PD', help = "Deck name to be processed")
    parser.add_argument("--gpu", type = str, default = '', help = "Specify the id of gpus separated by ','; empty string means using all devices.")
    parser.add_argument("--worker", type = int, default = 2, help = "Number of workers per gpu")
    parser.add_argument("--scan", type = int, default = -1, help = "Number of scans to process")
    parser.add_argument("--consumer", type = int, default = 4, help = "Number of consumers to")
    parser.add_argument("--percentage", type = float, default = 1.0, help = "Keep the percentage of data")
    return vars(parser.parse_args())

# example: base = '/media/eliza/8T/data'
if __name__ == "__main__":
    args = get_args()
    print(args)
    asset_parts(args['base'], args['deck'], args['gpu'], args['worker'], args['scan'], args['consumer'], args['percentage'])

