//
// Created by dave on 27.04.2026.
//
#include "game_loop.h"
#include "logger.h"

bool GameLoop::init(ANativeWindow* window, AAssetManager* assetMgr) {
    if (m_running.load()) {
        LOGW("GameLoop is already running");
        return true;
    }

    m_assetMgr = assetMgr;
    m_running.store(true);

    ANativeWindow_acquire(window);
    m_gameThread = std::thread(&GameLoop::threadWorker, this, window);
    LOGI("Render thread spawned successfully");
    return true;
}

void GameLoop::threadWorker(ANativeWindow *window) {
    LOGI("Render thread started execution");

    if (!m_egl.init(window)) {
        LOGE("EGL init failed");
        m_running.store(false);
        ANativeWindow_release(window);
        return;
    }

    ANativeWindow_release(window);

    m_renderer = std::make_unique<FgpRender>();
    if (!m_renderer->init(m_egl.getWidth(), m_egl.getHeight(), m_assetMgr)) {
        LOGE("Renderer initialization failed in render thread");
        m_egl.destroy();
        m_running.store(false);
        return;
    }

    LOGI("GameLoop setup complete. Entering main loop.");

    while (m_running.load()) {
        tick();
    }

    LOGI("Exiting main loop. Starting cleanup...");

    m_renderer->destroy();
    m_egl.destroy();

    LOGI("Render thread finished work");
}

void GameLoop::tick() {
    m_renderer->beginFrame();
    m_renderer->drawModel();
    m_renderer->endFrame();

    m_egl.swapBuffers();
}

void GameLoop::destroy() {
    if (!m_running.load()) return;

    m_running.store(false);

    if (m_gameThread.joinable()) {
        m_gameThread.join();
    }

    LOGI("GameLoop destroyed completely");
}