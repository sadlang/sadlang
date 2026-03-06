package sad.android.communication.alarm

import android.app.AlarmManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.os.Build
import android.provider.Settings
import java.util.Calendar

/**
 * جسر المنبه - جدولة المنبهات والتذكيرات
 * Alarm Bridge - Alarm and reminder scheduling
 * 
 * @author فريق لغة ص
 */
object SadAlarmBridge {
    
    /**
     * نوع المنبه / Alarm type
     */
    enum class AlarmType {
        EXACT,              // دقيق
        INEXACT,            // غير دقيق
        EXACT_ALLOW_IDLE,   // دقيق حتى في وضع الخمول
        REPEATING           // متكرر
    }
    
    /**
     * طلب منبه / Alarm request
     */
    data class AlarmRequest(
        val id: Int,
        val type: AlarmType = AlarmType.EXACT,
        val triggerTime: Long,
        val intervalMillis: Long = 0,  // للمنبهات المتكررة
        val action: String,
        val extras: Map<String, Any?> = emptyMap(),
        val wakeup: Boolean = true
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         جدولة المنبه
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جدولة منبه / Schedule alarm
     */
    fun scheduleAlarm(context: Context, request: AlarmRequest): Boolean {
        val alarmManager = context.getSystemService(Context.ALARM_SERVICE) as AlarmManager
        val pendingIntent = createPendingIntent(context, request)
        
        val alarmType = if (request.wakeup) {
            AlarmManager.RTC_WAKEUP
        } else {
            AlarmManager.RTC
        }
        
        return try {
            when (request.type) {
                AlarmType.EXACT -> {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                        alarmManager.setExactAndAllowWhileIdle(alarmType, request.triggerTime, pendingIntent)
                    } else {
                        alarmManager.setExact(alarmType, request.triggerTime, pendingIntent)
                    }
                }
                AlarmType.INEXACT -> {
                    alarmManager.set(alarmType, request.triggerTime, pendingIntent)
                }
                AlarmType.EXACT_ALLOW_IDLE -> {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                        alarmManager.setExactAndAllowWhileIdle(alarmType, request.triggerTime, pendingIntent)
                    } else {
                        alarmManager.setExact(alarmType, request.triggerTime, pendingIntent)
                    }
                }
                AlarmType.REPEATING -> {
                    alarmManager.setRepeating(
                        alarmType,
                        request.triggerTime,
                        request.intervalMillis,
                        pendingIntent
                    )
                }
            }
            true
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * جدولة منبه بسيط / Schedule simple alarm
     */
    fun scheduleAt(
        context: Context,
        id: Int,
        triggerTime: Long,
        action: String,
        extras: Map<String, Any?> = emptyMap()
    ): Boolean {
        return scheduleAlarm(context, AlarmRequest(
            id = id,
            triggerTime = triggerTime,
            action = action,
            extras = extras
        ))
    }
    
    /**
     * جدولة بعد مدة / Schedule after delay
     */
    fun scheduleAfter(
        context: Context,
        id: Int,
        delayMillis: Long,
        action: String,
        extras: Map<String, Any?> = emptyMap()
    ): Boolean {
        return scheduleAt(context, id, System.currentTimeMillis() + delayMillis, action, extras)
    }
    
    /**
     * جدولة في وقت محدد / Schedule at time
     */
    fun scheduleAtTime(
        context: Context,
        id: Int,
        hour: Int,
        minute: Int,
        action: String,
        extras: Map<String, Any?> = emptyMap()
    ): Boolean {
        val calendar = Calendar.getInstance().apply {
            set(Calendar.HOUR_OF_DAY, hour)
            set(Calendar.MINUTE, minute)
            set(Calendar.SECOND, 0)
            set(Calendar.MILLISECOND, 0)
            
            // إذا الوقت مر اليوم، جدوله للغد
            if (timeInMillis <= System.currentTimeMillis()) {
                add(Calendar.DAY_OF_YEAR, 1)
            }
        }
        
        return scheduleAt(context, id, calendar.timeInMillis, action, extras)
    }
    
    /**
     * جدولة منبه متكرر / Schedule repeating alarm
     */
    fun scheduleRepeating(
        context: Context,
        id: Int,
        firstTrigger: Long,
        intervalMillis: Long,
        action: String,
        extras: Map<String, Any?> = emptyMap()
    ): Boolean {
        return scheduleAlarm(context, AlarmRequest(
            id = id,
            type = AlarmType.REPEATING,
            triggerTime = firstTrigger,
            intervalMillis = intervalMillis,
            action = action,
            extras = extras
        ))
    }
    
    /**
     * جدولة منبه يومي / Schedule daily alarm
     */
    fun scheduleDaily(
        context: Context,
        id: Int,
        hour: Int,
        minute: Int,
        action: String,
        extras: Map<String, Any?> = emptyMap()
    ): Boolean {
        val calendar = Calendar.getInstance().apply {
            set(Calendar.HOUR_OF_DAY, hour)
            set(Calendar.MINUTE, minute)
            set(Calendar.SECOND, 0)
            
            if (timeInMillis <= System.currentTimeMillis()) {
                add(Calendar.DAY_OF_YEAR, 1)
            }
        }
        
        return scheduleRepeating(
            context,
            id,
            calendar.timeInMillis,
            AlarmManager.INTERVAL_DAY,
            action,
            extras
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإلغاء
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء منبه / Cancel alarm
     */
    fun cancelAlarm(context: Context, id: Int, action: String): Boolean {
        val alarmManager = context.getSystemService(Context.ALARM_SERVICE) as AlarmManager
        val intent = Intent(action)
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        val pendingIntent = PendingIntent.getBroadcast(context, id, intent, flags)
        
        return try {
            alarmManager.cancel(pendingIntent)
            pendingIntent.cancel()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * هل المنبه موجود؟ / Is alarm scheduled?
     */
    fun isAlarmScheduled(context: Context, id: Int, action: String): Boolean {
        val intent = Intent(action)
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_NO_CREATE or PendingIntent.FLAG_IMMUTABLE
        } else {
            PendingIntent.FLAG_NO_CREATE
        }
        
        val pendingIntent = PendingIntent.getBroadcast(context, id, intent, flags)
        return pendingIntent != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل يمكن جدولة منبهات دقيقة؟ / Can schedule exact alarms?
     */
    fun canScheduleExactAlarms(context: Context): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            val alarmManager = context.getSystemService(Context.ALARM_SERVICE) as AlarmManager
            alarmManager.canScheduleExactAlarms()
        } else {
            true
        }
    }
    
    /**
     * فتح إعدادات المنبهات / Open alarm settings
     */
    fun openAlarmSettings(context: Context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            val intent = Intent(Settings.ACTION_REQUEST_SCHEDULE_EXACT_ALARM)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فواصل زمنية شائعة
    // ═══════════════════════════════════════════════════════════════
    
    object Intervals {
        const val MINUTE = 60 * 1000L
        const val FIFTEEN_MINUTES = AlarmManager.INTERVAL_FIFTEEN_MINUTES
        const val HALF_HOUR = AlarmManager.INTERVAL_HALF_HOUR
        const val HOUR = AlarmManager.INTERVAL_HOUR
        const val HALF_DAY = AlarmManager.INTERVAL_HALF_DAY
        const val DAY = AlarmManager.INTERVAL_DAY
        const val WEEK = 7 * DAY
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun createPendingIntent(context: Context, request: AlarmRequest): PendingIntent {
        val intent = Intent(request.action).apply {
            request.extras.forEach { (key, value) ->
                when (value) {
                    is String -> putExtra(key, value)
                    is Int -> putExtra(key, value)
                    is Long -> putExtra(key, value)
                    is Boolean -> putExtra(key, value)
                    is Float -> putExtra(key, value)
                    is Double -> putExtra(key, value)
                }
            }
        }
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        return PendingIntent.getBroadcast(context, request.id, intent, flags)
    }
}
