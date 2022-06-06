import numpy as np
import gzip
import json
import logging

def load_model(fmodel, dim = 3,  dtype=np.float32):
    with open(fmodel, 'rb') as f:
        df = np.frombuffer(f.read(), dtype=dtype)
        df = df.reshape((-1, dim))
    return df

def save_model(fmodel, model):
    with open(fmodel, 'wb') as f:
        f.write(np.ndarray.tobytes(model))


def load_bin(f_bin, dim, dtype=np.int32):
    with open(f_bin, 'rb') as f:
        df = np.frombuffer(f.read(), dtype=dtype)
        df = df.reshape(dim)
    return df

def save_bin(f_bin, model):
    with open(f_bin, 'wb') as f:
        f.write(np.ndarray.tobytes(model))


def load_gz(f_gz, dim, dtype=np.int32):
    with gzip.open(f_gz, 'rb') as f:
        df = np.frombuffer(f.read(), dtype=dtype)
        df = df.reshape(dim)
    return df


def coco_to_seg(fname: str, mask_id: str, background: int = -1):
    with open(fname) as f:
        coco = json.loads(f.read())
    image_id_to_ann_index = {}
    for i, annotation in enumerate(coco.get('annotations', [])):
        try:
            image_id_to_ann_index[annotation['image_id']].append(i)
        except KeyError:
            try:
                image_id_to_ann_index[annotation['image_id']] = []
                image_id_to_ann_index[annotation['image_id']].append(i)
            except KeyError:
                logging.error("Annotation %s is missing key: 'image_id'", annotation)
    imgs = []
    for image_entry in coco.get('images', []):
        if not image_entry['id'] in image_id_to_ann_index:
            image_id_to_ann_index[image_entry['id']] = []
        anns = [coco['annotations'][i] for i in image_id_to_ann_index[image_entry['id']]]
        shape = image_entry['height'], image_entry['width']
        img = annotations_to_mask(anns, shape, id_type_string=mask_id, background=background)
        imgs.append(img)
    return np.array(imgs)

