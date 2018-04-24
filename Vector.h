//
// Created by serko on 24.04.2018.
//

#ifndef UNTITLED7_VECTOR_H
#define UNTITLED7_VECTOR_H

#include <cmath>
#include <windef.h>

template<typename T = int>
class Vector {
public:
    T x, y;

    explicit Vector(T x = 0, T y = 0);

    Vector(const std::pair<T, T> p);

    const long double length() const;

    const Vector<long double> ort() const;

    Vector<T> &operator=(const Vector &v)= default;

    const Vector<T> operator+(Vector v) const;

    const Vector<T> operator+(T a) const;

    friend const Vector<T> operator+(T a, const Vector<T> &v);

    const Vector<T> operator*(T a) const;

    friend const Vector<T> operator*(T a, const Vector<T> &v);

    const Vector<T> operator*(const Vector<T> &v);

    operator const std::pair<T, T>() const;

    operator POINT() const;

    const Vector<T> operator-(const Vector<T> v) const;

};

template<typename T>
Vector<T>::Vector(T x, T y) : x(x), y(y) {}

template<typename T>
const long double Vector<T>::length() const {
    return sqrt(x * x + y * y);
}

template<typename T>
const Vector<long double> Vector<T>::ort() const {
    return Vector<long double>(static_cast<long double>(x) / length(), static_cast<long double>(y) / length());
}

template<typename T>
const Vector<T> Vector<T>::operator+(const Vector v) const {
    return Vector(x + v.x, y + v.y);
}

template<typename T>
const Vector<T> Vector<T>::operator+(const T a) const {
    return Vector(x + a, y + a);
}

template<typename T>
const Vector<T> operator+(const T a, const Vector<T> &v) {
    return Vector(v.x + a, v.y + a);
}

template<typename T>
const Vector<T> Vector<T>::operator*(const T a) const {
    return Vector(x * a, y * a);
}

template<typename T>
const Vector<T> operator*(const T a, const Vector<T> &v) {
    return Vector(v.x * a, v.y * a);
}

template<typename T>
const Vector<T> Vector<T>::operator*(const Vector<T> &v) {
    return x * v.x + y * v.y;
}

template<typename T>
Vector<T>::operator const std::pair<T, T>() const {
    return std::make_pair(x, y);
}

template<typename T>
Vector<T>::Vector(const std::pair<T, T> p):x(p.first), y(p.second) {}

template<typename T>
Vector<T>::operator POINT() const {
    POINT a;
    a.x = static_cast<LONG>(x);
    a.y = static_cast<LONG>(y);
    return a;
}

template<typename T>
const Vector<T> Vector<T>::operator-(const Vector<T> v) const {
    return (operator+(v * -1));
}

#endif //UNTITLED7_VECTOR_H
