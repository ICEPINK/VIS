#include "cpu_renderer.hpp"

#include <algorithm>
#include <chrono>

namespace Vis {

Solid CpuRenderer::parse_obj(std::string object_string) {
    Solid solid{
        object_string,
        glm::mat4{},
        {{Topology::Line, 0, 4}},
        {
            Vertex({-0.5, -0.5, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {0.0, 0.0}),
            Vertex({0.5, -0.5, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 0.0}),
            Vertex({-0.5, 0.5, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {0.0, 1.0}),
            Vertex({0.5, 0.5, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0}),
        },
        {0, 1, 1, 3, 3, 2, 2, 0}};
    return solid;
}

void CpuRenderer::trasform_to_screen(Vertex &vertex) {
    float &x = vertex.position.x;
    float &y = vertex.position.y;
    x = (x + 1) / 2.0 * m_image->get_width();
    y = (y + 1) / 2.0 * m_image->get_height();
}

// void rasterize_point(Vertex a) {}
void CpuRenderer::rasterize_line(Vertex &a, Vertex &b) {
    trasform_to_screen(a);
    trasform_to_screen(b);

    ColorRGBA32f color = a.color;

    float alpha = (b.position.y - a.position.y) / (b.position.x - a.position.x);

    if (alpha * alpha > 1.0) {
        if (a.position.y > b.position.y) {
            std::swap(a, b);
        }

        alpha = (b.position.x - a.position.x) / (b.position.y - a.position.y);
        float x = a.position.x;
        for (size_t y = a.position.y; y <= b.position.y; ++y) {
            m_image->set_pixel(x, y, color);
            x += alpha;
        }
    } else {
        if (a.position.x > b.position.x) {
            std::swap(a, b);
        }

        float y = a.position.y;
        for (size_t x = a.position.x; x <= b.position.x; ++x) {
            m_image->set_pixel(x, y, color);
            y += alpha;
        }
    }
}
// void rasterize_triangle(Vertex a, Vertex b, Vertex c) {}

void CpuRenderer::render_line(Vertex &a, Vertex &b) {
    std::vector<Vertex> vertices = {a, b};

    // clip_before_dehomog();

    // dehomog
    std::for_each(vertices.begin(), vertices.end(), [](Vertex &vertex) {
        vertex = vertex * (1 / vertex.position.w);
    });

    // clip_after_dehomog();

    for (size_t i = 0; i < vertices.size(); i += 2) {
        rasterize_line(vertices[i], vertices[i + 1]);
    }
}

void CpuRenderer::render_solid(Solid &solid) {
    glm::mat4 model_matrix{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    Camera camera{{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, {}, {}, {}};
    glm::mat4 projection_matrix{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    glm::mat4 matrix = projection_matrix * camera.view_matrix * model_matrix;

    std::vector<Vertex> transformed_vertices;
    transformed_vertices.reserve(solid.vertices.size());

    for (Vertex &vertex : solid.vertices) {
        Vertex new_vertex{matrix * vertex.position, vertex.color, vertex.uv,
                          vertex.one};

        transformed_vertices.push_back(new_vertex);
    }

    for (Layout layout : solid.leyout) {
        switch (layout.topology) {
        case Topology::Point:
            break;
        case Topology::Line:
            for (size_t i = 0; i < layout.count; ++i) {
                render_line(
                    transformed_vertices[solid.indices[layout.start + (i * 2)]],
                    transformed_vertices[solid.indices[layout.start + (i * 2) +
                                                       1]]);
            }
            break;
        case Topology::Triangle:
            break;
        }
    }
}

CpuRenderer::CpuRenderer(SceneInfo &scene_info)
    : m_scene_info_ref(scene_info) {}
CpuRenderer::~CpuRenderer() {}

void *CpuRenderer::render_image(const size_t width, const size_t height) {
    if (width == 0 || height == 0) {
        return nullptr;
    }

    if (m_width != width || m_height != height) {
        m_image = std::make_unique<Image>(width, height);
        m_width = width;
        m_height = height;
    }

    Solid solid = parse_obj("Square");

    auto start = std::chrono::high_resolution_clock::now();

    m_image->clear(m_scene_info_ref.clear_color);
    render_solid(solid);

    auto end = std::chrono::high_resolution_clock::now();
    m_scene_info_ref.last_render = end - start;

    return m_image->get_image_buffer_ptr();
}

} // namespace Vis
