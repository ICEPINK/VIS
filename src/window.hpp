#pragma once

#include "glfw.hpp"

#include <memory>
#include <string>
#include <stdexcept>

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

    inline auto set_input_mode(const int mode, const int value) const -> auto {
        return p_glfw->set_input_mode(p_window, mode, value);
    }

    [[nodiscard]] inline auto get_glfw_window_ptr() const -> auto {
        return p_window;
    }

    [[nodiscard]] inline auto get_key(const int key) const -> auto {
        return p_glfw->get_key(p_window, key);
    }

    inline auto get_cursor_pos(double &mouse_pos_x, double &mouse_pos_y) const
        -> auto {
        p_glfw->get_cursor_pos(p_window, &mouse_pos_x, &mouse_pos_y);
    }

    inline auto set_cursor_pos(const double mouse_pos_x,
                               const double mouse_pos_y) const -> auto {
        p_glfw->set_cursor_pos(p_window, mouse_pos_x, mouse_pos_y);
    }

    inline auto get_window_size(int &width, int &height) const -> auto {
        p_glfw->get_window_size(p_window, &width, &height);
    }

  private:
    GLFWwindow *p_window;
    std::shared_ptr<Glfw> p_glfw;
};

} // namespace Vis
