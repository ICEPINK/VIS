#pragma once

#include <vector>

namespace Vis {

class DepthBuffer {
  public:
    DepthBuffer(const size_t width, const size_t height);

    void clear(const float depth);

    void set_depth(const size_t x, const size_t y, const float depth);

    size_t get_width() const;
    size_t get_height() const;
    float get_depth(const size_t x, const size_t y) const;

  private:
    const size_t m_width;
    const size_t m_height;
    std::vector<float> m_depth_buffer;
};

} // namespace Vis
