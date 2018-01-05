#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <set>
#include <vector>
#include <random>

#include "lib/matrix.h"

void get_rand_perm(long long n, int k, std::vector<int>& v) {
    // get a list of k distinct numbers in the range [0, n-1].
    double alpha = double(k)/n;
    if(alpha > 0.5) {
        v.resize(n);
        for(int i=0; i<n; ++i) {
            v[i] = i;
        }
        for(int i=0; i<k; ++i) {
            int j = i + unsigned(std::rand()) % (n-i);
            std::swap(v[i], v[j]);
        }
        v.resize(k);
    }
    else {
        int i=0;
        std::set<int> s;
        v.resize(k);
        while(i<k) {
            int x = unsigned(std::rand()) % n;
            if(s.insert(x).second) {
                v[i] = x;
                i++;
            }
        }
    }
}

Matrix gen_2d_gmix(int m, int n, int c, int t, double stddev) {
    /* select c cluster centres from an m by n grid. Each cluster will have t points. */
    std::vector<int> v;
    int ct = c * t;
    get_rand_perm((long long)(m)*n, c, v);
    std::minstd_rand gen(rand());
    std::normal_distribution<double> z_distr;
    Matrix X(ct, 2);
    for(int i1=0; i1<c; i1++) {
        double x0 = (v[i1] % n) * stddev;
        double y0 = (v[i1] / n) * stddev;
        for(int i2=0; i2<t; ++i2) {
            X(i1*t + i2, 0) = x0 + z_distr(gen);
            X(i1*t + i2, 1) = y0 + z_distr(gen);
        }
    }
    return X;
}

const char usage[] = "gen_gmix fname m n c t stddev [seed]";

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 7 || argc > 8) {
        fprintf(stderr, "%s\n", usage);
        return 2;
    }
    const char* fname = argv[1];
    int m = atoi(argv[2]);
    int n = atoi(argv[3]);
    int c = atoi(argv[4]);
    int t = atoi(argv[5]);
    double stddev = atof(argv[6]);
    if(argc == 8) {
        srand(atoi(argv[7]));
    }
    else {
        srand(time(nullptr));
    }

    savetxt(fname, gen_2d_gmix(m, n, c, t, stddev));
    return 0;
}
