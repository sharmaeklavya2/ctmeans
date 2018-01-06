#!/usr/bin/env python3

"""
Prepare the environment to run a clustering algorithm
"""

import sys
import os
from os.path import join as pjoin
import shutil
import argparse

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

import numpy as np

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('dataset')
    parser.add_argument('out_dir', help='Name of output directory used to store clustering output')
    args = parser.parse_args()

    out_dir_path = pjoin(BASE_DIR, 'output', args.out_dir)
    shutil.rmtree(out_dir_path, ignore_errors=True)
    os.makedirs(out_dir_path, exist_ok=True)
    os.symlink(os.path.abspath(args.dataset), pjoin(out_dir_path, 'in.txt'))
    X = np.loadtxt(args.dataset)
    n, d = X.shape
    with open(pjoin(out_dir_path, 'in.shape.txt'), 'w') as fobj:
        print(*(X.shape), file=fobj)


if __name__ == '__main__':
    main()
