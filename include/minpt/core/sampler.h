#pragma once

#include <memory>
#include <minpt/math/vector.h>

namespace minpt {

class Sampler {
public:
  explicit Sampler(std::int64_t samplesPerPixel) noexcept
    : samplesPerPixel(samplesPerPixel)
  { }

  virtual ~Sampler() = default;

  void startPixel() {
    currentPixelSampleIndex = 0;
  }

  bool startNextSample() {
    return ++currentPixelSampleIndex < samplesPerPixel;
  }

  virtual void prepare(const Vector2i& block) = 0;

  virtual float get1D() = 0;

  virtual Vector2f get2D() = 0;

  virtual std::unique_ptr<Sampler> clone() const = 0;

  virtual std::string toString() const = 0;

public:
  std::int64_t samplesPerPixel;

protected:
  std::int64_t currentPixelSampleIndex;
};

}
