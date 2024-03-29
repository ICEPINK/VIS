#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "solids/solid.hpp"

namespace Vis {

class Camera {
  public:
    virtual ~Camera() {}

    virtual glm::mat4 get_view_matrix() const = 0;
    virtual glm::mat4 get_projection_matrix() const = 0;
    virtual Solid generate_solid() const = 0;
};

struct PerspectiveData {
    glm::vec3 position;
    glm::vec2 rotation;
    float near_plane;
    float far_plane;
    float fov;
    float aspect_ratio;
};

class PerspectiveCamera : public Camera {

  public:
    PerspectiveCamera(const PerspectiveData &data);

    void move_forward(const float distance);
    void move_backward(const float distance);
    void move_left(const float distance);
    void move_right(const float distance);
    void move_up(const float distance);
    void move_down(const float distance);
    void rotate_horizon(const float angle);
    void rotate_vertical(const float angle);
    virtual Solid generate_solid() const override;

    virtual glm::mat4 get_view_matrix() const override;
    virtual glm::mat4 get_projection_matrix() const override;

    void set_position(const glm::vec3 &fov);
    void set_near_plane(const float near);
    void set_far_plane(const float far);
    void set_fov(const float fov);
    void set_aspect_ratio(const float fov);

  private:
    glm::vec3 calculate_direction() const;

  private:
    PerspectiveData m_data;
    glm::vec3 m_up_direction;
    glm::vec3 m_look_direction;
};

struct OrthographicData {};
class OrthographicCamera : public Camera {};

} // namespace Vis
