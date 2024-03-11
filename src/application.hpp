#pragma once

#include <memory>

#include "cpu_renderer.hpp"
#include "gpu_api.hpp"
#include "gui.hpp"
#include "texture.hpp"
#include "vis_utils.hpp"
#include "window.hpp"

namespace Vis {

class Application {
  public:
    Application();
    ~Application();

    int run();

  private:
    std::unique_ptr<Window> m_window_uptr;
    std::unique_ptr<Gui> m_gui_uptr;
    std::unique_ptr<Texture> m_texture_uptr;
    std::unique_ptr<CpuRenderer> m_cpu_renderer_uptr;
    GpuApi m_gpu_api;
    AppInfo m_app_info;
    SceneInfo m_scene_info;
};

} // namespace Vis
