#pragma once
#include <utility>

template <typename T1, typename T2>
class Pair {
public:
    T1 first;
    T2 second;

    Pair() = default;

    Pair(const T1& firstValue, const T2& secondValue) : first(firstValue), second(secondValue) {}

    Pair(T1&& firstValue, T2&& secondValue) : first(std::move(firstValue)), second(std::move(secondValue)) {}

    Pair(const Pair& other) = default;
    Pair& operator=(const Pair& other) = default;

    Pair(Pair&& other) noexcept = default;
    Pair& operator=(Pair&& other) noexcept = default;

    bool operator==(const Pair& other) const {
        return first == other.first && second == other.second;
    }

    bool operator!=(const Pair& other) const {
        return !(*this == other);
    }
};