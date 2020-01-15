#pragma once

#include <minpt/core/object.h>

namespace minpt {

enum class TransformType {
  Local,
  Global
};

struct Options {
  std::string outfile;
  std::string filename;
  TransformType transformType;
};

Object* loadFromXML(const Options& options);

}
