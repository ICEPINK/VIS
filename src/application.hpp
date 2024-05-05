#pragma once

#include "gui.hpp"
#include "image.hpp"
#include "pipeline.hpp"
#include "solids/solid.hpp"
#include "texture.hpp"
#include "window.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <string_view>
#include <vector>

namespace Vis {

// HACK: {
class Camera {
  public:
    virtual ~Camera() = default;
    virtual auto get_view() -> glm::dmat4 = 0;
    virtual auto get_projection() -> glm::dmat4 = 0;

    // TODO: virtual moves for Camera
    // virtual auto move_forward() -> void = 0;
    // virtual auto move_back() -> void = 0;
    // virtual auto move_left() -> void = 0;
    // virtual auto move_right() -> void = 0;
    // virtual auto move_up() -> void = 0;
    // virtual auto move_down() -> void = 0;
};

struct PerspectiveCameraInfo {
    double aspect_ratio{1.0};
    double far_plane{10.0};
    double fov{1.5};
    double near_plane{1.0};
    glm::dvec3 direction{1.0, 0.0, 0.0};
    glm::dvec3 position{0.0, 0.0, 0.0};
    glm::dvec3 up{0.0, 0.0, 1.0};
};

class PerspectiveCamera : public Camera {
  public:
    PerspectiveCamera(const PerspectiveCameraInfo &info) : m_info(info) {}
    ~PerspectiveCamera() = default;
    auto get_view() -> glm::dmat4 override {
        // WARN: Beware of direction
        return glm::lookAt(m_info.position, m_info.direction, m_info.up);
    }
    auto get_projection() -> glm::dmat4 override {
        return glm::perspective(m_info.fov, m_info.aspect_ratio,
                                m_info.near_plane, m_info.far_plane);
    }
    auto set_aspect_ratio(double aspect_ratio) -> void {
        m_info.aspect_ratio = aspect_ratio;
    };

  private:
    PerspectiveCameraInfo m_info;
};

// TODO: Orthogonal Camera
// class OrthogonalCamera : public Camera {};

struct SceneInfo {
    Solid simulated_solid{Solid::Cube()};
    bool render_axis{true};
    bool render_grid{true};
    glm::dmat4 model_matrix{1.0};
    glm::dmat4 simulated_model_matrix{1.0};
    std::unique_ptr<Camera> camera{nullptr};
    std::unique_ptr<Camera> simulated_camera{nullptr};
    Pipeline render_triangle_pipeline{};
    Pipeline render_line_pipeline{};
    Pipeline render_point_pipeline{};
};
// HACK: }

class Application {
  public:
    Application(const std::vector<std::string_view> &args = {});
    ~Application() = default;

    auto run() -> void;
    [[nodiscard]] auto exit() const -> bool;

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
    bool m_exit{false};
    double m_mouse_pos_x{0.0};
    double m_mouse_pos_y{0.0};
    Image m_image{};
    double m_last_loop_time{0};
    SceneInfo m_scene_info{};
    double test_blue{0.0};
};

} // namespace Vis
