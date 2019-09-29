#include <iostream>
#include <minpt/integrators/normals.h>
#include <minpt/samplers/random.h>
#include <minpt/accelerators/bvh.h>
#include <minpt/cameras/perspective.h>
#include <minpt/utils/obj.h>

using namespace minpt;

int main() {
  auto sampler = new RandomSampler(4);
  auto accel = new BVHAccel();
  auto integrator = new NormalIntegrator();
  auto camera = new PerspectiveCamera(
    Matrix4f::lookAt(
      Vector3f(-0.0315182, 0.284011, 0.7331),
      Vector3f(-0.0123771, 0.0540913, -0.239922),
      Vector3f(0.00717446, 0.973206, -0.229822)
    ),
    Vector2i(512),
    16.0f
  );
  Scene scene(camera, sampler, integrator, accel);
  scene.addMesh(new WavefrontOBJ("bunny.obj"));
  scene.activate();
  scene.render("bunny.exr");
  return 0;
}
