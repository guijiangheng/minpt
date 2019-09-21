#include <iostream>
#include <minpt/utils/obj.h>

using namespace std;
using namespace minpt;

int main() {
  WavefrontOBJ obj("./sphere.obj");
  std::cout << obj.toString() << std::endl;
  return 0;
}
