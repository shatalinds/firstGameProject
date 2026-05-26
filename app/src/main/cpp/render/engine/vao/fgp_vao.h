//
// Created by dave on 30.04.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include "AObject.h"

class FgpVao final: public AObject {
public:
    FgpVao();
    ~FgpVao() override { release(); }

    FgpVao(FgpVao&& other) noexcept;
    FgpVao& operator=(FgpVao&& other) noexcept;

    void bind() const;
    static void unbind();
    void release();

    [[nodiscard]]
    bool isReady() const { return m_status && m_vao > 0; }
private:
    GLuint m_vao = 0;
    bool m_status = false;
};