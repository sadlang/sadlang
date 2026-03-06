// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadPreferencesBridge.kt
// الوصف: جسر Kotlin للتفضيلات البسيطة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.content.SharedPreferences
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey

/**
 * جسر التفضيلات — SadPreferencesBridge
 */
object SadPreferencesBridge {
    
    private var context: Context? = null
    private val preferences = mutableMapOf<String, SharedPreferences>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun open(name: String = "sad_prefs", encrypted: Boolean = false): Boolean {
        val ctx = context ?: return false
        
        if (preferences.containsKey(name)) return true
        
        return try {
            val prefs = if (encrypted) {
                val masterKey = MasterKey.Builder(ctx)
                    .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
                    .build()
                
                EncryptedSharedPreferences.create(
                    ctx,
                    name,
                    masterKey,
                    EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
                    EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
                )
            } else {
                ctx.getSharedPreferences(name, Context.MODE_PRIVATE)
            }
            
            preferences[name] = prefs
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // String
    @JvmStatic
    fun putString(key: String, value: String, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().putString(key, value).commit()
    }
    
    @JvmStatic
    fun getString(key: String, default: String = "", prefsName: String = "sad_prefs"): String {
        val prefs = preferences[prefsName] ?: return default
        return prefs.getString(key, default) ?: default
    }
    
    // Int
    @JvmStatic
    fun putInt(key: String, value: Int, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().putInt(key, value).commit()
    }
    
    @JvmStatic
    fun getInt(key: String, default: Int = 0, prefsName: String = "sad_prefs"): Int {
        val prefs = preferences[prefsName] ?: return default
        return prefs.getInt(key, default)
    }
    
    // Long
    @JvmStatic
    fun putLong(key: String, value: Long, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().putLong(key, value).commit()
    }
    
    @JvmStatic
    fun getLong(key: String, default: Long = 0L, prefsName: String = "sad_prefs"): Long {
        val prefs = preferences[prefsName] ?: return default
        return prefs.getLong(key, default)
    }
    
    // Float
    @JvmStatic
    fun putFloat(key: String, value: Float, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().putFloat(key, value).commit()
    }
    
    @JvmStatic
    fun getFloat(key: String, default: Float = 0f, prefsName: String = "sad_prefs"): Float {
        val prefs = preferences[prefsName] ?: return default
        return prefs.getFloat(key, default)
    }
    
    // Boolean
    @JvmStatic
    fun putBool(key: String, value: Boolean, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().putBoolean(key, value).commit()
    }
    
    @JvmStatic
    fun getBool(key: String, default: Boolean = false, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return default
        return prefs.getBoolean(key, default)
    }
    
    // StringSet
    @JvmStatic
    fun putStringSet(key: String, value: Set<String>, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().putStringSet(key, value).commit()
    }
    
    @JvmStatic
    fun getStringSet(key: String, default: Set<String> = emptySet(), prefsName: String = "sad_prefs"): Set<String> {
        val prefs = preferences[prefsName] ?: return default
        return prefs.getStringSet(key, default) ?: default
    }
    
    // عمليات عامة
    @JvmStatic
    fun contains(key: String, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.contains(key)
    }
    
    @JvmStatic
    fun remove(key: String, prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().remove(key).commit()
    }
    
    @JvmStatic
    fun clear(prefsName: String = "sad_prefs"): Boolean {
        val prefs = preferences[prefsName] ?: return false
        return prefs.edit().clear().commit()
    }
    
    @JvmStatic
    fun getAll(prefsName: String = "sad_prefs"): Map<String, Any?> {
        val prefs = preferences[prefsName] ?: return emptyMap()
        return prefs.all
    }
    
    @JvmStatic
    fun getAllKeys(prefsName: String = "sad_prefs"): Set<String> {
        val prefs = preferences[prefsName] ?: return emptySet()
        return prefs.all.keys
    }
    
    @JvmStatic
    fun count(prefsName: String = "sad_prefs"): Int {
        val prefs = preferences[prefsName] ?: return 0
        return prefs.all.size
    }
}
