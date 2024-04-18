#include "gui.hpp"

namespace Vis {
size_t s_gui_count{0};

Gui::Gui(const Window &window, const char *glsl_version) {
    if (s_gui_count == 0) {
        if (!IMGUI_CHECKVERSION()) {
            throw std::runtime_error("Failed to check ImGui version!");
        }
        ImGui::CreateContext();

        if (!ImGui_ImplGlfw_InitForOpenGL(window.get_glfw_window_ptr(), true)) {

            ImGui::DestroyContext();
            throw std::runtime_error("Failed to initialize GLFW for ImGui!");
        }
        if (!ImGui_ImplOpenGL3_Init(glsl_version)) {

            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            throw std::runtime_error("Failed to initialize OpenGL for ImGui!");
        }
    }
    ++s_gui_count;
};
Gui::~Gui() {
    --s_gui_count;
    if (s_gui_count == 0) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

} // namespace Vis
