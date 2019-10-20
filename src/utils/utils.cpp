#include <sstream>
#include <iomanip>
#include <minpt/utils/utils.h>

namespace minpt {

std::string indent(const std::string& string, int amount) {
  std::istringstream iss(string);
  std::ostringstream oss;
  std::string spacer(amount, ' ');
  auto firstLine = true;
  for (std::string line; std::getline(iss, line);) {
    if (!firstLine)
      oss << spacer;
    oss << line;
    if (!iss.eof())
      oss << std::endl;
    firstLine = false;
  }
  return oss.str();
}

std::string toLower(const std::string& value) {
  std::string result;
  result.resize(value.size());
  std::transform(value.begin(), value.end(), result.begin(), ::tolower);
  return result;
}

bool toBool(const std::string& string) {
  auto value = toLower(string);
  if (value == "false")
    return false;
  if (value == "true")
    return true;
  throw Exception("Could not parse boolean value \"%s\"", string);
}

int toInt(const std::string& string) {
  char* endPtr = nullptr;
  auto result = (int)strtol(string.c_str(), &endPtr, 10);
  if (*endPtr != '\0')
    throw Exception("Could not parse integer value \"%s\"", string);
  return result;
}

float toFloat(const std::string& string) {
  char* end = nullptr;
  auto result = (float)strtof(string.c_str(), &end);
  if (*end != '\0')
    throw Exception("Could not parse integer value \"%s\"", string);
  return result;
}

unsigned int toUInt(const std::string& string) {
  char* endPtr = nullptr;
  auto result = (unsigned int)strtoul(string.c_str(), &endPtr, 10);
  if (*endPtr != '\0')
    throw Exception("Could not parse integer value \"%s\"", string);
  return result;
}

Vector2i toVector2i(const std::string& string) {
  auto tokens = tokenize(string);
  if (tokens.size() != 2)
    throw Exception("Expected 2 values");
  return Vector2i(toInt(tokens[0]), toInt(tokens[1]));
}

Vector2f toVector2f(const std::string& string) {
  auto tokens = tokenize(string);
  if (tokens.size() != 2)
    throw Exception("Expected 2 values");
  return Vector2f(toFloat(tokens[0]), toFloat(tokens[1]));
}

Vector3f toVector3f(const std::string& string) {
  auto tokens = tokenize(string);
  if (tokens.size() != 3)
    throw Exception("Expected 3 values");
  return Vector3f(toFloat(tokens[0]), toFloat(tokens[1]), toFloat(tokens[2]));
}

Color3f toColor3f(const std::string& string) {
  auto tokens = tokenize(string);
  if (tokens.size() != 3)
    throw Exception("Expected 3 values");
  return Color3f(toFloat(tokens[0]), toFloat(tokens[1]), toFloat(tokens[2]));
}

bool endsWith(const std::string& value, const std::string& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string timeString(double time, bool precise) {
  if (std::isnan(time) || std::isinf(time)) return "inf";
  auto suffix = "ms";
  if (time > 1000) {
    time /= 1000; suffix = "s";
    if (time > 60) {
      time /= 60; suffix = "m";
      if (time > 60) {
        time /= 60; suffix = "h";
        if (time > 12) {
          time /= 12; suffix = "d";
        }
      }
    }
  }
  std::ostringstream os;
  os << std::setprecision(precise ? 4 : 1)
     << std::fixed << time << suffix;
  return os.str();
}

std::string memString(size_t size, bool precise) {
  auto value = (double)size;
  const char* suffixes[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB" };
  auto suffix = 0;
  while (suffix < 5 && value > 1024.0f) {
    value /= 1024.0f;
    ++suffix;
  }
  std::ostringstream os;
  os << std::setprecision(suffix == 0 ? 0 : (precise ? 4 : 1))
     << std::fixed << value << " " << suffixes[suffix];
  return os.str();
}

std::vector<std::string> tokenize(const std::string& string, const std::string& delim, bool includeEmpty) {
  std::vector<std::string> tokens;
  std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
  while (lastPos != std::string::npos) {
    if (pos != lastPos || includeEmpty)
      tokens.push_back(string.substr(lastPos, pos - lastPos));
    lastPos = pos;
    if (lastPos != std::string::npos) {
      lastPos += 1;
      pos = string.find_first_of(delim, lastPos);
    }
  }
  return tokens;
}

filesystem::resolver* getFileResolver() {
  static auto resolver = filesystem::resolver();
  return &resolver;
}

}
