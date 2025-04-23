#pragma once
#include <cstddef>
#include <functional>
#include <algorithm> 
#include <stdexcept> 


template <typename E, typename Compare>
void merge(E* arr, E* buffer, std::size_t left, std::size_t mid, std::size_t right, const Compare& less) {
  
    std::copy(arr + left, arr + right + 1, buffer + left);
    
    std::size_t i = left;       
    std::size_t j = mid + 1;    
    std::size_t k = left;       

    while (i <= mid && j <= right) {
        if (less(buffer[i], buffer[j])) {
            arr[k++] = buffer[i++];
        }
        else {
            arr[k++] = buffer[j++];
        }
    }

    while (i <= mid) {
        arr[k++] = buffer[i++];
    }

   
}

template <typename E, typename Compare>
void mergeSort_recursive(E* arr, E* buffer, std::size_t left, std::size_t right, const Compare& less) {
    if (left >= right) return;

    std::size_t mid = left + (right - left) / 2;
    mergeSort_recursive(arr, buffer, left, mid, less);
    mergeSort_recursive(arr, buffer, mid + 1, right, less);
    merge(arr, buffer, left, mid, right, less);
}

template <typename E, typename Compare = std::less<E>> 
void mergeSort(E* arr, std::size_t size, const Compare& less = Compare{}) {
    if (size < 2) return; 

    E* buffer = new (std::nothrow) E[size];
    if (!buffer) {
        throw std::bad_alloc(); 
    }

    try {
        mergeSort_recursive(arr, buffer, 0, size - 1, less);
    }
    catch (...) {
        delete[] buffer; 
        throw;           
    }

    delete[] buffer; 
}