#pragma once
#include <utility>

template <typename T1, typename T2>
class Pair {
public:
    T1 first;
    T2 second;

    Pair() = default;

    Pair(const T1& f, const T2& s) : first(f), second(s) {}

    Pair(T1&& f, T2&& s) : first(std::move(f)), second(std::move(s)) {}

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