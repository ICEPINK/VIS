#include "cpu_renderer.hpp"

namespace Vis {

CpuRenderer::CpuRenderer(SceneInfo &scene_info) : m_scene_info_ref(scene_info) {}
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

    m_image->clear(m_scene_info_ref.clear_color);

    return m_image->get_image_buffer_ptr();
}

} // namespace Vis
