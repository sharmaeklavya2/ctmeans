#include <cstdio>
#include <vector>
#include <string>

#include "../lib/vecio.h"
#include "../lib/matrix.h"
#include "../lib/nniter_kd.h"

using namespace std;

string file2string(const char* fname) {
    FILE* fp = fopen(fname, "rb");
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    string s;
    s.resize(fsize+1);
    s[fsize] = '\0';
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fread(&(s[0]), fsize, 1, fp);
#pragma GCC diagnostic pop
    fclose(fp);
    return s;
}

int main() {
    // read shape of matrix
    int n, d;
    FILE* fp = nullptr;

    fp = fopen("fixtures/matrix.shape.txt", "r");
    if(fp == NULL) {
        perror("main: couldn't read shape");
        return 1;
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
    fscanf(fp, "%d%d", &n, &d);
#pragma GCC diagnostic pop
    fclose(fp);

    // load matrix
    Matrix X(n, d);
    loadtxt("fixtures/matrix.txt", X);

    NNIterKD nniter(X);

    // Save output to a file
    fp = fopen("var/nniter_kd.txt", "w");
    if(fp == NULL) {
        perror("main: couldn't open file for writing");
        return 1;
    }
    print_kd_tree(fp, nniter.root, 0);
    fclose(fp);

    // read file and compare with fixture
    string output = file2string("var/nniter_kd.txt");
    string prev_output = file2string("fixtures/kdtree.txt");
    if(output != prev_output) {
        fprintf(stderr, "test/nniter_kd: KD-tree output mismatch\n");
        fprintf(stderr, "Expected output:\n%s\n", prev_output.c_str());
        fprintf(stderr, "Actual output:\n%s\n", output.c_str());
        return 1;
    }

    KDHeapElem he;

    vector<double> p;
    loadtxt("fixtures/nniter_point.txt", p);
    nniter.set_point(p.data());

    vector<KDHeapElem> hev;
    fp = fopen("fixtures/nn_nodes.txt", "r");
    char charstr[3];
    int point;
    double prio;
    while(fscanf(fp, "%s%d%lf", charstr, &point, &prio) > 0) {
        hev.emplace_back();
        hev.back().kdnode = nullptr;
        hev.back().type = charstr[0];
        hev.back().point = point;
        hev.back().prio = prio;
    }
    fclose(fp);

    unsigned pops=0;
    for(pops=0; !nniter.pq.empty() && pops < hev.size(); ++pops) {
        he = nniter.get_neighbor_node();
        if(!(he == hev[pops])) {
            fprintf(stderr, "test/nniter_kd: KDHeapElem mismatch");
            fprintf(stderr, "Expected output:\n");
            hev[pops].print(stderr);
            fprintf(stderr, "Actual output:\n");
            he.print(stderr);
            return 1;
        }
    }

    if(pops != hev.size() || !nniter.pq.empty()) {
        fprintf(stderr, "test/nniter_kd: queue size mismatch");
        fprintf(stderr, "Expected length: %zd\n", hev.size());
        fprintf(stderr, "Actual length: %zd+\n", pops + nniter.pq.size());
        return 1;
    }
        
    return 0;
}
