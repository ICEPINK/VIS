#include "application.hpp"

#include "glad.hpp"
#include "gui.hpp"
#include "window.hpp"

#include <iostream>

namespace Vis {

Application::Application(const std::vector<std::string_view> &args) {
    if (handle_args(args)) {
        return;
    }

    m_info.glfw = std::make_shared<Glfw>();
    m_info.glfw->window_hint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    m_info.glfw->window_hint(GLFW_CONTEXT_VERSION_MINOR, 6);
    m_info.glfw->window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_info.glfw->window_hint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    WindowInfo info{m_info.width, m_info.height, m_info.title};
    m_info.window = std::make_unique<Window>(info, m_info.glfw);
    m_info.window->make_context_current();

    Glad::load_gl_loader((GLADloadproc)m_info.glfw->get_proc_address());

    glDebugMessageCallback(Glad::print_gl_message, nullptr);
    // std::cout << "OpenGL: Version " << glGetString(GL_VERSION) << '\n';

    Gui gui(*m_info.window, "#version 460");

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingWithShift = true;

    ImGui::StyleColorsDark();

    while (!m_info.window->should_close()) {
        m_info.glfw->poll_events();
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("App")) {
                if (ImGui::MenuItem("Item1")) {
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::DockSpaceOverViewport();
        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_info.window->swap_buffers();
    }
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

} // namespace Vis
