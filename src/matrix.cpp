#include <ostream>
#include <cstdio>

#include "matrix.h"

std::ostream& operator<<(std::ostream& os, const DenseMatrix& X) {
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

void output(FILE* fp, const DenseMatrix& X) {
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

void savetxt(const char* fname, const DenseMatrix& X) {
    FILE* fp = fopen(fname, "w");
    if(fp == NULL) {
        perror("savetxt");
    }
    else {
        output(fp, X);
        fclose(fp);
    }
}

void input(FILE* fp, DenseMatrix& X) {
    MatrixElemType* data = X.data();
    int mn = X.size();
    for(int i=0; i<mn; ++i) {
        fscanf(fp, "%lf", data+i);
    }
}

void loadtxt(const char* fname, DenseMatrix& X) {
    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        perror("loadtxt");
    }
    else {
        input(fp, X);
        fclose(fp);
    }
}
