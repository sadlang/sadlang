package sad.android.widgets.config

import android.app.Activity
import android.appwidget.AppWidgetManager
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر إعدادات الويدجت - إدارة تكوين الويدجت
 * Widget Config Bridge - Widget configuration management
 * 
 * @author فريق لغة ص
 */
object SadWidgetConfigBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val configSchemas = ConcurrentHashMap<String, ConfigSchema>()
    private const val PREFS_NAME = "sad_widget_config"
    
    /**
     * مخطط التكوين / Config schema
     */
    data class ConfigSchema(
        val widgetType: String,
        val fields: List<ConfigField>,
        val version: Int = 1
    )
    
    /**
     * حقل التكوين / Config field
     */
    data class ConfigField(
        val key: String,
        val type: FieldType,
        val label: String,
        val defaultValue: Any? = null,
        val required: Boolean = false,
        val options: List<FieldOption>? = null,  // للقوائم المنسدلة
        val min: Number? = null,
        val max: Number? = null,
        val hint: String? = null
    )
    
    /**
     * نوع الحقل / Field type
     */
    enum class FieldType {
        TEXT,
        NUMBER,
        BOOLEAN,
        COLOR,
        DROPDOWN,
        DATE,
        TIME,
        SLIDER
    }
    
    /**
     * خيار الحقل / Field option
     */
    data class FieldOption(
        val value: String,
        val label: String
    )
    
    /**
     * قيم التكوين / Config values
     */
    data class ConfigValues(
        val widgetId: Int,
        val widgetType: String,
        val values: MutableMap<String, Any?> = mutableMapOf()
    )
    
    /**
     * مستمع التكوين / Config listener
     */
    interface ConfigListener {
        fun onConfigSaved(widgetId: Int, values: ConfigValues)
        fun onConfigLoaded(widgetId: Int, values: ConfigValues)
        fun onConfigDeleted(widgetId: Int)
    }
    
    private val configListeners = mutableListOf<ConfigListener>()
    
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
    fun addConfigListener(listener: ConfigListener) {
        if (!configListeners.contains(listener)) {
            configListeners.add(listener)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة المخططات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل مخطط / Register schema
     */
    fun registerSchema(schema: ConfigSchema): Boolean {
        configSchemas[schema.widgetType] = schema
        return true
    }
    
    /**
     * الحصول على مخطط / Get schema
     */
    fun getSchema(widgetType: String): ConfigSchema? {
        return configSchemas[widgetType]
    }
    
    /**
     * بناء مخطط / Schema builder
     */
    class SchemaBuilder(private val widgetType: String) {
        private val fields = mutableListOf<ConfigField>()
        private var version = 1
        
        fun textField(
            key: String,
            label: String,
            defaultValue: String = "",
            required: Boolean = false,
            hint: String? = null
        ): SchemaBuilder {
            fields.add(ConfigField(key, FieldType.TEXT, label, defaultValue, required, hint = hint))
            return this
        }
        
        fun numberField(
            key: String,
            label: String,
            defaultValue: Number = 0,
            min: Number? = null,
            max: Number? = null,
            required: Boolean = false
        ): SchemaBuilder {
            fields.add(ConfigField(key, FieldType.NUMBER, label, defaultValue, required, min = min, max = max))
            return this
        }
        
        fun booleanField(
            key: String,
            label: String,
            defaultValue: Boolean = false
        ): SchemaBuilder {
            fields.add(ConfigField(key, FieldType.BOOLEAN, label, defaultValue))
            return this
        }
        
        fun colorField(
            key: String,
            label: String,
            defaultValue: Int = 0xFF000000.toInt()
        ): SchemaBuilder {
            fields.add(ConfigField(key, FieldType.COLOR, label, defaultValue))
            return this
        }
        
        fun dropdownField(
            key: String,
            label: String,
            options: List<FieldOption>,
            defaultValue: String? = null,
            required: Boolean = false
        ): SchemaBuilder {
            fields.add(ConfigField(key, FieldType.DROPDOWN, label, defaultValue, required, options = options))
            return this
        }
        
        fun sliderField(
            key: String,
            label: String,
            min: Number,
            max: Number,
            defaultValue: Number
        ): SchemaBuilder {
            fields.add(ConfigField(key, FieldType.SLIDER, label, defaultValue, min = min, max = max))
            return this
        }
        
        fun version(v: Int): SchemaBuilder {
            version = v
            return this
        }
        
        fun build(): ConfigSchema {
            return ConfigSchema(widgetType, fields.toList(), version)
        }
    }
    
    /**
     * إنشاء مخطط / Create schema
     */
    fun createSchema(widgetType: String): SchemaBuilder {
        return SchemaBuilder(widgetType)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         حفظ وتحميل
    // ═══════════════════════════════════════════════════════════════
    
    private fun getPrefs(): SharedPreferences? {
        return context?.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    }
    
    /**
     * حفظ التكوين / Save config
     */
    fun saveConfig(widgetId: Int, widgetType: String, values: Map<String, Any?>): Boolean {
        val prefs = getPrefs() ?: return false
        val editor = prefs.edit()
        
        // حفظ نوع الويدجت
        editor.putString("widget_${widgetId}_type", widgetType)
        
        // حفظ القيم
        values.forEach { (key, value) ->
            val prefKey = "widget_${widgetId}_$key"
            when (value) {
                is String -> editor.putString(prefKey, value)
                is Int -> editor.putInt(prefKey, value)
                is Long -> editor.putLong(prefKey, value)
                is Float -> editor.putFloat(prefKey, value)
                is Boolean -> editor.putBoolean(prefKey, value)
                null -> editor.remove(prefKey)
            }
        }
        
        val result = editor.commit()
        
        if (result) {
            val configValues = ConfigValues(widgetId, widgetType, values.toMutableMap())
            mainHandler.post {
                configListeners.forEach { it.onConfigSaved(widgetId, configValues) }
            }
        }
        
        return result
    }
    
    /**
     * تحميل التكوين / Load config
     */
    fun loadConfig(widgetId: Int): ConfigValues? {
        val prefs = getPrefs() ?: return null
        val widgetType = prefs.getString("widget_${widgetId}_type", null) ?: return null
        val schema = configSchemas[widgetType] ?: return null
        
        val values = mutableMapOf<String, Any?>()
        
        schema.fields.forEach { field ->
            val prefKey = "widget_${widgetId}_${field.key}"
            val value = when (field.type) {
                FieldType.TEXT -> prefs.getString(prefKey, field.defaultValue as? String)
                FieldType.NUMBER -> prefs.getInt(prefKey, (field.defaultValue as? Number)?.toInt() ?: 0)
                FieldType.BOOLEAN -> prefs.getBoolean(prefKey, field.defaultValue as? Boolean ?: false)
                FieldType.COLOR -> prefs.getInt(prefKey, (field.defaultValue as? Int) ?: 0xFF000000.toInt())
                FieldType.DROPDOWN -> prefs.getString(prefKey, field.defaultValue as? String)
                FieldType.SLIDER -> prefs.getFloat(prefKey, (field.defaultValue as? Number)?.toFloat() ?: 0f)
                FieldType.DATE, FieldType.TIME -> prefs.getLong(prefKey, (field.defaultValue as? Long) ?: 0L)
            }
            values[field.key] = value
        }
        
        val configValues = ConfigValues(widgetId, widgetType, values)
        mainHandler.post {
            configListeners.forEach { it.onConfigLoaded(widgetId, configValues) }
        }
        
        return configValues
    }
    
    /**
     * حذف التكوين / Delete config
     */
    fun deleteConfig(widgetId: Int): Boolean {
        val prefs = getPrefs() ?: return false
        val widgetType = prefs.getString("widget_${widgetId}_type", null)
        
        val editor = prefs.edit()
        
        // حذف نوع الويدجت
        editor.remove("widget_${widgetId}_type")
        
        // حذف جميع القيم
        if (widgetType != null) {
            val schema = configSchemas[widgetType]
            schema?.fields?.forEach { field ->
                editor.remove("widget_${widgetId}_${field.key}")
            }
        }
        
        val result = editor.commit()
        
        if (result) {
            mainHandler.post {
                configListeners.forEach { it.onConfigDeleted(widgetId) }
            }
        }
        
        return result
    }
    
    /**
     * الحصول على قيمة / Get value
     */
    fun <T> getValue(widgetId: Int, key: String, defaultValue: T): T {
        val config = loadConfig(widgetId) ?: return defaultValue
        @Suppress("UNCHECKED_CAST")
        return (config.values[key] as? T) ?: defaultValue
    }
    
    /**
     * تعيين قيمة / Set value
     */
    fun setValue(widgetId: Int, key: String, value: Any?): Boolean {
        val config = loadConfig(widgetId) ?: return false
        config.values[key] = value
        return saveConfig(widgetId, config.widgetType, config.values)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         نشاط التكوين
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معرف الويدجت من النشاط / Get widget ID from activity
     */
    fun getWidgetIdFromIntent(intent: Intent): Int {
        return intent.getIntExtra(
            AppWidgetManager.EXTRA_APPWIDGET_ID,
            AppWidgetManager.INVALID_APPWIDGET_ID
        )
    }
    
    /**
     * إنهاء نشاط التكوين بنجاح / Finish config activity with success
     */
    fun finishConfigActivity(activity: Activity, widgetId: Int) {
        val resultIntent = Intent().apply {
            putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, widgetId)
        }
        activity.setResult(Activity.RESULT_OK, resultIntent)
        activity.finish()
    }
    
    /**
     * إلغاء نشاط التكوين / Cancel config activity
     */
    fun cancelConfigActivity(activity: Activity, widgetId: Int) {
        val resultIntent = Intent().apply {
            putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, widgetId)
        }
        activity.setResult(Activity.RESULT_CANCELED, resultIntent)
        activity.finish()
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        configSchemas.clear()
        configListeners.clear()
        context = null
    }
}
