#include <iostream>
#include <minpt/core/ray.h>

using namespace std;
using namespace minpt;

int main() {
  Ray3f ray(Vector3f(3), Vector3f(1, 0, 0));
  std::cout << ray.toString() << std::endl;
  return 0;
}
