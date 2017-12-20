#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>
#include <cmath>
#include <algorithm>
#include "ctmeans.h"
#include "nniter.h"

void CTMeans::init_centroids_rand() {
    for(int i=0; i<c; ++i) {
        int index = unsigned(rand()) % n;
        std::memcpy(C.begin_row(i), X.begin_row(index), d * sizeof(double));
    }
}

double CTMeans::step(bool update_centroids, bool store_u) {
    using std::vector;
    typedef std::pair<double, int> dipair;

    double obj = 0.0;
    NNIterSort nniter(C);
    double mexp = 1/(1-m);
    vector<double> usum;    // usum[j] = sum(U(i, j)^m for i in range(n))
    if(update_centroids) {
        usum.resize(c, 0);
        C.set_to_zero();
    }
    if(store_u) {
        U.clear();
        U.resize(n);
    }
    vector<double> norms, us;
        // norms[j] = norm of x from jth nearest centroid (norms.size() = t)
        // us[j] = membership value of x_i with c_j
    vector<int> indices;
        // indices[j] = index of jth nearest centroid (indices.size() = t)
    norms.reserve(c);
    for(int i=0; i<n; ++i) {
        us.reserve(c); indices.reserve(c);
        double norms2sum=0, norm12;
            // norms2sum = denominator of all U(i, j)
            // norm12 = norm2 of a point to the nearest cluster
        nniter.set_point(X.begin_row(i), X.end_row(i));

        // get the first t norms and corresponding membership values
        for(int j=0; j<max_t; ++j) {
            dipair norm_index = nniter.get_neighbor();
            double norm = norm_index.first;
            int index = norm_index.second;
            if(index < 0 || index >= c) {
                fprintf(stderr, "step: i=%d, j=%d, index=%d\n", i, j, index);
            }
            if(norm == 0.0 || norm == -0.0) {
                norms.clear();
                indices.clear();
                us.clear();
                norms.push_back(0);
                indices.push_back(index);
                us.push_back(1);
                norms2sum = 1;
                // fprintf(stderr, "saw 0 norm\n");
                break;
            }
            double norm2 = std::pow(norm, mexp);
            if(j == 0) {norm12 = norm2;}
            double den_u = norms2sum + norm2;
            double u_j = norm2 / den_u;
            double u_1 = norm12 / den_u;
            double v_1 = norm12 / (den_u + (c-j)*norm2);
            if((u_1 - v_1 < eps_t) && u_j < eps_t) {
                break;
            }
            norms.push_back(norm);
            indices.push_back(index);
            us.push_back(norm2);
            norms2sum = den_u;
        }
        for(unsigned i2=0; i2 < us.size(); ++i2) {
            us[i2] /= norms2sum;
        }

        // calculate objective
        for(unsigned i2=0; i2<norms.size(); ++i2) {
            obj += norms[i2] * us[i2];
        }

        // update centroids
        if(update_centroids) {
            for(unsigned i2=0; i2<norms.size(); ++i2) {
                int j = indices[i2];
                double uij_m = std::pow(us[i2], m);
                usum[j] += uij_m;
                for(int k=0; k<d; ++k) {
                    C(j, k) += uij_m * X(i, k);
                }
            }
        }

        // store membership values
        norms.clear();
        if(store_u) {
            std::swap(us, U[i].first);
            std::swap(indices, U[i].second);
        }
        else {
            us.clear(); indices.clear();
        }
    }

    if(update_centroids) {
        for(int j=0; j<c; ++j) {
            for(int k=0; k<d; ++k) {
                C(j, k) /= usum[j];
            }
        }
    }

    return obj;
}

double CTMeans::cluster(int reps, int max_epochs, double obj_tol, FILE* fp, int epoch_interval) {
    double minobj = std::numeric_limits<double>::infinity();
    for(int repi=1; repi <= reps; ++repi) {
        init_centroids_rand();
        double obj = std::numeric_limits<double>::infinity();
        int epochi;
        for(epochi=1; epochi <= max_epochs; ++epochi) {
            double old_obj = obj;
            obj = step(true, false);
            if(fp != nullptr && epochi % epoch_interval == 1) {
                fprintf(fp, "ctmeans %d-%d: %lg\n", repi, epochi, obj);
            }
            if((old_obj - obj) < old_obj * obj_tol) {break;}
        }
        if(fp != nullptr) {
            fprintf(fp, "\n");
        }
        if(obj < minobj) {
            minobj = obj;
            std::swap(minC, C);
        }
    }
    C = minC;
    minobj = step(false, true);
    return minobj;
}

void CTMeans::get_flat_U(std::vector<double>& flatu) const {
    flatu.clear();
    int flatu_size = 0;
    for(int i=0; i<n; ++i) {
        flatu_size += U[i].first.size();
    }
    flatu.reserve(flatu_size);
    for(int i=0; i<n; ++i) {
        const std::vector<double>& us = U[i].first;
        for(unsigned i2=0; i2<us.size(); ++i2) {
            flatu.push_back(us[i2]);
        }
    }
    std::sort(flatu.begin(), flatu.end(), std::greater<double>());
}

void CTMeans::get_labels(std::vector<int>& labels) const {
    labels.resize(n);
    for(int i=0; i<n; ++i) {
        labels[i] = U[i].second[0];
    }
}

void CTMeans::get_dense_U(Matrix& U2) const {
    for(int i=0; i<n; ++i) {
        const std::vector<double>& us = U[i].first;
        const std::vector<int>& indices = U[i].second;
        for(unsigned i2=0; i2<us.size(); ++i2) {
            U2(i, indices[i2]) = us[i2];
        }
    }
}
