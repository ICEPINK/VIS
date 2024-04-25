#include "pipeline.hpp"

namespace Vis {
namespace Alg {

auto dehomog(std::vector<Vertex> &vertices) -> void {
    for (auto &vertex : vertices) {
        const auto w = vertex.pos.w;
        vertex.pos = {vertex.pos.x / w, vertex.pos.y / w, vertex.pos.z / w,
                      1.0};
    }
}

auto trasform_to_viewport(std::vector<Vertex> &vertices,
                          const Image &image) -> void {
    for (auto &vertex : vertices) {
        vertex.pos.x = ((vertex.pos.x + 1.0) / 2.0) * (image.get_width() - 1);
        vertex.pos.y = ((vertex.pos.y + 1.0) / 2.0) * (image.get_height() - 1);
    }
}

auto rasterize_triangle(std::vector<Vertex> &vertices, Image &image,
                        void (*set_pixel)(const Vertex &vertex,
                                          Image &image)) -> void {
    auto width = image.get_width();
    auto height = image.get_height();

    for (size_t i = 0; i < vertices.size(); i += 3) {
        auto &a = vertices.at(i);
        auto &b = vertices.at(i + 1);
        auto &c = vertices.at(i + 2);

        // a..y < b..y < c..y
        if (a.pos.y > b.pos.y) {
            std::swap(a, b);
        }
        if (b.pos.y > c.pos.y) {
            std::swap(b, c);
        }
        if (a.pos.y > b.pos.y) {
            std::swap(a, b);
        }

        const int64_t start_y_ab =
            std::max(static_cast<int64_t>(0), static_cast<int64_t>(a.pos.y));
        const int64_t end_y_ab = std::min(static_cast<int64_t>(height - 1),
                                          static_cast<int64_t>(b.pos.y));

        for (int64_t y = start_y_ab; y <= end_y_ab; ++y) {

            double ab_t = (y - a.pos.y) / (b.pos.y - a.pos.y);
            Vertex ab = Vertex::interpolate(ab_t, a, b);

            double ac_t = (y - a.pos.y) / (c.pos.y - a.pos.y);
            Vertex ac = Vertex::interpolate(ac_t, a, c);

            if (ab.pos.x > ac.pos.x) {
                std::swap(ab, ac);
            }

            const int64_t start_x_ab_ac = std::max(
                static_cast<int64_t>(0), static_cast<int64_t>(ab.pos.x));
            const int64_t end_x_ab_ac =
                std::min(static_cast<int64_t>(width - 1),
                         static_cast<int64_t>(ac.pos.x));

            for (int64_t x = start_x_ab_ac; x <= end_x_ab_ac; ++x) {

                double ab_ac_t = (x - ab.pos.x) / (ac.pos.x - ab.pos.x);
                Vertex ab_ac = Vertex::interpolate(ab_ac_t, ab, ac);
                ab_ac.pos.x = static_cast<double>(x);
                ab_ac.pos.y = static_cast<double>(y);

                set_pixel(ab_ac, image);
            }
        }

        const int64_t start_y_bc =
            std::max(static_cast<int64_t>(0), static_cast<int64_t>(b.pos.y));
        const int64_t end_y_bc = std::min(static_cast<int64_t>(height - 1),
                                          static_cast<int64_t>(c.pos.y));

        for (int64_t y = start_y_bc; y <= end_y_bc; ++y) {

            double bc_t = (y - b.pos.y) / (c.pos.y - b.pos.y);
            Vertex bc = Vertex::interpolate(bc_t, b, c);

            double ac_t = (y - a.pos.y) / (c.pos.y - a.pos.y);
            Vertex ac = Vertex::interpolate(ac_t, a, c);

            if (bc.pos.x > ac.pos.x) {
                std::swap(bc, ac);
            }

            const int64_t start_x_bc_ac = std::max(
                static_cast<int64_t>(0), static_cast<int64_t>(bc.pos.x));
            const int64_t end_x_bc_ac =
                std::min(static_cast<int64_t>(width - 1),
                         static_cast<int64_t>(ac.pos.x));

            for (int64_t x = start_x_bc_ac; x <= end_x_bc_ac; ++x) {

                double bc_ac_t = (x - bc.pos.x) / (ac.pos.x - bc.pos.x);
                Vertex bc_ac = Vertex::interpolate(bc_ac_t, bc, ac);
                bc_ac.pos.x = static_cast<double>(x);
                bc_ac.pos.y = static_cast<double>(y);

                set_pixel(bc_ac, image);
            }
        }
    }
};

auto set_pixel(const Vertex &vertex, Image &image) -> void {
    if (vertex.pos.x < 0 || vertex.pos.y < 0) {
        return;
    }
    image.set_pixel(static_cast<size_t>(vertex.pos.x),
                    static_cast<size_t>(vertex.pos.y), vertex.col);
}
} // namespace Alg
} // namespace Vis
