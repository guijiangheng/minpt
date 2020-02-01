#pragma once

#include <vector>
#include <fstream>
#include <algorithm>

#include <minpt/math/math.h>
#include <minpt/core/exception.h>

namespace minpt {

static constexpr int    CIE_samples = 471;
extern const float      CIE_lambda[CIE_samples];
extern const float      CIE_X[CIE_samples];
extern const float      CIE_Y[CIE_samples];
extern const float      CIE_Z[CIE_samples];
static constexpr float  CIE_Y_integral = 106.856895f;

inline float gammaCorrect(float v) {
  if (v <= 0.0031308f) return 12.92f * v;
  return 1.055f * std::pow(v, (1.f / 2.4f)) - 0.055f;
}

inline float inverseGammaCorrect(float v) {
  if (v <= 0.04045f) return v / 12.92f;
  return std::pow((v + 0.055f) / 1.055f, 2.4f);
}

inline void XYZToRGB(const float xyz[3], float rgb[3]) {
  rgb[0] =  3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
  rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
  rgb[2] =  0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}

inline void RGBToXYZ(const float rgb[3], float xyz[3]) {
  xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
  xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
  xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

class InterpolatedSpectrum {
public:
  InterpolatedSpectrum(const std::string& filename);

  InterpolatedSpectrum(const float* lambdas, const float* vals, int n);

  float eval(float lambda) const;

  float average(float lambdaStart, float lambdaEnd) const;

private:
  bool isSorted() const;

  InterpolatedSpectrum& sort();

private:
  std::vector<float> lambdas, vals;
};

template <int nSpectrumSamples>
class CoefficientSpectrum {
public:
  using Scalar = float;

  CoefficientSpectrum() = default;

  explicit CoefficientSpectrum(float v) {
    for (auto i = 0; i < nSpectrumSamples; ++i) c[i] = v;
  }

  CoefficientSpectrum operator-() const {
    CoefficientSpectrum ret;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] = -c[i];
    return ret;
  }

  CoefficientSpectrum operator+(const CoefficientSpectrum& s) const {
    auto ret = *this;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] += s[i];
    return ret;
  }

  CoefficientSpectrum& operator+=(const CoefficientSpectrum& s) {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      c[i] += s[i];
    return *this;
  }

  CoefficientSpectrum operator-(const CoefficientSpectrum& s) const {
    auto ret = *this;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] -= s[i];
    return ret;
  }

  CoefficientSpectrum& operator-=(const CoefficientSpectrum& s) {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      c[i] -= s[i];
    return *this;
  }

  CoefficientSpectrum operator*(const CoefficientSpectrum& s) const {
    auto ret = *this;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] *= s[i];
    return ret;
  }

  CoefficientSpectrum& operator*=(const CoefficientSpectrum& s) {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      c[i] *= s[i];
    return *this;
  }

  CoefficientSpectrum operator*(float k) const {
    auto ret = *this;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] *= k;
    return ret;
  }

  CoefficientSpectrum& operator*=(float k) {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      c[i] *= k;
    return *this;
  }

  CoefficientSpectrum operator/(const CoefficientSpectrum& s) const {
    auto ret = *this;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] /= s[i];
    return ret;
  }

  CoefficientSpectrum& operator/(const CoefficientSpectrum& s) {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      c[i] /= s[i];
    return *this;
  }

  CoefficientSpectrum operator/(float k) const {
    auto ret = *this;
    auto invK = 1.0f / k;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      ret[i] *= invK;
    return ret;
  }

  CoefficientSpectrum& operator/=(float k) {
    auto invK = 1.0f / k;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      c[i] *= invK;
    return *this;
  }

  float maxComponent() const {
    auto value = c[0];
    for (auto i = 1; i < nSpectrumSamples; ++i)
      value = std::max(value, c[i]);
    return value;
  }

  float& operator[](int index) {
    return c[index];
  }

  float operator[](int index) const {
    return c[index];
  }

  bool isBlack() const {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      if (c[i] != 0.0f) return false;
    return true;
  }

  bool isValid() const {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      if (c[i] < 0 || !std::isfinite(c[i]))
        return false;
    return true;
  }

  bool operator==(const CoefficientSpectrum& s) const {
    for (auto i = 0; i < nSpectrumSamples; ++i)
      if (c[i] != s[i]) return false;
    return true;
  }

  bool operator!=(const CoefficientSpectrum& s) const {
    return !(*this == s);
  }

  std::string toString() const {
    std::string ret = "[";
    for (auto i = 0; i < nSpectrumSamples; ++i) {
      ret += tfm::format("%f", c[i]);
      if (i + 1 < nSpectrumSamples) ret += ", ";
    }
    ret += "]";
    return ret;
  }

protected:
  float c[nSpectrumSamples];
};

template <int N>
CoefficientSpectrum<N> sqrt(const CoefficientSpectrum<N>& s) {
  CoefficientSpectrum<N> ret;
  for (auto i = 0; i < N; ++i)
    ret[i] = std::sqrt(s[i]);
  return ret;
}

template <int N>
CoefficientSpectrum<N> safe_sqrt(const CoefficientSpectrum<N>& s) {
  CoefficientSpectrum<N> ret;
  for (auto i = 0; i < N; ++i)
    ret[i] = safe_sqrt(s[i]);
  return ret;
}

template <int N>
CoefficientSpectrum<N> exp(const CoefficientSpectrum<N>& s) {
  CoefficientSpectrum<N> ret;
  for (auto i = 0; i < N; ++i)
    ret[i] = std::exp(s[i]);
  return ret;
}

