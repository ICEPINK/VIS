#pragma once

#include <memory>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "gui.hpp"

namespace Vis {

class Window {
  public:
    Window(const int32_t width, const int32_t height, const std::string title);
    ~Window();

    void update();
    void clear();
    void set_clear_color(float reg, float green, float blue, float alpha);
    bool should_window_close() const;

    void init_gui(const std::unique_ptr<Gui> &gui);

    size_t get_widht() const { return m_width; }
    size_t get_height() const { return m_height; }
    std::string get_title() const { return m_title; }

  private:
    GLFWwindow *m_window_ptr;
    size_t m_width;
    size_t m_height;
    std::string m_title;
    std::string m_glsl_version;
};

} // namespace Vis
