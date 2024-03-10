#include "application.hpp"

#include <cstdlib>

#include <glad/glad.h>
#include <imgui.h>

namespace Vis {

Application::Application() {
    m_window_uptr = std::make_unique<Window>(800, 600, "Vis");
    m_gui_uptr = std::make_unique<Gui>();
    m_window_uptr->init_gui(m_gui_uptr);
}
Application::~Application() {}

int Application::run() {
    uint32_t texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    while (!m_window_uptr->should_window_close()) {

        m_gui_uptr->new_frame();
        ImGui::DockSpaceOverViewport();
        ImGui::ShowDemoWindow();

        ImGui::Begin("View");
        auto view_width = ImGui::GetContentRegionAvail().x;
        auto view_height = ImGui::GetContentRegionAvail().y;

        ImGui::Image((void *)(intptr_t)texture,
                     ImVec2(view_width, view_height));
        ImGui::End();

        ImGui::Begin("Scene settings");
        ImGui::End();

        auto image = m_cpu_renderer.render_image(view_width, view_height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, view_width, view_height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);

        m_window_uptr->clear();
        m_gui_uptr->render();
        m_window_uptr->update();
    }

    return EXIT_SUCCESS;
}

} // namespace Vis
