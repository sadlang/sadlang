// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAlarmBridge.kt
// الوصف: جسر Kotlin للتنبيهات والمؤقتات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.AlarmManager
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.SystemClock
import java.util.*

/**
 * جسر التنبيهات — SadAlarmBridge
 */
object SadAlarmBridge {
    
    private var context: Context? = null
    private var alarmManager: AlarmManager? = null
    private val alarmCallbacks = mutableMapOf<Int, () -> Unit>()
    private var alarmCounter = 0
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        alarmManager = ctx.getSystemService(Context.ALARM_SERVICE) as? AlarmManager
    }
    
    @JvmStatic
    fun setAlarmAt(timeMillis: Long, callback: () -> Unit): Int {
        val ctx = context ?: return -1
        val am = alarmManager ?: return -1
        
        val requestCode = ++alarmCounter
        alarmCallbacks[requestCode] = callback
        
        val intent = Intent(ctx, AlarmReceiver::class.java).apply {
            putExtra("alarm_id", requestCode)
        }
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        val pendingIntent = PendingIntent.getBroadcast(ctx, requestCode, intent, flags)
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            am.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, timeMillis, pendingIntent)
        } else {
            am.setExact(AlarmManager.RTC_WAKEUP, timeMillis, pendingIntent)
        }
        
        return requestCode
    }
    
    @JvmStatic
    fun setAlarmAfter(delayMillis: Long, callback: () -> Unit): Int {
        return setAlarmAt(System.currentTimeMillis() + delayMillis, callback)
    }
    
    @JvmStatic
    fun setRepeatingAlarm(
        startTimeMillis: Long,
        intervalMillis: Long,
        callback: () -> Unit
    ): Int {
        val ctx = context ?: return -1
        val am = alarmManager ?: return -1
        
        val requestCode = ++alarmCounter
        alarmCallbacks[requestCode] = callback
        
        val intent = Intent(ctx, AlarmReceiver::class.java).apply {
            putExtra("alarm_id", requestCode)
            putExtra("repeating", true)
        }
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        val pendingIntent = PendingIntent.getBroadcast(ctx, requestCode, intent, flags)
        
        am.setRepeating(AlarmManager.RTC_WAKEUP, startTimeMillis, intervalMillis, pendingIntent)
        
        return requestCode
    }
    
    @JvmStatic
    fun setDailyAlarm(hour: Int, minute: Int, callback: () -> Unit): Int {
        val calendar = Calendar.getInstance().apply {
            set(Calendar.HOUR_OF_DAY, hour)
            set(Calendar.MINUTE, minute)
            set(Calendar.SECOND, 0)
            set(Calendar.MILLISECOND, 0)
            
            if (timeInMillis <= System.currentTimeMillis()) {
                add(Calendar.DAY_OF_YEAR, 1)
            }
        }
        
        return setRepeatingAlarm(
            calendar.timeInMillis,
            AlarmManager.INTERVAL_DAY,
            callback
        )
    }
    
    @JvmStatic
    fun cancel(alarmId: Int) {
        val ctx = context ?: return
        val am = alarmManager ?: return
        
        val intent = Intent(ctx, AlarmReceiver::class.java)
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        val pendingIntent = PendingIntent.getBroadcast(ctx, alarmId, intent, flags)
        am.cancel(pendingIntent)
        alarmCallbacks.remove(alarmId)
    }
    
    @JvmStatic
    fun cancelAll() {
        alarmCallbacks.keys.toList().forEach { cancel(it) }
    }
    
    internal fun handleAlarm(alarmId: Int) {
        alarmCallbacks[alarmId]?.invoke()
    }
    
    /**
     * مستقبل التنبيهات
     */
    class AlarmReceiver : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            val alarmId = intent.getIntExtra("alarm_id", -1)
            if (alarmId >= 0) {
                SadAlarmBridge.handleAlarm(alarmId)
                
                // إذا لم يكن تنبيه متكرر، احذف الـ callback
                if (!intent.getBooleanExtra("repeating", false)) {
                    alarmCallbacks.remove(alarmId)
                }
            }
        }
    }
}
