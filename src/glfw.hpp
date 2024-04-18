#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Vis {

class Glfw {
  public:
    Glfw();
    ~Glfw();

    inline auto poll_events() -> auto { glfwPollEvents(); }
    [[nodiscard]] inline auto create_window(int width, int height,
                                            const char *title,
                                            GLFWmonitor *monitor,
                                            GLFWwindow *share) -> auto {
        return glfwCreateWindow(width, height, title, monitor, share);
    }
    inline auto destroy_window(GLFWwindow *window) -> auto {
        glfwDestroyWindow(window);
    }
    inline auto make_context_current(GLFWwindow *window) -> auto {
        glfwMakeContextCurrent(window);
    }
    [[nodiscard]] inline auto window_should_close(GLFWwindow *window) -> auto {
        return glfwWindowShouldClose(window);
    }
    inline auto swap_buffers(GLFWwindow *window) -> auto {
        glfwSwapBuffers(window);
    };
    [[nodiscard]] inline auto get_proc_address() -> auto {
        return glfwGetProcAddress;
    };
    inline auto window_hint(int hint, int value) -> auto {
        glfwWindowHint(hint, value);
        ;
    };
};

} // namespace Vis
