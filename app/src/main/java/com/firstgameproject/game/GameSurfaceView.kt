package com.firstgameproject.game

import android.content.Context
import android.content.res.AssetManager
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView

class GameSurfaceView(context: Context) : SurfaceView(context),
    SurfaceHolder.Callback {

    private var isRunning = false
    private var surfaceReady = false

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        nativeSurfaceCreated(holder.surface, context.assets)
        surfaceReady = true
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        isRunning = false
        surfaceReady = false
        nativeSurfaceDestroyed()
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
    }

    fun onResume() {
        isRunning = true
    }

    fun onPause() {
        isRunning = false
    }

    private external fun nativeSurfaceCreated(surface: Surface, assetManager: AssetManager)
    private external fun nativeSurfaceDestroyed()

    companion object {
        init {
            System.loadLibrary("firstgameproject")
        }
    }
}