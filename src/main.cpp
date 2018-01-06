#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>

#include <unistd.h>

#include "lib/matrix.h"
#include "lib/ctmeans.h"
#include "lib/vecio.h"

const char* usage = "ctmeans directory n_clusters [n_reps] [use_kd] [max_epochs] [eps_obj] [max_t] [eps_t] [seed]";

using namespace std;

double get_elapsed(double start_clock, double end_clock) {
    return double(end_clock - start_clock) / CLOCKS_PER_SEC;
}

double output_vec_vec_to_file(const char* fname, const char* name, int n, int c, const vector<vector<int> >& V) {
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
    return double(sum) / count;
}

#ifdef DEBUG
#include <fenv.h>
#endif

string get_json_dict_template(const char* keys, const char* val_fmts) {
    string s = "{\n";
    /*
    int n=1;
    for(int i=0; keys[i] != '\0'; ++i) {
        if(keys[i] == ',') {n++;}
    }
    s.reserve(strlen(keys) + strlen(fmts) + 8*n + 4);
    */

    if(keys[0] == '\0' || val_fmts[0] == '\0') {
        return "{}\n";
    }

    char key[50], val_fmt[10];
    int j1beg=0, j2beg=0;
    for(int i=0; true; ++i) {
        char ch1='\0', ch2='\0';
        while(keys[j1beg] == ' ') {
            j1beg++;
        }
        while(val_fmts[j2beg] == ' ') {
            j2beg++;
        }
        for(int j1off=0; true; ++j1off) {
            ch1 = keys[j1beg + j1off];
            if(ch1 == ' ') {
                continue;
            }
            if(ch1 == ',' || ch1 == '\0') {
                key[j1off] = '\0';
                j1beg += j1off + 1;
                break;
            }
            else {
                key[j1off] = ch1;
            }
        }
        for(int j2off=0; true; ++j2off) {
            ch2 = val_fmts[j2beg + j2off];
            if(ch2 == ',' || ch2 == '\0') {
                val_fmt[j2off] = '\0';
                j2beg += j2off + 1;
                break;
            }
            else {
                val_fmt[j2off] = ch2;
            }
        }
        s.append("  \"").append(key).append("\": ").append(val_fmt).append(",\n");
        if(ch1 == '\0' || ch2 == '\0') {
            break;
        }
    }
    s.pop_back(); s.pop_back();
    s.append("\n}\n");
    return s;
}

int main(int argc, char* argv[]) {
    // parse command-line args

#ifdef DEBUG
    feenableexcept(FE_ALL_EXCEPT & ~FE_INEXACT);
#endif
    int c, reps=0, max_epochs=0, max_t=0, seed=0;
    double eps_obj=0, eps_t=0;
    bool use_kd = true;
    const char* directory = nullptr;

    if(argc < 3 || argc > 10) {
        fprintf(stderr, "usage: %s\n", usage);
        return 2;
    }
    else {
        directory = argv[1];
        c = atoi(argv[2]);
        switch(argc) {
        case 10:
            seed = atoi(argv[9]);
        case 9:
            eps_t = atof(argv[8]);
        case 8:
            max_t = atoi(argv[7]);
        case 7:
            eps_obj = atof(argv[6]);
        case 6:
            max_epochs = atoi(argv[5]);
        case 5:
            use_kd = bool(atoi(argv[4]));
        case 4:
            reps = atoi(argv[3]);
        }

        if(reps <= 0) {reps = 1;}
        if(max_epochs <= 0) {max_epochs = 300;}
        if(eps_obj <= 0.0) {eps_obj = 0.0001;}
        if(max_t <= 0) {max_t = c;}
        if(eps_t <= 0.0) {eps_t = 0.0;}
        if(seed <= 0) {seed = int(time(nullptr));}
    }
    srand(seed);

    if(chdir(directory) != 0) {
        perror("main: couldn't change directory");
        return 1;
    }

    FILE* fp = NULL;

    // store cmd args

    fp = fopen("args.json", "w");
    if(fp == nullptr) {
        perror("main: couldn't write args to file");
    }
    else {
        string s = get_json_dict_template(
            " c,reps,use_kd,max_epochs,eps_obj,max_t, eps_t,seed",
            "%d,  %d,    %s,        %d, %.10lg,   %d,%.10lg,  %d");
        fprintf(fp, s.c_str(),
            c, reps, (use_kd? "true": "false"), max_epochs, eps_obj, max_t, eps_t, seed);
        fclose(fp);
    }

    // read input

    int n, d;

    fp = fopen("in.shape.txt", "r");
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
    loadtxt("in.txt", X);
    clock_t end_clock = clock();
    double time_to_load_data = get_elapsed(start_clock, end_clock);
    printf("Time to load data: %lg\n", time_to_load_data);

    // cluster

    CTMeans model(X, c, 2, max_t, eps_t);
    start_clock = clock();
    double obj = model.cluster(use_kd, reps, max_epochs, eps_obj, stderr, 3);
    end_clock = clock();
    printf("Objective: %lg\n", obj);
    double time_to_cluster = get_elapsed(start_clock, end_clock);
    printf("Time to cluster: %lg\n", time_to_cluster);

    // write output

    start_clock = clock();
    // Matrix U(n, c, true);
    // model.get_dense_U(U);
    vector<int> labels;
    model.get_labels(labels);
    vector<double> flatu;
    model.get_flat_U(flatu);

    savetxt("centroids.txt", model.C);
    savetxt("labels.txt", labels.cbegin(), labels.cend());
    // savetxt("memberships.txt", U);
    savetxt("flatu.txt", flatu.cbegin(), flatu.cend());
    printf("flatu size: %zd, %lf, %lf\n", flatu.size(),
        double(flatu.size())/n, double(flatu.size())/n/c);
    end_clock = clock();

    double avg_sigC = output_vec_vec_to_file("sigc.txt", "SigC", n, c, model.SigC);
    printf("Average %s: %lf, %lf, %lf\n", "SigC", avg_sigC, avg_sigC/n, avg_sigC/n/c);
    double avg_heap_ops = 0;
    if(use_kd) {
        avg_heap_ops = output_vec_vec_to_file("heap_ops.txt", "heap_ops", n, c, model.P);
        printf("Average %s: %lf, %lf, %lf\n", "heap_ops", avg_heap_ops, avg_heap_ops/n, avg_heap_ops/n/c);
    }
    double time_to_save = get_elapsed(start_clock, end_clock);
    printf("Time to save: %lg\n", time_to_save);

    // write times

    fp = fopen("times.json", "w");
    if(fp == nullptr) {
        perror("main: couldn't write times to file");
    }
    else {
        string s = get_json_dict_template(
            "load_data,cluster,  save",
            "   %.10lg, %.10lg,%.10lg");
        fprintf(fp, s.c_str(), time_to_load_data, time_to_cluster, time_to_save);
        fclose(fp);
    }

    // write stats

    fp = fopen("stats.json", "w");
    if(fp == nullptr) {
        perror("main: couldn't write stats to file");
    }
    else {
        string s = get_json_dict_template(
            "objective,flatu_size,avg_sigC,avg_heap_ops",
            "   %.10lg,    %.10lg,  %.10lg,      %.10lg");
        fprintf(fp, s.c_str(), obj, double(flatu.size())/n, avg_sigC/n, avg_heap_ops/n);
        fclose(fp);
    }
    return 0;
}
