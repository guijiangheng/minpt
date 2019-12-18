#pragma once

#include <minpt/core/texture.h>
#include <minpt/utils/bitmap.h>

namespace minpt {

template <typename T>
class ImageTexture : public Texture<T> {
public:
  ImageTexture(const PropertyList& props);

  std::string toString() const override {
    return tfm::format(
      "ImageTexture[\n"
      "  filename = %s,\n"
      "  width = %d,\n"
      "  height = %d\n"
      "]",
      filename, std::to_string(width), std::to_string(height)
    );
  }

  T eval(const Vector2f& uv) const override {
    auto s = uv.x * width - 0.5f;
    auto t = uv.y * height - 0.5f;
    auto s0 = (int)std::floor(s);
    auto t0 = (int)std::floor(t);
    auto ds = s - s0;
    auto dt = t - t0;
    return texel(s0, t0) * (1 - ds) * (1 - dt) +
           texel(s0, t0 + 1) * (1 - ds) * dt +
           texel(s0 + 1, t0) * ds * (1 - dt) +
           texel(s0 + 1, t0 + 1) * ds * dt;
  }

  T texel(int s, int t) const {
    s = s % width;
    t = t % height;
    s = s < 0 ? s + width : s;
    t = t < 0 ? t + height : t;
    return data[t * height + s];
  }

private:
  std::string filename;
  int width;
  int height;
  std::unique_ptr<T[]> data;
};

template <>
ImageTexture<float>::ImageTexture(const PropertyList& props)
    : filename(getFileResolver()->resolve(props.getString("filename")).str()) {

  Bitmap bitmap(filename);
  width = bitmap.cols();
  height = bitmap.rows();
  data = std::make_unique<float[]>(width * height);

  auto i = 0;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
      data[i++] = bitmap(y, x).y();
}

template <>
ImageTexture<Color3f>::ImageTexture(const PropertyList& props)
    : filename(getFileResolver()->resolve(props.getString("filename")).str()) {

  Bitmap bitmap(filename);
  width = bitmap.cols();
  height = bitmap.rows();
  data = std::make_unique<Color3f[]>(width * height);

  auto i = 0;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
      data[i++] = bitmap(y, x);
}

}
