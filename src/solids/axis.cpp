#include "axis.hpp"

namespace Vis {

Axis::Axis(const std::string &name)
    : Solid(
          {name,
           {
               Vertex({0.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 0.0}),
               Vertex({1.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {0.0, 0.0}),
               Vertex({0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {0.0, 0.0}),
               Vertex({0.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {0.0, 0.0}),
           },
           {1, 0, 2, 0, 3, 0},
           {{Topology::Line, 0, 3}},
           {glm::mat4(1.0)}}) {}

} // namespace Vis
