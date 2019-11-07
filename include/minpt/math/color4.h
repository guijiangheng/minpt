#pragma once

#include <minpt/math/color3.h>

namespace minpt {

class Color4f {
public:
  Color4f() noexcept : r(0.0f), g(0.0f), b(0.0f), w(0.0f)
  { }

  explicit Color4f(const Color3f& c) noexcept
    : r(c.r), g(c.g), b(c.b), w(1.0f)
  { }

  Color4f(float r, float g, float b, float w) noexcept
    : r(r), g(g), b(b), w(w)
  { }

  Color3f eval() const {
    return w == 0 ? Color3f(0.0f) : Color3f(r, g, b) / w;
  }

  std::string toString() const {
    return tfm::format("[%f, %f, %f, %f]", r, g, b, w);
  }

public:
  float r, g, b, w;
};

}
