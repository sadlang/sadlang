package sad.android.communication.socket

import java.net.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.atomic.AtomicBoolean

/**
 * جسر UDP Socket - اتصالات UDP غير المتصلة
 * UDP Socket Bridge - Connectionless UDP communications
 * 
 * @author فريق لغة ص
 */
object SadUdpSocketBridge {
    
    private val executor: ExecutorService = Executors.newCachedThreadPool()
    private val sockets = mutableMapOf<String, UdpConnection>()
    
    /**
     * اتصال UDP / UDP connection
     */
    private class UdpConnection(
        val id: String,
        var socket: DatagramSocket? = null,
        val isRunning: AtomicBoolean = AtomicBoolean(false)
    )
    
    /**
     * رسالة UDP / UDP message
     */
    data class UdpMessage(
        val data: ByteArray,
        val text: String?,
        val senderAddress: String,
        val senderPort: Int
    ) {
        override fun equals(other: Any?): Boolean {
            if (this === other) return true
            if (javaClass != other?.javaClass) return false
            other as UdpMessage
            return data.contentEquals(other.data) && senderAddress == other.senderAddress && senderPort == other.senderPort
        }
        override fun hashCode(): Int {
            var result = data.contentHashCode()
            result = 31 * result + senderAddress.hashCode()
            result = 31 * result + senderPort
            return result
        }
    }
    
