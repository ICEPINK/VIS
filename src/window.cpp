#include "window.hpp"

#include <stdexcept>

#include <glad/glad.h>

namespace Vis {

static size_t s_window_count = 0;

Window::Window(const size_t width, const size_t height, const std::string title)
    : m_window_ptr(nullptr), m_width(width), m_height(height), m_title(title) {

    if (s_window_count == 0) {
        if (!glfwInit()) {
            throw std::runtime_error("Error: Faild to initialize GLFW!\n");
        }
    }

    m_window_ptr =
        glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!m_window_ptr) {
        throw std::runtime_error("Error: Faild to create Window!\n");
    }

    s_window_count += 1;

    m_glsl_version = "#version 460";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    glfwMakeContextCurrent(m_window_ptr);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Error: Faild to load Glad!\n");
    }
}

Window::~Window() {
    glfwDestroyWindow(m_window_ptr);
    s_window_count -= 1;

    if (s_window_count == 0) {
        glfwTerminate();
    }
}

void Window::update() {
    glfwSwapBuffers(m_window_ptr);
    glfwPollEvents();
}

bool Window::should_window_close() const {
    return glfwWindowShouldClose(m_window_ptr);
}

} // namespace Vis
