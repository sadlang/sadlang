package sad.examples.tasks.workers

import android.app.NotificationManager
import android.content.Context
import androidx.core.app.NotificationCompat
import androidx.work.Worker
import androidx.work.WorkerParameters
import sad.examples.tasks.R
import sad.examples.tasks.SadTasksApplication

/**
 * Worker للتذكيرات في الخلفية
 * Background reminder worker - implements Sad جدول_إشعارات
 */
class ReminderWorker(
    context: Context,
    params: WorkerParameters
) : Worker(context, params) {
    
    override fun doWork(): Result {
        val title = inputData.getString("title") ?: "مهامي"
        val message = inputData.getString("message") ?: "لديك مهام تنتظرك!"
        
        showNotification(title, message)
        
        return Result.success()
    }
    
    private fun showNotification(title: String, message: String) {
        val notification = NotificationCompat.Builder(
            applicationContext,
            SadTasksApplication.REMINDER_CHANNEL_ID
        )
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle(title)
            .setContentText(message)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .build()
        
        val notificationManager = applicationContext
            .getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
        
        notificationManager.notify(NOTIFICATION_ID, notification)
    }
    
    companion object {
        private const val NOTIFICATION_ID = 1001
    }
}
