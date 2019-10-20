#pragma once

#include <cmath>
#include <tinyformat.h>

namespace minpt {

template <typename T>
class Vector2 {
public:
  Vector2() noexcept = default;

  explicit Vector2(T x) noexcept : x(x), y(x)
  { }

  Vector2(T x, T y) noexcept : x(x), y(y)
  { }

  template <typename U>
  explicit Vector2(const Vector2<U>& v) noexcept : x((T)v.x), y((T)v.y)
  { }

  T& operator[](int index) {
    return (&x)[index];
  }

  T operator[](int index) const {
    return (&x)[index];
  }

  Vector2 operator+(const Vector2& v) const {
    return Vector2(x + v.x, y + v.y);
  }

  Vector2& operator+=(const Vector2& v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  Vector2 operator-(const Vector2& v) const {
    return { x - v.x, y - v.y };
  }

  Vector2& operator-=(const Vector2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  Vector2 operator*(T k) const {
    return Vector2(x * k, y * k);
  }

  Vector2& operator*=(T k) {
    x *= k;
    y *= k;
    return *this;
  }

  Vector2 operator/(T k) const {
    k = 1 / k;
    return Vector2(x * k, y * k);
  }

  Vector2 operator/=(T k) {
    k = 1 / k;
    x *= k;
    y *= k;
    return *this;
  }

  Vector2 operator-() const {
    return Vector2(-x, -y);
  }

  Vector2& normalize() {
    *this /= length();
    return *this;
  }

  T lengthSquared() const {
    return x * x + y * y;
  }

  T length() const {
    return std::sqrt(lengthSquared());
  }

  bool operator==(const Vector2& v) const {
    return x == v.x && y == v.y;
  }

  bool operator!=(const Vector2& v) const {
    return x != v.x || y != v.y;
  }

  std::string toString() const {
    return tfm::format(
      "[%s, %s]",
      std::to_string(x),
      std::to_string(y)
    );
  }

public:
  T x, y;
  using Scalar = T;
};

using Vector2i = Vector2<int>;
using Vector2f = Vector2<float>;

template <typename T>
T dot(const Vector2<T>& a, const Vector2<T>& b) {
  return a.x * b.x + a.y * b.y;
}

template <typename T>
T absdot(const Vector2<T>& a, const Vector2<T>& b) {
  return std::abs(dot(a, b));
}

template <typename T>
Vector2<T> abs(const Vector2<T>& v) {
  return Vector2<T>(std::abs(v.x), std::abs(v.y));
}

template <typename T>
Vector2<T> min(const Vector2<T>& a, const Vector2<T>& b) {
  return Vector2<T>(
    std::min(a.x, b.x),
    std::min(a.y, b.y)
  );
}

template <typename T>
Vector2<T> max(const Vector2<T>& a, const Vector2<T>& b) {
  return Vector2<T>(
    std::max(a.x, b.x),
    std::max(a.y, b.y)
  );
}

}
