# CT-means

The CT-means clustering algorithm is an algorithm by [Eklavya Sharma](https://github.com/sharmaeklavya2)
which uses KD-trees to implement a fast approximation to the C-means fuzzy clustering algorithm.
You can read about the algorithm here:
https://sharmaeklavya2.github.io/dl/papers/ctmeans.pdf

This repository contains an implementation of the CT-means clustering algorithm in C++.
It also contains code for benchmarking it against other algorithms, like K-means and C-means.

## Prerequisites

This project assumes a Unix-like environment.
You may need to modify some instructions below (and perhaps also modify some scripts)
for it to work on Windows.

Install the required python 3 packages if you don't already have them:

    $ pip install -r requirements.txt

Also compile the CT-means algorithm in C++. You should have the GCC C++ compiler installed for this.

    $ make

## Quickstart

This quickstart guide will have you run the K-means (scikit-learn implementation)
and CT-means (my C++ implementation) clustering algorithms on the
[S2 dataset](https://cs.joensuu.fi/sipu/datasets/s2.txt).
This dataset has 5000 2-dimensional points with 15 natural clusters.

1.  **Create working directories:**

    Create a working directory for K-means to use. To do this, run the `prepare.py` script.
    This script takes 2 command-line arguments:
    path to the data file and path at which you want to create the working directory.
    
        $ ./scripts/prepare.py data/s2.txt output/s2-kmeans

    Similarly create a working directory for CT-means:

        $ ./scripts/prepare.py data/s2.txt output/s2-ctmeans

    Alternatively, you can do this step manually

        $ mkdir output/s2-kmeans
        $ cp data/s2.txt output/s2-kmeans/in.txt
        $ echo '5000 2' > output/s2-kmeans/in.shape.txt  # Because in.txt contains 5000 2D points

2.  **Run clustering algorithms**:

        $ ./scripts/kmeans.py output/s2-kmeans 15 1
        $ ./build/release/ctmeans output/s2-ctmeans 15

    To get info on any python script, run it with the `--help` flag.
    When the `ctmeans` executable is run without arguments, it prints its usage format.

3.  **Visualize output**:

        $ ./scripts/plot.py output/s2-kmeans
        $ ./scripts/plot.py output/s2-ctmeans

## Further Exploration

You can:

* tinker around with hyper-parameters exposed as optional command-line arguments.
* check out files in the output directory.
* explore other scripts in the `scripts` directory.
