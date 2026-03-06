package sad.android.communication.nearby

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import androidx.core.content.ContextCompat
import com.google.android.gms.nearby.Nearby
import com.google.android.gms.nearby.connection.*

/**
 * جسر الاتصال القريب - للتواصل بين الأجهزة المجاورة
 * Nearby Connections Bridge - Device-to-device communication
 * 
 * @author فريق لغة ص
 */
object SadNearbyBridge {
    
    private var connectionsClient: ConnectionsClient? = null
    private var discoveredEndpoints = mutableMapOf<String, DiscoveredEndpoint>()
    private var connectedEndpoints = mutableMapOf<String, String>()
    
    /**
     * استراتيجية الاتصال / Connection strategy
     */
    enum class ConnectionStrategy {
        P2P_CLUSTER,    // أجهزة متعددة متصلة ببعض
        P2P_STAR,       // نجمة - جهاز رئيسي وأجهزة فرعية
        P2P_POINT_TO_POINT  // نقطة لنقطة
    }
    
    /**
     * نقطة نهاية مكتشفة / Discovered endpoint
     */
    data class DiscoveredEndpoint(
        val endpointId: String,
        val endpointName: String,
        val serviceId: String
    )
    
    /**
     * رسالة / Payload message
     */
    sealed class PayloadData {
        data class Bytes(val data: ByteArray) : PayloadData()
        data class File(val uri: android.net.Uri, val filename: String) : PayloadData()
        data class Stream(val inputStream: java.io.InputStream) : PayloadData()
    }
    
    /**
     * مستمع الاكتشاف / Discovery listener
     */
    interface DiscoveryListener {
        fun onEndpointFound(endpoint: DiscoveredEndpoint)
        fun onEndpointLost(endpointId: String)
    }
    
    /**
     * مستمع الاتصال / Connection listener
     */
    interface ConnectionListener {
        fun onConnectionInitiated(endpointId: String, endpointName: String)
        fun onConnectionResult(endpointId: String, success: Boolean)
        fun onDisconnected(endpointId: String)
    }
    
    /**
     * مستمع البيانات / Payload listener
     */
    interface PayloadListener {
        fun onPayloadReceived(endpointId: String, data: ByteArray)
        fun onPayloadProgress(endpointId: String, progress: Int)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(context: Context) {
        connectionsClient = Nearby.getConnectionsClient(context)
    }
    
