
//
// Created by dave on 25.05.2026.
//
#include "fgp_primitive.h"
#include "engine/shader/fgp_shader.h"
#include "logger.h"

namespace {
    constexpr GLuint kDiffuseUnit = 0;
    constexpr GLuint kSpecGlossUnit = 1;
    constexpr GLuint kNormalUnit = 2;
    constexpr GLuint kOcclusionUnit = 3;
}

FgpPrimitive::FgpPrimitive(const FgpSubMeshData& subMesh, const FgpPrimitiveTextures& textures) :
        vao(),
        vbo(subMesh.vertexData.data(), static_cast<GLsizeiptr>(subMesh.vertexData.size() * sizeof(float))),
        ebo(
                subMesh.use16BitIndices
                ? (const void*)subMesh.indices16.data()
                : (const void*)subMesh.indices32.data(),
                subMesh.use16BitIndices
                ? (GLsizeiptr)(subMesh.indices16.size() * sizeof(uint16_t))
                : (GLsizeiptr)(subMesh.indices32.size() * sizeof(uint32_t))
        ),
        indexCount(static_cast<GLsizei>(subMesh.use16BitIndices ? subMesh.indices16.size() : subMesh.indices32.size())),
        indexType(subMesh.use16BitIndices ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT),
        m_textures(textures)
{
    // Layout: pos(3) | normal(3) | uv(2) | tangent(4) = 12 floats
    const GLsizei stride = 12 * sizeof(float);

    vao.bind();
    vbo.bind();
    ebo.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

    FgpVao::unbind();
    FgpVbo::unbind();
    FgpEbo::unbind();

    LOGI("FgpPrimitive ready: %d indices, maps(d=%d n=%d s=%d o=%d)",
         indexCount,
         m_textures.diffuse != nullptr,
         m_textures.normal != nullptr,
         m_textures.specGloss != nullptr,
         m_textures.occlusion != nullptr);
}

void FgpPrimitive::applyMaterialUniforms(const FgpShader& shader) const {
    shader.setMaterialUniforms(
            hasDiffuse(),
            m_textures.normal && m_textures.normal->isValid(),
            m_textures.specGloss && m_textures.specGloss->isValid(),
            m_textures.occlusion && m_textures.occlusion->isValid()
    );
}

void FgpPrimitive::draw() const {
    if (m_textures.diffuse && m_textures.diffuse->isValid()) {
        m_textures.diffuse->bindTo(kDiffuseUnit);
    }
    if (m_textures.specGloss && m_textures.specGloss->isValid()) {
        m_textures.specGloss->bindTo(kSpecGlossUnit);
    }
    if (m_textures.normal && m_textures.normal->isValid()) {
        m_textures.normal->bindTo(kNormalUnit);
    }
    if (m_textures.occlusion && m_textures.occlusion->isValid()) {
        m_textures.occlusion->bindTo(kOcclusionUnit);
    }

    vao.bind();
    glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
    FgpVao::unbind();

    FgpTexture::unbind();
}