    /**
     * مستمع UDP / UDP listener
     */
    interface UdpListener {
        fun onMessageReceived(socketId: String, message: UdpMessage)
        fun onError(socketId: String, error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء Socket
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء UDP socket / Create UDP socket
     */
    fun create(port: Int = 0): String {
        val socketId = "udp:${port}:${System.currentTimeMillis()}"
        val socket = DatagramSocket(if (port == 0) null else port)
        
        val connection = UdpConnection(socketId, socket)
        sockets[socketId] = connection
        
        return socketId
    }
    
    /**
     * إنشاء socket للبث / Create broadcast socket
     */
    fun createBroadcast(port: Int = 0): String {
        val socketId = "udp-broadcast:${port}:${System.currentTimeMillis()}"
        val socket = DatagramSocket(if (port == 0) null else port)
        socket.broadcast = true
        
        val connection = UdpConnection(socketId, socket)
        sockets[socketId] = connection
        
        return socketId
    }
    
    /**
     * إنشاء socket متعدد البث / Create multicast socket
     */
    fun createMulticast(groupAddress: String, port: Int): String {
        val socketId = "udp-multicast:${groupAddress}:${port}:${System.currentTimeMillis()}"
        
        try {
            val socket = MulticastSocket(port)
            val group = InetAddress.getByName(groupAddress)
            socket.joinGroup(group)
            
            val connection = UdpConnection(socketId, socket)
            sockets[socketId] = connection
        } catch (e: Exception) {
            return ""
        }
        
        return socketId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإرسال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال نص / Send text
     */
    fun send(socketId: String, host: String, port: Int, message: String): Boolean {
        return sendBytes(socketId, host, port, message.toByteArray(Charsets.UTF_8))
    }
    
    /**
     * إرسال بايتات / Send bytes
     */
    fun sendBytes(socketId: String, host: String, port: Int, data: ByteArray): Boolean {
        val connection = sockets[socketId] ?: return false
        
        return try {
            val address = InetAddress.getByName(host)
            val packet = DatagramPacket(data, data.size, address, port)
            connection.socket?.send(packet)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * البث لجميع الأجهزة / Broadcast to all devices
     */
    fun broadcast(socketId: String, port: Int, message: String): Boolean {
        val connection = sockets[socketId] ?: return false
        
        return try {
            val data = message.toByteArray(Charsets.UTF_8)
            val address = InetAddress.getByName("255.255.255.255")
            val packet = DatagramPacket(data, data.size, address, port)
            connection.socket?.send(packet)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * البث على الشبكة المحلية / Broadcast on local network
     */
    fun broadcastLocal(socketId: String, port: Int, message: String): Boolean {
        val localBroadcast = getLocalBroadcastAddress() ?: return broadcast(socketId, port, message)
        
        val connection = sockets[socketId] ?: return false
        
        return try {
            val data = message.toByteArray(Charsets.UTF_8)
            val address = InetAddress.getByName(localBroadcast)
            val packet = DatagramPacket(data, data.size, address, port)
            connection.socket?.send(packet)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إرسال لمجموعة Multicast / Send to multicast group
     */
    fun sendMulticast(socketId: String, groupAddress: String, port: Int, message: String): Boolean {
        return send(socketId, groupAddress, port, message)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاستقبال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الاستماع / Start listening
     */
    fun startListening(socketId: String, bufferSize: Int = 65535, listener: UdpListener) {
        val connection = sockets[socketId] ?: return
        
        connection.isRunning.set(true)
        
        executor.execute {
            val buffer = ByteArray(bufferSize)
            
            while (connection.isRunning.get()) {
                try {
                    val packet = DatagramPacket(buffer, buffer.size)
                    connection.socket?.receive(packet)
                    
                    val data = packet.data.copyOf(packet.length)
                    val message = UdpMessage(
                        data = data,
                        text = String(data, Charsets.UTF_8),
                        senderAddress = packet.address.hostAddress ?: "unknown",
                        senderPort = packet.port
                    )
                    
                    listener.onMessageReceived(socketId, message)
                    
                } catch (e: SocketTimeoutException) {
                    // تجاهل - استمر في الاستماع
                } catch (e: Exception) {
                    if (connection.isRunning.get()) {
                        listener.onError(socketId, e.message ?: "خطأ في الاستقبال")
                    }
                    break
                }
            }
        }
    }
    
    /**
     * استقبال رسالة واحدة / Receive single message
     */
    fun receive(socketId: String, timeout: Int = 5000, bufferSize: Int = 65535): UdpMessage? {
        val connection = sockets[socketId] ?: return null
        
        return try {
            connection.socket?.soTimeout = timeout
            
            val buffer = ByteArray(bufferSize)
            val packet = DatagramPacket(buffer, buffer.size)
            connection.socket?.receive(packet)
            
            val data = packet.data.copyOf(packet.length)
            UdpMessage(
                data = data,
                text = String(data, Charsets.UTF_8),
                senderAddress = packet.address.hostAddress ?: "unknown",
                senderPort = packet.port
            )
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * إيقاف الاستماع / Stop listening
     */
    fun stopListening(socketId: String) {
        sockets[socketId]?.isRunning?.set(false)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إغلاق Socket
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إغلاق socket / Close socket
     */
    fun close(socketId: String): Boolean {
        val connection = sockets.remove(socketId) ?: return false
        
        return try {
            connection.isRunning.set(false)
            
            // مغادرة مجموعة Multicast إذا كان multicast socket
            (connection.socket as? MulticastSocket)?.let { multicastSocket ->
                // لا يمكن معرفة المجموعة بدون تخزينها - سيتم إغلاقها مباشرة
            }
            
            connection.socket?.close()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إغلاق جميع الـ sockets / Close all sockets
     */
    fun closeAll() {
        sockets.keys.toList().forEach { close(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإعدادات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين المهلة / Set timeout
     */
    fun setTimeout(socketId: String, timeout: Int): Boolean {
        val connection = sockets[socketId] ?: return false
        return try {
            connection.socket?.soTimeout = timeout
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * تعيين حجم المخزن المؤقت للإرسال / Set send buffer size
     */
    fun setSendBufferSize(socketId: String, size: Int): Boolean {
        val connection = sockets[socketId] ?: return false
        return try {
            connection.socket?.sendBufferSize = size
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * تعيين حجم المخزن المؤقت للاستقبال / Set receive buffer size
     */
    fun setReceiveBufferSize(socketId: String, size: Int): Boolean {
        val connection = sockets[socketId] ?: return false
        return try {
            connection.socket?.receiveBufferSize = size
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * تفعيل/تعطيل البث / Enable/disable broadcast
     */
    fun setBroadcast(socketId: String, enabled: Boolean): Boolean {
        val connection = sockets[socketId] ?: return false
        return try {
            connection.socket?.broadcast = enabled
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على المنفذ المحلي / Get local port
     */
    fun getLocalPort(socketId: String): Int {
        return sockets[socketId]?.socket?.localPort ?: -1
    }
    
    /**
     * هل الـ socket مفتوح؟ / Is socket open?
     */
    fun isOpen(socketId: String): Boolean {
        val socket = sockets[socketId]?.socket ?: return false
        return !socket.isClosed
    }
    
    /**
     * الحصول على عنوان البث المحلي / Get local broadcast address
     */
    private fun getLocalBroadcastAddress(): String? {
        return try {
            val interfaces = NetworkInterface.getNetworkInterfaces()
            while (interfaces.hasMoreElements()) {
                val iface = interfaces.nextElement()
                if (iface.isLoopback || !iface.isUp) continue
                
                for (address in iface.interfaceAddresses) {
                    val broadcast = address.broadcast
                    if (broadcast != null) {
                        return broadcast.hostAddress
                    }
                }
            }
            null
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        closeAll()
        executor.shutdown()
    }
}
