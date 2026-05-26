//
// Created by dave on 30.04.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include "AObject.h"

class FgpEbo final: public AObject {
public:
    FgpEbo(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);

    FgpEbo() = delete;
    FgpEbo(FgpEbo&& other) noexcept;
    FgpEbo& operator=(FgpEbo&& other) noexcept;

    void bind() const;
    static void unbind();

    void release();

    [[nodiscard]]
    bool isReady() const { return m_status && m_ebo > 0; }
    ~FgpEbo() override { release(); }

private:
    GLuint m_ebo = 0;
    bool m_status = false;
};