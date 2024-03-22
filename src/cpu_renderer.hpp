#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "depth_buffer.hpp"
#include "image.hpp"
#include "solids/solid.hpp"
#include "utils/scene_info.hpp"

namespace Vis {

class CpuRenderer {
  public:
    CpuRenderer(SceneInfo &scene_info);
    ~CpuRenderer();

    void *render_image(const size_t width, const size_t height);

  private:
    void render_solid(Solid &solid);
    void set_pixel(const int64_t x, const int64_t y, const Vertex &vertex);

  private:
    size_t m_width{0};
    size_t m_height{0};
    std::unique_ptr<Image> m_image;
    std::unique_ptr<DepthBuffer> m_depth_buffer;
    SceneInfo &m_scene_info_ref;
    std::unique_ptr<Pipeline> m_triangle_pipeline;
    std::unique_ptr<Pipeline> m_line_pipeline;
    std::unique_ptr<Pipeline> m_point_pipeline;
};

} // namespace Vis
