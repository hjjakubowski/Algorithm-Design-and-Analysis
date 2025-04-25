#pragma once
#include <algorithm> 
#include <functional>

template <typename E, typename Compare>
void quickSort(E* arr, int size, const Compare& less) {
    if (size <= 1) return;
    quickSortStep(arr, 0, size - 1, less);
}

template <typename E, typename Compare>
void quickSortStep(E* arr, int left, int right, const Compare& less) {
    if (left >= right) return;

    
    int mid = left + (right - left) / 2;
    E first = arr[left];
    E middle = arr[mid];
    E last = arr[right];

   
    E pivot = (less(first, middle)
        ? (less(middle, last) ? middle : (less(first, last) ? last : first))
        : (less(first, last) ? first : (less(middle, last) ? last : middle)));

    
    if (pivot == middle) {
        std::swap(arr[mid], arr[right]);
    }
    else if (pivot == first) {
        std::swap(arr[left], arr[right]);
    }

    int l = left;
    int r = right - 1;

    while (l <= r) {
        while (l <= r && !less(pivot, arr[l])) l++;
        while (r >= l && !less(arr[r], pivot)) r--;
        if (l < r) std::swap(arr[l], arr[r]);
    }

    std::swap(arr[l], arr[right]);

    quickSortStep(arr, left, l - 1, less);
    quickSortStep(arr, l + 1, right, less);
}


template <typename E>
void quickSort(E* arr, int size) {
    quickSort(arr, size, std::less<E>());
}
