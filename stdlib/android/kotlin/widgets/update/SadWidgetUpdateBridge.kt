package sad.android.widgets.update

import android.appwidget.AppWidgetManager
import android.content.ComponentName
import android.content.Context
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import androidx.work.*
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.TimeUnit

/**
 * جسر تحديث الويدجت - جدولة وإدارة تحديثات الويدجت
 * Widget Update Bridge - Scheduling and managing widget updates
 * 
 * @author فريق لغة ص
 */
object SadWidgetUpdateBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val updateCallbacks = ConcurrentHashMap<String, WidgetUpdateCallback>()
    private val scheduledUpdates = ConcurrentHashMap<String, UpdateSchedule>()
    
    /**
     * جدول التحديث / Update schedule
     */
    data class UpdateSchedule(
        val widgetKey: String,
        val intervalMinutes: Long,
        val workName: String,
        var isActive: Boolean = true
    )
    
    /**
     * قيود التحديث / Update constraints
     */
    data class UpdateConstraints(
        val requiresNetwork: Boolean = false,
        val requiresCharging: Boolean = false,
        val requiresBatteryNotLow: Boolean = false,
        val requiresStorageNotLow: Boolean = false,
        val requiresDeviceIdle: Boolean = false
    )
    
    /**
     * رد نداء التحديث / Update callback
     */
    interface WidgetUpdateCallback {
        fun onUpdateRequired(widgetIds: IntArray): RemoteViews?
    }
    
    /**
     * مستمع التحديث / Update listener
     */
    interface UpdateListener {
        fun onUpdateStarted(widgetKey: String)
        fun onUpdateCompleted(widgetKey: String, success: Boolean)
        fun onUpdateFailed(widgetKey: String, error: String)
    }
    
    private val updateListeners = mutableListOf<UpdateListener>()
    
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
    fun registerUpdateCallback(widgetKey: String, callback: WidgetUpdateCallback): Boolean {
        updateCallbacks[widgetKey] = callback
        return true
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addUpdateListener(listener: UpdateListener) {
        if (!updateListeners.contains(listener)) {
            updateListeners.add(listener)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحديث فوري
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحديث ويدجت واحد / Update single widget
     */
    fun updateWidget(widgetId: Int, layoutResId: Int, builder: (RemoteViews) -> Unit): Boolean {
        val ctx = context ?: return false
        
        val views = RemoteViews(ctx.packageName, layoutResId)
        builder(views)
        
        val manager = AppWidgetManager.getInstance(ctx)
        manager.updateAppWidget(widgetId, views)
        
        return true
    }
    
    /**
     * تحديث جميع الويدجت من نوع / Update all widgets of type
     */
    fun updateAllWidgets(
        providerClass: Class<*>,
        layoutResId: Int,
        builder: (RemoteViews, Int) -> Unit
    ): Int {
        val ctx = context ?: return 0
        
        val manager = AppWidgetManager.getInstance(ctx)
        val componentName = ComponentName(ctx, providerClass)
        val widgetIds = manager.getAppWidgetIds(componentName)
        
        widgetIds.forEach { id ->
            val views = RemoteViews(ctx.packageName, layoutResId)
            builder(views, id)
            manager.updateAppWidget(id, views)
        }
        
        return widgetIds.size
    }
    
    /**
     * تحديث جزئي / Partial update
     */
    fun partialUpdate(widgetId: Int, layoutResId: Int, builder: (RemoteViews) -> Unit): Boolean {
        val ctx = context ?: return false
        
        val views = RemoteViews(ctx.packageName, layoutResId)
        builder(views)
        
        val manager = AppWidgetManager.getInstance(ctx)
        manager.partiallyUpdateAppWidget(widgetId, views)
        
        return true
    }
    
    /**
     * إعلام تغيير البيانات / Notify data changed (للقوائم)
     */
    fun notifyDataChanged(widgetId: Int, viewId: Int): Boolean {
        val ctx = context ?: return false
        
        val manager = AppWidgetManager.getInstance(ctx)
        manager.notifyAppWidgetViewDataChanged(widgetId, viewId)
        
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         جدولة التحديث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جدولة تحديث دوري / Schedule periodic update
     */
    fun schedulePeriodicUpdate(
        widgetKey: String,
        intervalMinutes: Long,
        constraints: UpdateConstraints = UpdateConstraints()
    ): Boolean {
        val ctx = context ?: return false
        
        val workConstraints = Constraints.Builder().apply {
            if (constraints.requiresNetwork) {
                setRequiredNetworkType(NetworkType.CONNECTED)
            }
            setRequiresCharging(constraints.requiresCharging)
            setRequiresBatteryNotLow(constraints.requiresBatteryNotLow)
            setRequiresStorageNotLow(constraints.requiresStorageNotLow)
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
                setRequiresDeviceIdle(constraints.requiresDeviceIdle)
            }
        }.build()
        
        val workName = "widget_update_$widgetKey"
        
        val workRequest = PeriodicWorkRequestBuilder<WidgetUpdateWorker>(
            intervalMinutes, TimeUnit.MINUTES
        )
            .setConstraints(workConstraints)
            .setInputData(workDataOf("widget_key" to widgetKey))
            .addTag(widgetKey)
            .build()
        
        WorkManager.getInstance(ctx).enqueueUniquePeriodicWork(
            workName,
            ExistingPeriodicWorkPolicy.REPLACE,
            workRequest
        )
        
        scheduledUpdates[widgetKey] = UpdateSchedule(
            widgetKey = widgetKey,
            intervalMinutes = intervalMinutes,
            workName = workName
        )
        
        return true
    }
    
    /**
     * جدولة تحديث لمرة واحدة / Schedule one-time update
     */
    fun scheduleOneTimeUpdate(
        widgetKey: String,
        delayMinutes: Long = 0,
        constraints: UpdateConstraints = UpdateConstraints()
    ): Boolean {
        val ctx = context ?: return false
        
        val workConstraints = Constraints.Builder().apply {
            if (constraints.requiresNetwork) {
                setRequiredNetworkType(NetworkType.CONNECTED)
            }
            setRequiresCharging(constraints.requiresCharging)
            setRequiresBatteryNotLow(constraints.requiresBatteryNotLow)
        }.build()
        
        val workRequest = OneTimeWorkRequestBuilder<WidgetUpdateWorker>()
            .setConstraints(workConstraints)
            .setInitialDelay(delayMinutes, TimeUnit.MINUTES)
            .setInputData(workDataOf("widget_key" to widgetKey))
            .addTag(widgetKey)
            .build()
        
        WorkManager.getInstance(ctx).enqueue(workRequest)
        
        return true
    }
    
    /**
     * إلغاء الجدولة / Cancel schedule
     */
    fun cancelScheduledUpdate(widgetKey: String): Boolean {
        val ctx = context ?: return false
        
        val schedule = scheduledUpdates.remove(widgetKey)
        if (schedule != null) {
            WorkManager.getInstance(ctx).cancelUniqueWork(schedule.workName)
            return true
        }
        
        // حاول الإلغاء بالتاج
        WorkManager.getInstance(ctx).cancelAllWorkByTag(widgetKey)
        return true
    }
    
    /**
     * إلغاء جميع الجدولات / Cancel all schedules
     */
    fun cancelAllScheduledUpdates(): Boolean {
        val ctx = context ?: return false
        
        scheduledUpdates.keys.forEach { key ->
            WorkManager.getInstance(ctx).cancelAllWorkByTag(key)
        }
        scheduledUpdates.clear()
        
        return true
    }
    
    /**
     * الحصول على الجدولات النشطة / Get active schedules
     */
    fun getActiveSchedules(): List<UpdateSchedule> {
        return scheduledUpdates.values.filter { it.isActive }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         Worker
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عامل تحديث الويدجت / Widget update worker
     */
    class WidgetUpdateWorker(
        ctx: Context,
        params: WorkerParameters
    ) : Worker(ctx, params) {
        
        override fun doWork(): Result {
            val widgetKey = inputData.getString("widget_key") ?: return Result.failure()
            
            // إشعار البدء
            notifyUpdateStarted(widgetKey)
            
            try {
                val callback = updateCallbacks[widgetKey]
                if (callback != null) {
                    val ctx = applicationContext
                    val manager = AppWidgetManager.getInstance(ctx)
                    
                    // البحث عن widgetIds المرتبطة
                    // هنا يجب تمرير providerClass - يمكن تخزينها في البيانات
                    
                    notifyUpdateCompleted(widgetKey, true)
                    return Result.success()
                }
                
                notifyUpdateFailed(widgetKey, "لا يوجد callback مسجل")
                return Result.failure()
                
            } catch (e: Exception) {
                notifyUpdateFailed(widgetKey, e.message ?: "خطأ غير معروف")
                return Result.retry()
            }
        }
        
        private fun notifyUpdateStarted(key: String) {
            Handler(Looper.getMainLooper()).post {
                updateListeners.forEach { it.onUpdateStarted(key) }
            }
        }
        
        private fun notifyUpdateCompleted(key: String, success: Boolean) {
            Handler(Looper.getMainLooper()).post {
                updateListeners.forEach { it.onUpdateCompleted(key, success) }
            }
        }
        
        private fun notifyUpdateFailed(key: String, error: String) {
            Handler(Looper.getMainLooper()).post {
                updateListeners.forEach { it.onUpdateFailed(key, error) }
            }
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        updateCallbacks.clear()
        scheduledUpdates.clear()
        updateListeners.clear()
        context = null
    }
}
