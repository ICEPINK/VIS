#include "cpu_renderer.hpp"

#include <array>
#include <fstream>
#include <vector>

namespace Vis {

SceneObject load_obj(const std::string &filepath) {
    std::ifstream stream(filepath);

    SceneObject scene_object;

    std::string line;
    while (getline(stream, line)) {
        size_t separator = line.find(" ");

        if (line.find(" ") == static_cast<size_t>(-1)) {
            continue;
        }

        const std::string prefix = line.substr(0, separator);

        if (prefix == "o") {
            scene_object.m_name = line.substr(separator + 1, line.back());
        } else if (prefix == "v") {
            std::array<float, 3> vertices;

            int i = 0;

            while (separator != static_cast<size_t>(-1)) {
                vertices[i] = std::stof(
                    line.substr(separator + 1, line.find(" ", separator + 1)));

                separator = line.find(" ", separator + 1);

                ++i;
            }

            glm::vec4 vector = {vertices[0], vertices[1], vertices[2], 1};

            scene_object.m_positions.push_back(vector);
        } else if (prefix == "f") {
            std::array<size_t, 3> indices;

            int i = 0;

            while (separator != static_cast<size_t>(-1)) {
                indices[i] =
                    std::stoul(line.substr(separator + 1,
                                           line.find(" ", separator + 1))) -
                    1;

                separator = line.find(" ", separator + 1);
                ++i;
            }

            scene_object.m_indices.push_back(indices);
        } else {
            continue;
        }
    }

    return scene_object;
}
void draw_line(glm::vec2 start, glm::vec2 end, size_t width, size_t height,
               Image *image) {
    int d_x = end.x - start.x;
    int d_y = end.y - start.y;

    const float alpha = d_y / static_cast<float>(d_x);

    if ((alpha * alpha) <= 1) {
        if (start.x > end.x) {
            glm::vec2 t = end;
            end = start;
            start = t;
        }

        for (int i = start.x; i <= end.x; ++i) {
            const float alpha_x = d_y / static_cast<float>(d_x);

            const float b = start.y - alpha_x * start.x;

            const int r_y = alpha_x * i + b;

            if (i >= 0 && i < static_cast<int>(width) && r_y >= 0 &&
                r_y < static_cast<int>(height)) {
                image->set_pixel(i, r_y, {1.0, 0.0, 1.0, 1.0});
            }
        }
    } else {
        if (start.y > end.y) {
            glm::vec2 t = end;
            end = start;
            start = t;
        }

        for (int i = start.y; i <= end.y; ++i) {
            const float alpha_y = d_x / static_cast<float>(d_y);

            const float b = start.x - alpha_y * start.y;

            const int r_x = alpha_y * i + b;

            if (r_x >= 0 && r_x < static_cast<int>(width) && i >= 0 &&
                i < static_cast<int>(height)) {
                image->set_pixel(r_x, i, {1.0, 0.0, 1.0, 1.0});
            }
        }
    }
}

void draw_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, size_t width,
                   size_t height, uint8_t *texture_buffer) {
    if (a.y > b.y) {
        glm::vec3 temp = a;
        a = b;
        b = temp;
    }
    if (b.y > c.y) {
        glm::vec3 temp = b;
        b = c;
        c = temp;
    }
    if (a.y > b.y) {
        glm::vec3 temp = a;
        a = b;
        b = temp;
    }

    int start_y;
    int end_y;

    if (a.y < 0) {
        start_y = 0;
    } else {
        start_y = static_cast<int>(a.y);
    }
    if (b.y > height - 1) {
        end_y = height - 1;
    } else {
        end_y = static_cast<int>(b.y);
    }

    for (int y = start_y; y < end_y; ++y) {
        float t_ac = (y - a.y) / ((int)c.y - (int)a.y);
        float t_ab = (y - a.y) / ((int)b.y - (int)a.y);

        if ((int)c.y - (int)a.y == 0) {
            continue;
        }
        if ((int)b.y - (int)a.y == 0) {
            continue;
        }

        glm::vec3 ac = (1 - t_ac) * a + t_ac * c;
        glm::vec3 ab = (1 - t_ab) * a + t_ab * b;

        if (ac.x < ab.x) {
            glm::vec3 temp = ab;
            ab = ac;
            ac = temp;
        }

        int start_x;
        int end_x;

        if (ab.y < 0) {
            start_x = 0;
        } else {
            start_x = static_cast<int>(ab.x);
        }
        if (ac.y > width - 1) {
            end_x = width - 1;
        } else {
            end_x = static_cast<int>(ac.x);
        }

        for (int x = start_x; x <= end_x; ++x) {
            // float t = (x - ab.x) / (ac.x - ab.x);

            // glm::vec3 abac = (1 - t) * ab + t * ac;

            size_t coord = x * 4 + width * y * 4;

            if (x >= 0 && x < static_cast<int>(width) && y >= 0 &&
                y < static_cast<int>(height)) {
                texture_buffer[coord] = 255;
                texture_buffer[coord + 1] = 255;
                texture_buffer[coord + 2] = 255;
                texture_buffer[coord + 3] = 255;
            }
        }
    }

    if (b.y < 0) {
        start_y = 0;
    } else {
        start_y = static_cast<int>(b.y);
    }
    if (c.y > height - 1) {
        end_y = height - 1;
    } else {
        end_y = static_cast<int>(c.y);
    }

    for (int y = start_y; y <= end_y; ++y) {
        float t_ac = (y - a.y) / ((int)c.y - (int)a.y);
        float t_bc = (y - b.y) / ((int)c.y - (int)b.y);

        if ((int)c.y - (int)a.y == 0) {
            continue;
        }
        if ((int)c.y - (int)b.y == 0) {
            continue;
        }

        glm::vec3 ac = (1 - t_ac) * a + t_ac * c;
        glm::vec3 bc = (1 - t_bc) * b + t_bc * c;

        if (ac.x < bc.x) {
            glm::vec3 temp = bc;
            bc = ac;
            ac = temp;
        }

        int start_x;
        int end_x;

        if (bc.y < 0) {
            start_x = 0;
        } else {
            start_x = static_cast<int>(bc.x);
        }
        if (ac.y > width - 1) {
            end_x = width - 1;
        } else {
            end_x = static_cast<int>(ac.x);
        }

        for (int x = start_x; x <= end_x; ++x) {
            // float t = (x - ab.x) / (ac.x - ab.x);

            // glm::vec3 abac = (1 - t) * ab + t * ac;

            size_t coord = x * 4 + width * y * 4;

            if (x >= 0 && x < static_cast<int>(width) && y >= 0 &&
                y < static_cast<int>(height)) {
                texture_buffer[coord] = 255;
                texture_buffer[coord + 1] = 255;
                texture_buffer[coord + 2] = 255;
                texture_buffer[coord + 3] = 255;
            }
        }
    }
}

