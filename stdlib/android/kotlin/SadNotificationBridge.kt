// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadNotificationBridge.kt
// الوصف: جسر Kotlin لدوال الإشعارات في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_اشعار_* بـ Android NotificationManager
//
// الدوال المُنفّذة:
//   __اندرويد_اشعار_ارسل(title, message, icon?) -> bool
//   __اندرويد_اشعار_انشئ_قناة(id, name, importance) -> bool
//   __اندرويد_اشعار_الغ(id) -> bool
//
// المتطلبات:
//   - compileSdk 33+ (Tiramisu للإذن)
//   - إذن POST_NOTIFICATIONS في AndroidManifest.xml
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat

/**
 * جسر الإشعارات — SadNotificationBridge
 * 
 * يوفر واجهة لإرسال الإشعارات من كود لغة ص.
 */
object SadNotificationBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ثوابت
    // ═══════════════════════════════════════════════════════════════════════════
    
    private const val DEFAULT_CHANNEL_ID = "sad_default_channel"
    private const val DEFAULT_CHANNEL_NAME = "الإشعارات الافتراضية"
    
    private var notificationIdCounter = 0
    
    // متغير للسياق (يُضبط من MainActivity)
    private var appContext: Context? = null
    
    /**
     * تهيئة الجسر مع السياق
     * يجب استدعاؤها من onCreate في MainActivity
     */
    @JvmStatic
    fun initialize(context: Context) {
        appContext = context.applicationContext
        createDefaultChannel()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اشعار_انشئ_قناة — إنشاء قناة إشعارات
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء قناة إشعارات جديدة (مطلوب في Android 8+)
     * 
     * @param channelId معرّف القناة
     * @param channelName اسم القناة للمستخدم
     * @param importance مستوى الأهمية (0-4)
     *                   0 = لا شيء، 1 = منخفض، 2 = افتراضي، 3 = مرتفع، 4 = عاجل
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun createChannel(
        channelId: String,
        channelName: String,
        importance: Int = 3
    ): Boolean {
        val context = appContext ?: return false
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val androidImportance = when (importance) {
                0 -> NotificationManager.IMPORTANCE_NONE
                1 -> NotificationManager.IMPORTANCE_LOW
                2 -> NotificationManager.IMPORTANCE_DEFAULT
                3 -> NotificationManager.IMPORTANCE_HIGH
                else -> NotificationManager.IMPORTANCE_MAX
            }
            
            val channel = NotificationChannel(
                channelId,
                channelName,
                androidImportance
            ).apply {
                description = "قناة من تطبيق لغة ص"
                enableVibration(importance >= 3)
                enableLights(importance >= 2)
            }
            
            val notificationManager = context.getSystemService(NotificationManager::class.java)
            notificationManager.createNotificationChannel(channel)
            return true
        }
        
        return true // في الإصدارات القديمة لا حاجة للقنوات
    }
    
    /**
     * إنشاء القناة الافتراضية
     */
    private fun createDefaultChannel() {
        createChannel(DEFAULT_CHANNEL_ID, DEFAULT_CHANNEL_NAME, 3)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اشعار_ارسل — إرسال إشعار
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إرسال إشعار للمستخدم
     * 
     * @param title عنوان الإشعار
     * @param message نص الإشعار
     * @param channelId معرف القناة (افتراضي: sad_default_channel)
     * @param iconResId معرف أيقونة (اختياري)
     * @return معرف الإشعار المُرسل، أو -1 عند الفشل
     */
    @JvmStatic
    fun sendNotification(
        title: String,
        message: String,
        channelId: String = DEFAULT_CHANNEL_ID,
        iconResId: Int? = null
    ): Int {
        val context = appContext ?: return -1
        
        // التحقق من الإذن في Android 13+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            if (ActivityCompat.checkSelfPermission(
                    context,
                    Manifest.permission.POST_NOTIFICATIONS
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                return -1
            }
        }
        
        val notificationId = ++notificationIdCounter
        
        // أيقونة افتراضية (يجب أن تكون موجودة في res/drawable)
        val icon = iconResId ?: android.R.drawable.ic_dialog_info
        
        val builder = NotificationCompat.Builder(context, channelId)
            .setSmallIcon(icon)
            .setContentTitle(title)
            .setContentText(message)
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true) // إزالة عند النقر
        
        // إذا كان النص طويلاً، استخدم BigTextStyle
        if (message.length > 50) {
            builder.setStyle(
                NotificationCompat.BigTextStyle()
                    .bigText(message)
            )
        }
        
        NotificationManagerCompat.from(context).notify(notificationId, builder.build())
        
        return notificationId
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_اشعار_الغ — إلغاء إشعار
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إلغاء إشعار معين
     * 
     * @param notificationId معرف الإشعار
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun cancelNotification(notificationId: Int): Boolean {
        val context = appContext ?: return false
        NotificationManagerCompat.from(context).cancel(notificationId)
        return true
    }
    
    /**
     * إلغاء جميع الإشعارات
     * 
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun cancelAllNotifications(): Boolean {
        val context = appContext ?: return false
        NotificationManagerCompat.from(context).cancelAll()
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من وجود إذن الإشعارات
     * 
     * @return true إذا الإذن ممنوح
     */
    @JvmStatic
    fun hasNotificationPermission(): Boolean {
        val context = appContext ?: return false
        
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            ActivityCompat.checkSelfPermission(
                context,
                Manifest.permission.POST_NOTIFICATIONS
            ) == PackageManager.PERMISSION_GRANTED
        } else {
            true // الإصدارات القديمة لا تحتاج إذن
        }
    }
}
