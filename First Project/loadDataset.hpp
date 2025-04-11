#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

template <typename ListType>
void loadDataset(const std::string& fileName, ListType& list, std::size_t limit) {
    std::ifstream inFile(fileName);
    if (!inFile) {
        std::cerr << "Nie mozna otworzyc pliku do odczytu!" << std::endl;
        return;
    }

    std::string line;
    std::size_t count = 0;
    while (std::getline(inFile, line) && count < limit) {
        std::stringstream ss(line);
        std::string name;
        float value;

        if (std::getline(ss, name, ',') && ss >> value) {
            list.addBack(std::make_pair(name, value));
            ++count;
        }
    }

    inFile.close();
}
