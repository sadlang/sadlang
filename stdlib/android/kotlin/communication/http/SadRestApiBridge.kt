package sad.android.communication.http

import android.os.Handler
import android.os.Looper
import java.io.*
import java.net.HttpURLConnection
import java.net.URL
import java.net.URLEncoder
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import javax.net.ssl.HttpsURLConnection

/**
 * جسر REST API - طلبات HTTP RESTful
 * REST API Bridge - RESTful HTTP requests
 * 
 * @author فريق لغة ص
 */
object SadRestApiBridge {
    
    private val executor: ExecutorService = Executors.newFixedThreadPool(4)
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * طرق HTTP / HTTP methods
     */
    enum class HttpMethod {
        GET, POST, PUT, PATCH, DELETE, HEAD, OPTIONS
    }
    
    /**
     * طلب HTTP / HTTP request
     */
    data class HttpRequest(
        val url: String,
        val method: HttpMethod = HttpMethod.GET,
        val headers: Map<String, String> = emptyMap(),
        val params: Map<String, String> = emptyMap(),
        val body: String? = null,
        val contentType: String = "application/json",
        val connectTimeout: Int = 15000,
        val readTimeout: Int = 30000,
        val followRedirects: Boolean = true
    )
    
    /**
     * استجابة HTTP / HTTP response
     */
    data class HttpResponse(
        val statusCode: Int,
        val statusMessage: String,
        val headers: Map<String, String>,
        val body: String?,
        val bodyBytes: ByteArray? = null,
        val isSuccess: Boolean,
        val error: String? = null,
        val requestTime: Long = 0
    ) {
        override fun equals(other: Any?): Boolean {
            if (this === other) return true
            if (javaClass != other?.javaClass) return false
            other as HttpResponse
            return statusCode == other.statusCode && body == other.body
        }
        override fun hashCode(): Int = 31 * statusCode + (body?.hashCode() ?: 0)
    }
    
