//
// Created by dave on 21.05.2026.
//

#pragma once

#include <GLES3/gl3.h>
#include <string>
#include <atomic>
#include <vector>
#include <thread>
#include <android/asset_manager.h>
#include "fgp_primitive.h"
#include "fgp_meshdata.h"

class FgpModel final : public AObject {
public:
    enum class Status {
        NotLoaded,
        Loading,
        Parsed,
        Ready,
        Error
    };

    FgpModel() = default;
    ~FgpModel() override;

    bool loadFromAsset(AAssetManager *mgr, const std::string &assetPath);
    void update();
    void draw(const class FgpShader& shader) const;

    [[nodiscard]]
    Status getStatus() const { return m_status.load(); }

    [[nodiscard]]
    const FgpBounds& getBounds() const { return m_bounds; }

private:
    void backgroundParse(std::vector<uint8_t>&& binaryData);

private:
    std::vector< std::unique_ptr<FgpPrimitive> > m_primitives;
    std::vector< std::unique_ptr<FgpTexture> > m_textures;
    std::atomic<Status> m_status{Status::NotLoaded};
    std::shared_ptr<FgpMeshData> m_tmpMeshData;
    std::thread m_loadingThread;
    FgpBounds m_bounds;
};