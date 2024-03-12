#pragma once

#include <memory>

#include <glm/glm.hpp>

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

    Vertex operator+(Vertex &vertex) {
        return {position + vertex.position, color + vertex.color,
                uv + vertex.uv, one + vertex.one};
    }

    Vertex operator+(float f) {
        return {position + f, color + f, uv + f, one + f};
    }

    Vertex operator-(Vertex &vertex) {
        return {position - vertex.position, color - vertex.color,
                uv - vertex.uv, one - vertex.one};
    }

    Vertex operator-(float f) {
        return {position - f, color - f, uv - f, one - f};
    }

    Vertex operator*(Vertex &vertex) {
        return {position * vertex.position, color * vertex.color,
                uv * vertex.uv, one * vertex.one};
    }

    Vertex operator*(float f) {
        return {position * f, color * f, uv * f, one * f};
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
    void render_solid(Solid &solid);
    void render_line(Vertex &a, Vertex &b);
    void rasterize_line(Vertex &a, Vertex &b);
    void trasform_to_screen(Vertex &vertex);
    Solid parse_obj(std::string object_string);

  private:
    size_t m_width{0};
    size_t m_height{0};
    std::unique_ptr<Image> m_image;
    SceneInfo &m_scene_info_ref;
};

} // namespace Vis
