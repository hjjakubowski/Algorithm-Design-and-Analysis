#pragma once
#include <cstddef>
#include <functional>

template <typename E, typename Compare = std::less<E>>
void insertionSort(E* arr, std::size_t left, std::size_t right, const Compare& less = Compare{}) {
    for (std::size_t i = left + 1; i <= right; ++i) {
        E key = arr[i];
        std::size_t j = i;
        while (j > left && less(key, arr[j - 1])) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = key;
    }
}
