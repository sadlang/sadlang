// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadStringBridge.kt
// الوصف: جسر Kotlin لمعالجة النصوص
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import java.security.MessageDigest
import java.text.Normalizer
import java.util.*

/**
 * جسر النصوص — SadStringBridge
 */
object SadStringBridge {
    
    // التحويلات الأساسية
    @JvmStatic
    fun toUpperCase(text: String): String = text.uppercase()
    
    @JvmStatic
    fun toLowerCase(text: String): String = text.lowercase()
    
    @JvmStatic
    fun capitalize(text: String): String = text.replaceFirstChar { it.uppercaseChar() }
    
    @JvmStatic
    fun capitalizeWords(text: String): String {
        return text.split(" ").joinToString(" ") { word ->
            word.replaceFirstChar { it.uppercaseChar() }
        }
    }
    
    @JvmStatic
    fun reverse(text: String): String = text.reversed()
    
    @JvmStatic
    fun trim(text: String): String = text.trim()
    
    @JvmStatic
    fun trimStart(text: String): String = text.trimStart()
    
    @JvmStatic
    fun trimEnd(text: String): String = text.trimEnd()
    
    // البحث والاستبدال
    @JvmStatic
    fun contains(text: String, substring: String, ignoreCase: Boolean = false): Boolean {
        return text.contains(substring, ignoreCase)
    }
    
    @JvmStatic
    fun startsWith(text: String, prefix: String, ignoreCase: Boolean = false): Boolean {
        return text.startsWith(prefix, ignoreCase)
    }
    
    @JvmStatic
    fun endsWith(text: String, suffix: String, ignoreCase: Boolean = false): Boolean {
        return text.endsWith(suffix, ignoreCase)
    }
    
    @JvmStatic
    fun indexOf(text: String, substring: String): Int = text.indexOf(substring)
    
    @JvmStatic
    fun lastIndexOf(text: String, substring: String): Int = text.lastIndexOf(substring)
    
    @JvmStatic
    fun replace(text: String, old: String, new: String): String = text.replace(old, new)
    
    @JvmStatic
    fun replaceFirst(text: String, old: String, new: String): String = text.replaceFirst(old, new)
    
    @JvmStatic
    fun replaceRegex(text: String, regex: String, replacement: String): String {
        return text.replace(Regex(regex), replacement)
    }
    
    // التقسيم والدمج
    @JvmStatic
    fun split(text: String, delimiter: String): List<String> = text.split(delimiter)
    
    @JvmStatic
    fun join(parts: List<String>, separator: String = ""): String = parts.joinToString(separator)
    
    @JvmStatic
    fun splitLines(text: String): List<String> = text.lines()
    
    @JvmStatic
    fun words(text: String): List<String> = text.split(Regex("\\s+")).filter { it.isNotEmpty() }
    
    @JvmStatic
    fun wordCount(text: String): Int = words(text).size
    
    @JvmStatic
    fun characterCount(text: String, excludeSpaces: Boolean = false): Int {
        return if (excludeSpaces) text.replace(" ", "").length else text.length
    }
    
    // الاقتطاع
    @JvmStatic
    fun substring(text: String, start: Int, end: Int? = null): String {
        return if (end != null) text.substring(start, minOf(end, text.length))
        else text.substring(start)
    }
    
    @JvmStatic
    fun truncate(text: String, maxLength: Int, suffix: String = "..."): String {
        return if (text.length <= maxLength) text
        else text.take(maxLength - suffix.length) + suffix
    }
    
    @JvmStatic
    fun take(text: String, n: Int): String = text.take(n)
    
    @JvmStatic
    fun takeLast(text: String, n: Int): String = text.takeLast(n)
    
    @JvmStatic
    fun drop(text: String, n: Int): String = text.drop(n)
    
    @JvmStatic
    fun dropLast(text: String, n: Int): String = text.dropLast(n)
    
    // التنسيق
    @JvmStatic
    fun padStart(text: String, length: Int, char: Char = ' '): String = text.padStart(length, char)
    
    @JvmStatic
    fun padEnd(text: String, length: Int, char: Char = ' '): String = text.padEnd(length, char)
    
    @JvmStatic
    fun center(text: String, width: Int, char: Char = ' '): String {
        if (text.length >= width) return text
        val padding = width - text.length
        val left = padding / 2
        val right = padding - left
        return char.toString().repeat(left) + text + char.toString().repeat(right)
    }
    
