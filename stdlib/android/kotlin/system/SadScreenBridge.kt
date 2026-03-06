// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadScreenBridge.kt
// الوصف: جسر Kotlin لإدارة الشاشة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import android.os.Build
import android.provider.Settings
import android.view.View
import android.view.WindowInsets
import android.view.WindowInsetsController
import android.view.WindowManager

/**
 * جسر الشاشة — SadScreenBridge
 */
object SadScreenBridge {
    
    private var activity: Activity? = null
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(act: Activity) {
        activity = act
        context = act.applicationContext
    }
    
    @JvmStatic
    fun keepScreenOn(enable: Boolean = true) {
        val act = activity ?: return
        act.runOnUiThread {
            if (enable) {
                act.window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
            } else {
                act.window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
            }
        }
    }
    
    @JvmStatic
    fun setFullscreen(fullscreen: Boolean = true) {
        val act = activity ?: return
        act.runOnUiThread {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                val controller = act.window.insetsController
                if (fullscreen) {
                    controller?.hide(WindowInsets.Type.statusBars() or WindowInsets.Type.navigationBars())
                    controller?.systemBarsBehavior = WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
                } else {
                    controller?.show(WindowInsets.Type.statusBars() or WindowInsets.Type.navigationBars())
                }
            } else {
                @Suppress("DEPRECATION")
                if (fullscreen) {
                    act.window.decorView.systemUiVisibility = (
                        View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        or View.SYSTEM_UI_FLAG_FULLSCREEN
                        or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    )
                } else {
                    act.window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_VISIBLE
                }
            }
        }
    }
    
    @JvmStatic
    fun hideStatusBar() {
        val act = activity ?: return
        act.runOnUiThread {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                act.window.insetsController?.hide(WindowInsets.Type.statusBars())
            } else {
                @Suppress("DEPRECATION")
                act.window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
            }
        }
    }
    
    @JvmStatic
    fun showStatusBar() {
        val act = activity ?: return
        act.runOnUiThread {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                act.window.insetsController?.show(WindowInsets.Type.statusBars())
            } else {
                @Suppress("DEPRECATION")
                act.window.clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
            }
        }
    }
    
    @JvmStatic
    fun hideNavigationBar() {
        val act = activity ?: return
        act.runOnUiThread {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                act.window.insetsController?.hide(WindowInsets.Type.navigationBars())
            } else {
                @Suppress("DEPRECATION")
                act.window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            }
        }
    }
    
    @JvmStatic
    fun getBrightness(): Float {
        val act = activity ?: return -1f
        val layoutParams = act.window.attributes
        return layoutParams.screenBrightness
    }
    
    @JvmStatic
    fun setBrightness(brightness: Float) {
        val act = activity ?: return
        act.runOnUiThread {
            val layoutParams = act.window.attributes
            layoutParams.screenBrightness = brightness.coerceIn(0f, 1f)
            act.window.attributes = layoutParams
        }
    }
    
    @JvmStatic
    fun setSystemBrightness(brightness: Int): Boolean {
        val ctx = context ?: return false
        
        if (!Settings.System.canWrite(ctx)) {
            return false
        }
        
        val value = brightness.coerceIn(0, 255)
        return Settings.System.putInt(ctx.contentResolver, Settings.System.SCREEN_BRIGHTNESS, value)
    }
    
    @JvmStatic
    fun getSystemBrightness(): Int {
        val ctx = context ?: return -1
        return try {
            Settings.System.getInt(ctx.contentResolver, Settings.System.SCREEN_BRIGHTNESS)
        } catch (e: Exception) {
            -1
        }
    }
    
    @JvmStatic
    fun getRotation(): Int {
        val act = activity ?: return -1
        @Suppress("DEPRECATION")
        return act.windowManager.defaultDisplay.rotation
    }
    
    @JvmStatic
    fun isPortrait(): Boolean {
        val act = activity ?: return true
        return act.resources.configuration.orientation == android.content.res.Configuration.ORIENTATION_PORTRAIT
    }
    
    @JvmStatic
    fun isLandscape(): Boolean {
        val act = activity ?: return false
        return act.resources.configuration.orientation == android.content.res.Configuration.ORIENTATION_LANDSCAPE
    }
}
