#include "triangle.hpp"

namespace Vis {

Triangle::Triangle(const std::string &name)
    : Solid({name,
             {Vertex({1.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {0.0, 0.0}),
              Vertex({0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 0.0}),
              Vertex({0.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {0.0, 1.0})},
             {0, 1, 2},
             {{Topology::Triangle, 0, 1}},
             {glm::mat4(1.0)}}) {}

} // namespace Vis
