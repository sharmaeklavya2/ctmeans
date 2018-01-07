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

def main(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('dataset')
    parser.add_argument('out_dir', help='Path to output directory used to store clustering output')
    args = parser.parse_args(argv[1:])

    shutil.rmtree(args.out_dir, ignore_errors=True)
    os.makedirs(args.out_dir, exist_ok=True)
    os.symlink(os.path.abspath(args.dataset), pjoin(args.out_dir, 'in.txt'))
    X = np.loadtxt(args.dataset)
    n, d = X.shape
    with open(pjoin(args.out_dir, 'in.shape.txt'), 'w') as fobj:
        print(*(X.shape), file=fobj)


if __name__ == '__main__':
    main(sys.argv)
