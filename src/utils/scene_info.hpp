#pragma once

#include <chrono>
#include <memory>

#include "../camera.hpp"
#include "../pipeline.hpp"
#include "color.hpp"

namespace Vis {

struct SceneInfo {
    ColorRGBA32f clear_color{0.02, 0.02, 0.02, 1.0};
    std::chrono::duration<double> last_render{0};
    glm::mat4 model_matrix{glm::mat4(1.0)};
    std::unique_ptr<PerspectiveCamera> camera;
    PipelineData pipeline_triangle_data;
    PipelineData pipeline_line_data;
    PipelineData pipeline_point_data;
};

} // namespace Vis
