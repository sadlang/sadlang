package sad.android.communication.socket

import java.io.*
import java.net.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.atomic.AtomicBoolean
import javax.net.ssl.SSLSocketFactory

/**
 * جسر Socket TCP - اتصالات الشبكة المنخفضة المستوى
 * TCP Socket Bridge - Low-level network connections
 * 
 * @author فريق لغة ص
 */
object SadTcpSocketBridge {
    
    private val executor: ExecutorService = Executors.newCachedThreadPool()
    private val sockets = mutableMapOf<String, SocketConnection>()
    
    /**
     * حالة الاتصال / Connection state
     */
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        ERROR
    }
    
    /**
     * اتصال Socket / Socket connection
     */
    private class SocketConnection(
        val id: String,
        var socket: Socket? = null,
        var reader: BufferedReader? = null,
        var writer: PrintWriter? = null,
        val isRunning: AtomicBoolean = AtomicBoolean(false)
    )
    
    /**
     * مستمع Socket / Socket listener
     */
    interface SocketListener {
        fun onConnected(socketId: String)
        fun onDisconnected(socketId: String)
        fun onMessageReceived(socketId: String, message: String)
        fun onDataReceived(socketId: String, data: ByteArray)
        fun onError(socketId: String, error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بخادم TCP / Connect to TCP server
     */
    fun connect(
        host: String,
        port: Int,
        useSSL: Boolean = false,
        timeout: Int = 10000,
        listener: SocketListener
    ): String {
        val socketId = "${host}:${port}:${System.currentTimeMillis()}"
        val connection = SocketConnection(socketId)
        sockets[socketId] = connection
        
        executor.execute {
            try {
                val socket = if (useSSL) {
                    SSLSocketFactory.getDefault().createSocket() as Socket
                } else {
                    Socket()
                }
                
                socket.connect(InetSocketAddress(host, port), timeout)
                socket.soTimeout = timeout
                
                connection.socket = socket
                connection.reader = BufferedReader(InputStreamReader(socket.getInputStream()))
                connection.writer = PrintWriter(OutputStreamWriter(socket.getOutputStream()), true)
                connection.isRunning.set(true)
                
                listener.onConnected(socketId)
                
                // حلقة القراءة
                startReadLoop(connection, listener)
                
            } catch (e: Exception) {
                listener.onError(socketId, e.message ?: "خطأ في الاتصال")
            }
        }
        
        return socketId
    }
    
    /**
     * الاتصال بـ SSL/TLS / Connect with SSL/TLS
     */
    fun connectSecure(
        host: String,
        port: Int,
        timeout: Int = 10000,
        listener: SocketListener
    ): String {
        return connect(host, port, useSSL = true, timeout = timeout, listener = listener)
    }
    
    private fun startReadLoop(connection: SocketConnection, listener: SocketListener) {
        executor.execute {
            try {
                val buffer = CharArray(8192)
                while (connection.isRunning.get()) {
                    val read = connection.reader?.read(buffer) ?: -1
                    if (read == -1) {
                        connection.isRunning.set(false)
                        listener.onDisconnected(connection.id)
                        break
                    }
                    
                    val message = String(buffer, 0, read)
                    listener.onMessageReceived(connection.id, message)
                }
            } catch (e: SocketTimeoutException) {
                // تجاهل - سيحاول مرة أخرى
            } catch (e: Exception) {
                if (connection.isRunning.get()) {
                    listener.onError(connection.id, e.message ?: "خطأ في القراءة")
                    disconnect(connection.id)
                    listener.onDisconnected(connection.id)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإرسال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال نص / Send text
     */
    fun send(socketId: String, message: String): Boolean {
        val connection = sockets[socketId] ?: return false
        return try {
            connection.writer?.println(message)
            connection.writer?.flush()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إرسال بايتات / Send bytes
     */
    fun sendBytes(socketId: String, data: ByteArray): Boolean {
        val connection = sockets[socketId] ?: return false
        return try {
            connection.socket?.getOutputStream()?.write(data)
            connection.socket?.getOutputStream()?.flush()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إرسال سطر / Send line
     */
    fun sendLine(socketId: String, line: String): Boolean {
        return send(socketId, line)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         قطع الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * قطع الاتصال / Disconnect
     */
    fun disconnect(socketId: String): Boolean {
        val connection = sockets.remove(socketId) ?: return false
        return try {
            connection.isRunning.set(false)
            connection.reader?.close()
            connection.writer?.close()
            connection.socket?.close()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * قطع جميع الاتصالات / Disconnect all
     */
    fun disconnectAll() {
        sockets.keys.toList().forEach { disconnect(it) }
    }
    
    /**
     * هل متصل؟ / Is connected?
     */
    fun isConnected(socketId: String): Boolean {
        val connection = sockets[socketId] ?: return false
        return connection.socket?.isConnected == true && !connection.socket?.isClosed!!
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         خادم TCP
    // ═══════════════════════════════════════════════════════════════
    
    private var serverSocket: ServerSocket? = null
    private val serverRunning = AtomicBoolean(false)
    
    /**
     * مستمع الخادم / Server listener
     */
    interface ServerListener {
        fun onServerStarted(port: Int)
        fun onClientConnected(clientId: String, address: String)
        fun onClientDisconnected(clientId: String)
        fun onMessageReceived(clientId: String, message: String)
        fun onServerError(error: String)
    }
    
    /**
     * بدء خادم TCP / Start TCP server
     */
    fun startServer(port: Int, listener: ServerListener): Boolean {
        if (serverRunning.get()) return false
        
        executor.execute {
            try {
                serverSocket = ServerSocket(port)
                serverRunning.set(true)
                listener.onServerStarted(port)
                
                while (serverRunning.get()) {
                    try {
                        val clientSocket = serverSocket?.accept() ?: break
                        handleClient(clientSocket, listener)
                    } catch (e: SocketException) {
                        if (serverRunning.get()) {
                            listener.onServerError(e.message ?: "خطأ في الخادم")
                        }
                        break
                    }
                }
            } catch (e: Exception) {
                listener.onServerError(e.message ?: "فشل بدء الخادم")
            }
        }
        
        return true
    }
    
    private fun handleClient(clientSocket: Socket, listener: ServerListener) {
        val clientId = "${clientSocket.inetAddress.hostAddress}:${clientSocket.port}"
        val connection = SocketConnection(clientId, clientSocket)
        sockets[clientId] = connection
        
        listener.onClientConnected(clientId, clientSocket.inetAddress.hostAddress ?: "unknown")
        
        executor.execute {
            try {
                connection.reader = BufferedReader(InputStreamReader(clientSocket.getInputStream()))
                connection.writer = PrintWriter(OutputStreamWriter(clientSocket.getOutputStream()), true)
                connection.isRunning.set(true)
                
                val buffer = CharArray(8192)
                while (connection.isRunning.get()) {
                    val read = connection.reader?.read(buffer) ?: -1
                    if (read == -1) break
                    
                    val message = String(buffer, 0, read)
                    listener.onMessageReceived(clientId, message)
                }
            } catch (e: Exception) {
                // Client disconnected
            } finally {
                disconnect(clientId)
                listener.onClientDisconnected(clientId)
            }
        }
    }
    
    /**
     * إيقاف الخادم / Stop server
     */
    fun stopServer(): Boolean {
        return try {
            serverRunning.set(false)
            serverSocket?.close()
            serverSocket = null
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إرسال للعميل / Send to client
     */
    fun sendToClient(clientId: String, message: String): Boolean {
        return send(clientId, message)
    }
    
    /**
     * إرسال للجميع / Broadcast to all clients
     */
    fun broadcast(message: String) {
        sockets.keys.forEach { send(it, message) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على عنوان IP المحلي / Get local IP address
     */
    fun getLocalIpAddress(): String? {
        return try {
            val interfaces = NetworkInterface.getNetworkInterfaces()
            while (interfaces.hasMoreElements()) {
                val iface = interfaces.nextElement()
                val addresses = iface.inetAddresses
                while (addresses.hasMoreElements()) {
                    val addr = addresses.nextElement()
                    if (!addr.isLoopbackAddress && addr is Inet4Address) {
                        return addr.hostAddress
                    }
                }
            }
            null
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * التحقق من الاتصال بالمضيف / Check host reachable
     */
    fun isHostReachable(host: String, port: Int, timeout: Int = 3000): Boolean {
        return try {
            Socket().use { socket ->
                socket.connect(InetSocketAddress(host, port), timeout)
                true
            }
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopServer()
        disconnectAll()
        executor.shutdown()
    }
}
