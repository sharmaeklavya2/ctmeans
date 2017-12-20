#include <vector>
#include <algorithm>

#include "kdtree.h"
#include "vecio.h"

KDNode* get_kd_tree(
    std::vector<int>::iterator begin,
    std::vector<int>::iterator end,
    const Matrix& X, int depth, KDNode* parent
    ) {
    auto size = end - begin;
    if(size == 0) {
        return nullptr;
    }
    else {
        auto size1 = size / 2;
        std::vector<int>::iterator mid = begin + size1;
        if(size > 1) {
            std::nth_element(begin, mid, end, ColComparator(X, depth % X.cols()));
        }
        KDNode* root = new KDNode(depth, *mid, parent);
        int d = X.cols();
        root->cmin.reserve(d);
        root->cmin.assign(X.begin_row(root->point), X.end_row(root->point));
        root->cmax = root->cmin;

        KDNode* left = get_kd_tree(begin, mid, X, depth+1, root);
        KDNode* right = get_kd_tree(mid+1, end, X, depth+1, root);
        root->left = left;
        root->right = right;
        if(left != nullptr) {
            for(int j=0; j<d; ++j) {
                root->cmin[j] = std::min(root->cmin[j], left->cmin[j]);
                root->cmax[j] = std::max(root->cmax[j], left->cmax[j]);
            }
        }
        if(right != nullptr) {
            for(int j=0; j<d; ++j) {
                root->cmin[j] = std::min(root->cmin[j], right->cmin[j]);
                root->cmax[j] = std::max(root->cmax[j], right->cmax[j]);
            }
        }
        return root;
    }
}

void KDNode::print_nl(FILE* fp) const {
    fprintf(fp, "KD(%d, min=", point);
    print(fp, cmin.cbegin(), cmin.cend());
    fprintf(fp, ", max=");
    print(fp, cmax.cbegin(), cmax.cend());
    fprintf(fp, ")\n");
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
