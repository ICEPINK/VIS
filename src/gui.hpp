#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace Vis {

class Gui {
  public:
    Gui();
    ~Gui();

    void render();
    void new_frame();

    void glfw_init(GLFWwindow *window);
    void opengl_init(const std::string &glsl_version);
};

} // namespace Vis
