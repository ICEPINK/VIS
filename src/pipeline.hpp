#pragma once
// src includes
#include "image.hpp"
#include "vertex.hpp"
// std includes
#include <vector>
namespace Vis {
namespace Alg {
auto clip_after_dehomog_line(std::vector<Vertex> &vertices) -> void;
auto clip_after_dehomog_none(std::vector<Vertex> &vertices) -> void;
auto clip_after_dehomog_triangle(std::vector<Vertex> &vertices) -> void;
auto clip_before_dehomog_line(std::vector<Vertex> &vertices) -> void;
auto clip_before_dehomog_none(std::vector<Vertex> &vertices) -> void;
auto clip_before_dehomog_triangle(std::vector<Vertex> &vertices) -> void;
auto clip_fast_line(std::vector<Vertex> &vertices) -> void;
auto clip_fast_none(std::vector<Vertex> &) -> void;
auto clip_fast_point(std::vector<Vertex> &vertices) -> void;
auto clip_fast_triangle(std::vector<Vertex> &vertices) -> void;
auto dehomog_all(std::vector<Vertex> &vertices) -> void;
auto dehomog_none(std::vector<Vertex> &vertices) -> void;
auto dehomog_pos(std::vector<Vertex> &vertices) -> void;
auto rasterize_line(std::vector<Vertex> &vertices, Image &image, void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_none(std::vector<Vertex> &vertices, Image &image, void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_point(std::vector<Vertex> &vertices, Image &image, void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_triangle(std::vector<Vertex> &vertices, Image &image, void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto rasterize_triangle_as_lines(std::vector<Vertex> &vertices, Image &image, void (*set_pixel)(Vertex &vertex, Image &image)) -> void;
auto set_pixel_none(Vertex &vertex, Image &image) -> void;
auto set_pixel_rgba_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_rgba_no_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_z_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_z_no_depth(Vertex &vertex, Image &image) -> void;
auto set_pixel_tex(Vertex &vertex, Image &image) -> void;
auto set_pixel_white(Vertex &vertex, Image &image) -> void;
auto trasform_to_none(std::vector<Vertex> &vertices, const Image &image) -> void;
auto trasform_to_viewport(std::vector<Vertex> &vertices, const Image &image) -> void;
auto trasform_vertices_by_matrix(std::vector<Vertex> &vertices, const glm::dmat4 &matrix) -> void;
auto trasform_vertices_by_none(std::vector<Vertex> &vertices, const glm::dmat4 &matrix) -> void;
} // namespace Alg
struct Pipeline {
  void (*clip_after_dehomog)(std::vector<Vertex> &vertices){Alg::clip_after_dehomog_none};
  void (*clip_before_dehomog)(std::vector<Vertex> &vertices){Alg::clip_before_dehomog_none};
  void (*clip_fast)(std::vector<Vertex> &vertices){Alg::clip_fast_none};
  void (*dehomog)(std::vector<Vertex> &vertices){Alg::dehomog_none};
  void (*rasterize)(std::vector<Vertex> &vertices, Image &image, void (*set_pixel)(Vertex &vertex, Image &image)){Alg::rasterize_none};
  void (*set_pixel)(Vertex &vertex, Image &image){Alg::set_pixel_none};
  void (*trasform_to_viewport)(std::vector<Vertex> &vertices, const Image &image){Alg::trasform_to_none};
  void (*trasform_vertices)(std::vector<Vertex> &vertices, const glm::dmat4 &matrix){Alg::trasform_vertices_by_none};
};
} // namespace Vis
