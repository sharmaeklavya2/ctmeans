#include <cmath>
#include <cstdlib>
#include <set>
#include <vector>
#include <random>

#include "matrix.h"

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
