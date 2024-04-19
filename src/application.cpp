#include "application.hpp"

#include "glad.hpp"

#include <iostream>

namespace Vis {

Application::Application(const std::vector<std::string_view> &args) {
    if (handle_args(args)) {
        return;
    }

    p_glfw = std::make_shared<Glfw>();
    p_glfw->window_hint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    p_glfw->window_hint(GLFW_CONTEXT_VERSION_MINOR, 6);
    p_glfw->window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    p_glfw->window_hint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    const WindowInfo info{m_width, m_height, m_title};
    p_window = std::make_unique<Window>(info, p_glfw);
    p_window->make_context_current();

    Glad::load_gl_loader((GLADloadproc)p_glfw->get_proc_address());
    glDebugMessageCallback(Glad::print_gl_message, nullptr);
    // std::cout << "OpenGL: Version " << glGetString(GL_VERSION) << '\n';

    p_gui = std::make_unique<Gui>(*p_window, "#version 460");

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = true;

    ImGui::StyleColorsDark();
}

Application::~Application() {}

[[nodiscard]] auto
Application::handle_args(const std::vector<std::string_view> &args) -> bool {
    [[maybe_unused]] int i = 0;
    for (const auto &arg : args) {
        if (arg == "-h" || arg == "--help") {
            return print_help();
        }
        if (arg == "-v" || arg == "--version") {
            return print_version();
        }
        ++i;
    }
    return false;
}

auto Application::print_help() -> bool {
    std::cout << "VIS HELP:\n";
    std::cout << " --help, -h: print help\n";
    std::cout << " --version, -v: print version\n";
    return true;
}

auto Application::print_version() -> bool {
    std::cout << "VIS version: 0.0.0\n";
    return true;
}

auto Application::run_main_loop() -> void {
    while (!p_window->should_close()) {
        p_glfw->poll_events();
        glClear(GL_COLOR_BUFFER_BIT);

        p_gui->new_frame();

        make_gui(true);

        p_gui->render();
        p_window->swap_buffers();
    }
}

auto Application::make_gui(bool show_debug) -> void {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("App")) {
            if (ImGui::MenuItem("Item1")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport();

    if (show_debug) {
        ImGui::ShowDemoWindow();
    }
}

} // namespace Vis
