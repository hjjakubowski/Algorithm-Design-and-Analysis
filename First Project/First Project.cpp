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

    
    for (int i = 0; i < numSorted; ++i) {
        arr[i] = i + 1;
    }

   
    for (int i = numSorted; i < arraySize; ++i) {
        arr[i] = numSorted + 1 + (generator() % (arraySize - numSorted));
    }

    return arr;
}

int* generateReverseSortedArray(int arraySize) {
    int* arr = new (std::nothrow) int[arraySize];
    if (!arr) throw std::bad_alloc();

    for (int i = 0; i < arraySize; ++i) {
        arr[i] = arraySize - i;
    }

    return arr;
}

int* copyArray(const int* original, int arraySize) {
    int* copy = new (std::nothrow) int[arraySize];
    if (!copy) throw std::bad_alloc();
    std::memcpy(copy, original, sizeof(int) * arraySize);
    return copy;
}

void printArray(const int* arr, int arraySize, const std::string& description) {
    std::cout << description << ": ";
    for (int i = 0; i < std::min(100, arraySize); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
}


template <typename ModifierFunc>
double runMergeSortExperiment(int* baseArrays[], int arraySize, ModifierFunc&& modifyBeforeSort) {
    double totalTime = 0.0;

    for (int i = 0; i < 10; ++i) {
        int* arr = nullptr;
        try {
            arr = copyArray(baseArrays[i], arraySize);
            modifyBeforeSort(arr);

            auto start = std::chrono::high_resolution_clock::now();

            mergeSort<int>(arr, arraySize, [](int a, int b) { return a < b; });
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

template <typename ModifierFunc>
double runQuickSortExperiment(int* baseArrays[], int arraySize, ModifierFunc&& modifyBeforeSort) {
    double totalTime = 0.0;

    for (int i = 0; i < 10; ++i) {
        int* arr = nullptr;
        try {
            arr = copyArray(baseArrays[i], arraySize);
            modifyBeforeSort(arr);

            auto start = std::chrono::high_resolution_clock::now();

            quickSort<int>(arr, arraySize, [](int a, int b) { return a < b; }); 
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

template <typename ModifierFunc>
double runIntroSortExperiment(int* baseArrays[], int arraySize, ModifierFunc&& modifyBeforeSort) {
    double totalTime = 0.0;

    for (int i = 0; i < 10; ++i) {
        int* arr = nullptr;
        try {
            arr = copyArray(baseArrays[i], arraySize);
            modifyBeforeSort(arr);

            auto start = std::chrono::high_resolution_clock::now();

            introSort<int>(arr, arraySize, [](int a, int b) { return a < b; });
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




int main() {
    try {
        constexpr int arraySizes[] = { 100, 500, 1000, 5000, 10000, 50000, 100000, 250000, 500000, 1000000 };
        constexpr double percentages[] = { 0.25, 0.5, 0.75, 0.95, 0.99, 0.997 };
        constexpr int numSizes = sizeof(arraySizes) / sizeof(arraySizes[0]);
        constexpr int numPercents = sizeof(percentages) / sizeof(percentages[0]);
        constexpr int repeatCount = 10;
	    
        std::cout << "===== Merge Sort =====\n";
        for (int s = 0; s < numSizes; ++s) {
            int arraySize = arraySizes[s];
            std::cout << "===== Rozmiar tablicy: " << arraySize << " =====\n";

            
            int* baseArrays[repeatCount];
            for (int i = 0; i < repeatCount; ++i)
                baseArrays[i] = generateRandomArray(arraySize, i);

			// Wyświetlanie pierwszych 100 elementów tablicy przed sortowaniem
            //printArray(baseArrays[0], arraySize, "Przed posorotwaniem: ");
            

            double avgRandom = runMergeSortExperiment(baseArrays, arraySize, [](int*) {});
            std::cout << "Losowe:           " << std::fixed << std::setprecision(2) << avgRandom << " ms\n";
            //std::cout << std::fixed << std::setprecision(2) << avgRandom << "\n";
			
           /*
            //Wyświetlanie pierwszych 100 elementów posortowanych tablic
            int* sortedArray = copyArray(baseArrays[0], arraySize);
            mergeSort<int>(sortedArray, arraySize, [](int a, int b) { return a < b; });
            printArray(sortedArray, arraySize, "Po sortowaniu: ");
            delete[] sortedArray;
           */
            

            for (int p = 0; p < numPercents; ++p) {
                double percent = percentages[p];

                
                for (int i = 0; i < repeatCount; ++i) {
                    delete[] baseArrays[i];
                    baseArrays[i] = generatePartiallySortedArray(arraySize, percent, i);
                }
                // Wyświetlanie pierwszych 100 elementów tablicy przed sortowaniem
                //printArray(baseArrays[0], arraySize, "Losowe przed sortowaniem");

                double avg = runMergeSortExperiment(baseArrays, arraySize, [](int*) {});
                std::cout << "Sort " << std::setw(4) << int(percent * 100) << "%:        "  << std::fixed << std::setprecision(2) << avg << " ms\n";
                //std::cout << std::fixed << std::setprecision(2) << avg << "\n";

                
                /*
                //Wyświetlanie pierwszych 100 elementów posortowanych tablic
                sortedArray = copyArray(baseArrays[0], arraySize);
                mergeSort<int>(sortedArray, arraySize, [](int a, int b) { return a < b; });
				printArray(sortedArray, arraySize, "Po sortowaniu (Sort): ");
                delete[] sortedArray;
                */
                
            }

            
            for (int i = 0; i < repeatCount; ++i) {
                delete[] baseArrays[i];
                baseArrays[i] = generateReverseSortedArray(arraySize);
            }

            // Wyświetlanie pierwszych 100 elementów tablicy przed sortowaniem
            //printArray(baseArrays[0], arraySize, "Losowe przed sortowaniem");

            double avgReverse = runMergeSortExperiment(baseArrays, arraySize, [](int*) {});
            std::cout << "Odwrotne:         " << std::fixed << std::setprecision(2) << avgReverse << " ms\n";
            //std::cout << std::fixed << std::setprecision(2) << avgReverse << "\n";

            
            /*
            //Wyświetlanie pierwszych 10 elementów posortowanych tablic
            sortedArray = copyArray(baseArrays[0], arraySize);
            mergeSort<int>(sortedArray, arraySize, [](int a, int b) { return a < b; });
			printArray(sortedArray, arraySize, "Po sortowaniu (Odwrotne): ");
            std::cout << "\n";
            delete[] sortedArray;
            */
            
            
            for (int i = 0; i < repeatCount; ++i) delete[] baseArrays[i];
		}std::cout << "\n";

        std::cout << "===== Quick Sort =====\n";
        // Testy dla quickSort
        for (int s = 0; s < numSizes; ++s) {
            int arraySize = arraySizes[s];
            std::cout << "===== Rozmiar tablicy: " << arraySize << " =====\n";

            int* baseArrays[repeatCount];
            for (int i = 0; i < repeatCount; ++i)
                baseArrays[i] = generateRandomArray(arraySize, i);

            double avgRandom = runQuickSortExperiment(baseArrays, arraySize, [](int*) {});
            std::cout << "Losowe:           " << std::fixed << std::setprecision(2) << avgRandom << " ms\n";
            //std::cout << std::fixed << std::setprecision(2) << avgRandom << "\n";

            for (int p = 0; p < numPercents; ++p) {
                double percent = percentages[p];

                for (int i = 0; i < repeatCount; ++i) {
                    delete[] baseArrays[i];
                    baseArrays[i] = generatePartiallySortedArray(arraySize, percent, i);
                }

                double avg = runQuickSortExperiment(baseArrays, arraySize, [](int*) {});
                std::cout << "Sort " << std::setw(4) << int(percent * 100) << "%:        "<< std::fixed << std::setprecision(2) << avg << " ms\n";
                //std::cout << std::fixed << std::setprecision(2) << avg << "\n";
            }

            for (int i = 0; i < repeatCount; ++i) {
                delete[] baseArrays[i];
                baseArrays[i] = generateReverseSortedArray(arraySize);
            }
            double avgReverse = runQuickSortExperiment(baseArrays, arraySize, [](int*) {});
            std::cout << "Odwrotne:         " << std::fixed << std::setprecision(2) << avgReverse << " ms\n";
            //std::cout << std::fixed << std::setprecision(2) << avgReverse << "\n";

            for (int i = 0; i < repeatCount; ++i) delete[] baseArrays[i];
		}std::cout << "\n";

        std::cout << "===== Intro Sort =====\n";
		//Testy dla introSort
        for (int s = 0; s < numSizes; ++s) {
            int arraySize = arraySizes[s];
            std::cout << "===== Rozmiar tablicy: " << arraySize << " =====\n";

            int* baseArrays[repeatCount];
            for (int i = 0; i < repeatCount; ++i)
                baseArrays[i] = generateRandomArray(arraySize, i);

            double avgRandom = runIntroSortExperiment(baseArrays, arraySize, [](int*) {});
            std::cout << "Losowe:           " << std::fixed << std::setprecision(2) << avgRandom << " ms\n";
            //std::cout << std::fixed << std::setprecision(2) << avgRandom << "\n";

            for (int p = 0; p < numPercents; ++p) {
                double percent = percentages[p];

                for (int i = 0; i < repeatCount; ++i) {
                    delete[] baseArrays[i];
                    baseArrays[i] = generatePartiallySortedArray(arraySize, percent, i);
                }

                double avg = runIntroSortExperiment(baseArrays, arraySize, [](int*) {});
                std::cout << "Sort " << std::setw(4) << int(percent * 100) << "%:        " << std::fixed << std::setprecision(2) << avg << " ms\n";
                //std::cout << std::fixed << std::setprecision(2) << avg << "\n";

            }

            for (int i = 0; i < repeatCount; ++i) {
                delete[] baseArrays[i];
                baseArrays[i] = generateReverseSortedArray(arraySize);
            }
            double avgReverse = runIntroSortExperiment(baseArrays, arraySize, [](int*) {});
            std::cout << "Odwrotne:         " << std::fixed << std::setprecision(2) << avgReverse << " ms\n";
            //std::cout << std::fixed << std::setprecision(2) << avgReverse << "\n";

            for (int i = 0; i < repeatCount; ++i) delete[] baseArrays[i];
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
