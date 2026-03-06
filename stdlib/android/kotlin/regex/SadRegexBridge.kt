// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadRegexBridge.kt
// الوصف: جسر Kotlin للتعبيرات النمطية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import java.util.regex.Pattern
import java.util.regex.PatternSyntaxException

/**
 * جسر التعبيرات النمطية — SadRegexBridge
 */
object SadRegexBridge {
    
    // Pre-compiled common patterns
    private val emailPattern = Regex("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}")
    private val phonePattern = Regex("^[+]?[0-9]{10,15}$")
    private val urlPattern = Regex("https?://[\\w\\-._~:/?#\\[\\]@!$&'()*+,;=%]+")
    private val arabicPattern = Regex("[\\u0600-\\u06FF\\u0750-\\u077F]+")
    private val numberPattern = Regex("-?\\d+(\\.\\d+)?")
    private val integerPattern = Regex("-?\\d+")
    private val ipv4Pattern = Regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$")
    
    // Cached patterns
    private val patternCache = mutableMapOf<String, Regex>()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Pattern Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun compile(pattern: String, ignoreCase: Boolean = false): Regex? {
        return try {
            val options = if (ignoreCase) setOf(RegexOption.IGNORE_CASE) else emptySet()
            Regex(pattern, options)
        } catch (e: PatternSyntaxException) {
            null
        }
    }
    
    @JvmStatic
    fun compileWithOptions(
        pattern: String,
        ignoreCase: Boolean = false,
        multiline: Boolean = false,
        dotMatchesAll: Boolean = false
    ): Regex? {
        return try {
            val options = mutableSetOf<RegexOption>()
            if (ignoreCase) options.add(RegexOption.IGNORE_CASE)
            if (multiline) options.add(RegexOption.MULTILINE)
            if (dotMatchesAll) options.add(RegexOption.DOT_MATCHES_ALL)
            Regex(pattern, options)
        } catch (e: PatternSyntaxException) {
            null
        }
    }
    
    @JvmStatic
    fun getCached(pattern: String): Regex? {
        return patternCache.getOrPut(pattern) {
            compile(pattern) ?: return null
        }
    }
    
    @JvmStatic
    fun clearCache() {
        patternCache.clear()
    }
    
