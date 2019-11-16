#pragma once

#include <vector>
#include <minpt/math/math.h>

namespace minpt {

class Distribution1D {
public:
  explicit Distribution1D(std::size_t nEntries = 0) {
    cdf.reserve(nEntries + 1);
    cdf.push_back(0.0f);
  }

  std::size_t size() const {
    return cdf.size() - 1;
  }

  void reserve(std::size_t nEntries) {
    cdf.reserve(nEntries + 1);
  }

  float operator[](size_t index) const {
    return cdf[index + 1] - cdf[index];
  }

  void append(float pdf) {
    cdf.push_back(cdf.back() + pdf);
  }

  float normalize() {
    sum = cdf.back();
    auto sumInv = 1 / sum;
    for (std::size_t i = 1; i < cdf.size() - 1; ++i)
      cdf[i] *= sumInv;
    cdf.back() = 1.0f;
    return sum;
  }

  std::size_t sample(float u) const {
    auto itr = std::upper_bound(cdf.begin(), cdf.end(), u);
    std::size_t index = itr - cdf.begin() - 1;
    return std::min(index, cdf.size() - 2);
  }

  std::size_t sampleReuse(float& u) const {
    auto index = sample(u);
    u = (u - cdf[index]) / operator[](index);
    return index;
  }

  std::size_t sample(float u, float& pdf) const {
    auto index = sample(u);
    pdf = operator[](index);
    return index;
  }

  std::size_t sampleReuse(float& u, float& pdf) const {
    auto index = sample(u, pdf);
    u = (u - cdf[index]) / operator[](index);
    return index;
  }

  float sampleContinuous(float u, float& pdf, std::size_t& index) const {
    index = sampleReuse(u, pdf);
    auto nEntries = cdf.size() - 1;
    pdf *= nEntries;
    return (index + u) / nEntries;
  }

public:
  float sum;
  std::vector<float> cdf;
};

class Distribution2D {
public:
  Distribution2D() = default;

  Distribution2D(const float* pdf, int width, int height) {
    pMarginal.reserve(height);
    pConditional.reserve(height);
    for (auto y = 0; y < height; ++y) {
      pConditional.emplace_back(width);
      for (auto x = 0; x < width; ++x)
        pConditional.back().append(pdf[y * width + x]);
      pMarginal.append(pConditional.back().normalize());
    }
    pMarginal.normalize();
  }

  Vector2f sampleContinuous(const Vector2f& sample, float& pdf) const {
    std::size_t x, y;
    float pdfs[2];
    auto v = pMarginal.sampleContinuous(sample[0], pdfs[0], y);
    auto u = pConditional[y].sampleContinuous(sample[1], pdfs[1], x);
    pdf = pdfs[0] * pdfs[1];
    return Vector2f(u, v);
  }

  float pdf(const Vector2f& uv) const {
    auto width = pConditional[0].size();
    auto height = pMarginal.size();
    auto x = (int)(width * uv[0]);
    auto y = (int)(height * uv[1]);
    return pConditional[y][x] * pConditional[y].sum / pMarginal.sum;
  }

public:
  Distribution1D pMarginal;
  std::vector<Distribution1D> pConditional;
};

}
