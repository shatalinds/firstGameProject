//
// Created by dave on 30.04.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include <memory>
#include <string>
#include "AObject.h"

class FgpTexture final: public AObject {
public:
    FgpTexture(const char* image, GLuint slot, std::string_view texType = "diffuse");

    FgpTexture(const unsigned char *bytes, int width, int height, int channels, GLuint slot,
               std::string_view texType = "diffuse");

    FgpTexture(FgpTexture&&) noexcept;
    FgpTexture& operator=(FgpTexture&&) noexcept;

    static void texUnit(GLuint& shader, const char* uniform, GLint unit);
    void bind() const;
    void bindTo(GLuint unit) const;
    static void unbind();

    ~FgpTexture() override;

    [[nodiscard]]
    bool isValid() const { return m_texture != 0; }

    [[nodiscard]]
    const std::string& getType() const { return m_type; }
private:
    void genText(const unsigned char *bytes, int width, int height, int channels, GLuint slot,
                 std::string_view texType);
    GLuint m_texture = 0;
    std::string m_type;
    GLuint m_unit = 0;
    bool m_ownsTexture = true;
};