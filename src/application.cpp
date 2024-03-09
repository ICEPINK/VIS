#include "application.hpp"

#include <cstdlib>
#include <iostream>

#include <imgui.h>

namespace Vis {

Application::Application() {
    m_window_uptr = std::make_unique<Window>(800, 600, "Vis");
    m_gui_uptr = std::make_unique<Gui>();
    m_window_uptr->init_gui(m_gui_uptr);
}
Application::~Application() {}

int Application::run() {

    // std::cout << "OpenGL -> Version: " << glGetString(GL_VERSION) << '\n';

    // constexpr uint32_t image_width = 80;
    // constexpr uint32_t image_height = 60;

    // uint32_t image[image_width * image_height] = {};
    // for (size_t i = 0; i < image_width * image_height; ++i) {
    //     uint8_t *r = ((uint8_t *)&image[i]) + 0;
    //     uint8_t *g = ((uint8_t *)&image[i]) + 1;
    //     uint8_t *b = ((uint8_t *)&image[i]) + 2;
    //     uint8_t *a = ((uint8_t *)&image[i]) + 3;

    //     *r = 255;
    //     *g = 0;
    //     *b = 255;
    //     *a = 255;
    // }

    // uint32_t texture;
    // glGenTextures(1, &texture);
    // // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, texture);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image_width, image_height, 0,
    //              GL_RGBA, GL_UNSIGNED_BYTE, image);
    while (!m_window_uptr->should_window_close()) {
        m_gui_uptr->new_frame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("App")) {
                if (ImGui::MenuItem("Item1")) {
                }
                if (ImGui::MenuItem("Item2", "CTRL+Y", false, false)) {
                } // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Item3")) {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // ImGui::Begin("View");
        // ImGui::Text("pointer = %x", texture);
        // ImGui::Text("size = %d x %d", image_width, image_height);
        // ImGui::Image((void *)(intptr_t)texture,
        //              ImVec2(image_width, image_height));
        // ImGui::End();

        ImGui::Begin("Info panel");
        ImGui::Text("Panel text.");
        ImGui::End();


        m_window_uptr->clear();
        m_gui_uptr->render();
        m_window_uptr->update();
    }

    std::cout << "App -> Exit success\n";
    return EXIT_SUCCESS;
}

} // namespace Vis
