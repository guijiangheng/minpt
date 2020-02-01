#pragma once

#include <minpt/core/bsdf.h>

namespace minpt {

class Glass : public BSDF {
public:
  Glass(const PropertyList& props)
    : kr(props.getRGBSpectrum("kr", Spectrum(1.0f)))
    , kt(props.getRGBSpectrum("kt", Spectrum(1.0f)))
    , intIOR(props.getFloat("intIOR", 1.5046f))
    , extIOR(props.getFloat("extIOR", 1.000277f)) {
      eta = intIOR / extIOR;
  }

  bool isDelta() const override {
    return true;
  }

  Spectrum f(const BSDFQueryRecord& bRec) const override {
    return Spectrum(0.0f);
  }

  float pdf(const BSDFQueryRecord& bRec) const override {
    return 0;
  }

  Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override;

  std::string toString() const override {
    return tfm::format(
      "Glass[\n"
      "  kr = %s,\n"
      "  kt = %s,\n"
      "  intIOR = %f,\n"
      "  extIOR = %f\n"
      "]",
      kr.toString(), kt.toString(), intIOR, extIOR
    );
  }

public:
  Spectrum kr, kt;
  float eta;
  float intIOR;
  float extIOR;
};

}
