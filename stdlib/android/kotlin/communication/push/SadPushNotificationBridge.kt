package sad.android.communication.push

import android.Manifest
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.os.Build
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat
import androidx.core.content.ContextCompat

/**
 * جسر الإشعارات الفورية
 * Push Notifications Bridge - إشعارات FCM
 * 
 * @author فريق لغة ص
 */
object SadPushNotificationBridge {
    
    private const val DEFAULT_CHANNEL_ID = "default_channel"
    private const val DEFAULT_CHANNEL_NAME = "الإشعارات العامة"
    
    /**
     * بيانات الإشعار / Notification data
     */
    data class NotificationData(
        val id: Int,
        val title: String,
        val body: String,
        val channelId: String = DEFAULT_CHANNEL_ID,
        val smallIcon: Int = android.R.drawable.ic_notification,
        val largeIcon: Bitmap? = null,
        val autoCancel: Boolean = true,
        val priority: Int = NotificationCompat.PRIORITY_DEFAULT,
        val vibrate: Boolean = true,
        val sound: Boolean = true,
        val data: Map<String, String> = emptyMap()
    )
    
    /**
     * رسالة Push / Push message
     */
    data class PushMessage(
        val title: String?,
        val body: String?,
        val data: Map<String, String>,
        val imageUrl: String? = null,
        val clickAction: String? = null
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         إعداد القنوات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء قناة افتراضية / Create default channel
     */
    fun createDefaultChannel(context: Context) {
        createChannel(
            context,
            DEFAULT_CHANNEL_ID,
            DEFAULT_CHANNEL_NAME,
            NotificationManager.IMPORTANCE_DEFAULT
        )
    }
    
    /**
     * إنشاء قناة إشعارات / Create notification channel
     */
    fun createChannel(
        context: Context,
        channelId: String,
        channelName: String,
        importance: Int = NotificationManager.IMPORTANCE_DEFAULT,
        description: String = ""
    ) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(channelId, channelName, importance).apply {
                this.description = description
            }
            
            val notificationManager = context.getSystemService(NotificationManager::class.java)
            notificationManager.createNotificationChannel(channel)
        }
    }
    
    /**
     * إنشاء قناة عاجلة / Create urgent channel
     */
    fun createUrgentChannel(context: Context, channelId: String, channelName: String) {
        createChannel(context, channelId, channelName, NotificationManager.IMPORTANCE_HIGH)
    }
    
    /**
     * إنشاء قناة صامتة / Create silent channel
     */
    fun createSilentChannel(context: Context, channelId: String, channelName: String) {
        createChannel(context, channelId, channelName, NotificationManager.IMPORTANCE_LOW)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         عرض الإشعارات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عرض إشعار بسيط / Show simple notification
     */
    fun showSimple(
        context: Context,
        id: Int,
        title: String,
        body: String
    ): Boolean {
        return show(context, NotificationData(id, title, body))
    }
    
    /**
     * عرض إشعار / Show notification
     */
    fun show(context: Context, data: NotificationData): Boolean {
        if (!hasPermission(context)) return false
        
        val builder = NotificationCompat.Builder(context, data.channelId)
            .setSmallIcon(data.smallIcon)
            .setContentTitle(data.title)
            .setContentText(data.body)
            .setPriority(data.priority)
            .setAutoCancel(data.autoCancel)
        
        data.largeIcon?.let { builder.setLargeIcon(it) }
        
        if (!data.vibrate) {
            builder.setVibrate(longArrayOf(0))
        }
        
        if (!data.sound) {
            builder.setSound(null)
        }
        
        return try {
            NotificationManagerCompat.from(context).notify(data.id, builder.build())
            true
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * عرض إشعار مع صورة كبيرة / Show big picture notification
     */
    fun showBigPicture(
        context: Context,
        id: Int,
        title: String,
        body: String,
        bigPicture: Bitmap,
        channelId: String = DEFAULT_CHANNEL_ID
    ): Boolean {
        if (!hasPermission(context)) return false
        
        val style = NotificationCompat.BigPictureStyle()
            .bigPicture(bigPicture)
            .bigLargeIcon(null as Bitmap?)
        
        val notification = NotificationCompat.Builder(context, channelId)
            .setSmallIcon(android.R.drawable.ic_notification)
            .setContentTitle(title)
            .setContentText(body)
            .setStyle(style)
            .setAutoCancel(true)
            .build()
        
        return try {
            NotificationManagerCompat.from(context).notify(id, notification)
            true
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * عرض إشعار مع نص طويل / Show big text notification
     */
    fun showBigText(
        context: Context,
        id: Int,
        title: String,
        bigText: String,
        channelId: String = DEFAULT_CHANNEL_ID
    ): Boolean {
        if (!hasPermission(context)) return false
        
        val style = NotificationCompat.BigTextStyle()
            .bigText(bigText)
        
        val notification = NotificationCompat.Builder(context, channelId)
            .setSmallIcon(android.R.drawable.ic_notification)
            .setContentTitle(title)
            .setStyle(style)
            .setAutoCancel(true)
            .build()
        
        return try {
            NotificationManagerCompat.from(context).notify(id, notification)
            true
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * عرض إشعار مع أزرار / Show notification with actions
     */
    fun showWithActions(
        context: Context,
        id: Int,
        title: String,
        body: String,
        actions: List<Pair<String, PendingIntent>>,
        channelId: String = DEFAULT_CHANNEL_ID
    ): Boolean {
        if (!hasPermission(context)) return false
        
        val builder = NotificationCompat.Builder(context, channelId)
            .setSmallIcon(android.R.drawable.ic_notification)
            .setContentTitle(title)
            .setContentText(body)
            .setAutoCancel(true)
        
        actions.forEach { (actionTitle, pendingIntent) ->
            builder.addAction(0, actionTitle, pendingIntent)
        }
        
        return try {
            NotificationManagerCompat.from(context).notify(id, builder.build())
            true
        } catch (e: SecurityException) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الإشعارات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء إشعار / Cancel notification
     */
    fun cancel(context: Context, id: Int) {
        NotificationManagerCompat.from(context).cancel(id)
    }
    
    /**
     * إلغاء كل الإشعارات / Cancel all notifications
     */
    fun cancelAll(context: Context) {
        NotificationManagerCompat.from(context).cancelAll()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * التحقق من الإذن / Check permission
     */
    fun hasPermission(context: Context): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.POST_NOTIFICATIONS
            ) == PackageManager.PERMISSION_GRANTED
        } else {
            NotificationManagerCompat.from(context).areNotificationsEnabled()
        }
    }
    
    /**
     * الأذونات المطلوبة / Required permissions
     */
    fun requiredPermissions(): Array<String> {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            arrayOf(Manifest.permission.POST_NOTIFICATIONS)
        } else {
            emptyArray()
        }
    }
}
