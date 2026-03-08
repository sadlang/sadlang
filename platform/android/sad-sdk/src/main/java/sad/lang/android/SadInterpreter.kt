/**
 * =============================================================================
 * @file SadInterpreter.kt
 * @brief (AR) واجهة المفسر لأندرويد — تنفيذ كود لغة ص
 * @brief (EN) Android Interpreter Interface — Execute Sad code
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

/**
 * مفسر لغة ص للأندرويد
 * يوفر واجهة Kotlin للتفاعل مع المفسر الأصلي (C++)
 */
class SadInterpreter private constructor() {
    
    companion object {
        // Singleton
        @Volatile
        private var instance: SadInterpreter? = null
        
        fun getInstance(): SadInterpreter {
            return instance ?: synchronized(this) {
                instance ?: SadInterpreter().also { instance = it }
            }
        }
        
        init {
            System.loadLibrary("sad_android")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Native Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    private external fun nativeInit(configJson: String): Boolean
    private external fun nativeEvalLine(code: String): String
    private external fun nativeEvalFile(filePath: String): String
    private external fun nativeGetOutput(): String
    private external fun nativeSetVariable(name: String, value: String): Boolean
    private external fun nativeGetVariable(name: String): String
    private external fun nativeCallFunction(funcName: String, args: Array<String>): String
    private external fun nativeShutdown()
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Public API
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var isInitialized = false
    
    /**
     * تهيئة المفسر
     */
    fun init(context: Context, config: SadConfig = SadConfig()): Boolean {
        if (isInitialized) return true
        
        val configJson = config.toJson()
        isInitialized = nativeInit(configJson)
        return isInitialized
    }
    
    /**
     * تنفيذ سطر كود
     */
    fun eval(code: String): SadResult {
        if (!isInitialized) {
            return SadResult.Error("المفسر غير مهيأ")
        }
        
        return try {
            val result = nativeEvalLine(code)
            SadResult.Success(result)
        } catch (e: Exception) {
            SadResult.Error(e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * تنفيذ سطر كود (غير متزامن)
     */
    suspend fun evalAsync(code: String): SadResult = withContext(Dispatchers.Default) {
        eval(code)
    }
    
    /**
     * تنفيذ ملف
     */
    fun evalFile(filePath: String): SadResult {
        if (!isInitialized) {
            return SadResult.Error("المفسر غير مهيأ")
        }
        
        return try {
            val result = nativeEvalFile(filePath)
            SadResult.Success(result)
        } catch (e: Exception) {
            SadResult.Error(e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * الحصول على المخرجات
     */
    fun getOutput(): String {
        return if (isInitialized) nativeGetOutput() else ""
    }
    
    /**
     * تعيين متغير
     */
    fun setVariable(name: String, value: Any): Boolean {
        if (!isInitialized) return false
        return nativeSetVariable(name, value.toString())
    }
    
    /**
     * قراءة متغير
     */
    fun getVariable(name: String): String? {
        if (!isInitialized) return null
        return nativeGetVariable(name)
    }
    
    /**
     * استدعاء دالة
     */
    fun callFunction(funcName: String, vararg args: Any): SadResult {
        if (!isInitialized) {
            return SadResult.Error("المفسر غير مهيأ")
        }
        
        return try {
            val stringArgs = args.map { it.toString() }.toTypedArray()
            val result = nativeCallFunction(funcName, stringArgs)
            SadResult.Success(result)
        } catch (e: Exception) {
            SadResult.Error(e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * إنهاء المفسر
     */
    fun shutdown() {
        if (isInitialized) {
            nativeShutdown()
            isInitialized = false
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Supporting Classes
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إعدادات المفسر
 */
data class SadConfig(
    val enableDebug: Boolean = false,
    val maxExecutionTime: Long = 30000, // ms
    val memoryLimit: Long = 64 * 1024 * 1024, // 64MB
    val enableStdlib: Boolean = true
) {
    fun toJson(): String {
        return """
        {
            "enableDebug": $enableDebug,
            "maxExecutionTime": $maxExecutionTime,
            "memoryLimit": $memoryLimit,
            "enableStdlib": $enableStdlib
        }
        """.trimIndent()
    }
}

/**
 * نتيجة التنفيذ
 */
sealed class SadResult {
    data class Success(val value: String) : SadResult()
    data class Error(val message: String) : SadResult()
    
    fun isSuccess(): Boolean = this is Success
    fun isError(): Boolean = this is Error
    
    fun getOrNull(): String? = (this as? Success)?.value
    fun getOrDefault(default: String): String = getOrNull() ?: default
    fun errorMessage(): String? = (this as? Error)?.message
}
