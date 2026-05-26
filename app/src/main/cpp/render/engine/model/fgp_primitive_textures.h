//
// Created by dave on 26.05.2026.
//

#pragma once

#include "texture/fgp_texture.h"

struct FgpPrimitiveTextures {
    FgpTexture* diffuse = nullptr;
    FgpTexture* normal = nullptr;
    FgpTexture* specGloss = nullptr;
    FgpTexture* occlusion = nullptr;
};