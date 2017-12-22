#ifndef _NNITER_H
#define _NNITER_H

#include <utility>
#include <algorithm>
#include <vector>
#include "matrix.h"

class NNIterSort {
private:
    int c, d;
    int counter;
public:
    Matrix C;
    const MatrixElemType *x_beg;

    typedef std::pair<double, int> dipair;
    std::vector<dipair> norms;

    explicit NNIterSort(const Matrix& _C): c(_C.rows()), d(_C.cols()), counter(0),
        C(_C), x_beg(nullptr) {norms.resize(c);}

    void set_point(const MatrixElemType*);
    dipair get_neighbor();
};

#endif  // _NNITER_H
