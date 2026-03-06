// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadValidationBridge.kt
// الوصف: جسر Kotlin للتحقق من صحة البيانات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.util.Patterns
import java.util.regex.Pattern

/**
 * جسر التحقق — SadValidationBridge
 */
object SadValidationBridge {
    
    // الأنماط الشائعة
    private val arabicPattern = Pattern.compile("[\\u0600-\\u06FF\\u0750-\\u077F\\u08A0-\\u08FF]+")
    private val phonePattern = Pattern.compile("^[+]?[0-9]{10,15}$")
    private val saudiPhonePattern = Pattern.compile("^(\\+966|966|0)?5[0-9]{8}$")
    private val egyptPhonePattern = Pattern.compile("^(\\+20|20|0)?1[0-9]{9}$")
    private val usernamePattern = Pattern.compile("^[a-zA-Z0-9_]{3,20}$")
    private val strongPasswordPattern = Pattern.compile("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@\$!%*?&])[A-Za-z\\d@\$!%*?&]{8,}$")
    private val hexColorPattern = Pattern.compile("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})$")
    private val ipv4Pattern = Pattern.compile("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$")
    
    // البريد الإلكتروني
    @JvmStatic
    fun isValidEmail(email: String): Boolean {
        return Patterns.EMAIL_ADDRESS.matcher(email).matches()
    }
    
    // رقم الهاتف
    @JvmStatic
    fun isValidPhone(phone: String): Boolean {
        return phonePattern.matcher(phone.replace(" ", "").replace("-", "")).matches()
    }
    
    @JvmStatic
    fun isValidSaudiPhone(phone: String): Boolean {
        return saudiPhonePattern.matcher(phone.replace(" ", "").replace("-", "")).matches()
    }
    
    @JvmStatic
    fun isValidEgyptPhone(phone: String): Boolean {
        return egyptPhonePattern.matcher(phone.replace(" ", "").replace("-", "")).matches()
    }
    
    // الروابط
    @JvmStatic
    fun isValidUrl(url: String): Boolean {
        return Patterns.WEB_URL.matcher(url).matches()
    }
    
    // اسم المستخدم وكلمة المرور
    @JvmStatic
    fun isValidUsername(username: String): Boolean {
        return usernamePattern.matcher(username).matches()
    }
    
    @JvmStatic
    fun isValidPassword(password: String, minLength: Int = 6): Boolean {
        return password.length >= minLength
    }
    
    @JvmStatic
    fun isStrongPassword(password: String): Boolean {
        return strongPasswordPattern.matcher(password).matches()
    }
    
    @JvmStatic
    fun getPasswordStrength(password: String): String {
        var score = 0
        
        if (password.length >= 8) score++
        if (password.length >= 12) score++
        if (password.any { it.isLowerCase() }) score++
        if (password.any { it.isUpperCase() }) score++
        if (password.any { it.isDigit() }) score++
        if (password.any { !it.isLetterOrDigit() }) score++
        
        return when {
            score <= 2 -> "ضعيفة"
            score <= 4 -> "متوسطة"
            score <= 5 -> "قوية"
            else -> "قوية جداً"
        }
    }
    
    // الأرقام
    @JvmStatic
    fun isNumeric(value: String): Boolean {
        return value.all { it.isDigit() }
    }
    
    @JvmStatic
    fun isDecimal(value: String): Boolean {
        return try {
            value.toDouble()
            true
        } catch (e: NumberFormatException) {
            false
        }
    }
    
    @JvmStatic
    fun isInRange(value: Int, min: Int, max: Int): Boolean {
        return value in min..max
    }
    
    @JvmStatic
    fun isPositive(value: Number): Boolean = value.toDouble() > 0
    
    @JvmStatic
    fun isNegative(value: Number): Boolean = value.toDouble() < 0
    
    // النصوص
    @JvmStatic
    fun isEmpty(value: String?): Boolean = value.isNullOrEmpty()
    
    @JvmStatic
    fun isBlank(value: String?): Boolean = value.isNullOrBlank()
    
