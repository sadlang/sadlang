/**
 * =============================================================================
 * @file SadNotifications.kt
 * @brief (AR) واجهة الإشعارات لأندرويد
 * @brief (EN) Android Notifications Interface
 * =============================================================================
 */

package sad.lang.android

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.graphics.BitmapFactory
import android.os.Build
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat

/**
 * نظام الإشعارات للغة ص
 */
class SadNotifications(private val context: Context) {
    
    companion object {
        private const val DEFAULT_CHANNEL_ID = "sad_default"
        private const val DEFAULT_CHANNEL_NAME = "إشعارات التطبيق"
        
        private var notificationId = 1
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نوع أهمية الإشعار
     */
    enum class Importance {
        LOW,
        DEFAULT,
        HIGH,
        URGENT
    }
    
    /**
     * بيانات الإشعار
     */
    data class NotificationData(
        val id: Int = notificationId++,
        val title: String,
        val body: String,
        val channelId: String = DEFAULT_CHANNEL_ID,
        val smallIconRes: Int = android.R.drawable.ic_dialog_info,
        val largeIconRes: Int? = null,
        val autoCancel: Boolean = true,
        val priority: Int = NotificationCompat.PRIORITY_DEFAULT,
        val category: String? = null,
        val groupKey: String? = null,
        val actionIntent: Intent? = null
    )
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Channel Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val notificationManager: NotificationManager by lazy {
        context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
    }
    
    /**
     * إنشاء قناة إشعارات
     */
    fun createChannel(
        channelId: String,
        channelName: String,
        description: String = "",
        importance: Importance = Importance.DEFAULT
    ) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val androidImportance = when (importance) {
                Importance.LOW -> NotificationManager.IMPORTANCE_LOW
                Importance.DEFAULT -> NotificationManager.IMPORTANCE_DEFAULT
                Importance.HIGH -> NotificationManager.IMPORTANCE_HIGH
                Importance.URGENT -> NotificationManager.IMPORTANCE_HIGH
            }
            
            val channel = NotificationChannel(channelId, channelName, androidImportance).apply {
                this.description = description
                enableLights(true)
                enableVibration(importance >= Importance.HIGH)
            }
            
            notificationManager.createNotificationChannel(channel)
        }
    }
    
    /**
     * إنشاء القناة الافتراضية
     */
    fun createDefaultChannel() {
        createChannel(DEFAULT_CHANNEL_ID, DEFAULT_CHANNEL_NAME)
    }
    
    /**
     * حذف قناة
     */
    fun deleteChannel(channelId: String) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            notificationManager.deleteNotificationChannel(channelId)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Show Notifications
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * عرض إشعار بسيط
     */
    fun show(title: String, body: String): Int {
        return show(NotificationData(title = title, body = body))
    }
    
    /**
     * عرض إشعار
     */
    fun show(data: NotificationData): Int {
        // Ensure channel exists
        createDefaultChannel()
        
        val builder = NotificationCompat.Builder(context, data.channelId)
            .setSmallIcon(data.smallIconRes)
            .setContentTitle(data.title)
            .setContentText(data.body)
            .setAutoCancel(data.autoCancel)
            .setPriority(data.priority)
        
        // Large icon
        data.largeIconRes?.let { iconRes ->
            val largeIcon = BitmapFactory.decodeResource(context.resources, iconRes)
            builder.setLargeIcon(largeIcon)
        }
        
        // Category
        data.category?.let { builder.setCategory(it) }
        
        // Group
        data.groupKey?.let { builder.setGroup(it) }
        
        // Action intent
        data.actionIntent?.let { intent ->
            val pendingIntent = PendingIntent.getActivity(
                context,
                data.id,
                intent,
                PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
            )
            builder.setContentIntent(pendingIntent)
        }
        
        with(NotificationManagerCompat.from(context)) {
            try {
                notify(data.id, builder.build())
            } catch (e: SecurityException) {
                // Permission not granted
            }
        }
        
        return data.id
    }
    
    /**
     * عرض إشعار مع تقدم
     */
    fun showProgress(
        title: String,
        body: String,
        progress: Int,
        max: Int = 100,
        indeterminate: Boolean = false
    ): Int {
        createDefaultChannel()
        
        val id = notificationId++
        
        val builder = NotificationCompat.Builder(context, DEFAULT_CHANNEL_ID)
            .setSmallIcon(android.R.drawable.ic_popup_sync)
            .setContentTitle(title)
            .setContentText(body)
            .setProgress(max, progress, indeterminate)
            .setOngoing(true)
        
        with(NotificationManagerCompat.from(context)) {
            try {
                notify(id, builder.build())
            } catch (e: SecurityException) {
                // Permission not granted
            }
        }
        
        return id
    }
    
    /**
     * تحديث إشعار التقدم
     */
    fun updateProgress(notificationId: Int, progress: Int, max: Int = 100) {
        val builder = NotificationCompat.Builder(context, DEFAULT_CHANNEL_ID)
            .setSmallIcon(android.R.drawable.ic_popup_sync)
            .setProgress(max, progress, false)
            .setOngoing(progress < max)
        
        with(NotificationManagerCompat.from(context)) {
            try {
                notify(notificationId, builder.build())
            } catch (e: SecurityException) {
                // Permission not granted
            }
        }
    }
    
    /**
     * عرض إشعار مع BigText
     */
    fun showBigText(title: String, body: String, bigText: String): Int {
        createDefaultChannel()
        
        val id = notificationId++
        
        val style = NotificationCompat.BigTextStyle()
            .bigText(bigText)
            .setBigContentTitle(title)
        
        val builder = NotificationCompat.Builder(context, DEFAULT_CHANNEL_ID)
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .setContentTitle(title)
            .setContentText(body)
            .setStyle(style)
            .setAutoCancel(true)
        
        with(NotificationManagerCompat.from(context)) {
            try {
                notify(id, builder.build())
            } catch (e: SecurityException) {}
        }
        
        return id
    }
    
    /**
     * عرض إشعار مع أزرار
     */
    fun showWithActions(
        title: String,
        body: String,
        actions: List<Pair<String, PendingIntent>>
    ): Int {
        createDefaultChannel()
        
        val id = notificationId++
        
        val builder = NotificationCompat.Builder(context, DEFAULT_CHANNEL_ID)
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .setContentTitle(title)
            .setContentText(body)
            .setAutoCancel(true)
        
        actions.forEach { (actionTitle, pendingIntent) ->
            builder.addAction(0, actionTitle, pendingIntent)
        }
        
        with(NotificationManagerCompat.from(context)) {
            try {
                notify(id, builder.build())
            } catch (e: SecurityException) {}
        }
        
        return id
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Cancel Notifications
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إلغاء إشعار
     */
    fun cancel(notificationId: Int) {
        notificationManager.cancel(notificationId)
    }
    
    /**
     * إلغاء جميع الإشعارات
     */
    fun cancelAll() {
        notificationManager.cancelAll()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Permissions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل الإشعارات مفعلة؟
     */
    fun areNotificationsEnabled(): Boolean {
        return NotificationManagerCompat.from(context).areNotificationsEnabled()
    }
}
