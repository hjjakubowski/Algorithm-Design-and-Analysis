#pragma once
#include <cstddef>
#include <functional>
#include <cmath>
#include "QuickSort.hpp"
#include "HeapSort.hpp"
#include "InsertionSort.hpp"

template <typename E, typename Compare = std::less<E>>
void introSort(E* arr, std::size_t size, const Compare& less = Compare{}) {
    if (size < 2) return;

    std::size_t depthLimit = 2 * static_cast<std::size_t>(std::log2(size));
    const std::size_t sizeThreshold = 16;

    if (size <= sizeThreshold) {
        insertionSort(arr, 0, size - 1, less);
        return;
    }

    quickSortStep(arr, 0, static_cast<int>(size) - 1, less, &depthLimit);

    
    if (depthLimit == 0) {
        heapSort(arr, size, less);
    }
}
