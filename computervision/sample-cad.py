import numpy as np
import torch
import argparse

from utils.fio import load_bin, save_bin

DESCRIPTION = """
"""

def area(d1, d2) :
    return 0.5 * torch.norm( torch.cross( d1, d2 ), dim = 1 )

# See if there is still possible optimization space
def from_cad(model, unit = 0.0025):
    device = torch.device('cuda')
    model = torch.tensor(model).to(device)
    first_points = torch.tensor([]).to(device) #model[:,1:4] # keep this vetex
    d1 = model[:,4:7] - model[:,1:4]
    d2 = model[:,7:10] - model[:,1:4]
    areas = area(d1, d2) // unit
    areas1=torch.unique(areas)
    
    for i in range(1, len(areas1)):
        num = areas1[i]
        mask = (areas == num)
        count = torch.sum(mask)
        num -= 1
        if count <= 0:
            continue
        faces = model[mask,1:].reshape(-1,3,3)
        
        samples = torch.rand(int(count*(num+1)), 2).to(device)
        samples, _ = torch.sort(samples)
        matrix = torch.cat((samples[:,0].reshape((-1,1)),(samples[:,1]-samples[:,0]).reshape((-1,1)), 1-samples[:,1].reshape((-1,1))), 1).to(device)

        matrix = matrix.reshape((count,-1,3))
        samples = torch.matmul(matrix, faces).reshape((-1,3))
        first_points = torch.cat((first_points, samples))
    return np.array(first_points.cpu())

def model_enu_to_ned(enu_model, scale, zlimit=None):
    device = torch.device('cuda')
    id_ = torch.tensor([[1]], dtype=torch.float32).to(device)
    neg_yxz = torch.tensor([
          [0, -1, 0],
          [-1, 0, 0],
          [0, 0, -1]
        ], dtype=torch.float32, device=device) * scale
    neg_yxz_tri = torch.block_diag(id_, torch.block_diag(neg_yxz, torch.block_diag(neg_yxz, neg_yxz)))
    ned_model = torch.matmul(torch.from_numpy(enu_model).to(device), neg_yxz_tri)
    idx = torch.logical_and(torch.logical_and(ned_model[:,3]<zlimit,  ned_model[:,6]<zlimit), ned_model[:,9]<zlimit)
    ned_model = ned_model[idx,:]
    return np.array(ned_model.cpu())

output = '/tmp/model.out'

def get_args():

    parser = argparse.ArgumentParser(description=DESCRIPTION, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("file", help="Include name of file containing model data.")
    parser.add_argument("precision", type=float, help="Sepcify the output format.")
    parser.add_argument("type", help="Sepcify the output format.")

    return vars(parser.parse_args())

if __name__ == "__main__":
    args = get_args()
    df = load_bin(args["file"], 10)
    df = model_enu_to_ned(df, 1000, 10)
    df = from_cad(df, unit=args["precision"]**2)
    if args["type"] == 'csv':
        np.savetxt(output + '.csv', df, delimiter=",")
    elif args["type"] == 'bin':
        save_bin(output + '.bin', df)


