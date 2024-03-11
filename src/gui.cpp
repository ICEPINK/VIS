#include "gui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Vis {

Gui::Gui(AppInfo &app_info, SceneInfo &scene_info)
    : m_app_info_ref(app_info), m_scene_info_ref(scene_info) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigFlags |= ImGuiConfigFlags_NoMouse; Disable mouse in GUI

    io.ConfigDockingWithShift = true;

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

void Gui::prepare_gui(const bool demo) {
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

    if (demo) {
        ImGui::ShowDemoWindow();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0, 0.0});
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
    ImGui::PopStyleVar();

    static bool alpha_preview = true;
    static bool alpha_half_preview = false;
    static bool drag_and_drop = true;
    static bool options_menu = true;
    static bool hdr = false;
    ImGuiColorEditFlags misc_flags =
        (hdr ? ImGuiColorEditFlags_HDR : 0) |
        (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) |
        (alpha_half_preview
             ? ImGuiColorEditFlags_AlphaPreviewHalf
             : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) |
        (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

    ImGui::Begin("Scene settings");
    ImGui::Text("Clear color");
    ImGui::ColorEdit4("", &m_scene_info_ref.clear_color.r,
                      ImGuiColorEditFlags_Float | misc_flags);
    ImGui::End();
}

void Gui::glfw_init(GLFWwindow *window) {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void Gui::opengl_init(const std::string &glsl_version) {
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

} // namespace Vis
