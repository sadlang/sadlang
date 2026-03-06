package sad.android.widgets.pin

import android.app.PendingIntent
import android.appwidget.AppWidgetManager
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper

/**
 * جسر تثبيت الويدجت - طلب إضافة ويدجت للشاشة الرئيسية
 * Widget Pin Bridge - Request adding widget to home screen
 * 
 * متاح من Android 8.0 (API 26) فما فوق
 * 
 * @author فريق لغة ص
 */
object SadWidgetPinBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نتيجة التثبيت / Pin result
     */
    enum class PinResult {
        SUCCESS,
        NOT_SUPPORTED,
        REQUEST_SENT,
        FAILED,
        ALREADY_PINNED
    }
    
    /**
     * معلومات التثبيت / Pin info
     */
    data class PinInfo(
        val isPinningSupported: Boolean,
        val isRequestPinPending: Boolean
    )
    
    /**
     * رد نداء التثبيت / Pin callback
     */
    interface PinCallback {
        fun onPinRequested()
        fun onPinSuccess(widgetId: Int)
        fun onPinFailed(reason: String)
        fun onPinNotSupported()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحقق من الدعم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل التثبيت مدعوم؟ / Is pinning supported?
     */
    fun isPinningSupported(): Boolean {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return false
        }
        
        val ctx = context ?: return false
        val appWidgetManager = AppWidgetManager.getInstance(ctx)
        return appWidgetManager.isRequestPinAppWidgetSupported
    }
    
    /**
     * الحصول على معلومات التثبيت / Get pin info
     */
    fun getPinInfo(): PinInfo {
        return PinInfo(
            isPinningSupported = isPinningSupported(),
            isRequestPinPending = false
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلب التثبيت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب تثبيت ويدجت / Request pin widget
     */
    fun requestPinWidget(
        providerClass: Class<*>,
        extras: Bundle? = null,
        successCallback: PendingIntent? = null
    ): PinResult {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return PinResult.NOT_SUPPORTED
        }
        
        val ctx = context ?: return PinResult.FAILED
        val appWidgetManager = AppWidgetManager.getInstance(ctx)
        
        if (!appWidgetManager.isRequestPinAppWidgetSupported) {
            return PinResult.NOT_SUPPORTED
        }
        
        val componentName = ComponentName(ctx, providerClass)
        
        return try {
            val result = appWidgetManager.requestPinAppWidget(
                componentName,
                extras,
                successCallback
            )
            
            if (result) PinResult.REQUEST_SENT else PinResult.FAILED
        } catch (e: Exception) {
            PinResult.FAILED
        }
    }
    
    /**
     * طلب تثبيت ويدجت مع callback / Request pin widget with callback
     */
    fun requestPinWidget(
        providerClass: Class<*>,
        callbackActivityClass: Class<*>,
        extras: Bundle? = null
    ): PinResult {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return PinResult.NOT_SUPPORTED
        }
        
        val ctx = context ?: return PinResult.FAILED
        
        val callbackIntent = Intent(ctx, callbackActivityClass).apply {
            action = ACTION_WIDGET_PINNED
        }
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        val successCallback = PendingIntent.getActivity(
            ctx,
            0,
            callbackIntent,
            flags
        )
        
        return requestPinWidget(providerClass, extras, successCallback)
    }
    
    /**
     * طلب تثبيت ويدجت بسيط / Request simple pin widget
     */
    fun requestPinWidgetSimple(providerClass: Class<*>): PinResult {
        return requestPinWidget(providerClass, null, null)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة النتيجة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة intent النتيجة / Handle result intent
     */
    fun handlePinResult(intent: Intent): Int? {
        if (intent.action != ACTION_WIDGET_PINNED) {
            return null
        }
        
        val widgetId = intent.getIntExtra(
            AppWidgetManager.EXTRA_APPWIDGET_ID,
            AppWidgetManager.INVALID_APPWIDGET_ID
        )
        
        return if (widgetId != AppWidgetManager.INVALID_APPWIDGET_ID) {
            widgetId
        } else {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الويدجت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على عدد الويدجت المثبتة / Get pinned widget count
     */
    fun getPinnedWidgetCount(providerClass: Class<*>): Int {
        val ctx = context ?: return 0
        val appWidgetManager = AppWidgetManager.getInstance(ctx)
        val componentName = ComponentName(ctx, providerClass)
        return appWidgetManager.getAppWidgetIds(componentName).size
    }
    
    /**
     * هل يوجد ويدجت مثبت؟ / Has pinned widgets?
     */
    fun hasPinnedWidgets(providerClass: Class<*>): Boolean {
        return getPinnedWidgetCount(providerClass) > 0
    }
    
    /**
     * الحصول على معرفات الويدجت المثبتة / Get pinned widget IDs
     */
    fun getPinnedWidgetIds(providerClass: Class<*>): IntArray {
        val ctx = context ?: return intArrayOf()
        val appWidgetManager = AppWidgetManager.getInstance(ctx)
        val componentName = ComponentName(ctx, providerClass)
        return appWidgetManager.getAppWidgetIds(componentName)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الثوابت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إجراء تثبيت الويدجت / Widget pinned action
     */
    const val ACTION_WIDGET_PINNED = "sad.android.widgets.ACTION_WIDGET_PINNED"
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        context = null
    }
}
