#pragma once

#include <tbb/mutex.h>
#include <minpt/math/color4.h>
#include <minpt/core/filter.h>
#include <minpt/utils/bitmap.h>

namespace minpt {

class ImageBlock : public Eigen::Array<Color4f, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> {
public:
  ImageBlock(const Vector2i& size, const Filter* filter);

  ~ImageBlock() {
    delete[] filters;
    delete[] weightsX;
    delete[] weightsY;
  }

  void clear() {
    setConstant(Color4f());
  }

  void fromBitmap(const Bitmap& bitmap) {
    if (bitmap.cols() != cols() || bitmap.rows() != rows())
      throw Exception("Invalid bitmap dimensions!");
    for (auto y = 0; y < size.y; ++y)
      for (auto x = 0; x < size.x; ++x)
        coeffRef(y, x) = Color4f(bitmap(y, x));
  }

  Bitmap toBitmap() const {
    Bitmap result(size);
    for (auto y = 0; y < size.y; ++y)
      for (auto x = 0; x < size.x; ++x)
        result(y, x) = coeff(y + borderSize, x + borderSize).eval();
    return result;
  }

  void put(const Vector2f& pos, const Color3f& value);

  void put(const ImageBlock& block);

  void lock() const {
    mutex.lock();
  }

  void unlock() const {
    mutex.unlock();
  }

  std::string toString() const {
    return tfm::format(
      "ImageBlock[offset=%s, size=%s]",
      offset.toString(),
      size.toString()
    );
  }

public:
  int borderSize = 0;
  Vector2i offset;
  Vector2i size;

private:
  mutable tbb::mutex mutex;
  float filterRadius;
  float lookupFactor;
  float* filters = nullptr;
  float* weightsX = nullptr;
  float* weightsY = nullptr;
};

class BlockGenerator {
public:
  BlockGenerator(const Vector2i& size, int blockSize) noexcept
      : blockSize(blockSize)
      , size(size)
      , nBlocks(
        (size.x + blockSize - 1) / blockSize,
        (size.y + blockSize - 1) / blockSize) {
    blocksLeft = nBlocks.x * nBlocks.y;
  }

  int getBlockCount() const {
    return blocksLeft;
  }

  bool next(ImageBlock& block) {
    tbb::mutex::scoped_lock lock(mutex);

    if (!blocksLeft)
      return false;

    auto blockIndex = nBlocks.x * nBlocks.y - blocksLeft;
    Vector2i tile(blockIndex % nBlocks.x, blockIndex / nBlocks.x);
    block.offset = tile * blockSize;
    block.size = min(size - block.offset, Vector2i(blockSize));
    --blocksLeft;

    return true;
  }

private:
  int blockSize;
  int blocksLeft;
  Vector2i size;
  Vector2i nBlocks;
  tbb::mutex mutex;
};

}
