#ifndef _VECIO_H
#define _VECIO_H

#include <ostream>
#include <vector>
#include <cstdio>

template<class IterT>
void print(std::ostream& os, IterT begin, IterT end) {
    os << "[";
    if(begin != end)
    {
        os << (*begin);
        IterT it = begin; it++;
        for(; it != end; ++it) {
            os << ", " << (*it);
        }
    }
    os << "]";
}

template<class IterT>
void print(FILE* fp, IterT begin, IterT end) {
    fprintf(fp, "[");
    if(begin != end)
    {
        fprintf(fp, "%lf", double(*begin));
        IterT it = begin; it++;
        for(; it != end; ++it) {
            fprintf(fp, ", %lf", double(*it));
        }
    }
    fprintf(fp, "]");
}

template<class IterT>
void output(FILE* fp, IterT begin, IterT end) {
    for(IterT it = begin; it != end; ++it) {
        std::fprintf(fp, "%lf\n", *it);
    }
}

template<>
inline void output<std::vector<int>::const_iterator>(
    FILE* fp,
    std::vector<int>::const_iterator begin,
    std::vector<int>::const_iterator end
    ) {
    for(std::vector<int>::const_iterator it = begin; it != end; ++it) {
        std::fprintf(fp, "%d\n", *it);
    }
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    print(os, v.begin(), v.end());
    return os;
}

template<class IterT>
void savetxt(const char* fname, IterT begin, IterT end) {
    FILE* fp = fopen(fname, "w");
    if(fp == NULL) {
        perror("savetxt");
    }
    else {
        output(fp, begin, end);
        fclose(fp);
    }
}

template<class T>
void input(FILE* fp, std::vector<T>& v) {
    double x;
    while(fscanf(fp, "%lf", &x) > 0) {
        v.push_back(x);
    }
}

template<>
inline void input<int>(FILE* fp, std::vector<int>& v) {
    int x;
    while(fscanf(fp, "%d", &x) > 0) {
        v.push_back(x);
    }
}

template<class T>
void loadtxt(const char* fname, std::vector<T>& v) {
    FILE* fp = fopen(fname, "r");
    if(fp == NULL) {
        perror("loadtxt");
    }
    else {
        input(fp, v);
        fclose(fp);
    }
}

#endif  // _VECIO_H
