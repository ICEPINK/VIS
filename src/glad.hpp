#pragma once

#include <glad/glad.h>

namespace Vis {
class Glad {
public:
  static auto load_gl_loader(GLADloadproc load_proc) -> void;
  static auto print_gl_message([[maybe_unused]] GLenum source,
                               [[maybe_unused]] GLenum type,
                               [[maybe_unused]] GLuint id,
                               [[maybe_unused]] GLenum severity,
                               [[maybe_unused]] GLsizei length,
                               [[maybe_unused]] const GLchar *message,
                               [[maybe_unused]] const void *userParam) -> void;

private:
  Glad();
  ~Glad();
};

} // namespace Vis
