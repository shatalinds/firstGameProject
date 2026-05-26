//
// Created by dave on 26.05.2026.
//

#include "fgp_camera.h"

#include <cmath>
#include <glm/glm/gtc/matrix_transform.hpp>

const glm::vec3 FgpCamera::kWorldLightDirection{0.35f, 0.55f, 0.75f};
const glm::vec3 FgpCamera::kWorldUp{0.f, 1.f, 0.f};

void FgpCamera::advanceOrbitAngle(float deltaSeconds) {
    m_orbitAngle += deltaSeconds * FgpCameraConfig::kOrbitSpeedRadPerSec;
}

void FgpCamera::tick() {
    const auto now = std::chrono::steady_clock::now();
    if (m_hasLastFrameTime) {
        const float dt = std::chrono::duration<float>(now - m_lastFrameTime).count();
        advanceOrbitAngle(dt);
    }
    m_lastFrameTime = now;
    m_hasLastFrameTime = true;
}

glm::vec3 FgpCamera::orbitEyePosition(const FgpBounds& bounds) const {
    const float distance = bounds.radius * FgpCameraConfig::kOrbitDistanceFactor;
    return bounds.center + glm::vec3(
            distance * std::sin(m_orbitAngle),
            bounds.radius * FgpCameraConfig::kOrbitHeightFactor,
            distance * std::cos(m_orbitAngle)
    );
}

glm::mat4 FgpCamera::orbitViewMatrix(const FgpBounds& bounds) const {
    return glm::lookAt(orbitEyePosition(bounds), bounds.center, kWorldUp);
}

glm::mat4 FgpCamera::perspectiveForBounds(float aspect, const FgpBounds& bounds) {
    const float radius = bounds.valid ? bounds.radius : 1.f;
    return glm::perspective(
            glm::radians(FgpCameraConfig::kPerspectiveFovDeg),
            aspect,
            radius * FgpCameraConfig::kNearPlaneFactor,
            radius * FgpCameraConfig::kFarPlaneFactor
    );
}

glm::vec3 FgpCamera::lightDirectionView(const glm::mat4& view) {
    return glm::normalize(glm::vec3(view * glm::vec4(kWorldLightDirection, 0.f)));
}

FgpSceneTransforms FgpCamera::sceneTransforms(float aspect, const FgpBounds& bounds) const {
    const FgpBounds sceneBounds = bounds.valid ? bounds : FgpBounds{};

    FgpSceneTransforms transforms;
    transforms.model = glm::mat4(1.f);
    transforms.view = orbitViewMatrix(sceneBounds);
    transforms.projection = perspectiveForBounds(aspect, sceneBounds);
    transforms.lightDirView = lightDirectionView(transforms.view);
    return transforms;
}