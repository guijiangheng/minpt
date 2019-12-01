#include <minpt/core/bsdf.h>

namespace minpt {

class Mirror : public BSDF {
public:
  Mirror(const PropertyList& props) : kr(props.getColor3f("kr", Color3f(1.0f)))
  { }

  bool isDelta() const override {
    return true;
  }

  Color3f f(const Vector3f& wo, const Vector3f& wi) const override {
    return Color3f(0.0f);
  }

  float pdf(const Vector3f& wo, const Vector3f& wi) const override {
    return 0;
  }

  Color3f sample(
      const Vector2f& u,
      const Vector3f& wo,
      Vector3f& wi,
      float& pdf,
      float& etaScale) const override {

    etaScale = 1.0f;
    pdf = 1.0f;
    wi = Vector3f(-wo.x, -wo.y, wo.z);
    return kr;
  }

  std::string toString() const override {
    return tfm::format("Mirror[kr = %s]", kr.toString());
  }

public:
  Color3f kr;
};

}
