#include "application.hpp"

#include <cstdlib>

#include <glad/glad.h>

namespace Vis {

Application::Application() : m_gpu_api(GpuApiType::OpenGL) {
    m_window_uptr = std::make_unique<Window>(800, 600, "Vis", m_gpu_api);
    m_gui_uptr = std::make_unique<Gui>(m_app_info);
    m_window_uptr->init_gui(m_gui_uptr);
}
Application::~Application() {}

int Application::run() {
    glGenTextures(1, &m_app_info.view_texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_app_info.view_texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    while (!m_window_uptr->should_window_close()) {

        m_gui_uptr->new_frame();
        m_gui_uptr->prepare_gui();

        m_app_info.view_image_ptr = m_cpu_renderer.render_image(
            m_app_info.view_width, m_app_info.view_height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_app_info.view_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_app_info.view_width,
                     m_app_info.view_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     m_app_info.view_image_ptr);

        m_window_uptr->clear();
        m_gui_uptr->render();
        m_window_uptr->update();
    }

    return EXIT_SUCCESS;
}

} // namespace Vis
