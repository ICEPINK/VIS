#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../vis_utils.hpp"

namespace Vis {

enum class Topology { Point, Line, Triangle };

struct Layout {
    Topology topology;
    size_t start;
    size_t count;
};

struct SolidData {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<size_t> indices;
    std::vector<Layout> leyout;
    glm::mat4 matrix;
};

class Solid {
  public:
    Solid(const SolidData &data);

  public:
    SolidData data;
};

} // namespace Vis
