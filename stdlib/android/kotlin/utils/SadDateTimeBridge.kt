// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDateTimeBridge.kt
// الوصف: جسر Kotlin للتاريخ والوقت
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import java.text.SimpleDateFormat
import java.util.*

/**
 * جسر التاريخ والوقت — SadDateTimeBridge
 */
object SadDateTimeBridge {
    
    private val arabicMonths = arrayOf(
        "يناير", "فبراير", "مارس", "أبريل", "مايو", "يونيو",
        "يوليو", "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر"
    )
    
    private val arabicDays = arrayOf(
        "الأحد", "الإثنين", "الثلاثاء", "الأربعاء", "الخميس", "الجمعة", "السبت"
    )
    
    private val hijriMonths = arrayOf(
        "محرم", "صفر", "ربيع الأول", "ربيع الثاني", "جمادى الأولى", "جمادى الآخرة",
        "رجب", "شعبان", "رمضان", "شوال", "ذو القعدة", "ذو الحجة"
    )
    
    // الوقت الحالي
    @JvmStatic
    fun now(): Long = System.currentTimeMillis()
    
    @JvmStatic
    fun nowSeconds(): Long = System.currentTimeMillis() / 1000
    
    @JvmStatic
    fun currentYear(): Int = Calendar.getInstance().get(Calendar.YEAR)
    
    @JvmStatic
    fun currentMonth(): Int = Calendar.getInstance().get(Calendar.MONTH) + 1
    
    @JvmStatic
    fun currentDay(): Int = Calendar.getInstance().get(Calendar.DAY_OF_MONTH)
    
    @JvmStatic
    fun currentHour(): Int = Calendar.getInstance().get(Calendar.HOUR_OF_DAY)
    
    @JvmStatic
    fun currentMinute(): Int = Calendar.getInstance().get(Calendar.MINUTE)
    
    @JvmStatic
    fun currentSecond(): Int = Calendar.getInstance().get(Calendar.SECOND)
    
    @JvmStatic
    fun dayOfWeek(): Int = Calendar.getInstance().get(Calendar.DAY_OF_WEEK)
    
    @JvmStatic
    fun dayOfYear(): Int = Calendar.getInstance().get(Calendar.DAY_OF_YEAR)
    
    @JvmStatic
    fun weekOfYear(): Int = Calendar.getInstance().get(Calendar.WEEK_OF_YEAR)
    
    // التنسيق
    @JvmStatic
    fun format(millis: Long, pattern: String = "yyyy-MM-dd HH:mm:ss"): String {
        val sdf = SimpleDateFormat(pattern, Locale.getDefault())
        return sdf.format(Date(millis))
    }
    
    @JvmStatic
    fun formatNow(pattern: String = "yyyy-MM-dd HH:mm:ss"): String = format(now(), pattern)
    
    @JvmStatic
    fun formatArabic(millis: Long): String {
        val cal = Calendar.getInstance().apply { timeInMillis = millis }
        val day = cal.get(Calendar.DAY_OF_MONTH)
        val month = arabicMonths[cal.get(Calendar.MONTH)]
        val year = cal.get(Calendar.YEAR)
        return "$day $month $year"
    }
    
    @JvmStatic
    fun formatArabicFull(millis: Long): String {
        val cal = Calendar.getInstance().apply { timeInMillis = millis }
        val dayName = arabicDays[cal.get(Calendar.DAY_OF_WEEK) - 1]
        val day = cal.get(Calendar.DAY_OF_MONTH)
        val month = arabicMonths[cal.get(Calendar.MONTH)]
        val year = cal.get(Calendar.YEAR)
        val hour = cal.get(Calendar.HOUR_OF_DAY)
        val minute = cal.get(Calendar.MINUTE)
        return "$dayName، $day $month $year - $hour:${minute.toString().padStart(2, '0')}"
    }
    
    @JvmStatic
    fun formatTime(millis: Long, is24Hour: Boolean = true): String {
        val pattern = if (is24Hour) "HH:mm" else "hh:mm a"
        return format(millis, pattern)
    }
    
    @JvmStatic
    fun formatDate(millis: Long): String = format(millis, "yyyy-MM-dd")
    
    @JvmStatic
    fun formatISO(millis: Long): String = format(millis, "yyyy-MM-dd'T'HH:mm:ss'Z'")
    
    // التحليل
    @JvmStatic
    fun parse(dateString: String, pattern: String = "yyyy-MM-dd HH:mm:ss"): Long {
        return try {
            val sdf = SimpleDateFormat(pattern, Locale.getDefault())
            sdf.parse(dateString)?.time ?: 0L
        } catch (e: Exception) {
            0L
        }
    }
    
    @JvmStatic
    fun parseDate(dateString: String): Long = parse(dateString, "yyyy-MM-dd")
    
    @JvmStatic
    fun parseISO(isoString: String): Long = parse(isoString, "yyyy-MM-dd'T'HH:mm:ss'Z'")
    
    // الحسابات
    @JvmStatic
    fun addDays(millis: Long, days: Int): Long {
        return Calendar.getInstance().apply {
            timeInMillis = millis
            add(Calendar.DAY_OF_MONTH, days)
        }.timeInMillis
    }
    
    @JvmStatic
    fun addHours(millis: Long, hours: Int): Long {
        return Calendar.getInstance().apply {
            timeInMillis = millis
            add(Calendar.HOUR_OF_DAY, hours)
        }.timeInMillis
    }
    
