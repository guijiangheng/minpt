#include <minpt/core/bsdf.h>

namespace minpt {

class Mirror : public BSDF {
public:
  Mirror(const PropertyList& props) : kr(props.getColor3f("kr", Color3f(1.0f)))
  { }

  bool isDelta() const override {
    return true;
  }

  Color3f f(const BSDFQueryRecord& bRec) const override {
    return Color3f(0.0f);
  }

  float pdf(const BSDFQueryRecord& bRec) const override {
    return 0;
  }

  Color3f sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override {
    bRec.isDelta = true;
    pdf = 1.0f;
    bRec.wi = Vector3f(-bRec.wo.x, -bRec.wo.y, bRec.wo.z);
    return kr;
  }

  std::string toString() const override {
    return tfm::format("Mirror[kr = %s]", kr.toString());
  }

public:
  Color3f kr;
};

}
