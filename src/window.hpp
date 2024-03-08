#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Vis {

class Window {
  public:
    Window(const size_t width, const size_t height, const std::string title);
    ~Window();

    void update();
    bool should_window_close() const;

    GLFWwindow *get_window_ptr() const { return m_window_ptr; }
    size_t get_widht() const { return m_width; }
    size_t get_height() const { return m_height; }
    std::string get_title() const { return m_title; }
    std::string get_glsl_version() const { return m_glsl_version; }

  private:
    GLFWwindow *m_window_ptr;
    size_t m_width;
    size_t m_height;
    std::string m_title;
    std::string m_glsl_version;
};

} // namespace Vis
