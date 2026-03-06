// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadArabicStringBridge.kt
// الوصف: جسر Kotlin لمعالجة النصوص العربية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import java.text.Normalizer
import java.util.regex.Pattern

/**
 * جسر النصوص العربية — SadArabicStringBridge
 */
object SadArabicStringBridge {
    
    // Arabic numbers
    private val arabicNumerals = "٠١٢٣٤٥٦٧٨٩"
    private val englishNumerals = "0123456789"
    
    // Arabic diacritics (tashkeel)
    private val tashkeel = "\u064B\u064C\u064D\u064E\u064F\u0650\u0651\u0652\u0653\u0654\u0655\u0656\u0657\u0658\u0659\u065A\u065B\u065C\u065D\u065E\u065F"
    
    // Arabic letters
    private val arabicLetters = "ءآأإؤئابتثجحخدذرزسشصضطظعغفقكلمنهوىيةـ"
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Number Conversion
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun toArabicNumerals(text: String): String {
        val result = StringBuilder()
        for (char in text) {
            if (char.isDigit()) {
                result.append(arabicNumerals[char.digitToInt()])
            } else {
                result.append(char)
            }
        }
        return result.toString()
    }
    
    @JvmStatic
    fun toEnglishNumerals(text: String): String {
        val result = StringBuilder()
        for (char in text) {
            val index = arabicNumerals.indexOf(char)
            if (index >= 0) {
                result.append(englishNumerals[index])
            } else {
                result.append(char)
            }
        }
        return result.toString()
    }
    
    @JvmStatic
    fun formatNumberArabic(number: Long): String {
        return toArabicNumerals(number.toString())
    }
    
