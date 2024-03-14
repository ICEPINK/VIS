#include "cpu_renderer.hpp"

#include <algorithm>
#include <chrono>

#include <glm/ext.hpp>

#include "solids/axis.hpp"
#include "solids/solid.hpp"
#include "solids/square.hpp"

namespace Vis {

void CpuRenderer::trasform_to_screen(Vertex &vertex) {
    float &x = vertex.position.x;
    float &y = vertex.position.y;
    x = (x + 1) / 2.0 * m_image->get_width();
    y = (y + 1) / 2.0 * m_image->get_height();
}

void CpuRenderer::set_pixel(int64_t x, int64_t y, Vertex &vertex) {
    if (x < 0 || static_cast<size_t>(x) > m_width) {
        return;
    }
    if (y < 0 || static_cast<size_t>(y) > m_height) {
        return;
    }

    y = m_height - 1 - y;

    if (vertex.position.z < m_depth_buffer->get_depth(x, y)) {
        m_depth_buffer->set_depth(x, y, vertex.position.z);
        m_image->set_pixel(x, y, vertex.color * (1.0f / vertex.one));
    }
}

void CpuRenderer::rasterize_line(Vertex &a, Vertex &b) {
    // DDA
    trasform_to_screen(a);
    trasform_to_screen(b);

    Vertex vertex_atr = a;

    float alpha = (b.position.y - a.position.y) / (b.position.x - a.position.x);

    if (alpha * alpha > 1.0) {
        if (a.position.y > b.position.y) {
            std::swap(a, b);
        }

        alpha = (b.position.x - a.position.x) / (b.position.y - a.position.y);
        float x = a.position.x;
        for (int y = a.position.y; y <= b.position.y; ++y) {
            set_pixel(x, y, vertex_atr);
            x += alpha;
        }
    } else {
        if (a.position.x > b.position.x) {
            std::swap(a, b);
        }

        float y = a.position.y;
        for (int x = a.position.x; x <= b.position.x; ++x) {
            set_pixel(x, y, vertex_atr);
            y += alpha;
        }
    }
}

void CpuRenderer::render_triangle(Vertex &a, Vertex &b, Vertex &c) {
    std::vector<Vertex> vertices = {a, b, c};

    // clip_before_dehomog();

    // dehomog
    std::for_each(vertices.begin(), vertices.end(), [](Vertex &vertex) {
        vertex = vertex * (1.0 / vertex.position.w);
    });

    // clip_after_dehomog();

    for (size_t i = 0; i < vertices.size(); i += 3) {
        rasterize_triangle(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
}

void CpuRenderer::rasterize_triangle(Vertex &a, Vertex &b, Vertex &c) {
    trasform_to_screen(a);
    trasform_to_screen(b);
    trasform_to_screen(c);

    // a..y < b..y < c..y
    if (a.position.y > b.position.y) {
        std::swap(a, b);
    }
    if (b.position.y > c.position.y) {
        std::swap(b, c);
    }
    if (a.position.y > b.position.y) {
        std::swap(a, b);
    }

    for (size_t y = std::max(a.position.y, 0.0f);
         y < std::min(b.position.y, static_cast<float>(m_height)); ++y) {

        float ab_t = (y - a.position.y) / (b.position.y - a.position.y);
        Vertex ab = Vertex::interpolate(ab_t, a, b);

        float ac_t = (y - a.position.y) / (c.position.y - a.position.y);
        Vertex ac = Vertex::interpolate(ac_t, a, c);

        if (ab.position.x > ac.position.x) {
            std::swap(ab, ac);
        }

        for (size_t x = std::max(ab.position.x, 0.0f);
             x <= std::min(ac.position.x, static_cast<float>(m_width)); ++x) {

            float ab_ac_t =
                (x - ab.position.x) / (ac.position.x - ab.position.x);
            Vertex ab_ac = Vertex::interpolate(ab_ac_t, ab, ac);

            set_pixel(x, y, ab_ac);
        }
    }

    for (size_t y = std::max(b.position.y, 0.0f);
         y <= std::min(c.position.y, static_cast<float>(m_height)); ++y) {

        float bc_t = (y - b.position.y) / (c.position.y - b.position.y);
        Vertex bc = Vertex::interpolate(bc_t, b, c);

        float ac_t = (y - a.position.y) / (c.position.y - a.position.y);
        Vertex ac = Vertex::interpolate(ac_t, a, c);

        if (bc.position.x > ac.position.x) {
            std::swap(bc, ac);
        }

        for (size_t x = std::max(bc.position.x, 0.0f);
             x <= std::min(ac.position.x, static_cast<float>(m_width)); ++x) {

            float bc_ac_t =
                (x - bc.position.x) / (ac.position.x - bc.position.x);
            Vertex bc_ac = Vertex::interpolate(bc_ac_t, bc, ac);

            set_pixel(x, y, bc_ac);
        }
    }
}

void CpuRenderer::render_line(Vertex &a, Vertex &b) {
    std::vector<Vertex> vertices = {a, b};

    // clip_before_dehomog();

    // dehomog
    std::for_each(vertices.begin(), vertices.end(), [](Vertex &vertex) {
        vertex = vertex * (1.0 / vertex.position.w);
    });

    // clip_after_dehomog();

    for (size_t i = 0; i < vertices.size(); i += 2) {
        rasterize_line(vertices[i], vertices[i + 1]);
    }
}

void CpuRenderer::render_solid(Solid &solid) {

    SolidData &solid_data = solid.data;

    glm::mat4 model_matrix{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    PerspectiveData perspective_data{{2.0, 2.0, 2.0},
                                     {0.0, 0.0, 0.0},
                                     {0.0, 0.0, 1.0},
                                     1.0f,
                                     100.0f,
                                     3.1415f / 2.0f,
                                     m_width / static_cast<float>(m_height)};

    PerspectiveCamera camera{perspective_data};

    glm::mat4 view_matrix = camera.get_view_matrix();
    glm::mat4 projection_matrix = camera.get_projection_matrix();

    glm::mat4 matrix =
        projection_matrix * view_matrix * model_matrix * solid_data.matrix;

    std::vector<Vertex> transformed_vertices;
    transformed_vertices.reserve(solid_data.vertices.size());

    for (Vertex &vertex : solid_data.vertices) {
        Vertex new_vertex{matrix * vertex.position, vertex.color, vertex.uv,
                          vertex.one};

        transformed_vertices.push_back(new_vertex);
    }

    for (Layout layout : solid_data.leyout) {
        switch (layout.topology) {
        case Topology::Point:
            break;
        case Topology::Line:
            for (size_t i = 0; i < layout.count; ++i) {
                size_t &index_a = solid_data.indices[layout.start + (i * 2)];
                size_t &index_b =
                    solid_data.indices[layout.start + (i * 2) + 1];
                Vertex &vertex_a = transformed_vertices[index_a];
                Vertex &vertex_b = transformed_vertices[index_b];

                render_line(vertex_a, vertex_b);
            }
            break;
        case Topology::Triangle:
            for (size_t i = 0; i < layout.count; ++i) {
                size_t &index_a = solid_data.indices[layout.start + (i * 3)];
                size_t &index_b =
                    solid_data.indices[layout.start + (i * 3) + 1];
                size_t &index_c =
                    solid_data.indices[layout.start + (i * 3) + 2];
                Vertex &vertex_a = transformed_vertices[index_a];
                Vertex &vertex_b = transformed_vertices[index_b];
                Vertex &vertex_c = transformed_vertices[index_c];

                render_triangle(vertex_a, vertex_b, vertex_c);
            }
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
        m_depth_buffer = std::make_unique<DepthBuffer>(width, height);
        m_width = width;
        m_height = height;
    }

    Square square("Square");
    Axis axis("Axis");

    auto start = std::chrono::high_resolution_clock::now();

    m_image->clear(m_scene_info_ref.clear_color);
    m_depth_buffer->clear(1.0);

    // render_solid(square);
    render_solid(axis);

    auto end = std::chrono::high_resolution_clock::now();
    m_scene_info_ref.last_render = end - start;

    return m_image->get_image_buffer_ptr();
}

} // namespace Vis