template <int N>
CoefficientSpectrum<N> pow(const CoefficientSpectrum<N>& s, float e) {
  CoefficientSpectrum<N> ret;
  for (auto i = 0; i < N; ++i)
    ret[i] = std::pow(s[i], e);
}

static constexpr int nSpectrumSamples       = 60;
static constexpr float SampledLambdaStart   = 400;
static constexpr float SampledLambdaEnd     = 700;

class SampledSpectrum : public CoefficientSpectrum<nSpectrumSamples> {
public:
  using Base = CoefficientSpectrum<nSpectrumSamples>;
  using Base::Base;

  static void initialize();

  SampledSpectrum(const Base& base) : Base(base)
  { }

  SampledSpectrum& fromSpectrum(const InterpolatedSpectrum& spectrum) {
    for (auto i = 0; i < nSpectrumSamples; ++i) {
      auto lambda0 = lerp(SampledLambdaStart, SampledLambdaEnd, i / (float)nSpectrumSamples);
      auto lambda1 = lerp(SampledLambdaStart, SampledLambdaEnd, (i + 1) / (float)nSpectrumSamples);
      c[i] = spectrum.average(lambda0, lambda1);
    }
    return *this;
  }

  float y() const {
    auto yy = 0.0f;
    for (auto i = 0; i < nSpectrumSamples; ++i)
      yy += Y[i] * c[i];
    return yy * (SampledLambdaEnd - SampledLambdaStart) / (CIE_Y_integral * nSpectrumSamples);
  }

  void toRGB(float rgb[3]) const {
    float xyz[3];
    toXYZ(xyz);
    XYZToRGB(xyz, rgb);
  }

  void toXYZ(float xyz[3]) const {
    xyz[0] = xyz[1] = xyz[2] = 0.0f;
    for (auto i = 0; i < nSpectrumSamples; ++i) {
      xyz[0] += X[i] * c[i];
      xyz[1] += Y[i] * c[i];
      xyz[2] += Z[i] * c[i];
    }
    auto scale = (SampledLambdaEnd - SampledLambdaStart) / (CIE_Y_integral * nSpectrumSamples);
    xyz[0] *= scale;
    xyz[1] *= scale;
    xyz[2] *= scale;
  }

private:
  static SampledSpectrum X, Y, Z;
};

class RGBSpectrum : public CoefficientSpectrum<3> {
public:
  using Base = CoefficientSpectrum<3>;
  using Base::Base;

  RGBSpectrum(const Base& base) : Base(base)
  { }

  RGBSpectrum(float r, float g, float b) {
    c[0] = r;
    c[1] = g;
    c[2] = b;
  }

  RGBSpectrum& fromSpectrum(const InterpolatedSpectrum& spectrum) {
    float xyz[3] = { 0.0f, 0.0f, 0.0f };
    for (auto i = 0; i < CIE_samples; ++i) {
      auto val = spectrum.eval(CIE_lambda[i]);
      xyz[0] += val * CIE_X[i];
      xyz[1] += val * CIE_Y[i];
      xyz[2] += val * CIE_Z[i];
    }
    auto scale = (CIE_lambda[CIE_samples - 1] - CIE_lambda[0]) /
                 (CIE_Y_integral * CIE_samples);
    xyz[0] *= scale;
    xyz[1] *= scale;
    xyz[2] *= scale;
    XYZToRGB(xyz, c);
    return *this;
  }

  void toXYZ(float xyz[3]) const {
    RGBToXYZ(c, xyz);
  }

  void toRGB(float rgb[3]) const {
    rgb[0] = c[0];
    rgb[1] = c[1];
    rgb[2] = c[2];
  }

  float y() const {
    return c[0] * 0.212671f + c[1] * 0.715160f + c[2] * 0.072169f;
  }

  RGBSpectrum gammaCorrect() const {
    return RGBSpectrum(
      minpt::gammaCorrect(c[0]),
      minpt::gammaCorrect(c[1]),
      minpt::gammaCorrect(c[2])
    );
  }

  RGBSpectrum& gammaCorrect() {
    c[0] = minpt::gammaCorrect(c[0]);
    c[1] = minpt::gammaCorrect(c[1]);
    c[2] = minpt::gammaCorrect(c[2]);
    return *this;
  }

  RGBSpectrum inverseGammaCorrect() const {
    return RGBSpectrum(
      minpt::inverseGammaCorrect(c[0]),
      minpt::inverseGammaCorrect(c[1]),
      minpt::inverseGammaCorrect(c[2])
    );
  }

  RGBSpectrum& inverseGammaCorrect() {
    c[0] = minpt::inverseGammaCorrect(c[0]);
    c[1] = minpt::inverseGammaCorrect(c[1]);
    c[2] = minpt::inverseGammaCorrect(c[2]);
    return *this;
  }
};

class Color4f : public CoefficientSpectrum<4> {
public:
  Color4f() noexcept {
    c[0] = 0.0f;
    c[1] = 0.0f;
    c[2] = 0.0f;
    c[3] = 1.0f;
  }

  Color4f(const RGBSpectrum& spectrum) noexcept {
    c[0] = spectrum[0];
    c[1] = spectrum[1];
    c[2] = spectrum[2];
    c[3] = 1.0f;
  }

  Color4f(float r, float g, float b, float w) noexcept {
    c[0] = r;
    c[1] = g;
    c[2] = b;
    c[3] = w;
  }

  RGBSpectrum eval() const {
    return RGBSpectrum(c[0], c[1], c[2]) / c[3];
  }
};

using Spectrum = RGBSpectrum;

}
