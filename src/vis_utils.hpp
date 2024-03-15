#pragma once

#include <chrono>
#include <cstdint>

#include <glm/glm.hpp>

#include "camera.hpp"

namespace Vis {

typedef glm::vec4 ColorRGBA32f;

struct ColorRGBA8i {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct SceneInfo {
    ColorRGBA32f clear_color{0.02, 0.02, 0.02, 1.0};
    std::chrono::duration<double> last_render{0};
    glm::mat4 model_matrix{glm::mat4(1.0)};
    std::unique_ptr<PerspectiveCamera> camera;
};

struct AppInfo {
    void *view_image_ptr{nullptr};
    size_t fps{0};
    uint32_t view_texture_id{0};
    float view_width{0};
    float view_height{0};
    bool alt_mode{0};
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

} // namespace Vis
