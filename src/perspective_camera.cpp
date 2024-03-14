#include "camera.hpp"

#include <glm/ext.hpp>

namespace Vis {

PerspectiveCamera::PerspectiveCamera(const PerspectiveData &data)
    : m_data(data) {}

glm::mat4 PerspectiveCamera::get_view_matrix() {
    return glm::lookAt(m_data.position, m_data.look_direction,
                       glm::normalize(m_data.up_direction));
}
glm::mat4 PerspectiveCamera::get_projection_matrix() {
    return glm::perspective(m_data.fov, m_data.aspect_ratio, m_data.near_plane,
                            m_data.far_plane);
}

} // namespace Vis
