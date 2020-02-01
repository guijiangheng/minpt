#pragma once

#include <minpt/utils/utils.h>
#include <minpt/core/bsdf.h>

namespace minpt {

class Metal : public BSDF {
public:
  Metal(const PropertyList& props)
      : filename(props.getString("filename", "Cu"))
      , kr(props.getRGBSpectrum("kr", Spectrum(1.0f))) {
    auto resolver = getFileResolver();
    eta.fromSpectrum(InterpolatedSpectrum(resolver->resolve(filename + ".eta.spd").str()));
    k.fromSpectrum(InterpolatedSpectrum(resolver->resolve(filename + ".k.spd").str()));
  }

  bool isDelta() const override {
    return true;
  }

  Spectrum f(const BSDFQueryRecord& bRec) const override {
    return Spectrum(0.0f);
  }

  float pdf(const BSDFQueryRecord& bRec) const override {
    return 0.0f;
  }

  Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override {
    bRec.wi = Vector3f(-bRec.wo.x, -bRec.wo.y, bRec.wo.z);
    return kr * frConductor(absCosTheta(bRec.wi), eta, k);
  }

  std::string toString() const override {
    return tfm::format(
      "Metal[\n"
      "  filename = \"%s\",\n"
      "  kr = %s,\n"
      "  eta = %s,\n"
      "  k = %s\n"
      "]",
      filename, kr.toString(), eta.toString(), k.toString()
    );
  }

private:
  std::string filename;
  Spectrum kr;
  Spectrum eta, k;
};

}
