#include "camera.hpp"

#include <glm/ext.hpp>

namespace Vis {

PerspectiveCamera::PerspectiveCamera(const PerspectiveData &data)
    : m_data(data), m_up_direction({0.0f, 0.0f, 1.0f}),
      m_look_direction({1.0f, 0.0f, 0.0f}) {
    // TODO: if rotation
}

void PerspectiveCamera::move_forward(const float distance) {
    const glm::vec3 direction = glm::normalize(calculate_direction());
    m_data.position += (direction * distance);
}

void PerspectiveCamera::move_backward(const float distance) {
    const glm::vec3 direction = glm::normalize(calculate_direction());
    m_data.position -= (direction * distance);
}

void PerspectiveCamera::move_left(const float distance) {
    const glm::vec3 direction =
        glm::normalize(glm::cross(m_up_direction, calculate_direction()));
    m_data.position += (direction * distance);
}

void PerspectiveCamera::move_right(const float distance) {
    const glm::vec3 direction =
        glm::normalize(glm::cross(m_up_direction, calculate_direction()));
    m_data.position -= (direction * distance);
}

void PerspectiveCamera::move_up(const float distance) {
    const glm::vec3 direction = glm::normalize(m_up_direction);
    m_data.position += (direction * distance);
}

void PerspectiveCamera::move_down(const float distance) {
    const glm::vec3 direction = glm::normalize(m_up_direction);
    m_data.position -= (direction * distance);
}

void PerspectiveCamera::rotate_horizon(const float angle) {
    const float new_rotation = m_data.rotation.x + angle;
    if (new_rotation > glm::pi<float>() * 2) {
        // TODO:
    }
    if (new_rotation < -glm::pi<float>() * 2) {
        // TODO:
    }
    m_data.rotation.x = new_rotation;
}

void PerspectiveCamera::rotate_vertical(const float angle) {
    const float new_rotation = m_data.rotation.y + angle;
    if (new_rotation >= glm::pi<float>() / 2.0f) {
        m_data.rotation.y = glm::pi<float>() / 2.0f - 0.001;
        return;
    }
    if (new_rotation + angle <= -glm::pi<float>() / 2.0f) {
        m_data.rotation.y = -glm::pi<float>() / 2.0f + 0.001;
        return;
    }
    m_data.rotation.y = new_rotation;
}

glm::mat4 PerspectiveCamera::get_view_matrix() {
    return glm::lookAt(m_data.position, m_data.position + calculate_direction(),
                       glm::normalize(m_up_direction));
}

glm::mat4 PerspectiveCamera::get_projection_matrix() {
    return glm::perspective(m_data.fov, m_data.aspect_ratio, m_data.near_plane,
                            m_data.far_plane);
}

void PerspectiveCamera::set_position(const glm::vec3 &position) {
    m_data.position = position;
}

void PerspectiveCamera::set_near_plane(const float near) {
    m_data.near_plane = near;
}

void PerspectiveCamera::set_far_plane(const float far) {
    m_data.far_plane = far;
}

void PerspectiveCamera::set_fov(const float fov) { m_data.fov = fov; }

void PerspectiveCamera::set_aspect_ratio(const float aspect_ratio) {
    m_data.aspect_ratio = aspect_ratio;
}

glm::vec3 PerspectiveCamera::calculate_direction() {
    glm::quat rotation_h = glm::angleAxis(m_data.rotation.x, m_up_direction);
    glm::vec3 direction = rotation_h * m_look_direction;

    glm::quat rotation_v =
        glm::angleAxis(m_data.rotation.y,
                       glm::normalize(glm::cross(m_up_direction, direction)));
    direction = rotation_v * direction;

    return direction;
}

} // namespace Vis
