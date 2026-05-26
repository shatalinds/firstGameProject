package com.firstgameproject

import android.app.Application
import android.content.Context
import android.content.res.Configuration
import com.firstgameproject.di.appModule
import org.koin.android.ext.koin.androidContext
import org.koin.android.ext.koin.androidLogger
import org.koin.core.context.GlobalContext.startKoin

class FirstGameApplication : Application() {

    companion object Instance {
        var instance: FirstGameApplication? = null
            private set
    }

    override fun onCreate() {
        super.onCreate()

        startKoin{
            androidLogger()
            androidContext(this@FirstGameApplication)
            modules(appModule)
        }
    }

    override fun attachBaseContext(base: Context?) {
        super.attachBaseContext(base)
        instance = this
    }

    override fun onConfigurationChanged(newConfig: Configuration) {
        super.onConfigurationChanged(newConfig)
    }

    override fun onLowMemory() {
        super.onLowMemory()
    }

    override fun onTerminate() {
        super.onTerminate()
    }

    override fun onTrimMemory(level: Int) {
        super.onTrimMemory(level)
    }
}