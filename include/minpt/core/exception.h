#pragma once

#include <stdexcept>
#include <tinyformat.h>

namespace minpt {

class Exception : public std::runtime_error {
public:
  template <typename... Args>
  Exception(const char* format, const Args&... args)
    : std::runtime_error(tfm::format(format, args...))
  { }
};

}
