#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <cstring>
#include <string>
#include "MergeSort.hpp"
#include "QuickSort.hpp"
#include "IntroSort.hpp"

int* generateRandomArray(int arraySize, int seed) {
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> dis(1, 2147483647);
    int* arr = new (std::nothrow) int[arraySize];
    if (!arr) throw std::bad_alloc();
    for (int i = 0; i < arraySize; ++i)
        arr[i] = dis(generator);
    return arr;
}

int* generatePartiallySortedArray(int arraySize, double percentage, int seed) {
    std::mt19937 generator(seed);
    int* arr = new (std::nothrow) int[arraySize];
    if (!arr) throw std::bad_alloc();

    int numSorted = static_cast<int>(arraySize * percentage);
    for (int i = 0; i < numSorted; ++i)
        arr[i] = i + 1;
    for (int i = numSorted; i < arraySize; ++i)
        arr[i] = numSorted + 1 + (generator() % (arraySize - numSorted));

    return arr;
}

int* generateReverseSortedArray(int arraySize) {
    int* arr = new (std::nothrow) int[arraySize];
    if (!arr) throw std::bad_alloc();
    for (int i = 0; i < arraySize; ++i)
        arr[i] = arraySize - i;
    return arr;
}

int* copyArray(const int* original, int arraySize) {
    int* copy = new (std::nothrow) int[arraySize];
    if (!copy) throw std::bad_alloc();
    std::memcpy(copy, original, sizeof(int) * arraySize);
    return copy;
}

template <typename SortFunc>
double runSortExperiment(SortFunc sortFunc, int* baseArrays[], int arraySize, int repeatCount = 10) {
    double totalTime = 0.0;

    for (int i = 0; i < repeatCount; ++i) {
        int* arr = nullptr;
        try {
            arr = copyArray(baseArrays[i], arraySize);

            auto start = std::chrono::high_resolution_clock::now();
            sortFunc(arr, arraySize);
            auto end = std::chrono::high_resolution_clock::now();

            totalTime += std::chrono::duration<double, std::milli>(end - start).count();
        }
        catch (...) {
            delete[] arr;
            throw;
        }
        delete[] arr;
    }

    return totalTime / repeatCount;
}

template <typename SortFunc>
void runExperimentForAllCases(const std::string& sortName, SortFunc sortFunc) {
    constexpr int arraySizes[] = { 100, 500, 1000, 5000, 10000, 50000, 100000, 250000, 500000, 1000000 };
    constexpr double percentages[] = { 0.25, 0.5, 0.75, 0.95, 0.99, 0.997 };
    constexpr int numSizes = sizeof(arraySizes) / sizeof(arraySizes[0]);
    constexpr int numPercents = sizeof(percentages) / sizeof(percentages[0]);
    constexpr int repeatCount = 10;

    std::cout << "===== " << sortName << " =====\n";

    for (int s = 0; s < numSizes; ++s) {
        int arraySize = arraySizes[s];
        std::cout << "===== Rozmiar tablicy: " << arraySize << " =====\n";

        int* baseArrays[repeatCount];
        for (int i = 0; i < repeatCount; ++i)
            baseArrays[i] = generateRandomArray(arraySize, i);

        double avgRandom = runSortExperiment(sortFunc, baseArrays, arraySize);
        std::cout << "Losowe:           " << std::fixed << std::setprecision(2) << avgRandom << " ms\n";

        for (int p = 0; p < numPercents; ++p) {
            double percent = percentages[p];

            for (int i = 0; i < repeatCount; ++i) {
                delete[] baseArrays[i];
                baseArrays[i] = generatePartiallySortedArray(arraySize, percent, i);
            }

            double avg = runSortExperiment(sortFunc, baseArrays, arraySize);
            std::cout << "Sort " << std::setw(4) << int(percent * 100) << "%:        " << std::fixed << std::setprecision(2) << avg << " ms\n";
        }

        for (int i = 0; i < repeatCount; ++i) {
            delete[] baseArrays[i];
            baseArrays[i] = generateReverseSortedArray(arraySize);
        }

        double avgReverse = runSortExperiment(sortFunc, baseArrays, arraySize);
        std::cout << "Odwrotne:         " << std::fixed << std::setprecision(2) << avgReverse << " ms\n";

        for (int i = 0; i < repeatCount; ++i)
            delete[] baseArrays[i];

        std::cout << "\n";
    }
}

void mergeSortWrapper(int* arr, int size) {
    mergeSort<int>(arr, size, [](int a, int b) { return a < b; });
}

void quickSortWrapper(int* arr, int size) {
    quickSort<int>(arr, size, [](int a, int b) { return a < b; });
}

void introSortWrapper(int* arr, int size) {
    introSort<int>(arr, size, [](int a, int b) { return a < b; });
}

int main() {
    try {
        runExperimentForAllCases("Merge Sort", mergeSortWrapper);
        runExperimentForAllCases("Quick Sort", quickSortWrapper);
        runExperimentForAllCases("Intro Sort", introSortWrapper);
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "Błąd alokacji pamięci: " << e.what() << '\n';
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Wystąpił wyjątek: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Wystąpił nieznany błąd.\n";
        return 1;
    }

    return 0;
}
