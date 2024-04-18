#include "glfw.hpp"
#include <stdexcept>

namespace Vis {

static size_t s_instance_count{0};

Glfw::Glfw() {
    if (!s_instance_count) {
        if (!glfwInit()) {
            throw std::runtime_error("GLFW initialization failed!");
        }
    }

    ++s_instance_count;
}

Glfw::~Glfw() {
    --s_instance_count;

    if (!s_instance_count) {
        glfwTerminate();
    }
}

} // namespace Vis
