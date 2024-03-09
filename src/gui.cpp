#include "gui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Vis {

Gui::Gui() {
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

void Gui::glfw_init(GLFWwindow *window) {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void Gui::opengl_init(const std::string &glsl_version) {
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

} // namespace Vis
