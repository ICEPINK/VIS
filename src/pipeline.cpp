#include "pipeline.hpp"
#define PLANE_TEST(v_in, v_out, test_plane, value, comparison_operator_in)     \
  for (size_t j = 0; j < v_in.size(); ++j) {                                   \
    const auto &v1 = v_in[j];                                                  \
    const auto &v2 = v_in[(j + 1) % v_in.size()];                              \
    const auto v1_in = (v1.pos.test_plane comparison_operator_in value);       \
    const auto v2_in = (v2.pos.test_plane comparison_operator_in value);       \
    if (v1_in && v2_in) {                                                      \
      v_out.push_back(v2);                                                     \
    } else if (v1_in) {                                                        \
      const auto t =                                                           \
        (value - v1.pos.test_plane) / (v2.pos.test_plane - v1.pos.test_plane); \
      const auto v_new = Vertex::interpolate(t, v1, v2);                       \
      v_out.push_back(v_new);                                                  \
    } else if (v2_in) {                                                        \
      const auto t =                                                           \
        (value - v1.pos.test_plane) / (v2.pos.test_plane - v1.pos.test_plane); \
      const auto v_new = Vertex::interpolate(t, v1, v2);                       \
      v_out.push_back(v_new);                                                  \
      v_out.push_back(v2);                                                     \
    }                                                                          \
  }
