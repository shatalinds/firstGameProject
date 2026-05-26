//
// Created by dave on 26.05.2026.
//
#pragma once

#include <chrono>
#include <glm/glm/glm.hpp>
#include "AObject.h"
#include "camera/fgp_camera_config.h"
#include "math/fgp_math.h"

class FgpCamera final : public AObject {
public:
    void tick();

    [[nodiscard]]
    FgpSceneTransforms sceneTransforms(float aspect, const FgpBounds& bounds) const;

    static const glm::vec3 kWorldLightDirection;
    static const glm::vec3 kWorldUp;

private:
    void advanceOrbitAngle(float deltaSeconds);

    [[nodiscard]]
    glm::vec3 orbitEyePosition(const FgpBounds& bounds) const;

    [[nodiscard]]
    glm::mat4 orbitViewMatrix(const FgpBounds& bounds) const;

    [[nodiscard]]
    static glm::mat4 perspectiveForBounds(float aspect, const FgpBounds& bounds);

    [[nodiscard]]
    static glm::vec3 lightDirectionView(const glm::mat4& view);

    float m_orbitAngle = 0.f;
    std::chrono::steady_clock::time_point m_lastFrameTime{};
    bool m_hasLastFrameTime = false;
};