void draw_triangles(const std::vector<glm::vec4> &vertices,
                    const std::vector<std::array<size_t, 3>> &indices,
                    size_t count, size_t width, size_t height, Image *image) {
    for (size_t i = 0; i < count; ++i) {
        draw_line({vertices[indices[i][0]].x, vertices[indices[i][0]].y},
                  {vertices[indices[i][1]].x, vertices[indices[i][1]].y}, width,
                  height, image);
        draw_line({vertices[indices[i][1]].x, vertices[indices[i][1]].y},
                  {vertices[indices[i][2]].x, vertices[indices[i][2]].y}, width,
                  height, image);
        draw_line({vertices[indices[i][2]].x, vertices[indices[i][2]].y},
                  {vertices[indices[i][0]].x, vertices[indices[i][0]].y}, width,
                  height, image);
    }

    // for(size_t i = 0; i < count; ++i)
    // {
    //     draw_triangle({vertices[indices[i][0]].x, vertices[indices[i][0]].y,
    //                    vertices[indices[i][0]].z},
    //                   {vertices[indices[i][1]].x, vertices[indices[i][1]].y,
    //                    vertices[indices[i][1]].z},
    //                   {vertices[indices[i][2]].x, vertices[indices[i][2]].y,
    //                    vertices[indices[i][2]].z},
    //                   width, height, texture_buffer);
    // }
}
CpuRenderer::CpuRenderer() {}
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

    m_image->clear({0.1, 0.1, 0.1, 1.0});

    SceneObject scene_object = load_obj("./data/obj/Cube.obj");

    float p_aspect_ratio = m_width / static_cast<float>(m_height);
    float p_fov = 90.0 * 3.141526 / 180.0;
    float p_far = 100.0;
    float p_near = 0.1;
    float p_dif_distance = p_near - p_far;

    float x = 0;
    float y = 0;
    float z = -10;

    glm::mat4 model_matrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1};
    glm::mat4 view_matrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    glm::mat4 projection_matrix = {1 / p_aspect_ratio * tan(p_fov / 2),
                                   0,
                                   0,
                                   0,
                                   0,
                                   1 / tan(p_fov / 2),
                                   0,
                                   0,
                                   0,
                                   0,
                                   (p_far + p_near) / p_dif_distance,
                                   2 * p_far * p_near / p_dif_distance,
                                   0,
                                   0,
                                   -1,
                                   0};

    glm::mat4 combined_matrix = projection_matrix * view_matrix * model_matrix;

    std::vector<glm::vec4> trasformed_vertices;
    for (const glm::vec4 &vertex : scene_object.m_positions) {
        glm::vec4 temp_vertex = combined_matrix * vertex;

        // clip

        temp_vertex = temp_vertex / temp_vertex.w;

        temp_vertex.x = (temp_vertex.x + 1) * m_width / 2 + 0;
        temp_vertex.y = (1 - temp_vertex.y) * m_height / 2 + 0;

        trasformed_vertices.push_back(temp_vertex);
    }

    draw_triangles(trasformed_vertices, scene_object.m_indices,
                   scene_object.m_indices.size(), m_width, m_height,
                   m_image.get());

    return m_image->get_image_buffer_ptr();
}

} // namespace Vis
