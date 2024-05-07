#include "pipeline.hpp"
namespace Vis {
namespace Alg {
auto dehomog_all(std::vector<Vertex> &vertices) -> void {
  for (auto &vertex : vertices) {
    const auto w = vertex.pos.w;
    vertex.pos /= w;
    vertex.col /= w;
    vertex.one /= w;
  }
}
auto dehomog_pos(std::vector<Vertex> &vertices) -> void {
  for (auto &vertex : vertices) {
    const auto w = vertex.pos.w;
    vertex.pos /= w;
  }
}
auto trasform_to_viewport(std::vector<Vertex> &vertices,
                          const Image &image) -> void {
  for (auto &vertex : vertices) {
    vertex.pos.x = ((vertex.pos.x + 1.0) / 2.0) * (image.get_width() - 1);
    vertex.pos.y = ((vertex.pos.y + 1.0) / 2.0) * (image.get_height() - 1);
  }
}
auto rasterize_line(std::vector<Vertex> &vertices, Image &image,
                    void (*set_pixel)(Vertex &vertex, Image &image)) -> void {
  if (vertices.size() % 2 != 0) {
    return;
  }
  for (size_t vertices_index = 0; vertices_index < vertices.size();
       vertices_index += 2) {
    auto &v_a = vertices[vertices_index];
    auto &v_b = vertices[vertices_index + 1];
    double alfa = (v_b.pos.y - v_a.pos.y) / (v_b.pos.x - v_a.pos.x);
    if (alfa * alfa < 1) {
      if (v_a.pos.x > v_b.pos.x) {
        std::swap(v_a, v_b);
      }
      for (int64_t x = static_cast<int64_t>(v_a.pos.x); x <= v_b.pos.x; ++x) {
        if (v_b.pos.x == v_a.pos.x) {
          continue;
        }
        double t = (x - v_a.pos.x) / (v_b.pos.x - v_a.pos.x);
        auto vertex = Vertex::interpolate(t, v_a, v_b);
        if (std::isnan(vertex.pos.x)) {
          continue;
        }
        vertex.pos.x = static_cast<double>(x);
        set_pixel(vertex, image);
      }
    } else {
      if (v_a.pos.y > v_b.pos.y) {
        std::swap(v_a, v_b);
      }
      for (int64_t y = static_cast<int64_t>(v_a.pos.y); y <= v_b.pos.y; ++y) {
        if (v_b.pos.y == v_a.pos.y) {
          continue;
        }
        double t = (y - v_a.pos.y) / (v_b.pos.y - v_a.pos.y);
        auto vertex = Vertex::interpolate(t, v_a, v_b);
        if (std::isnan(vertex.pos.x)) {
          continue;
        }
        vertex.pos.y = static_cast<double>(y);
        set_pixel(vertex, image);
      }
    }
  }
}
auto rasterize_point(std::vector<Vertex> &vertices, Image &image,
                     void (*set_pixel)(Vertex &vertex, Image &image)) -> void {
  for (auto &vertex : vertices) {
    set_pixel(vertex, image);
  }
}
auto rasterize_triangle(std::vector<Vertex> &vertices, Image &image,
                        void (*set_pixel)(Vertex &vertex,
                                          Image &image)) -> void {
  if (vertices.size() % 3 != 0) {
    return;
  }
  for (size_t vertices_index = 0; vertices_index < vertices.size();
       vertices_index += 3) {
    auto &v_a = vertices[vertices_index];
    auto &v_b = vertices[vertices_index + 1];
    auto &v_c = vertices[vertices_index + 2];
    if (v_a.pos.y > v_b.pos.y) {
      std::swap(v_a, v_b);
    }
    if (v_b.pos.y > v_c.pos.y) {
      std::swap(v_b, v_c);
    }
    if (v_a.pos.y > v_b.pos.y) {
      std::swap(v_a, v_b);
    }
    int64_t start_y = static_cast<int64_t>(std::max(v_a.pos.y, 0.0));
    int64_t end_y = static_cast<int64_t>(
        std::min(v_b.pos.y, static_cast<double>(image.get_height() - 1)));
    for (int64_t y = start_y; y <= end_y; ++y) {
      const double t_ab = (y - v_a.pos.y) / (v_b.pos.y - v_a.pos.y);
      Vertex v_ab = Vertex::interpolate(t_ab, v_a, v_b);
      if (std::isnan(v_ab.pos.x)) {
        continue;
      }
      const double t_ac = (y - v_a.pos.y) / (v_c.pos.y - v_a.pos.y);
      Vertex v_ac = Vertex::interpolate(t_ac, v_a, v_c);
      if (std::isnan(v_ac.pos.x)) {
        continue;
      }
      if (v_ab.pos.x > v_ac.pos.x) {
        std::swap(v_ab, v_ac);
      }
      int64_t start_x = static_cast<int64_t>(std::max(v_ab.pos.x, 0.0));
      int64_t end_x = static_cast<int64_t>(
          std::min(v_ac.pos.x, static_cast<double>(image.get_width() - 1)));
      for (int64_t x = start_x; x <= end_x; ++x) {
        const double t_abac = (x - v_ab.pos.x) / (v_ac.pos.x - v_ab.pos.x);
        Vertex v_abac = Vertex::interpolate(t_abac, v_ab, v_ac);
        if (std::isnan(v_abac.pos.x)) {
          continue;
        }
        v_abac.pos.x = static_cast<double>(x);
        v_abac.pos.y = static_cast<double>(y);
        set_pixel(v_abac, image);
      }
    }
    start_y = static_cast<int64_t>(std::max(v_b.pos.y, 0.0));
    end_y = static_cast<int64_t>(
        std::min(v_c.pos.y, static_cast<double>(image.get_height() - 1)));
    for (int64_t y = start_y; y <= end_y; ++y) {
      const double t_bc = (y - v_b.pos.y) / (v_c.pos.y - v_b.pos.y);
      Vertex v_bc = Vertex::interpolate(t_bc, v_b, v_c);
      if (std::isnan(v_bc.pos.x)) {
        continue;
      }
      const double t_ac = (y - v_a.pos.y) / (v_c.pos.y - v_a.pos.y);
      Vertex v_ac = Vertex::interpolate(t_ac, v_a, v_c);
      if (std::isnan(v_ac.pos.x)) {
        continue;
      }
      if (v_bc.pos.x > v_ac.pos.x) {
        std::swap(v_bc, v_ac);
      }
      int64_t start_x = static_cast<int64_t>(std::max(v_bc.pos.x, 0.0));
      int64_t end_x = static_cast<int64_t>(
          std::min(v_ac.pos.x, static_cast<double>(image.get_width() - 1)));
      for (int64_t x = start_x; x <= end_x; ++x) {
        const double t_bcac = (x - v_bc.pos.x) / (v_ac.pos.x - v_bc.pos.x);
        Vertex v_bcac = Vertex::interpolate(t_bcac, v_bc, v_ac);
        if (std::isnan(v_bcac.pos.x)) {
          continue;
        }
        v_bcac.pos.x = static_cast<double>(x);
        v_bcac.pos.y = static_cast<double>(y);
        set_pixel(v_bcac, image);
      }
    }
  }
}
auto set_pixel_rgba_no_depth(Vertex &vertex, Image &image) -> void {
  if (vertex.pos.x < 0 || vertex.pos.y < 0) {
    return;
  }
  size_t x{static_cast<size_t>(vertex.pos.x)};
  size_t y{static_cast<size_t>(vertex.pos.y)};
  image.set_pixel(x, y, vertex.col * 1.0 / vertex.one);
}
auto set_pixel_rgba_depth(Vertex &vertex, Image &image) -> void {
  if (vertex.pos.x < 0 || vertex.pos.y < 0) {
    return;
  }
  size_t x{static_cast<size_t>(vertex.pos.x)};
  size_t y{static_cast<size_t>(vertex.pos.y)};
  if (vertex.pos.z > image.get_depth(x, y)) {
    return;
  }
  image.set_depth(x, y, vertex.pos.z);
  image.set_pixel(x, y, vertex.col * 1.0 / vertex.one);
}
auto set_pixel_w_depth(Vertex &vertex, Image &image) -> void {
  if (vertex.pos.x < 0 || vertex.pos.y < 0) {
    return;
  }
  size_t x{static_cast<size_t>(vertex.pos.x)};
  size_t y{static_cast<size_t>(vertex.pos.y)};
  if (vertex.pos.z > image.get_depth(x, y)) {
    return;
  }
  image.set_depth(x, y, vertex.pos.z);
  image.set_pixel(x, y, {vertex.pos.z, vertex.pos.z, vertex.pos.z, 1.0});
}
auto set_pixel_w_no_depth(Vertex &vertex, Image &image) -> void {
  if (vertex.pos.x < 0 || vertex.pos.y < 0) {
    return;
  }
  size_t x{static_cast<size_t>(vertex.pos.x)};
  size_t y{static_cast<size_t>(vertex.pos.y)};
  image.set_pixel(x, y, {vertex.pos.z, vertex.pos.z, vertex.pos.z, 1.0});
}
} // namespace Alg
} // namespace Vis
