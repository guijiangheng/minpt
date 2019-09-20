#include <sstream>
#include <minpt/common/common.h>

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

}
