#pragma once

#include <vector>

#include <glm/vec4.hpp>

namespace Vis {

typedef glm::vec4 ColorRGBA32f;

struct ColorRGBA8i {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

class Image {
  public:
    Image(const size_t width, const size_t height);

    void clear(const ColorRGBA32f &color);

    void set_pixel(const size_t x, const size_t y, const ColorRGBA32f &color);

    size_t get_width() const;
    size_t get_height() const;
    ColorRGBA32f get_pixel(const size_t x, const size_t y) const;
    ColorRGBA8i *get_image_buffer_ptr();

  private:
    const size_t m_width;
    const size_t m_height;
    std::vector<ColorRGBA8i> m_image_buffer;
};

} // namespace Vis
