//
// Created by Alexander Rossiter on 25/03/2021.
//

#ifndef FLOWENT_UTIL_H
#define FLOWENT_UTIL_H
#include "Block.h"
#include <sstream>
#include <string>
#include <vector>

namespace util {
struct point
{
    double x;
    double y;
    double z;
};
std::vector<std::string>
str_split(const std::string& inputString, const std::string& delimiter);
double
calculate_face_average(vector3d<double>& arr, int& faceId, int& i, int& j, int& k);
bool
at_least_two(bool a, bool b, bool c, bool d);
void
copy_grid_flow_variables(Block& b1, Block& b2, int i, int j, int k, int i2, int j2, int k2);

template<typename T>
std::vector<T>
vector_add(std::vector<T> a, std::vector<T> b)
{
    if (a.size() != b.size())
        throw std::exception();
    int n = a.size();

    std::vector<T> c(n);
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
    return c;
}

template<typename T>
std::vector<T>
vector_subtr(std::vector<T> a, std::vector<T> b)
{
    if (a.size() != b.size())
        throw std::exception();
    int n = a.size();

    std::vector<T> c(n);
    for (int i = 0; i < n; i++) {
        c[i] = a[i] - b[i];
    }
    return c;
}

template<typename T>
T
vector_dot(std::vector<T> a, std::vector<T> b)
{
    if (a.size() != b.size())
        throw std::exception();
    int n = a.size();

    T c = 0;
    for (int i = 0; i < n; i++) {
        c += a[i] * b[i];
    }
    return c;
}

template<typename T>
std::string
vector_to_string(const std::vector<T>& vector)
{
    std::ostringstream ss;
    ss << "[";

    for (int i = 0; i < vector.size(); i++) {
        ss << vector.at(i);
        if (i != vector.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";

    return ss.str();
}

}
#endif // FLOWENT_UTIL_H
