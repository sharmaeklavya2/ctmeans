#include <cstdio>
#include "nniter_kd.h"

double point_point_norm(int d, const double* p1, const double* p2) {
    double res = 0;
    for(int j=0; j<d; ++j) {
        double p = p1[j] - p2[j];
        res += p*p;
    }
    return res;
}

double point_range_norm(int d, const double* p, const double* rmin, const double* rmax) {
    double res = 0;
    for(int j=0; j<d; ++j) {
        double p1 = rmin[j] - p[j];
        if(p1 > 0) {
            res += p1*p1;
        }
        else {
            double p3 = p[j] - rmax[j];
            if(p3 > 0) {
                res += p3*p3;
            }
        }
    }
    return res;
}

KDHeapElem NNIterKD::get_neighbor_node() {
    if(pq.empty()) {
        std::fprintf(stderr, "NNIterKD: No neighbors left\n");
    }
    KDHeapElem top(pq.top());
    pq.pop();
    if(top.type == 'k') {
        pq.emplace(top.point, C, x_beg);
        if(top.kdnode->left != nullptr) {
            pq.emplace(top.kdnode->left, C, x_beg);
        }
        if(top.kdnode->right != nullptr) {
            pq.emplace(top.kdnode->right, C, x_beg);
        }
    }
    else if(top.type == 'p') {
        counter++;
    }
    heap_ops++;
    return top;
}

std::pair<double, int> NNIterKD::get_neighbor() {
    if(pq.empty()) {
        std::fprintf(stderr, "NNIterKD: No neighbors left\n");
    }
    KDHeapElem heap_elem;
    do {
        heap_elem = get_neighbor_node();
    } while(heap_elem.type != 'p');
    return std::pair<double, int>(heap_elem.prio, heap_elem.point);
}
