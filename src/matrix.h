#ifndef _MATRIX_H
#define _MATRIX_H

#include <utility>
#include <cstdlib>
#include <cstring>

typedef double MatrixElemType;

class DenseMatrix {
private:
    int m, n, mn;
    MatrixElemType* a;

protected:

public:
    int rows() const {return m;}
    int cols() const {return n;}
    int size() const {return mn;}
    const MatrixElemType* data() const {return a;}
    const MatrixElemType* begin() const {return a;}
    const MatrixElemType* end() const {return a + mn;}
    const MatrixElemType* begin_row(int i) const {return a + i*n;}
    const MatrixElemType* end_row(int i) const {return a + (i+1)*n;}
    MatrixElemType* data() {return a;}
    MatrixElemType* begin() {return a;}
    MatrixElemType* end() {return a + mn;}
    MatrixElemType* begin_row(int i) {return a + i*n;}
    MatrixElemType* end_row(int i) {return a + (i+1)*n;}

    DenseMatrix(): m(0), n(0), mn(0), a(nullptr) {}
    DenseMatrix(int _m, int _n, bool zero=false): m(_m), n(_n), mn(_m*_n) {
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
    DenseMatrix(const DenseMatrix& X): m(X.m), n(X.n), mn(X.mn) {
        if(mn) {
            a = (MatrixElemType*)(std::malloc(mn * sizeof(MatrixElemType)));
            std::memcpy(a, X.a, mn * sizeof(MatrixElemType));
        }
        else {a = nullptr;}
    }
    ~DenseMatrix() {if(mn) {free(a);}}
    void swap(DenseMatrix& X) {
        std::swap(m, X.m);
        std::swap(n, X.n);
        std::swap(mn, X.mn);
        std::swap(a, X.a);
    }
    DenseMatrix& operator=(DenseMatrix X) {
        swap(X);
        return *this;
    }
    DenseMatrix(DenseMatrix&& X): DenseMatrix() {
        swap(X);
    }

    MatrixElemType operator()(int i, int j) const {
        return a[i*n + j];
    }
    MatrixElemType& operator()(int i, int j) {
        return a[i*n + j];
    }

    void set_to_zero() {
        if(mn) {
            std::memset(a, 0, mn * sizeof(MatrixElemType));
        }
    }
};

namespace std {
    template <>
    inline void swap(DenseMatrix& X, DenseMatrix& Y) {
        X.swap(Y);
    }
}

std::ostream& operator<<(std::ostream&, const DenseMatrix&);
void output(FILE*, const DenseMatrix&);
void savetxt(const char*, const DenseMatrix&);
void input(FILE*, DenseMatrix&);
void loadtxt(const char*, DenseMatrix&);

#endif  // _MATRIX_H
