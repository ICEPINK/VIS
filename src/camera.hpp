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

    virtual glm::mat4 get_view_matrix() override;
    virtual glm::mat4 get_projection_matrix() override;

  private:
    PerspectiveData m_data;
};

struct OrthographicData {};
class OrthographicCamera : public Camera {};

} // namespace Vis
