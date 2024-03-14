#include "square.hpp"

namespace Vis {

Square::Square(const std::string &name)
    : Solid(
          {name,
           {
               Vertex({-0.5, -0.5, -1.0, 1.0}, {1.0, 0.0, 0.0, 1.0},
                      {0.0, 0.0}),
               Vertex({0.5, -0.5, -1.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 0.0}),
               Vertex({-0.5, 0.5, -1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {0.0, 1.0}),
               Vertex({0.5, 0.5, -1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0}),
           },
           {0, 1, 2, 1, 2, 3},
           {{Topology::Triangle, 0, 2}},
           {glm::mat4()}}) {}

} // namespace Vis
