#pragma once

#include <cstddef>
#include <cstdint>

namespace Vis {

struct AppInfo {
    void *view_image_ptr{nullptr};
    size_t fps{0};
    uint32_t view_texture_id{0};
    float view_width{0};
    float view_height{0};
    bool alt_mode{0};
    bool alt_mode_lock{0};
    double mouse_x{0};
    double mouse_y{0};
};

} // namespace Vis
