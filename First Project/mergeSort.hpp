#pragma once
#include <cstddef>   
#include <functional>
#include <algorithm> 

template <typename E, typename Compare>
void merge(E* arr, std::size_t left, std::size_t mid, std::size_t right, const Compare& less) {
    std::size_t n1 = mid - left + 1;
    std::size_t n2 = right - mid;

    E* L = new E[n1];
    E* R = new E[n2];

    for (std::size_t i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (std::size_t j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    std::size_t i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (less(L[i], R[j])) {
            arr[k++] = L[i++];
        }
        else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

template <typename E, typename Compare>
void mergeSort(E* arr, std::size_t left, std::size_t right, const Compare& less) {
    if (left >= right) return;

    std::size_t mid = left + (right - left) / 2;
    mergeSort(arr, left, mid, less);
    mergeSort(arr, mid + 1, right, less);
    merge(arr, left, mid, right, less);
}
