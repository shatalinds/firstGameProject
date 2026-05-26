//
// Created by dave on 30.04.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include "AObject.h"

class FgpVbo final: public AObject {
public:
    FgpVbo(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    ~FgpVbo() override { release(); }

    FgpVbo(FgpVbo&& other) noexcept;
    FgpVbo& operator=(FgpVbo&& other) noexcept;

    void bind() const;
    static void unbind();
    void release();

    [[nodiscard]]
    bool isReady() const { return m_status && m_vbo > 0; }
private:
    GLuint m_vbo = 0;
    bool m_status = false;
};