    @JvmStatic
    fun repeat(text: String, times: Int): String = text.repeat(times)
    
    // الأرقام العربية والإنجليزية
    @JvmStatic
    fun toArabicNumerals(text: String): String {
        val arabicDigits = charArrayOf('٠', '١', '٢', '٣', '٤', '٥', '٦', '٧', '٨', '٩')
        return text.map { char ->
            if (char.isDigit()) arabicDigits[char.digitToInt()]
            else char
        }.joinToString("")
    }
    
    @JvmStatic
    fun toEnglishNumerals(text: String): String {
        val arabicToEnglish = mapOf(
            '٠' to '0', '١' to '1', '٢' to '2', '٣' to '3', '٤' to '4',
            '٥' to '5', '٦' to '6', '٧' to '7', '٨' to '8', '٩' to '9'
        )
        return text.map { arabicToEnglish[it] ?: it }.joinToString("")
    }
    
    // التحويلات
    @JvmStatic
    fun toSlug(text: String): String {
        return Normalizer.normalize(text, Normalizer.Form.NFD)
            .replace(Regex("[^\\p{ASCII}]"), "")
            .lowercase()
            .replace(Regex("[^a-z0-9\\s-]"), "")
            .replace(Regex("\\s+"), "-")
            .replace(Regex("-+"), "-")
            .trim('-')
    }
    
    @JvmStatic
    fun toCamelCase(text: String): String {
        return text.split(Regex("[\\s_-]+"))
            .mapIndexed { index, word ->
                if (index == 0) word.lowercase()
                else word.replaceFirstChar { it.uppercaseChar() }
            }
            .joinToString("")
    }
    
    @JvmStatic
    fun toSnakeCase(text: String): String {
        return text.replace(Regex("([a-z])([A-Z])"), "$1_$2")
            .replace(Regex("[\\s-]+"), "_")
            .lowercase()
    }
    
    @JvmStatic
    fun toKebabCase(text: String): String {
        return text.replace(Regex("([a-z])([A-Z])"), "$1-$2")
            .replace(Regex("[\\s_]+"), "-")
            .lowercase()
    }
    
    // التشفير
    @JvmStatic
    fun md5(text: String): String {
        val md = MessageDigest.getInstance("MD5")
        return md.digest(text.toByteArray()).joinToString("") { "%02x".format(it) }
    }
    
    @JvmStatic
    fun sha256(text: String): String {
        val md = MessageDigest.getInstance("SHA-256")
        return md.digest(text.toByteArray()).joinToString("") { "%02x".format(it) }
    }
    
    @JvmStatic
    fun base64Encode(text: String): String {
        return Base64.getEncoder().encodeToString(text.toByteArray())
    }
    
    @JvmStatic
    fun base64Decode(encoded: String): String {
        return String(Base64.getDecoder().decode(encoded))
    }
    
    // إزالة التشكيل
    @JvmStatic
    fun removeArabicDiacritics(text: String): String {
        // إزالة الحركات العربية
        val diacriticsPattern = Regex("[\\u064B-\\u065F\\u0670]")
        return text.replace(diacriticsPattern, "")
    }
    
    // توليد نص عشوائي
    @JvmStatic
    fun randomString(length: Int, includeNumbers: Boolean = true, includeSpecial: Boolean = false): String {
        val letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        val numbers = "0123456789"
        val special = "!@#\$%^&*"
        
        var chars = letters
        if (includeNumbers) chars += numbers
        if (includeSpecial) chars += special
        
        return (1..length).map { chars.random() }.joinToString("")
    }
    
    @JvmStatic
    fun uuid(): String = UUID.randomUUID().toString()
    
    // مساعدات
    @JvmStatic
    fun isNullOrEmpty(text: String?): Boolean = text.isNullOrEmpty()
    
    @JvmStatic
    fun isNullOrBlank(text: String?): Boolean = text.isNullOrBlank()
    
    @JvmStatic
    fun orEmpty(text: String?): String = text ?: ""
    
    @JvmStatic
    fun ifEmpty(text: String, default: String): String = text.ifEmpty { default }
    
    @JvmStatic
    fun ifBlank(text: String, default: String): String = text.ifBlank { default }
}
