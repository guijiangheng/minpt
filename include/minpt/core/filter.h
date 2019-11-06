#pragma once

#include <minpt/core/object.h>

namespace minpt {

class Filter : public Object {
public:
   virtual float eval(float x) const = 0;

   EClassType getClassType() const override {
     return EFilter;
   }

public:
  float radius;
};

}
