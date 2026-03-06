// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadVibrationBridge.kt
// الوصف: جسر Kotlin للاهتزاز
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Build
import android.os.VibrationEffect
import android.os.Vibrator
import android.os.VibratorManager

/**
 * جسر الاهتزاز — SadVibrationBridge
 */
object SadVibrationBridge {
    
    private var context: Context? = null
    private var vibrator: Vibrator? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        vibrator = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            val vm = ctx.getSystemService(Context.VIBRATOR_MANAGER_SERVICE) as? VibratorManager
            vm?.defaultVibrator
        } else {
            @Suppress("DEPRECATION")
            ctx.getSystemService(Context.VIBRATOR_SERVICE) as? Vibrator
        }
    }
    
    @JvmStatic
    fun hasVibrator(): Boolean = vibrator?.hasVibrator() == true
    
    @JvmStatic
    fun vibrate(durationMs: Long = 200) {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            vib.vibrate(VibrationEffect.createOneShot(durationMs, VibrationEffect.DEFAULT_AMPLITUDE))
        } else {
            @Suppress("DEPRECATION")
            vib.vibrate(durationMs)
        }
    }
    
    @JvmStatic
    fun vibrateWithAmplitude(durationMs: Long, amplitude: Int) {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val amp = amplitude.coerceIn(1, 255)
            vib.vibrate(VibrationEffect.createOneShot(durationMs, amp))
        } else {
            @Suppress("DEPRECATION")
            vib.vibrate(durationMs)
        }
    }
    
    @JvmStatic
    fun vibratePattern(pattern: LongArray, repeat: Int = -1) {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            vib.vibrate(VibrationEffect.createWaveform(pattern, repeat))
        } else {
            @Suppress("DEPRECATION")
            vib.vibrate(pattern, repeat)
        }
    }
    
    @JvmStatic
    fun vibrateClick() {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            vib.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_CLICK))
        } else {
            vibrate(50)
        }
    }
    
    @JvmStatic
    fun vibrateDoubleClick() {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            vib.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_DOUBLE_CLICK))
        } else {
            vibratePattern(longArrayOf(0, 50, 100, 50))
        }
    }
    
    @JvmStatic
    fun vibrateHeavyClick() {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            vib.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_HEAVY_CLICK))
        } else {
            vibrate(100)
        }
    }
    
    @JvmStatic
    fun vibrateTick() {
        val vib = vibrator ?: return
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            vib.vibrate(VibrationEffect.createPredefined(VibrationEffect.EFFECT_TICK))
        } else {
            vibrate(20)
        }
    }
    
    @JvmStatic
    fun vibrateSuccess() {
        vibratePattern(longArrayOf(0, 50, 50, 50))
    }
    
    @JvmStatic
    fun vibrateError() {
        vibratePattern(longArrayOf(0, 100, 100, 100, 100, 100))
    }
    
    @JvmStatic
    fun vibrateWarning() {
        vibratePattern(longArrayOf(0, 200, 100, 200))
    }
    
    @JvmStatic
    fun cancel() {
        vibrator?.cancel()
    }
}
