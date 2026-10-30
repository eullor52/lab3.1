#pragma once

#include <stdexcept>
#include <utility>
#include "exceptions.hpp"
#include <cstddef>

template <typename T>
class DynamicArray {
private:
    T* data = nullptr;
    size_t length = 0;
public:
    class Iterator {
    private:
        T* current;
    public:
        Iterator(T* ptr = nullptr) : current(ptr) {};
        T& operator*() const {
            return *current;
        }
        Iterator& operator++() {
            ++current;
            return *this;
        }
        bool operator==(const Iterator& other) const {
            return current == other.current;
        }
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };
    DynamicArray() = default;
    DynamicArray(T* items, size_t count);
    DynamicArray(size_t size);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray(DynamicArray<T>&& other) noexcept;
    ~DynamicArray();
    T Get(size_t index) const;
    size_t GetSize() const;
    void Set(size_t index, T value);
    void Resize(size_t newSize);
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept;
    Iterator begin() const;
    Iterator end() const;
};

template <typename T>
DynamicArray<T>::DynamicArray(T* items, size_t count) {
    if (count > 0 && !items) {
        throw InvalidArgumentException("Invalid initialization parameters.");
    }
    T* tmp = count ? new T[count] : nullptr;
    try {
        for (size_t i = 0; i < count; ++i) {
            tmp[i] = items[i];
        }
    } catch (...) {
        delete[] tmp;
        throw;
    }
    data = tmp;
    length = count;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) {
    if (other.length == 0) {
        data = nullptr;
        length = 0;
        return;
    }
    T* tmp = new T[other.length];
    try {
        for (size_t i = 0; i < other.length; ++i) {
            tmp[i] = other.data[i];
        }
    } catch (...) {
        delete[] tmp;
        throw;
    }
    data = tmp;
    length = other.length;
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& other) noexcept {
    data = other.data;
    length = other.length;
    other.data = nullptr;
    other.length = 0;
}

template <typename T>
DynamicArray<T>::DynamicArray(size_t size) {
    if (size == 0) {
        data = nullptr;
        length = 0;
        return;
    }
    data = new T[size]();
    length = size;
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

template <typename T>
size_t DynamicArray<T>::GetSize() const {
    return length;
}

template <typename T>
T DynamicArray<T>::Get(size_t index) const {
    if (index >= length) {
        throw OutOfRangeException("Index out of range.");
    }
    return data[index];
}

template <typename T>
void DynamicArray<T>::Set(size_t index, T value) {
    if (index >= length) {
        throw OutOfRangeException("Index out of range.");
    }
    data[index] = value;
}

template <typename T>
void DynamicArray<T>::Resize(size_t newSize) {
    T* tmp = newSize ? new T[newSize] : nullptr;
    try {
        size_t copyCount = (newSize < length) ? newSize : length;
        for (size_t i = 0; i < copyCount; ++i) {
            tmp[i] = std::move(data[i]);
        }
    } catch (...) {
        delete[] tmp;
        throw;
    }
    delete[] data;
    data = tmp;
    length = newSize;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        Resize(other.length);
        for (size_t i = 0; i < other.length; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept {
    if (this != &other) {
        delete[] data;
        data = other.data;
        length = other.length;
        other.data = nullptr;
        other.length = 0;
    }
    return *this;
}

template <typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::begin() const {
    return Iterator(data);
}

template <typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::end() const {
    return Iterator(data + length);
}