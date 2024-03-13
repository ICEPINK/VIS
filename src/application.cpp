#include "application.hpp"

#include <cstdlib>

#include <glad/glad.h>

namespace Vis {

Application::Application() : m_gpu_api(GpuApiType::OpenGL) {
    m_scene_info = {};
    m_app_info = {};
    m_cpu_renderer_uptr = std::make_unique<CpuRenderer>(m_scene_info);
    m_window_uptr = std::make_unique<Window>(800, 600, "Vis", m_gpu_api);
    m_gui_uptr = std::make_unique<Gui>(m_app_info, m_scene_info);
    m_texture_uptr = std::make_unique<Texture>();
    m_window_uptr->init_gui(m_gui_uptr);
    m_app_info.view_texture_id = m_texture_uptr->get_id();
}
Application::~Application() {}

int Application::run() {

    while (!m_window_uptr->should_window_close()) {
        auto frame_start = std::chrono::high_resolution_clock::now();

        m_window_uptr->handle_input(m_app_info, m_scene_info);
        m_gui_uptr->new_frame();
        m_gui_uptr->prepare_gui(1);

        m_app_info.view_image_ptr = m_cpu_renderer_uptr->render_image(
            static_cast<size_t>(m_app_info.view_width),
            static_cast<size_t>(m_app_info.view_height));

        m_texture_uptr->set_image(static_cast<size_t>(m_app_info.view_width),
                                  static_cast<size_t>(m_app_info.view_height),
                                  m_app_info.view_image_ptr);

        m_window_uptr->clear();
        m_gui_uptr->render();
        m_window_uptr->update();

        auto frame_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frame_time = frame_end - frame_start;
        m_app_info.fps = static_cast<size_t>(1.0 / frame_time.count());
    }

    return EXIT_SUCCESS;
}

} // namespace Vis
