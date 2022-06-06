import numpy as np
import os

from utils.fio import load_bin, load_gz, save_bin

def view_points(fn: str):
    df = load_bin(fn, (-1, 3), dtype = np.float16).astype(np.float32)
    #save_bin(fn + '.bin', df)
    return df


if __name__ == "__main__":
    #view_points('/media/eliza/8T/data/parts/977')
    base_dir = '/media/eliza/8T/data'
    in_dir = '/parts'
    out_dir = '/make_parts'
    for fn in os.listdir(base_dir + in_dir + '/3'):
        df = view_points(base_dir + in_dir + '/3/' + fn)
        save_bin(base_dir + out_dir + '/' + fn, df)
