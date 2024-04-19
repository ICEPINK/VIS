#pragma once

#include "glfw.hpp"

#include <memory>
#include <string>

namespace Vis {

struct WindowInfo {
    size_t width{1};
    size_t height{1};
    std::string title{""};
    GLFWmonitor *monitor{nullptr};
    GLFWwindow *share{nullptr};
};

class Window {
  public:
    Window(const WindowInfo &info, std::shared_ptr<Glfw> glfw);
    ~Window();

    inline auto make_context_current() -> void {
        p_glfw->make_context_current(p_window);
    };

    inline auto swap_buffers() -> void { p_glfw->swap_buffers(p_window); };

    [[nodiscard]] inline auto should_close() -> bool {
        return p_glfw->window_should_close(p_window);
    }

    inline auto get_glfw_window_ptr() const -> auto { return p_window; }

  private:
    GLFWwindow *p_window;
    std::shared_ptr<Glfw> p_glfw;
};

} // namespace Vis
