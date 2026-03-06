// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadValidationBridge.kt
// الوصف: جسر Kotlin للتحقق من صحة البيانات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.text.TextUtils
import android.util.Patterns
import java.util.regex.Pattern

/**
 * جسر التحقق من صحة البيانات — SadValidationBridge
 */
object SadValidationBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // رسائل الخطأ بالعربية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    object ArabicMessages {
        const val REQUIRED = "هذا الحقل مطلوب"
        const val INVALID_EMAIL = "البريد الإلكتروني غير صالح"
        const val INVALID_PHONE = "رقم الهاتف غير صالح"
        const val INVALID_URL = "الرابط غير صالح"
        const val TOO_SHORT = "النص قصير جداً"
        const val TOO_LONG = "النص طويل جداً"
        const val TOO_SMALL = "القيمة صغيرة جداً"
        const val TOO_LARGE = "القيمة كبيرة جداً"
        const val INVALID_FORMAT = "التنسيق غير صالح"
        const val PASSWORDS_MISMATCH = "كلمات المرور غير متطابقة"
        const val WEAK_PASSWORD = "كلمة المرور ضعيفة"
        const val INVALID_DATE = "التاريخ غير صالح"
        const val INVALID_NUMBER = "الرقم غير صالح"
        const val ARABIC_ONLY = "يجب أن يحتوي على أحرف عربية فقط"
        const val LETTERS_ONLY = "يجب أن يحتوي على أحرف فقط"
        const val NUMBERS_ONLY = "يجب أن يحتوي على أرقام فقط"
        const val ALPHANUMERIC_ONLY = "يجب أن يحتوي على أحرف وأرقام فقط"
        const val INVALID_CREDIT_CARD = "رقم البطاقة غير صالح"
        const val INVALID_IBAN = "رقم الآيبان غير صالح"
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // نتيجة التحقق
    // ═══════════════════════════════════════════════════════════════════════════════
    
    data class ValidationResult(
        val isValid: Boolean,
        val errorMessage: String? = null,
        val errorCode: String? = null
    )
    
    @JvmStatic
    fun valid(): ValidationResult = ValidationResult(true)
    
    @JvmStatic
    fun invalid(message: String, code: String? = null): ValidationResult = 
        ValidationResult(false, message, code)
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من النص
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isRequired(value: String?): ValidationResult {
        return if (value.isNullOrBlank()) {
            invalid(ArabicMessages.REQUIRED, "REQUIRED")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isNotEmpty(value: String?): Boolean = !value.isNullOrEmpty()
    
    @JvmStatic
    fun isNotBlank(value: String?): Boolean = !value.isNullOrBlank()
    
    @JvmStatic
    fun minLength(value: String?, min: Int): ValidationResult {
        return if (value == null || value.length < min) {
            invalid("$min ${ArabicMessages.TOO_SHORT} - الحد الأدنى", "MIN_LENGTH")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun maxLength(value: String?, max: Int): ValidationResult {
        return if (value != null && value.length > max) {
            invalid("$max ${ArabicMessages.TOO_LONG} - الحد الأقصى", "MAX_LENGTH")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun lengthBetween(value: String?, min: Int, max: Int): ValidationResult {
        val minResult = minLength(value, min)
        if (!minResult.isValid) return minResult
        return maxLength(value, max)
    }
    
    @JvmStatic
    fun exactLength(value: String?, length: Int): ValidationResult {
        return if (value == null || value.length != length) {
            invalid("يجب أن يكون الطول $length حرفاً", "EXACT_LENGTH")
        } else {
            valid()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من البريد الإلكتروني
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isEmail(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !Patterns.EMAIL_ADDRESS.matcher(value).matches()) {
            invalid(ArabicMessages.INVALID_EMAIL, "INVALID_EMAIL")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isValidEmail(value: String?): Boolean {
        return !value.isNullOrBlank() && Patterns.EMAIL_ADDRESS.matcher(value).matches()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من رقم الهاتف
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isPhone(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !Patterns.PHONE.matcher(value).matches()) {
            invalid(ArabicMessages.INVALID_PHONE, "INVALID_PHONE")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isValidPhone(value: String?): Boolean {
        return !value.isNullOrBlank() && Patterns.PHONE.matcher(value).matches()
    }
    
    private val SAUDI_PHONE_PATTERN = Pattern.compile("^(\\+966|966|05|5)[0-9]{8}$")
    private val EGYPTIAN_PHONE_PATTERN = Pattern.compile("^(\\+20|20|01)[0-9]{9}$")
    private val UAE_PHONE_PATTERN = Pattern.compile("^(\\+971|971|05)[0-9]{8}$")
    
    @JvmStatic
    fun isSaudiPhone(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        val cleaned = value.replace(Regex("[\\s-]"), "")
        return SAUDI_PHONE_PATTERN.matcher(cleaned).matches()
    }
    
    @JvmStatic
    fun isEgyptianPhone(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        val cleaned = value.replace(Regex("[\\s-]"), "")
        return EGYPTIAN_PHONE_PATTERN.matcher(cleaned).matches()
    }
    
    @JvmStatic
    fun isUAEPhone(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        val cleaned = value.replace(Regex("[\\s-]"), "")
        return UAE_PHONE_PATTERN.matcher(cleaned).matches()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من الروابط
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isUrl(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !Patterns.WEB_URL.matcher(value).matches()) {
            invalid(ArabicMessages.INVALID_URL, "INVALID_URL")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isValidUrl(value: String?): Boolean {
        return !value.isNullOrBlank() && Patterns.WEB_URL.matcher(value).matches()
    }
    
    @JvmStatic
    fun isHttpUrl(value: String?): Boolean {
        return !value.isNullOrBlank() && 
               (value.startsWith("http://") || value.startsWith("https://")) &&
               Patterns.WEB_URL.matcher(value).matches()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من الأرقام
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isNumeric(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return value.all { it.isDigit() }
    }
    
    @JvmStatic
    fun isInteger(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return try {
            value.toLong()
            true
        } catch (e: NumberFormatException) {
            false
        }
    }
    
    @JvmStatic
    fun isDecimal(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return try {
            value.toDouble()
            true
        } catch (e: NumberFormatException) {
            false
        }
    }
    
    @JvmStatic
    fun isPositiveNumber(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return try {
            value.toDouble() > 0
        } catch (e: NumberFormatException) {
            false
        }
    }
    
    @JvmStatic
    fun isNegativeNumber(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return try {
            value.toDouble() < 0
        } catch (e: NumberFormatException) {
            false
        }
    }
    
    @JvmStatic
    fun numberInRange(value: Double, min: Double, max: Double): ValidationResult {
        return when {
            value < min -> invalid("${ArabicMessages.TOO_SMALL} - الحد الأدنى $min", "TOO_SMALL")
            value > max -> invalid("${ArabicMessages.TOO_LARGE} - الحد الأقصى $max", "TOO_LARGE")
            else -> valid()
        }
    }
    
    @JvmStatic
    fun intInRange(value: Int, min: Int, max: Int): ValidationResult {
        return when {
            value < min -> invalid("${ArabicMessages.TOO_SMALL} - الحد الأدنى $min", "TOO_SMALL")
            value > max -> invalid("${ArabicMessages.TOO_LARGE} - الحد الأقصى $max", "TOO_LARGE")
            else -> valid()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من كلمة المرور
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isStrongPassword(
        password: String?,
        minLength: Int = 8,
        requireUppercase: Boolean = true,
        requireLowercase: Boolean = true,
        requireDigit: Boolean = true,
        requireSpecialChar: Boolean = true
    ): ValidationResult {
        if (password.isNullOrBlank()) {
            return invalid(ArabicMessages.REQUIRED, "REQUIRED")
        }
        
        if (password.length < minLength) {
            return invalid("كلمة المرور يجب أن تكون $minLength أحرف على الأقل", "TOO_SHORT")
        }
        
        if (requireUppercase && !password.any { it.isUpperCase() }) {
            return invalid("كلمة المرور يجب أن تحتوي على حرف كبير", "NO_UPPERCASE")
        }
        
        if (requireLowercase && !password.any { it.isLowerCase() }) {
            return invalid("كلمة المرور يجب أن تحتوي على حرف صغير", "NO_LOWERCASE")
        }
        
        if (requireDigit && !password.any { it.isDigit() }) {
            return invalid("كلمة المرور يجب أن تحتوي على رقم", "NO_DIGIT")
        }
        
        if (requireSpecialChar && !password.any { !it.isLetterOrDigit() }) {
            return invalid("كلمة المرور يجب أن تحتوي على رمز خاص", "NO_SPECIAL_CHAR")
        }
        
        return valid()
    }
    
    @JvmStatic
    fun passwordsMatch(password: String?, confirmPassword: String?): ValidationResult {
        return if (password != confirmPassword) {
            invalid(ArabicMessages.PASSWORDS_MISMATCH, "PASSWORDS_MISMATCH")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun getPasswordStrength(password: String?): Int {
        if (password.isNullOrBlank()) return 0
        
        var strength = 0
        
        if (password.length >= 8) strength++
        if (password.length >= 12) strength++
        if (password.any { it.isUpperCase() }) strength++
        if (password.any { it.isLowerCase() }) strength++
        if (password.any { it.isDigit() }) strength++
        if (password.any { !it.isLetterOrDigit() }) strength++
        
        return strength.coerceAtMost(5) // 0-5 scale
    }
    
    @JvmStatic
    fun getPasswordStrengthLabel(strength: Int): String {
        return when (strength) {
            0 -> "ضعيفة جداً"
            1 -> "ضعيفة"
            2 -> "متوسطة"
            3 -> "جيدة"
            4 -> "قوية"
            5 -> "قوية جداً"
            else -> "غير معروفة"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من النصوص العربية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private val ARABIC_PATTERN = Pattern.compile("^[\\u0600-\\u06FF\\u0750-\\u077F\\s]+$")
    private val ARABIC_WITH_NUMBERS = Pattern.compile("^[\\u0600-\\u06FF\\u0750-\\u077F\\u0660-\\u0669\\u06F0-\\u06F9\\s0-9]+$")
    
    @JvmStatic
    fun isArabicOnly(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !ARABIC_PATTERN.matcher(value).matches()) {
            invalid(ArabicMessages.ARABIC_ONLY, "ARABIC_ONLY")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isArabicWithNumbers(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return ARABIC_WITH_NUMBERS.matcher(value).matches()
    }
    
    @JvmStatic
    fun containsArabic(value: String?): Boolean {
        if (value.isNullOrBlank()) return false
        return value.any { it.code in 0x0600..0x06FF || it.code in 0x0750..0x077F }
    }
    
    @JvmStatic
    fun isArabicName(value: String?): ValidationResult {
        if (value.isNullOrBlank()) {
            return invalid(ArabicMessages.REQUIRED, "REQUIRED")
        }
        
        if (!ARABIC_PATTERN.matcher(value.trim()).matches()) {
            return invalid("الاسم يجب أن يكون بالعربية فقط", "NOT_ARABIC")
        }
        
        val words = value.trim().split("\\s+".toRegex())
        if (words.size < 2) {
            return invalid("يرجى إدخال الاسم الثنائي على الأقل", "INCOMPLETE_NAME")
        }
        
        return valid()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من الأحرف
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isLettersOnly(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !value.all { it.isLetter() || it.isWhitespace() }) {
            invalid(ArabicMessages.LETTERS_ONLY, "LETTERS_ONLY")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isNumbersOnly(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !value.all { it.isDigit() }) {
            invalid(ArabicMessages.NUMBERS_ONLY, "NUMBERS_ONLY")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun isAlphanumeric(value: String?): ValidationResult {
        return if (value.isNullOrBlank() || !value.all { it.isLetterOrDigit() }) {
            invalid(ArabicMessages.ALPHANUMERIC_ONLY, "ALPHANUMERIC_ONLY")
        } else {
            valid()
        }
    }
    
    @JvmStatic
    fun hasNoWhitespace(value: String?): Boolean {
        if (value.isNullOrBlank()) return true
        return !value.any { it.isWhitespace() }
    }
    
    @JvmStatic
    fun hasNoSpecialChars(value: String?): Boolean {
        if (value.isNullOrBlank()) return true
        return value.all { it.isLetterOrDigit() || it.isWhitespace() }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من البطاقات المالية
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isCreditCard(value: String?): ValidationResult {
        if (value.isNullOrBlank()) {
            return invalid(ArabicMessages.REQUIRED, "REQUIRED")
        }
        
        val cleaned = value.replace(Regex("[\\s-]"), "")
        
        if (cleaned.length !in 13..19 || !cleaned.all { it.isDigit() }) {
            return invalid(ArabicMessages.INVALID_CREDIT_CARD, "INVALID_FORMAT")
        }
        
        // Luhn algorithm
        if (!passesLuhnCheck(cleaned)) {
            return invalid(ArabicMessages.INVALID_CREDIT_CARD, "INVALID_CHECKSUM")
        }
        
        return valid()
    }
    
    private fun passesLuhnCheck(number: String): Boolean {
        var sum = 0
        var alternate = false
        
        for (i in number.length - 1 downTo 0) {
            var digit = number[i] - '0'
            
            if (alternate) {
                digit *= 2
                if (digit > 9) {
                    digit -= 9
                }
            }
            
            sum += digit
            alternate = !alternate
        }
        
        return sum % 10 == 0
    }
    
    @JvmStatic
    fun getCreditCardType(value: String?): String {
        if (value.isNullOrBlank()) return "غير معروف"
        
        val cleaned = value.replace(Regex("[\\s-]"), "")
        
        return when {
            cleaned.startsWith("4") -> "فيزا"
            cleaned.startsWith("5") && cleaned[1] in '1'..'5' -> "ماستركارد"
            cleaned.startsWith("37") || cleaned.startsWith("34") -> "أمريكان إكسبريس"
            cleaned.startsWith("6011") || cleaned.startsWith("65") -> "ديسكفر"
            cleaned.startsWith("5078") || cleaned.startsWith("508") -> "مدى"
            else -> "غير معروف"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من التاريخ
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isValidDate(day: Int, month: Int, year: Int): ValidationResult {
        if (month !in 1..12) {
            return invalid("الشهر يجب أن يكون بين 1 و 12", "INVALID_MONTH")
        }
        
        val maxDays = when (month) {
            1, 3, 5, 7, 8, 10, 12 -> 31
            4, 6, 9, 11 -> 30
            2 -> if (isLeapYear(year)) 29 else 28
            else -> 0
        }
        
        if (day !in 1..maxDays) {
            return invalid("اليوم غير صالح لهذا الشهر", "INVALID_DAY")
        }
        
        return valid()
    }
    
    @JvmStatic
    fun isLeapYear(year: Int): Boolean {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)
    }
    
    @JvmStatic
    fun isValidAge(birthYear: Int, minAge: Int, maxAge: Int = 150): ValidationResult {
        val currentYear = java.util.Calendar.getInstance().get(java.util.Calendar.YEAR)
        val age = currentYear - birthYear
        
        return when {
            age < minAge -> invalid("يجب أن يكون العمر $minAge سنة على الأقل", "TOO_YOUNG")
            age > maxAge -> invalid("العمر غير صالح", "TOO_OLD")
            else -> valid()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق من الأنماط المخصصة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun matchesPattern(value: String?, pattern: String, errorMessage: String = ArabicMessages.INVALID_FORMAT): ValidationResult {
        return try {
            val regex = Pattern.compile(pattern)
            if (value.isNullOrBlank() || !regex.matcher(value).matches()) {
                invalid(errorMessage, "PATTERN_MISMATCH")
            } else {
                valid()
            }
        } catch (e: Exception) {
            invalid("خطأ في النمط", "INVALID_PATTERN")
        }
    }
    
    @JvmStatic
    fun containsPattern(value: String?, pattern: String): Boolean {
        if (value.isNullOrBlank()) return false
        return try {
            Pattern.compile(pattern).matcher(value).find()
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // التحقق المجمع
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun validateAll(vararg validations: ValidationResult): ValidationResult {
        for (validation in validations) {
            if (!validation.isValid) {
                return validation
            }
        }
        return valid()
    }
    
    @JvmStatic
    fun getAllErrors(vararg validations: ValidationResult): List<String> {
        return validations.filter { !it.isValid }.mapNotNull { it.errorMessage }
    }
    
    @JvmStatic
    fun isAllValid(vararg validations: ValidationResult): Boolean {
        return validations.all { it.isValid }
    }
}
