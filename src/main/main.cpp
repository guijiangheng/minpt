#include <iostream>
#include <minpt/core/bitmap.h>

using namespace std;
using namespace minpt;

int main() {
  Bitmap bitmap("./dragon.exr");
  bitmap.save("./dragon-test.exr");
  return 0;
}
