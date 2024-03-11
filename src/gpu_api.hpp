#pragma once

#include <glad/glad.h>

namespace Vis {

enum class GpuApiType { OpenGL };

class GpuApi {
  public:
    GpuApi(GpuApiType type);
    ~GpuApi();

    void init(void (*(*proc_address)(const char *))(void));

    GpuApiType get_api_type() const;

  private:
    GpuApiType m_gpu_api_type;
};

} // namespace Vis
