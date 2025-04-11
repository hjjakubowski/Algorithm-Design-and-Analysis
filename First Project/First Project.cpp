#include <iostream>
#include <string> 
#include "ArrayList.hpp"
#include "mergeSort.hpp"
#include "loadDataset.hpp"

int main()
{
    Arraylist<std::pair<std::string, float>> myList;

    loadDataset("dataset.csv", myList, 10);

    std::cout << "Przed sortowaniem:" << std::endl;
    for (std::size_t i = 0; i < myList.getSize(); ++i) {
        std::cout << myList[i].first << " : " << myList[i].second << std::endl;
    }


    mergeSort(myList, [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
        return a.second < b.second;
        });

    std::cout << "Po sortowaniu:" << std::endl;
    for (std::size_t i = 0; i < myList.getSize(); ++i) {
        std::cout << myList[i].first << " : " << myList[i].second << std::endl;
    }

    return 0;
}
