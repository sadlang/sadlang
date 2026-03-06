// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadStorageBridge.kt
// الوصف: جسر Kotlin للتخزين المحلي في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_تخزين_* بـ Android DataStore/SharedPreferences
//
// الدوال المُنفّذة:
//   __اندرويد_تخزين_احفظ(key, value) -> bool
//   __اندرويد_تخزين_اقرأ(key, default?) -> string
//   __اندرويد_تخزين_احذف(key) -> bool
//   __اندرويد_تخزين_امسح_الكل() -> bool
//
// المتطلبات في build.gradle.kts:
//   implementation("androidx.datastore:datastore-preferences:1.0.0")
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.content.SharedPreferences
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.first
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.*
import androidx.datastore.preferences.preferencesDataStore

// امتداد DataStore للسياق
private val Context.dataStore: DataStore<Preferences> by preferencesDataStore(name = "sad_storage")

/**
 * جسر التخزين — SadStorageBridge
 * 
 * يوفر واجهة للتخزين المحلي من كود لغة ص.
 * يدعم وضعين:
 *   1. SharedPreferences (متزامن، سريع، للقيم البسيطة)
 *   2. DataStore (غير متزامن، آمن، للقيم الكبيرة)
 */
object SadStorageBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد
    // ═══════════════════════════════════════════════════════════════════════════
    
    private const val PREFS_NAME = "sad_prefs"
    
    private var appContext: Context? = null
    private var prefs: SharedPreferences? = null
    
    /**
     * تهيئة الجسر مع السياق
     */
    @JvmStatic
    fun initialize(context: Context) {
        appContext = context.applicationContext
        prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // SharedPreferences — تخزين متزامن
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * __اندرويد_تخزين_احفظ — حفظ قيمة
     * 
     * @param key المفتاح
     * @param value القيمة (ستُحوّل إلى نص)
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun save(key: String, value: Any): Boolean {
        val editor = prefs?.edit() ?: return false
        
        when (value) {
            is String -> editor.putString(key, value)
            is Int -> editor.putInt(key, value)
            is Long -> editor.putLong(key, value)
            is Float -> editor.putFloat(key, value)
            is Double -> editor.putFloat(key, value.toFloat())
            is Boolean -> editor.putBoolean(key, value)
            else -> editor.putString(key, value.toString())
        }
        
        return editor.commit()
    }
    
    /**
     * __اندرويد_تخزين_اقرأ — قراءة قيمة نصية
     * 
     * @param key المفتاح
     * @param default القيمة الافتراضية
     * @return القيمة أو الافتراضية
     */
    @JvmStatic
    fun read(key: String, default: String = ""): String {
        return prefs?.getString(key, default) ?: default
    }
    
    /**
     * قراءة قيمة رقمية
     */
    @JvmStatic
    fun readInt(key: String, default: Int = 0): Int {
        return prefs?.getInt(key, default) ?: default
    }
    
    /**
     * قراءة قيمة منطقية
     */
    @JvmStatic
    fun readBool(key: String, default: Boolean = false): Boolean {
        return prefs?.getBoolean(key, default) ?: default
    }
    
    /**
     * __اندرويد_تخزين_احذف — حذف قيمة
     * 
     * @param key المفتاح
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun delete(key: String): Boolean {
        return prefs?.edit()?.remove(key)?.commit() ?: false
    }
    
    /**
     * __اندرويد_تخزين_امسح_الكل — مسح كل البيانات
     * 
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun clearAll(): Boolean {
        return prefs?.edit()?.clear()?.commit() ?: false
    }
    
    /**
     * التحقق من وجود مفتاح
     */
    @JvmStatic
    fun contains(key: String): Boolean {
        return prefs?.contains(key) ?: false
    }
    
    /**
     * الحصول على جميع المفاتيح
     */
    @JvmStatic
    fun getAllKeys(): Set<String> {
        return prefs?.all?.keys ?: emptySet()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // DataStore — تخزين غير متزامن (موصى به)
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * حفظ قيمة باستخدام DataStore (غير متزامن)
     */
    @JvmStatic
    suspend fun saveAsync(key: String, value: String): Boolean {
        val context = appContext ?: return false
        
        return try {
            context.dataStore.edit { preferences ->
                preferences[stringPreferencesKey(key)] = value
            }
            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
    
    /**
     * قراءة قيمة باستخدام DataStore (غير متزامن)
     */
    @JvmStatic
    suspend fun readAsync(key: String, default: String = ""): String {
        val context = appContext ?: return default
        
        return try {
            val preferences = context.dataStore.data.first()
            preferences[stringPreferencesKey(key)] ?: default
        } catch (e: Exception) {
            e.printStackTrace()
            default
        }
    }
    
    /**
     * حذف قيمة باستخدام DataStore
     */
    @JvmStatic
    suspend fun deleteAsync(key: String): Boolean {
        val context = appContext ?: return false
        
        return try {
            context.dataStore.edit { preferences ->
                preferences.remove(stringPreferencesKey(key))
            }
            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهات Callback للاستخدام من JNI/لغة ص
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * حفظ غير متزامن مع Callback
     */
    @JvmStatic
    fun saveWithCallback(key: String, value: String, callback: (Boolean) -> Unit) {
        CoroutineScope(Dispatchers.IO).launch {
            val result = saveAsync(key, value)
            withContext(Dispatchers.Main) {
                callback(result)
            }
        }
    }
    
    /**
     * قراءة غير متزامنة مع Callback
     */
    @JvmStatic
    fun readWithCallback(key: String, default: String = "", callback: (String) -> Unit) {
        CoroutineScope(Dispatchers.IO).launch {
            val result = readAsync(key, default)
            withContext(Dispatchers.Main) {
                callback(result)
            }
        }
    }
}
