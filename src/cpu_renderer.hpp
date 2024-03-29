#pragma once
////////////////////////////////////////////////////////////////////////////////
#include "depth_buffer.hpp"
#include "image.hpp"
#include "solids/solid.hpp"
#include "utils/scene_info.hpp"
////////////////////////////////////////////////////////////////////////////////
#include <memory>
////////////////////////////////////////////////////////////////////////////////
namespace Vis {
////////////////////////////////////////////////////////////////////////////////
class CpuRenderer {
  public:
    CpuRenderer(SceneInfo &scene_info);
    ~CpuRenderer();

    void *render_image(const size_t width, const size_t height);

  private:
    void render();
    void render_solid(Solid &solid);
    void render_scene(std::vector<Solid> &scene);
    void render_simulation();
    void set_pixel_rgba_depth(const int64_t x, const int64_t y,
                              const Vertex &vertex);
    void set_pixel_rgba_no_depth(const int64_t x, const int64_t y,
                                 const Vertex &vertex);
    void set_pixel_bw_depth(const int64_t x, const int64_t y,
                            const Vertex &vertex);
    void update_pipeline_settings();
    void resize_image(const size_t width, const size_t height);

  private:
    SceneInfo &m_scene_info_ref;
    size_t m_height{0};
    size_t m_width{0};
    std::unique_ptr<DepthBuffer> m_depth_buffer;
    std::unique_ptr<Image> m_image;
    std::unique_ptr<Pipeline> m_line_pipeline;
    std::unique_ptr<Pipeline> m_point_pipeline;
    std::unique_ptr<Pipeline> m_simulated_line_pipeline;
    std::unique_ptr<Pipeline> m_simulated_point_pipeline;
    std::unique_ptr<Pipeline> m_simulated_triangle_pipeline;
    std::unique_ptr<Pipeline> m_triangle_pipeline;
    std::unique_ptr<Solid> m_simulated_solid;
    std::vector<Solid> m_scene;
};
////////////////////////////////////////////////////////////////////////////////
} // namespace Vis
