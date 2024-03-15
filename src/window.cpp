#include "window.hpp"

#include <iostream>
#include <stdexcept>

#include <glad/glad.h>

namespace Vis {

static size_t s_window_count = 0;

Window::Window(const int32_t width, const int32_t height,
               const std::string title, GpuApi &gpu_api)
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

    gpu_api.init(glfwGetProcAddress);
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

void Window::clear() { glClear(GL_COLOR_BUFFER_BIT); }

void Window::handle_input([[maybe_unused]] AppInfo &app_info,
                          [[maybe_unused]] SceneInfo &scene_info) {

    if (!app_info.alt_mode) {
        int key;
        key = glfwGetKey(m_window_ptr, GLFW_KEY_W);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            if (scene_info.camera) {
                scene_info.camera->move_forward(0.02);
            }
        }

        key = glfwGetKey(m_window_ptr, GLFW_KEY_S);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            if (scene_info.camera) {
                scene_info.camera->move_backward(0.02);
            }
        }

        key = glfwGetKey(m_window_ptr, GLFW_KEY_A);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            if (scene_info.camera) {
                scene_info.camera->move_left(0.02);
            }
        }

        key = glfwGetKey(m_window_ptr, GLFW_KEY_D);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            if (scene_info.camera) {
                scene_info.camera->move_right(0.02);
            }
        }

        key = glfwGetKey(m_window_ptr, GLFW_KEY_SPACE);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            if (scene_info.camera) {
                scene_info.camera->move_up(0.02);
            }
        }

        key = glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_CONTROL);
        if (key == GLFW_PRESS || key == GLFW_REPEAT) {
            if (scene_info.camera) {
                scene_info.camera->move_down(0.02);
            }
        }
    }

    // int glfw_key_left_alt = glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_ALT);
    // if (glfw_key_left_alt == GLFW_RELEASE) {
    //     app_info.alt_mode = !app_info.alt_mode;
    // }
}

void Window::set_clear_color(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
}

bool Window::should_window_close() const {
    return glfwWindowShouldClose(m_window_ptr);
}

void Window::init_gui(const std::unique_ptr<Gui> &gui) {
    gui->glfw_init(m_window_ptr);
    gui->opengl_init(m_glsl_version);
}

} // namespace Vis
