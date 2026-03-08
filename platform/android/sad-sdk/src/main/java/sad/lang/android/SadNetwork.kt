/**
 * =============================================================================
 * @file SadNetwork.kt
 * @brief (AR) واجهة الشبكة لأندرويد — HTTP و WebSocket
 * @brief (EN) Android Network Interface — HTTP and WebSocket
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import kotlinx.coroutines.*
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.RequestBody.Companion.toRequestBody
import org.json.JSONObject
import java.io.IOException
import java.util.concurrent.TimeUnit

/**
 * نظام الشبكة للغة ص على أندرويد
 */
class SadNetwork(private val context: Context) {
    
    companion object {
        private const val DEFAULT_TIMEOUT = 30L
        
        init {
            System.loadLibrary("sad_android")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Native Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    private external fun nativeInit(): Boolean
    private external fun nativeHttpGet(url: String, callback: Any?): Int
    private external fun nativeHttpPost(url: String, body: String, contentType: String, callback: Any?): Int
    private external fun nativeCancelRequest(requestId: Int)
    private external fun nativeWebSocketConnect(url: String, listener: Any?): Int
    private external fun nativeWebSocketSend(connectionId: Int, message: String): Boolean
    private external fun nativeWebSocketClose(connectionId: Int)
    private external fun nativeShutdown()
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val client: OkHttpClient by lazy {
        OkHttpClient.Builder()
            .connectTimeout(DEFAULT_TIMEOUT, TimeUnit.SECONDS)
            .readTimeout(DEFAULT_TIMEOUT, TimeUnit.SECONDS)
            .writeTimeout(DEFAULT_TIMEOUT, TimeUnit.SECONDS)
            .build()
    }
    
    private val activeWebSockets = mutableMapOf<Int, WebSocket>()
    private var nextConnectionId = 1
    
    private val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نتيجة HTTP
     */
    data class HttpResponse(
        val statusCode: Int,
        val body: String,
        val headers: Map<String, String>,
        val isSuccess: Boolean
    )
    
    /**
     * مستمع WebSocket
     */
    interface WebSocketListener {
        fun onOpen()
        fun onMessage(message: String)
        fun onClosing(code: Int, reason: String)
        fun onClosed(code: Int, reason: String)
        fun onFailure(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Initialization
    // ═══════════════════════════════════════════════════════════════════════════
    
    fun init(): Boolean {
        return nativeInit()
    }
    
    fun shutdown() {
        scope.cancel()
        activeWebSockets.values.forEach { it.close(1000, "Shutdown") }
        activeWebSockets.clear()
        nativeShutdown()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  HTTP GET
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * طلب GET متزامن
     */
    suspend fun get(
        url: String,
        headers: Map<String, String> = emptyMap()
    ): Result<HttpResponse> = withContext(Dispatchers.IO) {
        try {
            val requestBuilder = Request.Builder().url(url)
            headers.forEach { (key, value) -> requestBuilder.addHeader(key, value) }
            
            val response = client.newCall(requestBuilder.build()).execute()
            val responseBody = response.body?.string() ?: ""
            val responseHeaders = response.headers.toMultimap()
                .mapValues { it.value.firstOrNull() ?: "" }
            
            Result.success(HttpResponse(
                statusCode = response.code,
                body = responseBody,
                headers = responseHeaders,
                isSuccess = response.isSuccessful
            ))
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * طلب GET غير متزامن
     */
    fun getAsync(
        url: String,
        headers: Map<String, String> = emptyMap(),
        callback: (Result<HttpResponse>) -> Unit
    ) {
        scope.launch {
            val result = get(url, headers)
            withContext(Dispatchers.Main) {
                callback(result)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  HTTP POST
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * طلب POST متزامن
     */
    suspend fun post(
        url: String,
        body: String,
        contentType: String = "application/json",
        headers: Map<String, String> = emptyMap()
    ): Result<HttpResponse> = withContext(Dispatchers.IO) {
        try {
            val requestBody = body.toRequestBody(contentType.toMediaType())
            val requestBuilder = Request.Builder()
                .url(url)
                .post(requestBody)
            headers.forEach { (key, value) -> requestBuilder.addHeader(key, value) }
            
            val response = client.newCall(requestBuilder.build()).execute()
            val responseBody = response.body?.string() ?: ""
            val responseHeaders = response.headers.toMultimap()
                .mapValues { it.value.firstOrNull() ?: "" }
            
            Result.success(HttpResponse(
                statusCode = response.code,
                body = responseBody,
                headers = responseHeaders,
                isSuccess = response.isSuccessful
            ))
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * طلب POST JSON
     */
    suspend fun postJson(
        url: String,
        json: JSONObject,
        headers: Map<String, String> = emptyMap()
    ): Result<HttpResponse> {
        return post(url, json.toString(), "application/json", headers)
    }
    
    /**
     * طلب POST غير متزامن
     */
    fun postAsync(
        url: String,
        body: String,
        contentType: String = "application/json",
        headers: Map<String, String> = emptyMap(),
        callback: (Result<HttpResponse>) -> Unit
    ) {
        scope.launch {
            val result = post(url, body, contentType, headers)
            withContext(Dispatchers.Main) {
                callback(result)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  HTTP PUT/DELETE/PATCH
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * طلب PUT
     */
    suspend fun put(
        url: String,
        body: String,
        contentType: String = "application/json",
        headers: Map<String, String> = emptyMap()
    ): Result<HttpResponse> = withContext(Dispatchers.IO) {
        try {
            val requestBody = body.toRequestBody(contentType.toMediaType())
            val requestBuilder = Request.Builder()
                .url(url)
                .put(requestBody)
            headers.forEach { (key, value) -> requestBuilder.addHeader(key, value) }
            
            val response = client.newCall(requestBuilder.build()).execute()
            Result.success(HttpResponse(
                statusCode = response.code,
                body = response.body?.string() ?: "",
                headers = response.headers.toMultimap().mapValues { it.value.firstOrNull() ?: "" },
                isSuccess = response.isSuccessful
            ))
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * طلب DELETE
     */
    suspend fun delete(
        url: String,
        headers: Map<String, String> = emptyMap()
    ): Result<HttpResponse> = withContext(Dispatchers.IO) {
        try {
            val requestBuilder = Request.Builder()
                .url(url)
                .delete()
            headers.forEach { (key, value) -> requestBuilder.addHeader(key, value) }
            
            val response = client.newCall(requestBuilder.build()).execute()
            Result.success(HttpResponse(
                statusCode = response.code,
                body = response.body?.string() ?: "",
                headers = response.headers.toMultimap().mapValues { it.value.firstOrNull() ?: "" },
                isSuccess = response.isSuccessful
            ))
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  WebSocket
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بـ WebSocket
     */
    fun connectWebSocket(
        url: String,
        listener: WebSocketListener
    ): Int {
        val connectionId = nextConnectionId++
        
        val request = Request.Builder().url(url).build()
        
        val wsListener = object : okhttp3.WebSocketListener() {
            override fun onOpen(webSocket: WebSocket, response: Response) {
                listener.onOpen()
            }
            
            override fun onMessage(webSocket: WebSocket, text: String) {
                listener.onMessage(text)
            }
            
            override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
                listener.onClosing(code, reason)
            }
            
            override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                activeWebSockets.remove(connectionId)
                listener.onClosed(code, reason)
            }
            
            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                activeWebSockets.remove(connectionId)
                listener.onFailure(t.message ?: "Unknown error")
            }
        }
        
        val webSocket = client.newWebSocket(request, wsListener)
        activeWebSockets[connectionId] = webSocket
        
        return connectionId
    }
    
    /**
     * إرسال رسالة عبر WebSocket
     */
    fun sendWebSocket(connectionId: Int, message: String): Boolean {
        return activeWebSockets[connectionId]?.send(message) ?: false
    }
    
    /**
     * إغلاق WebSocket
     */
    fun closeWebSocket(connectionId: Int, code: Int = 1000, reason: String = "Closed") {
        activeWebSockets[connectionId]?.close(code, reason)
        activeWebSockets.remove(connectionId)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Utilities
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحميل ملف
     */
    suspend fun downloadFile(
        url: String,
        outputPath: String,
        progressCallback: ((Long, Long) -> Unit)? = null
    ): Result<Unit> = withContext(Dispatchers.IO) {
        try {
            val request = Request.Builder().url(url).build()
            val response = client.newCall(request).execute()
            
            if (!response.isSuccessful) {
                return@withContext Result.failure(IOException("HTTP ${response.code}"))
            }
            
            val body = response.body ?: return@withContext Result.failure(IOException("Empty body"))
            val contentLength = body.contentLength()
            
            val file = java.io.File(outputPath)
            file.parentFile?.mkdirs()
            
            file.outputStream().use { output ->
                val buffer = ByteArray(8192)
                var bytesRead: Int
                var totalBytes = 0L
                
                body.byteStream().use { input ->
                    while (input.read(buffer).also { bytesRead = it } != -1) {
                        output.write(buffer, 0, bytesRead)
                        totalBytes += bytesRead
                        progressCallback?.invoke(totalBytes, contentLength)
                    }
                }
            }
            
            Result.success(Unit)
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * فحص الاتصال بالإنترنت
     */
    fun isConnected(): Boolean {
        val connectivityManager = context.getSystemService(Context.CONNECTIVITY_SERVICE) 
            as android.net.ConnectivityManager
        val network = connectivityManager.activeNetwork ?: return false
        val capabilities = connectivityManager.getNetworkCapabilities(network) ?: return false
        return capabilities.hasCapability(android.net.NetworkCapabilities.NET_CAPABILITY_INTERNET)
    }
}
