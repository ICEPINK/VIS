#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>

namespace Vis {

struct ColorRGBA8 {
  uint8_t r{0};
  uint8_t g{0};
  uint8_t b{0};
  uint8_t a{255};
};

class Image {
public:
  Image();
  Image(const size_t width, const size_t height);
  ~Image() = default;

  auto resize(const size_t width, const size_t height) -> void;

  auto clear(const glm::dvec4 &color = {0.0, 0.0, 0.0, 1.0},
             const double depth = 1.0) -> void;

  auto set_pixel(const size_t x, const size_t y,
                 const glm::dvec4 &color) -> void;
  auto set_depth(const size_t x, const size_t y, const double depth) -> void;

  [[nodiscard]] auto get_width() const -> size_t;
  [[nodiscard]] auto get_height() const -> size_t;
  [[nodiscard]] auto get_image_data() -> ColorRGBA8 *;
  [[nodiscard]] auto get_pixel(const size_t x,
                               const size_t y) const -> glm::dvec4;
  [[nodiscard]] auto get_depth(const size_t x, const size_t y) const -> double;

private:
  auto dvec4_to_rgba8(const glm::dvec4 &color) const -> ColorRGBA8;

  auto rgba8_to_dvec4(const ColorRGBA8 &color) const -> glm::dvec4;

private:
  size_t m_width{0};
  size_t m_height{0};
  std::vector<ColorRGBA8> m_color_buffer;
  std::vector<double> m_depth_buffer;
};

} // namespace Vis
