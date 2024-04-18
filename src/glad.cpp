#include "glad.hpp"

#include <iostream>
#include <stdexcept>

namespace Vis {

auto Glad::load_gl_loader(GLADloadproc load_proc) -> void {
    if (!gladLoadGLLoader(load_proc)) {
        throw std::runtime_error("Failed to initialize OpenGL context!");
    }
}

auto Glad::print_gl_message([[maybe_unused]] GLenum source,
                            [[maybe_unused]] GLenum type,
                            [[maybe_unused]] GLuint id,
                            [[maybe_unused]] GLenum severity,
                            [[maybe_unused]] GLsizei length,
                            [[maybe_unused]] const GLchar *message,
                            [[maybe_unused]] const void *userParam) -> void {
    std::cout << "OpenGL Debug: " << message << '\n';
}

} // namespace Vis
