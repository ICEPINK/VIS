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
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "OpenGL(Error): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        // std::cout << "OpenGL (DEPRECATED_BEHAVIOR): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        // std::cout << "OpenGL (UNDEFINED_BEHAVIOR): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        // std::cout << "OpenGL (PORTABILITY): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        // std::cout << "OpenGL (PERFORMANCE): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_OTHER:
        // std::cout << "OpenGL (OTHER): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_MARKER:
        // std::cout << "OpenGL (MARKER): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        // std::cout << "OpenGL (PUSH_GROUP): " << message << '\n';
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        // std::cout << "OpenGL (POP_GROUP): " << message << '\n';
        break;
    }
}

} // namespace Vis
