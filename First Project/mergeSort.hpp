 #pragma once
#include "Arraylist.hpp" 
#include <functional>

template <typename E, typename C>
void merge(Arraylist<E>& S1, Arraylist<E>& S2, Arraylist<E>& S, const C& less) {
    std::size_t i = 0, j = 0;
    Arraylist<E> temp;

    while (i < S1.getSize() && j < S2.getSize()) {
        if (less(S1[i], S2[j])) {
            temp.addBack(S1[i++]);
        }
        else {
            temp.addBack(S2[j++]);
        }
    }

    while (i < S1.getSize()) temp.addBack(S1[i++]);
    while (j < S2.getSize()) temp.addBack(S2[j++]);

    S = temp; 
}

template <typename E, typename C>
void mergeSort(Arraylist<E>& S, const C& less) {
    std::size_t n = S.getSize();
    if (n <= 1) return;

    Arraylist<E> S1, S2;

    for (std::size_t i = 0; i < n / 2; ++i) {
        S1.addBack(S[i]);
    }

    for (std::size_t i = n / 2; i < n; ++i) {
        S2.addBack(S[i]);
    }

    mergeSort(S1, less);
    mergeSort(S2, less);
    merge(S1, S2, S, less);
}
