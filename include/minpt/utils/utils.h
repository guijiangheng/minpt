#pragma once

#include <string>
#include <vector>
#include <filesystem/resolver.h>
#include <minpt/core/exception.h>
#include <minpt/math/math.h>
#include <minpt/math/color.h>

namespace minpt {

std::string indent(const std::string& string, int amount = 2);

std::vector<std::string> tokenize(const std::string& string, const std::string& delim = ", ", bool includeEmpty = false);

inline std::string toLower(const std::string& value) {
  std::string result;
  result.resize(value.size());
  std::transform(value.begin(), value.end(), result.begin(), ::tolower);
  return result;
}

inline bool toBool(const std::string& string) {
  auto value = toLower(string);
  if (value == "false")
    return false;
  if (value == "true")
    return true;
  throw Exception("Could not parse boolean value \"%s\"", string);
}

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

inline float toFloat(const std::string& string) {
  char* end = nullptr;
  auto result = (float)strtof(string.c_str(), &end);
  if (*end != '\0')
    throw Exception("Could not parse integer value \"%s\"", string);
  return result;
}

inline Vector3f toVector3f(const std::string& string) {
  auto tokens = tokenize(string);
  if (tokens.size() != 3)
    throw Exception("Expected 3 values");
  return Vector3f(toFloat(tokens[0]), toFloat(tokens[1]), toFloat(tokens[2]));
}

inline Color3f toColor3f(const std::string& string) {
  auto tokens = tokenize(string);
  if (tokens.size() != 3)
    throw Exception("Expected 3 values");
  return Color3f(toFloat(tokens[0]), toFloat(tokens[1]), toFloat(tokens[2]));
}

inline bool endsWith(const std::string& value, const std::string& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string timeString(double time, bool precise = false);

filesystem::resolver* getFileResolver();

}
