#include <iostream>
#include <minpt/math/frame.h>

using namespace std;
using namespace minpt;

int main() {
  Vector3f v = Eigen::Vector3f::Random();
  std::cout << v.toString() << std::endl;
  Frame f(v);
  std::cout << f.toString() << std::endl;
  return 0;
}
