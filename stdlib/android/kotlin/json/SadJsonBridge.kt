// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadJsonBridge.kt
// الوصف: جسر Kotlin لمعالجة JSON
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import org.json.JSONArray
import org.json.JSONObject
import org.json.JSONTokener

/**
 * جسر JSON — SadJsonBridge
 */
object SadJsonBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Parsing
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun parse(json: String): Any? {
        return try {
            when (val value = JSONTokener(json).nextValue()) {
                is JSONObject -> jsonObjectToMap(value)
                is JSONArray -> jsonArrayToList(value)
                else -> value
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun parseObject(json: String): Map<String, Any>? {
        return try {
            jsonObjectToMap(JSONObject(json))
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun parseArray(json: String): List<Any>? {
        return try {
            jsonArrayToList(JSONArray(json))
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun isValidJson(json: String): Boolean {
        return try {
            JSONTokener(json).nextValue()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun isJsonObject(json: String): Boolean {
        return try {
            JSONObject(json)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun isJsonArray(json: String): Boolean {
        return try {
            JSONArray(json)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Stringify
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun stringify(obj: Any?): String {
        return when (obj) {
            null -> "null"
            is String -> JSONObject.quote(obj)
            is Number, is Boolean -> obj.toString()
            is Map<*, *> -> mapToJsonObject(obj).toString()
            is List<*> -> listToJsonArray(obj).toString()
            is Array<*> -> listToJsonArray(obj.toList()).toString()
            else -> JSONObject.quote(obj.toString())
        }
    }
    
    @JvmStatic
    fun stringifyPretty(obj: Any?, indent: Int = 2): String {
        return when (obj) {
            null -> "null"
            is String -> JSONObject.quote(obj)
            is Number, is Boolean -> obj.toString()
            is Map<*, *> -> mapToJsonObject(obj).toString(indent)
            is List<*> -> listToJsonArray(obj).toString(indent)
            is Array<*> -> listToJsonArray(obj.toList()).toString(indent)
            else -> JSONObject.quote(obj.toString())
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Object Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createObject(): MutableMap<String, Any> {
        return mutableMapOf()
    }
    
    @JvmStatic
    fun createObject(vararg pairs: Pair<String, Any>): MutableMap<String, Any> {
        return mutableMapOf(*pairs)
    }
    
    @JvmStatic
    fun getValue(json: String, key: String): Any? {
        return try {
            val jsonObject = JSONObject(json)
            if (jsonObject.has(key)) {
                convertJsonValue(jsonObject.get(key))
            } else {
                null
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getString(json: String, key: String, default: String = ""): String {
        return try {
            JSONObject(json).optString(key, default)
        } catch (e: Exception) {
            default
        }
    }
    
    @JvmStatic
    fun getInt(json: String, key: String, default: Int = 0): Int {
        return try {
            JSONObject(json).optInt(key, default)
        } catch (e: Exception) {
            default
        }
    }
    
    @JvmStatic
    fun getLong(json: String, key: String, default: Long = 0L): Long {
        return try {
            JSONObject(json).optLong(key, default)
        } catch (e: Exception) {
            default
        }
    }
    
    @JvmStatic
    fun getDouble(json: String, key: String, default: Double = 0.0): Double {
        return try {
            JSONObject(json).optDouble(key, default)
        } catch (e: Exception) {
            default
        }
    }
    
    @JvmStatic
    fun getBoolean(json: String, key: String, default: Boolean = false): Boolean {
        return try {
            JSONObject(json).optBoolean(key, default)
        } catch (e: Exception) {
            default
        }
    }
    
    @JvmStatic
    fun getObject(json: String, key: String): Map<String, Any>? {
        return try {
            val jsonObject = JSONObject(json)
            if (jsonObject.has(key) && !jsonObject.isNull(key)) {
                jsonObjectToMap(jsonObject.getJSONObject(key))
            } else {
                null
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getArray(json: String, key: String): List<Any>? {
        return try {
            val jsonObject = JSONObject(json)
            if (jsonObject.has(key) && !jsonObject.isNull(key)) {
                jsonArrayToList(jsonObject.getJSONArray(key))
            } else {
                null
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun hasKey(json: String, key: String): Boolean {
        return try {
            JSONObject(json).has(key)
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getKeys(json: String): List<String> {
        return try {
            JSONObject(json).keys().asSequence().toList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Array Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createArray(): MutableList<Any> {
        return mutableListOf()
    }
    
    @JvmStatic
    fun createArray(vararg items: Any): MutableList<Any> {
        return mutableListOf(*items)
    }
    
    @JvmStatic
    fun getArrayLength(json: String): Int {
        return try {
            JSONArray(json).length()
        } catch (e: Exception) {
            0
        }
    }
    
    @JvmStatic
    fun getArrayItem(json: String, index: Int): Any? {
        return try {
            val array = JSONArray(json)
            if (index >= 0 && index < array.length()) {
                convertJsonValue(array.get(index))
            } else {
                null
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getArrayObjectAt(json: String, index: Int): Map<String, Any>? {
        return try {
            val array = JSONArray(json)
            if (index >= 0 && index < array.length()) {
                jsonObjectToMap(array.getJSONObject(index))
            } else {
                null
            }
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Modification
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setValueInObject(json: String, key: String, value: Any?): String {
        return try {
            val jsonObject = JSONObject(json)
            jsonObject.put(key, convertToJsonValue(value))
            jsonObject.toString()
        } catch (e: Exception) {
            json
        }
    }
    
    @JvmStatic
    fun removeFromObject(json: String, key: String): String {
        return try {
            val jsonObject = JSONObject(json)
            jsonObject.remove(key)
            jsonObject.toString()
        } catch (e: Exception) {
            json
        }
    }
    
    @JvmStatic
    fun appendToArray(json: String, value: Any?): String {
        return try {
            val array = JSONArray(json)
            array.put(convertToJsonValue(value))
            array.toString()
        } catch (e: Exception) {
            json
        }
    }
    
    @JvmStatic
    fun setArrayItem(json: String, index: Int, value: Any?): String {
        return try {
            val array = JSONArray(json)
            if (index >= 0 && index < array.length()) {
                array.put(index, convertToJsonValue(value))
            }
            array.toString()
        } catch (e: Exception) {
            json
        }
    }
    
    @JvmStatic
    fun removeFromArray(json: String, index: Int): String {
        return try {
            val array = JSONArray(json)
            if (index >= 0 && index < array.length()) {
                array.remove(index)
            }
            array.toString()
        } catch (e: Exception) {
            json
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Merge
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun mergeObjects(json1: String, json2: String): String {
        return try {
            val obj1 = JSONObject(json1)
            val obj2 = JSONObject(json2)
            
            obj2.keys().forEach { key ->
                obj1.put(key, obj2.get(key))
            }
            
            obj1.toString()
        } catch (e: Exception) {
            "{}"
        }
    }
    
    @JvmStatic
    fun concatArrays(json1: String, json2: String): String {
        return try {
            val arr1 = JSONArray(json1)
            val arr2 = JSONArray(json2)
            
            for (i in 0 until arr2.length()) {
                arr1.put(arr2.get(i))
            }
            
            arr1.toString()
        } catch (e: Exception) {
            "[]"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Query (JSONPath-like)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun query(json: String, path: String): Any? {
        return try {
            val parts = path.split(".")
            var current: Any = JSONTokener(json).nextValue()
            
            for (part in parts) {
                current = when {
                    part.matches(Regex("\\w+\\[\\d+\\]")) -> {
                        // Array access like "items[0]"
                        val name = part.substringBefore("[")
                        val index = part.substringAfter("[").substringBefore("]").toInt()
                        val obj = (current as JSONObject).getJSONArray(name)
                        obj.get(index)
                    }
                    part.matches(Regex("\\[\\d+\\]")) -> {
                        // Direct array index like "[0]"
                        val index = part.substringAfter("[").substringBefore("]").toInt()
                        (current as JSONArray).get(index)
                    }
                    else -> {
                        (current as JSONObject).get(part)
                    }
                }
            }
            
            convertJsonValue(current)
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Arabic Support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getFieldArabic(json: String, arabicKey: String): Any? {
        return getValue(json, arabicKey)
    }
    
    @JvmStatic
    fun createArabicObject(vararg pairs: Pair<String, Any>): String {
        val map = mapOf(*pairs)
        return stringify(map)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Internal Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun jsonObjectToMap(jsonObject: JSONObject): Map<String, Any> {
        val map = mutableMapOf<String, Any>()
        jsonObject.keys().forEach { key ->
            map[key] = convertJsonValue(jsonObject.get(key))
        }
        return map
    }
    
    private fun jsonArrayToList(jsonArray: JSONArray): List<Any> {
        return (0 until jsonArray.length()).map { i ->
            convertJsonValue(jsonArray.get(i))
        }
    }
    
    private fun convertJsonValue(value: Any): Any {
        return when (value) {
            is JSONObject -> jsonObjectToMap(value)
            is JSONArray -> jsonArrayToList(value)
            JSONObject.NULL -> "null"
            else -> value
        }
    }
    
    private fun convertToJsonValue(value: Any?): Any {
        return when (value) {
            null -> JSONObject.NULL
            is Map<*, *> -> mapToJsonObject(value)
            is List<*> -> listToJsonArray(value)
            is Array<*> -> listToJsonArray(value.toList())
            else -> value
        }
    }
    
    private fun mapToJsonObject(map: Map<*, *>): JSONObject {
        val jsonObject = JSONObject()
        map.forEach { (key, value) ->
            jsonObject.put(key.toString(), convertToJsonValue(value))
        }
        return jsonObject
    }
    
    private fun listToJsonArray(list: List<*>): JSONArray {
        val jsonArray = JSONArray()
        list.forEach { item ->
            jsonArray.put(convertToJsonValue(item))
        }
        return jsonArray
    }
}
