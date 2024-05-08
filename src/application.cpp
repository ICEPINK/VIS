#include "application.hpp"
#include "glad.hpp"
#include "utils/timer.hpp"
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <array>
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

  const WindowInfo window_info{m_width, m_height, m_title};
  p_window = std::make_unique<Window>(window_info, p_glfw);
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

  // HACK: Begin
  PerspectiveCameraInfo simulated_camera_info{};
  simulated_camera_info.width = static_cast<double>(m_width);
  simulated_camera_info.height = static_cast<double>(m_height);
  simulated_camera_info.position = {0.0, 0.0, 0.0};
  simulated_camera_info.near_plane = 1.0;
  simulated_camera_info.far_plane = 10.0;
  m_scene_info.simulated_camera =
    std::make_unique<PerspectiveCamera>(simulated_camera_info);
  m_scene_info.camera =
    std::make_unique<PerspectiveCamera>(simulated_camera_info);
  m_scene_info.active_camera = m_scene_info.simulated_camera.get();
  static constexpr auto matrix_trasform = [](std::vector<Vertex> &vertices,
                                             const glm::dmat4 &matrix) {
    for (auto &vertex : vertices) {
      vertex.pos = matrix * vertex.pos;
    }
  };
  m_scene_info.render_triangle_pipeline.trasform_vertices = matrix_trasform;
  m_scene_info.render_triangle_pipeline.trasform_to_viewport =
    Alg::trasform_to_viewport;
  m_scene_info.render_triangle_pipeline.rasterize = Alg::rasterize_triangle;
  m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_rgba_depth;
  m_scene_info.render_triangle_pipeline.dehomog = Alg::dehomog_all;
  m_scene_info.render_triangle_pipeline.clip_fast = Alg::clip_fast_triangle;
  m_scene_info.render_triangle_pipeline.clip_before_dehomog =
    Alg::clip_before_dehomog_triangle;
  m_scene_info.render_triangle_pipeline.clip_after_dehomog =
    Alg::clip_after_dehomog_none;

  m_scene_info.render_line_pipeline.trasform_vertices = matrix_trasform;
  m_scene_info.render_line_pipeline.trasform_to_viewport =
    Alg::trasform_to_viewport;
  m_scene_info.render_line_pipeline.rasterize = Alg::rasterize_line;
  m_scene_info.render_line_pipeline.set_pixel = Alg::set_pixel_rgba_depth;
  m_scene_info.render_line_pipeline.dehomog = Alg::dehomog_all;
  m_scene_info.render_line_pipeline.clip_fast = Alg::clip_fast_line;
  m_scene_info.render_line_pipeline.clip_before_dehomog =
    Alg::clip_before_dehomog_line;
  m_scene_info.render_line_pipeline.clip_after_dehomog =
    Alg::clip_after_dehomog_none;

  m_scene_info.simulate_triangle_pipeline.trasform_vertices = matrix_trasform;
  m_scene_info.simulate_triangle_pipeline.trasform_to_viewport =
    Alg::trasform_to_none;
  m_scene_info.simulate_triangle_pipeline.rasterize = Alg::rasterize_none;
  m_scene_info.simulate_triangle_pipeline.set_pixel = Alg::set_pixel_rgba_depth;
  m_scene_info.simulate_triangle_pipeline.dehomog = Alg::dehomog_all;
  m_scene_info.simulate_triangle_pipeline.clip_fast = Alg::clip_fast_triangle;
  m_scene_info.simulate_triangle_pipeline.clip_before_dehomog =
    Alg::clip_before_dehomog_triangle;
  m_scene_info.simulate_triangle_pipeline.clip_after_dehomog =
    Alg::clip_after_dehomog_triangle;
  // HACK: End
}
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

    p_gui->render();
    p_window->swap_buffers();
  }
}

