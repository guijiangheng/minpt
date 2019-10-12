#include <iostream>
#include <minpt/minpt.h>

using namespace minpt;

int main() {
  PropertyList props;
  props.setBoolean("active", true);
  props.setInteger("age", 23);
  props.setFloat("alpha", 0.111);
  props.setString("name", "guijiangheng");
  props.setColor("albedo", Color3f(1.0f));
  props.setVector("position", Vector3f(1.0f, 2.0f, 3.0f));
  props.setTransform("transform", Matrix4f::perspective(30.0f, 0.0001f, 1000.0f));
  std::cout
    << props.getBoolean("active") << " "
    << props.getInteger("age") << " "
    << props.getFloat("alpha") << " "
    << props.getString("name") << " "
    << std::endl;
  std::cout << props.getColor("albedo").toString() << std::endl;
  std::cout << props.getVector("position").toString() << std::endl;
  std::cout << props.getTransform("transform").toString() << std::endl;

  return 0;
}
