#pragma once

#include "glfw.hpp"
#include "window.hpp"

namespace Vis {

struct ApplicationInfo {
    std::shared_ptr<Glfw> glfw{nullptr};
    std::unique_ptr<Window> window{nullptr};
    size_t width{800};
    size_t height{600};
    std::string title{"VIS"};
};

} // namespace Vis
