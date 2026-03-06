// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadJsonBridge.kt
// الوصف: جسر Kotlin لمعالجة JSON في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_json_* بـ org.json و Gson
//
// الدوال المُنفّذة:
//   __اندرويد_json_parse(text) -> object
//   __اندرويد_json_stringify(object) -> string
//   __اندرويد_json_get(object, key) -> value
//   __اندرويد_json_set(object, key, value) -> object
//
// المتطلبات في build.gradle.kts:
//   implementation("com.google.code.gson:gson:2.10.1")
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import com.google.gson.Gson
import com.google.gson.GsonBuilder
import com.google.gson.JsonElement
import com.google.gson.JsonParser
import com.google.gson.JsonObject
import com.google.gson.JsonArray
import com.google.gson.JsonNull
import com.google.gson.JsonPrimitive

/**
 * جسر JSON — SadJsonBridge
 * 
 * يوفر واجهة لتحليل وتوليد JSON من كود لغة ص.
 * يستخدم Gson للأداء العالي والتوافق.
 */
object SadJsonBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد Gson
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val gson: Gson = GsonBuilder()
        .serializeNulls()
        .setPrettyPrinting()
        .create()
    
    private val gsonCompact: Gson = GsonBuilder()
        .serializeNulls()
        .create()
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_json_parse — تحليل JSON
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحليل نص JSON إلى كائن
     * 
     * @param jsonString نص JSON
     * @return Map أو List حسب نوع JSON
     */
    @JvmStatic
    fun parse(jsonString: String): Any? {
        return try {
            val element = JsonParser.parseString(jsonString)
            elementToKotlin(element)
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * تحليل JSON مع إرجاع خطأ إن وجد
     */
    @JvmStatic
    fun parseWithError(jsonString: String): Pair<Any?, String?> {
        return try {
            val element = JsonParser.parseString(jsonString)
            Pair(elementToKotlin(element), null)
        } catch (e: Exception) {
            Pair(null, e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_json_stringify — تحويل إلى JSON
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحويل كائن إلى نص JSON
     * 
     * @param obj الكائن (Map, List, أو قيمة بسيطة)
     * @param pretty تنسيق جميل؟
     * @return نص JSON
     */
    @JvmStatic
    fun stringify(obj: Any?, pretty: Boolean = false): String {
        val element = kotlinToElement(obj)
        return if (pretty) gson.toJson(element) else gsonCompact.toJson(element)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_json_get — قراءة قيمة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة قيمة من كائن JSON
     * 
     * @param obj الكائن (Map)
     * @param key المفتاح (يدعم مسار: "user.name")
     * @return القيمة أو null
     */
    @JvmStatic
    fun get(obj: Any?, key: String): Any? {
        if (obj !is Map<*, *>) return null
        
        val parts = key.split(".")
        var current: Any? = obj
        
        for (part in parts) {
            when (current) {
                is Map<*, *> -> current = current[part]
                is List<*> -> {
                    val index = part.toIntOrNull() ?: return null
                    current = current.getOrNull(index)
                }
                else -> return null
            }
        }
        
        return current
    }
    
    /**
     * قراءة قيمة نصية
     */
    @JvmStatic
    fun getString(obj: Any?, key: String, default: String = ""): String {
        return get(obj, key)?.toString() ?: default
    }
    
    /**
     * قراءة قيمة رقمية
     */
    @JvmStatic
    fun getInt(obj: Any?, key: String, default: Int = 0): Int {
        return when (val value = get(obj, key)) {
            is Number -> value.toInt()
            is String -> value.toIntOrNull() ?: default
            else -> default
        }
    }
    
    /**
     * قراءة قيمة منطقية
     */
    @JvmStatic
    fun getBool(obj: Any?, key: String, default: Boolean = false): Boolean {
        return when (val value = get(obj, key)) {
            is Boolean -> value
            is String -> value.lowercase() == "true"
            else -> default
        }
    }
    
    /**
     * قراءة مصفوفة
     */
    @JvmStatic
    fun getArray(obj: Any?, key: String): List<Any?>? {
        return get(obj, key) as? List<Any?>
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_json_set — كتابة قيمة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * كتابة قيمة في كائن JSON
     * 
     * @param obj الكائن (Map قابل للتعديل)
     * @param key المفتاح
     * @param value القيمة
     * @return الكائن المُعدّل
     */
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun set(obj: Any?, key: String, value: Any?): Any? {
        if (obj !is MutableMap<*, *>) {
            // تحويل إلى mutable
            val mutableObj = when (obj) {
                is Map<*, *> -> obj.toMutableMap()
                null -> mutableMapOf<String, Any?>()
                else -> return obj
            }
            (mutableObj as MutableMap<String, Any?>)[key] = value
            return mutableObj
        }
        
        (obj as MutableMap<String, Any?>)[key] = value
        return obj
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_json_merge — دمج كائنات
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * دمج كائنين JSON
     */
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun merge(obj1: Any?, obj2: Any?): Map<String, Any?> {
        val result = mutableMapOf<String, Any?>()
        
        if (obj1 is Map<*, *>) {
            obj1.forEach { (k, v) -> result[k.toString()] = v }
        }
        
        if (obj2 is Map<*, *>) {
            obj2.forEach { (k, v) -> result[k.toString()] = v }
        }
        
        return result
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال التحويل الداخلية
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحويل JsonElement إلى Kotlin
     */
    private fun elementToKotlin(element: JsonElement): Any? {
        return when {
            element.isJsonNull -> null
            element.isJsonPrimitive -> {
                val primitive = element.asJsonPrimitive
                when {
                    primitive.isBoolean -> primitive.asBoolean
                    primitive.isNumber -> {
                        val num = primitive.asNumber
                        if (num.toDouble() == num.toLong().toDouble()) {
                            num.toLong()
                        } else {
                            num.toDouble()
                        }
                    }
                    else -> primitive.asString
                }
            }
            element.isJsonArray -> {
                element.asJsonArray.map { elementToKotlin(it) }
            }
            element.isJsonObject -> {
                element.asJsonObject.entrySet().associate { (k, v) ->
                    k to elementToKotlin(v)
                }
            }
            else -> null
        }
    }
    
    /**
     * تحويل Kotlin إلى JsonElement
     */
    private fun kotlinToElement(obj: Any?): JsonElement {
        return when (obj) {
            null -> JsonNull.INSTANCE
            is Boolean -> JsonPrimitive(obj)
            is Number -> JsonPrimitive(obj)
            is String -> JsonPrimitive(obj)
            is List<*> -> {
                JsonArray().apply {
                    obj.forEach { add(kotlinToElement(it)) }
                }
            }
            is Map<*, *> -> {
                JsonObject().apply {
                    obj.forEach { (k, v) ->
                        add(k.toString(), kotlinToElement(v))
                    }
                }
            }
            else -> JsonPrimitive(obj.toString())
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء كائن JSON فارغ
     */
    @JvmStatic
    fun createObject(): MutableMap<String, Any?> = mutableMapOf()
    
    /**
     * إنشاء مصفوفة JSON فارغة
     */
    @JvmStatic
    fun createArray(): MutableList<Any?> = mutableListOf()
    
    /**
     * التحقق من صحة JSON
     */
    @JvmStatic
    fun isValid(jsonString: String): Boolean {
        return try {
            JsonParser.parseString(jsonString)
            true
        } catch (e: Exception) {
            false
        }
    }
}
