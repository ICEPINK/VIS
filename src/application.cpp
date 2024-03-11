#include "application.hpp"

#include <cstdlib>

#include <glad/glad.h>

namespace Vis {

Application::Application() : m_gpu_api(GpuApiType::OpenGL) {
    m_window_uptr = std::make_unique<Window>(800, 600, "Vis", m_gpu_api);
    m_gui_uptr = std::make_unique<Gui>(m_app_info);
    m_window_uptr->init_gui(m_gui_uptr);
    m_texture_uptr = std::make_unique<Texture>();
    m_app_info.view_texture_id = m_texture_uptr->get_id();
}
Application::~Application() {}

int Application::run() {

    while (!m_window_uptr->should_window_close()) {
        m_window_uptr->handle_input(m_app_info, m_scene_info);
        m_gui_uptr->new_frame();
        m_gui_uptr->prepare_gui(1);

        m_app_info.view_image_ptr = m_cpu_renderer.render_image(
            m_app_info.view_width, m_app_info.view_height);

        m_texture_uptr->set_image(m_app_info.view_width, m_app_info.view_height,
                                  m_app_info.view_image_ptr);

        m_window_uptr->clear();
        m_gui_uptr->render();
        m_window_uptr->update();
    }

    return EXIT_SUCCESS;
}

} // namespace Vis
