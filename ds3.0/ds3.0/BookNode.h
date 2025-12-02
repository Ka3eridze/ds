#pragma once
#include "Book.h"

template<typename T>
struct BookNode {
    T data;
    int height;
    BookNode<T>* left;
    BookNode<T>* right;

    BookNode(const T& b) : data(b), height(1), left(nullptr), right(nullptr) {}
};