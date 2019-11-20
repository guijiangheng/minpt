#include <minpt/lights/area.h>

namespace minpt {

Color3f Interaction::le(const Vector3f& w) const {
  if (mesh->light)
    return mesh->light->le(*this);
  return Color3f(0.0f);
}

}
