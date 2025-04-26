#pragma once
#include <cstddef>
#include <functional>
#include <algorithm>

template <typename E, typename Compare = std::less<E>>
void heapify(E* arr, std::size_t n, std::size_t i, const Compare& less = Compare{}) {
    std::size_t largest = i;
    std::size_t left = 2 * i + 1;
    std::size_t right = 2 * i + 2;

    if (left < n&& less(arr[largest], arr[left])) {
        largest = left;
    }
    if (right < n&& less(arr[largest], arr[right])) {
        largest = right;
    }
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest, less);
    }
}

template <typename E, typename Compare = std::less<E>>
void heapSort(E* arr, std::size_t size, const Compare& less = Compare{}) {
    for (std::size_t i = size / 2; i > 0; --i) {
        heapify(arr, size, i - 1, less);
    }

    for (std::size_t i = size - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0, less);
    }
}
