#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include "../game/game_loop.h"
#include "logger.h"

static GameLoop *ptrGameLoop = nullptr;

extern "C" {

JNIEXPORT void JNICALL
Java_com_firstgameproject_game_GameSurfaceView_nativeSurfaceCreated(
        JNIEnv *env,
        jobject /* this */,
        jobject surface,
        jobject assetManager) {

    ANativeWindow *ptrWindow = ANativeWindow_fromSurface(env, surface);
    if (!ptrWindow) {
        LOGE("Failed to get ANativeWindow");
        return;
    }

    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);

    if (ptrGameLoop != nullptr) {
        ptrGameLoop->destroy();
        delete ptrGameLoop;
        ptrGameLoop = nullptr;
    }

    ptrGameLoop = new GameLoop();

    if (!ptrGameLoop->init(ptrWindow, mgr)) {
        LOGE("GameLoop init failed");
        delete ptrGameLoop;
        ptrGameLoop = nullptr;
    }

    ANativeWindow_release(ptrWindow);
}

JNIEXPORT void JNICALL
Java_com_firstgameproject_game_GameSurfaceView_nativeSurfaceDestroyed(
        JNIEnv *ptrEnv,
        jobject /* this */) {
    if (ptrGameLoop != nullptr) {
        ptrGameLoop->destroy();
        delete ptrGameLoop;
        ptrGameLoop = nullptr;
    }
    LOGI("Surface destroyed JNI bridge");
}

}  // extern "C"