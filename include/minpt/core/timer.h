#pragma once

#include <chrono>
#include <minpt/utils/utils.h>

namespace minpt {

class Timer {
public:
  Timer() noexcept {
    reset();
  }

  void reset() {
    start = std::chrono::system_clock::now();
  }

  /// Return the number of milliseconds
  double elapsed() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    return (double)duration.count();
  }

  /// Like \ref elapsed(), but return a human-readable string
  std::string elapsedString(bool precise = false) const {
    return timeString(elapsed(), precise);
  }

  /// Return the number of milliseconds elapsed since the timer was last reset and then reset it
  double lap() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
    start = now;
    return (double)duration.count();
  }

  /// Link \ref lap(), but return a human-readable string
  std::string lapString(bool precise = false) {
    return timeString(lap(), precise);
  }

private:
  std::chrono::system_clock::time_point start;
};

}
