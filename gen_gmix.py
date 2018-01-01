#!/usr/bin/env python

"""
Generate a mixture of gaussians.
If --plot or --output is not given, the generated data matrix will be printed to stdout.
"""

import argparse
import random
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def gen_gmix(n, c, box_bounds):
    d = len(box_bounds)
    C = np.random.rand(c, d) * box_bounds
    X = np.repeat(C, n, axis=0) + np.random.randn(n*c, d)
    return X

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('c', type=int, help='Number of clusters')
    parser.add_argument('n', type=int, help='Number of points per cluster')
    parser.add_argument('d', type=int, help='Dimensionality')
    parser.add_argument('l', type=float, help='Length of bounding box')
    parser.add_argument('--seed', type=int, default=None)
    parser.add_argument('--plot', action='store_true', default=False,
        help='Plot the first 2 dimensions')
    parser.add_argument('--output')
    args = parser.parse_args()
    sns.set()

    if args.seed is not None:
        random.seed(args.seed)
        np.random.seed(args.seed)

    X = gen_gmix(args.n, args.c, np.full((args.d,), args.l))

    if args.output is not None:
        np.savetxt(args.output, X)

    if args.plot:
        plt.scatter(X[:, 0], X[:, 1], s=5)
        plt.show()

    if not args.plot and args.output is None:
        print(X)

if __name__ == '__main__':
    main()