    private fun getClient(context: Context): ConnectionsClient {
        return connectionsClient ?: Nearby.getConnectionsClient(context).also {
            connectionsClient = it
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإعلان
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الإعلان عن الجهاز / Start advertising
     */
    fun startAdvertising(
        context: Context,
        deviceName: String,
        serviceId: String,
        strategy: ConnectionStrategy = ConnectionStrategy.P2P_CLUSTER,
        listener: ConnectionListener? = null
    ): Boolean {
        if (!hasPermissions(context)) return false
        
        val advertisingOptions = AdvertisingOptions.Builder()
            .setStrategy(getStrategy(strategy))
            .build()
        
        val connectionLifecycleCallback = object : ConnectionLifecycleCallback() {
            override fun onConnectionInitiated(endpointId: String, info: ConnectionInfo) {
                listener?.onConnectionInitiated(endpointId, info.endpointName)
            }
            
            override fun onConnectionResult(endpointId: String, result: ConnectionResolution) {
                val success = result.status.isSuccess
                if (success) {
                    connectedEndpoints[endpointId] = endpointId
                }
                listener?.onConnectionResult(endpointId, success)
            }
            
            override fun onDisconnected(endpointId: String) {
                connectedEndpoints.remove(endpointId)
                listener?.onDisconnected(endpointId)
            }
        }
        
        getClient(context).startAdvertising(
            deviceName,
            serviceId,
            connectionLifecycleCallback,
            advertisingOptions
        )
        
        return true
    }
    
    /**
     * إيقاف الإعلان / Stop advertising
     */
    fun stopAdvertising(context: Context) {
        getClient(context).stopAdvertising()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاكتشاف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الاكتشاف / Start discovery
     */
    fun startDiscovery(
        context: Context,
        serviceId: String,
        strategy: ConnectionStrategy = ConnectionStrategy.P2P_CLUSTER,
        listener: DiscoveryListener? = null
    ): Boolean {
        if (!hasPermissions(context)) return false
        
        val discoveryOptions = DiscoveryOptions.Builder()
            .setStrategy(getStrategy(strategy))
            .build()
        
        val endpointDiscoveryCallback = object : EndpointDiscoveryCallback() {
            override fun onEndpointFound(endpointId: String, info: DiscoveredEndpointInfo) {
                val endpoint = DiscoveredEndpoint(
                    endpointId = endpointId,
                    endpointName = info.endpointName,
                    serviceId = info.serviceId
                )
                discoveredEndpoints[endpointId] = endpoint
                listener?.onEndpointFound(endpoint)
            }
            
            override fun onEndpointLost(endpointId: String) {
                discoveredEndpoints.remove(endpointId)
                listener?.onEndpointLost(endpointId)
            }
        }
        
        getClient(context).startDiscovery(
            serviceId,
            endpointDiscoveryCallback,
            discoveryOptions
        )
        
        return true
    }
    
    /**
     * إيقاف الاكتشاف / Stop discovery
     */
    fun stopDiscovery(context: Context) {
        getClient(context).stopDiscovery()
    }
    
    /**
     * جلب الأجهزة المكتشفة / Get discovered endpoints
     */
    fun getDiscoveredEndpoints(): List<DiscoveredEndpoint> {
        return discoveredEndpoints.values.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب اتصال / Request connection
     */
    fun requestConnection(
        context: Context,
        deviceName: String,
        endpointId: String,
        listener: ConnectionListener? = null
    ) {
        val connectionLifecycleCallback = object : ConnectionLifecycleCallback() {
            override fun onConnectionInitiated(endpointId: String, info: ConnectionInfo) {
                listener?.onConnectionInitiated(endpointId, info.endpointName)
            }
            
            override fun onConnectionResult(endpointId: String, result: ConnectionResolution) {
                val success = result.status.isSuccess
                if (success) {
                    connectedEndpoints[endpointId] = endpointId
                }
                listener?.onConnectionResult(endpointId, success)
            }
            
            override fun onDisconnected(endpointId: String) {
                connectedEndpoints.remove(endpointId)
                listener?.onDisconnected(endpointId)
            }
        }
        
        getClient(context).requestConnection(deviceName, endpointId, connectionLifecycleCallback)
    }
    
    /**
     * قبول الاتصال / Accept connection
     */
    fun acceptConnection(context: Context, endpointId: String, listener: PayloadListener? = null) {
        val payloadCallback = object : PayloadCallback() {
            override fun onPayloadReceived(endpointId: String, payload: Payload) {
                if (payload.type == Payload.Type.BYTES) {
                    payload.asBytes()?.let { bytes ->
                        listener?.onPayloadReceived(endpointId, bytes)
                    }
                }
            }
            
            override fun onPayloadTransferUpdate(endpointId: String, update: PayloadTransferUpdate) {
                if (update.totalBytes > 0) {
                    val progress = ((update.bytesTransferred * 100) / update.totalBytes).toInt()
                    listener?.onPayloadProgress(endpointId, progress)
                }
            }
        }
        
        getClient(context).acceptConnection(endpointId, payloadCallback)
    }
    
    /**
     * رفض الاتصال / Reject connection
     */
    fun rejectConnection(context: Context, endpointId: String) {
        getClient(context).rejectConnection(endpointId)
    }
    
    /**
     * قطع الاتصال / Disconnect
     */
    fun disconnect(context: Context, endpointId: String) {
        getClient(context).disconnectFromEndpoint(endpointId)
        connectedEndpoints.remove(endpointId)
    }
    
    /**
     * إيقاف جميع الاتصالات / Stop all connections
     */
    fun stopAllEndpoints(context: Context) {
        getClient(context).stopAllEndpoints()
        connectedEndpoints.clear()
        discoveredEndpoints.clear()
    }
    
    /**
     * الأجهزة المتصلة / Connected endpoints
     */
    fun getConnectedEndpoints(): List<String> {
        return connectedEndpoints.keys.toList()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال بايت / Send bytes
     */
    fun sendBytes(context: Context, endpointId: String, data: ByteArray) {
        val payload = Payload.fromBytes(data)
        getClient(context).sendPayload(endpointId, payload)
    }
    
    /**
     * إرسال نص / Send text
     */
    fun sendText(context: Context, endpointId: String, text: String) {
        sendBytes(context, endpointId, text.toByteArray(Charsets.UTF_8))
    }
    
    /**
     * إرسال لجميع المتصلين / Send to all
     */
    fun sendToAll(context: Context, data: ByteArray) {
        val payload = Payload.fromBytes(data)
        connectedEndpoints.keys.forEach { endpointId ->
            getClient(context).sendPayload(endpointId, payload)
        }
    }
    
    /**
     * إرسال ملف / Send file
     */
    fun sendFile(context: Context, endpointId: String, fileUri: android.net.Uri) {
        val payload = Payload.fromFile(fileUri)
        getClient(context).sendPayload(endpointId, payload)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    fun hasPermissions(context: Context): Boolean {
        val permissions = requiredPermissions()
        return permissions.all {
            ContextCompat.checkSelfPermission(context, it) == PackageManager.PERMISSION_GRANTED
        }
    }
    
    fun requiredPermissions(): Array<String> {
        val permissions = mutableListOf(
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION
        )
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            permissions.add(Manifest.permission.BLUETOOTH_ADVERTISE)
            permissions.add(Manifest.permission.BLUETOOTH_CONNECT)
            permissions.add(Manifest.permission.BLUETOOTH_SCAN)
        }
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            permissions.add(Manifest.permission.NEARBY_WIFI_DEVICES)
        }
        
        return permissions.toTypedArray()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getStrategy(strategy: ConnectionStrategy): Strategy {
        return when (strategy) {
            ConnectionStrategy.P2P_CLUSTER -> Strategy.P2P_CLUSTER
            ConnectionStrategy.P2P_STAR -> Strategy.P2P_STAR
            ConnectionStrategy.P2P_POINT_TO_POINT -> Strategy.P2P_POINT_TO_POINT
        }
    }
}
