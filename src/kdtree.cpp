#include <vector>
#include <algorithm>

#include "kdtree.h"

KDNode* get_kd_tree(
    std::vector<int>::iterator begin,
    std::vector<int>::iterator end,
    const Matrix& X, int depth, KDNode* parent
    ) {
    auto size = end - begin;
    if(size == 0) {
        return nullptr;
    }
    else if(size == 1) {
        return new KDNode(depth, *begin, parent);
    }
    else {
        auto size1 = size / 2;
        std::vector<int>::iterator mid = begin + size1;
        std::nth_element(begin, mid, end, ColComparator(X, depth % X.cols()));
        KDNode* root = new KDNode(depth, *mid, parent);
        root->left = get_kd_tree(begin, mid, X, depth+1, root);
        root->right = get_kd_tree(mid+1, end, X, depth+1, root);
        return root;
    }
}

void print_kd_tree(FILE* fp, const KDNode* root, int indent) {
    if(root != nullptr)
    {
        for(int i=0; i<indent; ++i) {
            fprintf(fp, "  ");
        }
        root->print_nl(fp);
        print_kd_tree(fp, root->left, indent+1);
        print_kd_tree(fp, root->right, indent+1);
    }
}
