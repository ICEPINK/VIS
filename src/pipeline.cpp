#include "pipeline.hpp"
////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <memory>
////////////////////////////////////////////////////////////////////////////////
namespace Vis {
////////////////////////////////////////////////////////////////////////////////
Pipeline::Pipeline(PipelineData &data) : m_data_ref(data) {}
////////////////////////////////////////////////////////////////////////////////
Pipeline::~Pipeline() {}
////////////////////////////////////////////////////////////////////////////////
bool Pipeline::fast_clip_line(std::unique_ptr<std::vector<Vertex>> &vertices) {
    const Vertex &a = vertices->at(0);
    const Vertex &b = vertices->at(1);

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

    return false;
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::clip_after_dehomog_line(
    std::unique_ptr<std::vector<Vertex>> &vertices) {

    for (size_t i = 0; i < vertices->size(); i += 2) {
        auto &a = vertices->at(i);
        auto &b = vertices->at(i + 1);

        if (a.position.x <= -1) {
            const auto t = (-1 - a.position.x) / (b.position.x - a.position.x);
            a = Vertex::interpolate(t, a, b);
        }
        if (b.position.x <= -1) {
            const auto t = (-1 - b.position.x) / (a.position.x - b.position.x);
            b = Vertex::interpolate(t, b, a);
        }
        if (a.position.x >= 1) {
            const auto t = (1 - a.position.x) / (b.position.x - a.position.x);
            a = Vertex::interpolate(t, a, b);
        }
        if (b.position.x >= 1) {
            const auto t = (1 - b.position.x) / (a.position.x - b.position.x);
            b = Vertex::interpolate(t, b, a);
        }
        if (a.position.y <= -1) {
            const auto t = (-1 - a.position.y) / (b.position.y - a.position.y);
            a = Vertex::interpolate(t, a, b);
        }
        if (b.position.y <= -1) {
            const auto t = (-1 - b.position.y) / (a.position.y - b.position.y);
            b = Vertex::interpolate(t, b, a);
        }
        if (a.position.y >= 1) {
            const auto t = (1 - a.position.y) / (b.position.y - a.position.y);
            a = Vertex::interpolate(t, a, b);
        }
        if (b.position.y >= 1) {
            const auto t = (1 - b.position.y) / (a.position.y - b.position.y);
            b = Vertex::interpolate(t, b, a);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::clip_before_dehomog_line(
    std::unique_ptr<std::vector<Vertex>> &vertices) {
    Vertex &a = vertices->at(0);
    Vertex &b = vertices->at(1);

    if (a.position.z > b.position.z) {
        std::swap(a, b);
    }

    if (a.position.z <= 0.0f) {
        const float t_ba =
            (0.0f - b.position.z) / (a.position.z - b.position.z);
        a = Vertex::interpolate(t_ba, b, a);
    }
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::rasterization_line_dda(
    std::unique_ptr<std::vector<Vertex>> &vertices,
    [[maybe_unused]] const size_t width, [[maybe_unused]] const size_t height,
    std::function<void(const int64_t x, const int64_t y, const Vertex &vertex)>
        set_pixel) {
    // NOTE: unused width, height
    for (size_t i = 0; i < vertices->size(); i += 2) {
        auto &a = vertices->at(i);
        auto &b = vertices->at(i + 1);

        float alpha =
            (b.position.y - a.position.y) / (b.position.x - a.position.x);

        if (alpha * alpha > 1.0) {
            if (a.position.y > b.position.y) {
                std::swap(a, b);
            }

            alpha =
                (b.position.x - a.position.x) / (b.position.y - a.position.y);
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
}
////////////////////////////////////////////////////////////////////////////////
std::optional<std::unique_ptr<std::vector<Vertex>>>
Pipeline::render(const std::vector<Vertex> &vertices) const {
    auto temp_vertices = std::make_unique<std::vector<Vertex>>(vertices);
    auto &d = m_data_ref;
    d.trasform_vertices_by_matrix(temp_vertices, d.matrix);

    if (d.fast_clip(temp_vertices)) {
        return {};
    }

    d.clip_before_dehomog(temp_vertices);
    d.dehomog_vertices(temp_vertices);
    d.clip_after_dehomog(temp_vertices);
    d.trasform_vertices_onto_viewport(temp_vertices, d.width, d.height);

    if (d.callback) {
        return temp_vertices;
    }

    d.rasterization(temp_vertices, d.width, d.height, d.set_pixel);

    return {};
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::update_matrix() const {
    auto &d = m_data_ref;
    d.matrix = d.matrix_calculation(d.solid_matrix, d.model_matrix,
                                    d.view_matrix, d.proj_matrix);
}
////////////////////////////////////////////////////////////////////////////////
glm::mat4 Pipeline::matrix_calculation_smvp(const glm::mat4 &solid,
                                            const glm::mat4 &model,
                                            const glm::mat4 &view,
                                            const glm::mat4 &proj) {
    return proj * view * model * solid;
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::trasform_vertices_by_matrix_position(
    std::unique_ptr<std::vector<Vertex>> &vertices, const glm::mat4 &matrix) {
    for (Vertex &vertex : *vertices) {
        vertex = {matrix * vertex.position, vertex.color, vertex.uv,
                  vertex.one};
    }
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::dehomog_vertices(
    std::unique_ptr<std::vector<Vertex>> &vertices) {
    std::for_each(vertices->begin(), vertices->end(), [](Vertex &vertex) {
        vertex = vertex * (1.0f / vertex.position.w);
    });
}
////////////////////////////////////////////////////////////////////////////////
bool Pipeline::fast_clip_triangle(
    std::unique_ptr<std::vector<Vertex>> &vertices) {

    const Vertex &a = vertices->at(0);
    const Vertex &b = vertices->at(1);
    const Vertex &c = vertices->at(2);

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

    return false;
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::clip_before_dehomog_triangle(
    std::unique_ptr<std::vector<Vertex>> &vertices) {

    Vertex &a = vertices->at(0);
    Vertex &b = vertices->at(1);
    Vertex &c = vertices->at(2);

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
        vertices->push_back(c);
        vertices->push_back(b);

        Vertex &a_new = vertices->at(0);
        Vertex &b_new = vertices->at(1);
        Vertex &c_new = vertices->at(2);

        const float t_ba =
            (0 - b_new.position.z) / (a_new.position.z - b_new.position.z);
        b_new = Vertex::interpolate(t_ba, b_new, a_new);

        const float t_ca =
            (0 - c_new.position.z) / (a_new.position.z - c_new.position.z);
        a_new = Vertex::interpolate(t_ca, c_new, a_new);

        vertices->push_back(b_new);
    }
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::clip_after_dehomog_triangle(
    [[maybe_unused]] std::unique_ptr<std::vector<Vertex>> &vertices) {
    // TODO: clip by every plane
    return;
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::clip_after_dehomog_none(
    [[maybe_unused]] std::unique_ptr<std::vector<Vertex>> &vertices) {
    return;
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::trasform_vertices_onto_viewport(
    std::unique_ptr<std::vector<Vertex>> &vertices, const size_t widht,
    const size_t height) {
    for (auto &vertex : *vertices) {
        float &x = vertex.position.x;
        float &y = vertex.position.y;
        x = (x + 1) / 2.0f * widht;
        y = (y + 1) / 2.0f * height;
    }
}
////////////////////////////////////////////////////////////////////////////////
void Pipeline::rasterization_triangle_fill(
    std::unique_ptr<std::vector<Vertex>> &vertices, const size_t width,
    const size_t height,
    std::function<void(const int64_t x, const int64_t y, const Vertex &vertex)>
        set_pixel) {
    for (size_t i = 0; i < vertices->size(); i += 3) {
        auto &a = vertices->at(i);
        auto &b = vertices->at(i + 1);
        auto &c = vertices->at(i + 2);

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

        const int64_t start_y_ab = std::max(static_cast<int64_t>(0),
                                            static_cast<int64_t>(a.position.y));
        const int64_t end_y_ab = std::min(static_cast<int64_t>(height - 1),
                                          static_cast<int64_t>(b.position.y));

        for (int64_t y = start_y_ab; y <= end_y_ab; ++y) {

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
                std::min(static_cast<int64_t>(width - 1),
                         static_cast<int64_t>(ac.position.x));

            for (int64_t x = start_x_ab_ac; x <= end_x_ab_ac; ++x) {

                float ab_ac_t =
                    (x - ab.position.x) / (ac.position.x - ab.position.x);
                Vertex ab_ac = Vertex::interpolate(ab_ac_t, ab, ac);

                set_pixel(x, y, ab_ac);
            }
        }

        const int64_t start_y_bc = std::max(static_cast<int64_t>(0),
                                            static_cast<int64_t>(b.position.y));
        const int64_t end_y_bc = std::min(static_cast<int64_t>(height - 1),
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
                std::min(static_cast<int64_t>(width - 1),
                         static_cast<int64_t>(ac.position.x));

            for (int64_t x = start_x_bc_ac; x <= end_x_bc_ac; ++x) {

                float bc_ac_t =
                    (x - bc.position.x) / (ac.position.x - bc.position.x);
                Vertex bc_ac = Vertex::interpolate(bc_ac_t, bc, ac);

                set_pixel(x, y, bc_ac);
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace Vis
