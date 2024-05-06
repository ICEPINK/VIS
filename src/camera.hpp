#pragma once
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>
namespace Vis {
class Camera {
public:
  virtual ~Camera() = default;
  virtual auto get_view() const -> glm::dmat4 = 0;
  virtual auto get_projection() const -> glm::dmat4 = 0;
  virtual auto set_width(const double widht) -> void = 0;
  virtual auto set_height(const double height) -> void = 0;
  virtual auto move_forward(const double distance) -> void = 0;
  virtual auto move_backward(const double distance) -> void = 0;
  virtual auto move_left(const double distance) -> void = 0;
  virtual auto move_right(const double distance) -> void = 0;
  virtual auto move_up(const double distance) -> void = 0;
  virtual auto move_down(const double distance) -> void = 0;
  virtual auto rotate_up(const double angle) -> void = 0;
  virtual auto rotate_down(const double angle) -> void = 0;
  virtual auto rotate_left(const double angle) -> void = 0;
  virtual auto rotate_right(const double angle) -> void = 0;
};
struct PerspectiveCameraInfo {
  double width{1};
  double height{1};
  double far_plane{10.0};
  double fov{1.5};
  double near_plane{1.0};
  glm::dvec3 direction{1.0, 0.0, 0.0};
  glm::dvec3 position{0.0, 0.0, 0.0};
  glm::dvec3 up{0.0, 0.0, 1.0};
};
class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(const PerspectiveCameraInfo &info);
  ~PerspectiveCamera() = default;
  auto get_view() const -> glm::dmat4 override;
  auto get_projection() const -> glm::dmat4 override;
  auto set_width(const double widht) -> void override;
  auto set_height(const double height) -> void override;
  auto move_forward(const double distance) -> void override;
  auto move_backward(const double distance) -> void override;
  auto move_left(const double distance) -> void override;
  auto move_right(const double distance) -> void override;
  auto move_up(const double distance) -> void override;
  auto move_down(const double distance) -> void override;
  auto rotate_up(const double angle) -> void override;
  auto rotate_down(const double angle) -> void override;
  auto rotate_left(const double angle) -> void override;
  auto rotate_right(const double angle) -> void override;

private:
  PerspectiveCameraInfo m_info;
};
// TODO: Orthogonal Camera
// class OrthogonalCamera : public Camera {};
} // namespace Vis
