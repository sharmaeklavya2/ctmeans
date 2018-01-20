#!/usr/bin/env python

"""
Benchmark or output a benchmarking script
"""

import sys
import os
from os.path import join as pjoin
import argparse
import json

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(pjoin(BASE_DIR, 'scripts'))

def main(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('datasets_fpath',
        help=('A JSON file where keys are relative (to json file)'
            ' paths to datasets in .txt files and values are number of clusters'))
    parser.add_argument('reps', type=int, help='number of repetitions')
    parser.add_argument('--script', action='store_true', default=False,
        help='Output a shell script for benchmarking')
    parser.add_argument('--seed', type=int)
    parser.add_argument('--max-epochs', type=int, default=300)
    parser.add_argument('--eps-obj', type=float, default=0.0001)
    parser.add_argument('--eps-t', type=float, default=0.1)
    args = parser.parse_args(argv[1:])

    if not args.script:
        import scripts.prepare
        import scripts.kmeans
        import subprocess

    with open(args.datasets_fpath) as fobj:
        cluster_map = json.load(fobj)

    os.chdir(BASE_DIR)
    data_dir = os.path.relpath(os.path.dirname(os.path.abspath(args.datasets_fpath)), BASE_DIR)
    if args.script:
        print('set -e\nset -x')
        print('cd', BASE_DIR)

    for data_fname, c in sorted(cluster_map.items(), key=(lambda x: (x[1], x[0]))):

        for use_fast in (True, False):
            out_dir = pjoin('output', 'bench', 'sklearn-kmeans' + ('.fast' if use_fast else '.slow'), data_fname)
            action1 = ['scripts/prepare.py', pjoin(data_dir, data_fname), out_dir]
            action2 = ['scripts/kmeans.py', out_dir, str(c), str(args.reps),
                '--max-epochs', str(args.max_epochs), '--eps-obj', str(args.eps_obj)]
            if args.seed is not None:
                action2 += ['--seed', str(args.seed)]

            print(*action1)
            if not args.script:
                scripts.prepare.main(action1)
            print(*action2)
            if not args.script:
                scripts.kmeans.main(action2)
            
        for use_kd in (True, False):
            for algo_name, max_t, eps_t in (('kmeans', 1, 0), ('ctmeans', c, args.eps_t), ('cmeans', c, 0)):
                out_dir = pjoin('output', 'bench', algo_name + ('.kd' if use_kd else '.nokd'), data_fname)
                action1 = ['scripts/prepare.py', pjoin(data_dir, data_fname), out_dir]
                action2 = ['./ctmeans', out_dir, str(c), str(args.reps), '1' if use_kd else '0',
                    str(args.max_epochs), str(args.eps_obj), str(max_t), str(eps_t)]
                if args.seed is not None:
                    action2.append(str(args.seed))
                    
                print(*action1)
                if not args.script:
                    scripts.prepare.main(action1)
                print(*action2)
                if not args.script:
                    subprocess.check_call(action2)


if __name__ == '__main__':
    main(sys.argv)
