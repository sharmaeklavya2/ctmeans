#!/usr/bin/env python3

"""
Run a clustering algorithm and plot the output.
"""

import sys
import os
from os.path import join as pjoin
import shutil
import argparse

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(pjoin(BASE_DIR, 'scripts'))

from timer import Timer

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('out_dir')
    parser.add_argument('--no-color', dest='color', action='store_false', default=True,
        help='Do not color points according to cluster')
    args = parser.parse_args()

    out_dir_path = pjoin(BASE_DIR, 'output', args.out_dir)

    with Timer('import modules'):
        import numpy as np
        from matplotlib import pyplot as plt
        import seaborn as sns
        sns.set()

    with Timer('read input, labels and centroids'):
        X = np.loadtxt(pjoin(out_dir_path, 'in.txt'))
        n, d = X.shape
        try:
            y = np.loadtxt(pjoin(out_dir_path, 'labels.txt'))
            assert(y.shape == (n,))
        except FileNotFoundError:
            y = None
        try:
            C = np.loadtxt(pjoin(out_dir_path, 'centroids.txt'))
            assert(C.shape[1] == d)
            c = C.shape[0]
        except FileNotFoundError:
            C = None

    with Timer('plot points'):
        plt.scatter(X[:, 0], X[:, 1], s=5, c=(y if y is not None and args.color else 'b'))
        if C is not None:
            plt.scatter(C[:, 0], C[:, 1], s=35, edgecolors='white', facecolors='none', linewidths=1)
            plt.scatter(C[:, 0], C[:, 1], s=55, edgecolors='black', facecolors='none', linewidths=1)
    plt.show()

    with Timer('plot flatu'):
        try:
            flatu = np.loadtxt(pjoin(out_dir_path, 'flatu.txt'))
        except FileNotFoundError:
            flatu = None
        if flatu is not None:
            plt.plot(np.arange(0, flatu.shape[0]) / n, flatu)
    if flatu is not None:
        plt.show()

    for fname in ('sigc.txt', 'heap_ops.txt'):
        with Timer('plot ' + fname):
            try:
                L = []
                with open(pjoin(out_dir_path, fname)) as fobj:
                    for line in fobj:
                        if line:
                            L.append([float(x) for x in line.split()])
            except FileNotFoundError:
                L = None
            if L is not None:
                for l in L:
                    plt.plot(np.arange(0, len(l)), l, '-o')
                plt.xlabel('epochs')
                plt.ylabel(fname.split('.')[0])
        if L is not None:
            plt.show()


if __name__ == '__main__':
    main()
