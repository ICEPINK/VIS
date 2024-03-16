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
    x = (x + 1) / 2.0f * m_image->get_width();
    y = (y + 1) / 2.0f * m_image->get_height();
}
void CpuRenderer::set_pixel(int64_t x, int64_t y, Vertex &vertex) {
    if (x < 0 || static_cast<size_t>(x) >= m_width) {
        return;
    }
    if (y < 0 || static_cast<size_t>(y) >= m_height) {
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

    float alpha = (b.position.y - a.position.y) / (b.position.x - a.position.x);

    if (alpha * alpha > 1.0) {
        if (a.position.y > b.position.y) {
            std::swap(a, b);
        }

        alpha = (b.position.x - a.position.x) / (b.position.y - a.position.y);
        float x = a.position.x;
        const int64_t a_y = static_cast<int64_t>(a.position.y);
        const int64_t b_y = static_cast<int64_t>(b.position.y);
        for (int64_t y = a_y; y <= b_y; ++y) {
            float t = (y - a_y) / static_cast<float>(b_y - a_y);
            Vertex vertex = Vertex::interpolate(t, a, b);
            set_pixel(static_cast<int64_t>(x), y, vertex);
            x += alpha;
        }
    } else {
        if (a.position.x > b.position.x) {
            std::swap(a, b);
        }

        float y = a.position.y;
        const int64_t a_x = static_cast<int64_t>(a.position.x);
        const int64_t b_x = static_cast<int64_t>(b.position.x);
        for (int64_t x = a_x; x <= b_x; ++x) {
            float t = (x - a_x) / static_cast<float>(b_x - a_x);
            Vertex vertex = Vertex::interpolate(t, a, b);
            set_pixel(x, static_cast<int64_t>(y), vertex);
            y += alpha;
        }
    }
}

bool CpuRenderer::fast_clip(const std::vector<Vertex> &vertices,
                            Topology topology) const {

    switch (topology) {
    case Topology::Point:
        break;
    case Topology::Line: {
        const Vertex &a = vertices.at(0);
        const Vertex &b = vertices.at(1);

        if (a.position.x <= -a.position.w && b.position.x <= -b.position.w) {
            return true;
        }
        if (a.position.x >= a.position.w && b.position.x >= b.position.w) {
            return true;
        }
        if (a.position.y <= -a.position.w && b.position.y <= -b.position.w) {
            return true;
        }
        if (a.position.y >= a.position.w && b.position.y >= b.position.w) {
            return true;
        }
        if (a.position.z <= 0 && b.position.z <= 0) {
            return true;
        }
        if (a.position.z >= a.position.w && b.position.z >= b.position.w) {
            return true;
        }
    }

    break;
    case Topology::Triangle: {

        const Vertex &a = vertices.at(0);
        const Vertex &b = vertices.at(1);
        const Vertex &c = vertices.at(2);

        if (a.position.x <= -a.position.w && b.position.x <= -b.position.w &&
            c.position.x <= -c.position.w) {
            return true;
        }
        if (a.position.x >= a.position.w && b.position.x >= b.position.w &&
            c.position.x >= c.position.w) {
            return true;
        }
        if (a.position.y <= -a.position.w && b.position.y <= -b.position.w &&
            c.position.y <= -c.position.w) {
            return true;
        }
        if (a.position.y >= a.position.w && b.position.y >= b.position.w &&
            c.position.y >= c.position.w) {
            return true;
        }
        if (a.position.z <= 0 && b.position.z <= 0 && c.position.z <= 0) {
            return true;
        }
        if (a.position.z >= a.position.w && b.position.z >= b.position.w &&
            c.position.z >= c.position.w) {
            return true;
        }
    }

    break;
    }

    return false;
}

void CpuRenderer::clip_triangle_z(std::vector<Vertex> &vertices) const {
    Vertex &a = vertices.at(0);
    Vertex &b = vertices.at(1);
    Vertex &c = vertices.at(2);

    if (a.position.z > b.position.z) {
        std::swap(a, b);
    }
    if (b.position.z > c.position.z) {
        std::swap(b, c);
    }
    if (a.position.z > b.position.z) {
        std::swap(a, b);
    }

    if (b.position.z <= 0) {
        const float t_cb = (0 - c.position.z) / (b.position.z - c.position.z);
        b = Vertex::interpolate(t_cb, c, b);

        const float t_ca = (0 - c.position.z) / (a.position.z - c.position.z);
        a = Vertex::interpolate(t_ca, c, a);

    } else if (a.position.z <= 0) {
        vertices.push_back(c);
        vertices.push_back(b);

        Vertex &a_new = vertices.at(0);
        Vertex &b_new = vertices.at(1);
        Vertex &c_new = vertices.at(2);

        const float t_ba =
            (0 - b_new.position.z) / (a_new.position.z - b_new.position.z);
        b_new = Vertex::interpolate(t_ba, b_new, a_new);

        const float t_ca =
            (0 - c_new.position.z) / (a_new.position.z - c_new.position.z);
        a_new = Vertex::interpolate(t_ca, c_new, a_new);

        vertices.push_back(b_new);
    }
}

void CpuRenderer::render_triangle(Vertex &a, Vertex &b, Vertex &c) {
    std::vector<Vertex> vertices = {a, b, c};

    if (fast_clip(vertices, Topology::Triangle)) {
        return;
    }

    clip_triangle_z(vertices);

    // Dehomog
    std::for_each(vertices.begin(), vertices.end(), [](Vertex &vertex) {
        vertex = vertex * (1.0f / vertex.position.w);
    });

    for (size_t i = 0; i < vertices.size(); i += 3) {
        rasterize_triangle(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
}

void CpuRenderer::clip_line_z(std::vector<Vertex> &vertices) const {

    Vertex &a = vertices.at(0);
    Vertex &b = vertices.at(1);

    if (a.position.z > b.position.z) {
        std::swap(a, b);
    }

    if (a.position.z <= 0) {
        const float t_ba = (0 - b.position.z) / (a.position.z - b.position.z);
        a = Vertex::interpolate(t_ba, b, a);
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

    const int64_t start_y_ab =
        std::max(static_cast<int64_t>(0), static_cast<int64_t>(a.position.y));
    const int64_t end_y_ab = std::min(static_cast<int64_t>(m_height - 1),
                                      static_cast<int64_t>(b.position.y));

    for (int64_t y = start_y_ab; y < end_y_ab; ++y) {

        float ab_t = (y - a.position.y) / (b.position.y - a.position.y);
        Vertex ab = Vertex::interpolate(ab_t, a, b);

        float ac_t = (y - a.position.y) / (c.position.y - a.position.y);
        Vertex ac = Vertex::interpolate(ac_t, a, c);

        if (ab.position.x > ac.position.x) {
            std::swap(ab, ac);
        }

        const int64_t start_x_ab_ac = std::max(
            static_cast<int64_t>(0), static_cast<int64_t>(ab.position.x));
        const int64_t end_x_ab_ac =
            std::min(static_cast<int64_t>(m_width - 1),
                     static_cast<int64_t>(ac.position.x));

        for (int64_t x = start_x_ab_ac; x <= end_x_ab_ac; ++x) {

            float ab_ac_t =
                (x - ab.position.x) / (ac.position.x - ab.position.x);
            Vertex ab_ac = Vertex::interpolate(ab_ac_t, ab, ac);

            set_pixel(x, y, ab_ac);
        }
    }

    const int64_t start_y_bc =
        std::max(static_cast<int64_t>(0), static_cast<int64_t>(b.position.y));
    const int64_t end_y_bc = std::min(static_cast<int64_t>(m_height - 1),
                                      static_cast<int64_t>(c.position.y));

    for (int64_t y = start_y_bc; y <= end_y_bc; ++y) {

        float bc_t = (y - b.position.y) / (c.position.y - b.position.y);
        Vertex bc = Vertex::interpolate(bc_t, b, c);

        float ac_t = (y - a.position.y) / (c.position.y - a.position.y);
        Vertex ac = Vertex::interpolate(ac_t, a, c);

        if (bc.position.x > ac.position.x) {
            std::swap(bc, ac);
        }

        const int64_t start_x_bc_ac = std::max(
            static_cast<int64_t>(0), static_cast<int64_t>(bc.position.x));
        const int64_t end_x_bc_ac =
            std::min(static_cast<int64_t>(m_width - 1),
                     static_cast<int64_t>(ac.position.x));

        for (int64_t x = start_x_bc_ac; x <= end_x_bc_ac; ++x) {

            float bc_ac_t =
                (x - bc.position.x) / (ac.position.x - bc.position.x);
            Vertex bc_ac = Vertex::interpolate(bc_ac_t, bc, ac);

            set_pixel(x, y, bc_ac);
        }
    }
}

void CpuRenderer::render_line(Vertex &a, Vertex &b) {
    std::vector<Vertex> vertices = {a, b};

    if (fast_clip(vertices, Topology::Line)) {
        return;
    }

    clip_line_z(vertices);

    // dehomog
    std::for_each(vertices.begin(), vertices.end(), [](Vertex &vertex) {
        vertex = vertex * (1.0f / vertex.position.w);
    });

    for (size_t i = 0; i < vertices.size(); i += 2) {
        rasterize_line(vertices[i], vertices[i + 1]);
    }
}

void CpuRenderer::render_solid(Solid &solid) {

    SolidData &solid_data = solid.data;

    glm::mat4 model_matrix{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    glm::mat4 view_matrix = m_scene_info_ref.camera->get_view_matrix();
    glm::mat4 projection_matrix =
        m_scene_info_ref.camera->get_projection_matrix();

    glm::mat4 matrix =
        projection_matrix * view_matrix * model_matrix * solid_data.matrix;

    std::vector<Vertex> transformed_vertices;
    transformed_vertices.reserve(solid_data.vertices.size());

    for (Vertex &vertex : solid_data.vertices) {
        Vertex new_vertex{matrix * vertex.position, vertex.color, vertex.uv,
                          vertex.one};

        transformed_vertices.push_back(new_vertex);
    }

    for (Layout &layout : solid_data.leyout) {
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

CpuRenderer::CpuRenderer(SceneInfo &scene_info) : m_scene_info_ref(scene_info) {
    PerspectiveData camera_data;
    camera_data.position = {-6.0f, 0.0f, 2.0f};
    camera_data.look_direction = {1.0f, 0.0f, 0.0f};
    camera_data.up_direction = {0.0f, 0.0f, 1.0f};
    camera_data.near_plane = {0.1f};
    camera_data.far_plane = {10.0f};
    camera_data.fov = {glm::pi<float>() / 180.0f * 90.0f};
    camera_data.aspect_ratio = {1.0f};

    scene_info.camera = std::make_unique<PerspectiveCamera>(camera_data);
}
CpuRenderer::~CpuRenderer() {}

void *CpuRenderer::render_image(const size_t width, const size_t height) {
    if (width == 0 || height == 0) {
        return nullptr;
    }

    if (m_width != width || m_height != height) {
        m_image = std::make_unique<Image>(width, height);
        m_depth_buffer = std::make_unique<DepthBuffer>(width, height);

        m_scene_info_ref.camera->set_aspect_ratio(width /
                                                  static_cast<float>(height));

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
