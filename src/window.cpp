#include "window.hpp"

#include <stdexcept>

#include <glad/glad.h>

#include <glm/ext.hpp>

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

    int glfw_key_left_alt = glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_ALT);
    if (glfw_key_left_alt == GLFW_PRESS) {
        if (!app_info.alt_mode_lock) {
            app_info.alt_mode = !app_info.alt_mode;
        }

        glfwGetCursorPos(m_window_ptr, &app_info.mouse_x, &app_info.mouse_y);
        app_info.alt_mode_lock = true;
    }

    if (glfw_key_left_alt == GLFW_RELEASE) {
        app_info.alt_mode_lock = false;
    }

    if (app_info.alt_mode) {
        glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (app_info.alt_mode) {
        if (scene_info.simulated) {
            double new_mouse_x;
            double new_mouse_y;
            glfwGetCursorPos(m_window_ptr, &new_mouse_x, &new_mouse_y);

            if (scene_info.simulated_camera) {
                float d_mouse_x =
                    static_cast<float>(new_mouse_x - app_info.mouse_x);
                constexpr float angel = -glm::pi<float>() / 180.0f;
                scene_info.simulated_camera->rotate_horizon(d_mouse_x * angel *
                                                            0.1f);
            }

            if (scene_info.simulated_camera) {
                float d_mouse_y =
                    static_cast<float>(new_mouse_y - app_info.mouse_y);
                constexpr float angel = glm::pi<float>() / 180.0f;
                scene_info.simulated_camera->rotate_vertical(d_mouse_y * angel *
                                                             0.1f);
            }

            app_info.mouse_x = 0;
            app_info.mouse_y = 0;
            glfwSetCursorPos(m_window_ptr, app_info.mouse_x, app_info.mouse_y);

            int key;
            key = glfwGetKey(m_window_ptr, GLFW_KEY_W);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->move_forward(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_S);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->move_backward(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_A);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->move_left(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_D);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->move_right(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_SPACE);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->move_up(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_CONTROL);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->move_down(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_Q);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->rotate_horizon(
                        glm::pi<float>() / 180 * 0.5f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_E);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->rotate_horizon(
                        -glm::pi<float>() / 180 * 0.5f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_Y);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->rotate_vertical(
                        -glm::pi<float>() / 180 * 0.5f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_Z);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.simulated_camera) {
                    scene_info.simulated_camera->rotate_vertical(
                        glm::pi<float>() / 180 * 0.5f);
                }
            }
        } else {
            double new_mouse_x;
            double new_mouse_y;
            glfwGetCursorPos(m_window_ptr, &new_mouse_x, &new_mouse_y);

            if (scene_info.camera) {
                float d_mouse_x =
                    static_cast<float>(new_mouse_x - app_info.mouse_x);
                constexpr float angel = -glm::pi<float>() / 180.0f;
                scene_info.camera->rotate_horizon(d_mouse_x * angel * 0.1f);
            }

            if (scene_info.camera) {
                float d_mouse_y =
                    static_cast<float>(new_mouse_y - app_info.mouse_y);
                constexpr float angel = glm::pi<float>() / 180.0f;
                scene_info.camera->rotate_vertical(d_mouse_y * angel * 0.1f);
            }

            app_info.mouse_x = 0;
            app_info.mouse_y = 0;
            glfwSetCursorPos(m_window_ptr, app_info.mouse_x, app_info.mouse_y);

            int key;
            key = glfwGetKey(m_window_ptr, GLFW_KEY_W);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->move_forward(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_S);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->move_backward(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_A);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->move_left(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_D);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->move_right(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_SPACE);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->move_up(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_CONTROL);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->move_down(0.02f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_Q);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->rotate_horizon(glm::pi<float>() / 180 *
                                                      0.5f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_E);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->rotate_horizon(-glm::pi<float>() / 180 *
                                                      0.5f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_Y);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->rotate_vertical(-glm::pi<float>() / 180 *
                                                       0.5f);
                }
            }

            key = glfwGetKey(m_window_ptr, GLFW_KEY_Z);
            if (key == GLFW_PRESS || key == GLFW_REPEAT) {
                if (scene_info.camera) {
                    scene_info.camera->rotate_vertical(glm::pi<float>() / 180 *
                                                       0.5f);
                }
            }
        }
    }
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
