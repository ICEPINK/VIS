#include "application.hpp"

#include "glad.hpp"
#include "pipeline.hpp"
#include "utils/timer.hpp"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <iostream>

namespace Vis {

Application::Application(const std::vector<std::string_view> &args) {
    if (handle_args(args)) {
        m_exit = true;
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
    [[maybe_unused]] size_t i = 0;
    for (const auto &arg : args) {
        if (arg == "-h" || arg == "--help") {
            return arg_print_help();
        }
        if (arg == "-v" || arg == "--version") {
            return arg_print_version();
        }
        if (arg == "-r" || arg == "--res") {
            if (i + 1 >= args.size() || args[i + 1][0] == '-') {
                throw std::runtime_error("Missing resolution argument");
            }
            arg_resolution(args[i + 1]);
        }
        ++i;
    }
    return false;
}

auto Application::arg_print_help() -> bool {
    std::cout << "VIS HELP:\n";
    std::cout << " --help, -h: print help\n";
    std::cout << " --version, -v: print version\n";
    std::cout << " --res, -r: sets resolution (default 800x600)\n";
    return true;
}

auto Application::arg_print_version() -> bool {
    std::cout << "VIS version: 0.0.0" << std::endl;
    return true;
}
auto Application::arg_resolution(const std::string_view resolution) -> void {
    auto x = resolution.find('x');
    if (x == std::string::npos) {
        throw std::runtime_error("Resolution argument is in wrong format!");
    }

    const auto width = resolution.substr(0, x);
    const auto height = resolution.substr(x + 1, resolution.size());
    if (width.empty() || height.empty()) {
        throw std::runtime_error("Resolution argument is in wrong format!");
    }

    if (!std::isdigit(width[0]) || !std::isdigit(height[0])) {
        throw std::runtime_error("Resolution argument is in wrong format!");
    }

    std::stringstream ss;
    ss << width;
    ss >> m_width;

    ss.clear();
    ss.str("");

    ss << height;
    ss >> m_height;
}

auto Application::run() -> void {
    while (!p_window->should_close()) {
        Timer timer(&m_last_loop_time);

        handle_input();
        make_gui(true);
        render_image();

        glClear(GL_COLOR_BUFFER_BIT);
        p_gui->render();
        p_window->swap_buffers();
    }
}

class Solid {
  public:
    Solid() {
        m_vertices = {
            Vertex({-1.0, -1.0, -1.0, 1.0}, {0.0, 0.0, 0.0, 1.0}),
            Vertex({1.0, -1.0, -1.0, 1.0}, {1.0, 0.0, 0.0, 1.0}),
            Vertex({-1.0, 1.0, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}),
            Vertex({1.0, 1.0, -1.0, 1.0}, {1.0, 1.0, 0.0, 1.0}),
            Vertex({-1.0, -1.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}),
            Vertex({1.0, -1.0, 1.0, 1.0}, {1.0, 0.0, 1.0, 1.0}),
            Vertex({-1.0, 1.0, 1.0, 1.0}, {0.0, 1.0, 1.0, 1.0}),
            Vertex({1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}),
        };
        m_indices = {0, 1, 2, 3, 2, 1, 4, 5, 7, 7, 6, 4, 1, 5, 3, 7, 3, 5,
                     4, 0, 6, 2, 6, 0, 2, 3, 6, 7, 6, 3, 4, 5, 0, 1, 5, 0};
    }
    ~Solid() = default;

    auto get_vertices() const -> auto { return m_vertices; }
    auto get_indices() const -> auto { return m_indices; }
    auto get_matrix() const -> auto { return m_matrix; }

  private:
    std::vector<Vertex> m_vertices;
    std::vector<size_t> m_indices;
    glm::dmat4 m_matrix{1.0};
};
Image *g_image;
auto render(std::vector<Vertex> &vertices, const Pipeline &pipeline) -> void {
    pipeline.dehomog(vertices);
    pipeline.trasform_to_viewport(vertices, *g_image);
    pipeline.rasterize(vertices, *g_image, pipeline.set_pixel);
}

Pipeline g_pipeline;

auto render_solid(const Solid &solid) -> void {
    glm::dmat4 model{1.0};

    glm::dvec3 pos{-5.0, 0.0, 0.0};
    glm::dvec3 at{0.0, 0.0, 0.0};
    glm::dvec3 up{0.0, 1.0, 0.0};
    glm::dmat4 view = glm::lookAt(pos, at, up);

    double fov{glm::pi<double>() / 180.0 * 70.0};
    double aspect_ratio{g_image->get_width() /
                        static_cast<double>(g_image->get_height())};
    double near_plane{1.0};
    double far_plane{20.0};
    glm::dmat4 proj =
        glm::perspective(fov, aspect_ratio, near_plane, far_plane);

    glm::dmat4 matrix = proj * view * model * solid.get_matrix();

    decltype(solid.get_vertices()) vertices;
    vertices.reserve(solid.get_vertices().size());

    for (const auto &vertex : solid.get_vertices()) {
        vertices.push_back({matrix * vertex.pos, vertex.col, vertex.one});
    }

    const auto &indices = solid.get_indices();
    for (size_t i = 0; i < indices.size(); i += 3) {
        const auto &a = vertices[indices[i]];
        const auto &b = vertices[indices[i + 1]];
        const auto &c = vertices[indices[i + 2]];

        std::vector<Vertex> triangle{a, b, c};
        render(triangle, g_pipeline);
    }
}

Solid g_solid;

auto Application::render_image() -> void {
    if (m_panel_width != m_image.get_width() ||
        m_panel_height != m_image.get_height()) {
        m_image.resize(static_cast<size_t>(m_panel_width),
                       static_cast<size_t>(m_panel_height));
    }

    m_image.clear({0.0, 0.0, 0.0, 1.0});

    g_pipeline.dehomog = Alg::dehomog;
    g_pipeline.trasform_to_viewport = Alg::trasform_to_viewport;
    g_pipeline.rasterize = Alg::rasterize_triangle;
    g_pipeline.set_pixel = Alg::set_pixel;

    g_image = &m_image;
    render_solid(g_solid);

    p_texture->bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 static_cast<GLsizei>(m_panel_width),
                 static_cast<GLsizei>(m_panel_height), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_image.get_image_data());
}

auto Application::handle_input() -> void {
    p_glfw->poll_events();

    int width{};
    int height{};
    p_window->get_window_size(width, height);
    m_width = static_cast<size_t>(width);
    m_height = static_cast<size_t>(height);

    p_window->get_cursor_pos(m_mouse_pos_x, m_mouse_pos_y);

    static bool s_alt_mode_lock{false};
    auto key_left_alt = p_window->get_key(GLFW_KEY_LEFT_ALT);
    if (key_left_alt == GLFW_PRESS) {
        if (!s_alt_mode_lock) {
            m_alt_mode = !m_alt_mode;
        }
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

        m_mouse_pos_x = 0;
        m_mouse_pos_y = 0;
        p_window->set_cursor_pos(m_mouse_pos_x, m_mouse_pos_y);
    }
}

auto Application::make_gui(bool show_debug) -> void {
    p_gui->new_frame();

    ImGuiIO &io = ImGui::GetIO();
    if (m_alt_mode) {
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    } else {
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }

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

    ImGui::Begin("Application Info");
    ImGui::SeparatorText("Applicaton Info");
    ImGui::Text("p_glfw: %p", (void *)p_glfw.get());
    ImGui::Text("p_window: %p", (void *)p_window.get());
    ImGui::Text("- glfw_window_ptr: %p",
                (void *)p_window->get_glfw_window_ptr());
    ImGui::Text("p_gui: %p", (void *)p_gui.get());
    ImGui::Text("p_texture: %p", (void *)p_texture.get());
    ImGui::Text("- opengl_id: %u", p_texture->get_id());
    ImGui::Text("m_width: %zu", m_width);
    ImGui::Text("m_height: %zu", m_height);
    ImGui::Text("m_panel_width: %f", m_panel_width);
    ImGui::Text("m_panel_height: %f", m_panel_height);
    ImGui::Text("m_title: %s", m_title.c_str());
    ImGui::Text("m_alt_mode: %s", m_alt_mode ? "true" : "false");
    ImGui::Text("m_mouse_pos_x: %f", m_mouse_pos_x);
    ImGui::Text("m_mouse_pos_y: %f", m_mouse_pos_y);
    ImGui::Text("m_image:");
    ImGui::Text("- width: %zu", m_image.get_width());
    ImGui::Text("- height: %zu", m_image.get_height());
    ImGui::Text("m_last_loop_time: %f", m_last_loop_time);
    ImGui::Text("- fps: %f", 1 / m_last_loop_time);
    ImGui::SeparatorText("Test variables");
    ImGui::Text("test_blue: %f", test_blue);
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0, 0.0});
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
    ImGui::PopStyleVar();
}

[[nodiscard]] auto Application::exit() const -> bool { return m_exit; }

} // namespace Vis
