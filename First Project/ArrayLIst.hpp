#pragma once
#include <iostream>
#include <stdexcept>

template <typename E>
class Arraylist {
private:
    E* arr;
    std::size_t size;
    std::size_t capacity;

    void increaseSize();

public:
    Arraylist(std::size_t capacity = 2);
    ~Arraylist();

    std::size_t getSize() const;
    std::size_t getCapacity() const;
    bool isEmpty() const;
    void clear();

    void addBack(const E& elem);
    void addFront(const E& elem);
    void add(const E& elem, std::size_t index);

    void removeBack();
    void removeFront();
    void remove(std::size_t index);

    std::size_t Find(const E& elem) const;

    E& operator[](std::size_t index);
    const E& operator[](std::size_t index) const;
};


template <typename E>
Arraylist<E>::Arraylist(std::size_t capacity) : size(0), capacity(capacity) {
    arr = new E[capacity];
}


template <typename E>
Arraylist<E>::~Arraylist() {
    delete[] arr;
}


template <typename E>
void Arraylist<E>::increaseSize() {
    if (size == capacity) {
        capacity *= 2;
        E* temp = new E[capacity];
        for (std::size_t i = 0; i < size; ++i) {
            temp[i] = arr[i];
        }
        delete[] arr;
        arr = temp;
    }
}


template <typename E>
std::size_t Arraylist<E>::getSize() const {
    return size;
}

template <typename E>
std::size_t Arraylist<E>::getCapacity() const {
    return capacity;
}

template <typename E>
bool Arraylist<E>::isEmpty() const {
    return size == 0;
}

template <typename E>
void Arraylist<E>::clear() {
    size = 0;
}


template <typename E>
void Arraylist<E>::addBack(const E& elem) {
    if (elem.first.size() > std::string::max_size()) {
        throw std::length_error("String too long");
    }

    increaseSize();
    arr[size++] = elem;
}

template <typename E>
void Arraylist<E>::addFront(const E& elem) {
    if (elem.first.size() > std::string::max_size()) {
        throw std::length_error("String too long");
    }

    increaseSize();
    for (std::size_t i = size; i > 0; --i) {
        arr[i] = arr[i - 1];
    }
    arr[0] = elem;
    ++size;
}

template <typename E>
void Arraylist<E>::add(const E& elem, std::size_t index) {
    if (elem.first.size() > std::string::max_size()) {
        throw std::length_error("String too long");
    }

    if (index > size) return;
    increaseSize();
    for (std::size_t i = size; i > index; --i) {
        arr[i] = arr[i - 1];
    }
    arr[index] = elem;
    ++size;
}


template <typename E>
void Arraylist<E>::removeBack() {
    if (size > 0) --size;
}

template <typename E>
void Arraylist<E>::removeFront() {
    if (size > 0) {
        for (std::size_t i = 1; i < size; ++i) {
            arr[i - 1] = arr[i];
        }
        --size;
    }
}

template <typename E>
void Arraylist<E>::remove(std::size_t index) {
    if (index >= size) return;
    for (std::size_t i = index; i < size - 1; ++i) {
        arr[i] = arr[i + 1];
    }
    --size;
}


template <typename E>
std::size_t Arraylist<E>::Find(const E& elem) const {
    for (std::size_t i = 0; i < size; ++i) {
        if (arr[i] == elem) return i;
    }
    return static_cast<std::size_t>(-1); 
}


template <typename E>
E& Arraylist<E>::operator[](std::size_t index) {
    if (index >= size) {
        throw std::out_of_range("Index out of bounds");
    }
    return arr[index];
}

template <typename E>
const E& Arraylist<E>::operator[](std::size_t index) const {
    if (index >= size) {
        throw std::out_of_range("Index out of bounds");
    }
    return arr[index];
}
