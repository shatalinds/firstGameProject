//
// Created by dave on 30.04.2026.
//

#include "fgp_shader.h"
#include "logger.h"
#include "global.h"
#include <glm/glm/gtc/type_ptr.hpp>
#include <utility>

void FgpShaderLight::cacheLocations(GLuint program) {
    direction = glGetUniformLocation(program, "lightDir");
    ambientColor = glGetUniformLocation(program, "ambientColor");
    diffuseColor = glGetUniformLocation(program, "diffuseColor");
}

void FgpShaderMaterials::cacheLocations(GLuint program) {
    projection = glGetUniformLocation(program, "projection");
    view = glGetUniformLocation(program, "view");
    model = glGetUniformLocation(program, "model");
    hasDiffuse = glGetUniformLocation(program, "hasDiffuse");
    hasNormal = glGetUniformLocation(program, "hasNormal");
    hasSpecGloss = glGetUniformLocation(program, "hasSpecGloss");
    hasOcclusion = glGetUniformLocation(program, "hasOcclusion");
    diffuseTex = glGetUniformLocation(program, "diffuseTex");
    specGlossTex = glGetUniformLocation(program, "specGlossTex");
    normalTex = glGetUniformLocation(program, "normalTex");
    occlusionTex = glGetUniformLocation(program, "occlusionTex");
}

FgpShader::FgpShader(const char *vertShader, const char *fragShader) {
    GLuint vert = compileShader(GL_VERTEX_SHADER, vertShader);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragShader);
    m_status = !(!vert || !frag);
    if (!m_status) {
        return;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    glLinkProgram(m_program);

    GLint linked = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint logLength = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0) {
            m_error.resize(logLength);
            glGetProgramInfoLog(m_program, logLength, nullptr, &m_error[0]);
            LOGE("Shader link error: %s", m_error.c_str());
        }

        glDeleteProgram(m_program);
        m_program = 0;
        m_status = false;
    } else {
        LOGI("Shader Link OK");
        cacheUniformLocations();
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void FgpShader::cacheUniformLocations() {
    m_light.cacheLocations(m_program);
    m_materials.cacheLocations(m_program);
}

GLuint FgpShader::compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        if (logLength > 0) {
            m_error.resize(logLength);
            glGetShaderInfoLog(shader, logLength, nullptr, &m_error[0]);
            LOGE("Shader compile error (%s): %s",
                 type == GL_VERTEX_SHADER ? "vert" : "frag", m_error.c_str());
        }

        glDeleteShader(shader);
        return 0;
    }
    LOGI("Shader compile OK: %s", type == GL_VERTEX_SHADER ? "vert" : "frag");
    return shader;
}

FgpShader::~FgpShader() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

void FgpShader::useProgram() const {
    glUseProgram(m_program);
}

void FgpShader::clearProgram() {
    glUseProgram(0);
}

void FgpShader::setMat4(GLint location, const glm::mat4& matrix) {
    if (location < 0) {
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void FgpShader::setVec3(GLint location, const glm::vec3& value) {
    if (location < 0) {
        return;
    }
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void FgpShader::setInt(GLint location, int value) {
    if (location < 0) {
        return;
    }
    glUniform1i(location, value);
}

void FgpShader::bindTextureUnits() const {
    setInt(m_materials.diffuseTex, 0);
    setInt(m_materials.specGlossTex, 1);
    setInt(m_materials.normalTex, 2);
    setInt(m_materials.occlusionTex, 3);
}

void FgpShader::setSceneUniforms(const FgpSceneTransforms& transforms) const {
    setMat4(m_materials.projection, transforms.projection);
    setMat4(m_materials.view, transforms.view);
    setMat4(m_materials.model, transforms.model);
    setVec3(m_light.direction, transforms.lightDirView);
    if (m_light.ambientColor >= 0) {
        glUniform3fv(m_light.ambientColor, 1, FgpShaderLight::kAmbientColor);
    }
    if (m_light.diffuseColor >= 0) {
        glUniform3fv(m_light.diffuseColor, 1, FgpShaderLight::kDiffuseLightColor);
    }
}

void FgpShader::setMaterialUniforms(bool hasDiffuse, bool hasNormal, bool hasSpecGloss, bool hasOcclusion) const {
    setInt(m_materials.hasDiffuse, hasDiffuse ? 1 : 0);
    setInt(m_materials.hasNormal, hasNormal ? 1 : 0);
    setInt(m_materials.hasSpecGloss, hasSpecGloss ? 1 : 0);
    setInt(m_materials.hasOcclusion, hasOcclusion ? 1 : 0);
}
