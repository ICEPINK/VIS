#pragma once
////////////////////////////////////////////////////////////////////////////////
#include "utils/vertex.hpp"
////////////////////////////////////////////////////////////////////////////////
#include <glm/glm.hpp>
////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <memory>
#include <optional>
#include <vector>
////////////////////////////////////////////////////////////////////////////////
namespace Vis {
////////////////////////////////////////////////////////////////////////////////
struct PipelineData {
    bool (*fast_clip)(std::unique_ptr<std::vector<Vertex>> &vertices);
    glm::mat4 (*matrix_calculation)(const glm::mat4 &solid_matrix,
                                    const glm::mat4 &model_matrix,
                                    const glm::mat4 &view_matrix,
                                    const glm::mat4 &proj_matrix);
    std::function<void(const int64_t x, const int64_t y, const Vertex &vertex)>
        set_pixel;
    void (*clip_after_dehomog)(std::unique_ptr<std::vector<Vertex>> &vertices);
    void (*clip_before_dehomog)(std::unique_ptr<std::vector<Vertex>> &vertices);
    void (*dehomog_vertices)(std::unique_ptr<std::vector<Vertex>> &vertices);
    void (*rasterization)(std::unique_ptr<std::vector<Vertex>> &vertices,
                          const size_t widht, const size_t height,
                          std::function<void(const int64_t x, const int64_t y,
                                             const Vertex &vertex)>);
    void (*trasform_vertices_by_matrix)(
        std::unique_ptr<std::vector<Vertex>> &vertices,
        const glm::mat4 &matrix);
    void (*trasform_vertices_onto_viewport)(
        std::unique_ptr<std::vector<Vertex>> &vertices, const size_t widht,
        const size_t height);
    size_t width;
    size_t height;
    glm::mat4 solid_matrix;
    glm::mat4 model_matrix;
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
    glm::mat4 matrix;
    bool callback{false};
};
////////////////////////////////////////////////////////////////////////////////
class Pipeline {
  public:
    Pipeline(PipelineData &data);
    ~Pipeline();

    std::optional<std::unique_ptr<std::vector<Vertex>>>
    render(const std::vector<Vertex> &vertices) const;
    void update_matrix() const;

    static bool fast_clip_line(std::unique_ptr<std::vector<Vertex>> &vertices);
    static bool
    fast_clip_triangle(std::unique_ptr<std::vector<Vertex>> &vertices);
    static glm::mat4 matrix_calculation_smvp(const glm::mat4 &solid,
                                             const glm::mat4 &model,
                                             const glm::mat4 &view,
                                             const glm::mat4 &proj);
    static void
    clip_after_dehomog_line(std::unique_ptr<std::vector<Vertex>> &vertices);
    static void
    clip_after_dehomog_none(std::unique_ptr<std::vector<Vertex>> &vertices);
    static void
    clip_after_dehomog_triangle(std::unique_ptr<std::vector<Vertex>> &vertices);
    static void
    clip_before_dehomog_line(std::unique_ptr<std::vector<Vertex>> &vertices);
    static void clip_before_dehomog_triangle(
        std::unique_ptr<std::vector<Vertex>> &vertices);
    static void
    dehomog_vertices(std::unique_ptr<std::vector<Vertex>> &vertices);
    static void
    rasterization_line_dda(std::unique_ptr<std::vector<Vertex>> &vertices,
                           const size_t width, const size_t height,
                           std::function<void(const int64_t x, const int64_t y,
                                              const Vertex &vertex)>
                               set_pixel);
    static void rasterization_triangle_fill_color(
        std::unique_ptr<std::vector<Vertex>> &vertices, const size_t width,
        const size_t height,
        std::function<void(const int64_t x, const int64_t y,
                           const Vertex &vertex)>
            set_pixel);
    static void trasform_vertices_by_matrix_all(
        std::unique_ptr<std::vector<Vertex>> &vertices,
        const glm::mat4 &matrix);
    static void trasform_vertices_onto_viewport(
        std::unique_ptr<std::vector<Vertex>> &vertices, const size_t widht,
        const size_t height);

  private:
    PipelineData &m_data_ref;
};
////////////////////////////////////////////////////////////////////////////////
} // namespace Vis
