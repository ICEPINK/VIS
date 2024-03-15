#include "camera.hpp"

#include <glm/ext.hpp>

namespace Vis {

PerspectiveCamera::PerspectiveCamera(const PerspectiveData &data)
    : m_data(data) {}

void PerspectiveCamera::move_forward(const float distance) {
    const glm::vec3 direction = glm::normalize(m_data.look_direction);
    m_data.position += (direction * distance);
}

void PerspectiveCamera::move_backward(const float distance) {
    const glm::vec3 direction = glm::normalize(m_data.look_direction);
    m_data.position -= (direction * distance);
}

void PerspectiveCamera::move_left(const float distance) {
    const glm::vec3 direction =
        glm::normalize(glm::cross(m_data.up_direction, m_data.look_direction));
    m_data.position += (direction * distance);
}

void PerspectiveCamera::move_right(const float distance) {
    const glm::vec3 direction =
        glm::normalize(glm::cross(m_data.up_direction, m_data.look_direction));
    m_data.position -= (direction * distance);
}

void PerspectiveCamera::move_up(const float distance) {
    const glm::vec3 direction = glm::normalize(m_data.up_direction);
    m_data.position += (direction * distance);
}

void PerspectiveCamera::move_down(const float distance) {
    const glm::vec3 direction = glm::normalize(m_data.up_direction);
    m_data.position -= (direction * distance);
}

glm::mat4 PerspectiveCamera::get_view_matrix() {
    return glm::lookAt(m_data.position, m_data.position + m_data.look_direction,
                       glm::normalize(m_data.up_direction));
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

void PerspectiveCamera::set_fov(const float fov) {
    m_data.fov = fov;
}

void PerspectiveCamera::set_aspect_ratio(const float aspect_ratio) {
    m_data.aspect_ratio = aspect_ratio;
}

} // namespace Vis
