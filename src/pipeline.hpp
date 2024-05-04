#pragma once

#include "image.hpp"
#include "utils/vertex.hpp"

#include <vector>

namespace Vis {

struct Pipeline {
    void (*vertex_trasform)(std::vector<Vertex> &vertices,const glm::dmat4 &matrix);
    void (*dehomog)(std::vector<Vertex> &vertices);
    void (*trasform_to_viewport)(std::vector<Vertex> &vertices,
                                 const Image &image);
    void (*rasterize)(std::vector<Vertex> &vertices, Image &image,
                      void (*se_pixel)(const Vertex &vertex, Image &image));
    void (*set_pixel)(const Vertex &vertex, Image &image);
};

namespace Alg {
auto dehomog(std::vector<Vertex> &vertices) -> void;
auto trasform_to_viewport(std::vector<Vertex> &vertices,
                          const Image &image) -> void;
auto rasterize_triangle(std::vector<Vertex> &vertices, Image &image,
                        void (*set_pixel)(const Vertex &vertex,
                                          Image &image)) -> void;
auto set_pixel(const Vertex &vertex, Image &image) -> void;
} // namespace Alg

} // namespace Vis
