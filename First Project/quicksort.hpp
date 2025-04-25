#pragma once
#include <algorithm>
#include <functional>
#include <random>

// G³ówna funkcja quickSort
template <typename E, typename Compare>
void quickSort(E* arr, int size, const Compare& less) {
    if (size <= 1) return;
    quickSortStep(arr, 0, size - 1, less);
}

// Funkcja pomocnicza quickSortStep z losowym pivotem i iteracyjn¹ optymalizacj¹
template <typename E, typename Compare>
void quickSortStep(E* arr, int left, int right, const Compare& less) {
    // Generator losowy do wyboru pivota
    std::mt19937 generator(std::random_device{}());

    while (left < right) {
        // Losowy wybór pivota
        std::uniform_int_distribution<int> dist(left, right);
        int pivotIndex = dist(generator);
        std::swap(arr[pivotIndex], arr[right]);

        E pivot = arr[right];
        int l = left;
        int r = right - 1;

        // Podzia³ tablicy na elementy mniejsze i wiêksze od pivota
        while (l <= r) {
            while (l <= r && !less(pivot, arr[l])) l++;
            while (r >= l && less(pivot, arr[r])) r--;
            if (l < r) std::swap(arr[l], arr[r]);
        }

        std::swap(arr[l], arr[right]);

        // Optymalizacja: mniejszy podzia³ rekurencyjny, wiêkszy iteracyjny
        if (l - left < right - l) {
            quickSortStep(arr, left, l - 1, less);
            left = l + 1;
        }
        else {
            quickSortStep(arr, l + 1, right, less);
            right = l - 1;
        }
    }
}

template <typename E>
void quickSort(E* arr, int size) {
    quickSort(arr, size, std::less<E>());
}
