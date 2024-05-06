#include "depth_buffer.hpp"

namespace Vis {

DepthBuffer::DepthBuffer(size_t width, size_t height)
    : m_width(width), m_height(height), m_depth_buffer(width * height) {}

void DepthBuffer::clear(const float t_depth) {
  for (auto &depth : m_depth_buffer) {
    depth = t_depth;
  }
}

void DepthBuffer::set_depth(const size_t x, const size_t y, const float depth) {
  m_depth_buffer[x + y * m_width] = depth;
}

size_t DepthBuffer::get_width() const { return m_width; }

size_t DepthBuffer::get_height() const { return m_height; }

float DepthBuffer::get_depth(const size_t x, const size_t y) const {
  return m_depth_buffer[x + y * m_width];
}

} // namespace Vis
