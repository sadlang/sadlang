// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFormatterBridge.kt
// الوصف: جسر Kotlin لتنسيق النصوص والأرقام
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import java.text.DecimalFormat
import java.text.DecimalFormatSymbols
import java.text.NumberFormat
import java.text.SimpleDateFormat
import java.util.*

/**
 * جسر التنسيق — SadFormatterBridge
 */
object SadFormatterBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق الأرقام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun formatNumber(number: Double, decimalPlaces: Int = 2): String {
        val pattern = if (decimalPlaces > 0) {
            "#,##0." + "0".repeat(decimalPlaces)
        } else {
            "#,##0"
        }
        return DecimalFormat(pattern).format(number)
    }
    
    @JvmStatic
    fun formatNumberArabic(number: Double, decimalPlaces: Int = 2): String {
        val symbols = DecimalFormatSymbols(Locale("ar"))
        val pattern = if (decimalPlaces > 0) {
            "#,##0." + "0".repeat(decimalPlaces)
        } else {
            "#,##0"
        }
        return DecimalFormat(pattern, symbols).format(number)
    }
    
    @JvmStatic
    fun formatInteger(number: Long): String {
        return NumberFormat.getIntegerInstance().format(number)
    }
    
    @JvmStatic
    fun formatIntegerArabic(number: Long): String {
        return NumberFormat.getIntegerInstance(Locale("ar")).format(number)
    }
    
    @JvmStatic
    fun formatPercent(number: Double, decimalPlaces: Int = 0): String {
        val format = NumberFormat.getPercentInstance()
        format.minimumFractionDigits = decimalPlaces
        format.maximumFractionDigits = decimalPlaces
        return format.format(number)
    }
    
    @JvmStatic
    fun formatPercentArabic(number: Double, decimalPlaces: Int = 0): String {
        val format = NumberFormat.getPercentInstance(Locale("ar"))
        format.minimumFractionDigits = decimalPlaces
        format.maximumFractionDigits = decimalPlaces
        return format.format(number)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق العملات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun formatCurrency(amount: Double, currencyCode: String = "SAR"): String {
        val format = NumberFormat.getCurrencyInstance()
        format.currency = Currency.getInstance(currencyCode)
        return format.format(amount)
    }
    
    @JvmStatic
    fun formatCurrencyArabic(amount: Double, currencyCode: String = "SAR"): String {
        val format = NumberFormat.getCurrencyInstance(Locale("ar"))
        format.currency = Currency.getInstance(currencyCode)
        return format.format(amount)
    }
    
    @JvmStatic
    fun formatSAR(amount: Double): String = "${formatNumber(amount)} ر.س"
    
    @JvmStatic
    fun formatEGP(amount: Double): String = "${formatNumber(amount)} ج.م"
    
    @JvmStatic
    fun formatAED(amount: Double): String = "${formatNumber(amount)} د.إ"
    
    @JvmStatic
    fun formatUSD(amount: Double): String = "$${formatNumber(amount)}"
    
    @JvmStatic
    fun formatEUR(amount: Double): String = "€${formatNumber(amount)}"
    
    @JvmStatic
    fun formatCurrencyWithSymbol(amount: Double, symbol: String, symbolBefore: Boolean = true): String {
        val formatted = formatNumber(amount)
        return if (symbolBefore) "$symbol$formatted" else "$formatted $symbol"
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق أحجام الملفات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun formatFileSize(bytes: Long): String {
        return when {
            bytes < 1024 -> "$bytes B"
            bytes < 1024 * 1024 -> "${formatNumber(bytes / 1024.0, 1)} KB"
            bytes < 1024 * 1024 * 1024 -> "${formatNumber(bytes / (1024.0 * 1024.0), 2)} MB"
            else -> "${formatNumber(bytes / (1024.0 * 1024.0 * 1024.0), 2)} GB"
        }
    }
    
    @JvmStatic
    fun formatFileSizeArabic(bytes: Long): String {
        return when {
            bytes < 1024 -> "$bytes بايت"
            bytes < 1024 * 1024 -> "${formatNumber(bytes / 1024.0, 1)} كيلوبايت"
            bytes < 1024 * 1024 * 1024 -> "${formatNumber(bytes / (1024.0 * 1024.0), 2)} ميجابايت"
            else -> "${formatNumber(bytes / (1024.0 * 1024.0 * 1024.0), 2)} جيجابايت"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق المدة الزمنية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun formatDuration(seconds: Long): String {
        val hours = seconds / 3600
        val minutes = (seconds % 3600) / 60
        val secs = seconds % 60
        
        return when {
            hours > 0 -> String.format("%d:%02d:%02d", hours, minutes, secs)
            else -> String.format("%d:%02d", minutes, secs)
        }
    }
    
    @JvmStatic
    fun formatDurationLong(seconds: Long): String {
        val hours = seconds / 3600
        val minutes = (seconds % 3600) / 60
        val secs = seconds % 60
        
        val parts = mutableListOf<String>()
        if (hours > 0) parts.add("${hours}h")
        if (minutes > 0) parts.add("${minutes}m")
        if (secs > 0 || parts.isEmpty()) parts.add("${secs}s")
        
        return parts.joinToString(" ")
    }
    
    @JvmStatic
    fun formatDurationArabic(seconds: Long): String {
        val hours = seconds / 3600
        val minutes = (seconds % 3600) / 60
        val secs = seconds % 60
        
        val parts = mutableListOf<String>()
        if (hours > 0) parts.add("$hours ساعة")
        if (minutes > 0) parts.add("$minutes دقيقة")
        if (secs > 0 || parts.isEmpty()) parts.add("$secs ثانية")
        
        return parts.joinToString(" و ")
    }
    
    @JvmStatic
    fun formatMilliseconds(millis: Long): String {
        return formatDuration(millis / 1000)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق التاريخ والوقت
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun formatDate(date: Date, pattern: String = "yyyy-MM-dd"): String {
        return SimpleDateFormat(pattern, Locale.getDefault()).format(date)
    }
    
    @JvmStatic
    fun formatDateArabic(date: Date, pattern: String = "yyyy-MM-dd"): String {
        return SimpleDateFormat(pattern, Locale("ar")).format(date)
    }
    
    @JvmStatic
    fun formatTime(date: Date, pattern: String = "HH:mm:ss"): String {
        return SimpleDateFormat(pattern, Locale.getDefault()).format(date)
    }
    
    @JvmStatic
    fun formatDateTime(date: Date, pattern: String = "yyyy-MM-dd HH:mm:ss"): String {
        return SimpleDateFormat(pattern, Locale.getDefault()).format(date)
    }
    
    @JvmStatic
    fun formatRelativeTime(timestamp: Long): String {
        val now = System.currentTimeMillis()
        val diff = now - timestamp
        
        val seconds = diff / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        val days = hours / 24
        val weeks = days / 7
        val months = days / 30
        val years = days / 365
        
        return when {
            seconds < 60 -> "just now"
            minutes < 60 -> "$minutes minutes ago"
            hours < 24 -> "$hours hours ago"
            days < 7 -> "$days days ago"
            weeks < 4 -> "$weeks weeks ago"
            months < 12 -> "$months months ago"
            else -> "$years years ago"
        }
    }
    
    @JvmStatic
    fun formatRelativeTimeArabic(timestamp: Long): String {
        val now = System.currentTimeMillis()
        val diff = now - timestamp
        
        val seconds = diff / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        val days = hours / 24
        val weeks = days / 7
        val months = days / 30
        val years = days / 365
        
        return when {
            seconds < 60 -> "الآن"
            minutes == 1L -> "منذ دقيقة"
            minutes == 2L -> "منذ دقيقتين"
            minutes in 3..10 -> "منذ $minutes دقائق"
            minutes < 60 -> "منذ $minutes دقيقة"
            hours == 1L -> "منذ ساعة"
            hours == 2L -> "منذ ساعتين"
            hours in 3..10 -> "منذ $hours ساعات"
            hours < 24 -> "منذ $hours ساعة"
            days == 1L -> "أمس"
            days == 2L -> "منذ يومين"
            days in 3..10 -> "منذ $days أيام"
            days < 7 -> "منذ $days يوماً"
            weeks == 1L -> "منذ أسبوع"
            weeks == 2L -> "منذ أسبوعين"
            weeks < 4 -> "منذ $weeks أسابيع"
            months == 1L -> "منذ شهر"
            months == 2L -> "منذ شهرين"
            months in 3..10 -> "منذ $months أشهر"
            months < 12 -> "منذ $months شهراً"
            years == 1L -> "منذ سنة"
            years == 2L -> "منذ سنتين"
            years in 3..10 -> "منذ $years سنوات"
            else -> "منذ $years سنة"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق الأرقام بالكلمات العربية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private val arabicOnes = arrayOf(
        "", "واحد", "اثنان", "ثلاثة", "أربعة", "خمسة",
        "ستة", "سبعة", "ثمانية", "تسعة", "عشرة",
        "أحد عشر", "اثنا عشر", "ثلاثة عشر", "أربعة عشر", "خمسة عشر",
        "ستة عشر", "سبعة عشر", "ثمانية عشر", "تسعة عشر"
    )
    
    private val arabicTens = arrayOf(
        "", "", "عشرون", "ثلاثون", "أربعون", "خمسون",
        "ستون", "سبعون", "ثمانون", "تسعون"
    )
    
    private val arabicHundreds = arrayOf(
        "", "مائة", "مائتان", "ثلاثمائة", "أربعمائة", "خمسمائة",
        "ستمائة", "سبعمائة", "ثمانمائة", "تسعمائة"
    )
    
    @JvmStatic
    fun numberToArabicWords(number: Int): String {
        if (number == 0) return "صفر"
        if (number < 0) return "سالب ${numberToArabicWords(-number)}"
        if (number < 20) return arabicOnes[number]
        if (number < 100) {
            val tens = number / 10
            val ones = number % 10
            return if (ones == 0) arabicTens[tens]
            else "${arabicOnes[ones]} و${arabicTens[tens]}"
        }
        if (number < 1000) {
            val hundreds = number / 100
            val remainder = number % 100
            return if (remainder == 0) arabicHundreds[hundreds]
            else "${arabicHundreds[hundreds]} و${numberToArabicWords(remainder)}"
        }
        if (number < 1000000) {
            val thousands = number / 1000
            val remainder = number % 1000
            val thousandWord = when (thousands) {
                1 -> "ألف"
                2 -> "ألفان"
                in 3..10 -> "${numberToArabicWords(thousands)} آلاف"
                else -> "${numberToArabicWords(thousands)} ألف"
            }
            return if (remainder == 0) thousandWord
            else "$thousandWord و${numberToArabicWords(remainder)}"
        }
        return number.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تحويل الأرقام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun toArabicNumerals(number: String): String {
        val arabicDigits = charArrayOf('٠', '١', '٢', '٣', '٤', '٥', '٦', '٧', '٨', '٩')
        val sb = StringBuilder()
        for (c in number) {
            if (c in '0'..'9') {
                sb.append(arabicDigits[c - '0'])
            } else {
                sb.append(c)
            }
        }
        return sb.toString()
    }
    
    @JvmStatic
    fun toEnglishNumerals(number: String): String {
        val sb = StringBuilder()
        for (c in number) {
            when (c) {
                '٠' -> sb.append('0')
                '١' -> sb.append('1')
                '٢' -> sb.append('2')
                '٣' -> sb.append('3')
                '٤' -> sb.append('4')
                '٥' -> sb.append('5')
                '٦' -> sb.append('6')
                '٧' -> sb.append('7')
                '٨' -> sb.append('8')
                '٩' -> sb.append('9')
                else -> sb.append(c)
            }
        }
        return sb.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق النصوص
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun capitalize(text: String): String = text.replaceFirstChar { it.uppercase() }
    
    @JvmStatic
    fun capitalizeWords(text: String): String {
        return text.split(" ").joinToString(" ") { capitalize(it) }
    }
    
    @JvmStatic
    fun truncate(text: String, maxLength: Int, suffix: String = "..."): String {
        return if (text.length <= maxLength) text
        else text.take(maxLength - suffix.length) + suffix
    }
    
    @JvmStatic
    fun padLeft(text: String, length: Int, padChar: Char = ' '): String {
        return text.padStart(length, padChar)
    }
    
    @JvmStatic
    fun padRight(text: String, length: Int, padChar: Char = ' '): String {
        return text.padEnd(length, padChar)
    }
    
    @JvmStatic
    fun center(text: String, length: Int, padChar: Char = ' '): String {
        if (text.length >= length) return text
        val padding = (length - text.length) / 2
        return text.padStart(text.length + padding, padChar).padEnd(length, padChar)
    }
    
    @JvmStatic
    fun removeWhitespace(text: String): String = text.replace("\\s+".toRegex(), "")
    
    @JvmStatic
    fun normalizeWhitespace(text: String): String = text.replace("\\s+".toRegex(), " ").trim()
    
    @JvmStatic
    fun reverse(text: String): String = text.reversed()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق رقم الهاتف
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun formatPhoneSaudi(phone: String): String {
        val cleaned = phone.replace(Regex("[^0-9]"), "")
        return when {
            cleaned.length == 10 && cleaned.startsWith("05") -> {
                "${cleaned.substring(0, 4)} ${cleaned.substring(4, 7)} ${cleaned.substring(7)}"
            }
            cleaned.length == 12 && cleaned.startsWith("966") -> {
                "+966 ${cleaned.substring(3, 5)} ${cleaned.substring(5, 8)} ${cleaned.substring(8)}"
            }
            else -> phone
        }
    }
    
    @JvmStatic
    fun formatPhoneInternational(phone: String, countryCode: String = "+966"): String {
        val cleaned = phone.replace(Regex("[^0-9]"), "")
        if (cleaned.startsWith("0")) {
            return "$countryCode ${cleaned.substring(1)}"
        }
        return phone
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تنسيق الأرقام الترتيبية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun toOrdinal(number: Int): String {
        val suffix = when {
            number % 100 in 11..13 -> "th"
            number % 10 == 1 -> "st"
            number % 10 == 2 -> "nd"
            number % 10 == 3 -> "rd"
            else -> "th"
        }
        return "$number$suffix"
    }
    
    @JvmStatic
    fun toOrdinalArabic(number: Int): String {
        return when (number) {
            1 -> "الأول"
            2 -> "الثاني"
            3 -> "الثالث"
            4 -> "الرابع"
            5 -> "الخامس"
            6 -> "السادس"
            7 -> "السابع"
            8 -> "الثامن"
            9 -> "التاسع"
            10 -> "العاشر"
            else -> "رقم $number"
        }
    }
}
