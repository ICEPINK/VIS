#pragma once

#include <glm/glm.hpp>

namespace Vis {

class Camera {
  public:
    virtual glm::mat4 get_view_matrix() = 0;
    virtual glm::mat4 get_projection_matrix() = 0;
};

struct PerspectiveData {
    glm::vec3 position;
    glm::vec3 look_direction;
    glm::vec3 up_direction;
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

    virtual glm::mat4 get_view_matrix() override;
    virtual glm::mat4 get_projection_matrix() override;

    void set_position(const glm::vec3 &fov);
    void set_near_plane(const float near);
    void set_far_plane(const float far);
    void set_fov(const float fov);
    void set_aspect_ratio(const float fov);

  private:
    PerspectiveData m_data;
};

struct OrthographicData {};
class OrthographicCamera : public Camera {};

} // namespace Vis
