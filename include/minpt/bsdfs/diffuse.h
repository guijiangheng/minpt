#pragma once

#include <minpt/utils/utils.h>
#include <minpt/math/math.h>
#include <minpt/core/bsdf.h>
#include <minpt/core/texture.h>
#include <minpt/core/sampling.h>

namespace minpt {

class Diffuse : public BSDF {
public:
  Diffuse(const PropertyList& _props) : albedo(nullptr) {
    if (_props.has("albedo")) {
      PropertyList props;
      props.setRGBSpectrum("value", _props.getRGBSpectrum("albedo"));
      albedo.reset(static_cast<Texture<Spectrum>*>(ObjectFactory::createInstance("constant_color", props)));
    }
  }

  void addChild(Object* object) override {
    switch (object->getClassType()) {
      case ETexture:
        if (object->getName() == "albedo") {
          if (albedo)
            throw Exception("There is already an albedo defined!");
          albedo.reset(static_cast<Texture<Spectrum>*>(object));
        } else {
          throw Exception("The name of this texture does not match any field!");
        }
        break;
      default:
        throw Exception(
          "Diffuse::addChild() is not implemented for objects of type '%s'!",
          classTypeName(object->getClassType())
        );
    }
  }

  void activate() override {
    if (!albedo) {
      PropertyList props;
      props.setRGBSpectrum("value", Spectrum(0.0f));
      albedo.reset(static_cast<Texture<Spectrum>*>(ObjectFactory::createInstance("constant_color", props)));
    }
  }

  Spectrum f(const BSDFQueryRecord& bRec) const override {
    if (!sameHemisphere(bRec.wo, bRec.wi))
      return Spectrum(0.0f);
    return albedo->eval(bRec.uv) * InvPi;
  }

  float pdf(const BSDFQueryRecord& bRec) const override {
    return sameHemisphere(bRec.wo, bRec.wi) ? absCosTheta(bRec.wi) * InvPi : 0;
  }

  Spectrum sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override {
    bRec.wi = cosineSampleHemisphere(u);
    pdf = bRec.wi.z * InvPi;
    if (bRec.wo.z < 0.0f) bRec.wi.z = -bRec.wi.z;
    return albedo->eval(bRec.uv);
  }

  std::string toString() const override {
    return tfm::format(
      "Diffuse[\n"
      "  albedo = %s\n"
      "]",
      indent(albedo->toString())
    );
  }

private:
  std::unique_ptr<Texture<Spectrum>> albedo;
};

}
