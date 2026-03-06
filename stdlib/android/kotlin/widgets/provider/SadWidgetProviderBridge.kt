package sad.android.widgets.provider

import android.appwidget.AppWidgetManager
import android.appwidget.AppWidgetProvider
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر مزود الويدجت - قاعدة AppWidgetProvider
 * Widget Provider Bridge - AppWidgetProvider base
 * 
 * @author فريق لغة ص
 */
object SadWidgetProviderBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val providerCallbacks = ConcurrentHashMap<String, ProviderCallback>()
    private val widgetProviders = ConcurrentHashMap<String, WidgetProviderInfo>()
    
    /**
     * معلومات المزود / Provider info
     */
    data class WidgetProviderInfo(
        val className: String,
        val minWidth: Int,
        val minHeight: Int,
        val updatePeriodMillis: Long,
        val previewImage: Int = 0,
        val resizeMode: Int = 3,  // RESIZE_HORIZONTAL | RESIZE_VERTICAL
        val widgetCategory: Int = 1  // HOME_SCREEN
    )
    
    /**
     * أحداث المزود / Provider events
     */
    enum class ProviderEvent {
        UPDATE,
        ENABLED,
        DISABLED,
        DELETED,
        OPTIONS_CHANGED,
        RESTORE,
        APP_WIDGET_OPTIONS_CHANGED
    }
    
    /**
     * بيانات الحدث / Event data
     */
    data class ProviderEventData(
        val event: ProviderEvent,
        val widgetIds: IntArray = intArrayOf(),
        val options: Bundle? = null
    )
    
    /**
     * رد نداء المزود / Provider callback
     */
    interface ProviderCallback {
        fun onEvent(eventData: ProviderEventData)
    }
    
    /**
     * مستمع دورة الحياة / Lifecycle listener
     */
    interface LifecycleListener {
        fun onWidgetCreated(widgetId: Int)
        fun onWidgetUpdated(widgetId: Int)
        fun onWidgetDeleted(widgetId: Int)
        fun onFirstWidgetEnabled()
        fun onLastWidgetDisabled()
    }
    
    private val lifecycleListeners = mutableListOf<LifecycleListener>()
    
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
     * تسجيل رد نداء / Register callback
     */
    fun registerCallback(providerKey: String, callback: ProviderCallback) {
        providerCallbacks[providerKey] = callback
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addLifecycleListener(listener: LifecycleListener) {
        if (!lifecycleListeners.contains(listener)) {
            lifecycleListeners.add(listener)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة المزودين
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل معلومات المزود / Register provider info
     */
    fun registerProvider(info: WidgetProviderInfo): Boolean {
        widgetProviders[info.className] = info
        return true
    }
    
    /**
     * الحصول على معلومات المزود / Get provider info
     */
    fun getProviderInfo(className: String): WidgetProviderInfo? {
        return widgetProviders[className]
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة الأحداث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة حدث التحديث / Handle update event
     */
    fun handleUpdate(providerKey: String, widgetIds: IntArray) {
        val eventData = ProviderEventData(ProviderEvent.UPDATE, widgetIds)
        
        providerCallbacks[providerKey]?.onEvent(eventData)
        
        mainHandler.post {
            widgetIds.forEach { id ->
                lifecycleListeners.forEach { it.onWidgetUpdated(id) }
            }
        }
    }
    
    /**
     * معالجة حدث التفعيل / Handle enabled event
     */
    fun handleEnabled(providerKey: String) {
        val eventData = ProviderEventData(ProviderEvent.ENABLED)
        providerCallbacks[providerKey]?.onEvent(eventData)
        
        mainHandler.post {
            lifecycleListeners.forEach { it.onFirstWidgetEnabled() }
        }
    }
    
    /**
     * معالجة حدث التعطيل / Handle disabled event
     */
    fun handleDisabled(providerKey: String) {
        val eventData = ProviderEventData(ProviderEvent.DISABLED)
        providerCallbacks[providerKey]?.onEvent(eventData)
        
        mainHandler.post {
            lifecycleListeners.forEach { it.onLastWidgetDisabled() }
        }
    }
    
    /**
     * معالجة حدث الحذف / Handle deleted event
     */
    fun handleDeleted(providerKey: String, widgetIds: IntArray) {
        val eventData = ProviderEventData(ProviderEvent.DELETED, widgetIds)
        providerCallbacks[providerKey]?.onEvent(eventData)
        
        mainHandler.post {
            widgetIds.forEach { id ->
                lifecycleListeners.forEach { it.onWidgetDeleted(id) }
            }
        }
    }
    
    /**
     * معالجة تغيير الخيارات / Handle options changed
     */
    fun handleOptionsChanged(providerKey: String, widgetId: Int, options: Bundle?) {
        val eventData = ProviderEventData(
            ProviderEvent.OPTIONS_CHANGED,
            intArrayOf(widgetId),
            options
        )
        providerCallbacks[providerKey]?.onEvent(eventData)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مزود أساسي
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مزود الويدجت الأساسي / Base widget provider
     */
    abstract class SadAppWidgetProvider : AppWidgetProvider() {
        
        /**
         * مفتاح المزود / Provider key
         */
        abstract val providerKey: String
        
        override fun onUpdate(
            context: Context,
            appWidgetManager: AppWidgetManager,
            appWidgetIds: IntArray
        ) {
            handleUpdate(providerKey, appWidgetIds)
            onWidgetUpdate(context, appWidgetManager, appWidgetIds)
        }
        
        override fun onEnabled(context: Context) {
            handleEnabled(providerKey)
            onWidgetEnabled(context)
        }
        
        override fun onDisabled(context: Context) {
            handleDisabled(providerKey)
            onWidgetDisabled(context)
        }
        
        override fun onDeleted(context: Context, appWidgetIds: IntArray) {
            handleDeleted(providerKey, appWidgetIds)
            onWidgetDeleted(context, appWidgetIds)
        }
        
        override fun onAppWidgetOptionsChanged(
            context: Context,
            appWidgetManager: AppWidgetManager,
            appWidgetId: Int,
            newOptions: Bundle?
        ) {
            handleOptionsChanged(providerKey, appWidgetId, newOptions)
            onWidgetOptionsChanged(context, appWidgetManager, appWidgetId, newOptions)
        }
        
        override fun onReceive(context: Context, intent: Intent) {
            super.onReceive(context, intent)
            onWidgetReceive(context, intent)
        }
        
        // دوال للتجاوز
        open fun onWidgetUpdate(
            context: Context,
            manager: AppWidgetManager,
            widgetIds: IntArray
        ) {}
        
        open fun onWidgetEnabled(context: Context) {}
        open fun onWidgetDisabled(context: Context) {}
        open fun onWidgetDeleted(context: Context, widgetIds: IntArray) {}
        
        open fun onWidgetOptionsChanged(
            context: Context,
            manager: AppWidgetManager,
            widgetId: Int,
            options: Bundle?
        ) {}
        
        open fun onWidgetReceive(context: Context, intent: Intent) {}
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على خيارات الويدجت / Get widget options
     */
    fun getWidgetOptions(widgetId: Int): Bundle? {
        val ctx = context ?: return null
        val manager = AppWidgetManager.getInstance(ctx)
        return manager.getAppWidgetOptions(widgetId)
    }
    
    /**
     * الحصول على عرض الويدجت / Get widget width
     */
    fun getWidgetWidth(widgetId: Int): Int {
        val options = getWidgetOptions(widgetId) ?: return 0
        return options.getInt(AppWidgetManager.OPTION_APPWIDGET_MIN_WIDTH, 0)
    }
    
    /**
     * الحصول على ارتفاع الويدجت / Get widget height
     */
    fun getWidgetHeight(widgetId: Int): Int {
        val options = getWidgetOptions(widgetId) ?: return 0
        return options.getInt(AppWidgetManager.OPTION_APPWIDGET_MIN_HEIGHT, 0)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        providerCallbacks.clear()
        widgetProviders.clear()
        lifecycleListeners.clear()
        context = null
    }
}
