#include <iostream>
#include <minpt/math/math.h>

using namespace std;
using namespace minpt;

int main() {
  Bounds3f b(Vector3f(3), Vector3f(4));
  std::cout << b.toString() << std::endl;
  return 0;
}
