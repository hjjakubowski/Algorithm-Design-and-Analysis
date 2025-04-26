#pragma once
#include <algorithm>
#include <functional>
#include <random>
#include <cstddef>

template <typename E, typename Compare>
void quickSortStep(E* arr, int left, int right, const Compare& less, std::size_t* depthLimit = nullptr) {
    if (left >= right) return;

    
    if (depthLimit) {
        if (*depthLimit == 0) return; 
        --(*depthLimit);
    }

    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> dist(left, right);
    int pivotIndex = dist(generator);
    std::swap(arr[pivotIndex], arr[right]);

    E pivot = arr[right];
    int l = left;
    int r = right - 1;

    while (l <= r) {
        while (l <= r && !less(pivot, arr[l])) l++;
        while (r >= l && less(pivot, arr[r])) r--;
        if (l < r) std::swap(arr[l], arr[r]);
    }

    std::swap(arr[l], arr[right]);

    if (l > 0) quickSortStep(arr, left, l - 1, less, depthLimit);
    quickSortStep(arr, l + 1, right, less, depthLimit);
}

template <typename E, typename Compare = std::less<E>>
void quickSort(E* arr, int size, const Compare& less = Compare{}) {
    if (size <= 1) return;
    quickSortStep(arr, 0, size - 1, less);
}
