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
import json

import numpy as np
import sklearn.cluster

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(pjoin(BASE_DIR, 'scripts'))

import timeit

def main(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('out_dir')
    parser.add_argument('c', help='Number of clusters', type=int)
    parser.add_argument('reps', type=int, default=10, help='number of repetitions')
    parser.add_argument('--seed', type=int)
    parser.add_argument('--max-epochs', type=int, default=300)
    parser.add_argument('--eps-obj', type=float, default=0.0001)
    parser.add_argument('--fast', action='store_true', default=False,
        help='Use kmeans++ and elkan')
    args = parser.parse_args(argv[1:])

    os.chdir(args.out_dir)
    if args.seed == -1:
        args.seed = None

    # Save args
    init_algo = 'k-means++' if args.fast else 'random'
    cluster_algo = 'elkan' if args.fast else 'full'
    data = {
        'c': args.c,
        'reps': args.reps,
        'seed': args.seed,
        'max_epochs': args.max_epochs,
        'eps_obj': args.eps_obj,
        'init_algo': init_algo,
        'cluster_algo': cluster_algo
    }
    with open('args.json', 'w') as fobj:
        json.dump(data, fobj, indent=2, sort_keys=True)

    # Load timer
    load_time = timeit.default_timer()
    X = np.loadtxt('in.txt')
    load_time = timeit.default_timer() - load_time
    print('Time to load data:', load_time)

    # Cluster
    model = sklearn.cluster.KMeans(n_clusters=args.c, n_init=args.reps,
        init=init_algo, tol=args.eps_obj, max_iter=args.max_epochs, algorithm=cluster_algo,
        random_state=args.seed)
    cluster_time = timeit.default_timer()
    model.fit(X)
    cluster_time = timeit.default_timer() - cluster_time
    print('Time to cluster:', cluster_time)
    print('Objective:', model.inertia_)

    # Save output
    save_time = timeit.default_timer()
    np.savetxt('labels.txt', model.labels_)
    np.savetxt('centroids.txt', model.cluster_centers_)
    save_time = timeit.default_timer() - save_time

    # Save times and stats
    data = {
        'load_time': load_time,
        'cluster_time': cluster_time,
        'save_time': save_time
    }
    with open('times.json', 'w') as fobj:
        json.dump(data, fobj, indent=2, sort_keys=True)
    data = {'objective': model.inertia_}
    with open('stats.json', 'w') as fobj:
        json.dump(data, fobj, indent=2, sort_keys=True)

if __name__ == '__main__':
    main(sys.argv)
