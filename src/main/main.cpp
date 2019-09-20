#include <iostream>
#include <Eigen/Geometry>
#include <minpt/math/color.h>
#include <minpt/math/vector.h>

using namespace std;
using namespace minpt;

int main() {
  Vector3f a(1, 2, 3);
  Vector3f b(2, 3, 4);
  Vector3f c = a.cross(b);
  std::cout << c.toString() << std::endl;
  return 0;
}
