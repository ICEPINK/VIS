#pragma once

#include <glad/glad.h>

namespace Vis {

class Texture {
public:
  Texture();
  ~Texture();

  auto bind() -> void;
  auto unbind() -> void;
  static auto active_texture(int texture_slot) -> void;
  [[nodiscard]] auto get_id() -> GLuint;

private:
  GLuint m_id{0};
};

} // namespace Vis
