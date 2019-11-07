#include <minpt/core/filter.h>

namespace minpt {

class GaussianFilter : public Filter {
public:
  GaussianFilter(const PropertyList& props) {
    radius = props.getFloat("radius", 2.0f);
    stddev = props.getFloat("stddev", 0.5f);
    alpha = -0.5f / (stddev * stddev);
    radiusSquared = radius * radius;
  }

  float eval(float x) const override {
    return std::max(0.0f, std::exp(alpha * x * x) - std::exp(alpha * radiusSquared));
  }

  std::string toString() const override {
    return tfm::format("GaussianFilter[radius=%f, stddev=%f]", radius, stddev);
  }

private:
  float stddev;
  float alpha;
  float radiusSquared;
};

MINPT_REGISTER_CLASS(GaussianFilter, "gaussian");

}
