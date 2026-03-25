#pragma once

#include <ostream>
#include <functional>
#include <sstream>
#include <cmath>

template <typename T>
class Vec {
public:
    Vec(T x=T{}, T y=T{})
        :x{x}, y{y} {}

    T x, y;
};

template <typename T>
Vec<T>& operator+=(Vec<T>& left, const Vec<T>& right) {
    left.x += right.x;
    left.y += right.y;
    return left;
}

template <typename T>
Vec<T> operator+(Vec<T> left, const Vec<T>& right) {
    return left += right;
}

template <typename T>
Vec<T>& operator-=(Vec<T>& left, const Vec<T>& right) {
    left.x -= right.x;
    left.y -= right.y;
    return left;
}

template <typename T>
Vec<T> operator-(Vec<T> left, const Vec<T>& right) {
    return left -= right;
}

template <typename T>
Vec<T>& operator*=(Vec<T>& left, const Vec<T>& right) {
    left.x *= right.x;
    left.y *= right.y;
    return left;
}

template <typename T>
Vec<T> operator*(Vec<T> left, const Vec<T>& right) {
    return left *= right;
}


template <typename T>
Vec<T> operator*(const Vec<T>& left, T scalar) {
    return left * Vec{scalar, scalar};
}

template <typename T>
Vec<T> operator*(T scalar, const Vec<T>& right) {
    return right * scalar;
}

template <typename T>
Vec<T>& operator/=(Vec<T>& left, T scalar) {
    if (scalar == 0) {
        std::stringstream msg{"Cannot divide "};
        msg << left << " by " << scalar;
        throw std::runtime_error(msg.str());
    }
    left.x /= scalar;
    left.y /= scalar;
    return left;
}

template <typename T>
Vec<T> operator/(Vec<T> left, T scalar) {
    return left /= scalar;
}

template <typename T>
bool operator==(const Vec<T>& left, const Vec<T>& right) {
    return left.x == right.x && left.y == right.y;
}

template <typename T>
bool operator!=(const Vec<T>& left, const Vec<T>& right) {
    return !(left == right);
}

template <typename T>
bool operator<(const Vec<T>& left, const Vec<T>& right) {
    if (left.x != right.x) {
        return left.x < right.x;
    }
    return left.y < right.y;
}

template <typename T>
float distance(const Vec<T>& a, const Vec<T>& b) {
    Vec difference = a - b;
    return length(difference);
}

template <typename T>
float length(const Vec<T>& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

template <typename T>
Vec<T> unit(const Vec<T>& v) {
    float length = length(v);
    return v / length;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec<T>& vec) {
    return os << "(" << vec.x << ", " << vec.y << ")";
}