#pragma once

#include <minpt/math/math.h>
#include <minpt/core/exception.h>

namespace minpt {

enum class ImageWrap { Repeat, Black, Clamp };

inline std::string toString(ImageWrap warpMode) {
  switch (warpMode) {
    case ImageWrap::Repeat: return "Repeat";
    case ImageWrap::Black: return "Black";
    case ImageWrap::Clamp: return "Clamp";
  }
}

inline ImageWrap toImageWrap(const std::string& string) {
  if (string == "Repeat") return ImageWrap::Repeat;
  if (string == "Black") return ImageWrap::Black;
  if (string == "Clamp") return ImageWrap::Clamp;
  throw Exception("Unknown ImageWrap enum value \"%s\"!", string);
}

template <typename T>
class MIPMap {
public:
  MIPMap(const Vector2i& resolution, ImageWrap wrapMode = ImageWrap::Repeat)
      : resolution(resolution)
      , wrapMode(wrapMode)
      , data(new T[resolution.x * resolution.y])
  { }

  const T& texel(int s, int t) const {
    switch (wrapMode) {
    case ImageWrap::Repeat:
      s = mod(s, resolution.x);
      t = mod(t, resolution.y);
      break;
    case ImageWrap::Clamp:
      s = clamp(s, 0, resolution.x - 1);
      t = clamp(t, 0, resolution.y - 1);
      break;
    case ImageWrap::Black:
      static const T black(0.0f);
      if (s < 0 || s >= resolution.x || t < 0 || t >= resolution.y)
        return black;
      break;
    }
    return data[t * resolution.x + s];
  }

  T lookup(const Vector2f& uv) const {
    auto s = uv.x * resolution.x - 0.5f;
    auto t = uv.y * resolution.y - 0.5f;
    auto s0 = std::floor(s), t0 = std::floor(t);
    auto ds = s - s0, dt = t - t0;
    return texel(s0, t0) * (1 - ds) * (1 - dt) +
           texel(s0, t0 + 1) * (1 - ds) * dt +
           texel(s0 + 1, t0) * ds * (1 - dt) +
           texel(s0 + 1, t0 + 1) * ds * dt;
  }

  std::string toString() const {
    return tfm::format(
      "MipMap[\n"
      "  resolution = %s,\n"
      "  wrapMode = %s\n"
      "]",
      resolution.toString(), minpt::toString(wrapMode)
    );
  }

public:
  Vector2i resolution;
  ImageWrap wrapMode;
  std::unique_ptr<T[]> data;
};

}
