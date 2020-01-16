#pragma once

#include <minpt/core/texture.h>
#include <minpt/utils/bitmap.h>

namespace minpt {

template <typename T>
class ImageTexture : public Texture<T> {
public:
  ImageTexture(const PropertyList& props);

  T eval(const Vector2f& _uv) const override {
    auto uv = mapping.map(_uv);
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
    return data[t * width + s];
  }

  std::string toString() const override {
    return tfm::format(
      "ImageTexture[\n"
      "  filename = %s,\n"
      "  width = %d,\n"
      "  height = %d,\n"
      "  scale = %f,\n"
      "  mapping = %s\n"
      "]",
      filename, width, height, scale, indent(mapping.toString())
    );
  }

public:
  std::string filename;
  int width;
  int height;
  float scale;
  TextureMapping2D mapping;
  std::unique_ptr<T[]> data;
};

template <>
ImageTexture<float>::ImageTexture(const PropertyList& props)
    : filename(getFileResolver()->resolve(props.getString("filename")).str())
    , scale(props.getFloat("color_scale", 1.0f))
    , mapping(
        props.getVector2f("scale", Vector2f(1.0f)),
        props.getVector2f("delta", Vector2f(0.0f))) {

  Bitmap bitmap(filename);
  width = bitmap.cols();
  height = bitmap.rows();
  data = std::make_unique<float[]>(width * height);

  auto i = 0;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
      data[i++] = scale * bitmap(height - 1 - y, x).y();
}

template <>
ImageTexture<Color3f>::ImageTexture(const PropertyList& props)
    : filename(getFileResolver()->resolve(props.getString("filename")).str())
    , scale(props.getFloat("color_scale", 1.0f))
    , mapping(
        props.getVector2f("scale", Vector2f(1.0f)),
        props.getVector2f("delta", Vector2f(0.0f))) {

  Bitmap bitmap(filename);
  width = bitmap.cols();
  height = bitmap.rows();
  data = std::make_unique<Color3f[]>(width * height);

  auto i = 0;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
      data[i++] = bitmap(height - 1 - y, x) * scale;
}

}
