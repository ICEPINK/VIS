#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex.hpp"

namespace Vis {

enum class Topology { Point, Line, Triangle };

struct Layout {
  Topology topology;
  size_t start;
  size_t count;
};

struct Solid {
  std::string name{""};
  std::vector<Vertex> vertices{};
  std::vector<size_t> indices{};
  std::vector<Layout> layout{};
  glm::dmat4 matrix{1.0};

  static auto Cube(const std::string_view name = "") -> Solid;
  static auto Axis(const std::string_view name = "") -> Solid;
  static auto Triangle(const std::string_view name = "") -> Solid;
  static auto Square(const std::string_view name = "") -> Solid;
  static auto Icosphere(const std::string_view name = "") -> Solid;
};

} // namespace Vis
