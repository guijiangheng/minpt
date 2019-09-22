#pragma once

#include <pcg32.h>
#include <minpt/core/sampler.h>

namespace minpt {

class RandomSampler : public Sampler {
public:
  explicit RandomSampler(std::int64_t samplesPerPixel) noexcept : Sampler(samplesPerPixel)
  { }

  void prepare(const Vector2i& block) override {
    random.seed(block.x(), block.y());
  }

  float get1D() override {
    return random.nextFloat();
  }

  Vector2f get2D() override {
    return Vector2f(random.nextFloat(), random.nextFloat());
  }

  std::unique_ptr<Sampler> clone() const override {
    auto cloned = new RandomSampler(samplesPerPixel);
    cloned->random = random;
    return std::unique_ptr<Sampler>(cloned);
  }

  std::string toString() const override {
    return tfm::format("RandomSampler[samplesPerPixel=%i]", samplesPerPixel);
  }

private:
  pcg32 random;
};

}
