#include "cpu_renderer.hpp"

#include "solids/axis.hpp"
#include "solids/cube.hpp"
#include "solids/grid.hpp"
#include "solids/solid.hpp"
#include "solids/square.hpp"
#include "utils/timer.hpp"

#include <glm/ext.hpp>

#include <chrono>
#include <iostream>

namespace Vis {

void CpuRenderer::set_pixel_bw_depth(const int64_t x, const int64_t y,
                                     const Vertex &vertex) {
    if (x < 0 || static_cast<size_t>(x) >= m_width) {
        return;
    }
    if (y < 0 || static_cast<size_t>(y) >= m_height) {
        return;
    }
    auto new_y = m_height - 1 - y;

    if (vertex.position.z < m_depth_buffer->get_depth(x, new_y)) {
        m_depth_buffer->set_depth(x, new_y, vertex.position.z);
        m_image->set_pixel(x, new_y,
                           ColorRGBA32f{1.0f, 1.0f, 1.0f, 1.0f} *
                               std::max(0.0f, vertex.position.z));
    }
}
void CpuRenderer::set_pixel_rgba_depth(const int64_t x, const int64_t y,
                                       const Vertex &vertex) {
    if (x < 0 || static_cast<size_t>(x) >= m_width) {
        return;
    }
    if (y < 0 || static_cast<size_t>(y) >= m_height) {
        return;
    }

    auto new_y = m_height - 1 - y;

    if (vertex.position.z < m_depth_buffer->get_depth(x, new_y)) {
        m_depth_buffer->set_depth(x, new_y, vertex.position.z);
        m_image->set_pixel(x, new_y, vertex.color * (1.0f / vertex.one));
    }
}

void CpuRenderer::set_pixel_rgba_no_depth(const int64_t x, const int64_t y,
                                          const Vertex &vertex) {
    if (x < 0 || static_cast<size_t>(x) >= m_width) {
        return;
    }
    if (y < 0 || static_cast<size_t>(y) >= m_height) {
        return;
    }

    auto new_y = m_height - 1 - y;
    m_image->set_pixel(x, new_y, vertex.color * (1.0f / vertex.one));
}

void CpuRenderer::render_solid(Solid &solid) {

    auto &vertices = solid.data.vertices;
    auto &pipeline_line_data = m_scene_info_ref.pipeline_line_data;
    auto &pipeline_triangle_data = m_scene_info_ref.pipeline_triangle_data;

    for (Layout &layout : solid.data.layout) {
        switch (layout.topology) {
        case Topology::Point:
            break;
        case Topology::Line:
            pipeline_line_data.width = m_width;
            pipeline_line_data.height = m_height;
            pipeline_line_data.solid_matrix = solid.data.matrix;
            pipeline_line_data.model_matrix = glm::mat4{1.0f};
            pipeline_line_data.view_matrix =
                m_scene_info_ref.camera->get_view_matrix();
            pipeline_line_data.proj_matrix =
                m_scene_info_ref.camera->get_projection_matrix();

            m_line_pipeline->update_matrix();

            for (size_t i = 0; i < layout.count; ++i) {
                size_t &index_a = solid.data.indices[layout.start + (i * 2)];
                size_t &index_b =
                    solid.data.indices[layout.start + (i * 2) + 1];

                m_line_pipeline->render({vertices[index_a], vertices[index_b]});
            }
            break;
        case Topology::Triangle:
            pipeline_triangle_data.width = m_width;
            pipeline_triangle_data.height = m_height;
            pipeline_triangle_data.solid_matrix = solid.data.matrix;
            pipeline_triangle_data.model_matrix = glm::mat4{1.0f};
            pipeline_triangle_data.view_matrix =
                m_scene_info_ref.camera->get_view_matrix();
            pipeline_triangle_data.proj_matrix =
                m_scene_info_ref.camera->get_projection_matrix();

            m_triangle_pipeline->update_matrix();

            for (size_t i = 0; i < layout.count; ++i) {
                size_t &index_a = solid.data.indices[layout.start + (i * 3)];
                size_t &index_b =
                    solid.data.indices[layout.start + (i * 3) + 1];
                size_t &index_c =
                    solid.data.indices[layout.start + (i * 3) + 2];

                m_triangle_pipeline->render(
                    {vertices[index_a], vertices[index_b], vertices[index_c]});
            }
            break;
        }
    }
}

void CpuRenderer::render_scene(std::vector<Solid> &scene) {
    for (auto &solid : scene) {
        render_solid(solid);
    };
}

CpuRenderer::CpuRenderer(SceneInfo &scene_info) : m_scene_info_ref(scene_info) {
    PerspectiveData camera_data;
    camera_data.position = {-6.0f, 0.0f, 2.0f};
    camera_data.rotation = {0.0f, 0.0f};
    camera_data.near_plane = {0.1f};
    camera_data.far_plane = {100.0f};
    camera_data.fov = {glm::pi<float>() / 180.0f * 90.0f};
    camera_data.aspect_ratio = {1.0f};
    scene_info.camera = std::make_unique<PerspectiveCamera>(camera_data);

    PerspectiveData simulated_camera_data;
    simulated_camera_data.position = {0.0f, 0.0f, 0.0f};
    simulated_camera_data.rotation = {0.0f, 0.0f};
    simulated_camera_data.near_plane = {1.0f};
    simulated_camera_data.far_plane = {5.0f};
    simulated_camera_data.fov = {glm::pi<float>() / 180.0f * 90.0f};
    simulated_camera_data.aspect_ratio = {1.0f};
    scene_info.simulated_camera =
        std::make_unique<PerspectiveCamera>(simulated_camera_data);

    auto &pipeline_triangle_data = scene_info.pipeline_triangle_data;
    pipeline_triangle_data.matrix_calculation =
        Pipeline::matrix_calculation_smvp;
    pipeline_triangle_data.trasform_vertices_by_matrix =
        Pipeline::trasform_vertices_by_matrix_position;
    pipeline_triangle_data.fast_clip = Pipeline::fast_clip_triangle;
    pipeline_triangle_data.clip_before_dehomog =
        Pipeline::clip_before_dehomog_triangle;
    pipeline_triangle_data.dehomog_vertices = Pipeline::dehomog_vertices;
    pipeline_triangle_data.clip_after_dehomog =
        Pipeline::clip_after_dehomog_none;
    pipeline_triangle_data.trasform_vertices_onto_viewport =
        Pipeline::trasform_vertices_onto_viewport;
    pipeline_triangle_data.rasterization =
        Pipeline::rasterization_triangle_fill;
    pipeline_triangle_data.set_pixel = std::bind(
        &CpuRenderer::set_pixel_rgba_depth, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    m_triangle_pipeline = std::make_unique<Pipeline>(pipeline_triangle_data);

    auto &pipeline_line_data = scene_info.pipeline_line_data;
    pipeline_line_data.matrix_calculation = Pipeline::matrix_calculation_smvp;
    pipeline_line_data.trasform_vertices_by_matrix =
        Pipeline::trasform_vertices_by_matrix_position;
    pipeline_line_data.fast_clip = Pipeline::fast_clip_line;
    pipeline_line_data.clip_before_dehomog = Pipeline::clip_before_dehomog_line;
    pipeline_line_data.dehomog_vertices = Pipeline::dehomog_vertices;
    pipeline_line_data.clip_after_dehomog = Pipeline::clip_after_dehomog_line;
    pipeline_line_data.trasform_vertices_onto_viewport =
        Pipeline::trasform_vertices_onto_viewport;
    pipeline_line_data.rasterization = Pipeline::rasterization_line_dda;
    pipeline_line_data.set_pixel = std::bind(
        &CpuRenderer::set_pixel_rgba_depth, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    m_line_pipeline = std::make_unique<Pipeline>(pipeline_line_data);

    // auto &pipeline_point_data = scene_info.pipeline_point_data;

    auto &pipeline_simulated_line_data =
        scene_info.pipeline_simulated_line_data;
    pipeline_simulated_line_data.matrix_calculation =
        Pipeline::matrix_calculation_smvp;
    pipeline_simulated_line_data.trasform_vertices_by_matrix =
        Pipeline::trasform_vertices_by_matrix_position;
    pipeline_simulated_line_data.fast_clip = Pipeline::fast_clip_line;
    pipeline_simulated_line_data.clip_before_dehomog =
        Pipeline::clip_before_dehomog_line;
    pipeline_simulated_line_data.dehomog_vertices = Pipeline::dehomog_vertices;
    pipeline_simulated_line_data.clip_after_dehomog =
        Pipeline::clip_after_dehomog_line;
    pipeline_simulated_line_data.trasform_vertices_onto_viewport =
        Pipeline::trasform_vertices_onto_viewport;
    pipeline_simulated_line_data.rasterization =
        Pipeline::rasterization_line_dda;
    pipeline_simulated_line_data.set_pixel = std::bind(
        &CpuRenderer::set_pixel_rgba_depth, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    m_simulated_line_pipeline =
        std::make_unique<Pipeline>(pipeline_simulated_line_data);

    auto &pipeline_simulated_triangle_data =
        scene_info.pipeline_simulated_triangle_data;
    pipeline_simulated_triangle_data.matrix_calculation =
        Pipeline::matrix_calculation_smvp;
    pipeline_simulated_triangle_data.trasform_vertices_by_matrix =
        Pipeline::trasform_vertices_by_matrix_position;
    pipeline_simulated_triangle_data.fast_clip = Pipeline::fast_clip_triangle;
    pipeline_simulated_triangle_data.clip_before_dehomog =
        Pipeline::clip_before_dehomog_triangle;
    pipeline_simulated_triangle_data.dehomog_vertices =
        Pipeline::dehomog_vertices;
    pipeline_simulated_triangle_data.clip_after_dehomog =
        Pipeline::clip_after_dehomog_triangle;
    pipeline_simulated_triangle_data.trasform_vertices_onto_viewport =
        Pipeline::trasform_vertices_onto_viewport;
    pipeline_simulated_triangle_data.rasterization =
        Pipeline::rasterization_triangle_fill;
    pipeline_simulated_triangle_data.set_pixel = std::bind(
        &CpuRenderer::set_pixel_rgba_depth, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);
    m_simulated_triangle_pipeline =
        std::make_unique<Pipeline>(pipeline_simulated_triangle_data);

    m_simulated_solid = std::make_unique<Cube>("Cube");
    m_simulated_solid->data.matrix =
        glm::translate(glm::mat4{1.0f}, {3.0f, 0.0f, 0.0f});

    m_scene.push_back(Axis("_axis"));
}
CpuRenderer::~CpuRenderer() {}

void CpuRenderer::update_pipeline_settings() {
    switch (m_scene_info_ref.set_pixel_setting) {
    case SetPixelEnum::RGBA_DEPTH:
        m_scene_info_ref.pipeline_simulated_triangle_data.set_pixel = std::bind(
            &CpuRenderer::set_pixel_rgba_depth, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
        break;
    case SetPixelEnum::RGBA_NO_DEPTH:
        m_scene_info_ref.pipeline_simulated_triangle_data.set_pixel = std::bind(
            &CpuRenderer::set_pixel_rgba_no_depth, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
        break;
    case SetPixelEnum::BW_DEPTH:
        m_scene_info_ref.pipeline_simulated_triangle_data.set_pixel = std::bind(
            &CpuRenderer::set_pixel_bw_depth, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
        break;
    }
}

void CpuRenderer::resize_image(const size_t width, const size_t height) {
    if (m_width != width || m_height != height) {
        m_image = std::make_unique<Image>(width, height);
        m_depth_buffer = std::make_unique<DepthBuffer>(width, height);

        m_scene_info_ref.camera->set_aspect_ratio(width /
                                                  static_cast<float>(height));

        m_scene_info_ref.simulated_camera->set_aspect_ratio(
            width / static_cast<float>(height));

        m_width = width;
        m_height = height;
    }
}

void CpuRenderer::render_simulation() {
    auto &solid = *m_simulated_solid;
    auto &vertices = solid.data.vertices;
    auto &pipeline_line_data = m_scene_info_ref.pipeline_simulated_line_data;
    auto &pipeline_triangle_data =
        m_scene_info_ref.pipeline_simulated_triangle_data;

    pipeline_line_data.callback = false;
    pipeline_triangle_data.callback = false;

    for (Layout &layout : solid.data.layout) {
        switch (layout.topology) {
        case Topology::Point:
            break;
        case Topology::Line:
            pipeline_line_data.width = m_width;
            pipeline_line_data.height = m_height;
            pipeline_line_data.solid_matrix = solid.data.matrix;
            pipeline_line_data.model_matrix = glm::mat4{1.0f};
            pipeline_line_data.view_matrix =
                m_scene_info_ref.simulated_camera->get_view_matrix();
            pipeline_line_data.proj_matrix =
                m_scene_info_ref.simulated_camera->get_projection_matrix();

            m_simulated_line_pipeline->update_matrix();

            for (size_t i = 0; i < layout.count; ++i) {
                size_t &index_a = solid.data.indices[layout.start + (i * 2)];
                size_t &index_b =
                    solid.data.indices[layout.start + (i * 2) + 1];

                m_simulated_line_pipeline->render(
                    {vertices[index_a], vertices[index_b]});
            }
            break;
        case Topology::Triangle:
            pipeline_triangle_data.width = m_width;
            pipeline_triangle_data.height = m_height;
            pipeline_triangle_data.solid_matrix = solid.data.matrix;
            pipeline_triangle_data.model_matrix = glm::mat4{1.0f};
            pipeline_triangle_data.view_matrix =
                m_scene_info_ref.simulated_camera->get_view_matrix();
            pipeline_triangle_data.proj_matrix =
                m_scene_info_ref.simulated_camera->get_projection_matrix();

            m_simulated_triangle_pipeline->update_matrix();

            for (size_t i = 0; i < layout.count; ++i) {
                size_t &index_a = solid.data.indices[layout.start + (i * 3)];
                size_t &index_b =
                    solid.data.indices[layout.start + (i * 3) + 1];
                size_t &index_c =
                    solid.data.indices[layout.start + (i * 3) + 2];

                m_simulated_triangle_pipeline->render(
                    {vertices[index_a], vertices[index_b], vertices[index_c]});
            }
            break;
        }
    }
}

void CpuRenderer::render() {
    if (m_scene_info_ref.simulated) {
        render_simulation();
    } else {
        render_scene(m_scene);
    }
}

void *CpuRenderer::render_image(const size_t width, const size_t height) {
    if (width == 0 || height == 0) {
        return nullptr;
    }

    Timer timer;

    resize_image(width, height);

    if (m_scene_info_ref.scene_settings_changed) {
        update_pipeline_settings();
    }

    m_image->clear(m_scene_info_ref.clear_color);
    m_depth_buffer->clear(1.0);

    render();

    m_scene_info_ref.last_render = timer.get_time();

    return m_image->get_image_buffer_ptr();
}
} // namespace Vis
