#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace Vis {

typedef glm::vec4 ColorRGBA32f;

struct ColorRGBA8i {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct SceneInfo {};

struct AppInfo {
    void *view_image_ptr{nullptr};
    uint32_t view_texture_id{0};
    float view_width{0};
    float view_height{0};
};

} // namespace Vis