auto Application::render(std::vector<Vertex> &vertices,
                         const Pipeline &pipeline,
                         const glm::dmat4 &matrix) -> void {
  pipeline.trasform_vertices(vertices, matrix);
  pipeline.clip_fast(vertices);
  pipeline.clip_before_dehomog(vertices);
  pipeline.dehomog(vertices);
  pipeline.clip_after_dehomog(vertices);
  pipeline.trasform_to_viewport(vertices, m_image);
  pipeline.rasterize(vertices, m_image, pipeline.set_pixel);
}
auto Application::simulate_solid(const Solid &solid) -> Solid {
  // NOTE: Rethink matrix
  auto matrix = m_scene_info.simulated_camera->get_projection() *
                m_scene_info.simulated_camera->get_view() *
                m_scene_info.model_matrix * solid.matrix;
  Solid new_solid;
  new_solid.name = solid.name;
  for (const auto &layout : solid.layout) {
    size_t vertices_per_primitie = 1;
    Pipeline *pipeline{nullptr};
    switch (layout.topology) {
    case Topology::Point: {
      vertices_per_primitie = 1;
      pipeline = &m_scene_info.simulate_point_pipeline;
    } break;
    case Topology::Line: {
      vertices_per_primitie = 2;
      pipeline = &m_scene_info.simulate_line_pipeline;
    } break;
    case Topology::Triangle: {
      vertices_per_primitie = 3;
      pipeline = &m_scene_info.simulate_triangle_pipeline;
    } break;
    }
    for (size_t i = layout.start;
         i < layout.start + layout.count * vertices_per_primitie;
         i += vertices_per_primitie) {
      std::vector<Vertex> triangle;
      triangle.reserve(vertices_per_primitie);
      for (size_t j = 0; j < vertices_per_primitie; ++j) {
        triangle.push_back(solid.vertices[solid.indices[i + j]]);
      }
      render(triangle, *pipeline, matrix);
      const size_t new_size = new_solid.vertices.size() + triangle.size();
      new_solid.vertices.reserve(new_size);
      new_solid.indices.reserve(new_size);
      if (triangle.size() % vertices_per_primitie == 0) {
        if (new_solid.layout.size() > 0 &&
            new_solid.layout.back().topology == layout.topology) {
          for (size_t j = 0; j < triangle.size(); j += vertices_per_primitie) {
            for (size_t k = 0; k < vertices_per_primitie; ++k) {
              new_solid.vertices.push_back(triangle[j + k]);
              new_solid.indices.push_back(new_solid.indices.size());
            }
            ++new_solid.layout.back().count;
          }
        } else {
          new_solid.layout.push_back(
            {layout.topology, new_solid.vertices.size(), 0});
          for (size_t j = 0; j < triangle.size(); j += vertices_per_primitie) {
            for (size_t k = 0; k < vertices_per_primitie; ++k) {
              new_solid.vertices.push_back(triangle[j + k]);
              new_solid.indices.push_back(new_solid.indices.size());
            }
            ++new_solid.layout.back().count;
          }
        }
      }
    }
  }
  return new_solid;
}
auto get_camera_model(const PerspectiveCamera &camera) -> Solid {
  PerspectiveCameraInfo info = camera.get_info();
  Solid solid{};
  solid.name = "";
  solid.matrix = glm::dmat4{1.0};
  solid.vertices.reserve(9);
  glm::dvec4 color = {1.0, 1.0, 1.0, 1.0};
  Vertex v1{};
  v1.pos = {info.position.x, info.position.y, info.position.z, 1.0};
  v1.col = color;
  solid.vertices.push_back(v1);
  std::array<Vertex, 8> vertices{};
  vertices[0].pos = {-1.0, -1.0, 0.0, 1.0};
  vertices[1].pos = {1.0, -1.0, 0.0, 1.0};
  vertices[2].pos = {-1.0, 1.0, 0.0, 1.0};
  vertices[3].pos = {1.0, 1.0, 0.0, 1.0};
  vertices[4].pos = {-1.0, -1.0, 1.0, 1.0};
  vertices[5].pos = {1.0, -1.0, 1.0, 1.0};
  vertices[6].pos = {-1.0, 1.0, 1.0, 1.0};
  vertices[7].pos = {1.0, 1.0, 1.0, 1.0};
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertices[i].col = color;
    vertices[i].pos = glm::inverse(camera.get_view()) *
                      glm::inverse(camera.get_projection()) * vertices[i].pos;
    solid.vertices.push_back(vertices[i]);
  }
  solid.indices = {0, 5, 0, 6, 0, 7, 0, 8, 1, 2, 2, 4,
                   4, 3, 3, 1, 5, 6, 6, 8, 8, 7, 7, 5};
  solid.layout.push_back({Topology::Line, 0, 12});
  return solid;
};
auto Application::render_solid(const Solid &solid) -> void {
  auto matrix = m_scene_info.active_camera->get_projection() *
                m_scene_info.active_camera->get_view() *
                m_scene_info.model_matrix * solid.matrix;
  for (const auto &layout : solid.layout) {
    size_t vertices_per_primitie = 1;
    Pipeline *pipeline{nullptr};
    switch (layout.topology) {
    case Topology::Point: {
      vertices_per_primitie = 1;
      pipeline = &m_scene_info.render_point_pipeline;
    } break;
    case Topology::Line: {
      vertices_per_primitie = 2;
      pipeline = &m_scene_info.render_line_pipeline;
    } break;
    case Topology::Triangle: {
      vertices_per_primitie = 3;
      pipeline = &m_scene_info.render_triangle_pipeline;
    } break;
    }
    for (size_t i = layout.start;
         i < layout.start + layout.count * vertices_per_primitie;
         i += vertices_per_primitie) {
      std::vector<Vertex> triangle;
      triangle.reserve(vertices_per_primitie);
      for (size_t j = 0; j < vertices_per_primitie; ++j) {
        triangle.push_back(solid.vertices[solid.indices[i + j]]);
      }
      render(triangle, *pipeline, matrix);
    }
  }
}

