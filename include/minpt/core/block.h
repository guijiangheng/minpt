#pragma once

#include <tbb/mutex.h>
#include <minpt/math/color4.h>
#include <minpt/core/filter.h>
#include <minpt/utils/bitmap.h>

namespace minpt {

class ImageBlock : public Eigen::Array<Color4f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
public:
  ImageBlock(const Vector2i& size, const Filter* filter);

  ~ImageBlock();

  void clear() {
    setConstant(Color4f());
  }

  void fromBitmap(const Bitmap& bitmap);

  Bitmap toBitmap() const;

  void put(const Vector2f& pos, const Color3f& value);

  void put(ImageBlock& block);

  std::string toString() const;

private:
  mutable tbb::mutex mutex;
  int borderSize = 0;
};

}
