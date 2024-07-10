#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace Vis {

class Camera {
public:
  Camera() = default;
  ~Camera() = default;
  auto get_projection() const -> glm::dmat4;
  auto get_view() const -> glm::dmat4;
  auto move_backward(const double distance) -> void;
  auto move_down(const double distance) -> void;
  auto move_forward(const double distance) -> void;
  auto move_left(const double distance) -> void;
  auto move_right(const double distance) -> void;
  auto move_up(const double distance) -> void;
  auto rotate_down(const double angle) -> void;
  auto rotate_left(const double angle) -> void;
  auto rotate_right(const double angle) -> void;
  auto rotate_up(const double angle) -> void;

public:
  double width{1};
  double height{1};
  double far_plane{10.0};
  double fov{1.5};
  double near_plane{1.0};
  glm::dvec3 direction{1.0, 0.0, 0.0};
  glm::dvec3 position{0.0, 0.0, 0.0};
  glm::dvec3 up{0.0, 0.0, 1.0};
};
} // namespace VIS
