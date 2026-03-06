// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDataStoreBridge.kt
// الوصف: جسر Kotlin لتخزين البيانات DataStore
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.*
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * جسر تخزين البيانات — SadDataStoreBridge
 */
object SadDataStoreBridge {
    
    private var context: Context? = null
    private var dataStore: DataStore<Preferences>? = null
    private val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())
    
    private const val DEFAULT_DATASTORE_NAME = "sad_settings"
    
    @JvmStatic
    fun initialize(ctx: Context, name: String = DEFAULT_DATASTORE_NAME) {
        context = ctx.applicationContext
        // DataStore instances are created lazily via extension
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // String Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putString(key: String, value: String, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[stringPreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getString(key: String, defaultValue: String = "", callback: (String) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[stringPreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    @JvmStatic
    fun observeString(key: String, defaultValue: String = ""): Flow<String> {
        val ctx = context ?: return flowOf(defaultValue)
        
        return ctx.dataStore.data
            .catch { emit(emptyPreferences()) }
            .map { preferences ->
                preferences[stringPreferencesKey(key)] ?: defaultValue
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Int Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putInt(key: String, value: Int, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[intPreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getInt(key: String, defaultValue: Int = 0, callback: (Int) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[intPreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    @JvmStatic
    fun observeInt(key: String, defaultValue: Int = 0): Flow<Int> {
        val ctx = context ?: return flowOf(defaultValue)
        
        return ctx.dataStore.data
            .catch { emit(emptyPreferences()) }
            .map { preferences ->
                preferences[intPreferencesKey(key)] ?: defaultValue
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Long Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putLong(key: String, value: Long, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[longPreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getLong(key: String, defaultValue: Long = 0L, callback: (Long) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[longPreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Float/Double Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putFloat(key: String, value: Float, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[floatPreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getFloat(key: String, defaultValue: Float = 0f, callback: (Float) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[floatPreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    @JvmStatic
    fun putDouble(key: String, value: Double, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[doublePreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getDouble(key: String, defaultValue: Double = 0.0, callback: (Double) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[doublePreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Boolean Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putBoolean(key: String, value: Boolean, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[booleanPreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getBoolean(key: String, defaultValue: Boolean = false, callback: (Boolean) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[booleanPreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    @JvmStatic
    fun observeBoolean(key: String, defaultValue: Boolean = false): Flow<Boolean> {
        val ctx = context ?: return flowOf(defaultValue)
        
        return ctx.dataStore.data
            .catch { emit(emptyPreferences()) }
            .map { preferences ->
                preferences[booleanPreferencesKey(key)] ?: defaultValue
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // String Set Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun putStringSet(key: String, value: Set<String>, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences[stringSetPreferencesKey(key)] = value
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun getStringSet(key: String, defaultValue: Set<String> = emptySet(), callback: (Set<String>) -> Unit) {
        val ctx = context ?: run {
            callback(defaultValue)
            return
        }
        
        scope.launch {
            try {
                val value = ctx.dataStore.data.first()[stringSetPreferencesKey(key)]
                withContext(Dispatchers.Main) {
                    callback(value ?: defaultValue)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(defaultValue)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Remove & Clear
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun remove(key: String, callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    // Try all key types
                    preferences.remove(stringPreferencesKey(key))
                    preferences.remove(intPreferencesKey(key))
                    preferences.remove(longPreferencesKey(key))
                    preferences.remove(floatPreferencesKey(key))
                    preferences.remove(doublePreferencesKey(key))
                    preferences.remove(booleanPreferencesKey(key))
                    preferences.remove(stringSetPreferencesKey(key))
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    @JvmStatic
    fun clear(callback: ((Boolean) -> Unit)? = null) {
        val ctx = context ?: run {
            callback?.invoke(false)
            return
        }
        
        scope.launch {
            try {
                ctx.dataStore.edit { preferences ->
                    preferences.clear()
                }
                withContext(Dispatchers.Main) {
                    callback?.invoke(true)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback?.invoke(false)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Contains Key
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun containsKey(key: String, callback: (Boolean) -> Unit) {
        val ctx = context ?: run {
            callback(false)
            return
        }
        
        scope.launch {
            try {
                val preferences = ctx.dataStore.data.first()
                val contains = preferences.contains(stringPreferencesKey(key)) ||
                               preferences.contains(intPreferencesKey(key)) ||
                               preferences.contains(longPreferencesKey(key)) ||
                               preferences.contains(floatPreferencesKey(key)) ||
                               preferences.contains(doublePreferencesKey(key)) ||
                               preferences.contains(booleanPreferencesKey(key)) ||
                               preferences.contains(stringSetPreferencesKey(key))
                
                withContext(Dispatchers.Main) {
                    callback(contains)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(false)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // All Keys
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAllKeys(callback: (Set<String>) -> Unit) {
        val ctx = context ?: run {
            callback(emptySet())
            return
        }
        
        scope.launch {
            try {
                val preferences = ctx.dataStore.data.first()
                val keys = preferences.asMap().keys.map { it.name }.toSet()
                
                withContext(Dispatchers.Main) {
                    callback(keys)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(emptySet())
                }
            }
        }
    }
    
    @JvmStatic
    fun getAllEntries(callback: (Map<String, Any?>) -> Unit) {
        val ctx = context ?: run {
            callback(emptyMap())
            return
        }
        
        scope.launch {
            try {
                val preferences = ctx.dataStore.data.first()
                val entries = preferences.asMap().mapKeys { it.key.name }
                
                withContext(Dispatchers.Main) {
                    callback(entries)
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    callback(emptyMap())
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cleanup
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun cleanup() {
        scope.cancel()
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Extension for DataStore
// ═══════════════════════════════════════════════════════════════════════════════

private val Context.dataStore: DataStore<Preferences> by preferencesDataStore(
    name = "sad_settings"
)