    @JvmStatic
    fun hasMinLength(value: String, minLength: Int): Boolean = value.length >= minLength
    
    @JvmStatic
    fun hasMaxLength(value: String, maxLength: Int): Boolean = value.length <= maxLength
    
    @JvmStatic
    fun isLengthBetween(value: String, min: Int, max: Int): Boolean {
        return value.length in min..max
    }
    
    @JvmStatic
    fun containsArabic(text: String): Boolean {
        return arabicPattern.matcher(text).find()
    }
    
    @JvmStatic
    fun isOnlyArabic(text: String): Boolean {
        return text.replace(" ", "").all { it.code in 0x0600..0x06FF || it.code in 0x0750..0x077F }
    }
    
    @JvmStatic
    fun containsOnlyLetters(text: String): Boolean {
        return text.all { it.isLetter() }
    }
    
    @JvmStatic
    fun containsOnlyLettersAndNumbers(text: String): Boolean {
        return text.all { it.isLetterOrDigit() }
    }
    
    // الشبكة
    @JvmStatic
    fun isValidIPv4(ip: String): Boolean {
        return ipv4Pattern.matcher(ip).matches()
    }
    
    // الألوان
    @JvmStatic
    fun isValidHexColor(color: String): Boolean {
        return hexColorPattern.matcher(color).matches()
    }
    
    // التاريخ
    @JvmStatic
    fun isValidDate(date: String, pattern: String = "yyyy-MM-dd"): Boolean {
        return try {
            val sdf = java.text.SimpleDateFormat(pattern, java.util.Locale.getDefault())
            sdf.isLenient = false
            sdf.parse(date) != null
        } catch (e: Exception) {
            false
        }
    }
    
    // بطاقات الائتمان (خوارزمية Luhn)
    @JvmStatic
    fun isValidCreditCard(number: String): Boolean {
        val digits = number.replace(" ", "").replace("-", "")
        if (!digits.all { it.isDigit() } || digits.length < 13 || digits.length > 19) {
            return false
        }
        
        var sum = 0
        var alternate = false
        
        for (i in digits.length - 1 downTo 0) {
            var n = digits[i].digitToInt()
            
            if (alternate) {
                n *= 2
                if (n > 9) n -= 9
            }
            
            sum += n
            alternate = !alternate
        }
        
        return sum % 10 == 0
    }
    
    // التحقق المخصص
    @JvmStatic
    fun matchesPattern(text: String, regex: String): Boolean {
        return try {
            Pattern.compile(regex).matcher(text).matches()
        } catch (e: Exception) {
            false
        }
    }
    
    // نتيجة التحقق مع رسالة
    @JvmStatic
    fun validateEmail(email: String): Map<String, Any> {
        val isValid = isValidEmail(email)
        return mapOf(
            "valid" to isValid,
            "message" to if (isValid) "بريد إلكتروني صحيح" else "البريد الإلكتروني غير صحيح"
        )
    }
    
    @JvmStatic
    fun validatePhone(phone: String): Map<String, Any> {
        val isValid = isValidPhone(phone)
        return mapOf(
            "valid" to isValid,
            "message" to if (isValid) "رقم هاتف صحيح" else "رقم الهاتف غير صحيح"
        )
    }
    
    @JvmStatic
    fun validatePassword(password: String): Map<String, Any> {
        val strength = getPasswordStrength(password)
        val isValid = password.length >= 6
        
        val messages = mutableListOf<String>()
        if (password.length < 6) messages.add("يجب أن تكون 6 أحرف على الأقل")
        if (!password.any { it.isUpperCase() }) messages.add("يُفضل إضافة حرف كبير")
        if (!password.any { it.isDigit() }) messages.add("يُفضل إضافة رقم")
        if (!password.any { !it.isLetterOrDigit() }) messages.add("يُفضل إضافة رمز خاص")
        
        return mapOf(
            "valid" to isValid,
            "strength" to strength,
            "suggestions" to messages
        )
    }
}
