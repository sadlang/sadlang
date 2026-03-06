package sad.android.widgets.data

import android.content.Context
import android.content.SharedPreferences
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap
import org.json.JSONArray
import org.json.JSONObject

/**
 * جسر بيانات الويدجت - تخزين واسترجاع بيانات الويدجت
 * Widget Data Bridge - Widget data storage and retrieval
 * 
 * @author فريق لغة ص
 */
object SadWidgetDataBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private const val PREFS_NAME = "sad_widget_data"
    private val dataChangeListeners = ConcurrentHashMap<Int, MutableList<DataChangeListener>>()
    
    /**
     * مستمع تغيير البيانات / Data change listener
     */
    interface DataChangeListener {
        fun onDataChanged(widgetId: Int, key: String, value: Any?)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addDataChangeListener(widgetId: Int, listener: DataChangeListener) {
        dataChangeListeners.getOrPut(widgetId) { mutableListOf() }.add(listener)
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeDataChangeListener(widgetId: Int, listener: DataChangeListener) {
        dataChangeListeners[widgetId]?.remove(listener)
    }
    
    private fun getPrefs(): SharedPreferences? {
        return context?.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    }
    
    private fun notifyChange(widgetId: Int, key: String, value: Any?) {
        mainHandler.post {
            dataChangeListeners[widgetId]?.forEach { it.onDataChanged(widgetId, key, value) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تخزين بسيط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حفظ نص / Save string
     */
    fun saveString(widgetId: Int, key: String, value: String): Boolean {
        val result = getPrefs()?.edit()?.putString("${widgetId}_$key", value)?.commit() ?: false
        if (result) notifyChange(widgetId, key, value)
        return result
    }
    
    /**
     * تحميل نص / Load string
     */
    fun loadString(widgetId: Int, key: String, defaultValue: String = ""): String {
        return getPrefs()?.getString("${widgetId}_$key", defaultValue) ?: defaultValue
    }
    
    /**
     * حفظ رقم / Save int
     */
    fun saveInt(widgetId: Int, key: String, value: Int): Boolean {
        val result = getPrefs()?.edit()?.putInt("${widgetId}_$key", value)?.commit() ?: false
        if (result) notifyChange(widgetId, key, value)
        return result
    }
    
    /**
     * تحميل رقم / Load int
     */
    fun loadInt(widgetId: Int, key: String, defaultValue: Int = 0): Int {
        return getPrefs()?.getInt("${widgetId}_$key", defaultValue) ?: defaultValue
    }
    
    /**
     * حفظ رقم طويل / Save long
     */
    fun saveLong(widgetId: Int, key: String, value: Long): Boolean {
        val result = getPrefs()?.edit()?.putLong("${widgetId}_$key", value)?.commit() ?: false
        if (result) notifyChange(widgetId, key, value)
        return result
    }
    
    /**
     * تحميل رقم طويل / Load long
     */
    fun loadLong(widgetId: Int, key: String, defaultValue: Long = 0L): Long {
        return getPrefs()?.getLong("${widgetId}_$key", defaultValue) ?: defaultValue
    }
    
    /**
     * حفظ عشري / Save float
     */
    fun saveFloat(widgetId: Int, key: String, value: Float): Boolean {
        val result = getPrefs()?.edit()?.putFloat("${widgetId}_$key", value)?.commit() ?: false
        if (result) notifyChange(widgetId, key, value)
        return result
    }
    
    /**
     * تحميل عشري / Load float
     */
    fun loadFloat(widgetId: Int, key: String, defaultValue: Float = 0f): Float {
        return getPrefs()?.getFloat("${widgetId}_$key", defaultValue) ?: defaultValue
    }
    
    /**
     * حفظ منطقي / Save boolean
     */
    fun saveBoolean(widgetId: Int, key: String, value: Boolean): Boolean {
        val result = getPrefs()?.edit()?.putBoolean("${widgetId}_$key", value)?.commit() ?: false
        if (result) notifyChange(widgetId, key, value)
        return result
    }
    
    /**
     * تحميل منطقي / Load boolean
     */
    fun loadBoolean(widgetId: Int, key: String, defaultValue: Boolean = false): Boolean {
        return getPrefs()?.getBoolean("${widgetId}_$key", defaultValue) ?: defaultValue
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تخزين معقد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حفظ قائمة نصوص / Save string list
     */
    fun saveStringList(widgetId: Int, key: String, values: List<String>): Boolean {
        val json = JSONArray(values).toString()
        return saveString(widgetId, key, json)
    }
    
    /**
     * تحميل قائمة نصوص / Load string list
     */
    fun loadStringList(widgetId: Int, key: String): List<String> {
        val json = loadString(widgetId, key, "[]")
        return try {
            val array = JSONArray(json)
            (0 until array.length()).map { array.getString(it) }
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * حفظ خريطة / Save map
     */
    fun saveMap(widgetId: Int, key: String, values: Map<String, Any?>): Boolean {
        val json = JSONObject(values).toString()
        return saveString(widgetId, key, json)
    }
    
    /**
     * تحميل خريطة / Load map
     */
    fun loadMap(widgetId: Int, key: String): Map<String, Any?> {
        val json = loadString(widgetId, key, "{}")
        return try {
            val obj = JSONObject(json)
            val map = mutableMapOf<String, Any?>()
            obj.keys().forEach { k ->
                map[k] = obj.opt(k)
            }
            map
        } catch (e: Exception) {
            emptyMap()
        }
    }
    
    /**
     * حفظ JSON / Save JSON
     */
    fun saveJson(widgetId: Int, key: String, json: String): Boolean {
        return saveString(widgetId, key, json)
    }
    
    /**
     * تحميل JSON / Load JSON
     */
    fun loadJson(widgetId: Int, key: String): String? {
        val value = loadString(widgetId, key, "")
        return if (value.isNotEmpty()) value else null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حذف قيمة / Delete value
     */
    fun deleteValue(widgetId: Int, key: String): Boolean {
        val result = getPrefs()?.edit()?.remove("${widgetId}_$key")?.commit() ?: false
        if (result) notifyChange(widgetId, key, null)
        return result
    }
    
    /**
     * حذف جميع بيانات الويدجت / Delete all widget data
     */
    fun deleteAllWidgetData(widgetId: Int): Boolean {
        val prefs = getPrefs() ?: return false
        val editor = prefs.edit()
        val prefix = "${widgetId}_"
        
        prefs.all.keys.filter { it.startsWith(prefix) }.forEach { key ->
            editor.remove(key)
        }
        
        val result = editor.commit()
        if (result) {
            dataChangeListeners.remove(widgetId)
        }
        return result
    }
    
    /**
     * التحقق من وجود قيمة / Check if value exists
     */
    fun hasValue(widgetId: Int, key: String): Boolean {
        return getPrefs()?.contains("${widgetId}_$key") ?: false
    }
    
    /**
     * الحصول على جميع مفاتيح الويدجت / Get all widget keys
     */
    fun getAllKeys(widgetId: Int): List<String> {
        val prefs = getPrefs() ?: return emptyList()
        val prefix = "${widgetId}_"
        return prefs.all.keys
            .filter { it.startsWith(prefix) }
            .map { it.removePrefix(prefix) }
    }
    
    /**
     * الحصول على جميع بيانات الويدجت / Get all widget data
     */
    fun getAllData(widgetId: Int): Map<String, Any?> {
        val prefs = getPrefs() ?: return emptyMap()
        val prefix = "${widgetId}_"
        val result = mutableMapOf<String, Any?>()
        
        prefs.all.forEach { (key, value) ->
            if (key.startsWith(prefix)) {
                result[key.removePrefix(prefix)] = value
            }
        }
        
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         نسخ البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نسخ بيانات ويدجت / Copy widget data
     */
    fun copyWidgetData(fromWidgetId: Int, toWidgetId: Int): Boolean {
        val data = getAllData(fromWidgetId)
        if (data.isEmpty()) return false
        
        val prefs = getPrefs() ?: return false
        val editor = prefs.edit()
        
        data.forEach { (key, value) ->
            val newKey = "${toWidgetId}_$key"
            when (value) {
                is String -> editor.putString(newKey, value)
                is Int -> editor.putInt(newKey, value)
                is Long -> editor.putLong(newKey, value)
                is Float -> editor.putFloat(newKey, value)
                is Boolean -> editor.putBoolean(newKey, value)
            }
        }
        
        return editor.commit()
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        dataChangeListeners.clear()
        context = null
    }
}
