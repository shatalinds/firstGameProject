//
// Created by dave on 21.05.2026.
//
#include <GLES3/gl3.h>
#include "logger.h"
#include "fgp_vbo.h"

FgpVbo::FgpVbo(const void *data, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, &m_vbo);
    if (m_vbo > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_status = true;
    }
}

FgpVbo::FgpVbo(FgpVbo &&other) noexcept: m_vbo(other.m_vbo), m_status(other.m_status) {
    other.m_vbo = 0;
    other.m_status = false;
}

FgpVbo &FgpVbo::operator=(FgpVbo &&other) noexcept {
    if (this != &other) {
        release();
        m_vbo = other.m_vbo;
        m_status = other.m_status;
        other.m_vbo = 0;
        other.m_status = false;
    }
    return *this;
}

void FgpVbo::bind() const {
    if (m_vbo > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    }
}

void FgpVbo::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FgpVbo::release() {
    if (m_vbo > 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
        m_status = false;
    }
}