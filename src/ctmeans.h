#ifndef _CTMEANS_H
#define _CTMEANS_H

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#include "matrix.h"

class CTMeans {
private:
    int c, m, max_t, n, d;
    double eps_t;
public:
    const Matrix& X;
    Matrix C, minC;
    std::vector<std::pair<std::vector<double>, std::vector<int> > > U;

public:
    CTMeans(
        const Matrix& _X,   // points to cluster
        int _c,             // number of clusters
        int _m,             // fuzzifier
        int _max_t,         // max number of clusters considered for a point
        double _eps_t       // min membership required for a cluster
    ): c(_c), m(_m), max_t(_max_t), n(_X.rows()), d(_X.cols()), eps_t(_eps_t), X(_X), C(c, _X.cols()), minC(c, _X.cols()) {}

    void init_centroids_rand();  // initialize centroids randomly

    double step(
        // run one iteration of CT-means. Return objective before centroids are updated
        bool update_centroids,  // whether centroids matrix C should be updated
        bool store_u           // whether membership matrix should be stored in U
    );

    double cluster(
        /*  Run CT-means several times with different starting points.
            Store the best C in minC.
            Return objective before the last centroids update
        */
        int reps=10,            // number of random initializations to start with
        int epochs=300,         // number of iterations in each run of CT-means
        double obj_tol=0.0001,  // relative reduction threshold for termination
        FILE* fp=nullptr,       // file for logging progress
        int epoch_interval=1    // log in file after every 'epoch_interval' epochs
    );


    void get_flat_U(std::vector<double>& flatu) const;
    // get a descending-sorted vector containing all non-zero memberships
    void get_labels(std::vector<int>& labels) const;
    // for each point, get cluster number of cluster closest to it
    void get_dense_U(Matrix& U2) const;
    // U2 should be initialized to 0
};

#endif  // _CTMEANS_H
