#pragma once

#include <minpt/core/mipmap.h>
#include <minpt/utils/bitmap.h>
#include <minpt/core/texture.h>

namespace minpt {

template <typename T>
class ImageTexture : public Texture<T> {
public:
  ImageTexture(const PropertyList& props);

  T eval(const Vector2f& uv) const override {
    return mipmap->lookup(mapping.map(uv));
  }

  std::string toString() const override {
    return tfm::format(
      "ImageTexture[\n"
      "  filename = %s,\n"
      "  scale = %f,\n"
      "  resolution = %s,\n"
      "  wrapMode = %s,\n"
      "  mapping = %s,\n"
      "]",
      filename,
      scale,
      mipmap->resolution.toString(),
      minpt::toString(mipmap->wrapMode),
      indent(mapping.toString())
    );
  }

public:
  std::string filename;
  float scale;
  TextureMapping2D mapping;
  std::unique_ptr<MIPMap<T>> mipmap;
};

template <>
ImageTexture<float>::ImageTexture(const PropertyList& props)
    : filename(getFileResolver()->resolve(props.getString("filename")).str())
    , scale(props.getFloat("color_scale", 1.0f))
    , mapping(
        props.getVector2f("scale", Vector2f(1.0f)),
        props.getVector2f("delta", Vector2f(0.0f))) {

  Bitmap bitmap(filename);
  auto width = bitmap.cols();
  auto height = bitmap.rows();
  auto wrapMode = toImageWrap(props.getString("wrapMode", "Repeat"));
  mipmap.reset(new MIPMap<float>(Vector2i(width, height), wrapMode));
  auto data = mipmap->data.get();

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
  auto width = bitmap.cols();
  auto height = bitmap.rows();
  auto wrapMode = toImageWrap(props.getString("wrapMode", "Repeat"));
  mipmap.reset(new MIPMap<Color3f>(Vector2i(width, height), wrapMode));
  auto data = mipmap->data.get();

  auto i = 0;
  for (auto y = 0; y < height; ++y)
    for (auto x = 0; x < width; ++x)
      data[i++] = bitmap(height - 1 - y, x) * scale;
}

}
