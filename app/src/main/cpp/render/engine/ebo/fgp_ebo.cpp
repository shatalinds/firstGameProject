//
// Created by dave on 21.05.2026.
//

#include "logger.h"
#include <GLES3/gl3.h>
#include <vector>
#include "fgp_ebo.h"

FgpEbo::FgpEbo(const void* data, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, &m_ebo);
    if (m_ebo > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
        m_status = true;
        LOGI("EBO created, size: %ld", (long)size);
    }
}

FgpEbo::FgpEbo(FgpEbo&& other) noexcept
:   m_ebo(other.m_ebo),
    m_status(other.m_status) {
    other.m_ebo = 0;
    other.m_status = false;
}

FgpEbo& FgpEbo::operator=(FgpEbo&& other) noexcept {
    if (this != &other) {
        release();
        m_ebo = other.m_ebo;
        m_status = other.m_status;
        other.m_ebo = 0;
        other.m_status = false;
    }
    return *this;
}

void FgpEbo::bind() const {
    if (m_ebo > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    }
}

void FgpEbo::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void FgpEbo::release() {
    if (m_ebo > 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
        m_status = false;
    }
}