//
// Created by dave on 27.04.2026.
//

#include "egl_manager.h"
#include "logger.h"

bool EglManager::init(ANativeWindow* window) {
    m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglInitialize(m_display, nullptr, nullptr) == false) {
        LOGE("EGL found error initialization");
        return false;
    }

    EGLint attrs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,
            EGL_DEPTH_SIZE,      16,
            EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(m_display, attrs, &config, 1, &numConfigs);
    if (numConfigs == 0) {
        LOGE("No EGL config found");
        return false;
    }

    EGLint ctxAttrib[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    m_context = eglCreateContext(m_display, config, EGL_NO_CONTEXT, ctxAttrib);

    m_surface = eglCreateWindowSurface(m_display, config, window, nullptr);
    eglMakeCurrent(m_display, m_surface, m_surface, m_context);

    eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_width);
    eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_height);

    LOGI("EGL ready: %dx%d", m_width, m_height);
    m_ready = true;
    return true;
}

void EglManager::destroy() {
    if (m_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_context != EGL_NO_CONTEXT) eglDestroyContext(m_display, m_context);
        if (m_surface != EGL_NO_SURFACE) eglDestroySurface(m_display, m_surface);
        eglTerminate(m_display);
    }
    m_display = EGL_NO_DISPLAY;
    m_context = EGL_NO_CONTEXT;
    m_surface = EGL_NO_SURFACE;
    m_ready = false;
}

void EglManager::swapBuffers() {
    eglSwapBuffers(m_display, m_surface);
}