#include <iostream>
#include <minpt/cameras/perspective.h>

using namespace minpt;

int main() {
  PerspectiveCamera camera(
    Matrix4f::identity(),
    Vector2i(512),
    30.0f
  );
  std::cout << camera.toString() << std::endl;
  return 0;
}
