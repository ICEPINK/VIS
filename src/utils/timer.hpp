#pragma once

#include <chrono>

namespace Vis {

class Timer {
  public:
    Timer() { m_start = std::chrono::high_resolution_clock::now(); }

    Timer(double *dtor_return) : p_dtor_return{dtor_return} {
        m_start = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        if (p_dtor_return) {
            *p_dtor_return = duration();
        }
    }

    [[nodiscard]] auto now()
        -> decltype(std::chrono::high_resolution_clock::now()) {
        return std::chrono::high_resolution_clock::now();
    }
    auto reset() -> void {
        m_start = std::chrono::high_resolution_clock::now();
    }
    [[nodiscard]] auto duration() -> double {
        const auto end{std::chrono::high_resolution_clock::now()};
        std::chrono::duration<double> duration{end - m_start};
        return duration.count();
    }

  private:
    double *p_dtor_return{nullptr};
    decltype(std::chrono::high_resolution_clock::now()) m_start;
};

} // namespace Vis
