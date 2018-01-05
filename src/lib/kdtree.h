#ifndef _KDTREE_H
#define _KDTREE_H

#include <vector>
#include "matrix.h"

class ColComparator {
    const Matrix& X;
    int col;

public:
    ColComparator(const Matrix& _X, int _col): X(_X), col(_col) {}
    bool operator()(int i, int j) const {
        return X(i, col) < X(j, col);
    }
};

class KDNode {
public:
    int depth, point;
    KDNode *parent, *left, *right;
    std::vector<double> cmin, cmax;

    KDNode(int _depth, int _point, KDNode* _parent):
        depth(_depth), point(_point), parent(_parent), left(nullptr), right(nullptr) {}
    ~KDNode() {
        parent = nullptr;
        delete left;
        delete right;
        left = nullptr;
        right = nullptr;
    }

    void print_nl(FILE* fp) const;
};

KDNode* get_kd_tree(const Matrix& X);

KDNode* get_kd_tree(std::vector<int>::iterator begin, std::vector<int>::iterator end,
    const Matrix& X, int depth, KDNode* parent);

void print_kd_tree(FILE* fp, const KDNode* root, int indent);

#endif  // _KDTREE_H
