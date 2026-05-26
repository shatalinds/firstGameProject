//
// Created by dave on 21.05.2026.
//

#include "fgp_model.h"
#include "fgp_meshdata.h"
#include "engine/shader/fgp_shader.h"
#include "logger.h"
#include <thread>

bool FgpModel::loadFromAsset(AAssetManager* mgr, const std::string& assetPath) {
    AAsset* asset = AAssetManager_open(mgr, assetPath.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        LOGE("FgpModel: Cannot open asset '%s'", assetPath.c_str());
        return false;
    }

    size_t size = AAsset_getLength(asset);
    std::vector<uint8_t> binaryData(size);
    AAsset_read(asset, binaryData.data(), size);
    AAsset_close(asset);

    m_primitives.clear();
    m_status.store(Status::Loading);
    m_tmpMeshData = std::make_shared<FgpMeshData>();

    LOGI("FgpModel: Asset read OK. Starting background thread...");

    m_loadingThread = std::thread(&FgpModel::backgroundParse, this, std::move(binaryData));
    return true;
}

void FgpModel::backgroundParse(std::vector<uint8_t>&& binaryData) {
    bool success = m_tmpMeshData->loadFromGltfAsync(binaryData);

    if (success) {
        LOGI("Background jthread: Parsing completed successfully.");
        m_status.store(Status::Parsed);
    } else {
        LOGE("Background jthread: Parsing failed.");
        m_status.store(Status::Error);
    }
}

void FgpModel::update() {
    if (m_status.load() == Status::Parsed) {
        LOGI("Creating OpenGL buffers and textures in update()...");

        m_textures.clear();
        const auto& rawImages = m_tmpMeshData->getRawImages();

        for (size_t i = 0; i < rawImages.size(); ++i) {
            const auto& img = rawImages[i];
            if (img.width <= 0 || img.height <= 0 || img.pixels.empty()) {
                LOGE("FgpModel: skip image %zu (empty or %dx%d)", i, img.width, img.height);
                m_textures.push_back(nullptr);
                continue;
            }
            auto texture = std::make_unique<FgpTexture>(
                    img.pixels.data(),
                    img.width,
                    img.height,
                    img.components,
                    0,
                    "diffuse"
            );
            if (!texture->isValid()) {
                LOGE("FgpModel: failed to upload image %zu", i);
                m_textures.push_back(nullptr);
                continue;
            }
            LOGI("FgpModel: image %zu -> %dx%d ch=%d", i, img.width, img.height, img.components);
            m_textures.push_back(std::move(texture));
        }

        m_bounds = m_tmpMeshData->getBounds();

        const auto resolveTexture = [this](int imageIndex) -> FgpTexture* {
            if (imageIndex < 0 || static_cast<size_t>(imageIndex) >= m_textures.size()) {
                return nullptr;
            }
            FgpTexture* tex = m_textures[imageIndex].get();
            return tex && tex->isValid() ? tex : nullptr;
        };

        const auto& subMeshes = m_tmpMeshData->getSubMeshes();
        for (const auto& subMesh : subMeshes) {
            const auto& mat = subMesh.material;
            FgpPrimitiveTextures maps;
            maps.diffuse = resolveTexture(mat.diffuseImage);
            maps.normal = resolveTexture(mat.normalImage);
            maps.specGloss = resolveTexture(mat.specGlossImage);
            maps.occlusion = resolveTexture(mat.occlusionImage);
            auto primitive = std::make_unique<FgpPrimitive>(subMesh, maps);
            m_primitives.push_back(std::move(primitive));
        }

        m_tmpMeshData.reset();
        m_status.store(Status::Ready);
    }
}

void FgpModel::draw(const FgpShader& shader) const {
    if (m_status.load() != Status::Ready) {
        return;
    }

    for (const auto& primitive : m_primitives) {
        if (!primitive) {
            continue;
        }
        primitive->applyMaterialUniforms(shader);
        primitive->draw();
    }
}

FgpModel::~FgpModel() {
    if (m_loadingThread.joinable()) {
        LOGI("~FgpModel: Waiting for background thread to finish...");
        m_loadingThread.join();
    }
}