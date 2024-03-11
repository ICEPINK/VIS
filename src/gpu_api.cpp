#include "gpu_api.hpp"

#include <stdexcept>

namespace Vis {

GpuApi::GpuApi(GpuApiType type) : m_gpu_api_type(type) {}

GpuApi::~GpuApi() {}

void GpuApi::init(void (*(*proc_address)(const char *))(void)) {
    switch (m_gpu_api_type) {
    case Vis::GpuApiType::OpenGL:
        if (!gladLoadGLLoader((GLADloadproc)proc_address)) {
            throw std::runtime_error("Error: Faild to load Glad!\n");
        }
        break;
    }
}

GpuApiType GpuApi::get_api_type() const { return m_gpu_api_type; }

} // namespace Vis
