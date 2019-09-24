#include <iostream>
#include <minpt/core/film.h>
#include <minpt/math/transform.h>

using namespace std;
using namespace minpt;

int main() {
  Transform t;
  std::cout << t.toString() << std::endl;

  Film film(Vector2i(512, 512));
  std::cout << film.toString() << std::endl;

  Bounds2i b;
  std::cout << b.toString() << std::endl;

  return 0;
}
