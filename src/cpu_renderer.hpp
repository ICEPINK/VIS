#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "depth_buffer.hpp"
#include "image.hpp"
#include "solids/solid.hpp"

namespace Vis {

class CpuRenderer {
  public:
    CpuRenderer(SceneInfo &scene_info);
    ~CpuRenderer();

    void *render_image(const size_t width, const size_t height);

  private:
    void render_solid(Solid &solid);
    // void render_point(Vertex &a);
    void render_line(Vertex &a, Vertex &b);
    void render_triangle(Vertex &a, Vertex &b, Vertex &c);
    // void rasterize_point(Vertex &a);
    void rasterize_line(Vertex &a, Vertex &b);
    void rasterize_triangle(Vertex &a, Vertex &b, Vertex &c);
    void trasform_to_screen(Vertex &vertex);
    bool fast_clip(const std::vector<Vertex> &vertices, Topology topology) const;
    void clip_z(std::vector<Vertex>& vertices) const;

    void set_pixel(int64_t x, int64_t y, Vertex &vertex);

  private:
    size_t m_width{0};
    size_t m_height{0};
    std::unique_ptr<Image> m_image;
    std::unique_ptr<DepthBuffer> m_depth_buffer;
    SceneInfo &m_scene_info_ref;
};

} // namespace Vis
