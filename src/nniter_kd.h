#ifndef _NNITER_KD_H
#define _NNITER_KD_H

#include <queue>
#include "kdtree.h"
#include "matrix.h"

double point_point_norm(int d, const double* p1, const double* p2);
double point_range_norm(int d, const double* p, const double* rmin, const double* rmax);

class KDHeapElem {
public:
    const KDNode* kdnode;
    int point;
    char type;      // 'k' for kdnode, 'p' for point, 'u' for invalid
    double prio;    // heap priority

    KDHeapElem(): type('u') {}

    KDHeapElem(int _point, const Matrix& X, const double* p2):
        kdnode(nullptr), point(_point), type('p') {
        prio = point_point_norm(X.cols(), X.begin_row(_point), p2);
    }

    KDHeapElem(const KDNode* _kdnode, const Matrix& X, const double* p2):
        kdnode(_kdnode), point(kdnode->point) {
        if(_kdnode->left == nullptr && _kdnode->right == nullptr) {
            type = 'p';
            prio = point_point_norm(X.cols(), p2, X.begin_row(_kdnode->point));
        }
        else {
            type = 'k';
            prio = point_range_norm(X.cols(), p2, kdnode->cmin.data(), kdnode->cmax.data());
        }
    }

    void print(FILE* fp) const {
        fprintf(fp, "KDHeapElem(type=%c, point=%d, prio=%lf)\n", type, point, prio);
    }
};

inline bool operator<(const KDHeapElem& x, const KDHeapElem& y) {
    if(x.prio != y.prio) {
        return x.prio > y.prio;
    }
    else {
        return x.point > y.point;
    }
}

typedef std::priority_queue<KDHeapElem> KDPQ;

class NNIterKD {
protected:
    int heap_ops;
    int counter;
public:
    KDPQ pq;
    Matrix C;
    KDNode* root;
    const double* x_beg;

    int get_heap_ops() const {return heap_ops;}
    int get_counter() const {return counter;}

    ~NNIterKD() {delete root; root = nullptr;}
    explicit NNIterKD(const Matrix& _C): C(_C), x_beg(nullptr) {
        root = get_kd_tree(C);
    }

    void set_point(const double* _x_beg) {
        x_beg = _x_beg;
        pq = KDPQ();
        pq.emplace(root, C, x_beg);
        heap_ops = 0;
        counter = 0;
    }

    KDHeapElem get_neighbor_node();
};

#endif  // _NNITER_KD_H
