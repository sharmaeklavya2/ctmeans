#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "lib/matrix.h"
#include "lib/ctmeans.h"
#include "lib/vecio.h"

const char* usage = "ctmeans n_clusters [n_reps] [use_kd] [max_epochs] [eps_obj] [max_t] [eps_t] [seed]";

using namespace std;

double get_elapsed(double start_clock, double end_clock) {
    return double(end_clock - start_clock) / CLOCKS_PER_SEC;
}

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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(fp, "%d%d", &n, &d);
#pragma GCC diagnostic pop
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
