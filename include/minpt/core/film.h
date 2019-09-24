#pragma once

#include <minpt/math/color.h>
#include <minpt/math/bounds.h>

namespace minpt {

class Film {
public:
  Film(const Vector2i& resolution,
       const Bounds2f& cropWindow = Bounds2f(Vector2f(0), Vector2f(1)))
    : resolution(resolution)
    , pixelBounds(
      Vector2i((int)std::ceil(resolution.x() * cropWindow.min().x()), (int)std::ceil(resolution.y() * cropWindow.min().y())),
      Vector2i((int)std::ceil(resolution.x() * cropWindow.max().x()), (int)std::ceil(resolution.y() * cropWindow.max().y()))
    ) {
    auto diag = pixelBounds.diagonal();
    pixels.resize(diag.y(), diag.x());
  }

public:
  Vector2i resolution;
  Bounds2i pixelBounds;
  Eigen::Array<Color3f, Eigen::Dynamic, Eigen::Dynamic> pixels;
};

}
