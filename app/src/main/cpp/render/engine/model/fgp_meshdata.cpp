//
// Created by dave on 25.05.2026.
//


#include "fgp_meshdata.h"

#include <vector>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include "fgp_model.h"
#include "logger.h"

static const uint8_t *accessorPtr(const tinygltf::Model &model, int accessorIdx) {
    const auto &acc = model.accessors[accessorIdx];
    const auto &view = model.bufferViews[acc.bufferView];
    const auto &buf = model.buffers[view.buffer];
    return buf.data.data() + view.byteOffset + acc.byteOffset;
}

static size_t accessorStride(const tinygltf::Model &model, int accessorIdx) {
    const auto &acc = model.accessors[accessorIdx];
    const auto &view = model.bufferViews[acc.bufferView];
    if (view.byteStride != 0) return view.byteStride;
    int compSize = tinygltf::GetComponentSizeInBytes(acc.componentType);
    int compCount = tinygltf::GetNumComponentsInType(acc.type);
    return compSize * compCount;
}

static void printModelInfo(const tinygltf::Model &model) {
    LOGI("Model parsed OK: %zu mesh(es)", model.meshes.size());
    for (auto &mesh: model.meshes) {
        LOGI("  mesh '%s': %zu primitive(s)",
             mesh.name.c_str(), mesh.primitives.size());
    }

    for (auto &mesh: model.meshes) {
        for (auto &prim: mesh.primitives) {
            LOGI("  indices accessor: %d", prim.indices);
            for (auto &attr: prim.attributes) {
                LOGI("  attribute: %s -> accessor %d",
                     attr.first.c_str(), attr.second);
            }
        }
    }

    auto &posAcc = model.accessors[1];
    LOGI("  POSITION: count=%zu, componentType=%d, type=%d",
         posAcc.count,
         posAcc.componentType,
         posAcc.type);

    auto &idxAcc = model.accessors[0];
    LOGI("  indices: count=%zu, componentType=%d",
         idxAcc.count,
         idxAcc.componentType);

    LOGI("animations: %zu", model.animations.size());
    LOGI("skins: %zu", model.skins.size());

}

bool FgpMeshData::loadFromGltfAsync(const std::vector<uint8_t> &binaryData) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ok = loader.LoadBinaryFromMemory(
            &model, &err, &warn, binaryData.data(), binaryData.size());

    if (!warn.empty()) LOGW("TinyGLTF warn: %s", warn.c_str());
    if (!ok) {
        LOGE("TinyGLTF error: %s", err.c_str());
        return false;
    }

    printModelInfo(model);

    m_subMeshes.clear();
    m_bounds = {};
    FgpAabb aabb;
    FgpMath::initAabb(aabb);

    for (const auto &mesh: model.meshes) {
        for (const auto &prim: mesh.primitives) {
            if (prim.attributes.find("POSITION") == prim.attributes.end()) {
                continue;
            }
            FgpSubMeshData subMesh;
            parseAttributes(model, prim, subMesh, aabb);
            parseIndices(model, prim, subMesh);
            parseMaterial(model, prim, subMesh);

            m_subMeshes.push_back(std::move(subMesh));
        }
    }

    m_rawImages.clear();
    m_rawImages.reserve(model.images.size());
    for (size_t i = 0; i < model.images.size(); ++i) {
        const auto& gltfImg = model.images[i];
        FgpRawImage rawImg;
        rawImg.width = gltfImg.width;
        rawImg.height = gltfImg.height;
        rawImg.components = gltfImg.component;
        rawImg.pixels = gltfImg.image;

        LOGI("Image[%zu]: %dx%d ch=%d bytes=%zu",
             i, rawImg.width, rawImg.height, rawImg.components, rawImg.pixels.size());
        m_rawImages.push_back(std::move(rawImg));
    }

    if (!m_subMeshes.empty()) {
        m_bounds = FgpMath::boundsFromAabb(aabb);
        LOGI("Model bounds: center=(%.2f, %.2f, %.2f) radius=%.2f",
             m_bounds.center.x, m_bounds.center.y, m_bounds.center.z, m_bounds.radius);
    }

    return !m_subMeshes.empty();
}

