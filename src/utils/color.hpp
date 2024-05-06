#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace Vis {

typedef glm::vec4 ColorRGBA32f;

struct ColorRGBA8i {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
};

} // namespace Vis
