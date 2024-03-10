#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "image.hpp"

namespace Vis {

class SceneObject {
  public:
    std::string m_name;
    std::vector<glm::vec4> m_positions;
    std::vector<std::array<size_t, 3>> m_indices;
    glm::mat4 m_object_space_matrix;
};

class CpuRenderer {
  public:
    CpuRenderer();
    ~CpuRenderer();

    void *render_image(const size_t width, const size_t height);

  private:
    size_t m_width{0};
    size_t m_height{0};
    std::unique_ptr<Image> m_image;
};

} // namespace Vis
