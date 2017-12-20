#include <iostream>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "matrix.h"
#include "ctmeans.h"
#include "vecio.h"

const char* usage = "ctmeans n_clusters [n_reps] [max_epochs] [eps_obj] [max_t] [eps_t] [seed]";

using namespace std;

double get_elapsed(double start_clock, double end_clock) {
    return double(end_clock - start_clock) / CLOCKS_PER_SEC;
}

int main(int argc, char* argv[]) {
    // parse command-line args

    int c, reps=0, max_epochs=0, max_t=0, seed=0;
    double eps_obj=0, eps_t=0;

    if(argc < 2 || argc > 8) {
        fprintf(stderr, "usage: %s\n", usage);
        return 1;
    }
    else {
        c = atoi(argv[1]);
        switch(argc) {
        case 8:
            seed = atoi(argv[7]);
            srand(seed);
        case 7:
            eps_t = atof(argv[6]);
        case 6:
            max_t = atoi(argv[5]);
        case 5:
            eps_obj = atof(argv[4]);
        case 4:
            max_epochs = atoi(argv[3]);
        case 3:
            reps = atoi(argv[2]);
        }

        if(reps <= 0) {reps = 1;}
        if(max_epochs <= 0) {max_epochs = 300;}
        if(eps_obj <= 0.0) {eps_obj = 0.0001;}
        if(max_t <= 0) {max_t = c;}
        if(eps_t <= 0.0) {eps_t = 0.0001;}
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
    double obj = model.cluster(reps, max_epochs, eps_obj, stderr, 3);
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
    printf("Time to save: %lg\n", get_elapsed(start_clock, end_clock));

    return 0;
}
