#pragma once

#include <memory>

#include "window.hpp"
#include "gui.hpp"

namespace Vis {

class Application {
  public:
    Application();
    ~Application();

    int run();

  private:
    // int init_window();
    // int init_gui();
    // int init_opengl();
  private:
    std::unique_ptr<Window> m_window_uptr;
    std::unique_ptr<Gui> m_gui_uptr;
};

} // namespace Vis
