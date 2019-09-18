#pragma once

#include <string>

namespace minpt {

inline std::string toLower(const std::string& value) {
  std::string ret;
  ret.resize(value.size());
  std::transform(value.begin(), value.end(), ret.begin(), ::tolower);
  return ret;
}

inline bool endsWith(const std::string& value, const std::string& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

}
