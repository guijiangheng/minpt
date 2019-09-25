#include <iostream>
#include <minpt/math/matrix.h>

using namespace minpt;

int main() {
  Matrix4f m;
  m << 1, 2, 3, 4,
       2, 3, 4, 5,
       4, 5, 6, 7,
       7, 8, 9, 10;
  m.setIdentity();
  m = Matrix4f::perspective(30, 0.01f, 1000.0f);
  std::cout << Matrix4f::Identity().toString() << std::endl;
  std::cout << m.toString() << std::endl;
  return 0;
}
