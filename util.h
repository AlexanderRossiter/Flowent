//
// Created by Alexander Rossiter on 25/03/2021.
//

#ifndef FLOWENT_UTIL_H
#define FLOWENT_UTIL_H
#include <vector>
namespace util {
    struct point {
            float x;
            float y;
            float z;
    };
    template<typename T>
    std::vector<T> vector_add(std::vector<T> a, std::vector<T> b) {
        if (a.size() != b.size()) throw std::exception();
        int n = a.size();

        std::vector<T> c(n);
        for (int i = 0; i < n; i++) {
            c[i] = a[i] + b[i];
        }
        return c;
    }

    template<typename T>
    std::vector<T> vector_subtr(std::vector<T> a, std::vector<T> b) {
        if (a.size() != b.size()) throw std::exception();
        int n = a.size();

        std::vector<T> c(n);
        for (int i = 0; i < n; i++) {
            c[i] = a[i] - b[i];
        }
        return c;
    }

    template<typename T>
    T vector_dot(std::vector<T> a, std::vector<T> b) {
        if (a.size() != b.size()) throw std::exception();
        int n = a.size();

        T c = 0;
        for (int i = 0; i < n; i++) {
            c += a[i]*b[i];
        }
        return c;
    }

}
#endif //FLOWENT_UTIL_H
