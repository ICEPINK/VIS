#pragma once

#include <glm/glm.hpp>

namespace Vis {

struct Vertex {
    glm::dvec4 pos;
    glm::dvec4 col{1.0f, 1.0f, 1.0f, 1.0f};
    double one{1.0};

    constexpr inline Vertex operator+(const Vertex &vertex) const {
        return {pos + vertex.pos, col + vertex.col, one + vertex.one};
    }

    constexpr inline Vertex operator+(const double f) const {
        return {pos + f, col + f, one + f};
    }

    constexpr inline Vertex operator-(const Vertex &vertex) const {
        return {pos - vertex.pos, col - vertex.col, one - vertex.one};
    }

    constexpr inline Vertex operator-(const double f) const {
        return {pos - f, col - f, one - f};
    }

    constexpr inline Vertex operator*(const Vertex &vertex) const {
        return {pos * vertex.pos, col * vertex.col, one * vertex.one};
    }

    constexpr inline Vertex operator*(const double f) const {
        return {pos * f, col * f, one * f};
    }

    constexpr inline static Vertex interpolate(const double t, const Vertex &a,
                                               const Vertex &b) {
        if (t <= 0.0) {
            return a;
        }
        if (t >= 1.0) {
            return b;
        }
        return (a * (1 - t)) + (b * t);
    }
};

} // namespace Vis
