//
// Created by dave on 27.04.2026.
//

#include "fgp_render.h"
#include "logger.h"
#include "engine/shader/fgp_shader.h"
#include "model/fgp_meshdata.h"
#include "tyrannosaurus.h"

bool FgpRender::init(int width, int height, AAssetManager* assetMgr) {
    m_assetMgr = assetMgr;
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Модель doubleSided — не отсекаем задние грани
    glDisable(GL_CULL_FACE);
    return initShaders();
}

bool FgpRender::initShaders() {
    m_shader = std::make_unique<FgpShader>(TYRANNOSAURUS_VERT, TYRANNOSAURUS_FRAG);
    if (!m_shader->getStatus()) {
        return false;
    }

    m_shader->useProgram();
    m_shader->bindTextureUnits();
    FgpShader::clearProgram();

    m_model = std::make_unique<FgpModel>();
    m_model->loadFromAsset(m_assetMgr, "models/tyrannosaurus.glb");
    LOGI("Shaders ready");
    m_ready = true;
    return true;
}

void FgpRender::beginFrame() {
    m_camera.tick();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if (m_ready && m_model) {
        m_model->update();
    }
}

void FgpRender::drawModel() {
    if (!m_ready || !m_shader->getStatus() || !m_model) return;
    if (m_model->getStatus() != FgpModel::Status::Ready) return;

    const float aspect = m_height > 0 ? static_cast<float>(m_width) / static_cast<float>(m_height) : 1.f;
    const FgpSceneTransforms transforms = m_camera.sceneTransforms(aspect, m_model->getBounds());

    m_shader->useProgram();
    m_shader->setSceneUniforms(transforms);
    m_model->draw(*m_shader);
    FgpShader::clearProgram();
}

void FgpRender::endFrame() {
}

void FgpRender::destroy() {
    m_ready = false;
}
