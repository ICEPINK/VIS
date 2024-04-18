#include "window.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

namespace Vis {

Window::Window(WindowInfo info, std::shared_ptr<Glfw> glfw) : p_glfw(glfw) {
    p_window = p_glfw->create_window(
        std::min(info.width,
                 static_cast<size_t>(std::numeric_limits<int>::max())),
        std::min(info.height,
                 static_cast<size_t>(std::numeric_limits<int>::max())),
        info.title.c_str(), info.monitor, info.share);

    if (!p_window) {
        if (!(info.width > 0)) {
            std::cout << "WARN: Window width must be greater than zero.\n";
        }
        if (!(info.height > 0)) {
            std::cout << "WARN: Window height must be greater than zero.\n";
        }
        throw std::runtime_error("Window creation failed!");
    }
}
Window::~Window() { p_glfw->destroy_window(p_window); }

} // namespace Vis
