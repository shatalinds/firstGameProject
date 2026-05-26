//
// Created by dave on 30.04.2026.
//

#include "fgp_texture.h"
#include "stb_image.h"
#include "logger.h"

FgpTexture::FgpTexture(const char *image, GLuint slot, std::string_view texType)
        : m_type(texType), m_ownsTexture(true) {
    m_unit = slot;
    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
    if (!bytes) {
        LOGE("Failed to load texture: %s", image);
        return;
    }
    genText(bytes, widthImg, heightImg, numColCh, slot, texType);
    stbi_image_free(bytes);
}

FgpTexture::FgpTexture(const unsigned char *bytes, int width, int height, int channels, GLuint slot,
                       std::string_view texType) : m_type(texType), m_ownsTexture(true) {
    m_unit = slot;
    genText(bytes, width, height, channels, slot, m_type);
}

void FgpTexture::genText(const unsigned char *bytes, int width, int height, int channels, GLuint slot,
                    std::string_view texType) {
    if (!bytes || width <= 0 || height <= 0) {
        LOGE("Failed to create texture: data=%p size=%dx%d channels=%d",
             bytes, width, height, channels);
        return;
    }

    m_unit = slot;
    glGenTextures(1, &m_texture);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (channels) {
        case 4: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, bytes);
            break;
        }
        case 3: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, bytes);
            break;
        }
        case 1: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED,
                         GL_UNSIGNED_BYTE, bytes);
            break;
        }
        default: {
            LOGE("Automatic Texture type recognition failed for raw bytes");
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    LOGI("Texture OK: %dx%d ch=%d id=%u unit=%u", width, height, channels, m_texture, m_unit);
}

FgpTexture::FgpTexture(FgpTexture &&other) noexcept
        : m_texture(other.m_texture),
          m_type(std::move(other.m_type)),
          m_unit(other.m_unit),
          m_ownsTexture(other.m_ownsTexture) {
    other.m_texture = 0;
    other.m_ownsTexture = false;
}

FgpTexture &FgpTexture::operator=(FgpTexture &&other) noexcept {
    if (this != &other) {
        if (m_texture != 0 && m_ownsTexture) {
            glDeleteTextures(1, &m_texture);
        }

        m_texture = other.m_texture;
        m_type = std::move(other.m_type);
        m_unit = other.m_unit;
        m_ownsTexture = other.m_ownsTexture;

        other.m_texture = 0;
        other.m_ownsTexture = false;
    }
    return *this;
}

void FgpTexture::texUnit(GLuint &shader, const char *uniform, GLint unit) {
    GLint texUni = glGetUniformLocation(shader, uniform);
    glUseProgram(shader);
    glUniform1i(texUni, unit);
}

void FgpTexture::bind() const {
    bindTo(m_unit);
}

void FgpTexture::bindTo(GLuint unit) const {
    if (m_texture == 0) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void FgpTexture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

FgpTexture::~FgpTexture() {
    if (m_texture != 0 && m_ownsTexture) {
        glDeleteTextures(1, &m_texture);
    }
}