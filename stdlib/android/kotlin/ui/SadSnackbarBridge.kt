// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadSnackbarBridge.kt
// الوصف: جسر Kotlin لـ Snackbar
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.view.View
import com.google.android.material.snackbar.Snackbar

/**
 * جسر Snackbar — SadSnackbarBridge
 */
object SadSnackbarBridge {
    
    private var activity: Activity? = null
    private var rootView: View? = null
    private var currentSnackbar: Snackbar? = null
    
    @JvmStatic
    fun initialize(act: Activity) {
        activity = act
        rootView = act.findViewById(android.R.id.content)
    }
    
    @JvmStatic
    fun show(message: String, duration: Int = Snackbar.LENGTH_SHORT) {
        val view = rootView ?: return
        
        activity?.runOnUiThread {
            currentSnackbar?.dismiss()
            currentSnackbar = Snackbar.make(view, message, duration)
            currentSnackbar?.show()
        }
    }
    
    @JvmStatic
    fun showShort(message: String) {
        show(message, Snackbar.LENGTH_SHORT)
    }
    
    @JvmStatic
    fun showLong(message: String) {
        show(message, Snackbar.LENGTH_LONG)
    }
    
    @JvmStatic
    fun showIndefinite(message: String) {
        show(message, Snackbar.LENGTH_INDEFINITE)
    }
    
    @JvmStatic
    fun showWithAction(
        message: String,
        actionText: String,
        duration: Int = Snackbar.LENGTH_LONG,
        onAction: () -> Unit
    ) {
        val view = rootView ?: return
        
        activity?.runOnUiThread {
            currentSnackbar?.dismiss()
            currentSnackbar = Snackbar.make(view, message, duration)
                .setAction(actionText) { onAction() }
            currentSnackbar?.show()
        }
    }
    
    @JvmStatic
    fun showWithUndo(message: String, onUndo: () -> Unit) {
        showWithAction(message, "تراجع", Snackbar.LENGTH_LONG, onUndo)
    }
    
    @JvmStatic
    fun showWithRetry(message: String, onRetry: () -> Unit) {
        showWithAction(message, "إعادة المحاولة", Snackbar.LENGTH_INDEFINITE, onRetry)
    }
    
    @JvmStatic
    fun dismiss() {
        activity?.runOnUiThread {
            currentSnackbar?.dismiss()
            currentSnackbar = null
        }
    }
    
    @JvmStatic
    fun isShown(): Boolean = currentSnackbar?.isShown == true
}
