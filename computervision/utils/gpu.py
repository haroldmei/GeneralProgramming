
import torch

devices = {}
def get_gpu(t_id, gpu = ''):
    
    if t_id in devices:
        return devices[t_id]
    
    num_gpu = torch.cuda.device_count()
    if not gpu:
        gpu = ','.join(str(x) for x in range(num_gpu))
    gpu = gpu.split(',')

    num_devices = len(gpu)
    devices[t_id] = 'cuda:' + gpu[(len(devices)) % num_devices]
    
    return devices[t_id]
