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
