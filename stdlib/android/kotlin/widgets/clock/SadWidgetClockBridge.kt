package sad.android.widgets.clock

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر ويدجت الساعة - عرض الوقت والتاريخ
 * Clock Widget Bridge - Time and date display
 * 
 * @author فريق لغة ص
 */
object SadWidgetClockBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val activeClocks = ConcurrentHashMap<String, ClockTask>()
    
    /**
     * مهمة الساعة / Clock task
     */
    data class ClockTask(
        val id: String,
        val widgetId: Int,
        val format: ClockFormat,
        val timezone: TimeZone,
        var isRunning: Boolean = true
    )
    
    /**
     * تنسيق الساعة / Clock format
     */
    data class ClockFormat(
        val timePattern: String = "HH:mm",
        val datePattern: String = "yyyy/MM/dd",
        val showSeconds: Boolean = false,
        val use24Hour: Boolean = true,
        val locale: Locale = Locale("ar")
    )
    
    /**
     * بيانات الوقت / Time data
     */
    data class TimeData(
        val hours: Int,
        val minutes: Int,
        val seconds: Int,
        val day: Int,
        val month: Int,
        val year: Int,
        val dayOfWeek: Int,
        val formattedTime: String,
        val formattedDate: String,
        val timestamp: Long
    )
    
    /**
     * مستمع الوقت / Time listener
     */
    interface TimeUpdateListener {
        fun onTimeUpdate(clockId: String, timeData: TimeData)
    }
    
    private val timeListeners = ConcurrentHashMap<String, TimeUpdateListener>()
    
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
    //                         إدارة الساعات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء ساعة / Start clock
     */
    fun startClock(
        widgetId: Int,
        format: ClockFormat = ClockFormat(),
        timezone: TimeZone = TimeZone.getDefault(),
        listener: TimeUpdateListener
    ): String {
        val clockId = "clock:${widgetId}:${System.currentTimeMillis()}"
        val task = ClockTask(clockId, widgetId, format, timezone)
        activeClocks[clockId] = task
        timeListeners[clockId] = listener
        
        val updateInterval = if (format.showSeconds) 1000L else 60000L
        
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning) {
                    activeClocks.remove(clockId)
                    timeListeners.remove(clockId)
                    return
                }
                
                val timeData = getCurrentTime(format, timezone)
                mainHandler.post {
                    listener.onTimeUpdate(clockId, timeData)
                }
                
                mainHandler.postDelayed(this, updateInterval)
            }
        }
        
        mainHandler.post(runnable)
        return clockId
    }
    
    /**
     * إيقاف ساعة / Stop clock
     */
    fun stopClock(clockId: String): Boolean {
        val task = activeClocks[clockId]
        if (task != null) {
            task.isRunning = false
            activeClocks.remove(clockId)
            timeListeners.remove(clockId)
            return true
        }
        return false
    }
    
    /**
     * إيقاف ساعات الويدجت / Stop widget clocks
     */
    fun stopWidgetClocks(widgetId: Int): Int {
        var count = 0
        activeClocks.values.filter { it.widgetId == widgetId }.forEach {
            it.isRunning = false
            activeClocks.remove(it.id)
            timeListeners.remove(it.id)
            count++
        }
        return count
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الوقت الحالي
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الوقت الحالي / Get current time
     */
    fun getCurrentTime(
        format: ClockFormat = ClockFormat(),
        timezone: TimeZone = TimeZone.getDefault()
    ): TimeData {
        val calendar = Calendar.getInstance(timezone)
        val now = calendar.time
        
        val timeFormatter = SimpleDateFormat(format.timePattern, format.locale).apply {
            this.timeZone = timezone
        }
        
        val dateFormatter = SimpleDateFormat(format.datePattern, format.locale).apply {
            this.timeZone = timezone
        }
        
        return TimeData(
            hours = calendar.get(Calendar.HOUR_OF_DAY),
            minutes = calendar.get(Calendar.MINUTE),
            seconds = calendar.get(Calendar.SECOND),
            day = calendar.get(Calendar.DAY_OF_MONTH),
            month = calendar.get(Calendar.MONTH) + 1,
            year = calendar.get(Calendar.YEAR),
            dayOfWeek = calendar.get(Calendar.DAY_OF_WEEK),
            formattedTime = timeFormatter.format(now),
            formattedDate = dateFormatter.format(now),
            timestamp = calendar.timeInMillis
        )
    }
    
    /**
     * تنسيق الوقت / Format time
     */
    fun formatTime(
        timestamp: Long,
        pattern: String = "HH:mm",
        timezone: TimeZone = TimeZone.getDefault(),
        locale: Locale = Locale("ar")
    ): String {
        val formatter = SimpleDateFormat(pattern, locale).apply {
            this.timeZone = timezone
        }
        return formatter.format(Date(timestamp))
    }
    
    /**
     * تنسيق التاريخ / Format date
     */
    fun formatDate(
        timestamp: Long,
        pattern: String = "yyyy/MM/dd",
        timezone: TimeZone = TimeZone.getDefault(),
        locale: Locale = Locale("ar")
    ): String {
        val formatter = SimpleDateFormat(pattern, locale).apply {
            this.timeZone = timezone
        }
        return formatter.format(Date(timestamp))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تنسيقات جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيقات الوقت / Time formats
     */
    object TimeFormats {
        val HOURS_MINUTES_24 = "HH:mm"
        val HOURS_MINUTES_12 = "hh:mm a"
        val HOURS_MINUTES_SECONDS_24 = "HH:mm:ss"
        val HOURS_MINUTES_SECONDS_12 = "hh:mm:ss a"
        val HOURS_ONLY = "HH"
        val MINUTES_ONLY = "mm"
    }
    
    /**
     * تنسيقات التاريخ / Date formats
     */
    object DateFormats {
        val YEAR_MONTH_DAY = "yyyy/MM/dd"
        val DAY_MONTH_YEAR = "dd/MM/yyyy"
        val MONTH_DAY_YEAR = "MM/dd/yyyy"
        val FULL_DATE = "EEEE, d MMMM yyyy"
        val SHORT_DATE = "d MMM"
        val DAY_NAME = "EEEE"
        val MONTH_NAME = "MMMM"
    }
    
    /**
     * أسماء الأيام العربية / Arabic day names
     */
    fun getArabicDayName(dayOfWeek: Int): String {
        return when (dayOfWeek) {
            Calendar.SUNDAY -> "الأحد"
            Calendar.MONDAY -> "الإثنين"
            Calendar.TUESDAY -> "الثلاثاء"
            Calendar.WEDNESDAY -> "الأربعاء"
            Calendar.THURSDAY -> "الخميس"
            Calendar.FRIDAY -> "الجمعة"
            Calendar.SATURDAY -> "السبت"
            else -> ""
        }
    }
    
    /**
     * أسماء الشهور العربية / Arabic month names
     */
    fun getArabicMonthName(month: Int): String {
        return when (month) {
            1 -> "يناير"
            2 -> "فبراير"
            3 -> "مارس"
            4 -> "أبريل"
            5 -> "مايو"
            6 -> "يونيو"
            7 -> "يوليو"
            8 -> "أغسطس"
            9 -> "سبتمبر"
            10 -> "أكتوبر"
            11 -> "نوفمبر"
            12 -> "ديسمبر"
            else -> ""
        }
    }
    
    /**
     * أسماء الشهور الهجرية / Hijri month names
     */
    fun getHijriMonthName(month: Int): String {
        return when (month) {
            1 -> "محرم"
            2 -> "صفر"
            3 -> "ربيع الأول"
            4 -> "ربيع الثاني"
            5 -> "جمادى الأولى"
            6 -> "جمادى الآخرة"
            7 -> "رجب"
            8 -> "شعبان"
            9 -> "رمضان"
            10 -> "شوال"
            11 -> "ذو القعدة"
            12 -> "ذو الحجة"
            else -> ""
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المناطق الزمنية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مناطق زمنية شائعة / Common timezones
     */
    object CommonTimezones {
        val MECCA = TimeZone.getTimeZone("Asia/Riyadh")
        val CAIRO = TimeZone.getTimeZone("Africa/Cairo")
        val DUBAI = TimeZone.getTimeZone("Asia/Dubai")
        val LONDON = TimeZone.getTimeZone("Europe/London")
        val NEW_YORK = TimeZone.getTimeZone("America/New_York")
        val LOS_ANGELES = TimeZone.getTimeZone("America/Los_Angeles")
        val TOKYO = TimeZone.getTimeZone("Asia/Tokyo")
        val UTC = TimeZone.getTimeZone("UTC")
    }
    
    /**
     * الحصول على فرق التوقيت / Get timezone offset
     */
    fun getTimezoneOffset(timezone: TimeZone): Int {
        return timezone.rawOffset / (1000 * 60 * 60)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        activeClocks.values.forEach { it.isRunning = false }
        activeClocks.clear()
        timeListeners.clear()
        context = null
    }
}
