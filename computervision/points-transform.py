import numpy as np
import torch
from utils.fio import load_model, save_model

def transform(model, mtx):
    model = torch.from_numpy(model)
    mtx = torch.from_numpy(mtx)
    model_tr = torch.matmul(model[:,1:], mtx[:3,:3]) + mtx[:3,3:].T
    model = torch.cat((model[:,0:1], model_tr), 1)
    return np.array(model)

fmtx = '/home/hmei/tmp/transform-mat.txt'
fmodel = '/model/sample.0.025.bin'
output = '/home/hmei/tmp/hm/sample.0.05.out.bin'

if __name__ == "__main__":
    model = load_model(fmodel, 4)
    mtx = np.loadtxt(fmtx, usecols=range(4), dtype=np.float32)
    model = transform(model, mtx)
    save_model(output, model)
