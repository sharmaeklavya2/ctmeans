#include <ostream>
#include <cstdio>

#include "matrix.h"

void Matrix::_init(int _m, int _n, bool zero) {
    m = _m; n = _n, mn = _m * _n;
    if(mn) {
        if(zero) {
            a = (MatrixElemType*)(std::calloc(mn, sizeof(MatrixElemType)));
        }
        else {
            a = (MatrixElemType*)(std::malloc(mn * sizeof(MatrixElemType)));
        }
    }
    else {a = nullptr;}
}

void Matrix::_init(const Matrix& X) {
    m = X.m; n = X.n, mn = X.mn;
    if(mn) {
        a = (MatrixElemType*)(std::malloc(mn * sizeof(MatrixElemType)));
        std::memcpy(a, X.a, mn * sizeof(MatrixElemType));
    }
    else {a = nullptr;}
}

std::ostream& operator<<(std::ostream& os, const Matrix& X) {
    const char indent[] = "\t";
    int m = X.rows(), n = X.cols();
    for(int i=0; i<m; ++i) {
        os << X(i, 0);
        for(int j=1; j<n; ++j) {
            os << indent << X(i, j);
        }
        os << '\n';
    }
    return os;
}

void output(FILE* fp, const Matrix& X) {
    const char indent[] = "\t";
    int m = X.rows(), n = X.cols();
    for(int i=0; i<m; ++i) {
        std::fprintf(fp, "%lf", X(i, 0));
        for(int j=1; j<n; ++j) {
            std::fprintf(fp, "%s%lf", indent, X(i, j));
        }
        std::fprintf(fp, "\n");
    }
}

void savetxt(const char* fname, const Matrix& X) {
    FILE* fp = fopen(fname, "w");
    if(fp == NULL) {
        fprintf(stderr, "Couldn't open %s\n", fname);
        perror("savetxt");
    }
    else {
        output(fp, X);
        fclose(fp);
    }
}

void input(FILE* fp, Matrix& X) {
    MatrixElemType* data = X.data();
    int mn = X.size();
    for(int i=0; i<mn; ++i) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        fscanf(fp, "%lf", data+i);
#pragma GCC diagnostic pop
    }
}

void loadtxt(const char* fname, Matrix& X) {
    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        fprintf(stderr, "Couldn't open %s\n", fname);
        perror("loadtxt");
    }
    else {
        input(fp, X);
        fclose(fp);
    }
}
