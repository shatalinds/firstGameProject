//
// Created by dave on 27.04.2026.
//
#pragma once

#include <GLES3/gl3.h>
#include "engine/shader/fgp_shader.h"
#include "engine/vbo/fgp_vbo.h"
#include "engine/vao/fgp_vao.h"
#include <memory>
#include <android/asset_manager.h>
#include "engine/camera/fgp_camera.h"
#include "model/fgp_model.h"

class IFgpRender {
public:
    virtual ~IFgpRender() = default;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
};

class FgpRender : public IFgpRender, public AObject {
public:
    bool init(int width, int height, AAssetManager* assetMgr);
    void destroy();
    void beginFrame() override;
    void endFrame() override;
    void drawModel();

private:
    bool initShaders();

    AAssetManager* m_assetMgr = nullptr;
    std::unique_ptr<FgpShader> m_shader;
    std::unique_ptr<FgpModel> m_model;
    FgpCamera m_camera;

    int m_width = 0;
    int m_height = 0;
    bool m_ready = false;
};