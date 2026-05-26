//
// Created by dave on 27.04.2026.
//
#pragma once

#include <EGL/egl.h>
#include <android/native_window.h>

class EglManager final {
public:
    bool init(ANativeWindow* window);
    void destroy();
    void swapBuffers();

    [[nodiscard]]
    int getWidth() const { return m_width; }

    [[nodiscard]]
    int getHeight() const { return m_height; }

    [[nodiscard]]
    bool isReady() const { return m_ready; }

private:
    EGLDisplay m_display = EGL_NO_DISPLAY;
    EGLSurface m_surface = EGL_NO_SURFACE;
    EGLContext m_context = EGL_NO_CONTEXT;
    int m_width = 0;
    int m_height = 0;
    bool m_ready = false;
};