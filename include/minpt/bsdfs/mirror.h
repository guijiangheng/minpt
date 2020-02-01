#pragma once

#include <minpt/core/bsdf.h>

namespace minpt {

class Mirror : public BSDF {
public:
  Mirror(const PropertyList& props) : kr(props.getRGBSpectrum("kr", Spectrum(1.0f)))
  { }

  bool isDelta() const override {
    return true;
  }

  Spectrum f(const BSDFQueryRecord& bRec) const override {
    return Spectrum(0.0f);
  }

  float pdf(const BSDFQueryRecord& bRec) const override {
    return 0;
  }

  Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override {
    pdf = 1.0f;
    bRec.wi = Vector3f(-bRec.wo.x, -bRec.wo.y, bRec.wo.z);
    return kr;
  }

  std::string toString() const override {
    return tfm::format("Mirror[kr = %s]", kr.toString());
  }

public:
  Spectrum kr;
};

}
