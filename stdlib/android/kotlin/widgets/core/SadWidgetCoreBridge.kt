package sad.android.widgets.core

import android.appwidget.AppWidgetManager
import android.content.ComponentName
import android.content.Context
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import java.util.concurrent.ConcurrentHashMap

/**
 * المحرك الأساسي للويدجت - إدارة دورة حياة الويدجت
 * Widget Core Engine - Widget lifecycle management
 * 
 * @author فريق لغة ص
 */
object SadWidgetCoreBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val widgetConfigs = ConcurrentHashMap<String, WidgetConfig>()
    private val widgetStates = ConcurrentHashMap<Int, WidgetState>()
    private val lifecycleListeners = mutableListOf<WidgetLifecycleListener>()
    
    /**
     * تكوين الويدجت / Widget configuration
     */
    data class WidgetConfig(
        val id: String,
        val className: String,
        val minWidth: Int = 40,
        val minHeight: Int = 40,
        val maxWidth: Int = 0,
        val maxHeight: Int = 0,
        val updatePeriodMillis: Long = 1800000, // 30 دقيقة
        val resizeMode: ResizeMode = ResizeMode.BOTH,
        val category: WidgetCategory = WidgetCategory.HOME_SCREEN,
        val previewLayout: Int = 0,
        val description: String = ""
    )
    
    /**
     * حالة الويدجت / Widget state
     */
    data class WidgetState(
        val widgetId: Int,
        val configId: String,
        val createdAt: Long = System.currentTimeMillis(),
        var lastUpdated: Long = System.currentTimeMillis(),
        var data: MutableMap<String, Any?> = mutableMapOf(),
        var isActive: Boolean = true
    )
    
    /**
     * وضع التحجيم / Resize mode
     */
    enum class ResizeMode {
        NONE,
        HORIZONTAL,
        VERTICAL,
        BOTH
    }
    
    /**
     * فئة الويدجت / Widget category
     */
    enum class WidgetCategory {
        HOME_SCREEN,
        KEYGUARD,     // شاشة القفل
        SEARCHBOX
    }
    
    /**
     * أحداث دورة الحياة / Lifecycle events
     */
    enum class LifecycleEvent {
        CREATED,
        UPDATED,
        DELETED,
        ENABLED,
        DISABLED,
        OPTIONS_CHANGED
    }
    
    /**
     * مستمع دورة الحياة / Lifecycle listener
     */
    interface WidgetLifecycleListener {
        fun onWidgetEvent(widgetId: Int, event: LifecycleEvent, data: Map<String, Any>?)
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
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addLifecycleListener(listener: WidgetLifecycleListener) {
        if (!lifecycleListeners.contains(listener)) {
            lifecycleListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeLifecycleListener(listener: WidgetLifecycleListener) {
        lifecycleListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تسجيل الويدجت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل تكوين ويدجت / Register widget config
     */
    fun registerWidgetConfig(config: WidgetConfig): Boolean {
        widgetConfigs[config.id] = config
        return true
    }
    
    /**
     * الحصول على تكوين / Get config
     */
    fun getWidgetConfig(configId: String): WidgetConfig? {
        return widgetConfigs[configId]
    }
    
    /**
     * جميع التكوينات / All configs
     */
    fun getAllConfigs(): List<WidgetConfig> {
        return widgetConfigs.values.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الحالة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء حالة ويدجت / Create widget state
     */
    fun createWidgetState(widgetId: Int, configId: String): WidgetState {
        val state = WidgetState(
            widgetId = widgetId,
            configId = configId
        )
        widgetStates[widgetId] = state
        notifyListeners(widgetId, LifecycleEvent.CREATED, null)
        return state
    }
    
    /**
     * الحصول على حالة / Get state
     */
    fun getWidgetState(widgetId: Int): WidgetState? {
        return widgetStates[widgetId]
    }
    
    /**
     * تحديث البيانات / Update data
     */
    fun updateWidgetData(widgetId: Int, key: String, value: Any?): Boolean {
        val state = widgetStates[widgetId] ?: return false
        state.data[key] = value
        state.lastUpdated = System.currentTimeMillis()
        return true
    }
    
    /**
     * تحديث مجموعة بيانات / Update batch data
     */
    fun updateWidgetDataBatch(widgetId: Int, data: Map<String, Any?>): Boolean {
        val state = widgetStates[widgetId] ?: return false
        state.data.putAll(data)
        state.lastUpdated = System.currentTimeMillis()
        return true
    }
    
    /**
     * الحصول على بيانات / Get data
     */
    fun getWidgetData(widgetId: Int, key: String): Any? {
        return widgetStates[widgetId]?.data?.get(key)
    }
    
    /**
     * مسح البيانات / Clear data
     */
    fun clearWidgetData(widgetId: Int): Boolean {
        val state = widgetStates[widgetId] ?: return false
        state.data.clear()
        return true
    }
    
    /**
     * حذف حالة / Delete state
     */
    fun deleteWidgetState(widgetId: Int): Boolean {
        val removed = widgetStates.remove(widgetId)
        if (removed != null) {
            notifyListeners(widgetId, LifecycleEvent.DELETED, null)
            return true
        }
        return false
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحديث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحديث ويدجت / Update widget
     */
    fun updateWidget(widgetId: Int, layoutId: Int, builder: RemoteViewsBuilder.() -> Unit): Boolean {
        val ctx = context ?: return false
        
        val remoteViews = RemoteViews(ctx.packageName, layoutId)
        val rvBuilder = RemoteViewsBuilder(remoteViews)
        builder(rvBuilder)
        
        val manager = AppWidgetManager.getInstance(ctx)
        manager.updateAppWidget(widgetId, remoteViews)
        
        widgetStates[widgetId]?.lastUpdated = System.currentTimeMillis()
        notifyListeners(widgetId, LifecycleEvent.UPDATED, null)
        
        return true
    }
    
    /**
     * تحديث جميع ويدجت من نوع / Update all widgets of type
     */
    fun updateAllWidgets(providerClass: Class<*>, layoutId: Int, builder: RemoteViewsBuilder.() -> Unit): Int {
        val ctx = context ?: return 0
        
        val manager = AppWidgetManager.getInstance(ctx)
        val componentName = ComponentName(ctx, providerClass)
        val widgetIds = manager.getAppWidgetIds(componentName)
        
        widgetIds.forEach { id ->
            updateWidget(id, layoutId, builder)
        }
        
        return widgetIds.size
    }
    
    /**
     * تحديث جزئي / Partial update
     */
    fun partialUpdateWidget(widgetId: Int, layoutId: Int, builder: RemoteViewsBuilder.() -> Unit): Boolean {
        val ctx = context ?: return false
        
        val remoteViews = RemoteViews(ctx.packageName, layoutId)
        val rvBuilder = RemoteViewsBuilder(remoteViews)
        builder(rvBuilder)
        
        val manager = AppWidgetManager.getInstance(ctx)
        manager.partiallyUpdateAppWidget(widgetId, remoteViews)
        
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         RemoteViews Builder
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بناء RemoteViews / RemoteViews builder
     */
    class RemoteViewsBuilder(private val views: RemoteViews) {
        
        /**
         * تعيين نص / Set text
         */
        fun setText(viewId: Int, text: String) {
            views.setTextViewText(viewId, text)
        }
        
        /**
         * تعيين صورة من مورد / Set image from resource
         */
        fun setImageResource(viewId: Int, resId: Int) {
            views.setImageViewResource(viewId, resId)
        }
        
        /**
         * تعيين الرؤية / Set visibility
         */
        fun setVisibility(viewId: Int, visible: Boolean) {
            views.setViewVisibility(viewId, if (visible) android.view.View.VISIBLE else android.view.View.GONE)
        }
        
        /**
         * تعيين لون النص / Set text color
         */
        fun setTextColor(viewId: Int, color: Int) {
            views.setTextColor(viewId, color)
        }
        
        /**
         * تعيين لون الخلفية / Set background color
         */
        fun setBackgroundColor(viewId: Int, color: Int) {
            views.setInt(viewId, "setBackgroundColor", color)
        }
        
        /**
         * تعيين شريط التقدم / Set progress bar
         */
        fun setProgressBar(viewId: Int, max: Int, progress: Int, indeterminate: Boolean = false) {
            views.setProgressBar(viewId, max, progress, indeterminate)
        }
        
        /**
         * تعيين حجم النص / Set text size
         */
        fun setTextSize(viewId: Int, unit: Int, size: Float) {
            views.setTextViewTextSize(viewId, unit, size)
        }
        
        fun getRemoteViews(): RemoteViews = views
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الويدجت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معرفات الويدجت / Get widget IDs
     */
    fun getWidgetIds(providerClass: Class<*>): IntArray {
        val ctx = context ?: return intArrayOf()
        val manager = AppWidgetManager.getInstance(ctx)
        val componentName = ComponentName(ctx, providerClass)
        return manager.getAppWidgetIds(componentName)
    }
    
    /**
     * عدد الويدجت / Widget count
     */
    fun getWidgetCount(providerClass: Class<*>): Int {
        return getWidgetIds(providerClass).size
    }
    
    /**
     * هل يوجد ويدجت؟ / Has widgets?
     */
    fun hasWidgets(providerClass: Class<*>): Boolean {
        return getWidgetIds(providerClass).isNotEmpty()
    }
    
    /**
     * جميع الويدجت النشطة / All active widgets
     */
    fun getAllActiveWidgetIds(): List<Int> {
        return widgetStates.keys.toList()
    }
    
    private fun notifyListeners(widgetId: Int, event: LifecycleEvent, data: Map<String, Any>?) {
        mainHandler.post {
            lifecycleListeners.forEach { it.onWidgetEvent(widgetId, event, data) }
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        widgetConfigs.clear()
        widgetStates.clear()
        lifecycleListeners.clear()
        context = null
    }
}
