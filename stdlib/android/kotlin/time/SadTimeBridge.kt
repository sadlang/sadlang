// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadTimeBridge.kt
// الوصف: جسر Kotlin للتاريخ والوقت
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.os.Build
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.TimeUnit

/**
 * جسر التاريخ والوقت — SadTimeBridge
 */
object SadTimeBridge {
    
    // Arabic month names
    private val arabicMonths = listOf(
        "يناير", "فبراير", "مارس", "أبريل", "مايو", "يونيو",
        "يوليو", "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر"
    )
    
    // Arabic Hijri months (approximate mapping)
    private val hijriMonths = listOf(
        "محرم", "صفر", "ربيع الأول", "ربيع الثاني",
        "جمادى الأولى", "جمادى الآخرة", "رجب", "شعبان",
        "رمضان", "شوال", "ذو القعدة", "ذو الحجة"
    )
    
    // Arabic day names
    private val arabicDays = listOf(
        "الأحد", "الإثنين", "الثلاثاء", "الأربعاء",
        "الخميس", "الجمعة", "السبت"
    )
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Current Time
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun now(): Long = System.currentTimeMillis()
    
    @JvmStatic
    fun nowNanos(): Long = System.nanoTime()
    
    @JvmStatic
    fun currentTimeSeconds(): Long = System.currentTimeMillis() / 1000
    
    @JvmStatic
    fun getDate(): Map<String, Any> {
        val calendar = Calendar.getInstance()
        return mapOf(
            "year" to calendar.get(Calendar.YEAR),
            "month" to (calendar.get(Calendar.MONTH) + 1),
            "day" to calendar.get(Calendar.DAY_OF_MONTH),
            "hour" to calendar.get(Calendar.HOUR_OF_DAY),
            "minute" to calendar.get(Calendar.MINUTE),
            "second" to calendar.get(Calendar.SECOND),
            "millisecond" to calendar.get(Calendar.MILLISECOND),
            "dayOfWeek" to calendar.get(Calendar.DAY_OF_WEEK),
            "dayOfYear" to calendar.get(Calendar.DAY_OF_YEAR),
            "weekOfYear" to calendar.get(Calendar.WEEK_OF_YEAR),
            "timestamp" to calendar.timeInMillis
        )
    }
    
    @JvmStatic
    fun getYear(): Int = Calendar.getInstance().get(Calendar.YEAR)
    
    @JvmStatic
    fun getMonth(): Int = Calendar.getInstance().get(Calendar.MONTH) + 1
    
    @JvmStatic
    fun getDay(): Int = Calendar.getInstance().get(Calendar.DAY_OF_MONTH)
    
    @JvmStatic
    fun getHour(): Int = Calendar.getInstance().get(Calendar.HOUR_OF_DAY)
    
    @JvmStatic
    fun getMinute(): Int = Calendar.getInstance().get(Calendar.MINUTE)
    
    @JvmStatic
    fun getSecond(): Int = Calendar.getInstance().get(Calendar.SECOND)
    
    @JvmStatic
    fun getDayOfWeek(): Int = Calendar.getInstance().get(Calendar.DAY_OF_WEEK)
    
    @JvmStatic
    fun getDayOfWeekArabic(): String {
        val dayOfWeek = Calendar.getInstance().get(Calendar.DAY_OF_WEEK)
        return arabicDays[dayOfWeek - 1]
    }
    
