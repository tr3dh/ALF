#pragma once

#include <vector>
#include <algorithm>

template <typename T>
bool vectorContains(const std::vector<T>& vec, const T& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}