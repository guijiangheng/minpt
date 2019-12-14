#include <minpt/core/bsdf.h>

namespace minpt {

class Glass : public BSDF {
public:
  Glass(const PropertyList& props)
    : kr(props.getColor3f("kr", Color3f(1.0f)))
    , kt(props.getColor3f("kt", Color3f(1.0f)))
    , eta(props.getFloat("eta", 1.4f))
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

  Color3f sample(BSDFQueryRecord& bRec, const Vector2f& u, float& pdf) const override;

  std::string toString() const override {
    return tfm::format(
      "Glass[\n"
      "  kr = %s,\n"
      "  kt = %s,\n"
      "  eta = %f\n"
      "]",
      kr.toString(), kt.toString(), eta
    );
  }

public:
  Color3f kr, kt;
  float eta;
};

}
