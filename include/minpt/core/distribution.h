#pragma once

#include <vector>

namespace minpt {

class Distribution1D {
public:
  explicit Distribution1D(std::size_t nEntries = 0) {
    cdf.reserve(nEntries + 1);
    cdf.push_back(0.0f);
  }

  float operator[](size_t index) const {
    return cdf[index + 1] - cdf[index];
  }

  void append(float pdf) {
    cdf.push_back(cdf.back() + pdf);
  }

  float normalize() {
    auto sum = cdf.back();
    auto sumInv = 1 / sum;
    for (std::size_t i = 1, len = cdf.size() - 1; i < len; ++i)
      cdf[i] *= sumInv;
    cdf.back() = 1.0f;
    return sum;
  }

  std::size_t sample(float u) const {
    auto itr = std::lower_bound(cdf.begin(), cdf.end(), u);
    return itr - cdf.begin() - 1;
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

public:
  std::vector<float> cdf;
};

}
