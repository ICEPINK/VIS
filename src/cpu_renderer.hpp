#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "depth_buffer.hpp"
#include "image.hpp"

namespace Vis {

struct Camera {
    glm::mat4 view_matrix;
    glm::vec3 position;
    glm::vec1 azimuth;
    glm::vec1 alitude;
};

struct Vertex {
    glm::vec4 position;
    glm::vec4 color;
    glm::vec2 uv;
    float one{1.0};

    Vertex operator+(const Vertex &vertex) const {
        return {position + vertex.position, color + vertex.color,
                uv + vertex.uv, one + vertex.one};
    }

    Vertex operator+(const float f) const {
        return {position + f, color + f, uv + f, one + f};
    }

    Vertex operator-(const Vertex &vertex) const {
        return {position - vertex.position, color - vertex.color,
                uv - vertex.uv, one - vertex.one};
    }

    Vertex operator-(const float f) const {
        return {position - f, color - f, uv - f, one - f};
    }

    Vertex operator*(const Vertex &vertex) const {
        return {position * vertex.position, color * vertex.color,
                uv * vertex.uv, one * vertex.one};
    }

    Vertex operator*(const float f) const {
        return {position * f, color * f, uv * f, one * f};
    }

    static Vertex interpolate(const float t, const Vertex &a, const Vertex &b) {
        if (t < 0.0) {
            return a;
        } else if (t > 1.0) {
            return b;
        } else {
            return (a * (1 - t)) + (b * t);
        }
    }
};

enum class Topology { Point, Line, Triangle };

struct Layout {
    Topology topology;
    size_t start;
    size_t count;
};

struct Solid {
    std::string name;
    glm::mat4 solid_matrix;
    std::vector<Layout> leyout;
    std::vector<Vertex> vertices;
    std::vector<size_t> indices;
};

class CpuRenderer {
  public:
    CpuRenderer(SceneInfo &scene_info);
    ~CpuRenderer();

    void *render_image(const size_t width, const size_t height);

  private:
    void render_solid(Solid &solid);
    // void render_point(Vertex &a);
    void render_line(Vertex &a, Vertex &b);
    void render_triangle(Vertex &a, Vertex &b, Vertex &c);
    // void rasterize_point(Vertex &a);
    void rasterize_line(Vertex &a, Vertex &b);
    void rasterize_triangle(Vertex &a, Vertex &b, Vertex &c);
    void trasform_to_screen(Vertex &vertex);
    Solid parse_obj(std::string object_string);
    void set_pixel(int64_t x, int64_t y, Vertex &vertex);

  private:
    size_t m_width{0};
    size_t m_height{0};
    std::unique_ptr<Image> m_image;
    std::unique_ptr<DepthBuffer> m_depth_buffer;
    SceneInfo &m_scene_info_ref;
};

} // namespace Vis
