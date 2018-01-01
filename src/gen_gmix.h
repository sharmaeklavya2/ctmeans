#ifndef _GEN_GMIX_H
#define _GEN_GMIX_H

#include <vector>
#include "matrix.h"

void get_rand_perm(long long n, int k, std::vector<int>& v);

Matrix gen_2d_gmix(int m, int n, int c, int t, double stddev);

#endif  // _GEN_GMIX_H
