#include "application.hpp"

#include "image.hpp"
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

    p_texture = std::make_unique<Texture>();
    p_texture->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
    Image image;

    while (!p_window->should_close()) {
        p_glfw->poll_events();
        handle_input();

        p_gui->new_frame();
        make_gui(true);

        image.resize(static_cast<size_t>(m_panel_width),
                     static_cast<size_t>(m_panel_height));

        image.clear({0.0, 0.0, test_blue, 1.0});

        p_texture->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     static_cast<GLsizei>(m_panel_width),
                     static_cast<GLsizei>(m_panel_height), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image.get_image_data());

        glClear(GL_COLOR_BUFFER_BIT);
        p_gui->render();
        p_window->swap_buffers();
    }
}

auto Application::handle_input() -> void {
    static bool s_alt_mode_lock{false};

    auto key_left_alt = p_window->get_key(GLFW_KEY_LEFT_ALT);
    if (key_left_alt == GLFW_PRESS) {
        if (!s_alt_mode_lock) {
            m_alt_mode = !m_alt_mode;
        }
        p_window->get_cursor_pos(m_mouse_pos_x, m_mouse_pos_y);
        s_alt_mode_lock = true;
    }

    if (key_left_alt == GLFW_RELEASE) {
        s_alt_mode_lock = false;
    }

    if (m_alt_mode) {
        p_window->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        p_window->set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (m_alt_mode) {
        int key;
        key = p_window->get_key(GLFW_KEY_W);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            test_blue += 0.01;
            if (test_blue > 1.0) {
                test_blue = 1.0;
            }
        }
        key = p_window->get_key(GLFW_KEY_S);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            test_blue -= 0.01;
            if (test_blue < 0.0) {
                test_blue = 0.0;
            }
        }
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

    ImGui::Begin("Info");
    ImGui::Text("Alt mode: %s", m_alt_mode ? "true" : "false");
    ImGui::End();

    ImGui::Begin("Viewport");

    m_panel_width = ImGui::GetContentRegionAvail().x;
    m_panel_height = ImGui::GetContentRegionAvail().y;

    if (m_panel_width < 0.0f) {
        m_panel_width = 0.0f;
    }

    if (m_panel_height < 0.0f) {
        m_panel_height = 0.0f;
    }

    ImGui::Image((ImTextureID)(intptr_t)p_texture->get_id(),
                 ImVec2{m_panel_width, m_panel_height}, ImVec2(0, 1),
                 ImVec2(1, 0));
    ImGui::End();
}

} // namespace Vis
