#include <minpt/core/filter.h>

namespace minpt {

class BoxFilter : public Filter {
public:
  BoxFilter(const PropertyList& props) {
    radius = 0.5f;
  }

  float eval(float x) const override {
    return 1.0f;
  }

  std::string toString() const override {
    return "BoxFilter[]";
  }
};

MINPT_REGISTER_CLASS(BoxFilter, "box");

}
