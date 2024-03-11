#pragma once

#include <cstddef>
#include <cstdint>

namespace Vis {

class Texture {
  public:
    Texture();
    ~Texture();

    void bind();
    void unbind();

    void set_image(size_t width, size_t height, void *image_ptr);

    uint32_t get_id();

  private:
    uint32_t m_id;
};

} // namespace Vis
