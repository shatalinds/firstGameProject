//
// Created by dave on 21.05.2026.
//
#include <GLES3/gl3.h>
#include "fgp_vao.h"
#include "logger.h"

FgpVao::FgpVao() {
    glGenVertexArrays(1, &m_vao);
    if (m_vao > 0) {
        m_status = true;
        LOGI("VAO created: %u", m_vao);
    } else {
        LOGE("Failed to create VAO");
    }
}

FgpVao::FgpVao(FgpVao &&other) noexcept: m_vao(other.m_vao), m_status(other.m_status) {
    other.m_vao = 0;
    other.m_status = false;
}

FgpVao &FgpVao::operator=(FgpVao &&other) noexcept {
    if (this != &other) {
        release();
        m_vao = other.m_vao;
        m_status = other.m_status;
        other.m_vao = 0;
        other.m_status = false;
    }
    return *this;
}

void FgpVao::bind() const {
    if (m_vao > 0) {
        glBindVertexArray(m_vao);
    }
}

void FgpVao::unbind() {
    glBindVertexArray(0);
}

void FgpVao::release() {
    if (m_vao > 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
        m_status = false;
    }
}