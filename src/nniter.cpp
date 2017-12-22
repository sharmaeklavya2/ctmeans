#include <cstdio>
#include <functional>
#include "nniter.h"
#include "matrix.h"

void NNIterSort::set_point(const MatrixElemType* _x_beg) {
    x_beg = _x_beg;
    counter = 0;
    for(int i=0; i<c; ++i) {
        norms[i].second = i;
        double dist = 0;
        for(int j=0; j<d; ++j) {
            dist += (x_beg[j] - C(i, j)) * (x_beg[j] - C(i, j));
        }
        norms[i].first = dist;
    }
    // std::sort(norms.begin(), norms.end());
    std::make_heap(norms.begin(), norms.end(), std::greater<dipair>());
}

typename NNIterSort::dipair NNIterSort::get_neighbor() {
    if(counter >= c) {
        fprintf(stderr, "NNIterSort: counter overflow\n");
    }
    // return norms[counter++];
    std::pop_heap(norms.begin(), norms.end() - counter, std::greater<dipair>());
    return norms[c - (++counter)];
}
