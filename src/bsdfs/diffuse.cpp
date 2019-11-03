#include <minpt/utils/utils.h>
#include <minpt/math/math.h>
#include <minpt/core/bsdf.h>
#include <minpt/core/sampling.h>

namespace minpt {

class Diffuse : public BSDF {
public:
  Diffuse(const PropertyList& props)
    : albedo(props.getColor3f("albedo", Color3f(0.0f)))
  { }

  Color3f f(const Vector3f& wo, const Vector3f& wi) const override {
    if (!sameHemisphere(wo, wi))
      return Color3f(0);
    return albedo * InvPi;
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    return sameHemisphere(wo, wi) ? absCosTheta(wi) * InvPi : 0;
  }

  Color3f sample(Vector2f& u, const Vector3f& wo, Vector3f& wi, float& pdf) const override {
    wi = cosineSampleHemisphere(u);
    pdf = wi.z * InvPi;
    if (wo.z < 0) wi.z = -wi.z;
    return albedo * InvPi;
  }

  std::string toString() const {
    return tfm::format(
      "Diffuse[\n"
      "  albedo = %s\n"
      "]",
      indent(albedo.toString())
    );
  }

private:
  Color3f albedo;
};

MINPT_REGISTER_CLASS(Diffuse, "diffuse");

}
