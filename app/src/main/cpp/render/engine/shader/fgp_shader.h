//
// Created by dave on 30.04.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include <string>
#include <glm/glm/glm.hpp>
#include "AObject.h"
#include "camera/fgp_camera.h"

struct FgpShaderLight {
    GLint direction = -1;
    GLint ambientColor = -1;
    GLint diffuseColor = -1;

    static constexpr float kAmbientColor[3] = {0.25f, 0.25f, 0.28f};
    static constexpr float kDiffuseLightColor[3] = {0.85f, 0.85f, 0.82f};

    void cacheLocations(GLuint program);
};

struct FgpShaderMaterials {
    GLint projection = -1;
    GLint view = -1;
    GLint model = -1;
    GLint hasDiffuse = -1;
    GLint hasNormal = -1;
    GLint hasSpecGloss = -1;
    GLint hasOcclusion = -1;
    GLint diffuseTex = -1;
    GLint specGlossTex = -1;
    GLint normalTex = -1;
    GLint occlusionTex = -1;

    void cacheLocations(GLuint program);
};

class FgpShader final : public AObject {
public:
    FgpShader(const char* vertShader, const char* fragShader);

    FgpShader(FgpShader&&) noexcept = default;
    FgpShader& operator=(FgpShader&&) noexcept = default;

    [[nodiscard]]
    GLuint getProgram() const { return m_program; }

    [[nodiscard]]
    GLboolean getStatus() const { return m_status; }

    [[nodiscard]]
    const std::string& getError() const { return m_error; }

    void useProgram() const;
    static void clearProgram();

    void bindTextureUnits() const;
    void setSceneUniforms(const FgpSceneTransforms& transforms) const;
    void setMaterialUniforms(bool hasDiffuse, bool hasNormal, bool hasSpecGloss, bool hasOcclusion) const;

    ~FgpShader() override;

private:
    GLuint compileShader(GLenum type, const char* src);
    void cacheUniformLocations();

    static void setMat4(GLint location, const glm::mat4& matrix);
    static void setVec3(GLint location, const glm::vec3& value);
    static void setInt(GLint location, int value);

    GLboolean m_status = false;
    std::string m_error;
    GLuint m_program = 0;

    FgpShaderLight m_light;
    FgpShaderMaterials m_materials;
};
