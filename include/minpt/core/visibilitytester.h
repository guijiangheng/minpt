#pragma once

#include <minpt/core/interaction.h>

namespace minpt {

class Scene;

class VisibilityTester {
public:
  VisibilityTester() = default;

  VisibilityTester(const Interaction& ref, const Vector3f& target) noexcept
    : ref(&ref), target(target)
  { }

  bool occluded(const Scene& scene) const;

private:
  const Interaction* ref;
  Vector3f target;
};

}
