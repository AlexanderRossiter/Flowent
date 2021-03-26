//
// Created by Alexander Rossiter on 25/03/2021.
//

#ifndef FLOWENT_TYPEDEFINITIONS_H
#define FLOWENT_TYPEDEFINITIONS_H
template<typename T>
using vector3d = boost::multi_array<T, 3>;
template<typename T, int N>
using vectornd = boost::multi_array<T, N> ;
#endif //FLOWENT_TYPEDEFINITIONS_H