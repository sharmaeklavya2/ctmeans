#include <iostream>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "matrix.h"
#include "ctmeans.h"
#include "vecio.h"
#include "kdtree.h"
#include "nniter_kd.h"

const char* usage = "ctmeans n_clusters [n_reps] [use_kd] [max_epochs] [eps_obj] [max_t] [eps_t] [seed]";

using namespace std;

double get_elapsed(double start_clock, double end_clock) {
    return double(end_clock - start_clock) / CLOCKS_PER_SEC;
}

int test_nniter_kd() {
    int n=6, d=2;
    Matrix X(n, d);
    X(0, 0) = 2; X(0, 1) = 3;
    X(1, 0) = 5; X(1, 1) = 4;
    X(2, 0) = 9; X(2, 1) = 6;
    X(3, 0) = 4; X(3, 1) = 7;
    X(4, 0) = 8; X(4, 1) = 1;
    X(5, 0) = 7; X(5, 1) = 2;

    NNIterKD nniter(X);

    print_kd_tree(stdout, nniter.root, 0);

    /* Expected output:

KD(5, min=[2, 1], max=[9, 7])
  KD(1, min=[2, 3], max=[5, 7])
    KD(0, min=[2, 3], max=[2, 3])
    KD(3, min=[4, 7], max=[4, 7])
  KD(2, min=[8, 1], max=[9, 6])
    KD(4, min=[8, 1], max=[8, 1])

    */

    KDHeapElem he;

    while(true) {
        fprintf(stderr, "Enter a point: ");
        vector<double> p;
        for(int i=0; i<d; ++i) {
            double p_i;
            scanf("%lf", &p_i);
            p.push_back(p_i);
        }

        nniter.set_point(p.data());

        for(int i=0; !nniter.pq.empty(); ++i) {
            he = nniter.get_neighbor_node();
            he.print(stdout);
        }
    }

    /*
    Expected output for input "6 4":

KDHeapElem(type=k, point=5, prio=0)
KDHeapElem(type=k, point=1, prio=1)
KDHeapElem(type=p, point=1, prio=1)
KDHeapElem(type=k, point=2, prio=4)
KDHeapElem(type=p, point=5, prio=5)
KDHeapElem(type=p, point=2, prio=13)
KDHeapElem(type=p, point=3, prio=13)
KDHeapElem(type=p, point=4, prio=13)
KDHeapElem(type=p, point=0, prio=17)
    */
    return 0;
}

/*
int main() {
    test_nniter_kd();
}
*/

void output_vec_vec_to_file(const char* fname, const char* name, int n, int c, const vector<vector<int> >& V) {
    long long sum = 0;
    int count = 0;
    FILE* fp = fopen(fname, "w");
    for(const vector<int>& v: V) {
        if(v.size() > 0) {
            count += v.size();
            sum += v[0];
            fprintf(fp, "%lf", double(v[0])/n);
            for(unsigned i=1; i<v.size(); ++i) {
                sum += v[i];
                fprintf(fp, " %lf", double(v[i])/n);
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    double avg = double(sum) / count;
    printf("Average %s: %lf, %lf, %lf\n", name, avg, avg/n, avg/n/c);
}

#ifdef DEBUG
#include <fenv.h>
#endif

int main(int argc, char* argv[]) {
    // parse command-line args

#ifdef DEBUG
    feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);
#endif
    int c, reps=0, max_epochs=0, max_t=0, seed=0;
    double eps_obj=0, eps_t=0;
    bool use_kd = true;

    if(argc < 2 || argc > 9) {
        fprintf(stderr, "usage: %s\n", usage);
        return 1;
    }
    else {
        c = atoi(argv[1]);
        switch(argc) {
        case 9:
            seed = atoi(argv[8]);
            srand(seed);
        case 8:
            eps_t = atof(argv[7]);
        case 7:
            max_t = atoi(argv[6]);
        case 6:
            eps_obj = atof(argv[5]);
        case 5:
            max_epochs = atoi(argv[4]);
        case 4:
            use_kd = bool(atoi(argv[3]));
        case 3:
            reps = atoi(argv[2]);
        }

        if(reps <= 0) {reps = 1;}
        if(max_epochs <= 0) {max_epochs = 300;}
        if(eps_obj <= 0.0) {eps_obj = 0.0001;}
        if(max_t <= 0) {max_t = c;}
        if(eps_t <= 0.0) {eps_t = 0.0;}
        if(seed <= 0) {srand(time(NULL));}
    }

    // read input

    FILE* fp = NULL;
    int n, d;

    fp = fopen("var/in.shape.txt", "r");
    if(fp == NULL) {
        perror("main: couldn't read shape");
        return 1;
    }
    fscanf(fp, "%d%d", &n, &d);
    fclose(fp);

    Matrix X(n, d);
    clock_t start_clock = clock();
    loadtxt("var/in.txt", X);
    clock_t end_clock = clock();
    printf("Time to load data: %lg\n", get_elapsed(start_clock, end_clock));

    // cluster

    CTMeans model(X, c, 2, max_t, eps_t);
    start_clock = clock();
    double obj = model.cluster(use_kd, reps, max_epochs, eps_obj, stderr, 3);
    end_clock = clock();
    printf("Objective: %lg\n", obj);
    printf("Time to cluster: %lg\n", get_elapsed(start_clock, end_clock));

    // write output

    start_clock = clock();
    // Matrix U(n, c, true);
    // model.get_dense_U(U);
    vector<int> labels;
    model.get_labels(labels);
    vector<double> flatu;
    model.get_flat_U(flatu);

    savetxt("var/centroids.txt", model.C);
    savetxt("var/labels.txt", labels.cbegin(), labels.cend());
    // savetxt("var/memberships.txt", U);
    savetxt("var/flatu.txt", flatu.cbegin(), flatu.cend());
    printf("flatu size: %zd, %lf, %lf\n", flatu.size(),
        double(flatu.size())/n, double(flatu.size())/n/c);
    end_clock = clock();

    output_vec_vec_to_file("var/sigc.txt", "SigC", n, c, model.SigC);
    if(use_kd) {
        output_vec_vec_to_file("var/heap_ops.txt", "heap_ops", n, c, model.P);
    }
    printf("Time to save: %lg\n", get_elapsed(start_clock, end_clock));

    return 0;
}

/*
#include "gen_gmix.h"

int main() {
    srand(time(NULL));
    savetxt("secret/small.txt", gen_2d_gmix(10, 10, 30, 40, 5));
    savetxt("secret/big.txt", gen_2d_gmix(100, 100, 3000, 40, 5));
    return 0;
}
*/
