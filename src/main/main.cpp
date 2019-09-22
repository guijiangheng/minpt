#include <iostream>
#include <minpt/samplers/random.h>

using namespace std;
using namespace minpt;

int main() {
  RandomSampler sampler(512);
  std::cout << sampler.toString() << std::endl;
  return 0;
}
