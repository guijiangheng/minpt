#include <iostream>
#include <minpt/math/bounds.h>

using namespace std;
using namespace minpt;

int main() {
  Bounds3f b(Vector3f(-1), Vector3f(1));
  Ray3f ray(Vector3f(0, 0, 2), Vector3f(0, 0, 1));
  Vector3f invDir(1 / 0.0f, 1 / 0.0f, 1 / 1.0f);
  int dirIsNeg[3] = { 0, 0, 0 };
  auto intersect = b.intersect(ray, invDir, dirIsNeg);
  std::cout << intersect << std::endl;
  return 0;
}
