package sad.android.communication.p2p

import android.content.Context
import android.net.wifi.p2p.*
import android.net.wifi.p2p.WifiP2pManager.*
import android.os.Looper
import java.io.*
import java.net.InetSocketAddress
import java.net.ServerSocket
import java.net.Socket
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * جسر Wi-Fi Direct P2P - الاتصال المباشر بين الأجهزة
 * Wi-Fi Direct P2P Bridge - Direct device-to-device communication
 * 
 * @author فريق لغة ص
 */
object SadWifiP2pBridge {
    
    private var manager: WifiP2pManager? = null
    private var channel: Channel? = null
    private val executor: ExecutorService = Executors.newCachedThreadPool()
    
    private const val SOCKET_PORT = 8888
    
    /**
     * معلومات الجهاز / Device info
     */
    data class P2pDevice(
        val name: String,
        val address: String,
        val status: DeviceStatus,
        val primaryDeviceType: String?,
        val secondaryDeviceType: String?,
        val isGroupOwner: Boolean
    )
    
    /**
     * حالة الجهاز / Device status
     */
    enum class DeviceStatus {
        CONNECTED,
        INVITED,
        FAILED,
        AVAILABLE,
        UNAVAILABLE
    }
    
    /**
     * معلومات المجموعة / Group info
     */
    data class P2pGroup(
        val networkName: String,
        val passphrase: String,
        val isGroupOwner: Boolean,
        val ownerAddress: String?,
        val clients: List<P2pDevice>
    )
    
    /**
     * معلومات الاتصال / Connection info
     */
    data class P2pConnection(
        val groupFormed: Boolean,
        val isGroupOwner: Boolean,
        val groupOwnerAddress: String?
    )
    
    /**
     * مستمع P2P / P2P listener
     */
    interface P2pListener {
        fun onWifiP2pEnabled(enabled: Boolean)
        fun onPeersDiscovered(peers: List<P2pDevice>)
        fun onConnectionChanged(connection: P2pConnection)
        fun onDeviceChanged(device: P2pDevice)
        fun onGroupCreated(group: P2pGroup)
        fun onDataReceived(data: ByteArray, fromAddress: String)
        fun onError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة الجسر / Initialize bridge
     */
    fun initialize(context: Context): Boolean {
        manager = context.getSystemService(Context.WIFI_P2P_SERVICE) as? WifiP2pManager
        if (manager == null) return false
        
        channel = manager?.initialize(context, Looper.getMainLooper(), null)
        return channel != null
    }
    