void FgpMeshData::parseAttributes(
        const tinygltf::Model &model,
        const tinygltf::Primitive &prim,
        FgpSubMeshData &subMesh,
        FgpAabb& bounds
) {
    int posIdx = prim.attributes.at("POSITION");
    const bool hasNormal = prim.attributes.count("NORMAL") > 0;
    const bool hasUv = prim.attributes.count("TEXCOORD_0") > 0;
    const bool hasTangent = prim.attributes.count("TANGENT") > 0;
    const int normIdx = hasNormal ? prim.attributes.at("NORMAL") : -1;
    const int uvIdx = hasUv ? prim.attributes.at("TEXCOORD_0") : -1;
    const int tangentIdx = hasTangent ? prim.attributes.at("TANGENT") : -1;

    const uint8_t *posPtr = accessorPtr(model, posIdx);
    const uint8_t *normPtr = hasNormal ? accessorPtr(model, normIdx) : nullptr;
    const uint8_t *uvPtr = hasUv ? accessorPtr(model, uvIdx) : nullptr;
    const uint8_t *tangentPtr = hasTangent ? accessorPtr(model, tangentIdx) : nullptr;

    size_t posStride = accessorStride(model, posIdx);
    size_t normStride = hasNormal ? accessorStride(model, normIdx) : 0;
    size_t uvStride = hasUv ? accessorStride(model, uvIdx) : 0;
    size_t tangentStride = hasTangent ? accessorStride(model, tangentIdx) : 0;

    size_t vertexCount = model.accessors[posIdx].count;
    subMesh.vertexData.reserve(vertexCount * 12);

    for (size_t i = 0; i < vertexCount; i++) {
        const auto *p = reinterpret_cast<const float *>(posPtr + i * posStride);
        float n[3] = {0.f, 1.f, 0.f};
        float uv[2] = {0.f, 0.f};
        float t[4] = {1.f, 0.f, 0.f, 1.f};

        if (normPtr) {
            const auto *nv = reinterpret_cast<const float *>(normPtr + i * normStride);
            n[0] = nv[0]; n[1] = nv[1]; n[2] = nv[2];
        }
        if (uvPtr) {
            const auto *tv = reinterpret_cast<const float *>(uvPtr + i * uvStride);
            uv[0] = tv[0]; uv[1] = tv[1];
        }
        if (tangentPtr) {
            const auto *tv = reinterpret_cast<const float *>(tangentPtr + i * tangentStride);
            t[0] = tv[0]; t[1] = tv[1]; t[2] = tv[2]; t[3] = tv[3];
        }

        subMesh.vertexData.push_back(p[0]);  subMesh.vertexData.push_back(p[1]);  subMesh.vertexData.push_back(p[2]);
        subMesh.vertexData.push_back(n[0]);  subMesh.vertexData.push_back(n[1]);  subMesh.vertexData.push_back(n[2]);
        subMesh.vertexData.push_back(uv[0]); subMesh.vertexData.push_back(uv[1]);
        subMesh.vertexData.push_back(t[0]);  subMesh.vertexData.push_back(t[1]);
        subMesh.vertexData.push_back(t[2]);  subMesh.vertexData.push_back(t[3]);

        FgpMath::expandAabb(bounds, p[0], p[1], p[2]);
    }
}

void FgpMeshData::parseIndices(
        const tinygltf::Model &model,
        const tinygltf::Primitive &prim,
        FgpSubMeshData &subMesh
) {
    if (prim.indices < 0) {
        return;
    }

    const auto &idxAcc = model.accessors[prim.indices];
    const uint8_t *idxPtr = accessorPtr(model, prim.indices);
    size_t idxCount = idxAcc.count;

    if (idxAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT) {
        subMesh.use16BitIndices = true;
        subMesh.indices16.resize(idxCount);
        std::memcpy(subMesh.indices16.data(), idxPtr, idxCount * sizeof(uint16_t));
    } else if (idxAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT) {
        subMesh.use16BitIndices = false;
        subMesh.indices32.resize(idxCount);
        std::memcpy(subMesh.indices32.data(), idxPtr, idxCount * sizeof(uint32_t));
    }
}

static int textureIndexToImageIndex(const tinygltf::Model &model, int textureIdx) {
    if (textureIdx < 0 || static_cast<size_t>(textureIdx) >= model.textures.size()) {
        return -1;
    }
    const int imageIdx = model.textures[textureIdx].source;
    if (imageIdx < 0 || static_cast<size_t>(imageIdx) >= model.images.size()) {
        return -1;
    }
    return imageIdx;
}

static int textureIndexFromExtension(const tinygltf::Material &mat,
                                     const char* extensionName,
                                     const char* textureField) {
    const auto extIt = mat.extensions.find(extensionName);
    if (extIt == mat.extensions.end() || !extIt->second.IsObject()) {
        return -1;
    }

    const auto &ext = extIt->second;
    if (!ext.Has(textureField)) {
        return -1;
    }

    const auto &textureInfo = ext.Get(textureField);
    if (!textureInfo.IsObject() || !textureInfo.Has("index")) {
        return -1;
    }

    return textureInfo.Get("index").GetNumberAsInt();
}

static int findDiffuseTextureIndex(const tinygltf::Material &mat) {
    int textureIdx = mat.pbrMetallicRoughness.baseColorTexture.index;
    if (textureIdx >= 0) {
        return textureIdx;
    }
    return textureIndexFromExtension(mat, "KHR_materials_pbrSpecularGlossiness", "diffuseTexture");
}

static int findSpecGlossTextureIndex(const tinygltf::Material &mat) {
    return textureIndexFromExtension(mat, "KHR_materials_pbrSpecularGlossiness", "specularGlossinessTexture");
}

void FgpMeshData::parseMaterial(const tinygltf::Model &model, const tinygltf::Primitive &prim,
                                FgpSubMeshData &subMesh) {
    subMesh.material = {};
    if (prim.material < 0 || static_cast<size_t>(prim.material) >= model.materials.size()) {
        return;
    }

    const auto &mat = model.materials[prim.material];

    subMesh.material.diffuseImage = textureIndexToImageIndex(model, findDiffuseTextureIndex(mat));
    subMesh.material.specGlossImage = textureIndexToImageIndex(model, findSpecGlossTextureIndex(mat));
    subMesh.material.normalImage = textureIndexToImageIndex(model, mat.normalTexture.index);
    subMesh.material.occlusionImage = textureIndexToImageIndex(model, mat.occlusionTexture.index);

    LOGI("Material '%s': diffuse=%d normal=%d specGloss=%d occlusion=%d",
         mat.name.c_str(),
         subMesh.material.diffuseImage,
         subMesh.material.normalImage,
         subMesh.material.specGlossImage,
         subMesh.material.occlusionImage);
}