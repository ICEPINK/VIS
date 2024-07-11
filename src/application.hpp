#pragma once
// src includes
#include "camera.hpp"
#include "gui.hpp"
#include "image.hpp"
#include "pipeline.hpp"
#include "solid.hpp"
#include "texture.hpp"
#include "window.hpp"
// lib includes
#include <glm/ext.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
// std includes
#include <string_view>
#include <vector>
namespace Vis {
enum class SceneSpace { SolidModel, SceneModel, View, Projection };
enum class AddToNewSolid { False, True };
struct SceneInfo {
  Solid simulated_solid{Solid::Cube()};
  SceneSpace scene_space{SceneSpace::SceneModel};
  bool render_axis{true};
  bool render_grid{false};
  bool simulate{false};
  glm::dmat4 model_matrix{1.0};
  glm::dmat4 simulated_model_matrix{1.0};
  std::unique_ptr<Camera> render_camera{nullptr};
  std::unique_ptr<Camera> simulated_camera{nullptr};
  Camera *active_camera{nullptr};
  Pipeline render_triangle_pipeline{
      .clip_after_dehomog = Alg::clip_after_dehomog_triangle,
      .clip_before_dehomog = Alg::clip_before_dehomog_triangle,
      .clip_fast = Alg::clip_fast_triangle,
      .dehomog = Alg::dehomog_all,
      .rasterize = Alg::rasterize_triangle,
      .set_pixel = Alg::set_pixel_rgba_depth,
      .trasform_to_viewport = Alg::trasform_to_viewport,
      .trasform_vertices = Alg::trasform_vertices_by_matrix,
  };
  Pipeline render_line_pipeline{
      .clip_after_dehomog = Alg::clip_after_dehomog_line,
      .clip_before_dehomog = Alg::clip_before_dehomog_line,
      .clip_fast = Alg::clip_fast_line,
      .dehomog = Alg::dehomog_all,
      .rasterize = Alg::rasterize_line,
      .set_pixel = Alg::set_pixel_rgba_depth,
      .trasform_to_viewport = Alg::trasform_to_viewport,
      .trasform_vertices = Alg::trasform_vertices_by_matrix,
  };
  Pipeline render_point_pipeline{
      .clip_after_dehomog = Alg::clip_after_dehomog_none,
      .clip_before_dehomog = Alg::clip_before_dehomog_none,
      .clip_fast = Alg::clip_fast_point,
      .dehomog = Alg::dehomog_all,
      .rasterize = Alg::rasterize_point,
      .set_pixel = Alg::set_pixel_rgba_depth,
      .trasform_to_viewport = Alg::trasform_to_viewport,
      .trasform_vertices = Alg::trasform_vertices_by_matrix,
  };
  Pipeline simulate_triangle_pipeline{
      .clip_after_dehomog = Alg::clip_after_dehomog_triangle,
      .clip_before_dehomog = Alg::clip_before_dehomog_triangle,
      .clip_fast = Alg::clip_fast_triangle,
      .dehomog = Alg::dehomog_all,
      .rasterize = Alg::rasterize_none,
      .set_pixel = Alg::set_pixel_none,
      .trasform_to_viewport = Alg::trasform_to_none,
      .trasform_vertices = Alg::trasform_vertices_by_matrix,
  };
  Pipeline simulate_line_pipeline{};
  Pipeline simulate_point_pipeline{};
};
class Application {
public:
  Application(const std::vector<std::string_view> &args = {});
  ~Application() = default;
  auto run() -> void;

private:
  [[nodiscard]] auto
  handle_args(const std::vector<std::string_view> &args) -> bool;
  auto arg_print_help() -> bool;
  auto arg_print_version() -> bool;
  auto arg_resolution(std::string_view resolution) -> void;
  auto make_gui(bool show_debug = false) -> void;
  auto handle_input() -> void;
  auto render_image() -> void;
  auto render_solid(const Solid &solid) -> void;
  auto render(std::vector<Vertex> &vertices, const Pipeline &pipeline,
              const glm::dmat4 &matrix) -> void;
  [[nodiscard]] auto simulate_solid(const Solid &solid) -> Solid;
  template <size_t vertices_per_primitie, AddToNewSolid add_to_new_solid>
  auto render_topology(const Layout &layout, const Solid &solid,
                       const Pipeline &pipeline, const glm::dmat4 &matrix,
                       Solid *new_solid = nullptr) -> void;

private:
  std::shared_ptr<Glfw> p_glfw{nullptr};
  std::unique_ptr<Window> p_window{nullptr};
  std::unique_ptr<Gui> p_gui{nullptr};
  std::unique_ptr<Texture> p_texture{nullptr};
  size_t m_width{800};
  size_t m_height{600};
  float m_panel_width{0.0f};
  float m_panel_height{0.0f};
  std::string m_title{"VIS"};
  bool m_alt_mode{false};
  double m_mouse_pos_x{0.0};
  double m_mouse_pos_y{0.0};
  Image m_image{};
  double m_last_loop_time{0};
  SceneInfo m_scene_info{};
  double test_blue{0.0};
};

} // namespace Vis
