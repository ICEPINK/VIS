#pragma once
////////////////////////////////////////////////////////////////////////////////
#include <chrono>
////////////////////////////////////////////////////////////////////////////////
namespace Vis {
////////////////////////////////////////////////////////////////////////////////
class Timer {
  public:
    Timer() : m_start_time(std::chrono::high_resolution_clock::now()) {}

    ~Timer() {}

    void reset() { m_start_time = std::chrono::high_resolution_clock::now(); }

    auto get_start() { return m_start_time; }

    auto get_now() { return std::chrono::high_resolution_clock::now(); }

    auto get_time() {
        return std::chrono::high_resolution_clock::now() - m_start_time;
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
};
////////////////////////////////////////////////////////////////////////////////
}; // namespace Vis
