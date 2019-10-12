#pragma once

#include <memory>
#include <minpt/core/camera.h>

namespace minpt {

class Sampler : public Object {
public:
  Sampler() = default;

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

  CameraSample getCameraSample(const Vector2i& pFilm) {
    return { Vector2f((float)pFilm.x(), (float)pFilm.y()) + get2D() };
  }

  virtual void prepare(const Vector2i& block) = 0;

  virtual float get1D() = 0;

  virtual Vector2f get2D() = 0;

  virtual std::unique_ptr<Sampler> clone() const = 0;

  EClassType getClassType() const override {
    return ESampler;
  }

public:
  std::int64_t samplesPerPixel;

protected:
  std::int64_t currentPixelSampleIndex;
};

}