    @JvmStatic
    fun isValidPattern(pattern: String): Boolean {
        return try {
            Regex(pattern)
            true
        } catch (e: PatternSyntaxException) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Matching
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun matches(text: String, pattern: String, ignoreCase: Boolean = false): Boolean {
        val regex = compile(pattern, ignoreCase) ?: return false
        return regex.matches(text)
    }
    
    @JvmStatic
    fun containsMatch(text: String, pattern: String, ignoreCase: Boolean = false): Boolean {
        val regex = compile(pattern, ignoreCase) ?: return false
        return regex.containsMatchIn(text)
    }
    
    @JvmStatic
    fun findFirst(text: String, pattern: String, ignoreCase: Boolean = false): String? {
        val regex = compile(pattern, ignoreCase) ?: return null
        return regex.find(text)?.value
    }
    
    @JvmStatic
    fun findAll(text: String, pattern: String, ignoreCase: Boolean = false): List<String> {
        val regex = compile(pattern, ignoreCase) ?: return emptyList()
        return regex.findAll(text).map { it.value }.toList()
    }
    
    @JvmStatic
    fun findAllWithPositions(text: String, pattern: String): List<Map<String, Any>> {
        val regex = compile(pattern) ?: return emptyList()
        return regex.findAll(text).map { match ->
            mapOf(
                "value" to match.value,
                "start" to match.range.first,
                "end" to match.range.last + 1,
                "groups" to match.groupValues
            )
        }.toList()
    }
    
    @JvmStatic
    fun matchGroups(text: String, pattern: String): List<String>? {
        val regex = compile(pattern) ?: return null
        return regex.find(text)?.groupValues
    }
    
    @JvmStatic
    fun matchNamedGroups(text: String, pattern: String): Map<String, String?>? {
        val regex = compile(pattern) ?: return null
        val match = regex.find(text) ?: return null
        
        val result = mutableMapOf<String, String?>()
        match.groups.forEachIndexed { index, group ->
            result["group$index"] = group?.value
        }
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Replace
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun replace(
        text: String,
        pattern: String,
        replacement: String,
        ignoreCase: Boolean = false
    ): String {
        val regex = compile(pattern, ignoreCase) ?: return text
        return regex.replace(text, replacement)
    }
    
    @JvmStatic
    fun replaceFirst(
        text: String,
        pattern: String,
        replacement: String,
        ignoreCase: Boolean = false
    ): String {
        val regex = compile(pattern, ignoreCase) ?: return text
        return regex.replaceFirst(text, replacement)
    }
    
    @JvmStatic
    fun replaceWithTransform(
        text: String,
        pattern: String,
        transform: (String) -> String
    ): String {
        val regex = compile(pattern) ?: return text
        return regex.replace(text) { matchResult ->
            transform(matchResult.value)
        }
    }
    
    @JvmStatic
    fun remove(text: String, pattern: String, ignoreCase: Boolean = false): String {
        return replace(text, pattern, "", ignoreCase)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Split
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun split(text: String, pattern: String, limit: Int = 0): List<String> {
        val regex = compile(pattern) ?: return listOf(text)
        return if (limit > 0) {
            regex.split(text, limit)
        } else {
            regex.split(text)
        }
    }
    
    @JvmStatic
    fun splitToSequence(text: String, pattern: String): Sequence<String> {
        val regex = compile(pattern) ?: return sequenceOf(text)
        return regex.splitToSequence(text)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Common Validations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isEmail(text: String): Boolean {
        return emailPattern.matches(text)
    }
    
    @JvmStatic
    fun isPhone(text: String): Boolean {
        return phonePattern.matches(text.replace(Regex("[\\s\\-()]"), ""))
    }
    
    @JvmStatic
    fun isUrl(text: String): Boolean {
        return urlPattern.matches(text)
    }
    
    @JvmStatic
    fun isNumber(text: String): Boolean {
        return numberPattern.matches(text)
    }
    
    @JvmStatic
    fun isInteger(text: String): Boolean {
        return integerPattern.matches(text)
    }
    
    @JvmStatic
    fun isIpv4(text: String): Boolean {
        return ipv4Pattern.matches(text)
    }
    
    @JvmStatic
    fun isArabicOnly(text: String): Boolean {
        return arabicPattern.matches(text.replace(" ", ""))
    }
    
    @JvmStatic
    fun containsArabic(text: String): Boolean {
        return arabicPattern.containsMatchIn(text)
    }
    
    @JvmStatic
    fun isAlphanumeric(text: String): Boolean {
        return text.matches(Regex("^[a-zA-Z0-9]+$"))
    }
    
    @JvmStatic
    fun isAlpha(text: String): Boolean {
        return text.matches(Regex("^[a-zA-Z]+$"))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Extractors
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun extractEmails(text: String): List<String> {
        return emailPattern.findAll(text).map { it.value }.toList()
    }
    
    @JvmStatic
    fun extractUrls(text: String): List<String> {
        return urlPattern.findAll(text).map { it.value }.toList()
    }
    
    @JvmStatic
    fun extractNumbers(text: String): List<String> {
        return numberPattern.findAll(text).map { it.value }.toList()
    }
    
    @JvmStatic
    fun extractArabicWords(text: String): List<String> {
        return arabicPattern.findAll(text).map { it.value }.toList()
    }
    
    @JvmStatic
    fun extractHashtags(text: String): List<String> {
        val hashtagPattern = Regex("#[\\w\\u0600-\\u06FF]+")
        return hashtagPattern.findAll(text).map { it.value }.toList()
    }
    
    @JvmStatic
    fun extractMentions(text: String): List<String> {
        val mentionPattern = Regex("@[\\w\\u0600-\\u06FF]+")
        return mentionPattern.findAll(text).map { it.value }.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun escape(text: String): String {
        return Pattern.quote(text)
    }
    
    @JvmStatic
    fun countMatches(text: String, pattern: String, ignoreCase: Boolean = false): Int {
        val regex = compile(pattern, ignoreCase) ?: return 0
        return regex.findAll(text).count()
    }
    
    @JvmStatic
    fun getMatchPositions(text: String, pattern: String): List<IntRange> {
        val regex = compile(pattern) ?: return emptyList()
        return regex.findAll(text).map { it.range }.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Arabic-specific Patterns
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getArabicLettersPattern(): String = "[\\u0600-\\u06FF]+"
    
    @JvmStatic
    fun getArabicWithTashkeelPattern(): String = "[\\u0600-\\u06FF\\u064B-\\u065F]+"
    
    @JvmStatic
    fun getTashkeelPattern(): String = "[\\u064B-\\u065F]+"
    
    @JvmStatic
    fun getArabicNumbersPattern(): String = "[٠-٩]+"
    
    @JvmStatic
    fun getArabicPunctuationPattern(): String = "[،؛؟]"
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Pattern Builders
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun wordBoundary(word: String): String {
        return "\\b${escape(word)}\\b"
    }
    
    @JvmStatic
    fun startsWith(prefix: String): String {
        return "^${escape(prefix)}"
    }
    
    @JvmStatic
    fun endsWith(suffix: String): String {
        return "${escape(suffix)}$"
    }
    
    @JvmStatic
    fun anyOf(vararg options: String): String {
        return options.joinToString("|") { escape(it) }
    }
    
    @JvmStatic
    fun optional(pattern: String): String {
        return "(?:$pattern)?"
    }
    
    @JvmStatic
    fun repeat(pattern: String, min: Int, max: Int? = null): String {
        return when {
            max == null -> "(?:$pattern){$min,}"
            min == max -> "(?:$pattern){$min}"
            else -> "(?:$pattern){$min,$max}"
        }
    }
    
    @JvmStatic
    fun group(pattern: String, name: String? = null): String {
        return if (name != null) {
            "(?<$name>$pattern)"
        } else {
            "($pattern)"
        }
    }
    
    @JvmStatic
    fun nonCapturingGroup(pattern: String): String {
        return "(?:$pattern)"
    }
}
