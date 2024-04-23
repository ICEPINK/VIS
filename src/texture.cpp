#include "texture.hpp"

namespace Vis {
Texture::Texture() { glGenTextures(1, &m_id); }
Texture::~Texture() {}

auto Texture::bind() -> void { glBindTexture(GL_TEXTURE_2D, m_id); }

auto Texture::unbind() -> void { glBindTexture(GL_TEXTURE_2D, 0); }

auto Texture::active_texture(const int texture_slot) -> void {
    glActiveTexture(texture_slot);
}

[[nodiscard]] auto Texture::get_id() -> GLuint { return m_id; }

} // namespace Vis
