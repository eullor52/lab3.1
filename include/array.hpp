#pragma once

#include <stdexcept>
#include <utility>
#include "exceptions.hpp"
#include <cstddef>
#include <memory>

template <typename T>
class DynamicArray {
private:
    std::shared_ptr<T[]> data;
    size_t length = 0;
public:
    class Iterator {
    private:
        std::weak_ptr<T[]> arrayData;
        size_t currentIndex = 0;
    public:
        Iterator(std::shared_ptr<T[]> pointer = nullptr, size_t index = 0) 
            : arrayData(pointer), currentIndex(index) {}

        T& operator*() const { 
            auto sharedData = arrayData.lock();
            if (!sharedData) {
                throw NullPointerException("Pointer is expired or null.");
            }
            return sharedData[currentIndex]; 
        }

        Iterator& operator++() { 
            ++currentIndex; 
            return *this; 
        }

        bool operator==(const Iterator& other) const { 
            return currentIndex == other.currentIndex && arrayData.lock() == other.arrayData.lock(); 
        }

        bool operator!=(const Iterator& other) const { 
            return !(*this == other); 
        }
    };

    DynamicArray() = default;
    DynamicArray(const std::shared_ptr<T[]>& items, size_t count);
    DynamicArray(size_t size);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray(DynamicArray<T>&& other) noexcept;
    ~DynamicArray() = default;
    T get(size_t index) const;
    size_t getSize() const;
    void set(size_t index, T value);
    void resize(size_t newSize);
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(DynamicArray<T>&& other) noexcept;
    Iterator begin() const;
    Iterator end() const;
};

template <typename T>
DynamicArray<T>::DynamicArray(const std::shared_ptr<T[]>& items, size_t count) {
    if (count > 0 && !items) {
        throw InvalidArgumentException("Invalid initialization parameters.");
    }
    std::shared_ptr<T[]> temporaryArray(count ? new T[count] : nullptr);
    for (size_t i = 0; i < count; ++i) {
        temporaryArray[i] = items[i];
    }
    data = std::move(temporaryArray);
    length = count;
}

template <typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other) {
    if (other.length == 0) {
        length = 0;
        return;
    }
    std::shared_ptr<T[]> temporaryArray(new T[other.length]);
    for (size_t i = 0; i < other.length; ++i) {
        temporaryArray[i] = other.data[i];
    }
    data = std::move(temporaryArray);
    length = other.length;
}

template <typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& other) noexcept 
    : data(std::move(other.data)), length(other.length) {
    other.length = 0;
}

template <typename T>
DynamicArray<T>::DynamicArray(size_t size) {
    if (size == 0) {
        length = 0;
        return;
    }
    data = std::shared_ptr<T[]>(new T[size]());
    length = size;
}

template <typename T>
size_t DynamicArray<T>::getSize() const {
    return length;
}

template <typename T>
T DynamicArray<T>::get(size_t index) const {
    if (index >= length) {
        throw OutOfRangeException("Index out of range.");
    }
    return data[index];
}

template <typename T>
void DynamicArray<T>::set(size_t index, T value) {
    if (index >= length) {
        throw OutOfRangeException("Index out of range.");
    }
    data[index] = value;
}

template <typename T>
void DynamicArray<T>::resize(size_t newSize) {
    std::shared_ptr<T[]> temporaryArray(newSize ? new T[newSize] : nullptr);
    size_t copyCount = (newSize < length) ? newSize : length;
    for (size_t i = 0; i < copyCount; ++i) {
        temporaryArray[i] = std::move(data[i]);
    }
    data = std::move(temporaryArray);
    length = newSize;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        resize(other.length);
        for (size_t i = 0; i < other.length; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& other) noexcept {
    if (this != &other) {
        data = std::move(other.data);
        length = other.length;
        other.length = 0;
    }
    return *this;
}

template <typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::begin() const {
    return Iterator(data, 0);
}

template <typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::end() const {
    return Iterator(data, length);
}