auto Application::render_image() -> void {
  if (m_panel_width != m_image.get_width() ||
      m_panel_height != m_image.get_height()) {
    m_image.resize(static_cast<size_t>(m_panel_width),
                   static_cast<size_t>(m_panel_height));
    m_scene_info.active_camera->set_width(m_panel_width);
    m_scene_info.active_camera->set_height(m_panel_height);
  }
  m_image.clear({0.0, 0.0, 0.0, 1.0});
  // HACK: Begin
  if (m_scene_info.simulate) {
    Solid simulated = simulate_solid(m_scene_info.simulated_solid);
    simulated.matrix =
      glm::dmat4{glm::inverse(m_scene_info.simulated_camera->get_view()) *
                 glm::inverse(m_scene_info.simulated_camera->get_projection())};
    render_solid(simulated);
    render_solid(get_camera_model(
      *(PerspectiveCamera *)(m_scene_info.simulated_camera.get())));
    render_solid(Solid::Axis());
  } else {
    render_solid(m_scene_info.simulated_solid);
  }
  // HACK: End
  p_texture->bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(m_panel_width),
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
      m_mouse_pos_x = 0.0;
      m_mouse_pos_y = 0.0;
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
      m_scene_info.active_camera->move_forward(0.02);
    }
    key = p_window->get_key(GLFW_KEY_S);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->move_backward(0.02);
    }
    key = p_window->get_key(GLFW_KEY_A);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->move_left(0.02);
    }
    key = p_window->get_key(GLFW_KEY_D);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->move_right(0.02);
    }
    key = p_window->get_key(GLFW_KEY_SPACE);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->move_up(0.02);
    }
    key = p_window->get_key(GLFW_KEY_LEFT_CONTROL);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->move_down(0.02);
    }
    key = p_window->get_key(GLFW_KEY_Q);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->rotate_left(0.01);
    }
    key = p_window->get_key(GLFW_KEY_E);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->rotate_right(0.01);
    }
    key = p_window->get_key(GLFW_KEY_Y);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->rotate_up(0.01);
    }
    key = p_window->get_key(GLFW_KEY_Z);
    if (key == GLFW_PRESS || key == GLFW_REPEAT) {
      m_scene_info.active_camera->rotate_down(0.01);
    }
    if (m_mouse_pos_y < 0.0) {
      for (auto i = 0.0; i < -m_mouse_pos_y; ++i) {
        m_scene_info.active_camera->rotate_up(1 / 500.0);
      }
    }
    if (m_mouse_pos_y > 0.0) {
      for (auto i = 0.0; i < m_mouse_pos_y; ++i) {
        m_scene_info.active_camera->rotate_down(1 / 500.0);
      }
    }
    if (m_mouse_pos_x < 0.0) {
      for (auto i = 0.0; i < -m_mouse_pos_x; ++i) {
        m_scene_info.active_camera->rotate_left(1 / 500.0);
      }
    }
    if (m_mouse_pos_x > 0.0) {
      for (auto i = 0.0; i < m_mouse_pos_x; ++i) {
        m_scene_info.active_camera->rotate_right(1 / 500.0);
      }
    }
    m_mouse_pos_x = 0.0;
    m_mouse_pos_y = 0.0;
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
  ImGui::Text("- glfw_window_ptr: %p", (void *)p_window->get_glfw_window_ptr());
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

  ImGui::Begin("Settings");
  {
    auto change = ImGui::Checkbox("Simulate", &m_scene_info.simulate);
    if (change) {
      if (m_scene_info.simulate) {
        m_scene_info.active_camera = m_scene_info.camera.get();
        m_scene_info.active_camera->set_width(m_panel_width);
        m_scene_info.active_camera->set_height(m_panel_height);
      } else {
        m_scene_info.active_camera = m_scene_info.simulated_camera.get();
        m_scene_info.active_camera->set_width(m_panel_width);
        m_scene_info.active_camera->set_height(m_panel_height);
      }
    }
  }
  if (ImGui::CollapsingHeader("Render triangle pipeline")) {
    {
      enum class ClipFast { CLIP_FAST_TRIANGLE, CLIP_FAST_NONE };
      constexpr std::array<const char *, 2> clip_fast_text = {
        "clip_fast_triangle", "clip_fast_none"};
      static int clip_fast{static_cast<int>(ClipFast::CLIP_FAST_TRIANGLE)};
      auto change = ImGui::Combo("Clip Fast", &clip_fast, clip_fast_text.data(),
                                 static_cast<int>(clip_fast_text.size()));
      if (change) {
        switch (static_cast<ClipFast>(clip_fast)) {
        case ClipFast::CLIP_FAST_TRIANGLE: {
          m_scene_info.render_triangle_pipeline.clip_fast =
            Alg::clip_fast_triangle;
        } break;
        case ClipFast::CLIP_FAST_NONE: {
          m_scene_info.render_triangle_pipeline.clip_fast = Alg::clip_fast_none;
        } break;
        }
      }
    }
    {
      enum class ClipBeforeDehomog {
        CLIP_BEFORE_DEHOMOG_TRIANGLE,
        CLIP_BEFORE_DEHOMOG_NONE
      };
      constexpr std::array<const char *, 2> clip_before_dehomog_text = {
        "clip_before_dehomog_triangle", "clip_before_dehomog_none"};
      static int clip_before_demohog{
        static_cast<int>(ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_TRIANGLE)};
      auto change =
        ImGui::Combo("Clip before dehomog", &clip_before_demohog,
                     clip_before_dehomog_text.data(),
                     static_cast<int>(clip_before_dehomog_text.size()));
      if (change) {
        switch (static_cast<ClipBeforeDehomog>(clip_before_demohog)) {
        case ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_TRIANGLE: {
          m_scene_info.render_triangle_pipeline.clip_before_dehomog =
            Alg::clip_before_dehomog_triangle;
        } break;
        case ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_NONE: {
          m_scene_info.render_triangle_pipeline.clip_before_dehomog =
            Alg::clip_before_dehomog_none;
        } break;
        }
      }
    }
    {
      enum class Dehomog { DEHOMOG_ALL, DEHOMOG_POS };
      constexpr std::array<const char *, 2> dehomog_text = {"dehomog_all",
                                                            "dehomog_pos"};
      static int dehomog{static_cast<int>(Dehomog::DEHOMOG_ALL)};
      auto change = ImGui::Combo("Dehomog", &dehomog, dehomog_text.data(),
                                 static_cast<int>(dehomog_text.size()));
      if (change) {
        switch (static_cast<Dehomog>(dehomog)) {
        case Dehomog::DEHOMOG_ALL: {
          m_scene_info.render_triangle_pipeline.dehomog = Alg::dehomog_all;
        } break;
        case Dehomog::DEHOMOG_POS: {
          m_scene_info.render_triangle_pipeline.dehomog = Alg::dehomog_pos;
        } break;
        }
      }
    }
    {
      enum class SetPixel {
        SET_PIXEL_RGBA_DEPTH,
        SET_PIXEL_RGBA_NO_DEPTH,
        SET_PIXEL_W_DEPTH,
        SET_PIXEL_W_NO_DEPTH
      };
      constexpr std::array<const char *, 4> set_pixel_text = {
        "set_pixel_rgba_depth", "set_pixel_rgba_no_depth", "set_pixel_w_depth",
        "set_pixel_w_no_depth"};
      static int set_pixel{static_cast<int>(SetPixel::SET_PIXEL_RGBA_DEPTH)};
      auto change = ImGui::Combo("Set Pixel", &set_pixel, set_pixel_text.data(),
                                 static_cast<int>(set_pixel_text.size()));
      if (change) {
        switch (static_cast<SetPixel>(set_pixel)) {
        case SetPixel::SET_PIXEL_RGBA_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel =
            Alg::set_pixel_rgba_depth;
        } break;
        case SetPixel::SET_PIXEL_RGBA_NO_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel =
            Alg::set_pixel_rgba_no_depth;
        } break;
        case SetPixel::SET_PIXEL_W_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel =
            Alg::set_pixel_w_depth;
        } break;
        case SetPixel::SET_PIXEL_W_NO_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel =
            Alg::set_pixel_w_no_depth;
        } break;
        }
      }
    }
  }
  if (ImGui::CollapsingHeader("Render line pipeline")) {
    {
      enum class ClipFast { CLIP_FAST_LINE, CLIP_FAST_NONE };
      constexpr std::array<const char *, 2> clip_fast_text = {"clip_fast_line",
                                                              "clip_fast_none"};
      static int clip_fast{static_cast<int>(ClipFast::CLIP_FAST_LINE)};
      auto change = ImGui::Combo("Clip Fast", &clip_fast, clip_fast_text.data(),
                                 static_cast<int>(clip_fast_text.size()));
      if (change) {
        switch (static_cast<ClipFast>(clip_fast)) {
        case ClipFast::CLIP_FAST_LINE: {
          m_scene_info.render_line_pipeline.clip_fast = Alg::clip_fast_line;
        } break;
        case ClipFast::CLIP_FAST_NONE: {
          m_scene_info.render_line_pipeline.clip_fast = Alg::clip_fast_none;
        } break;
        }
      }
    }
    {
      enum class Dehomog { DEHOMOG_ALL, DEHOMOG_POS };
      constexpr std::array<const char *, 2> dehomog_text = {"dehomog_all",
                                                            "dehomog_pos"};
      static int dehomog{static_cast<int>(Dehomog::DEHOMOG_ALL)};
      auto change = ImGui::Combo("Dehomog", &dehomog, dehomog_text.data(),
                                 static_cast<int>(dehomog_text.size()));
      if (change) {
        switch (static_cast<Dehomog>(dehomog)) {
        case Dehomog::DEHOMOG_ALL: {
          m_scene_info.render_line_pipeline.dehomog = Alg::dehomog_all;
        } break;
        case Dehomog::DEHOMOG_POS: {
          m_scene_info.render_line_pipeline.dehomog = Alg::dehomog_pos;
        } break;
        }
      }
    }
    {
      enum class SetPixel {
        SET_PIXEL_RGBA_DEPTH,
        SET_PIXEL_RGBA_NO_DEPTH,
        SET_PIXEL_W_DEPTH,
        SET_PIXEL_W_NO_DEPTH
      };
      constexpr std::array<const char *, 4> set_pixel_text = {
        "set_pixel_rgba_depth", "set_pixel_rgba_no_depth", "set_pixel_w_depth",
        "set_pixel_w_no_depth"};
      static int set_pixel{static_cast<int>(SetPixel::SET_PIXEL_RGBA_DEPTH)};
      auto change = ImGui::Combo("Set Pixel", &set_pixel, set_pixel_text.data(),
                                 static_cast<int>(set_pixel_text.size()));
      if (change) {
        switch (static_cast<SetPixel>(set_pixel)) {
        case SetPixel::SET_PIXEL_RGBA_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel =
            Alg::set_pixel_rgba_depth;
        } break;
        case SetPixel::SET_PIXEL_RGBA_NO_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel =
            Alg::set_pixel_rgba_no_depth;
        } break;
        case SetPixel::SET_PIXEL_W_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel = Alg::set_pixel_w_depth;
        } break;
        case SetPixel::SET_PIXEL_W_NO_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel =
            Alg::set_pixel_w_no_depth;
        } break;
        }
      }
    }
  }
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
