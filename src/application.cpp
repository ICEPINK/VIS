#include "application.hpp"

#include "glad.hpp"
#include "timer.hpp"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <array>
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
  const WindowInfo window_info{m_width, m_height, m_title};
  p_window = std::make_unique<Window>(window_info, p_glfw);
  p_window->make_context_current();
  Glad::load_gl_loader((GLADloadproc)p_glfw->get_proc_address());
  glDebugMessageCallback(Glad::print_gl_message, nullptr);
  std::cout << "OpenGL: Version " << glGetString(GL_VERSION) << '\n';
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
  m_scene_info.simulated_camera = std::make_unique<Camera>();
  m_scene_info.simulated_camera->width = static_cast<double>(m_width);
  m_scene_info.simulated_camera->height = static_cast<double>(m_height);
  m_scene_info.simulated_camera->position = {-2.0, 0.0, 0.0};
  m_scene_info.simulated_camera->near_plane = 1.0;
  m_scene_info.simulated_camera->far_plane = 10.0;
  m_scene_info.render_camera = std::make_unique<Camera>();
  m_scene_info.render_camera->width = static_cast<double>(m_width);
  m_scene_info.render_camera->height = static_cast<double>(m_height);
  m_scene_info.render_camera->position = {-1.0, 0.0, 0.0};
  m_scene_info.render_camera->near_plane = 0.1;
  m_scene_info.render_camera->far_plane = 100.0;
  m_scene_info.active_camera = m_scene_info.simulated_camera.get();
  m_scene_info.simulated_solid.matrix = glm::translate(glm::dmat4{1.0}, {3.0, 0.0, 0.0});
  run();
}

[[nodiscard]] auto Application::handle_args(const std::vector<std::string_view> &args) -> bool {
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
    make_gui();
    render_image();

    p_gui->render();
    p_window->swap_buffers();
  }
}

auto Application::render(std::vector<Vertex> &vertices, const Pipeline &pipeline, const glm::dmat4 &matrix) -> void {
  pipeline.trasform_vertices(vertices, matrix);
  pipeline.clip_fast(vertices);
  pipeline.clip_before_dehomog(vertices);
  pipeline.dehomog(vertices);
  pipeline.clip_after_dehomog(vertices);
  pipeline.trasform_to_viewport(vertices, m_image);
  pipeline.rasterize(vertices, m_image, pipeline.set_pixel);
}

template <size_t vertices_per_primitie, AddToNewSolid add_to_new_solid> auto Application::render_topology(const Layout &layout, const Solid &solid, const Pipeline &pipeline, const glm::dmat4 &matrix, Solid *new_solid) -> void {
  std::vector<Vertex> primitive;
  primitive.reserve(vertices_per_primitie);
  for (size_t i = layout.start; i < layout.start + layout.count * vertices_per_primitie; i += vertices_per_primitie) {
    primitive.clear();
    for (size_t j = 0; j < vertices_per_primitie; ++j) {
      primitive.push_back(solid.vertices[solid.indices[i + j]]);
    }
    render(primitive, pipeline, matrix);
    if constexpr (add_to_new_solid == AddToNewSolid::True) {
      const size_t new_size = new_solid->vertices.size() + primitive.size();
      new_solid->vertices.reserve(new_size);
      new_solid->indices.reserve(new_size);
      if (primitive.size() % vertices_per_primitie == 0) {
        if (new_solid->layout.size() > 0 && new_solid->layout.back().topology == layout.topology) {
          for (size_t j = 0; j < primitive.size(); j += vertices_per_primitie) {
            for (size_t k = 0; k < vertices_per_primitie; ++k) {
              new_solid->vertices.push_back(primitive[j + k]);
              new_solid->indices.push_back(new_solid->indices.size());
            }
            ++new_solid->layout.back().count;
          }
        } else {
          new_solid->layout.push_back({layout.topology, new_solid->vertices.size(), 0});
          for (size_t j = 0; j < primitive.size(); j += vertices_per_primitie) {
            for (size_t k = 0; k < vertices_per_primitie; ++k) {
              new_solid->vertices.push_back(primitive[j + k]);
              new_solid->indices.push_back(new_solid->indices.size());
            }
            ++new_solid->layout.back().count;
          }
        }
      }
    }
  }
}

