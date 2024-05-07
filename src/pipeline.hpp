#pragma once
// src includes
#include "image.hpp"
#include "utils/vertex.hpp"
// std includes
#include <vector>
namespace Vis {
struct Pipeline {
  void (*clip_after_dehomog)(std::vector<Vertex> &vertices);
  void (*clip_before_dehemog)(std::vector<Vertex> &vertices);
  void (*clip_fast)(std::vector<Vertex> &vertices);
  void (*dehomog)(std::vector<Vertex> &vertices);
  void (*rasterize)(std::vector<Vertex> &vertices, Image &image,
                    void (*set_pixel)(Vertex &vertex, Image &image));
  void (*set_pixel)(Vertex &vertex, Image &image);
  void (*trasform_to_viewport)(std::vector<Vertex> &vertices,
                               const Image &image);
  void (*trasform_vertices)(std::vector<Vertex> &vertices,
                            const glm::dmat4 &matrix);
};
namespace Alg {
auto clip_after_dehemog_none(std::vector<Vertex> &vertices) -> void;
auto clip_before_dehemog_line(std::vector<Vertex> &vertices) -> void;
auto clip_before_dehemog_none(std::vector<Vertex> &vertices) -> void;
auto clip_before_dehemog_triangle(std::vector<Vertex> &vertices) -> void;
auto clip_fast_line(std::vector<Vertex> &vertices) -> void;
auto clip_fast_none(std::vector<Vertex> &) -> void;
auto clip_fast_point(std::vector<Vertex> &vertices) -> void;
auto clip_fast_triangle(std::vector<Vertex> &vertices) -> void;
auto dehomog_all(std::vector<Vertex> &vertices) -> void;
auto dehomog_none(std::vector<Vertex> &vertices) -> void;
auto dehomog_pos(std::vector<Vertex> &vertices) -> void;
auto rasterize_line(std::vector<Vertex> &vertices, Image &image,
                    void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_none(std::vector<Vertex> &vertices, Image &image,
                    void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_point(std::vector<Vertex> &vertices, Image &image,
                     void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_triangle(std::vector<Vertex> &vertices, Image &image,
                        void (*set_pixel)(Vertex &vertex,
                                          Image &image)) -> void;
auto set_pixel_none(Vertex &vertex, Image &image) -> void;
auto set_pixel_rgba_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_rgba_no_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_w_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_w_no_depth(Vertex &vertex, Image &image) -> void;
auto trasform_to_none(std::vector<Vertex> &vertices,
                      const Image &image) -> void;
auto trasform_to_viewport(std::vector<Vertex> &vertices,
                          const Image &image) -> void;
} // namespace Alg
} // namespace Vis
