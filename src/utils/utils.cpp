#include <sstream>
#include <minpt/utils/utils.h>

namespace minpt {

std::string indent(const std::string& string, int amount) {
  std::istringstream iss(string);
  std::ostringstream oss;
  std::string spacer(amount, ' ');
  std::string line;
  std::getline(iss, line);
  oss << line << std::endl;
  while (std::getline(iss, line)) {
    oss << spacer << line;
    if (!iss.eof()) oss << std::endl;
  }
  return oss.str();
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
  static filesystem::resolver *resolver = new filesystem::resolver();
  return resolver;
}

}
