#include <iostream>
#include <minpt/math/bounds.h>

using namespace std;
using namespace minpt;

int main() {
  Eigen::Matrix3f m;
  m << 1, 2, 3,
       2, 3, 4,
       3, 4, 5;
  Vector3f v(1.0f);
  v = m.col(1);
  std::cout << v.toString() << std::endl;

  return 0;
}