    @JvmStatic
    fun formatNumberArabic(number: Double, decimals: Int = 2): String {
        val formatted = String.format("%.${decimals}f", number)
        return toArabicNumerals(formatted)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Tashkeel (Diacritics)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun removeTashkeel(text: String): String {
        return text.replace(Regex("[$tashkeel]"), "")
    }
    
    @JvmStatic
    fun hasTashkeel(text: String): Boolean {
        return text.any { it in tashkeel }
    }
    
    @JvmStatic
    fun getTashkeelCount(text: String): Int {
        return text.count { it in tashkeel }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Character Analysis
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isArabic(text: String): Boolean {
        if (text.isEmpty()) return false
        val withoutTashkeel = removeTashkeel(text.replace(" ", ""))
        return withoutTashkeel.all { it in arabicLetters || it.isWhitespace() }
    }
    
    @JvmStatic
    fun containsArabic(text: String): Boolean {
        return text.any { it in arabicLetters }
    }
    
    @JvmStatic
    fun isArabicLetter(char: Char): Boolean {
        return char in arabicLetters
    }
    
    @JvmStatic
    fun getArabicLetterCount(text: String): Int {
        return text.count { it in arabicLetters }
    }
    
    @JvmStatic
    fun getArabicPercentage(text: String): Double {
        if (text.isEmpty()) return 0.0
        val totalLetters = text.count { it.isLetter() || it in arabicLetters }
        if (totalLetters == 0) return 0.0
        val arabicCount = getArabicLetterCount(text)
        return (arabicCount.toDouble() / totalLetters.toDouble()) * 100
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Normalization
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun normalizeAlef(text: String): String {
        return text
            .replace('أ', 'ا')
            .replace('إ', 'ا')
            .replace('آ', 'ا')
            .replace('ٱ', 'ا')
    }
    
    @JvmStatic
    fun normalizeYeh(text: String): String {
        return text
            .replace('ى', 'ي')
            .replace('ئ', 'ي')
    }
    
    @JvmStatic
    fun normalizeHeh(text: String): String {
        return text.replace('ة', 'ه')
    }
    
    @JvmStatic
    fun normalizeWaw(text: String): String {
        return text.replace('ؤ', 'و')
    }
    
    @JvmStatic
    fun normalizeArabic(text: String): String {
        return text
            .let { normalizeAlef(it) }
            .let { normalizeYeh(it) }
            .let { normalizeHeh(it) }
            .let { normalizeWaw(it) }
            .let { removeTashkeel(it) }
    }
    
    @JvmStatic
    fun normalize(text: String): String {
        return Normalizer.normalize(text, Normalizer.Form.NFKC)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Search & Compare
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun arabicEquals(text1: String, text2: String): Boolean {
        return normalizeArabic(text1) == normalizeArabic(text2)
    }
    
    @JvmStatic
    fun arabicContains(text: String, searchTerm: String): Boolean {
        return normalizeArabic(text).contains(normalizeArabic(searchTerm))
    }
    
    @JvmStatic
    fun arabicStartsWith(text: String, prefix: String): Boolean {
        return normalizeArabic(text).startsWith(normalizeArabic(prefix))
    }
    
    @JvmStatic
    fun arabicEndsWith(text: String, suffix: String): Boolean {
        return normalizeArabic(text).endsWith(normalizeArabic(suffix))
    }
    
    @JvmStatic
    fun findArabic(text: String, searchTerm: String): Int {
        return normalizeArabic(text).indexOf(normalizeArabic(searchTerm))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Word Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getWords(text: String): List<String> {
        return text.split(Regex("\\s+")).filter { it.isNotEmpty() }
    }
    
    @JvmStatic
    fun getArabicWords(text: String): List<String> {
        return getWords(text).filter { containsArabic(it) }
    }
    
    @JvmStatic
    fun getWordCount(text: String): Int {
        return getWords(text).size
    }
    
    @JvmStatic
    fun getCharacterCount(text: String, excludeSpaces: Boolean = false): Int {
        return if (excludeSpaces) {
            text.replace(" ", "").length
        } else {
            text.length
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // RTL Support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isRtl(text: String): Boolean {
        return containsArabic(text)
    }
    
    @JvmStatic
    fun addRtlMark(text: String): String {
        return "\u200F$text"
    }
    
    @JvmStatic
    fun addLtrMark(text: String): String {
        return "\u200E$text"
    }
    
    @JvmStatic
    fun wrapWithRtlEmbed(text: String): String {
        return "\u202B$text\u202C"
    }
    
    @JvmStatic
    fun wrapWithLtrEmbed(text: String): String {
        return "\u202A$text\u202C"
    }
    
    @JvmStatic
    fun removeDirectionalMarks(text: String): String {
        return text.replace(Regex("[\u200E\u200F\u202A\u202B\u202C\u202D\u202E]"), "")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Keyboard Support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun convertEnglishToArabicKeyboard(text: String): String {
        val mapping = mapOf(
            'q' to 'ض', 'w' to 'ص', 'e' to 'ث', 'r' to 'ق', 't' to 'ف', 'y' to 'غ',
            'u' to 'ع', 'i' to 'ه', 'o' to 'خ', 'p' to 'ح', '[' to 'ج', ']' to 'د',
            'a' to 'ش', 's' to 'س', 'd' to 'ي', 'f' to 'ب', 'g' to 'ل', 'h' to 'ا',
            'j' to 'ت', 'k' to 'ن', 'l' to 'م', ';' to 'ك', '\'' to 'ط',
            'z' to 'ئ', 'x' to 'ء', 'c' to 'ؤ', 'v' to 'ر', 'b' to 'ﻻ', 'n' to 'ى',
            'm' to 'ة', ',' to 'و', '.' to 'ز', '/' to 'ظ'
        )
        
        return text.map { char ->
            mapping[char.lowercaseChar()] ?: char
        }.joinToString("")
    }
    
    @JvmStatic
    fun convertArabicToEnglishKeyboard(text: String): String {
        val mapping = mapOf(
            'ض' to 'q', 'ص' to 'w', 'ث' to 'e', 'ق' to 'r', 'ف' to 't', 'غ' to 'y',
            'ع' to 'u', 'ه' to 'i', 'خ' to 'o', 'ح' to 'p', 'ج' to '[', 'د' to ']',
            'ش' to 'a', 'س' to 's', 'ي' to 'd', 'ب' to 'f', 'ل' to 'g', 'ا' to 'h',
            'ت' to 'j', 'ن' to 'k', 'م' to 'l', 'ك' to ';', 'ط' to '\'',
            'ئ' to 'z', 'ء' to 'x', 'ؤ' to 'c', 'ر' to 'v', 'ى' to 'n',
            'ة' to 'm', 'و' to ',', 'ز' to '.', 'ظ' to '/'
        )
        
        return text.map { char ->
            mapping[char] ?: char
        }.joinToString("")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Pluralization (Arabic)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun pluralize(count: Int, singular: String, dual: String, plural: String): String {
        return when {
            count == 0 -> "لا $plural"
            count == 1 -> "$singular واحد"
            count == 2 -> "$dual"
            count in 3..10 -> "$count $plural"
            else -> "$count $singular"
        }
    }
    
    @JvmStatic
    fun pluralizeSimple(count: Int, word: String): String {
        return when {
            count == 0 -> "لا ${word}ات"
            count == 1 -> "$word واحد"
            count == 2 -> "${word}ان"
            else -> "$count ${word}ات"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Formatting
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun reverseWords(text: String): String {
        return getWords(text).reversed().joinToString(" ")
    }
    
    @JvmStatic
    fun reverseString(text: String): String {
        return text.reversed()
    }
    
    @JvmStatic
    fun truncate(text: String, maxLength: Int, suffix: String = "..."): String {
        return if (text.length <= maxLength) {
            text
        } else {
            text.take(maxLength - suffix.length) + suffix
        }
    }
    
    @JvmStatic
    fun padStart(text: String, length: Int, padChar: Char = ' '): String {
        return text.padStart(length, padChar)
    }
    
    @JvmStatic
    fun padEnd(text: String, length: Int, padChar: Char = ' '): String {
        return text.padEnd(length, padChar)
    }
    
    @JvmStatic
    fun center(text: String, length: Int, padChar: Char = ' '): String {
        if (text.length >= length) return text
        val totalPadding = length - text.length
        val leftPadding = totalPadding / 2
        val rightPadding = totalPadding - leftPadding
        return padChar.toString().repeat(leftPadding) + text + padChar.toString().repeat(rightPadding)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Validation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isArabicName(text: String): Boolean {
        val normalized = removeTashkeel(text.replace(" ", ""))
        return normalized.isNotEmpty() && normalized.all { it in arabicLetters }
    }
    
    @JvmStatic
    fun isArabicSentence(text: String): Boolean {
        val arabicPercentage = getArabicPercentage(text)
        return arabicPercentage >= 50.0
    }
    
    @JvmStatic
    fun hasArabicPunctuation(text: String): Boolean {
        val arabicPunctuation = "،؛؟"
        return text.any { it in arabicPunctuation }
    }
    
    @JvmStatic
    fun replaceArabicPunctuation(text: String): String {
        return text
            .replace('،', ',')
            .replace('؛', ';')
            .replace('؟', '?')
    }
    
    @JvmStatic
    fun replaceEnglishPunctuation(text: String): String {
        return text
            .replace(',', '،')
            .replace(';', '؛')
            .replace('?', '؟')
    }
}
