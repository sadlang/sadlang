// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  محرك لغة ص — Android SDK
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.notes

import android.content.Context
import android.util.Log

/**
 * محرك لغة ص للأندرويد
 * Sad Language Engine for Android
 */
class SadEngine private constructor(context: Context) {
    
    companion object {
        private const val TAG = "SadEngine"
        
        @Volatile
        private var instance: SadEngine? = null
        
        fun getInstance(context: Context): SadEngine {
            return instance ?: synchronized(this) {
                instance ?: SadEngine(context.applicationContext).also { instance = it }
            }
        }
        
        init {
            try {
                System.loadLibrary("sadnotes")
                Log.i(TAG, "تم تحميل مكتبة sadnotes بنجاح")
            } catch (e: UnsatisfiedLinkError) {
                Log.e(TAG, "فشل تحميل مكتبة sadnotes: ${e.message}")
            }
        }
    }
    
    private var isInitialized = false
    
    init {
        initialize()
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // التهيئة والإنهاء
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * تهيئة المحرك
     */
    fun initialize(): Boolean {
        if (isInitialized) return true
        
        return try {
            isInitialized = nativeInit()
            Log.i(TAG, if (isInitialized) "تم تهيئة المحرك" else "فشلت التهيئة")
            isInitialized
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في التهيئة: ${e.message}")
            false
        }
    }
    
    /**
     * إنهاء المحرك وتحرير الموارد
     */
    fun shutdown() {
        if (isInitialized) {
            nativeShutdown()
            isInitialized = false
            instance = null
            Log.i(TAG, "تم إنهاء المحرك")
        }
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // تنفيذ الكود
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * تنفيذ كود لغة ص
     * @param code الكود المراد تنفيذه
     * @return النتيجة كنص
     */
    fun eval(code: String): SadResult {
        return try {
            val result = nativeEval(code)
            SadResult.Success(result)
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في تنفيذ الكود: ${e.message}")
            SadResult.Error(e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * تنفيذ ملف لغة ص
     * @param filepath مسار الملف
     * @return النتيجة
     */
    fun evalFile(filepath: String): SadResult {
        return try {
            val result = nativeEvalFile(filepath)
            SadResult.Success(result)
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في تنفيذ الملف: ${e.message}")
            SadResult.Error(e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * استدعاء دالة معرّفة في لغة ص
     * @param name اسم الدالة
     * @param args المعاملات
     * @return النتيجة
     */
    fun call(name: String, vararg args: String): SadResult {
        return try {
            val result = nativeCall(name, args.toList().toTypedArray())
            SadResult.Success(result)
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في استدعاء الدالة: ${e.message}")
            SadResult.Error(e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * الحصول على نسخة المحرك
     */
    fun getVersion(): String {
        return try {
            nativeGetVersion()
        } catch (e: Exception) {
            "غير معروف"
        }
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // دوال مساعدة
    // ─────────────────────────────────────────────────────────────────────
    
    /**
     * تحويل لون من صيغة hex إلى int
     */
    fun parseColor(hex: String): Int {
        return try {
            nativeParseColor(hex)
        } catch (e: Exception) {
            0xFF000000.toInt() // أسود افتراضي
        }
    }
    
    // ─────────────────────────────────────────────────────────────────────
    // Native Methods
    // ─────────────────────────────────────────────────────────────────────
    
    private external fun nativeInit(): Boolean
    private external fun nativeShutdown()
    private external fun nativeEval(code: String): String
    private external fun nativeEvalFile(filepath: String): String
    private external fun nativeCall(name: String, args: Array<String>): String
    private external fun nativeGetVersion(): String
    private external fun nativeParseColor(hex: String): Int
}

/**
 * نتيجة تنفيذ كود لغة ص
 */
sealed class SadResult {
    data class Success(val value: String) : SadResult()
    data class Error(val message: String) : SadResult()
    
    val isSuccess: Boolean get() = this is Success
    val isError: Boolean get() = this is Error
    
    fun getOrNull(): String? = (this as? Success)?.value
    fun getOrThrow(): String = when (this) {
        is Success -> value
        is Error -> throw RuntimeException(message)
    }
}
