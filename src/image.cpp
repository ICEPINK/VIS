#include "image.hpp"

namespace Vis {

uint8_t cast_color_32f_to_8i(const float color) {
    return static_cast<uint8_t>(color * 255.999);
}

void cast_pixel_32f_to_8i(ColorRGBA8i &pixel, const ColorRGBA32f &color) {
    pixel.red = cast_color_32f_to_8i(color.r);
    pixel.green = cast_color_32f_to_8i(color.g);
    pixel.blue = cast_color_32f_to_8i(color.b);
    pixel.alpha = cast_color_32f_to_8i(color.a);
}

Image::Image(size_t width, size_t height)
    : m_width(width), m_height(height), m_image_buffer(width * height) {}

void Image::clear(const ColorRGBA32f &color) {
    for (auto &pixel : m_image_buffer) {
        cast_pixel_32f_to_8i(pixel, color);
    }
}

void Image::set_pixel(const size_t x, const size_t y,
                      const ColorRGBA32f &color) {
    ColorRGBA8i &pixel = m_image_buffer[x + y * m_width];
    cast_pixel_32f_to_8i(pixel, color);
}

size_t Image::get_width() const { return m_width; }

size_t Image::get_height() const { return m_height; }

ColorRGBA32f Image::get_pixel(const size_t x, const size_t y) const {
    float red = m_image_buffer[x + y * m_width].red / 255.0;
    float green = m_image_buffer[x + y * m_width].green / 255.0;
    float blue = m_image_buffer[x + y * m_width].blue / 255.0;
    float alpha = m_image_buffer[x + y * m_width].alpha / 255.0;

    return {red, green, blue, alpha};
}

ColorRGBA8i *Image::get_image_buffer_ptr() { return m_image_buffer.data(); }

} // namespace Vis
