#include <iostream>
#include <minpt/math/bounds.h>

using namespace std;
using namespace minpt;

int main() {
  Bounds3f b(Vector3f(1, 2, 3), Vector3f(2, 3, 4));
  auto min = b.min();
  std::cout << min.x() << std::endl;
  std::cout << b.toString() << std::endl;
  std::cout << b.volume() << std::endl;
  std::cout << Vector3f(b.center()).toString() << std::endl;

  Bounds3f bbox = Eigen::AlignedBox<float, 3>(Vector3f(2, 3, 4), Vector3f(3, 4, 5));
  std::cout << bbox.toString() << std::endl;

  return 0;
}
