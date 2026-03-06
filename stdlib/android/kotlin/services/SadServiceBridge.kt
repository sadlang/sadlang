// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadServiceBridge.kt
// الوصف: جسر Kotlin للخدمات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat

/**
 * جسر الخدمات — SadServiceBridge
 */
object SadServiceBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun startService(serviceClass: Class<out Service>, extras: Map<String, Any>? = null) {
        val ctx = context ?: return
        
        val intent = Intent(ctx, serviceClass)
        extras?.forEach { (key, value) ->
            when (value) {
                is String -> intent.putExtra(key, value)
                is Int -> intent.putExtra(key, value)
                is Long -> intent.putExtra(key, value)
                is Boolean -> intent.putExtra(key, value)
                is Float -> intent.putExtra(key, value)
                is Double -> intent.putExtra(key, value)
            }
        }
        
        ctx.startService(intent)
    }
    
    @JvmStatic
    fun startForegroundService(serviceClass: Class<out Service>, extras: Map<String, Any>? = null) {
        val ctx = context ?: return
        
        val intent = Intent(ctx, serviceClass)
        extras?.forEach { (key, value) ->
            when (value) {
                is String -> intent.putExtra(key, value)
                is Int -> intent.putExtra(key, value)
                is Long -> intent.putExtra(key, value)
                is Boolean -> intent.putExtra(key, value)
                is Float -> intent.putExtra(key, value)
                is Double -> intent.putExtra(key, value)
            }
        }
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            ctx.startForegroundService(intent)
        } else {
            ctx.startService(intent)
        }
    }
    
    @JvmStatic
    fun stopService(serviceClass: Class<out Service>) {
        val ctx = context ?: return
        ctx.stopService(Intent(ctx, serviceClass))
    }
    
    /**
     * كلاس أساسي للخدمات الأمامية
     */
    abstract class SadForegroundService : Service() {
        
        companion object {
            const val CHANNEL_ID = "sad_foreground_channel"
            const val NOTIFICATION_ID = 1001
        }
        
        override fun onCreate() {
            super.onCreate()
            createNotificationChannel()
        }
        
        override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
            val notification = createNotification()
            startForeground(NOTIFICATION_ID, notification)
            onServiceStarted(intent)
            return START_STICKY
        }
        
        override fun onBind(intent: Intent?): IBinder? = null
        
        private fun createNotificationChannel() {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                val channel = NotificationChannel(
                    CHANNEL_ID,
                    getChannelName(),
                    NotificationManager.IMPORTANCE_LOW
                ).apply {
                    description = getChannelDescription()
                }
                
                val manager = getSystemService(NotificationManager::class.java)
                manager.createNotificationChannel(channel)
            }
        }
        
        private fun createNotification(): Notification {
            val pendingIntent = createPendingIntent()
            
            return NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle(getNotificationTitle())
                .setContentText(getNotificationText())
                .setSmallIcon(getNotificationIcon())
                .setContentIntent(pendingIntent)
                .setOngoing(true)
                .build()
        }
        
        protected fun updateNotification(title: String, text: String) {
            val notification = NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle(title)
                .setContentText(text)
                .setSmallIcon(getNotificationIcon())
                .setOngoing(true)
                .build()
            
            val manager = getSystemService(NotificationManager::class.java)
            manager.notify(NOTIFICATION_ID, notification)
        }
        
        // يجب تنفيذها في الكلاس الفرعي
        abstract fun getNotificationTitle(): String
        abstract fun getNotificationText(): String
        abstract fun getNotificationIcon(): Int
        abstract fun getChannelName(): String
        abstract fun getChannelDescription(): String
        abstract fun createPendingIntent(): PendingIntent?
        abstract fun onServiceStarted(intent: Intent?)
    }
}
