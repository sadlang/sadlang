// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadHttpBridge.kt
// الوصف: جسر Kotlin لطلبات HTTP (OkHttp/Retrofit)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import java.io.*
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLEncoder
import java.nio.charset.StandardCharsets
import java.util.concurrent.Executors
import javax.net.ssl.HttpsURLConnection
import org.json.JSONObject
import org.json.JSONArray

/**
 * جسر HTTP — SadHttpBridge
 */
object SadHttpBridge {
    
    private val executor = Executors.newCachedThreadPool()
    
    private var defaultTimeout = 30000 // 30 seconds
    private var defaultHeaders = mutableMapOf<String, String>()
    private var baseUrl: String? = null
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Configuration
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setBaseUrl(url: String) {
        baseUrl = url.trimEnd('/')
    }
    
    @JvmStatic
    fun setTimeout(milliseconds: Int) {
        defaultTimeout = milliseconds
    }
    
    @JvmStatic
    fun setDefaultHeader(key: String, value: String) {
        defaultHeaders[key] = value
    }
    
    @JvmStatic
    fun setAuthorizationToken(token: String) {
        defaultHeaders["Authorization"] = "Bearer $token"
    }
    
    @JvmStatic
    fun clearDefaultHeaders() {
        defaultHeaders.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Synchronous Requests
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun get(
        url: String,
        headers: Map<String, String>? = null,
        queryParams: Map<String, String>? = null
    ): HttpResponse {
        val fullUrl = buildUrl(url, queryParams)
        return executeRequest("GET", fullUrl, headers, null)
    }
    
    @JvmStatic
    fun post(
        url: String,
        body: String?,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val fullUrl = buildUrl(url, null)
        return executeRequest("POST", fullUrl, headers, body)
    }
    
    @JvmStatic
    fun postJson(
        url: String,
        jsonBody: Map<String, Any>,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val mutableHeaders = (headers?.toMutableMap() ?: mutableMapOf())
        mutableHeaders["Content-Type"] = "application/json"
        
        val json = mapToJson(jsonBody)
        return post(url, json, mutableHeaders)
    }
    
    @JvmStatic
    fun postForm(
        url: String,
        formData: Map<String, String>,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val mutableHeaders = (headers?.toMutableMap() ?: mutableMapOf())
        mutableHeaders["Content-Type"] = "application/x-www-form-urlencoded"
        
        val body = formData.entries.joinToString("&") { (key, value) ->
            "${URLEncoder.encode(key, "UTF-8")}=${URLEncoder.encode(value, "UTF-8")}"
        }
        
        return post(url, body, mutableHeaders)
    }
    
    @JvmStatic
    fun put(
        url: String,
        body: String?,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val fullUrl = buildUrl(url, null)
        return executeRequest("PUT", fullUrl, headers, body)
    }
    
    @JvmStatic
    fun putJson(
        url: String,
        jsonBody: Map<String, Any>,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val mutableHeaders = (headers?.toMutableMap() ?: mutableMapOf())
        mutableHeaders["Content-Type"] = "application/json"
        
        val json = mapToJson(jsonBody)
        return put(url, json, mutableHeaders)
    }
    
    @JvmStatic
    fun patch(
        url: String,
        body: String?,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val fullUrl = buildUrl(url, null)
        return executeRequest("PATCH", fullUrl, headers, body)
    }
    
    @JvmStatic
    fun delete(
        url: String,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val fullUrl = buildUrl(url, null)
        return executeRequest("DELETE", fullUrl, headers, null)
    }
    
    @JvmStatic
    fun head(
        url: String,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val fullUrl = buildUrl(url, null)
        return executeRequest("HEAD", fullUrl, headers, null)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Asynchronous Requests
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAsync(
        url: String,
        headers: Map<String, String>? = null,
        queryParams: Map<String, String>? = null,
        callback: (HttpResponse) -> Unit
    ) {
        executor.execute {
            val response = get(url, headers, queryParams)
            callback(response)
        }
    }
    
    @JvmStatic
    fun postAsync(
        url: String,
        body: String?,
        headers: Map<String, String>? = null,
        callback: (HttpResponse) -> Unit
    ) {
        executor.execute {
            val response = post(url, body, headers)
            callback(response)
        }
    }
    
    @JvmStatic
    fun postJsonAsync(
        url: String,
        jsonBody: Map<String, Any>,
        headers: Map<String, String>? = null,
        callback: (HttpResponse) -> Unit
    ) {
        executor.execute {
            val response = postJson(url, jsonBody, headers)
            callback(response)
        }
    }
    
    @JvmStatic
    fun putAsync(
        url: String,
        body: String?,
        headers: Map<String, String>? = null,
        callback: (HttpResponse) -> Unit
    ) {
        executor.execute {
            val response = put(url, body, headers)
            callback(response)
        }
    }
    
    @JvmStatic
    fun deleteAsync(
        url: String,
        headers: Map<String, String>? = null,
        callback: (HttpResponse) -> Unit
    ) {
        executor.execute {
            val response = delete(url, headers)
            callback(response)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // File Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun downloadFile(
        url: String,
        destinationPath: String,
        progressCallback: ((Long, Long) -> Unit)? = null
    ): HttpResponse {
        var connection: HttpURLConnection? = null
        
        try {
            val fullUrl = buildUrl(url, null)
            connection = URL(fullUrl).openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.connectTimeout = defaultTimeout
            connection.readTimeout = defaultTimeout
            
            applyDefaultHeaders(connection)
            
            val responseCode = connection.responseCode
            val contentLength = connection.contentLengthLong
            
            if (responseCode == HttpURLConnection.HTTP_OK) {
                val file = File(destinationPath)
                file.parentFile?.mkdirs()
                
                connection.inputStream.use { input ->
                    FileOutputStream(file).use { output ->
                        val buffer = ByteArray(8192)
                        var totalBytesRead = 0L
                        var bytesRead: Int
                        
                        while (input.read(buffer).also { bytesRead = it } != -1) {
                            output.write(buffer, 0, bytesRead)
                            totalBytesRead += bytesRead
                            progressCallback?.invoke(totalBytesRead, contentLength)
                        }
                    }
                }
                
                return HttpResponse(
                    statusCode = responseCode,
                    statusMessage = "تم التحميل بنجاح",
                    body = destinationPath,
                    headers = connection.headerFields.mapValues { it.value.joinToString() },
                    success = true
                )
            } else {
                return HttpResponse(
                    statusCode = responseCode,
                    statusMessage = "فشل التحميل",
                    body = readErrorStream(connection),
                    headers = emptyMap(),
                    success = false
                )
            }
        } catch (e: Exception) {
            return HttpResponse(
                statusCode = -1,
                statusMessage = "خطأ: ${e.message}",
                body = null,
                headers = emptyMap(),
                success = false
            )
        } finally {
            connection?.disconnect()
        }
    }
    
    @JvmStatic
    fun uploadFile(
        url: String,
        filePath: String,
        fieldName: String = "file",
        additionalFields: Map<String, String>? = null,
        headers: Map<String, String>? = null
    ): HttpResponse {
        val boundary = "===" + System.currentTimeMillis() + "==="
        var connection: HttpURLConnection? = null
        
        try {
            val file = File(filePath)
            if (!file.exists()) {
                return HttpResponse(
                    statusCode = -1,
                    statusMessage = "الملف غير موجود",
                    body = null,
                    headers = emptyMap(),
                    success = false
                )
            }
            
            val fullUrl = buildUrl(url, null)
            connection = URL(fullUrl).openConnection() as HttpURLConnection
            connection.requestMethod = "POST"
            connection.doOutput = true
            connection.connectTimeout = defaultTimeout
            connection.readTimeout = defaultTimeout
            connection.setRequestProperty("Content-Type", "multipart/form-data; boundary=$boundary")
            
            applyDefaultHeaders(connection)
            headers?.forEach { (key, value) -> connection.setRequestProperty(key, value) }
            
            DataOutputStream(connection.outputStream).use { output ->
                // Additional fields
                additionalFields?.forEach { (key, value) ->
                    output.writeBytes("--$boundary\r\n")
                    output.writeBytes("Content-Disposition: form-data; name=\"$key\"\r\n\r\n")
                    output.writeBytes("$value\r\n")
                }
                
                // File
                output.writeBytes("--$boundary\r\n")
                output.writeBytes("Content-Disposition: form-data; name=\"$fieldName\"; filename=\"${file.name}\"\r\n")
                output.writeBytes("Content-Type: application/octet-stream\r\n\r\n")
                
                FileInputStream(file).use { input ->
                    val buffer = ByteArray(4096)
                    var bytesRead: Int
                    while (input.read(buffer).also { bytesRead = it } != -1) {
                        output.write(buffer, 0, bytesRead)
                    }
                }
                
                output.writeBytes("\r\n--$boundary--\r\n")
            }
            
            return readResponse(connection)
        } catch (e: Exception) {
            return HttpResponse(
                statusCode = -1,
                statusMessage = "خطأ في الرفع: ${e.message}",
                body = null,
                headers = emptyMap(),
                success = false
            )
        } finally {
            connection?.disconnect()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // JSON Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun parseJson(json: String): Map<String, Any>? {
        return try {
            val jsonObject = JSONObject(json)
            jsonObjectToMap(jsonObject)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun parseJsonArray(json: String): List<Map<String, Any>>? {
        return try {
            val jsonArray = JSONArray(json)
            (0 until jsonArray.length()).map { i ->
                jsonObjectToMap(jsonArray.getJSONObject(i))
            }
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun buildUrl(path: String, queryParams: Map<String, String>?): String {
        val fullPath = if (baseUrl != null && !path.startsWith("http")) {
            "${baseUrl}/${path.trimStart('/')}"
        } else {
            path
        }
        
        if (queryParams.isNullOrEmpty()) return fullPath
        
        val queryString = queryParams.entries.joinToString("&") { (key, value) ->
            "${URLEncoder.encode(key, "UTF-8")}=${URLEncoder.encode(value, "UTF-8")}"
        }
        
        return if (fullPath.contains("?")) {
            "$fullPath&$queryString"
        } else {
            "$fullPath?$queryString"
        }
    }
    
    private fun executeRequest(
        method: String,
        url: String,
        headers: Map<String, String>?,
        body: String?
    ): HttpResponse {
        var connection: HttpURLConnection? = null
        
        try {
            connection = URL(url).openConnection() as HttpURLConnection
            connection.requestMethod = method
            connection.connectTimeout = defaultTimeout
            connection.readTimeout = defaultTimeout
            connection.doInput = true
            
            applyDefaultHeaders(connection)
            headers?.forEach { (key, value) -> connection.setRequestProperty(key, value) }
            
            if (body != null && method in listOf("POST", "PUT", "PATCH")) {
                connection.doOutput = true
                OutputStreamWriter(connection.outputStream, StandardCharsets.UTF_8).use { writer ->
                    writer.write(body)
                }
            }
            
            return readResponse(connection)
        } catch (e: Exception) {
            return HttpResponse(
                statusCode = -1,
                statusMessage = "خطأ في الاتصال: ${e.message}",
                body = null,
                headers = emptyMap(),
                success = false
            )
        } finally {
            connection?.disconnect()
        }
    }
    
    private fun applyDefaultHeaders(connection: HttpURLConnection) {
        defaultHeaders.forEach { (key, value) ->
            connection.setRequestProperty(key, value)
        }
    }
    
    private fun readResponse(connection: HttpURLConnection): HttpResponse {
        val statusCode = connection.responseCode
        val success = statusCode in 200..299
        
        val body = if (success) {
            connection.inputStream.bufferedReader().use { it.readText() }
        } else {
            readErrorStream(connection)
        }
        
        val headers = connection.headerFields
            .filterKeys { it != null }
            .mapValues { it.value.joinToString(", ") }
        
        return HttpResponse(
            statusCode = statusCode,
            statusMessage = getStatusMessage(statusCode),
            body = body,
            headers = headers,
            success = success
        )
    }
    
    private fun readErrorStream(connection: HttpURLConnection): String? {
        return try {
            connection.errorStream?.bufferedReader()?.use { it.readText() }
        } catch (e: Exception) {
            null
        }
    }
    
    private fun mapToJson(map: Map<String, Any>): String {
        return JSONObject(map).toString()
    }
    
    private fun jsonObjectToMap(jsonObject: JSONObject): Map<String, Any> {
        val map = mutableMapOf<String, Any>()
        jsonObject.keys().forEach { key ->
            val value = jsonObject.get(key)
            map[key] = when (value) {
                is JSONObject -> jsonObjectToMap(value)
                is JSONArray -> (0 until value.length()).map { i ->
                    val item = value.get(i)
                    if (item is JSONObject) jsonObjectToMap(item) else item
                }
                else -> value
            }
        }
        return map
    }
    
    private fun getStatusMessage(code: Int): String {
        return when (code) {
            200 -> "نجاح"
            201 -> "تم الإنشاء"
            204 -> "لا محتوى"
            301 -> "تحويل دائم"
            302 -> "تحويل مؤقت"
            304 -> "لم يتغير"
            400 -> "طلب غير صالح"
            401 -> "غير مصرح"
            403 -> "ممنوع"
            404 -> "غير موجود"
            405 -> "الطريقة غير مسموحة"
            408 -> "انتهاء الوقت"
            409 -> "تعارض"
            422 -> "كيان غير قابل للمعالجة"
            429 -> "طلبات كثيرة"
            500 -> "خطأ داخلي في الخادم"
            502 -> "بوابة سيئة"
            503 -> "الخدمة غير متاحة"
            504 -> "انتهاء وقت البوابة"
            else -> "رمز الحالة: $code"
        }
    }
    
    /**
     * Response data class
     */
    data class HttpResponse(
        val statusCode: Int,
        val statusMessage: String,
        val body: String?,
        val headers: Map<String, String>,
        val success: Boolean
    )
}