namespace Vis {
namespace Alg {
auto clip_after_dehomog_none(std::vector<Vertex> &) -> void {}
auto clip_after_dehomog_triangle(std::vector<Vertex> &vertices) -> void {
  if (vertices.size() % 3 != 0) {
    return;
  }
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices.size() * 9);
  for (size_t i = 0; i < vertices.size(); i += 3) {
    std::vector<Vertex> v_in;
    v_in.reserve(9);
    v_in.push_back(vertices[i]);
    v_in.push_back(vertices[i + 1]);
    v_in.push_back(vertices[i + 2]);
    std::vector<Vertex> v_out;
    v_out.reserve(9);
    PLANE_TEST(v_in, v_out, x, -1, >)
    v_in = v_out;
    v_out = {};
    PLANE_TEST(v_in, v_out, x, 1, <)
    v_in = v_out;
    v_out = {};
    PLANE_TEST(v_in, v_out, y, -1, >)
    v_in = v_out;
    v_out = {};
    PLANE_TEST(v_in, v_out, y, 1, <)
    v_in = v_out;
    v_out = {};
    PLANE_TEST(v_in, v_out, z, 1, <)
    for (size_t k = 2; k < v_out.size(); ++k) {
      new_vertices.push_back(v_out[0]);
      new_vertices.push_back(v_out[k - 1]);
      new_vertices.push_back(v_out[k]);
    }
  }
  vertices = new_vertices;
}
auto clip_before_dehomog_line(std::vector<Vertex> &vertices) -> void {
  if (vertices.size() % 2 != 0) {
    return;
  }
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices.size());
  for (size_t i = 0; i < vertices.size(); i += 2) {
    auto &v1 = vertices[i];
    auto &v2 = vertices[i + 1];
    // WARN: pos.z no dehomo?
    if (v1.pos.z > v2.pos.z) {
      std::swap(v1, v2);
    }
    if (v1.pos.z <= 0) {
      const double t_21 = (0 - v2.pos.z) / (v1.pos.z - v2.pos.z);
      v1 = Vertex::interpolate(t_21, v2, v1);
    }
    new_vertices.push_back(v1);
    new_vertices.push_back(v2);
  }
  vertices = new_vertices;
}
auto clip_before_dehomog_none(std::vector<Vertex> &) -> void {}
auto clip_before_dehomog_triangle(std::vector<Vertex> &vertices) -> void {
  if (vertices.size() % 3 != 0) {
    return;
  }
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices.size() * 2);
  for (size_t i = 0; i < vertices.size(); i += 3) {
    auto &v1 = vertices[i];
    auto &v2 = vertices[i + 1];
    auto &v3 = vertices[i + 2];
    if (v1.pos.z > 0 && v2.pos.z > 0 && v3.pos.z > 0) {
      new_vertices.push_back(v1);
      new_vertices.push_back(v2);
      new_vertices.push_back(v3);
      continue;
    }
    // WARN: pos.z no dehomo?
    if (v1.pos.z > v2.pos.z) {
      std::swap(v1, v2);
    }
    if (v2.pos.z > v3.pos.z) {
      std::swap(v2, v3);
    }
    if (v1.pos.z > v2.pos.z) {
      std::swap(v1, v2);
    }
    if (v2.pos.z <= 0) {
      const double t_32 = (0 - v3.pos.z) / (v2.pos.z - v3.pos.z);
      v2 = Vertex::interpolate(t_32, v3, v2);
      const double t_31 = (0 - v3.pos.z) / (v1.pos.z - v3.pos.z);
      v1 = Vertex::interpolate(t_31, v3, v1);
      new_vertices.push_back(v1);
      new_vertices.push_back(v2);
      new_vertices.push_back(v3);
    } else {
      const double t_21 = (0 - v2.pos.z) / (v1.pos.z - v2.pos.z);
      auto v21 = Vertex::interpolate(t_21, v2, v1);
      const double t_31 = (0 - v3.pos.z) / (v1.pos.z - v3.pos.z);
      auto v31 = Vertex::interpolate(t_31, v3, v1);
      new_vertices.push_back(v21);
      new_vertices.push_back(v31);
      new_vertices.push_back(v3);
      new_vertices.push_back(v2);
      new_vertices.push_back(v21);
      new_vertices.push_back(v3);
    }
  }
  vertices = new_vertices;
}
auto clip_fast_line(std::vector<Vertex> &vertices) -> void {
  if (vertices.size() % 2 != 0) {
    return;
  }
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices.size());
  for (size_t i = 0; i < vertices.size(); i += 2) {
    const auto &v1 = vertices[i];
    const auto &v2 = vertices[i + 1];
    if ((v1.pos.x < -v1.pos.w && v2.pos.x < -v2.pos.w) ||
        (v1.pos.x > v1.pos.w && v2.pos.x > v2.pos.w) ||
        (v1.pos.y < -v1.pos.w && v2.pos.y < -v2.pos.w) ||
        (v1.pos.y > v1.pos.w && v2.pos.y > v2.pos.w) ||
        (v1.pos.z < 0 && v2.pos.z < 0) ||
        (v1.pos.z > v1.pos.w && v2.pos.z > v2.pos.w)) {
      continue;
    }
    new_vertices.push_back(v1);
    new_vertices.push_back(v2);
  }
  vertices = new_vertices;
}
auto clip_fast_none(std::vector<Vertex> &) -> void {}
auto clip_fast_point(std::vector<Vertex> &vertices) -> void {
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices.size());
  for (size_t i = 0; i < vertices.size(); ++i) {
    const auto &v1 = vertices[i];
    if ((v1.pos.x < -v1.pos.w) || (v1.pos.x > v1.pos.w) ||
        (v1.pos.y < -v1.pos.w) || (v1.pos.y > v1.pos.w) || (v1.pos.z < 0) ||
        (v1.pos.z > v1.pos.w)) {
      continue;
    }
    new_vertices.push_back(v1);
  }
  vertices = new_vertices;
}
auto clip_fast_triangle(std::vector<Vertex> &vertices) -> void {
  if (vertices.size() % 3 != 0) {
    return;
  }
  std::vector<Vertex> new_vertices;
  new_vertices.reserve(vertices.size());
  for (size_t i = 0; i < vertices.size(); i += 3) {
    const auto &v1 = vertices[i];
    const auto &v2 = vertices[i + 1];
    const auto &v3 = vertices[i + 2];
    if ((v1.pos.x < -v1.pos.w && v2.pos.x < -v2.pos.w &&
         v3.pos.x < -v3.pos.w) ||
        (v1.pos.x > v1.pos.w && v2.pos.x > v2.pos.w && v3.pos.x > v3.pos.w) ||
        (v1.pos.y < -v1.pos.w && v2.pos.y < -v2.pos.w &&
         v3.pos.y < -v3.pos.w) ||
        (v1.pos.y > v1.pos.w && v2.pos.y > v2.pos.w && v3.pos.y > v3.pos.w) ||
        (v1.pos.z < 0 && v2.pos.z < 0 && v3.pos.z < 0) ||
        (v1.pos.z > v1.pos.w && v2.pos.z > v2.pos.w && v3.pos.z > v3.pos.w)) {
      continue;
    }
    new_vertices.push_back(v1);
    new_vertices.push_back(v2);
    new_vertices.push_back(v3);
  }
  vertices = new_vertices;
}
auto dehomog_all(std::vector<Vertex> &vertices) -> void {
  for (auto &vertex : vertices) {
    const auto w = vertex.pos.w;
    vertex.pos /= w;
    vertex.col /= w;
    vertex.one /= w;
  }
}

auto dehomog_none(std::vector<Vertex> &) -> void {}
auto dehomog_pos(std::vector<Vertex> &vertices) -> void {
  for (auto &vertex : vertices) {
    const auto w = vertex.pos.w;
    vertex.pos /= w;
  }
}
auto trasform_to_none(std::vector<Vertex> &, const Image &) -> void {}
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
    if (std::isnan(v_a.pos.x) || std::isnan(v_b.pos.x)) {
      continue;
    }
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
auto rasterize_none(std::vector<Vertex> &, Image &,
                    void (*)(Vertex &, Image &)) -> void {}
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
    if (std::isnan(v_a.pos.x) || std::isnan(v_b.pos.x) ||
        std::isnan(v_c.pos.x)) {
      continue;
    }
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
auto set_pixel_none(Vertex &, Image &) -> void {}
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
