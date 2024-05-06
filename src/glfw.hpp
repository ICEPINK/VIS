#pragma once

#include <GLFW/glfw3.h>

namespace Vis {

class Glfw {
public:
  Glfw();
  ~Glfw();

  inline auto poll_events() const -> auto { glfwPollEvents(); }
  [[nodiscard]] inline auto
  create_window(const int width, const int height, const char *const title,
                GLFWmonitor *const monitor,
                GLFWwindow *const share) const -> auto {
    return glfwCreateWindow(width, height, title, monitor, share);
  }
  inline auto destroy_window(GLFWwindow *const window) const -> auto {
    glfwDestroyWindow(window);
  }
  inline auto make_context_current(GLFWwindow *const window) const -> auto {
    glfwMakeContextCurrent(window);
  }
  [[nodiscard]] inline auto
  window_should_close(GLFWwindow *const window) const -> auto {
    return glfwWindowShouldClose(window);
  }
  inline auto swap_buffers(GLFWwindow *const window) const -> auto {
    glfwSwapBuffers(window);
  }
  [[nodiscard]] inline auto get_proc_address() const -> auto {
    return glfwGetProcAddress;
  }
  inline auto window_hint(const int hint, const int value) const -> auto {
    glfwWindowHint(hint, value);
  }
  inline auto set_input_mode(GLFWwindow *const window, const int mode,
                             const int value) const -> auto {
    return glfwSetInputMode(window, mode, value);
  }

  inline auto get_key(GLFWwindow *const window, const int key) const -> auto {
    return glfwGetKey(window, key);
  }

  inline auto get_cursor_pos(GLFWwindow *const window,
                             double *const mouse_pos_x,
                             double *const mouse_pos_y) const -> auto {
    return glfwGetCursorPos(window, mouse_pos_x, mouse_pos_y);
  }

  inline auto get_window_size(GLFWwindow *const window, int *const width,
                              int *const height) const -> auto {
    glfwGetWindowSize(window, width, height);
  }

  inline auto set_cursor_pos(GLFWwindow *const window, const double mouse_pos_x,
                             const double mouse_pos_y) -> auto {
    glfwSetCursorPos(window, mouse_pos_x, mouse_pos_y);
  }
};

} // namespace Vis
