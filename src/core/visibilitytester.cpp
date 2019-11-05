#include <minpt/core/scene.h>
#include <minpt/core/visibilitytester.h>

namespace minpt {

bool VisibilityTester::occluded(const Scene& scene) const {
  return scene.intersect(ref->spawnRayTo(target));
}

}
