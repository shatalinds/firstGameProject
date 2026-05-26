//
// Created by dave on 26.05.2026.
//
#pragma once

#include <glm/glm/glm.hpp>
#include "AObject.h"

struct FgpAabb {
    float min[3]{};
    float max[3]{};
};

struct FgpBounds {
    glm::vec3 center{0.f};
    float radius = 1.f;
    bool valid = false;
};

class FgpMath final : public AObject {
public:
    static void initAabb(FgpAabb& box);
    static void expandAabb(FgpAabb& box, float x, float y, float z);
    [[nodiscard]]
    static FgpBounds boundsFromAabb(const FgpAabb& box);
};
