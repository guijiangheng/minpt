#pragma once

#include <string>
#include <vector>
#include <filesystem/resolver.h>
#include <minpt/core/exception.h>
#include <minpt/math/math.h>

namespace minpt {

std::string indent(const std::string& string, int amount = 2);

inline int toInt(const std::string& string) {
  char* endPtr = nullptr;
  auto result = (int)strtol(string.c_str(), &endPtr, 10);
  if (*endPtr != '\0')
    throw Exception("Could not parse integer value \"%s\"", string);
  return result;
}

inline int toUInt(const std::string& string) {
  char* endPtr = nullptr;
  auto result = (unsigned int)strtoul(string.c_str(), &endPtr, 10);
  if (*endPtr != '\0')
    throw Exception("Could not parse integer value \"%s\"", string);
  return result;
}

inline std::string toLower(const std::string& value) {
  std::string result;
  result.resize(value.size());
  std::transform(value.begin(), value.end(), result.begin(), ::tolower);
  return result;
}

inline bool endsWith(const std::string& value, const std::string& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string timeString(double time, bool precise = false);

filesystem::resolver* getFileResolver();

std::vector<std::string> tokenize(const std::string& string, const std::string& delim = ", ", bool includeEmpty = false);

}
