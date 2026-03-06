// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadToastBridge.kt
// الوصف: جسر Kotlin لرسائل Toast
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.widget.Toast

/**
 * جسر رسائل Toast — SadToastBridge
 */
object SadToastBridge {
    
    private var context: Context? = null
    private var currentToast: Toast? = null
    private val handler = Handler(Looper.getMainLooper())
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun show(message: String, long: Boolean = false) {
        val ctx = context ?: return
        
        handler.post {
            currentToast?.cancel()
            currentToast = Toast.makeText(
                ctx,
                message,
                if (long) Toast.LENGTH_LONG else Toast.LENGTH_SHORT
            )
            currentToast?.show()
        }
    }
    
    @JvmStatic
    fun showShort(message: String) {
        show(message, false)
    }
    
    @JvmStatic
    fun showLong(message: String) {
        show(message, true)
    }
    
    @JvmStatic
    fun cancel() {
        handler.post {
            currentToast?.cancel()
            currentToast = null
        }
    }
    
    // رسائل مسبقة التنسيق
    @JvmStatic
    fun showSuccess(message: String) {
        show("✓ $message")
    }
    
    @JvmStatic
    fun showError(message: String) {
        show("✗ $message", true)
    }
    
    @JvmStatic
    fun showWarning(message: String) {
        show("⚠ $message")
    }
    
    @JvmStatic
    fun showInfo(message: String) {
        show("ℹ $message")
    }
}
