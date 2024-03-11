#include "gui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Vis {

Gui::Gui(AppInfo &app_info) : m_app_info_ref(app_info) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
}
Gui::~Gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Gui::new_frame() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::prepare_gui() {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("App")) {
            if (ImGui::MenuItem("Item1")) {
            }
            if (ImGui::MenuItem("Item2", "CTRL+Y", false, false)) {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Item3")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport();

    ImGui::Begin("View");
    m_app_info_ref.view_width = ImGui::GetContentRegionAvail().x;
    m_app_info_ref.view_height = ImGui::GetContentRegionAvail().y;

    if (m_app_info_ref.view_width < 0) {
        m_app_info_ref.view_width = 0;
    }

    if (m_app_info_ref.view_height < 0) {
        m_app_info_ref.view_height = 0;
    }

    ImGui::Image((void *)(intptr_t)m_app_info_ref.view_texture_id,
                 ImVec2(m_app_info_ref.view_width, m_app_info_ref.view_height));
    ImGui::End();

    ImGui::Begin("Scene settings");
    ImGui::End();
}

void Gui::glfw_init(GLFWwindow *window) {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void Gui::opengl_init(const std::string &glsl_version) {
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

} // namespace Vis
