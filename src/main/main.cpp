#include <iostream>
#include <minpt/cameras/perspective.h>

using namespace minpt;

int main() {
  Film film(Vector2i(512));
  PerspectiveCamera camera(
    Matrix4f::identity(),
    film,
    Bounds2f(Vector2f(-1.0f), Vector2f(1.0f)),
    30.0f
  );
  std::cout << camera.toString() << std::endl;
  return 0;
}
