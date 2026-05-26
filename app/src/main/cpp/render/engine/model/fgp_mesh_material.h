//
// Created by dave on 26.05.2026.
//

#pragma once

#include <vector>

struct FgpRawImage {
    std::vector<unsigned char> pixels;
    int width = 0;
    int height = 0;
    int components = 0;
};

struct FgpSubMeshMaterial {
    int diffuseImage = -1;
    int normalImage = -1;
    int specGlossImage = -1;
    int occlusionImage = -1;
};

struct FgpSubMeshData {
    std::vector<float> vertexData;
    std::vector<uint16_t> indices16;
    std::vector<uint32_t> indices32;
    bool use16BitIndices = true;
    FgpSubMeshMaterial material;
};