#pragma once

#include <map>

namespace minpt {

class Property;

class PropertyList {
public:
  PropertyList() = default;

  std::map<std::string, Property> props;
};

}
