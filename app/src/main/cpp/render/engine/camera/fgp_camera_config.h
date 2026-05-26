//
// Created by dave on 26.05.2026.
//
#pragma once

namespace FgpCameraConfig {
    inline constexpr float kOrbitSpeedRadPerSec = 0.45f;
    inline constexpr float kOrbitDistanceFactor = 2.5f;
    inline constexpr float kOrbitHeightFactor = 0.35f;
    inline constexpr float kPerspectiveFovDeg = 45.f;
    inline constexpr float kNearPlaneFactor = 0.01f;
    inline constexpr float kFarPlaneFactor = 20.f;
}

struct FgpSceneTransforms {
    glm::mat4 model{1.f};
    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::vec3 lightDirView{0.f, 0.f, 1.f};
};