#pragma once

#include <minpt/math/vector3.h>

namespace minpt {

class Color3f {
public:
  Color3f() noexcept = default;

  explicit Color3f(float v) noexcept : r(v), g(v), b(v)
  { }

  Color3f(float r, float g, float b) noexcept : r(r), g(g), b(b)
  { }

  explicit Color3f(const Vector3f& v) noexcept : r(v.x), g(v.y), b(v.z)
  { }

  Color3f operator+(const Color3f& rhs) const {
    return Color3f(r + rhs.r, g + rhs.g, b + rhs.b);
  }

  Color3f& operator+=(const Color3f& rhs) {
    r += rhs.r;
    g += rhs.g;
    b += rhs.b;
    return *this;
  }

  Color3f operator-(const Color3f& rhs) const {
    return Color3f(r - rhs.r, g - rhs.g, b - rhs.b);
  }

  Color3f& operator-=(const Color3f& rhs) {
    r -= rhs.r;
    g -= rhs.g;
    b -= rhs.b;
    return *this;
  }

  Color3f operator*(float k) const {
    return Color3f(r * k, g * k, b * k);
  }

  Color3f& operator*=(float k) {
    r *= k;
    g *= k;
    b *= k;
    return *this;
  }

  Color3f operator/(float k) const {
    k = 1 / k;
    return Color3f(r * k, g * k, b * k);
  }

  Color3f& operator/=(float k) {
    k = 1 / k;
    r *= k;
    g *= k;
    b *= k;
    return *this;
  }

  Color3f operator*(const Color3f& rhs) const {
    return Color3f(r * rhs.r, g * rhs.g, b * rhs.b);
  }

  Color3f& operator*=(const Color3f& rhs) {
    r *= rhs.r;
    g *= rhs.g;
    b *= rhs.b;
    return *this;
  }

  Color3f operator/(const Color3f& rhs) const {
    return Color3f(r / rhs.r, g / rhs.g, b / rhs.b);
  }

  Color3f operator/=(const Color3f& rhs) {
    r /= rhs.r;
    g /= rhs.g;
    b /= rhs.b;
    return *this;
  }

  Color3f toRGB() const {
    return Color3f(
       3.240479f * r - 1.537150f * g - 0.498535f * b,
      -0.969256f * r + 1.875991f * g + 0.041556f * b,
       0.055648f * r - 0.204043f * g + 1.057311f * b
    );
  }

  Color3f toXYZ() const {
    return Color3f(
      0.412453f * b + 0.357580f * g + 0.180423f * b,
      0.212671f * b + 0.715160f * g + 0.072169f * b,
      0.019334f * b + 0.119193f * g + 0.950227f * b
    );
  }

  float& operator[](int index) {
    return (&r)[index];
  }

  float operator[](int index) const {
    return (&r)[index];
  }

  float y() const {
    return r * 0.212671f + g * 0.715160f + b * 0.072169f;
  }

  float maxComponent() const {
    return r > g ? (r > b ? r : b) : (g > b ? g : b);
  }

  bool isBlack() const {
    return r == 0 && g == 0 && b == 0;
  }

  bool isValid() const {
    if (r < 0 || !std::isfinite(r)) return false;
    if (g < 0 || !std::isfinite(g)) return false;
    if (b < 0 || !std::isfinite(b)) return false;
    return true;
  }

  bool operator==(const Color3f& rhs) const {
    return r == rhs.r && g == rhs.g && b == rhs.b;
  }

  bool operator!=(const Color3f& rhs) const {
    return r != rhs.r || g != rhs.g || b != rhs.b;
  }

  std::string toString() const {
    return tfm::format("[%f, %f, %f]", r, g, b);
  }

public:
  float r, g, b;
  using Scalar = float;
};

inline float gammaCorrect(float v) {
  if (v <= 0.0031308f) return 12.92f * v;
  return 1.055f * std::pow(v, (1.f / 2.4f)) - 0.055f;
}

inline float inverseGammaCorrect(float v) {
  if (v <= 0.04045f) return v / 12.92f;
  return std::pow((v + 0.055f) / 1.055f, 2.4f);
}

inline Color3f gammaCorrect(const Color3f& c) {
  return Color3f(
    gammaCorrect(c.r),
    gammaCorrect(c.g),
    gammaCorrect(c.b)
  );
}

inline Color3f inverseGammaCorrect(const Color3f& c) {
  return Color3f(
    inverseGammaCorrect(c.r),
    inverseGammaCorrect(c.g),
    inverseGammaCorrect(c.b)
  );
}

}
