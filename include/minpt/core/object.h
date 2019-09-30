#pragma once

#include <string>

namespace minpt {

class Object {
protected:
  virtual ~Object() = default;

public:
  virtual std::string toString() const = 0;
};

}
