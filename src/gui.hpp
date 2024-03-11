#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "vis_utils.hpp"

namespace Vis {

class Gui {
  public:
    Gui(AppInfo &app_info);
    ~Gui();

    void render();
    void new_frame();
    void prepare_gui();

    void glfw_init(GLFWwindow *window);
    void opengl_init(const std::string &glsl_version);

  private:
    AppInfo &m_app_info_ref;
};

} // namespace Vis
