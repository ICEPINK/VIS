#pragma once

#include <glm/glm.hpp>

namespace Vis {

struct Vertex {
    glm::vec4 position;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec2 uv{0.0f, 0.0f};
    float one{1.0f};

    constexpr inline Vertex operator+(const Vertex &vertex) const {
        return {position + vertex.position, color + vertex.color,
                uv + vertex.uv, one + vertex.one};
    }

    constexpr inline Vertex operator+(const float f) const {
        return {position + f, color + f, uv + f, one + f};
    }

    constexpr inline Vertex operator-(const Vertex &vertex) const {
        return {position - vertex.position, color - vertex.color,
                uv - vertex.uv, one - vertex.one};
    }

    constexpr inline Vertex operator-(const float f) const {
        return {position - f, color - f, uv - f, one - f};
    }

    constexpr inline Vertex operator*(const Vertex &vertex) const {
        return {position * vertex.position, color * vertex.color,
                uv * vertex.uv, one * vertex.one};
    }

    constexpr inline Vertex operator*(const float f) const {
        return {position * f, color * f, uv * f, one * f};
    }

    constexpr inline static Vertex interpolate(const float t, const Vertex &a,
                                               const Vertex &b) {
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