    @JvmStatic
    fun addMinutes(millis: Long, minutes: Int): Long {
        return Calendar.getInstance().apply {
            timeInMillis = millis
            add(Calendar.MINUTE, minutes)
        }.timeInMillis
    }
    
    @JvmStatic
    fun addMonths(millis: Long, months: Int): Long {
        return Calendar.getInstance().apply {
            timeInMillis = millis
            add(Calendar.MONTH, months)
        }.timeInMillis
    }
    
    @JvmStatic
    fun addYears(millis: Long, years: Int): Long {
        return Calendar.getInstance().apply {
            timeInMillis = millis
            add(Calendar.YEAR, years)
        }.timeInMillis
    }
    
    @JvmStatic
    fun diffDays(millis1: Long, millis2: Long): Long {
        return (millis2 - millis1) / (24 * 60 * 60 * 1000)
    }
    
    @JvmStatic
    fun diffHours(millis1: Long, millis2: Long): Long {
        return (millis2 - millis1) / (60 * 60 * 1000)
    }
    
    @JvmStatic
    fun diffMinutes(millis1: Long, millis2: Long): Long {
        return (millis2 - millis1) / (60 * 1000)
    }
    
    @JvmStatic
    fun diffSeconds(millis1: Long, millis2: Long): Long {
        return (millis2 - millis1) / 1000
    }
    
    // الوقت النسبي
    @JvmStatic
    fun timeAgo(millis: Long): String {
        val diff = now() - millis
        val seconds = diff / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        val days = hours / 24
        val months = days / 30
        val years = days / 365
        
        return when {
            years > 0 -> "منذ $years ${if (years == 1L) "سنة" else "سنوات"}"
            months > 0 -> "منذ $months ${if (months == 1L) "شهر" else "أشهر"}"
            days > 0 -> "منذ $days ${if (days == 1L) "يوم" else "أيام"}"
            hours > 0 -> "منذ $hours ${if (hours == 1L) "ساعة" else "ساعات"}"
            minutes > 0 -> "منذ $minutes ${if (minutes == 1L) "دقيقة" else "دقائق"}"
            seconds > 10 -> "منذ $seconds ثانية"
            else -> "الآن"
        }
    }
    
    @JvmStatic
    fun timeUntil(millis: Long): String {
        val diff = millis - now()
        if (diff <= 0) return "انتهى"
        
        val seconds = diff / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        val days = hours / 24
        val months = days / 30
        val years = days / 365
        
        return when {
            years > 0 -> "بعد $years ${if (years == 1L) "سنة" else "سنوات"}"
            months > 0 -> "بعد $months ${if (months == 1L) "شهر" else "أشهر"}"
            days > 0 -> "بعد $days ${if (days == 1L) "يوم" else "أيام"}"
            hours > 0 -> "بعد $hours ${if (hours == 1L) "ساعة" else "ساعات"}"
            minutes > 0 -> "بعد $minutes ${if (minutes == 1L) "دقيقة" else "دقائق"}"
            else -> "بعد $seconds ثانية"
        }
    }
    
    // معلومات
    @JvmStatic
    fun isToday(millis: Long): Boolean {
        val today = Calendar.getInstance()
        val date = Calendar.getInstance().apply { timeInMillis = millis }
        
        return today.get(Calendar.YEAR) == date.get(Calendar.YEAR) &&
               today.get(Calendar.DAY_OF_YEAR) == date.get(Calendar.DAY_OF_YEAR)
    }
    
    @JvmStatic
    fun isYesterday(millis: Long): Boolean {
        val yesterday = Calendar.getInstance().apply { add(Calendar.DAY_OF_YEAR, -1) }
        val date = Calendar.getInstance().apply { timeInMillis = millis }
        
        return yesterday.get(Calendar.YEAR) == date.get(Calendar.YEAR) &&
               yesterday.get(Calendar.DAY_OF_YEAR) == date.get(Calendar.DAY_OF_YEAR)
    }
    
    @JvmStatic
    fun isTomorrow(millis: Long): Boolean {
        val tomorrow = Calendar.getInstance().apply { add(Calendar.DAY_OF_YEAR, 1) }
        val date = Calendar.getInstance().apply { timeInMillis = millis }
        
        return tomorrow.get(Calendar.YEAR) == date.get(Calendar.YEAR) &&
               tomorrow.get(Calendar.DAY_OF_YEAR) == date.get(Calendar.DAY_OF_YEAR)
    }
    
    @JvmStatic
    fun isWeekend(millis: Long): Boolean {
        val dayOfWeek = Calendar.getInstance().apply { timeInMillis = millis }.get(Calendar.DAY_OF_WEEK)
        return dayOfWeek == Calendar.FRIDAY || dayOfWeek == Calendar.SATURDAY
    }
    
    @JvmStatic
    fun getDayNameArabic(dayOfWeek: Int): String {
        return arabicDays.getOrElse(dayOfWeek - 1) { "غير معروف" }
    }
    
    @JvmStatic
    fun getMonthNameArabic(month: Int): String {
        return arabicMonths.getOrElse(month - 1) { "غير معروف" }
    }
    
    @JvmStatic
    fun getHijriMonthName(month: Int): String {
        return hijriMonths.getOrElse(month - 1) { "غير معروف" }
    }
    
    @JvmStatic
    fun getTimezone(): String = TimeZone.getDefault().id
    
    @JvmStatic
    fun getTimezoneOffset(): Int = TimeZone.getDefault().rawOffset / 1000 / 60
}
