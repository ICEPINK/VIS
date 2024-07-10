#include "camera.hpp"

namespace Vis {

auto Camera::get_projection() const -> glm::dmat4 { return glm::perspective(fov, width / height, near_plane, far_plane); }

auto Camera::get_view() const -> glm::dmat4 { return glm::lookAt(position, direction + position, up); }

auto Camera::move_backward(const double distance) -> void { position = position - direction * distance; }

auto Camera::move_down(const double distance) -> void { position = position - up * distance; }

auto Camera::move_forward(const double distance) -> void { position = position + direction * distance; }

auto Camera::move_left(const double distance) -> void { position = position - glm::normalize(glm::cross(direction, up)) * distance; }

auto Camera::move_right(const double distance) -> void { position = position + glm::normalize(glm::cross(direction, up)) * distance; }

auto Camera::move_up(const double distance) -> void { position = position + up * distance; }

auto Camera::rotate_down(const double angle) -> void {
  glm::dquat rotation = glm::angleAxis(-angle, glm::normalize(glm::cross(direction, up)));
  auto new_direction = rotation * direction;
  // WARN: Beware of big angles
  if (new_direction.z < 0.999 && new_direction.z > -0.999) {
    direction = new_direction;
  }
}

auto Camera::rotate_left(const double angle) -> void {
  glm::dquat rotation = glm::angleAxis(angle, up);
  direction = rotation * direction;
}

auto Camera::rotate_right(const double angle) -> void {
  glm::dquat rotation = glm::angleAxis(-angle, up);
  direction = rotation * direction;
}

auto Camera::rotate_up(const double angle) -> void {
  glm::dquat rotation = glm::angleAxis(angle, glm::normalize(glm::cross(direction, up)));
  auto new_direction = rotation * direction;
  // WARN: Beware of big angles
  if (new_direction.z < 0.999 && new_direction.z > -0.999) {
    direction = new_direction;
  }
}

} // namespace Vis
