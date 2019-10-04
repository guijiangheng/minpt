#include <iostream>
#include <minpt/minpt.h>

using namespace minpt;

int main() {
  auto sampler = new RandomSampler(1);
  auto accel = new BVHAccel();
  auto integrator = new NormalIntegrator();
  auto camera = new PerspectiveCamera(
    Matrix4f::lookAt(
      Vector3f(-0.0315182, 0.284011, 0.7331),
      Vector3f(-0.0123771, 0.0540913, -0.239922),
      Vector3f(0.00717446, 0.973206, -0.229822)
    ),
    Vector2i(768, 768),
    16.0f
  );
  Scene scene(camera, sampler, integrator, accel);
  scene.addMesh(new WavefrontOBJ("bunny.obj"));
  scene.activate();
  std::cout << scene.toString() << std::endl;
  scene.render("bunny.exr");
  return 0;
}
