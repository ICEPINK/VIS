#include "image.hpp"

namespace Vis {
Image::Image() {}
Image::Image(const size_t width, const size_t height)
    : m_width(width), m_height(height) {
    m_buffer.resize(width * height);
}

auto Image::resize(const size_t width, const size_t height) -> void {
    m_width = width;
    m_height = height;
    m_buffer.resize(width * height);
}

auto Image::clear(const glm::dvec4 &color) -> void {
    for (auto &pixel : m_buffer) {
        pixel = dvec4_to_rgba8(color);
    }
}

auto Image::set_pixel(const size_t x, const size_t y, const glm::dvec4 &color)
    -> void {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
        return;
    }
    m_buffer[x + y * m_width] = dvec4_to_rgba8(color);
}

[[nodiscard]] auto Image::get_width() const -> size_t { return m_width; }
[[nodiscard]] auto Image::get_height() const -> size_t { return m_height; }
[[nodiscard]] auto Image::get_image_data() -> ColorRGBA8 * {
    return m_buffer.data();
}
[[nodiscard]] auto Image::get_pixel(const size_t x, const size_t y) const
    -> glm::dvec4 {
    return rgba8_to_dvec4(m_buffer[x + y * m_width]);
}

auto Image::dvec4_to_rgba8(const glm::dvec4 &color) const -> ColorRGBA8 {
    ColorRGBA8 ret_color{static_cast<uint8_t>(color.r * 255.999),
                         static_cast<uint8_t>(color.g * 255.999),
                         static_cast<uint8_t>(color.b * 255.999),
                         static_cast<uint8_t>(color.a * 255.999)};
    return ret_color;
}

auto Image::rgba8_to_dvec4(const ColorRGBA8 &color) const -> glm::dvec4 {
    glm::dvec4 ret_color{color.r / 255.999, color.g / 255.999,
                         color.b / 255.999, color.a / 255.999};
    return ret_color;
}

} // namespace Vis
