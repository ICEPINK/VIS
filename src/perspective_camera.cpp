#include "camera.hpp"

#include <iostream>

#include <glm/ext.hpp>

namespace Vis {

PerspectiveCamera::PerspectiveCamera(const PerspectiveData &data)
    : m_data(data), m_up_direction({0.0f, 0.0f, 1.0f}),
      m_look_direction({1.0f, 0.0f, 0.0f}) {

    rotate_horizon(0.0f);
    rotate_vertical(0.0f);
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
    float new_rotation = m_data.rotation.x + angle;

    while (new_rotation > glm::pi<float>() * 2) {
        new_rotation -= glm::pi<float>() * 2;
    }
    while (new_rotation < -glm::pi<float>() * 2) {
        new_rotation += glm::pi<float>() * 2;
    }

    m_data.rotation.x = new_rotation;
}

void PerspectiveCamera::rotate_vertical(const float angle) {
    const float new_rotation = m_data.rotation.y + angle;

    if (new_rotation >= glm::pi<float>() / 2.0f) {
        m_data.rotation.y = glm::pi<float>() / 2.0f - 0.001f;
        return;
    }

    if (new_rotation + angle <= -glm::pi<float>() / 2.0f) {
        m_data.rotation.y = -glm::pi<float>() / 2.0f + 0.001f;
        return;
    }

    m_data.rotation.y = new_rotation;
}
Solid PerspectiveCamera::generate_solid() const {
    std::vector<Vertex> vertices;
    vertices.reserve(9);
    vertices.push_back({{m_data.position, 1.0f}});

    const glm::mat4 view = get_view_matrix();
    const glm::mat4 proj = get_projection_matrix();

    Vertex vec = {{-1.0f, -1.0, 0.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{1.0f, -1.0f, 0.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{-1.0f, 1.0f, 0.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{1.0f, 1.0f, 0.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{-1.0f, -1.0f, 1.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{1.0f, -1.0f, 1.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{-1.0f, 1.0f, 1.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    vec = {{1.0f, 1.0f, 1.0f, 1.0f}};
    vec.position = glm::inverse(view) * glm::inverse(proj) * vec.position;
    vertices.push_back(vec);

    std::vector<size_t> indices = {0, 5, 0, 6, 0, 7, 0, 8, 1, 2, 2, 4,
                                   4, 3, 3, 1, 5, 6, 6, 8, 8, 7, 7, 5};

    Layout layout{};
    layout.start = 0;
    layout.count = indices.size() / 2;
    layout.topology = Topology::Line;

    SolidData data{};
    data.name = "Camera";
    data.vertices = vertices;
    data.indices = indices;
    data.layout = {layout};
    data.matrix = {glm::mat4{1.0f}};

    return {data};
}

glm::mat4 PerspectiveCamera::get_view_matrix() const {
    return glm::lookAt(m_data.position, m_data.position + calculate_direction(),
                       glm::normalize(m_up_direction));
}

glm::mat4 PerspectiveCamera::get_projection_matrix() const {
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

glm::vec3 PerspectiveCamera::calculate_direction() const {
    glm::quat rotation_h = glm::angleAxis(m_data.rotation.x, m_up_direction);
    glm::vec3 direction = rotation_h * m_look_direction;

    glm::quat rotation_v =
        glm::angleAxis(m_data.rotation.y,
                       glm::normalize(glm::cross(m_up_direction, direction)));
    direction = rotation_v * direction;

    return direction;
}

} // namespace Vis
