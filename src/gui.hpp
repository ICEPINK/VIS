#pragma once

#include "window.hpp"

#define GLFW_INCLUDE_NONE
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Vis {

class Gui {
  public:
    Gui(const Window &window, const char *glsl_version);
    ~Gui();

    auto new_frame() -> void;
    auto render() -> void;

  private:
};

} // namespace Vis
