// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadKeyboardBridge.kt
// الوصف: جسر Kotlin للوحة المفاتيح
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import android.graphics.Rect
import android.view.View
import android.view.ViewTreeObserver
import android.view.inputmethod.InputMethodManager

/**
 * جسر لوحة المفاتيح — SadKeyboardBridge
 */
object SadKeyboardBridge {
    
    private var activity: Activity? = null
    private var inputMethodManager: InputMethodManager? = null
    private var keyboardListener: ViewTreeObserver.OnGlobalLayoutListener? = null
    private var onKeyboardVisibilityChanged: ((Boolean, Int) -> Unit)? = null
    
    @JvmStatic
    fun initialize(act: Activity) {
        activity = act
        inputMethodManager = act.getSystemService(Context.INPUT_METHOD_SERVICE) as? InputMethodManager
    }
    
    @JvmStatic
    fun show(view: View? = null) {
        val act = activity ?: return
        val imm = inputMethodManager ?: return
        
        act.runOnUiThread {
            val targetView = view ?: act.currentFocus ?: act.window.decorView
            targetView.requestFocus()
            imm.showSoftInput(targetView, InputMethodManager.SHOW_IMPLICIT)
        }
    }
    
    @JvmStatic
    fun hide() {
        val act = activity ?: return
        val imm = inputMethodManager ?: return
        
        act.runOnUiThread {
            val view = act.currentFocus ?: act.window.decorView
            imm.hideSoftInputFromWindow(view.windowToken, 0)
        }
    }
    
    @JvmStatic
    fun toggle() {
        val imm = inputMethodManager ?: return
        imm.toggleSoftInput(InputMethodManager.SHOW_IMPLICIT, 0)
    }
    
    @JvmStatic
    fun isVisible(): Boolean {
        val act = activity ?: return false
        val rootView = act.window.decorView
        val rect = Rect()
        rootView.getWindowVisibleDisplayFrame(rect)
        
        val screenHeight = rootView.height
        val keypadHeight = screenHeight - rect.bottom
        
        return keypadHeight > screenHeight * 0.15
    }
    
    @JvmStatic
    fun getKeyboardHeight(): Int {
        val act = activity ?: return 0
        val rootView = act.window.decorView
        val rect = Rect()
        rootView.getWindowVisibleDisplayFrame(rect)
        
        return rootView.height - rect.bottom
    }
    
    @JvmStatic
    fun addKeyboardVisibilityListener(callback: (Boolean, Int) -> Unit) {
        val act = activity ?: return
        
        removeKeyboardVisibilityListener()
        
        onKeyboardVisibilityChanged = callback
        val rootView = act.window.decorView
        
        keyboardListener = ViewTreeObserver.OnGlobalLayoutListener {
            val rect = Rect()
            rootView.getWindowVisibleDisplayFrame(rect)
            
            val screenHeight = rootView.height
            val keypadHeight = screenHeight - rect.bottom
            val isVisible = keypadHeight > screenHeight * 0.15
            
            onKeyboardVisibilityChanged?.invoke(isVisible, keypadHeight)
        }
        
        rootView.viewTreeObserver.addOnGlobalLayoutListener(keyboardListener)
    }
    
    @JvmStatic
    fun removeKeyboardVisibilityListener() {
        val act = activity ?: return
        
        keyboardListener?.let {
            act.window.decorView.viewTreeObserver.removeOnGlobalLayoutListener(it)
        }
        keyboardListener = null
        onKeyboardVisibilityChanged = null
    }
    
    @JvmStatic
    fun clearFocus() {
        val act = activity ?: return
        act.runOnUiThread {
            act.currentFocus?.clearFocus()
        }
    }
}
