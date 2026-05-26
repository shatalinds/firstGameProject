//
// Created by dave on 26.05.2026.
//

#include "fgp_math.h"

#include <algorithm>
#include <cmath>
#include <limits>

void FgpMath::initAabb(FgpAabb& box) {
    for (int i = 0; i < 3; ++i) {
        box.min[i] = std::numeric_limits<float>::max();
        box.max[i] = std::numeric_limits<float>::lowest();
    }
}

void FgpMath::expandAabb(FgpAabb& box, float x, float y, float z) {
    box.min[0] = std::min(box.min[0], x);
    box.min[1] = std::min(box.min[1], y);
    box.min[2] = std::min(box.min[2], z);
    box.max[0] = std::max(box.max[0], x);
    box.max[1] = std::max(box.max[1], y);
    box.max[2] = std::max(box.max[2], z);
}

FgpBounds FgpMath::boundsFromAabb(const FgpAabb& box) {
    FgpBounds bounds;
    bounds.center = glm::vec3(
            (box.min[0] + box.max[0]) * 0.5f,
            (box.min[1] + box.max[1]) * 0.5f,
            (box.min[2] + box.max[2]) * 0.5f
    );

    const float dx = box.max[0] - box.min[0];
    const float dy = box.max[1] - box.min[1];
    const float dz = box.max[2] - box.min[2];
    bounds.radius = 0.5f * std::sqrt(dx * dx + dy * dy + dz * dz);
    if (bounds.radius < 1e-4f) {
        bounds.radius = 1.f;
    }
    bounds.valid = true;
    return bounds;
}
