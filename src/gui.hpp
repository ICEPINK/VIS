#pragma once

#include "window.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Vis {

class Gui {
  public:
    Gui(const Window &window, const char *glsl_version);
    ~Gui();

  private:
};

} // namespace Vis