auto Application::simulate_solid(const Solid &solid) -> Solid {
  auto matrix = m_scene_info.simulated_camera->get_projection() * m_scene_info.simulated_camera->get_view() * m_scene_info.simulated_model_matrix * solid.matrix;
  Solid new_solid;
  new_solid.name = solid.name;
  new_solid.matrix = glm::dmat4{1.0};
  for (const auto layout : solid.layout) {
    switch (layout.topology) {
    case Topology::Point: {
      render_topology<1, AddToNewSolid::True>(layout, solid, m_scene_info.simulate_point_pipeline, matrix, &new_solid);
    } break;
    case Topology::Line: {
      render_topology<2, AddToNewSolid::True>(layout, solid, m_scene_info.simulate_line_pipeline, matrix, &new_solid);
    } break;
    case Topology::Triangle: {
      render_topology<3, AddToNewSolid::True>(layout, solid, m_scene_info.simulate_triangle_pipeline, matrix, &new_solid);
    } break;
    }
  }
  return new_solid;
}

auto get_camera_model(const Camera &camera) -> Solid {
  Solid solid{};
  solid.name = "";
  solid.matrix = glm::dmat4{1.0};
  solid.vertices.reserve(9);
  glm::dvec4 color = {1.0, 1.0, 1.0, 1.0};
  Vertex v1{};
  v1.pos = {camera.position.x, camera.position.y, camera.position.z, 1.0};
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
    vertices[i].pos = glm::inverse(camera.get_view()) * glm::inverse(camera.get_projection()) * vertices[i].pos;
    solid.vertices.push_back(vertices[i]);
  }
  solid.indices = {0, 5, 0, 6, 0, 7, 0, 8, 1, 2, 2, 4, 4, 3, 3, 1, 5, 6, 6, 8, 8, 7, 7, 5};
  solid.layout.push_back({Topology::Line, 0, 12});
  return solid;
};

auto Application::render_solid(const Solid &solid) -> void {
  auto matrix = m_scene_info.active_camera->get_projection() * m_scene_info.active_camera->get_view() * m_scene_info.model_matrix * solid.matrix;
  for (const auto layout : solid.layout) {
    switch (layout.topology) {
    case Topology::Point: {
      render_topology<1, AddToNewSolid::False>(layout, solid, m_scene_info.render_point_pipeline, matrix);
    } break;
    case Topology::Line: {
      render_topology<2, AddToNewSolid::False>(layout, solid, m_scene_info.render_line_pipeline, matrix);
    } break;
    case Topology::Triangle: {
      render_topology<3, AddToNewSolid::False>(layout, solid, m_scene_info.render_triangle_pipeline, matrix);
    } break;
    }
  }
}

