//
// Created by dave on 25.05.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include "engine/texture/fgp_texture.h"
#include "fgp_primitive_textures.h"
#include "vbo/fgp_vbo.h"
#include "vao/fgp_vao.h"
#include "ebo/fgp_ebo.h"
#include "fgp_meshdata.h"

class FgpPrimitive final : public AObject{
public:
    FgpPrimitive(const FgpSubMeshData& subMesh, const FgpPrimitiveTextures& textures);
    ~FgpPrimitive() override = default;

    void draw() const;
    void applyMaterialUniforms(const class FgpShader& shader) const;

    [[nodiscard]]
    bool hasDiffuse() const { return m_textures.diffuse != nullptr && m_textures.diffuse->isValid(); }

private:
    FgpVao vao;
    FgpVbo vbo;
    FgpEbo ebo;
    GLsizei indexCount = 0;
    GLenum  indexType   = GL_UNSIGNED_INT;
    FgpPrimitiveTextures m_textures;
};