    /**
     * هل Wi-Fi Direct مدعوم؟ / Is Wi-Fi Direct supported?
     */
    fun isSupported(context: Context): Boolean {
        return context.packageManager.hasSystemFeature("android.hardware.wifi.direct")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اكتشاف الأجهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء اكتشاف الأجهزة / Start peer discovery
     */
    fun discoverPeers(listener: P2pListener) {
        manager?.discoverPeers(channel, object : ActionListener {
            override fun onSuccess() {
                // سيتم استدعاء onPeersChanged من خلال BroadcastReceiver
            }
            
            override fun onFailure(reason: Int) {
                listener.onError("فشل اكتشاف الأجهزة: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * إيقاف الاكتشاف / Stop discovery
     */
    fun stopPeerDiscovery(listener: P2pListener) {
        manager?.stopPeerDiscovery(channel, object : ActionListener {
            override fun onSuccess() {}
            override fun onFailure(reason: Int) {
                listener.onError("فشل إيقاف الاكتشاف: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * طلب قائمة الأجهزة / Request peers list
     */
    fun requestPeers(listener: P2pListener) {
        manager?.requestPeers(channel) { peers ->
            val devices = peers.deviceList.map { deviceToP2pDevice(it) }
            listener.onPeersDiscovered(devices)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بجهاز / Connect to device
     */
    fun connect(deviceAddress: String, listener: P2pListener) {
        val config = WifiP2pConfig().apply {
            this.deviceAddress = deviceAddress
            // WPS Push Button Configuration
            wps.setup = WpsInfo.PBC
        }
        
        manager?.connect(channel, config, object : ActionListener {
            override fun onSuccess() {
                // سيتم استدعاء onConnectionChanged من خلال BroadcastReceiver
            }
            
            override fun onFailure(reason: Int) {
                listener.onError("فشل الاتصال: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * الاتصال مع تعيين كـ Group Owner / Connect as Group Owner
     */
    fun connectAsGroupOwner(deviceAddress: String, listener: P2pListener) {
        val config = WifiP2pConfig().apply {
            this.deviceAddress = deviceAddress
            wps.setup = WpsInfo.PBC
            groupOwnerIntent = 15 // أعلى أولوية لتكون Group Owner
        }
        
        manager?.connect(channel, config, object : ActionListener {
            override fun onSuccess() {}
            override fun onFailure(reason: Int) {
                listener.onError("فشل الاتصال: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * قطع الاتصال / Disconnect
     */
    fun disconnect(listener: P2pListener) {
        manager?.removeGroup(channel, object : ActionListener {
            override fun onSuccess() {}
            override fun onFailure(reason: Int) {
                listener.onError("فشل قطع الاتصال: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * إلغاء الاتصال المعلق / Cancel pending connection
     */
    fun cancelConnect(listener: P2pListener) {
        manager?.cancelConnect(channel, object : ActionListener {
            override fun onSuccess() {}
            override fun onFailure(reason: Int) {
                listener.onError("فشل الإلغاء: ${getFailureReason(reason)}")
            }
        })
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء مجموعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء مجموعة / Create group
     */
    fun createGroup(listener: P2pListener) {
        manager?.createGroup(channel, object : ActionListener {
            override fun onSuccess() {}
            override fun onFailure(reason: Int) {
                listener.onError("فشل إنشاء المجموعة: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * حذف المجموعة / Remove group
     */
    fun removeGroup(listener: P2pListener) {
        manager?.removeGroup(channel, object : ActionListener {
            override fun onSuccess() {}
            override fun onFailure(reason: Int) {
                listener.onError("فشل حذف المجموعة: ${getFailureReason(reason)}")
            }
        })
    }
    
    /**
     * طلب معلومات المجموعة / Request group info
     */
    fun requestGroupInfo(listener: P2pListener) {
        manager?.requestGroupInfo(channel) { group ->
            if (group != null) {
                val p2pGroup = P2pGroup(
                    networkName = group.networkName,
                    passphrase = group.passphrase ?: "",
                    isGroupOwner = group.isGroupOwner,
                    ownerAddress = group.owner?.deviceAddress,
                    clients = group.clientList.map { deviceToP2pDevice(it) }
                )
                listener.onGroupCreated(p2pGroup)
            }
        }
    }
    
    /**
     * طلب معلومات الاتصال / Request connection info
     */
    fun requestConnectionInfo(listener: P2pListener) {
        manager?.requestConnectionInfo(channel) { info ->
            val connection = P2pConnection(
                groupFormed = info.groupFormed,
                isGroupOwner = info.isGroupOwner,
                groupOwnerAddress = info.groupOwnerAddress?.hostAddress
            )
            listener.onConnectionChanged(connection)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         نقل البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء خادم استقبال (Group Owner) / Start server (Group Owner)
     */
    fun startServer(port: Int = SOCKET_PORT, listener: P2pListener) {
        executor.execute {
            try {
                val serverSocket = ServerSocket(port)
                val client = serverSocket.accept()
                
                val inputStream = client.getInputStream()
                val buffer = ByteArray(1024)
                val bytesRead = inputStream.read(buffer)
                
                if (bytesRead > 0) {
                    val data = buffer.copyOf(bytesRead)
                    listener.onDataReceived(data, client.inetAddress.hostAddress ?: "unknown")
                }
                
                client.close()
                serverSocket.close()
            } catch (e: IOException) {
                listener.onError("خطأ في الخادم: ${e.message}")
            }
        }
    }
    
    /**
     * بدء خادم استقبال مستمر / Start continuous server
     */
    private var serverRunning = false
    
    fun startContinuousServer(port: Int = SOCKET_PORT, listener: P2pListener) {
        serverRunning = true
        
        executor.execute {
            try {
                val serverSocket = ServerSocket(port)
                
                while (serverRunning) {
                    val client = serverSocket.accept()
                    
                    executor.execute {
                        try {
                            val inputStream = client.getInputStream()
                            val buffer = ByteArray(4096)
                            var bytesRead: Int
                            
                            while (client.isConnected) {
                                bytesRead = inputStream.read(buffer)
                                if (bytesRead == -1) break
                                
                                val data = buffer.copyOf(bytesRead)
                                listener.onDataReceived(data, client.inetAddress.hostAddress ?: "unknown")
                            }
                        } catch (e: IOException) {
                            // Client disconnected
                        } finally {
                            client.close()
                        }
                    }
                }
                
                serverSocket.close()
            } catch (e: IOException) {
                if (serverRunning) {
                    listener.onError("خطأ في الخادم: ${e.message}")
                }
            }
        }
    }
    
    /**
     * إيقاف الخادم / Stop server
     */
    fun stopServer() {
        serverRunning = false
    }
    
    /**
     * إرسال بيانات للـ Group Owner / Send data to Group Owner
     */
    fun sendData(hostAddress: String, data: ByteArray, port: Int = SOCKET_PORT, listener: P2pListener) {
        executor.execute {
            var socket: Socket? = null
            try {
                socket = Socket()
                socket.connect(InetSocketAddress(hostAddress, port), 5000)
                
                val outputStream = socket.getOutputStream()
                outputStream.write(data)
                outputStream.flush()
                
            } catch (e: IOException) {
                listener.onError("خطأ في الإرسال: ${e.message}")
            } finally {
                socket?.close()
            }
        }
    }
    
    /**
     * إرسال نص / Send text
     */
    fun sendText(hostAddress: String, text: String, port: Int = SOCKET_PORT, listener: P2pListener) {
        sendData(hostAddress, text.toByteArray(Charsets.UTF_8), port, listener)
    }
    
    /**
     * إرسال ملف / Send file
     */
    fun sendFile(hostAddress: String, filePath: String, port: Int = SOCKET_PORT, listener: P2pListener) {
        executor.execute {
            var socket: Socket? = null
            var fileInputStream: FileInputStream? = null
            
            try {
                socket = Socket()
                socket.connect(InetSocketAddress(hostAddress, port), 10000)
                
                val file = File(filePath)
                fileInputStream = FileInputStream(file)
                val outputStream = socket.getOutputStream()
                
                // إرسال اسم الملف وحجمه أولاً
                val header = "${file.name}|${file.length()}\n"
                outputStream.write(header.toByteArray())
                
                // إرسال محتوى الملف
                val buffer = ByteArray(8192)
                var bytesRead: Int
                
                while (fileInputStream.read(buffer).also { bytesRead = it } != -1) {
                    outputStream.write(buffer, 0, bytesRead)
                }
                
                outputStream.flush()
                
            } catch (e: IOException) {
                listener.onError("خطأ في إرسال الملف: ${e.message}")
            } finally {
                fileInputStream?.close()
                socket?.close()
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إعدادات الجهاز
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تغيير اسم الجهاز / Change device name
     */
    fun setDeviceName(name: String, listener: P2pListener) {
        try {
            val method = manager?.javaClass?.getMethod(
                "setDeviceName",
                Channel::class.java,
                String::class.java,
                ActionListener::class.java
            )
            
            method?.invoke(manager, channel, name, object : ActionListener {
                override fun onSuccess() {}
                override fun onFailure(reason: Int) {
                    listener.onError("فشل تغيير الاسم: ${getFailureReason(reason)}")
                }
            })
        } catch (e: Exception) {
            listener.onError("تغيير الاسم غير مدعوم")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun deviceToP2pDevice(device: WifiP2pDevice): P2pDevice {
        return P2pDevice(
            name = device.deviceName,
            address = device.deviceAddress,
            status = when (device.status) {
                WifiP2pDevice.CONNECTED -> DeviceStatus.CONNECTED
                WifiP2pDevice.INVITED -> DeviceStatus.INVITED
                WifiP2pDevice.FAILED -> DeviceStatus.FAILED
                WifiP2pDevice.AVAILABLE -> DeviceStatus.AVAILABLE
                else -> DeviceStatus.UNAVAILABLE
            },
            primaryDeviceType = device.primaryDeviceType,
            secondaryDeviceType = device.secondaryDeviceType,
            isGroupOwner = device.isGroupOwner
        )
    }
    
    private fun getFailureReason(reason: Int): String {
        return when (reason) {
            ERROR -> "خطأ داخلي"
            P2P_UNSUPPORTED -> "P2P غير مدعوم"
            BUSY -> "النظام مشغول"
            else -> "سبب غير معروف ($reason)"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopServer()
        executor.shutdown()
        channel = null
        manager = null
    }
}