auto Application::render_image() -> void {
  if (m_panel_width != m_image.get_width() || m_panel_height != m_image.get_height()) {
    m_image.resize(static_cast<size_t>(m_panel_width), static_cast<size_t>(m_panel_height));
    m_scene_info.active_camera->width = m_panel_width;
    m_scene_info.active_camera->height = m_panel_height;
  }
  m_image.clear({0.05, 0.05, 0.05, 1.0});
  if (m_scene_info.simulate) {
    Solid simulated = simulate_solid(m_scene_info.simulated_solid);
    glm::dmat4 scene_matrix = {1.0};
    Solid camera_solid = get_camera_model(*m_scene_info.simulated_camera.get());
    switch (m_scene_info.scene_space) {
    case SceneSpace::SolidModel: {
      simulated.matrix = glm::dmat4{glm::inverse(m_scene_info.simulated_solid.matrix) * glm::inverse(m_scene_info.simulated_camera->get_view()) * glm::inverse(m_scene_info.simulated_camera->get_projection())};
    } break;
    case SceneSpace::SceneModel: {
      simulated.matrix = glm::dmat4{glm::inverse(m_scene_info.simulated_camera->get_view()) * glm::inverse(m_scene_info.simulated_camera->get_projection())};
    } break;
    case SceneSpace::View: {
      simulated.matrix = glm::dmat4{glm::inverse(m_scene_info.simulated_camera->get_projection())};
      scene_matrix = {0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
      camera_solid.matrix = m_scene_info.simulated_camera->get_view();
    } break;
    case SceneSpace::Projection: {
      scene_matrix = {0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
      camera_solid.indices[0] = 1;
      camera_solid.indices[2] = 2;
      camera_solid.indices[4] = 3;
      camera_solid.indices[6] = 4;
      camera_solid.matrix = m_scene_info.simulated_camera->get_projection() * m_scene_info.simulated_camera->get_view();
    } break;
    }
    std::swap(scene_matrix, m_scene_info.model_matrix);
    render_solid(simulated);
    if (m_scene_info.scene_space != SceneSpace::SolidModel) {
      render_solid(camera_solid);
    }
    if (m_scene_info.render_axis) {
      render_solid(Solid::Axis());
    }
    std::swap(scene_matrix, m_scene_info.model_matrix);
  } else {
    render_solid(m_scene_info.simulated_solid);
  }
  p_texture->bind();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(m_panel_width), static_cast<GLsizei>(m_panel_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.get_image_data());
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
      if (ImGui::MenuItem("Quit")) {
        p_window->set_should_close(true);
      }
      if (ImGui::MenuItem("Reset camera")) {
        m_scene_info.active_camera->position = {-2.0, 0.0, 0.0};
        m_scene_info.active_camera->direction = {1.0, 0.0, 0.0};
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
  // ImGui::Text("p_glfw: %p", (void *)p_glfw.get());
  // ImGui::Text("p_window: %p", (void *)p_window.get());
  // ImGui::Text("- glfw_window_ptr: %p", (void *)p_window->get_glfw_window_ptr());
  // ImGui::Text("p_gui: %p", (void *)p_gui.get());
  // ImGui::Text("p_texture: %p", (void *)p_texture.get());
  // ImGui::Text("- opengl_id: %u", p_texture->get_id());
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
  ImGui::End();

  ImGui::Begin("Settings");
  {
    auto change = ImGui::Checkbox("Simulate", &m_scene_info.simulate);
    if (change) {
      if (m_scene_info.simulate) {
        m_scene_info.active_camera = m_scene_info.render_camera.get();
        m_scene_info.active_camera->width = m_panel_width;
        m_scene_info.active_camera->height = m_panel_height;
      } else {
        m_scene_info.active_camera = m_scene_info.simulated_camera.get();
        m_scene_info.active_camera->width = m_panel_width;
        m_scene_info.active_camera->height = m_panel_height;
      }
    }
  }
  if (m_scene_info.simulate) {
    ImGui::Checkbox("Render axis", &m_scene_info.render_axis);
  }
  {

    enum class Solids { Triangle, Square, Cube, IcoSphere };
    constexpr std::array<const char *, 4> solids_text = {"triangle", "square", "cube", "icosphere"};
    static int solids{static_cast<int>(Solids::Cube)};
    auto change = ImGui::Combo("Solids##1", &solids, solids_text.data(), static_cast<int>(solids_text.size()));
    if (change) {
      switch (static_cast<Solids>(solids)) {
      case Solids::Triangle: {
        m_scene_info.simulated_solid = Solid::Triangle();
      } break;
      case Solids::Square: {
        m_scene_info.simulated_solid = Solid::Square();
      } break;
      case Solids::Cube: {
        m_scene_info.simulated_solid = Solid::Cube();
      } break;
      case Solids::IcoSphere: {
        m_scene_info.simulated_solid = Solid::Icosphere();
      } break;
      }
    }
  }
  if (m_scene_info.simulate) {
    {
      constexpr std::array<const char *, 4> scene_space_text = {"Solid", "Scene", "View", "Proj"};
      static int scene_space{static_cast<int>(SceneSpace::SceneModel)};
      auto change = ImGui::Combo("Scene Space##1", &scene_space, scene_space_text.data(), static_cast<int>(scene_space_text.size()));
      if (change) {
        m_scene_info.scene_space = static_cast<SceneSpace>(scene_space);
      }
    }
  }
  if (ImGui::CollapsingHeader("Render triangle pipeline")) {
    {
      enum class ClipFast { CLIP_FAST_TRIANGLE, CLIP_FAST_NONE };
      constexpr std::array<const char *, 2> clip_fast_text = {"clip_fast_triangle", "clip_fast_none"};
      static int clip_fast{static_cast<int>(ClipFast::CLIP_FAST_TRIANGLE)};
      auto change = ImGui::Combo("Clip Fast##1", &clip_fast, clip_fast_text.data(), static_cast<int>(clip_fast_text.size()));
      if (change) {
        switch (static_cast<ClipFast>(clip_fast)) {
        case ClipFast::CLIP_FAST_TRIANGLE: {
          m_scene_info.render_triangle_pipeline.clip_fast = Alg::clip_fast_triangle;
        } break;
        case ClipFast::CLIP_FAST_NONE: {
          m_scene_info.render_triangle_pipeline.clip_fast = Alg::clip_fast_none;
        } break;
        }
      }
    }
    {
      enum class ClipBeforeDehomog { CLIP_BEFORE_DEHOMOG_TRIANGLE, CLIP_BEFORE_DEHOMOG_NONE };
      constexpr std::array<const char *, 2> clip_before_dehomog_text = {"clip_before_dehomog_triangle", "clip_before_dehomog_none"};
      static int clip_before_demohog{static_cast<int>(ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_TRIANGLE)};
      auto change = ImGui::Combo("Clip before dehomog##1", &clip_before_demohog, clip_before_dehomog_text.data(), static_cast<int>(clip_before_dehomog_text.size()));
      if (change) {
        switch (static_cast<ClipBeforeDehomog>(clip_before_demohog)) {
        case ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_TRIANGLE: {
          m_scene_info.render_triangle_pipeline.clip_before_dehomog = Alg::clip_before_dehomog_triangle;
        } break;
        case ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_NONE: {
          m_scene_info.render_triangle_pipeline.clip_before_dehomog = Alg::clip_before_dehomog_none;
        } break;
        }
      }
    }
    {
      enum class Dehomog { DEHOMOG_ALL, DEHOMOG_POS };
      constexpr std::array<const char *, 2> dehomog_text = {"dehomog_all", "dehomog_pos"};
      static int dehomog{static_cast<int>(Dehomog::DEHOMOG_ALL)};
      auto change = ImGui::Combo("Dehomog##1", &dehomog, dehomog_text.data(), static_cast<int>(dehomog_text.size()));
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
      enum class ClipAfterDehomog { CLIP_AFTER_DEHOMOG_TRIANGLE, CLIP_AFTER_DEHOMOG_NONE };
      constexpr std::array<const char *, 2> clip_after_dehomog_text = {"clip_after_dehomog_triangle", "clip_after_dehomog_none"};
      static int clip_after_demohog{static_cast<int>(ClipAfterDehomog::CLIP_AFTER_DEHOMOG_TRIANGLE)};
      auto change = ImGui::Combo("Clip after dehomog##1", &clip_after_demohog, clip_after_dehomog_text.data(), static_cast<int>(clip_after_dehomog_text.size()));
      if (change) {
        switch (static_cast<ClipAfterDehomog>(clip_after_demohog)) {
        case ClipAfterDehomog::CLIP_AFTER_DEHOMOG_TRIANGLE: {
          m_scene_info.render_triangle_pipeline.clip_after_dehomog = Alg::clip_after_dehomog_triangle;
        } break;
        case ClipAfterDehomog::CLIP_AFTER_DEHOMOG_NONE: {
          m_scene_info.render_triangle_pipeline.clip_after_dehomog = Alg::clip_after_dehomog_none;
        } break;
        }
      }
    }
    {
      enum class RasterizeTriangle { RasterizeNone, RasterizeTriangle, RasterizeTriangleAsLines };
      constexpr std::array<const char *, 3> rasterize_triangle_text = {"rasterize_none", "rasterize_triangle", "rasterize_triangle_as_lines"};
      static int rasterize_triangle{static_cast<int>(RasterizeTriangle::RasterizeTriangle)};
      auto change = ImGui::Combo("Rasterize triangle##1", &rasterize_triangle, rasterize_triangle_text.data(), static_cast<int>(rasterize_triangle_text.size()));
      if (change) {
        switch (static_cast<RasterizeTriangle>(rasterize_triangle)) {
        case RasterizeTriangle::RasterizeNone: {
          m_scene_info.render_triangle_pipeline.rasterize = Alg::rasterize_none;
        } break;
        case RasterizeTriangle::RasterizeTriangle: {
          m_scene_info.render_triangle_pipeline.rasterize = Alg::rasterize_triangle;
        } break;
        case RasterizeTriangle::RasterizeTriangleAsLines: {
          m_scene_info.render_triangle_pipeline.rasterize = Alg::rasterize_triangle_as_lines;
        } break;
        }
      }
    }
    {
      enum class SetPixel { SET_PIXEL_RGBA_DEPTH, SET_PIXEL_RGBA_NO_DEPTH, SET_PIXEL_Z_DEPTH, SET_PIXEL_Z_NO_DEPTH, SET_PIXEL_TEX, SET_PIXEL_WHITE };
      constexpr std::array<const char *, 6> set_pixel_text = {"set_pixel_rgba_depth", "set_pixel_rgba_no_depth", "set_pixel_z_depth", "set_pixel_z_no_depth", "set_pixel_tex", "set_pixel_white"};
      static int set_pixel{static_cast<int>(SetPixel::SET_PIXEL_RGBA_DEPTH)};
      auto change = ImGui::Combo("Set Pixel##1", &set_pixel, set_pixel_text.data(), static_cast<int>(set_pixel_text.size()));
      if (change) {
        switch (static_cast<SetPixel>(set_pixel)) {
        case SetPixel::SET_PIXEL_RGBA_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_rgba_depth;
        } break;
        case SetPixel::SET_PIXEL_RGBA_NO_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_rgba_no_depth;
        } break;
        case SetPixel::SET_PIXEL_Z_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_z_depth;
        } break;
        case SetPixel::SET_PIXEL_Z_NO_DEPTH: {
          m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_z_no_depth;
        } break;
        case SetPixel::SET_PIXEL_TEX: {
          m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_tex;
        } break;
        case SetPixel::SET_PIXEL_WHITE: {
          m_scene_info.render_triangle_pipeline.set_pixel = Alg::set_pixel_white;
        } break;
        }
      }
    }
  }
  if (ImGui::CollapsingHeader("Render line pipeline")) {
    {
      enum class ClipFast { CLIP_FAST_LINE, CLIP_FAST_NONE };
      constexpr std::array<const char *, 2> clip_fast_text = {"clip_fast_line", "clip_fast_none"};
      static int clip_fast{static_cast<int>(ClipFast::CLIP_FAST_LINE)};
      auto change = ImGui::Combo("Clip Fast##2", &clip_fast, clip_fast_text.data(), static_cast<int>(clip_fast_text.size()));
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
      constexpr std::array<const char *, 2> dehomog_text = {"dehomog_all", "dehomog_pos"};
      static int dehomog{static_cast<int>(Dehomog::DEHOMOG_ALL)};
      auto change = ImGui::Combo("Dehomog##2", &dehomog, dehomog_text.data(), static_cast<int>(dehomog_text.size()));
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
      enum class SetPixel { SET_PIXEL_RGBA_DEPTH, SET_PIXEL_RGBA_NO_DEPTH, SET_PIXEL_Z_DEPTH, SET_PIXEL_Z_NO_DEPTH };
      constexpr std::array<const char *, 4> set_pixel_text = {"set_pixel_rgba_depth", "set_pixel_rgba_no_depth", "set_pixel_z_depth", "set_pixel_z_no_depth"};
      static int set_pixel{static_cast<int>(SetPixel::SET_PIXEL_RGBA_DEPTH)};
      auto change = ImGui::Combo("Set Pixel##2", &set_pixel, set_pixel_text.data(), static_cast<int>(set_pixel_text.size()));
      if (change) {
        switch (static_cast<SetPixel>(set_pixel)) {
        case SetPixel::SET_PIXEL_RGBA_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel = Alg::set_pixel_rgba_depth;
        } break;
        case SetPixel::SET_PIXEL_RGBA_NO_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel = Alg::set_pixel_rgba_no_depth;
        } break;
        case SetPixel::SET_PIXEL_Z_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel = Alg::set_pixel_z_depth;
        } break;
        case SetPixel::SET_PIXEL_Z_NO_DEPTH: {
          m_scene_info.render_line_pipeline.set_pixel = Alg::set_pixel_z_no_depth;
        } break;
        }
      }
    }
  }
  if (m_scene_info.simulate) {
    if (ImGui::CollapsingHeader("Simulate triangle pipeline")) {
      {
        enum class ClipFast { CLIP_FAST_TRIANGLE, CLIP_FAST_NONE };
        constexpr std::array<const char *, 2> clip_fast_text = {"clip_fast_triangle", "clip_fast_none"};
        static int clip_fast{static_cast<int>(ClipFast::CLIP_FAST_TRIANGLE)};
        auto change = ImGui::Combo("Clip Fast##3", &clip_fast, clip_fast_text.data(), static_cast<int>(clip_fast_text.size()));
        if (change) {
          switch (static_cast<ClipFast>(clip_fast)) {
          case ClipFast::CLIP_FAST_TRIANGLE: {
            m_scene_info.simulate_triangle_pipeline.clip_fast = Alg::clip_fast_triangle;
          } break;
          case ClipFast::CLIP_FAST_NONE: {
            m_scene_info.simulate_triangle_pipeline.clip_fast = Alg::clip_fast_none;
          } break;
          }
        }
      }
      {
        enum class ClipBeforeDehomog { CLIP_BEFORE_DEHOMOG_TRIANGLE, CLIP_BEFORE_DEHOMOG_NONE };
        constexpr std::array<const char *, 2> clip_before_dehomog_text = {"clip_before_dehomog_triangle", "clip_before_dehomog_none"};
        static int clip_before_demohog{static_cast<int>(ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_TRIANGLE)};
        auto change = ImGui::Combo("Clip before dehomog##3", &clip_before_demohog, clip_before_dehomog_text.data(), static_cast<int>(clip_before_dehomog_text.size()));
        if (change) {
          switch (static_cast<ClipBeforeDehomog>(clip_before_demohog)) {
          case ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_TRIANGLE: {
            m_scene_info.simulate_triangle_pipeline.clip_before_dehomog = Alg::clip_before_dehomog_triangle;
          } break;
          case ClipBeforeDehomog::CLIP_BEFORE_DEHOMOG_NONE: {
            m_scene_info.simulate_triangle_pipeline.clip_before_dehomog = Alg::clip_before_dehomog_none;
          } break;
          }
        }
      }
      {
        enum class Dehomog { DEHOMOG_ALL, DEHOMOG_POS };
        constexpr std::array<const char *, 2> dehomog_text = {"dehomog_all", "dehomog_pos"};
        static int dehomog{static_cast<int>(Dehomog::DEHOMOG_ALL)};
        auto change = ImGui::Combo("Dehomog##3", &dehomog, dehomog_text.data(), static_cast<int>(dehomog_text.size()));
        if (change) {
          switch (static_cast<Dehomog>(dehomog)) {
          case Dehomog::DEHOMOG_ALL: {
            m_scene_info.simulate_triangle_pipeline.dehomog = Alg::dehomog_all;
          } break;
          case Dehomog::DEHOMOG_POS: {
            m_scene_info.simulate_triangle_pipeline.dehomog = Alg::dehomog_pos;
          } break;
          }
        }
      }
      {
        enum class ClipAfterDehomog { CLIP_AFTER_DEHOMOG_TRIANGLE, CLIP_AFTER_DEHOMOG_NONE };
        constexpr std::array<const char *, 2> clip_after_dehomog_text = {"clip_after_dehomog_triangle", "clip_after_dehomog_none"};
        static int clip_after_demohog{static_cast<int>(ClipAfterDehomog::CLIP_AFTER_DEHOMOG_TRIANGLE)};
        auto change = ImGui::Combo("Clip after dehomog", &clip_after_demohog, clip_after_dehomog_text.data(), static_cast<int>(clip_after_dehomog_text.size()));
        if (change) {
          switch (static_cast<ClipAfterDehomog>(clip_after_demohog)) {
          case ClipAfterDehomog::CLIP_AFTER_DEHOMOG_TRIANGLE: {
            m_scene_info.simulate_triangle_pipeline.clip_after_dehomog = Alg::clip_after_dehomog_triangle;
          } break;
          case ClipAfterDehomog::CLIP_AFTER_DEHOMOG_NONE: {
            m_scene_info.simulate_triangle_pipeline.clip_after_dehomog = Alg::clip_after_dehomog_none;
          } break;
          }
        }
      }
    }
  }
  if (ImGui::CollapsingHeader("Camera settings")) {
    static float near_plane = static_cast<float>(m_scene_info.simulated_camera->near_plane);
    if (ImGui::SliderFloat("Near plane", &near_plane, 0.1f, 9.9f, "%.3f")) {
      m_scene_info.simulated_camera->near_plane = near_plane;
    }
    static float fov = 360 * static_cast<float>(m_scene_info.simulated_camera->fov) / (glm::pi<float>() * 2);
    if (ImGui::SliderFloat("Field of view", &fov, 0.1f, 179.0f, "%.3f")) {
      m_scene_info.simulated_camera->fov = (fov / 360) * 2 * glm::pi<double>();
    }
  }
  if (ImGui::CollapsingHeader("Solid")) {
      static float vec3[3] = { 0.0f, 0.0f, 0.0f };
      vec3[0] = static_cast<float>(m_scene_info.simulated_solid.matrix[3].x);
      vec3[1] = static_cast<float>(m_scene_info.simulated_solid.matrix[3].y);
      vec3[2] = static_cast<float>(m_scene_info.simulated_solid.matrix[3].z);
      if (ImGui::InputFloat3("Position", vec3)) {
          m_scene_info.simulated_solid.matrix[3].x = static_cast<double>(vec3[0]);
          m_scene_info.simulated_solid.matrix[3].y = static_cast<double>(vec3[1]);
          m_scene_info.simulated_solid.matrix[3].z = static_cast<double>(vec3[2]);
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

  ImGui::Image((ImTextureID)(intptr_t)p_texture->get_id(), ImVec2{m_panel_width, m_panel_height}, ImVec2(0, 1), ImVec2(1, 0));
  ImGui::End();
  ImGui::PopStyleVar();
}
} // namespace Vis
