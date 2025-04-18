#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <cstring> // dla memcpy
#include <string>
#include "mergeSort.hpp"

// =================== Generowanie tablic ========================

int* generateRandomArray(int arraySize, int seed) {
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> dis(1, 2147483647);
    int* arr = new (std::nothrow) int[arraySize];
    if (!arr) throw std::bad_alloc();
    for (int i = 0; i < arraySize; ++i)
        arr[i] = dis(generator);
    return arr;
}

int* copyArray(const int* original, int arraySize) {
    int* copy = new (std::nothrow) int[arraySize];
    if (!copy) throw std::bad_alloc();
    std::memcpy(copy, original, sizeof(int) * arraySize);
    return copy;
}

void partialSort(int* arr, int arraySize, double percentage) {
    int n = static_cast<int>(arraySize * percentage);
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

void fullReverseSort(int* arr, int arraySize) {
    for (int i = 1; i < arraySize; ++i) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
    for (int i = 0; i < arraySize / 2; ++i) {
        int temp = arr[i];
        arr[i] = arr[arraySize - 1 - i];
        arr[arraySize - 1 - i] = temp;
    }
}

// ======================= Pomiar i sortowanie ========================

template <typename ModifierFunc>
double runExperimentAverage(int* baseArrays[], int arraySize, ModifierFunc&& modifyBeforeSort) {
    double totalTime = 0.0;

    for (int i = 0; i < 10; ++i) {
        int* arr = nullptr;
        try {
            arr = copyArray(baseArrays[i], arraySize);
            modifyBeforeSort(arr);

            auto start = std::chrono::high_resolution_clock::now();

            mergeSort<int>(arr, 0, arraySize - 1, [](int a, int b) { return a < b; });

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            totalTime += duration.count();
        }
        catch (...) {
            delete[] arr;
            throw;
        }
        delete[] arr;
    }

    return totalTime / 10.0;
}

// ============================ Main ==============================

int main() {
    try {
        constexpr int arraySizes[] = { 100, 500, 1000, 5000, 10000, 50000, 100000, 250000, 500000, 1000000 };
        constexpr double percentages[] = { 0.25, 0.5, 0.75, 0.95, 0.99, 0.997 };
        constexpr int numSizes = sizeof(arraySizes) / sizeof(arraySizes[0]);
        constexpr int numPercents = sizeof(percentages) / sizeof(percentages[0]);
        constexpr int repeatCount = 10;

        for (int s = 0; s < numSizes; ++s) {
            int arraySize = arraySizes[s];
            std::cout << "===== Rozmiar tablicy: " << arraySize << " =====\n";

            // Generuj bazowe tablice
            int* baseArrays[repeatCount];
            for (int i = 0; i < repeatCount; ++i)
                baseArrays[i] = generateRandomArray(arraySize, i);

            // Losowe (bez modyfikacji)
            double avgRandom = runExperimentAverage(baseArrays, arraySize, [](int*) {});
            std::cout << "Losowe:           " << std::fixed << std::setprecision(2) << avgRandom << " ms\n";

            // Częściowo posortowane
            for (int p = 0; p < numPercents; ++p) {
                double percent = percentages[p];
                double avg = runExperimentAverage(baseArrays, arraySize, [percent, arraySize](int* arr) {
                    partialSort(arr, arraySize, percent);
                    });
                std::cout << "Sort " << std::setw(4) << int(percent * 100) << "%:        "
                    << std::fixed << std::setprecision(2) << avg << " ms\n";
            }

            // Odwrotne
            double avgReverse = runExperimentAverage(baseArrays, arraySize, [arraySize](int* arr) {
                fullReverseSort(arr, arraySize);
                });
            std::cout << "Odwrotne:         " << std::fixed << std::setprecision(2) << avgReverse << " ms\n\n";

            // Zwolnij bazowe tablice
            for (int i = 0; i < repeatCount; ++i)
                delete[] baseArrays[i];
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wystąpił błąd: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Wystąpił nieznany błąd." << std::endl;
    }

    return 0;
}