    @JvmStatic
    fun getMonthNameArabic(): String {
        val month = Calendar.getInstance().get(Calendar.MONTH)
        return arabicMonths[month]
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Formatting
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun format(timestamp: Long, pattern: String, locale: String = "ar"): String {
        return try {
            val loc = if (locale == "ar") Locale("ar") else Locale(locale)
            val sdf = SimpleDateFormat(pattern, loc)
            sdf.format(Date(timestamp))
        } catch (e: Exception) {
            ""
        }
    }
    
    @JvmStatic
    fun formatNow(pattern: String, locale: String = "ar"): String {
        return format(System.currentTimeMillis(), pattern, locale)
    }
    
    @JvmStatic
    fun formatDate(timestamp: Long, locale: String = "ar"): String {
        return format(timestamp, "yyyy/MM/dd", locale)
    }
    
    @JvmStatic
    fun formatTime(timestamp: Long, locale: String = "ar"): String {
        return format(timestamp, "HH:mm:ss", locale)
    }
    
    @JvmStatic
    fun formatDateTime(timestamp: Long, locale: String = "ar"): String {
        return format(timestamp, "yyyy/MM/dd HH:mm:ss", locale)
    }
    
    @JvmStatic
    fun formatArabic(timestamp: Long): String {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        
        val day = calendar.get(Calendar.DAY_OF_MONTH)
        val month = arabicMonths[calendar.get(Calendar.MONTH)]
        val year = calendar.get(Calendar.YEAR)
        val dayName = arabicDays[calendar.get(Calendar.DAY_OF_WEEK) - 1]
        
        return "$dayName، $day $month $year"
    }
    
    @JvmStatic
    fun formatTimeArabic(timestamp: Long): String {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        
        val hour = calendar.get(Calendar.HOUR)
        val minute = calendar.get(Calendar.MINUTE)
        val amPm = if (calendar.get(Calendar.AM_PM) == Calendar.AM) "صباحاً" else "مساءً"
        
        return String.format("%d:%02d %s", if (hour == 0) 12 else hour, minute, amPm)
    }
    
    @JvmStatic
    fun formatRelative(timestamp: Long): String {
        val now = System.currentTimeMillis()
        val diff = now - timestamp
        
        val seconds = TimeUnit.MILLISECONDS.toSeconds(diff)
        val minutes = TimeUnit.MILLISECONDS.toMinutes(diff)
        val hours = TimeUnit.MILLISECONDS.toHours(diff)
        val days = TimeUnit.MILLISECONDS.toDays(diff)
        
        return when {
            seconds < 60 -> "منذ لحظات"
            minutes < 60 -> "منذ $minutes دقيقة"
            hours < 24 -> "منذ $hours ساعة"
            days < 7 -> "منذ $days يوم"
            days < 30 -> "منذ ${days / 7} أسبوع"
            days < 365 -> "منذ ${days / 30} شهر"
            else -> "منذ ${days / 365} سنة"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Parsing
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun parse(dateString: String, pattern: String, locale: String = "en"): Long? {
        return try {
            val loc = Locale(locale)
            val sdf = SimpleDateFormat(pattern, loc)
            sdf.parse(dateString)?.time
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun parseIso8601(dateString: String): Long? {
        return parse(dateString, "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'", "en")
            ?: parse(dateString, "yyyy-MM-dd'T'HH:mm:ss'Z'", "en")
            ?: parse(dateString, "yyyy-MM-dd", "en")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Date Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createDate(
        year: Int,
        month: Int,
        day: Int,
        hour: Int = 0,
        minute: Int = 0,
        second: Int = 0
    ): Long {
        val calendar = Calendar.getInstance()
        calendar.set(year, month - 1, day, hour, minute, second)
        calendar.set(Calendar.MILLISECOND, 0)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun startOfDay(timestamp: Long): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.set(Calendar.HOUR_OF_DAY, 0)
        calendar.set(Calendar.MINUTE, 0)
        calendar.set(Calendar.SECOND, 0)
        calendar.set(Calendar.MILLISECOND, 0)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun endOfDay(timestamp: Long): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.set(Calendar.HOUR_OF_DAY, 23)
        calendar.set(Calendar.MINUTE, 59)
        calendar.set(Calendar.SECOND, 59)
        calendar.set(Calendar.MILLISECOND, 999)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun startOfMonth(timestamp: Long): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.set(Calendar.DAY_OF_MONTH, 1)
        return startOfDay(calendar.timeInMillis)
    }
    
    @JvmStatic
    fun endOfMonth(timestamp: Long): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.set(Calendar.DAY_OF_MONTH, calendar.getActualMaximum(Calendar.DAY_OF_MONTH))
        return endOfDay(calendar.timeInMillis)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Date Arithmetic
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addDays(timestamp: Long, days: Int): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.add(Calendar.DAY_OF_MONTH, days)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun addMonths(timestamp: Long, months: Int): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.add(Calendar.MONTH, months)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun addYears(timestamp: Long, years: Int): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.add(Calendar.YEAR, years)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun addHours(timestamp: Long, hours: Int): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.add(Calendar.HOUR_OF_DAY, hours)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun addMinutes(timestamp: Long, minutes: Int): Long {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        calendar.add(Calendar.MINUTE, minutes)
        return calendar.timeInMillis
    }
    
    @JvmStatic
    fun addSeconds(timestamp: Long, seconds: Int): Long {
        return timestamp + (seconds * 1000)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Differences
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun diffInSeconds(timestamp1: Long, timestamp2: Long): Long {
        return TimeUnit.MILLISECONDS.toSeconds(kotlin.math.abs(timestamp2 - timestamp1))
    }
    
    @JvmStatic
    fun diffInMinutes(timestamp1: Long, timestamp2: Long): Long {
        return TimeUnit.MILLISECONDS.toMinutes(kotlin.math.abs(timestamp2 - timestamp1))
    }
    
    @JvmStatic
    fun diffInHours(timestamp1: Long, timestamp2: Long): Long {
        return TimeUnit.MILLISECONDS.toHours(kotlin.math.abs(timestamp2 - timestamp1))
    }
    
    @JvmStatic
    fun diffInDays(timestamp1: Long, timestamp2: Long): Long {
        return TimeUnit.MILLISECONDS.toDays(kotlin.math.abs(timestamp2 - timestamp1))
    }
    
    @JvmStatic
    fun diffDetailed(timestamp1: Long, timestamp2: Long): Map<String, Long> {
        val diff = kotlin.math.abs(timestamp2 - timestamp1)
        
        val days = TimeUnit.MILLISECONDS.toDays(diff)
        val hours = TimeUnit.MILLISECONDS.toHours(diff) % 24
        val minutes = TimeUnit.MILLISECONDS.toMinutes(diff) % 60
        val seconds = TimeUnit.MILLISECONDS.toSeconds(diff) % 60
        
        return mapOf(
            "days" to days,
            "hours" to hours,
            "minutes" to minutes,
            "seconds" to seconds,
            "totalMilliseconds" to diff
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Comparisons
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isBefore(timestamp1: Long, timestamp2: Long): Boolean {
        return timestamp1 < timestamp2
    }
    
    @JvmStatic
    fun isAfter(timestamp1: Long, timestamp2: Long): Boolean {
        return timestamp1 > timestamp2
    }
    
    @JvmStatic
    fun isSameDay(timestamp1: Long, timestamp2: Long): Boolean {
        val cal1 = Calendar.getInstance().apply { timeInMillis = timestamp1 }
        val cal2 = Calendar.getInstance().apply { timeInMillis = timestamp2 }
        
        return cal1.get(Calendar.YEAR) == cal2.get(Calendar.YEAR) &&
               cal1.get(Calendar.DAY_OF_YEAR) == cal2.get(Calendar.DAY_OF_YEAR)
    }
    
    @JvmStatic
    fun isToday(timestamp: Long): Boolean {
        return isSameDay(timestamp, System.currentTimeMillis())
    }
    
    @JvmStatic
    fun isYesterday(timestamp: Long): Boolean {
        return isSameDay(timestamp, addDays(System.currentTimeMillis(), -1))
    }
    
    @JvmStatic
    fun isTomorrow(timestamp: Long): Boolean {
        return isSameDay(timestamp, addDays(System.currentTimeMillis(), 1))
    }
    
    @JvmStatic
    fun isWeekend(timestamp: Long): Boolean {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        val dayOfWeek = calendar.get(Calendar.DAY_OF_WEEK)
        return dayOfWeek == Calendar.FRIDAY || dayOfWeek == Calendar.SATURDAY
    }
    
    @JvmStatic
    fun isLeapYear(year: Int): Boolean {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Timezone
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getTimezone(): String {
        return TimeZone.getDefault().id
    }
    
    @JvmStatic
    fun getTimezoneOffset(): Int {
        return TimeZone.getDefault().rawOffset / (1000 * 60 * 60)
    }
    
    @JvmStatic
    fun getAvailableTimezones(): List<String> {
        return TimeZone.getAvailableIDs().toList()
    }
    
    @JvmStatic
    fun convertTimezone(timestamp: Long, fromTz: String, toTz: String): Long {
        val fromZone = TimeZone.getTimeZone(fromTz)
        val toZone = TimeZone.getTimeZone(toTz)
        
        val offset = toZone.rawOffset - fromZone.rawOffset
        return timestamp + offset
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Arabic Calendar Support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getArabicMonthNames(): List<String> = arabicMonths
    
    @JvmStatic
    fun getArabicDayNames(): List<String> = arabicDays
    
    @JvmStatic
    fun getHijriMonthNames(): List<String> = hijriMonths
    
    @JvmStatic
    fun formatFullArabic(timestamp: Long): String {
        val calendar = Calendar.getInstance()
        calendar.timeInMillis = timestamp
        
        val dayName = arabicDays[calendar.get(Calendar.DAY_OF_WEEK) - 1]
        val day = calendar.get(Calendar.DAY_OF_MONTH)
        val month = arabicMonths[calendar.get(Calendar.MONTH)]
        val year = calendar.get(Calendar.YEAR)
        val hour = calendar.get(Calendar.HOUR)
        val minute = calendar.get(Calendar.MINUTE)
        val amPm = if (calendar.get(Calendar.AM_PM) == Calendar.AM) "صباحاً" else "مساءً"
        
        return "$dayName، $day $month $year - ${String.format("%d:%02d", if (hour == 0) 12 else hour, minute)} $amPm"
    }
}
