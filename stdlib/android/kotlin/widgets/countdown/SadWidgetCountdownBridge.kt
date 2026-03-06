package sad.android.widgets.countdown

import android.content.Context
import android.os.Handler
import android.os.Looper
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر ويدجت العد التنازلي - عد تنازلي للأحداث
 * Countdown Widget Bridge - Event countdown
 * 
 * @author فريق لغة ص
 */
object SadWidgetCountdownBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val activeCountdowns = ConcurrentHashMap<String, CountdownTask>()
    
    /**
     * مهمة العد التنازلي / Countdown task
     */
    data class CountdownTask(
        val id: String,
        val widgetId: Int,
        val targetTime: Long,
        val title: String,
        var isRunning: Boolean = true
    )
    
    /**
     * بيانات العد التنازلي / Countdown data
     */
    data class CountdownData(
        val id: String,
        val title: String,
        val targetTime: Long,
        val remainingDays: Long,
        val remainingHours: Long,
        val remainingMinutes: Long,
        val remainingSeconds: Long,
        val totalRemainingMs: Long,
        val isExpired: Boolean,
        val progress: Float  // 0 إلى 1
    )
    
    /**
     * مستمع العد التنازلي / Countdown listener
     */
    interface CountdownListener {
        fun onCountdownUpdate(data: CountdownData)
        fun onCountdownExpired(id: String)
    }
    
    private val countdownListeners = ConcurrentHashMap<String, CountdownListener>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة العد التنازلي
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء عد تنازلي / Start countdown
     */
    fun startCountdown(
        widgetId: Int,
        targetTime: Long,
        title: String,
        updateInterval: Long = 1000,
        listener: CountdownListener
    ): String {
        val id = "countdown:${widgetId}:${System.currentTimeMillis()}"
        val task = CountdownTask(id, widgetId, targetTime, title)
        activeCountdowns[id] = task
        countdownListeners[id] = listener
        
        val startTime = System.currentTimeMillis()
        val totalDuration = targetTime - startTime
        
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning) {
                    activeCountdowns.remove(id)
                    countdownListeners.remove(id)
                    return
                }
                
                val now = System.currentTimeMillis()
                val remaining = targetTime - now
                
                if (remaining <= 0) {
                    task.isRunning = false
                    val data = calculateCountdownData(id, title, targetTime, startTime, 0)
                    mainHandler.post {
                        listener.onCountdownUpdate(data)
                        listener.onCountdownExpired(id)
                    }
                    activeCountdowns.remove(id)
                    countdownListeners.remove(id)
                    return
                }
                
                val data = calculateCountdownData(id, title, targetTime, startTime, remaining)
                mainHandler.post { listener.onCountdownUpdate(data) }
                mainHandler.postDelayed(this, updateInterval)
            }
        }
        
        mainHandler.post(runnable)
        return id
    }
    
    /**
     * بدء عد تنازلي بتاريخ / Start countdown with date
     */
    fun startCountdownToDate(
        widgetId: Int,
        year: Int,
        month: Int,
        day: Int,
        hour: Int = 0,
        minute: Int = 0,
        title: String,
        listener: CountdownListener
    ): String {
        val calendar = Calendar.getInstance().apply {
            set(year, month - 1, day, hour, minute, 0)
            set(Calendar.MILLISECOND, 0)
        }
        return startCountdown(widgetId, calendar.timeInMillis, title, 1000, listener)
    }
    
    /**
     * بدء عد تنازلي بمدة / Start countdown with duration
     */
    fun startCountdownWithDuration(
        widgetId: Int,
        durationMs: Long,
        title: String,
        listener: CountdownListener
    ): String {
        val targetTime = System.currentTimeMillis() + durationMs
        return startCountdown(widgetId, targetTime, title, 1000, listener)
    }
    
    /**
     * إيقاف العد التنازلي / Stop countdown
     */
    fun stopCountdown(id: String): Boolean {
        val task = activeCountdowns[id]
        if (task != null) {
            task.isRunning = false
            activeCountdowns.remove(id)
            countdownListeners.remove(id)
            return true
        }
        return false
    }
    
    /**
     * إيقاف كل العدادات للويدجت / Stop all widget countdowns
     */
    fun stopWidgetCountdowns(widgetId: Int): Int {
        var count = 0
        activeCountdowns.values.filter { it.widgetId == widgetId }.forEach {
            it.isRunning = false
            activeCountdowns.remove(it.id)
            countdownListeners.remove(it.id)
            count++
        }
        return count
    }
    
    private fun calculateCountdownData(
        id: String,
        title: String,
        targetTime: Long,
        startTime: Long,
        remainingMs: Long
    ): CountdownData {
        val totalDuration = targetTime - startTime
        val progress = if (totalDuration > 0) {
            1f - (remainingMs.toFloat() / totalDuration.toFloat())
        } else 1f
        
        val seconds = remainingMs / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        val days = hours / 24
        
        return CountdownData(
            id = id,
            title = title,
            targetTime = targetTime,
            remainingDays = days,
            remainingHours = hours % 24,
            remainingMinutes = minutes % 60,
            remainingSeconds = seconds % 60,
            totalRemainingMs = remainingMs,
            isExpired = remainingMs <= 0,
            progress = progress.coerceIn(0f, 1f)
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحصول على البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على بيانات العد التنازلي / Get countdown data
     */
    fun getCountdownData(id: String): CountdownData? {
        val task = activeCountdowns[id] ?: return null
        val remaining = task.targetTime - System.currentTimeMillis()
        return calculateCountdownData(id, task.title, task.targetTime, 0, maxOf(0, remaining))
    }
    
    /**
     * حساب الوقت المتبقي / Calculate remaining time
     */
    fun calculateRemainingTime(targetTime: Long): CountdownData {
        val now = System.currentTimeMillis()
        val remaining = maxOf(0, targetTime - now)
        return calculateCountdownData("temp", "", targetTime, now, remaining)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق العد التنازلي / Format countdown
     */
    fun formatCountdown(data: CountdownData, format: CountdownFormat = CountdownFormat.FULL): String {
        return when (format) {
            CountdownFormat.FULL -> {
                buildString {
                    if (data.remainingDays > 0) append("${data.remainingDays} يوم ")
                    if (data.remainingHours > 0) append("${data.remainingHours} ساعة ")
                    if (data.remainingMinutes > 0) append("${data.remainingMinutes} دقيقة ")
                    append("${data.remainingSeconds} ثانية")
                }
            }
            
            CountdownFormat.SHORT -> {
                when {
                    data.remainingDays > 0 -> "${data.remainingDays} يوم"
                    data.remainingHours > 0 -> "${data.remainingHours} ساعة"
                    data.remainingMinutes > 0 -> "${data.remainingMinutes} دقيقة"
                    else -> "${data.remainingSeconds} ثانية"
                }
            }
            
            CountdownFormat.DIGITAL -> {
                if (data.remainingDays > 0) {
                    String.format("%d:%02d:%02d:%02d", 
                        data.remainingDays, data.remainingHours, 
                        data.remainingMinutes, data.remainingSeconds)
                } else {
                    String.format("%02d:%02d:%02d", 
                        data.remainingHours, data.remainingMinutes, data.remainingSeconds)
                }
            }
            
            CountdownFormat.DAYS_ONLY -> "${data.remainingDays} يوم"
            
            CountdownFormat.HOURS_MINUTES -> {
                String.format("%02d:%02d", data.remainingHours, data.remainingMinutes)
            }
        }
    }
    
    /**
     * تنسيق العد التنازلي / Countdown format
     */
    enum class CountdownFormat {
        FULL,         // X يوم X ساعة X دقيقة X ثانية
        SHORT,        // أكبر وحدة فقط
        DIGITAL,      // 00:00:00:00
        DAYS_ONLY,    // X يوم
        HOURS_MINUTES // 00:00
    }
    
    /**
     * تنسيق التاريخ المستهدف / Format target date
     */
    fun formatTargetDate(timestamp: Long, locale: Locale = Locale("ar")): String {
        val formatter = SimpleDateFormat("EEEE, d MMMM yyyy - HH:mm", locale)
        return formatter.format(Date(timestamp))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مناسبات شائعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على تاريخ رأس السنة القادم / Get next New Year
     */
    fun getNextNewYear(): Long {
        val calendar = Calendar.getInstance()
        calendar.set(calendar.get(Calendar.YEAR) + 1, Calendar.JANUARY, 1, 0, 0, 0)
        return calendar.timeInMillis
    }
    
    /**
     * الحصول على تاريخ عيد الفطر التقريبي / Get approximate Eid al-Fitr
     * ملاحظة: التاريخ الهجري يحتاج مكتبة خاصة للدقة
     */
    fun getApproximateEidAlFitr(year: Int): Long {
        // تقريبي - يتغير سنوياً
        val calendar = Calendar.getInstance()
        calendar.set(year, Calendar.APRIL, 10, 0, 0, 0)
        return calendar.timeInMillis
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        activeCountdowns.values.forEach { it.isRunning = false }
        activeCountdowns.clear()
        countdownListeners.clear()
        context = null
    }
}
