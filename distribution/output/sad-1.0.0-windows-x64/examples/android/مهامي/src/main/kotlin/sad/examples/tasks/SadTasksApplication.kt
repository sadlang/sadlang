package sad.examples.tasks

import android.app.Application
import android.app.NotificationChannel
import android.app.NotificationManager
import android.os.Build

/**
 * تطبيق مهامي - نقطة دخول لغة ص
 * Sad Tasks Application - Entry point for Sad language
 */
class SadTasksApplication : Application() {
    
    override fun onCreate() {
        super.onCreate()
        
        // إنشاء قناة الإشعارات
        createNotificationChannel()
    }
    
    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                REMINDER_CHANNEL_ID,
                "تذكيرات",  // Reminders
                NotificationManager.IMPORTANCE_DEFAULT
            ).apply {
                description = "إشعارات تذكير المهام"  // Task reminder notifications
            }
            
            val notificationManager = getSystemService(NotificationManager::class.java)
            notificationManager.createNotificationChannel(channel)
        }
    }
    
    companion object {
        const val REMINDER_CHANNEL_ID = "reminders"
    }
}
