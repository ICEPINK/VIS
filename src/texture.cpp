#include "texture.hpp"

#include <glad/glad.h>

namespace Vis {
Texture::Texture() {
    glGenTextures(1, &m_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

Texture::~Texture() { glDeleteTextures(1, &m_id); }

void Texture::bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::set_image(size_t width, size_t height, void *image_ptr) {
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image_ptr);
}

uint32_t Texture::get_id() { return m_id; }

} // namespace Vis
