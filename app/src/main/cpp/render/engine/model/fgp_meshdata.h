//
// Created by dave on 25.05.2026.
//

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "AObject.h"
#include "math/fgp_math.h"
#include "fgp_mesh_material.h"
#include "tiny_gltf.h"

class FgpMeshData : public AObject {
public:
    FgpMeshData() = default;

    bool loadFromGltfAsync(const std::vector<uint8_t> &binaryData);

    [[nodiscard]]
    const std::vector<FgpRawImage>& getRawImages() const { return m_rawImages; }

    [[nodiscard]]
    const std::vector<FgpSubMeshData> &getSubMeshes() const { return m_subMeshes; }

    [[nodiscard]]
    const FgpBounds& getBounds() const { return m_bounds; }

private:
    std::vector<FgpSubMeshData> m_subMeshes;
    std::vector<FgpRawImage> m_rawImages;
    FgpBounds m_bounds;

    static void parseAttributes(const tinygltf::Model &model, const tinygltf::Primitive &prim,
                                FgpSubMeshData &subMesh, FgpAabb& bounds);

    static void parseIndices(const tinygltf::Model &model, const tinygltf::Primitive &prim,
                             FgpSubMeshData &subMesh);

    static void parseMaterial(const tinygltf::Model &model, const tinygltf::Primitive &prim,
                              FgpSubMeshData &subMesh);
};