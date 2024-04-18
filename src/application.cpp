#include "application.hpp"

#include "glad.hpp"
#include "window.hpp"

#include <iostream>

namespace Vis {

Application::Application(const std::vector<std::string_view> &args) {
    if (handle_args(args)) {
        return;
    }

    auto glfw = std::make_shared<Glfw>();
    // const auto glsl_version = "#version 460";
    glfw->window_hint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfw->window_hint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfw->window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfw->window_hint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    Window window({800, 600, "VIS"}, glfw);
    window.make_context_current();

    Glad::load_gl_loader((GLADloadproc)glfw->get_proc_address());
    glDebugMessageCallback(Glad::print_gl_message, nullptr);
    std::cout << "OpenGL: version " << glGetString(GL_VERSION) << '\n';

    while (!window.should_close()) {
        glfw->poll_events();
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_LINES, 0, 3);
        window.swap_buffers();
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
