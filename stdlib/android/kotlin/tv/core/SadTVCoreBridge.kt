package sad.android.tv.core

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.view.KeyEvent
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر نواة التلفزيون - الإدارة الأساسية لتطبيقات Android TV
 * TV Core Bridge - Basic management for Android TV apps
 * 
 * يدعم Leanback UI و D-pad navigation
 * 
 * @author فريق لغة ص
 */
object SadTVCoreBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var currentFocusPosition = FocusPosition(0, 0)
    private val focusListeners = mutableListOf<TVFocusListener>()
    
    /**
     * حالة التطبيق / App state
     */
    enum class TVAppState {
        INACTIVE,
        BROWSING,
        PLAYING,
        SEARCHING,
        SETTINGS,
        PAUSED
    }
    
    /**
     * موضع التركيز / Focus position
     */
    data class FocusPosition(
        val row: Int,
        val column: Int
    )
    
    /**
     * تكوين التلفزيون / TV config
     */
    data class TVConfig(
        val appName: String,
        val appVersion: String,
        val brandColor: Int = 0xFF1E88E5.toInt(),
        val backgroundColor: Int = 0xFF212121.toInt(),
        val enableVoiceSearch: Boolean = true,
        val enableRecommendations: Boolean = true,
        val autoPlayPreviews: Boolean = true,
        val previewDurationMs: Long = 3000
    )
    
    /**
     * نتيجة إجراء / Action result
     */
    data class ActionResult(
        val success: Boolean,
        val action: String,
        val message: String = ""
    )
    
    /**
     * مستمع التركيز / Focus listener
     */
    interface TVFocusListener {
        fun onFocusChanged(oldPos: FocusPosition, newPos: FocusPosition)
        fun onItemSelected(row: Int, column: Int)
        fun onItemClicked(row: Int, column: Int)
    }
    
    /**
     * مستمع حالة التطبيق / App state listener
     */
    interface TVAppStateListener {
        fun onStateChanged(oldState: TVAppState, newState: TVAppState)
    }
    
    private var currentState = TVAppState.INACTIVE
    private var config = TVConfig("تطبيق ص", "1.0.0")
    private val stateListeners = mutableListOf<TVAppStateListener>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: TVConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): TVConfig = config
    
    /**
     * إضافة مستمع التركيز / Add focus listener
     */
    fun addFocusListener(listener: TVFocusListener) {
        if (!focusListeners.contains(listener)) {
            focusListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع التركيز / Remove focus listener
     */
    fun removeFocusListener(listener: TVFocusListener) {
        focusListeners.remove(listener)
    }
    
    /**
     * إضافة مستمع الحالة / Add state listener
     */
    fun addStateListener(listener: TVAppStateListener) {
        if (!stateListeners.contains(listener)) {
            stateListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع الحالة / Remove state listener
     */
    fun removeStateListener(listener: TVAppStateListener) {
        stateListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحقق من البيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل الجهاز تلفزيون؟ / Is device a TV?
     */
    fun isTVDevice(): Boolean {
        val ctx = context ?: return false
        val uiModeManager = ctx.getSystemService(Context.UI_MODE_SERVICE) as? android.app.UiModeManager
        return uiModeManager?.currentModeType == android.content.res.Configuration.UI_MODE_TYPE_TELEVISION
    }
    
    /**
     * هل Leanback مدعوم؟ / Is Leanback supported?
     */
    fun isLeanbackSupported(): Boolean {
        val ctx = context ?: return false
        return ctx.packageManager.hasSystemFeature("android.software.leanback")
    }
    
    /**
     * هل يوجد شاشة لمس؟ / Has touchscreen?
     */
    fun hasTouchscreen(): Boolean {
        val ctx = context ?: return false
        return ctx.packageManager.hasSystemFeature("android.hardware.touchscreen")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الحالة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين الحالة / Set state
     */
    fun setState(newState: TVAppState) {
        val oldState = currentState
        if (oldState != newState) {
            currentState = newState
            mainHandler.post {
                stateListeners.forEach { it.onStateChanged(oldState, newState) }
            }
        }
    }
    
    /**
     * الحصول على الحالة / Get state
     */
    fun getState(): TVAppState = currentState
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة التركيز
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحريك التركيز / Move focus
     */
    fun moveFocus(direction: FocusDirection): ActionResult {
        val oldPos = currentFocusPosition
        val newPos = when (direction) {
            FocusDirection.UP -> FocusPosition(maxOf(0, oldPos.row - 1), oldPos.column)
            FocusDirection.DOWN -> FocusPosition(oldPos.row + 1, oldPos.column)
            FocusDirection.LEFT -> FocusPosition(oldPos.row, maxOf(0, oldPos.column - 1))
            FocusDirection.RIGHT -> FocusPosition(oldPos.row, oldPos.column + 1)
            FocusDirection.CENTER -> oldPos
        }
        
        currentFocusPosition = newPos
        
        mainHandler.post {
            focusListeners.forEach { it.onFocusChanged(oldPos, newPos) }
            focusListeners.forEach { it.onItemSelected(newPos.row, newPos.column) }
        }
        
        return ActionResult(true, "moveFocus", "تم تحريك التركيز")
    }
    
    /**
     * تحديد عنصر / Select item
     */
    fun selectCurrentItem(): ActionResult {
        val pos = currentFocusPosition
        mainHandler.post {
            focusListeners.forEach { it.onItemClicked(pos.row, pos.column) }
        }
        return ActionResult(true, "select", "تم تحديد العنصر")
    }
    
    /**
     * تعيين موضع التركيز / Set focus position
     */
    fun setFocusPosition(row: Int, column: Int) {
        val oldPos = currentFocusPosition
        currentFocusPosition = FocusPosition(row, column)
        mainHandler.post {
            focusListeners.forEach { it.onFocusChanged(oldPos, currentFocusPosition) }
        }
    }
    
    /**
     * الحصول على موضع التركيز / Get focus position
     */
    fun getFocusPosition(): FocusPosition = currentFocusPosition
    
    /**
     * اتجاه التركيز / Focus direction
     */
    enum class FocusDirection {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        CENTER
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة المفاتيح
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة ضغط مفتاح / Handle key press
     */
    fun handleKeyEvent(keyCode: Int): Boolean {
        return when (keyCode) {
            KeyEvent.KEYCODE_DPAD_UP -> {
                moveFocus(FocusDirection.UP)
                true
            }
            KeyEvent.KEYCODE_DPAD_DOWN -> {
                moveFocus(FocusDirection.DOWN)
                true
            }
            KeyEvent.KEYCODE_DPAD_LEFT -> {
                moveFocus(FocusDirection.LEFT)
                true
            }
            KeyEvent.KEYCODE_DPAD_RIGHT -> {
                moveFocus(FocusDirection.RIGHT)
                true
            }
            KeyEvent.KEYCODE_DPAD_CENTER, KeyEvent.KEYCODE_ENTER -> {
                selectCurrentItem()
                true
            }
            KeyEvent.KEYCODE_BACK -> {
                // معالجة الرجوع
                false
            }
            KeyEvent.KEYCODE_MEDIA_PLAY, KeyEvent.KEYCODE_MEDIA_PAUSE -> {
                true
            }
            else -> false
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        focusListeners.clear()
        stateListeners.clear()
        context = null
    }
}
