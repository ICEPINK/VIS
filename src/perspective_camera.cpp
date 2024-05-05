#include "camera.hpp"
namespace Vis {
PerspectiveCamera::PerspectiveCamera(const PerspectiveCameraInfo &info)
    : m_info(info) {}
auto PerspectiveCamera::get_view() const -> glm::dmat4 {
    return glm::lookAt(m_info.position, m_info.direction + m_info.position,
                       m_info.up);
}
auto PerspectiveCamera::get_projection() const -> glm::dmat4 {
    return glm::perspective(m_info.fov, m_info.width / m_info.height,
                            m_info.near_plane, m_info.far_plane);
}
auto PerspectiveCamera::set_width(const double widht) -> void {
    m_info.width = widht;
}
auto PerspectiveCamera::set_height(const double height) -> void {
    m_info.height = height;
}
auto PerspectiveCamera::move_forward(const double distance) -> void {
    m_info.position = m_info.position + m_info.direction * distance;
}
auto PerspectiveCamera::move_backward(const double distance) -> void {
    m_info.position = m_info.position - m_info.direction * distance;
}
auto PerspectiveCamera::move_left(const double distance) -> void {
    m_info.position =
        m_info.position -
        glm::normalize(glm::cross(m_info.direction, m_info.up)) * distance;
}
auto PerspectiveCamera::move_right(const double distance) -> void {
    m_info.position =
        m_info.position +
        glm::normalize(glm::cross(m_info.direction, m_info.up)) * distance;
}
auto PerspectiveCamera::move_up(const double distance) -> void {
    m_info.position = m_info.position + m_info.up * distance;
}
auto PerspectiveCamera::move_down(const double distance) -> void {
    m_info.position = m_info.position - m_info.up * distance;
}
auto PerspectiveCamera::rotate_up(const double angle) -> void {
    glm::dquat rotation = glm::angleAxis(
        angle, glm::normalize(glm::cross(m_info.direction, m_info.up)));
    auto new_direction = rotation * m_info.direction;
    // WARN: Beware of big angles
    if (new_direction.z < 0.99999 && new_direction.z > -0.99999) {
        m_info.direction = new_direction;
    }
}
auto PerspectiveCamera::rotate_down(const double angle) -> void {
    glm::dquat rotation = glm::angleAxis(
        -angle, glm::normalize(glm::cross(m_info.direction, m_info.up)));
    auto new_direction = rotation * m_info.direction;
    // WARN: Beware of big angles
    if (new_direction.z < 0.99999 && new_direction.z > -0.99999) {
        m_info.direction = new_direction;
    }
}
auto PerspectiveCamera::rotate_left(const double angle) -> void {
    glm::dquat rotation = glm::angleAxis(angle, m_info.up);
    m_info.direction = rotation * m_info.direction;
}
auto PerspectiveCamera::rotate_right(const double angle) -> void {
    glm::dquat rotation = glm::angleAxis(-angle, m_info.up);
    m_info.direction = rotation * m_info.direction;
}
} // namespace Vis