    /**
     * مستمع الطلب / Request listener
     */
    interface RequestCallback {
        fun onSuccess(response: HttpResponse)
        fun onError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلبات GET
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب GET متزامن / Synchronous GET request
     */
    fun get(url: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.GET, headers = headers))
    }
    
    /**
     * طلب GET غير متزامن / Asynchronous GET request
     */
    fun getAsync(url: String, headers: Map<String, String> = emptyMap(), callback: RequestCallback) {
        executeAsync(HttpRequest(url = url, method = HttpMethod.GET, headers = headers), callback)
    }
    
    /**
     * طلب GET مع بارامترات / GET with query parameters
     */
    fun getWithParams(url: String, params: Map<String, String>, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.GET, params = params, headers = headers))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلبات POST
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب POST متزامن / Synchronous POST request
     */
    fun post(url: String, body: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.POST, body = body, headers = headers))
    }
    
    /**
     * طلب POST غير متزامن / Asynchronous POST request
     */
    fun postAsync(url: String, body: String, headers: Map<String, String> = emptyMap(), callback: RequestCallback) {
        executeAsync(HttpRequest(url = url, method = HttpMethod.POST, body = body, headers = headers), callback)
    }
    
    /**
     * إرسال JSON / Post JSON
     */
    fun postJson(url: String, json: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        val allHeaders = headers + ("Content-Type" to "application/json")
        return execute(HttpRequest(url = url, method = HttpMethod.POST, body = json, headers = allHeaders))
    }
    
    /**
     * إرسال Form Data / Post form data
     */
    fun postForm(url: String, formData: Map<String, String>, headers: Map<String, String> = emptyMap()): HttpResponse {
        val body = formData.entries.joinToString("&") { (k, v) ->
            "${URLEncoder.encode(k, "UTF-8")}=${URLEncoder.encode(v, "UTF-8")}"
        }
        val allHeaders = headers + ("Content-Type" to "application/x-www-form-urlencoded")
        return execute(HttpRequest(url = url, method = HttpMethod.POST, body = body, headers = allHeaders))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلبات PUT
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب PUT / PUT request
     */
    fun put(url: String, body: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.PUT, body = body, headers = headers))
    }
    
    /**
     * طلب PUT غير متزامن / Asynchronous PUT request
     */
    fun putAsync(url: String, body: String, headers: Map<String, String> = emptyMap(), callback: RequestCallback) {
        executeAsync(HttpRequest(url = url, method = HttpMethod.PUT, body = body, headers = headers), callback)
    }
    
    /**
     * تحديث JSON / Put JSON
     */
    fun putJson(url: String, json: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        val allHeaders = headers + ("Content-Type" to "application/json")
        return execute(HttpRequest(url = url, method = HttpMethod.PUT, body = json, headers = allHeaders))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلبات PATCH
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب PATCH / PATCH request
     */
    fun patch(url: String, body: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.PATCH, body = body, headers = headers))
    }
    
    /**
     * طلب PATCH غير متزامن / Asynchronous PATCH request
     */
    fun patchAsync(url: String, body: String, headers: Map<String, String> = emptyMap(), callback: RequestCallback) {
        executeAsync(HttpRequest(url = url, method = HttpMethod.PATCH, body = body, headers = headers), callback)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلبات DELETE
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب DELETE / DELETE request
     */
    fun delete(url: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.DELETE, headers = headers))
    }
    
    /**
     * طلب DELETE غير متزامن / Asynchronous DELETE request
     */
    fun deleteAsync(url: String, headers: Map<String, String> = emptyMap(), callback: RequestCallback) {
        executeAsync(HttpRequest(url = url, method = HttpMethod.DELETE, headers = headers), callback)
    }
    
    /**
     * طلب DELETE مع body / DELETE with body
     */
    fun deleteWithBody(url: String, body: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.DELETE, body = body, headers = headers))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         طلبات أخرى
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب HEAD / HEAD request
     */
    fun head(url: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.HEAD, headers = headers))
    }
    
    /**
     * طلب OPTIONS / OPTIONS request
     */
    fun options(url: String, headers: Map<String, String> = emptyMap()): HttpResponse {
        return execute(HttpRequest(url = url, method = HttpMethod.OPTIONS, headers = headers))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنفيذ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ طلب / Execute request
     */
    fun execute(request: HttpRequest): HttpResponse {
        val startTime = System.currentTimeMillis()
        var connection: HttpURLConnection? = null
        
        try {
            // بناء URL مع البارامترات
            val fullUrl = if (request.params.isNotEmpty() && request.method == HttpMethod.GET) {
                val queryString = request.params.entries.joinToString("&") { (k, v) ->
                    "${URLEncoder.encode(k, "UTF-8")}=${URLEncoder.encode(v, "UTF-8")}"
                }
                if (request.url.contains("?")) "${request.url}&$queryString"
                else "${request.url}?$queryString"
            } else {
                request.url
            }
            
            val url = URL(fullUrl)
            connection = url.openConnection() as HttpURLConnection
            
            // إعدادات الاتصال
            connection.requestMethod = if (request.method == HttpMethod.PATCH) "POST" else request.method.name
            connection.connectTimeout = request.connectTimeout
            connection.readTimeout = request.readTimeout
            connection.instanceFollowRedirects = request.followRedirects
            connection.doInput = true
            
            // PATCH workaround
            if (request.method == HttpMethod.PATCH) {
                connection.setRequestProperty("X-HTTP-Method-Override", "PATCH")
            }
            
            // الترويسات
            request.headers.forEach { (key, value) ->
                connection.setRequestProperty(key, value)
            }
            
            // إرسال body إذا وجد
            if (request.body != null && request.method in listOf(HttpMethod.POST, HttpMethod.PUT, HttpMethod.PATCH, HttpMethod.DELETE)) {
                connection.doOutput = true
                if (!request.headers.containsKey("Content-Type")) {
                    connection.setRequestProperty("Content-Type", request.contentType)
                }
                
                connection.outputStream.bufferedWriter().use { writer ->
                    writer.write(request.body)
                }
            }
            
            // قراءة الاستجابة
            val responseCode = connection.responseCode
            val responseMessage = connection.responseMessage ?: ""
            
            val responseHeaders = mutableMapOf<String, String>()
            connection.headerFields.forEach { (key, values) ->
                if (key != null && values.isNotEmpty()) {
                    responseHeaders[key] = values.joinToString(", ")
                }
            }
            
            val responseBody = if (responseCode in 200..299) {
                connection.inputStream.bufferedReader().readText()
            } else {
                connection.errorStream?.bufferedReader()?.readText()
            }
            
            val requestTime = System.currentTimeMillis() - startTime
            
            return HttpResponse(
                statusCode = responseCode,
                statusMessage = responseMessage,
                headers = responseHeaders,
                body = responseBody,
                isSuccess = responseCode in 200..299,
                requestTime = requestTime
            )
            
        } catch (e: Exception) {
            return HttpResponse(
                statusCode = -1,
                statusMessage = "",
                headers = emptyMap(),
                body = null,
                isSuccess = false,
                error = e.message,
                requestTime = System.currentTimeMillis() - startTime
            )
        } finally {
            connection?.disconnect()
        }
    }
    
    /**
     * تنفيذ طلب غير متزامن / Execute async request
     */
    fun executeAsync(request: HttpRequest, callback: RequestCallback) {
        executor.execute {
            val response = execute(request)
            mainHandler.post {
                if (response.isSuccess) {
                    callback.onSuccess(response)
                } else {
                    callback.onError(response.error ?: "HTTP ${response.statusCode}: ${response.statusMessage}")
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المصادقة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة ترويسة Bearer Token / Add Bearer token header
     */
    fun withBearerToken(token: String): Map<String, String> {
        return mapOf("Authorization" to "Bearer $token")
    }
    
    /**
     * إضافة ترويسة Basic Auth / Add Basic auth header
     */
    fun withBasicAuth(username: String, password: String): Map<String, String> {
        val credentials = "$username:$password"
        val encoded = android.util.Base64.encodeToString(credentials.toByteArray(), android.util.Base64.NO_WRAP)
        return mapOf("Authorization" to "Basic $encoded")
    }
    
    /**
     * إضافة API Key / Add API key
     */
    fun withApiKey(key: String, headerName: String = "X-API-Key"): Map<String, String> {
        return mapOf(headerName to key)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات JSON
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحويل Map إلى JSON / Convert Map to JSON
     */
    fun mapToJson(map: Map<String, Any?>): String {
        val entries = map.entries.joinToString(",") { (key, value) ->
            val valueStr = when (value) {
                null -> "null"
                is String -> "\"${escapeJson(value)}\""
                is Number -> value.toString()
                is Boolean -> value.toString()
                is Map<*, *> -> mapToJson(value as Map<String, Any?>)
                is List<*> -> listToJson(value)
                else -> "\"${escapeJson(value.toString())}\""
            }
            "\"$key\":$valueStr"
        }
        return "{$entries}"
    }
    
    private fun listToJson(list: List<*>): String {
        val items = list.joinToString(",") { item ->
            when (item) {
                null -> "null"
                is String -> "\"${escapeJson(item)}\""
                is Number -> item.toString()
                is Boolean -> item.toString()
                is Map<*, *> -> mapToJson(item as Map<String, Any?>)
                is List<*> -> listToJson(item)
                else -> "\"${escapeJson(item.toString())}\""
            }
        }
        return "[$items]"
    }
    
    private fun escapeJson(str: String): String {
        return str.replace("\\", "\\\\")
            .replace("\"", "\\\"")
            .replace("\n", "\\n")
            .replace("\r", "\\r")
            .replace("\t", "\\t")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بناء URL
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بناء URL مع بارامترات / Build URL with parameters
     */
    fun buildUrl(baseUrl: String, params: Map<String, String>): String {
        if (params.isEmpty()) return baseUrl
        
        val queryString = params.entries.joinToString("&") { (k, v) ->
            "${URLEncoder.encode(k, "UTF-8")}=${URLEncoder.encode(v, "UTF-8")}"
        }
        
        return if (baseUrl.contains("?")) "$baseUrl&$queryString"
        else "$baseUrl?$queryString"
    }
    
    /**
     * دمج المسارات / Join paths
     */
    fun joinPath(baseUrl: String, vararg paths: String): String {
        var result = baseUrl.trimEnd('/')
        paths.forEach { path ->
            result = "$result/${path.trim('/')}"
        }
        return result
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        executor.shutdown()
    }
}
