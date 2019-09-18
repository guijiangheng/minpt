#include <iostream>
#include <minpt/math/vector.h>

using namespace std;
using namespace minpt;

int main() {
  auto c = Vector3f(0, 1, 2);
  cout << c.toString() << endl;
  return 0;
}
