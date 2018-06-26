#!/usr/bin/env python3

"""
Plot the output of a clustering algorithm.
"""

import sys
import os
from os.path import join as pjoin
import shutil
import argparse

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(pjoin(BASE_DIR, 'scripts'))

import timeit
from timer import Timer

def main(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('out_dir')
    parser.add_argument('--no-color', dest='color', action='store_false', default=True,
        help='Do not color points according to cluster')
    parser.add_argument('--pca', action='store_true', default=False,
        help='Use PCA to reduce dimensions. Default is to take the first 2 dimensions')
    parser.add_argument('--3d', dest='three_dims', action='store_true', default=False,
        help='Use 3 dimensions')
    parser.add_argument('--no-flatu', dest='flatu', action='store_false', default=True)
    parser.add_argument('--no-points', dest='points', action='store_false', default=True)
    args = parser.parse_args(argv[1:])
    args = parser.parse_args(argv[1:])
    args = parser.parse_args(argv[1:])
    dims = 3 if args.three_dims else 2

    with Timer('import modules'):
        import numpy as np
        from matplotlib import pyplot as plt
        if dims == 3:
            from mpl_toolkits.mplot3d import Axes3D
        import seaborn as sns
        sns.set()
        if args.pca:
            from sklearn import decomposition

    with Timer('read input, labels and centroids'):
        X = np.loadtxt(pjoin(args.out_dir, 'in.txt'))
        n, d = X.shape
        try:
            y = np.loadtxt(pjoin(args.out_dir, 'labels.txt'))
            assert(y.shape == (n,))
        except FileNotFoundError:
            y = None
        try:
            C = np.loadtxt(pjoin(args.out_dir, 'centroids.txt'))
            assert(C.shape[1] == d)
            c = C.shape[0]
        except FileNotFoundError:
            C = None

    if args.pca:
        with Timer('apply PCA'):
            pca = decomposition.PCA(n_components=dims)
            pca.fit(X)
            X = pca.transform(X)
            if C is not None:
                C = pca.transform(C)

    if args.points:
        with Timer('plot points'):
            colors = y if (y is not None and args.color) else 'b'
            if dims == 3:
                fig = plt.figure()
                ax = fig.add_subplot(111, projection='3d')
                ax.scatter(X[:, 0], X[:, 1], X[:, 2], s=5, c=colors)
                if C is not None:
                    ax.scatter(C[:, 0], C[:, 1], C[:, 2], s=35, edgecolors='white', facecolors='none', linewidths=1)
                    ax.scatter(C[:, 0], C[:, 1], C[:, 2], s=55, edgecolors='black', facecolors='none', linewidths=1)
            else:
                plt.scatter(X[:, 0], X[:, 1], s=5, c=colors)
                if C is not None:
                    plt.scatter(C[:, 0], C[:, 1], s=35, edgecolors='white', facecolors='none', linewidths=1)
                    plt.scatter(C[:, 0], C[:, 1], s=55, edgecolors='black', facecolors='none', linewidths=1)
        plt.show()

    if args.flatu:
        start_time = timeit.default_timer()
        try:
            flatu = np.loadtxt(pjoin(args.out_dir, 'flatu.txt'))
        except FileNotFoundError:
            flatu = None
        if flatu is not None:
            plt.plot(np.arange(0, flatu.shape[0]) / n, flatu)
            plot_time = timeit.default_timer() - start_time
            print('Time to plot flatu:', plot_time)
            plt.show()

    for fname in ('sigc.txt', 'heap_ops.txt'):
        start_time = timeit.default_timer()
        try:
            L = []
            with open(pjoin(args.out_dir, fname)) as fobj:
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
            plot_time = timeit.default_timer() - start_time
            print('Time to plot {}:'.format(fname), plot_time)
            plt.show()


if __name__ == '__main__':
    main(sys.argv)
