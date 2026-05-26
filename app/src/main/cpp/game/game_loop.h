//
// Created by dave on 27.04.2026.
//

#pragma once

#include <android/native_window.h>
#include <android/asset_manager.h>
#include <thread>
#include "../platform/egl_manager.h"
#include "fgp_render.h"
#include "AObject.h"

class GameLoop: public AObject {
public:
    GameLoop() = default;
    ~GameLoop() override { destroy(); }

    bool init(ANativeWindow* window, AAssetManager* assetMgr);
    void destroy();

    [[nodiscard]]
    bool isRunning() const { return m_running.load(); }
private:
    void threadWorker(ANativeWindow* window);
    void tick();

    EglManager m_egl;
    std::unique_ptr<FgpRender> m_renderer;
    AAssetManager* m_assetMgr = nullptr;

    std::thread m_gameThread;
    std::atomic<bool> m_running{false};
};