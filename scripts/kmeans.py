#!/usr/bin/env python

"""
Cluster in.txt using k means from scikit-learn,
output labels in labels.txt
and output centroids in centroids.txt
"""

from __future__ import print_function

import sys
import os
from os.path import join as pjoin
import argparse

import numpy as np
import sklearn.cluster

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(pjoin(BASE_DIR, 'scripts'))

from timer import Timer

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('out_dir')
    parser.add_argument('n', help='Number of clusters', type=int)
    parser.add_argument('reps', type=int, default=10, help='number of repetitions')
    parser.add_argument('seed', type=int, default=-1, nargs='?')
    args = parser.parse_args()

    os.chdir(args.out_dir)
    if args.seed == -1:
        args.seed = None

    with Timer('load data'):
        X = np.loadtxt('in.txt')

    model = sklearn.cluster.KMeans(n_clusters=args.n, n_init=args.reps,
        init='random', tol=0.0001, max_iter=300, algorithm='full',
        random_state=args.seed)
    with Timer('cluster'):
        model.fit(X)
    print('Objective:', model.inertia_)

    np.savetxt('labels.txt', model.labels_)
    np.savetxt('centroids.txt', model.cluster_centers_)

if __name__ == '__main__':
    main()
