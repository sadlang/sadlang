// ═══════════════════════════════════════════════════════════════════════════
// SadRuntime.kt - وقت تشغيل لغة ص لأندرويد
// ═══════════════════════════════════════════════════════════════════════════
//
// هذا الملف يمثل الجسر بين Kotlin والمكتبة الأصلية لتشغيل كود ص
//
// ═══════════════════════════════════════════════════════════════════════════

package sad.android.runtime

import android.content.Context
import android.content.res.AssetManager
import android.util.Log

/**
 * نتيجة تنفيذ كود ص
 */
data class ExecutionResult(
    val success: Boolean,
    val output: String = "",
    val error: String = ""
)

/**
 * وقت تشغيل لغة ص - Singleton
 */
object SadRuntime {
    private const val TAG = "SadRuntime"
    
    private var initialized = false
    private lateinit var applicationContext: Context
    
    /**
     * تحميل المكتبة الأصلية
     */
    init {
        try {
            System.loadLibrary("sad_android")
            Log.i(TAG, "تم تحميل مكتبة sad_android بنجاح")
        } catch (e: UnsatisfiedLinkError) {
            Log.e(TAG, "فشل تحميل مكتبة sad_android: ${e.message}")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // الدوال الأصلية (JNI)
    // ═══════════════════════════════════════════════════════════════════════
    
    private external fun nativeInit(context: Context, assetManager: AssetManager): Boolean
    private external fun nativeShutdown()
    private external fun nativeExecuteFile(filePath: String): String
    private external fun nativeExecuteCode(code: String): String
    private external fun nativeCallFunction(functionName: String, args: Array<String>): String
    private external fun nativeSetVariable(name: String, value: String)
    private external fun nativeGetVariable(name: String): String
    private external fun nativeGetUIDescription(): String
    private external fun nativeUpdateUI(uiJson: String)
    
    // ═══════════════════════════════════════════════════════════════════════
    // التهيئة والإيقاف
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة Runtime
     * يجب استدعاء هذه الدالة قبل أي عملية أخرى
     */
    fun initialize(context: Context): Boolean {
        if (initialized) {
            Log.w(TAG, "Runtime مُهيأ مسبقاً")
            return true
        }
        
        applicationContext = context.applicationContext
        
        return try {
            initialized = nativeInit(applicationContext, applicationContext.assets)
            if (initialized) {
                Log.i(TAG, "تم تهيئة Runtime بنجاح")
            } else {
                Log.e(TAG, "فشل تهيئة Runtime")
            }
            initialized
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في التهيئة: ${e.message}")
            false
        }
    }
    
    /**
     * إيقاف Runtime وتحرير الموارد
     */
    fun shutdown() {
        if (!initialized) return
        
        try {
            nativeShutdown()
            initialized = false
            Log.i(TAG, "تم إيقاف Runtime")
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في الإيقاف: ${e.message}")
        }
    }
    
    /**
     * التحقق من التهيئة
     */
    fun isInitialized(): Boolean = initialized
    
    // ═══════════════════════════════════════════════════════════════════════
    // تنفيذ الكود
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ ملف ص من assets
     * @param fileName اسم الملف (مثال: "main.ص")
     */
    fun executeFile(fileName: String): ExecutionResult {
        ensureInitialized()
        
        return try {
            val resultJson = nativeExecuteFile(fileName)
            parseResult(resultJson)
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في تنفيذ الملف: ${e.message}")
            ExecutionResult(false, error = e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * تنفيذ كود ص مباشرة
     * @param code الكود المراد تنفيذه
     */
    fun executeCode(code: String): ExecutionResult {
        ensureInitialized()
        
        return try {
            val resultJson = nativeExecuteCode(code)
            parseResult(resultJson)
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في تنفيذ الكود: ${e.message}")
            ExecutionResult(false, error = e.message ?: "خطأ غير معروف")
        }
    }
    
    /**
     * استدعاء دالة في كود ص
     * @param functionName اسم الدالة
     * @param args وسائط الدالة
     */
    fun callFunction(functionName: String, vararg args: String): ExecutionResult {
        ensureInitialized()
        
        return try {
            val resultJson = nativeCallFunction(functionName, args.toList().toTypedArray())
            parseResult(resultJson)
        } catch (e: Exception) {
            Log.e(TAG, "خطأ في استدعاء الدالة: ${e.message}")
            ExecutionResult(false, error = e.message ?: "خطأ غير معروف")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // إدارة المتغيرات
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * تعيين قيمة متغير
     */
    fun setVariable(name: String, value: Any) {
        ensureInitialized()
        nativeSetVariable(name, value.toString())
    }
    
    /**
     * الحصول على قيمة متغير
     */
    fun getVariable(name: String): String {
        ensureInitialized()
        return nativeGetVariable(name)
    }
    
    /**
     * الحصول على قيمة متغير كرقم
     */
    fun getVariableAsInt(name: String, default: Int = 0): Int {
        return getVariable(name).toIntOrNull() ?: default
    }
    
    /**
     * الحصول على قيمة متغير كرقم عشري
     */
    fun getVariableAsDouble(name: String, default: Double = 0.0): Double {
        return getVariable(name).toDoubleOrNull() ?: default
    }
    
    /**
     * الحصول على قيمة متغير كمنطقي
     */
    fun getVariableAsBool(name: String, default: Boolean = false): Boolean {
        val value = getVariable(name).lowercase()
        return when (value) {
            "true", "صحيح", "1" -> true
            "false", "خطأ", "0" -> false
            else -> default
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // UI
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على وصف UI من كود ص
     */
    fun getUIDescription(): String {
        ensureInitialized()
        return nativeGetUIDescription()
    }
    
    /**
     * تحديث UI
     */
    fun updateUI(uiJson: String) {
        ensureInitialized()
        nativeUpdateUI(uiJson)
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═══════════════════════════════════════════════════════════════════════
    
    private fun ensureInitialized() {
        if (!initialized) {
            throw IllegalStateException("Runtime غير مُهيأ. استدعِ initialize() أولاً")
        }
    }
    
    private fun parseResult(json: String): ExecutionResult {
        // تحليل JSON بسيط
        return try {
            val successMatch = Regex(""""success"\s*:\s*(true|false)""").find(json)
            val outputMatch = Regex(""""output"\s*:\s*"([^"]*)"""").find(json)
            val errorMatch = Regex(""""error"\s*:\s*"([^"]*)"""").find(json)
            
            val success = successMatch?.groupValues?.get(1) == "true"
            val output = outputMatch?.groupValues?.get(1) ?: ""
            val error = errorMatch?.groupValues?.get(1) ?: ""
            
            ExecutionResult(success, output, error)
        } catch (e: Exception) {
            ExecutionResult(false, error = "خطأ في تحليل النتيجة: ${e.message}")
        }
    }
}

/**
 * Extension functions لسهولة الاستخدام
 */
fun Context.initSadRuntime(): Boolean = SadRuntime.initialize(this)

fun String.executeSadCode(): ExecutionResult = SadRuntime.executeCode(this)

fun String.callSadFunction(vararg args: String): ExecutionResult = 
    SadRuntime.callFunction(this, *args)
