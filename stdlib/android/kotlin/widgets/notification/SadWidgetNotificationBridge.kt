package sad.android.widgets.notification

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.os.Build
import android.os.Handler
import android.os.Looper
import androidx.core.app.NotificationCompat
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر إشعارات الويدجت - إشعارات مرتبطة بالويدجت
 * Widget Notification Bridge - Widget-related notifications
 * 
 * @author فريق لغة ص
 */
object SadWidgetNotificationBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var notificationManager: NotificationManager? = null
    private val channels = ConcurrentHashMap<String, ChannelConfig>()
    private var notificationIdCounter = 1000
    
    /**
     * تكوين القناة / Channel config
     */
    data class ChannelConfig(
        val id: String,
        val name: String,
        val description: String = "",
        val importance: Importance = Importance.DEFAULT,
        val showBadge: Boolean = true,
        val enableVibration: Boolean = true,
        val enableLights: Boolean = true,
        val lightColor: Int = 0xFF0000FF.toInt()
    )
    
    /**
     * الأهمية / Importance
     */
    enum class Importance {
        MIN,
        LOW,
        DEFAULT,
        HIGH,
        MAX
    }
    
    /**
     * تكوين الإشعار / Notification config
     */
    data class NotificationConfig(
        val channelId: String,
        val title: String,
        val content: String,
        val smallIconResId: Int,
        val largeIcon: Bitmap? = null,
        val autoCancel: Boolean = true,
        val ongoing: Boolean = false,
        val priority: Priority = Priority.DEFAULT,
        val category: String? = null,
        val group: String? = null,
        val actions: List<NotificationAction> = emptyList(),
        val progress: Progress? = null,
        val style: NotificationStyle? = null,
        val clickIntent: PendingIntent? = null
    )
    
    /**
     * الأولوية / Priority
     */
    enum class Priority {
        MIN,
        LOW,
        DEFAULT,
        HIGH,
        MAX
    }
    
    /**
     * إجراء الإشعار / Notification action
     */
    data class NotificationAction(
        val iconResId: Int,
        val title: String,
        val pendingIntent: PendingIntent
    )
    
    /**
     * التقدم / Progress
     */
    data class Progress(
        val max: Int,
        val current: Int,
        val indeterminate: Boolean = false
    )
    
    /**
     * نمط الإشعار / Notification style
     */
    sealed class NotificationStyle {
        data class BigText(val bigText: String) : NotificationStyle()
        data class BigPicture(val picture: Bitmap, val contentTitle: String? = null) : NotificationStyle()
        data class Inbox(val lines: List<String>) : NotificationStyle()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        notificationManager = ctx.getSystemService(Context.NOTIFICATION_SERVICE) as? NotificationManager
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة القنوات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء قناة / Create channel
     */
    fun createChannel(config: ChannelConfig): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val importance = when (config.importance) {
                Importance.MIN -> NotificationManager.IMPORTANCE_MIN
                Importance.LOW -> NotificationManager.IMPORTANCE_LOW
                Importance.DEFAULT -> NotificationManager.IMPORTANCE_DEFAULT
                Importance.HIGH -> NotificationManager.IMPORTANCE_HIGH
                Importance.MAX -> NotificationManager.IMPORTANCE_MAX
            }
            
            val channel = NotificationChannel(config.id, config.name, importance).apply {
                description = config.description
                setShowBadge(config.showBadge)
                enableVibration(config.enableVibration)
                enableLights(config.enableLights)
                lightColor = config.lightColor
            }
            
            notificationManager?.createNotificationChannel(channel)
        }
        
        channels[config.id] = config
        return true
    }
    
    /**
     * إنشاء قناة افتراضية / Create default channel
     */
    fun createDefaultChannel(): Boolean {
        return createChannel(
            ChannelConfig(
                id = "sad_widget_default",
                name = "إشعارات الويدجت",
                description = "إشعارات عامة من الويدجت"
            )
        )
    }
    
    /**
     * حذف قناة / Delete channel
     */
    fun deleteChannel(channelId: String): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            notificationManager?.deleteNotificationChannel(channelId)
        }
        return channels.remove(channelId) != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال الإشعارات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال إشعار / Send notification
     */
    fun sendNotification(config: NotificationConfig): Int {
        val ctx = context ?: return -1
        
        val builder = NotificationCompat.Builder(ctx, config.channelId).apply {
            setContentTitle(config.title)
            setContentText(config.content)
            setSmallIcon(config.smallIconResId)
            setAutoCancel(config.autoCancel)
            setOngoing(config.ongoing)
            
            config.largeIcon?.let { setLargeIcon(it) }
            config.clickIntent?.let { setContentIntent(it) }
            config.category?.let { setCategory(it) }
            config.group?.let { setGroup(it) }
            
            priority = when (config.priority) {
                Priority.MIN -> NotificationCompat.PRIORITY_MIN
                Priority.LOW -> NotificationCompat.PRIORITY_LOW
                Priority.DEFAULT -> NotificationCompat.PRIORITY_DEFAULT
                Priority.HIGH -> NotificationCompat.PRIORITY_HIGH
                Priority.MAX -> NotificationCompat.PRIORITY_MAX
            }
            
            config.progress?.let { progress ->
                setProgress(progress.max, progress.current, progress.indeterminate)
            }
            
            config.actions.forEach { action ->
                addAction(action.iconResId, action.title, action.pendingIntent)
            }
            
            config.style?.let { style ->
                when (style) {
                    is NotificationStyle.BigText -> {
                        setStyle(NotificationCompat.BigTextStyle().bigText(style.bigText))
                    }
                    is NotificationStyle.BigPicture -> {
                        setStyle(
                            NotificationCompat.BigPictureStyle()
                                .bigPicture(style.picture)
                                .also { style.contentTitle?.let { title -> it.setBigContentTitle(title) } }
                        )
                    }
                    is NotificationStyle.Inbox -> {
                        val inboxStyle = NotificationCompat.InboxStyle()
                        style.lines.forEach { line -> inboxStyle.addLine(line) }
                        setStyle(inboxStyle)
                    }
                }
            }
        }
        
        val notificationId = notificationIdCounter++
        notificationManager?.notify(notificationId, builder.build())
        
        return notificationId
    }
    
    /**
     * إرسال إشعار بسيط / Send simple notification
     */
    fun sendSimpleNotification(
        title: String,
        content: String,
        smallIconResId: Int,
        channelId: String = "sad_widget_default"
    ): Int {
        // إنشاء القناة الافتراضية إذا لم تكن موجودة
        if (!channels.containsKey(channelId)) {
            createDefaultChannel()
        }
        
        return sendNotification(
            NotificationConfig(
                channelId = channelId,
                title = title,
                content = content,
                smallIconResId = smallIconResId
            )
        )
    }
    
    /**
     * تحديث إشعار / Update notification
     */
    fun updateNotification(notificationId: Int, config: NotificationConfig): Boolean {
        val ctx = context ?: return false
        
        val builder = NotificationCompat.Builder(ctx, config.channelId).apply {
            setContentTitle(config.title)
            setContentText(config.content)
            setSmallIcon(config.smallIconResId)
            setAutoCancel(config.autoCancel)
            setOngoing(config.ongoing)
            
            config.progress?.let { progress ->
                setProgress(progress.max, progress.current, progress.indeterminate)
            }
        }
        
        notificationManager?.notify(notificationId, builder.build())
        return true
    }
    
    /**
     * إلغاء إشعار / Cancel notification
     */
    fun cancelNotification(notificationId: Int): Boolean {
        notificationManager?.cancel(notificationId)
        return true
    }
    
    /**
     * إلغاء جميع الإشعارات / Cancel all notifications
     */
    fun cancelAllNotifications(): Boolean {
        notificationManager?.cancelAll()
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إشعارات الويدجت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إشعار تحديث الويدجت / Widget update notification
     */
    fun notifyWidgetUpdated(widgetId: Int, widgetName: String, iconResId: Int): Int {
        return sendSimpleNotification(
            title = "تم تحديث $widgetName",
            content = "تم تحديث الويدجت بنجاح",
            smallIconResId = iconResId
        )
    }
    
    /**
     * إشعار خطأ الويدجت / Widget error notification
     */
    fun notifyWidgetError(widgetId: Int, error: String, iconResId: Int): Int {
        return sendSimpleNotification(
            title = "خطأ في الويدجت",
            content = error,
            smallIconResId = iconResId
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        channels.clear()
        notificationManager = null
        context = null
    }
}
