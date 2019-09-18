#pragma once

#include <tinyformat.h>
#include <Eigen/Core>

namespace minpt {

class Color3f : public Eigen::Array3f {
public:
  using Base = Eigen::Array3f;

  Color3f(float value = 0.0f) : Base(value, value, value)
  { }

  Color3f(float r, float g, float b) : Base(r, g, b)
  { }

  float& r() {
    return x();
  }

  float r() const {
    return x();
  }

  float& g() {
    return y();
  }

  float g() const {
    return y();
  }

  float& b() {
    return z();
  }

  float b() const {
    return z();
  }

  float getLuminance() const {
    return coeff(0) * 0.212671f + coeff(1) * 0.715160f + coeff(2) * 0.072169f;
  }

  std::string toString() const {
    return tfm::format("[%f, %f, %f]", coeff(0), coeff(1), coeff(2));
  }
};

}
