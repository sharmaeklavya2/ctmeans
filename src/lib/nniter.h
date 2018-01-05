#ifndef _NNITER_H
#define _NNITER_H

#include <utility>
#include <algorithm>
#include <vector>
#include "matrix.h"

class NNIterSort {
protected:
    int c, d;
    int counter;
public:
    Matrix C;
    const MatrixElemType *x_beg;

    typedef std::pair<double, int> dipair;
    std::vector<dipair> norms;

    int get_counter() const {return counter;}

    explicit NNIterSort(): c(0), d(0), counter(0), x_beg(nullptr) {}

    void init(const Matrix& _C) {
        c = _C.rows(); d = _C.cols(); C.init(_C);
        norms.resize(c);
    }
    explicit NNIterSort(const Matrix& _C): c(_C.rows()), d(_C.cols()), counter(0),
        C(_C), x_beg(nullptr) {norms.resize(c);}

    void set_point(const MatrixElemType*);
    dipair get_neighbor();
};

#endif  // _NNITER_H
