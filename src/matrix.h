#ifndef _MATRIX_H
#define _MATRIX_H

#include <ostream>
#include <utility>
#include <cstdlib>
#include <cstring>

typedef double MatrixElemType;

class Matrix {
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

    Matrix(): m(0), n(0), mn(0), a(nullptr) {}
    Matrix(int _m, int _n, bool zero=false): m(_m), n(_n), mn(_m*_n) {
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
    Matrix(const Matrix& X): m(X.m), n(X.n), mn(X.mn) {
        if(mn) {
            a = (MatrixElemType*)(std::malloc(mn * sizeof(MatrixElemType)));
            std::memcpy(a, X.a, mn * sizeof(MatrixElemType));
        }
        else {a = nullptr;}
    }
    ~Matrix() {if(mn) {free(a);}}
    void swap(Matrix& X) {
        std::swap(m, X.m);
        std::swap(n, X.n);
        std::swap(mn, X.mn);
        std::swap(a, X.a);
    }
    Matrix& operator=(Matrix X) {
        swap(X);
        return *this;
    }
    Matrix(Matrix&& X): Matrix() {
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
    inline void swap(Matrix& X, Matrix& Y) {
        X.swap(Y);
    }
}

std::ostream& operator<<(std::ostream&, const Matrix&);
void output(FILE*, const Matrix&);
void savetxt(const char*, const Matrix&);
void input(FILE*, Matrix&);
void loadtxt(const char*, Matrix&);

#endif  // _MATRIX_H
