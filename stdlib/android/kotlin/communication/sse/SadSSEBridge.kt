package sad.android.communication.sse

import android.os.Handler
import android.os.Looper
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.HttpURLConnection
import java.net.URL
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.atomic.AtomicBoolean

/**
 * جسر Server-Sent Events - استقبال الأحداث من الخادم
 * SSE Bridge - Receive server-sent events
 * 
 * @author فريق لغة ص
 */
object SadSSEBridge {
    
    private val executor: ExecutorService = Executors.newCachedThreadPool()
    private val mainHandler = Handler(Looper.getMainLooper())
    private val connections = mutableMapOf<String, SSEConnection>()
    
    /**
     * اتصال SSE / SSE connection
     */
    private data class SSEConnection(
        val id: String,
        val url: String,
        val isRunning: AtomicBoolean = AtomicBoolean(false),
        var lastEventId: String? = null,
        var retryMs: Long = 3000
    )
    
    /**
     * حدث SSE / SSE event
     */
    data class SSEEvent(
        val id: String?,
        val event: String,
        val data: String,
        val retry: Long?
    )
    
    /**
     * خيارات SSE / SSE options
     */
    data class SSEOptions(
        val headers: Map<String, String> = emptyMap(),
        val lastEventId: String? = null,
        val connectTimeout: Int = 10000,
        val readTimeout: Int = 0, // 0 = infinite
        val autoReconnect: Boolean = true,
        val maxReconnectAttempts: Int = 10,
        val initialRetryMs: Long = 3000
    )
    
    /**
     * مستمع SSE / SSE listener
     */
    interface SSEListener {
        fun onOpen(connectionId: String)
        fun onMessage(connectionId: String, event: SSEEvent)
        fun onError(connectionId: String, error: String)
        fun onClosed(connectionId: String)
        fun onReconnecting(connectionId: String, attempt: Int)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بـ SSE endpoint / Connect to SSE endpoint
     */
    fun connect(
        url: String,
        options: SSEOptions = SSEOptions(),
        listener: SSEListener
    ): String {
        val connectionId = "sse:${System.currentTimeMillis()}"
        
        val connection = SSEConnection(
            id = connectionId,
            url = url,
            lastEventId = options.lastEventId,
            retryMs = options.initialRetryMs
        )
        
        connections[connectionId] = connection
        connection.isRunning.set(true)
        
        startConnection(connection, options, listener, 0)
        
        return connectionId
    }
    
    private fun startConnection(
        connection: SSEConnection,
        options: SSEOptions,
        listener: SSEListener,
        attempt: Int
    ) {
        executor.execute {
            var httpConnection: HttpURLConnection? = null
            
            try {
                val url = URL(connection.url)
                httpConnection = url.openConnection() as HttpURLConnection
                
                httpConnection.apply {
                    requestMethod = "GET"
                    connectTimeout = options.connectTimeout
                    readTimeout = options.readTimeout
                    setRequestProperty("Accept", "text/event-stream")
                    setRequestProperty("Cache-Control", "no-cache")
                    
                    options.headers.forEach { (key, value) ->
                        setRequestProperty(key, value)
                    }
                    
                    connection.lastEventId?.let {
                        setRequestProperty("Last-Event-ID", it)
                    }
                }
                
                if (httpConnection.responseCode != 200) {
                    throw Exception("HTTP ${httpConnection.responseCode}")
                }
                
                mainHandler.post { listener.onOpen(connection.id) }
                
                val reader = BufferedReader(InputStreamReader(httpConnection.inputStream))
                parseEvents(connection, reader, listener)
                
            } catch (e: Exception) {
                if (connection.isRunning.get()) {
                    mainHandler.post { 
                        listener.onError(connection.id, e.message ?: "خطأ في الاتصال") 
                    }
                    
                    // إعادة الاتصال
                    if (options.autoReconnect && attempt < options.maxReconnectAttempts) {
                        mainHandler.post { 
                            listener.onReconnecting(connection.id, attempt + 1) 
                        }
                        
                        Thread.sleep(connection.retryMs)
                        
                        if (connection.isRunning.get()) {
                            startConnection(connection, options, listener, attempt + 1)
                        }
                    } else {
                        connection.isRunning.set(false)
                        mainHandler.post { listener.onClosed(connection.id) }
                    }
                }
            } finally {
                httpConnection?.disconnect()
            }
        }
    }
    
    private fun parseEvents(
        connection: SSEConnection,
        reader: BufferedReader,
        listener: SSEListener
    ) {
        var eventType = "message"
        var eventId: String? = null
        var eventData = StringBuilder()
        
        while (connection.isRunning.get()) {
            val line = reader.readLine() ?: break
            
            when {
                line.isEmpty() -> {
                    // نهاية الحدث - إرسال
                    if (eventData.isNotEmpty()) {
                        val data = eventData.toString().trimEnd('\n')
                        val event = SSEEvent(
                            id = eventId,
                            event = eventType,
                            data = data,
                            retry = null
                        )
                        
                        eventId?.let { connection.lastEventId = it }
                        
                        mainHandler.post {
                            listener.onMessage(connection.id, event)
                        }
                    }
                    
                    // إعادة تعيين
                    eventType = "message"
                    eventId = null
                    eventData = StringBuilder()
                }
                
                line.startsWith("event:") -> {
                    eventType = line.removePrefix("event:").trim()
                }
                
                line.startsWith("data:") -> {
                    val data = line.removePrefix("data:").trimStart()
                    eventData.append(data).append("\n")
                }
                
                line.startsWith("id:") -> {
                    eventId = line.removePrefix("id:").trim()
                }
                
                line.startsWith("retry:") -> {
                    line.removePrefix("retry:").trim().toLongOrNull()?.let {
                        connection.retryMs = it
                    }
                }
                
                line.startsWith(":") -> {
                    // تعليق - تجاهل
                }
            }
        }
        
        if (!connection.isRunning.get()) {
            mainHandler.post { listener.onClosed(connection.id) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإغلاق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إغلاق الاتصال / Close connection
     */
    fun disconnect(connectionId: String): Boolean {
        val connection = connections.remove(connectionId) ?: return false
        connection.isRunning.set(false)
        return true
    }
    
    /**
     * إغلاق جميع الاتصالات / Close all connections
     */
    fun disconnectAll() {
        connections.keys.toList().forEach { disconnect(it) }
    }
    
    /**
     * هل الاتصال نشط؟ / Is connection active?
     */
    fun isConnected(connectionId: String): Boolean {
        return connections[connectionId]?.isRunning?.get() == true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على آخر Event ID / Get last event ID
     */
    fun getLastEventId(connectionId: String): String? {
        return connections[connectionId]?.lastEventId
    }
    
    /**
     * عدد الاتصالات النشطة / Active connections count
     */
    fun activeConnectionsCount(): Int {
        return connections.count { it.value.isRunning.get() }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        disconnectAll()
        executor.shutdown()
    }
}
