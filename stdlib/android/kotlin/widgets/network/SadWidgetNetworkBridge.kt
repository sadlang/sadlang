package sad.android.widgets.network

import android.content.Context
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.net.wifi.WifiManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.telephony.TelephonyManager
import java.net.InetAddress

/**
 * جسر ويدجت حالة الشبكة - عرض معلومات الاتصال
 * Network Status Widget Bridge - Display connection info
 * 
 * @author فريق لغة ص
 */
object SadWidgetNetworkBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var connectivityManager: ConnectivityManager? = null
    private var networkCallback: ConnectivityManager.NetworkCallback? = null
    
    private val networkListeners = mutableListOf<NetworkListener>()
    
    /**
     * نوع الاتصال / Connection type
     */
    enum class ConnectionType {
        NONE,
        WIFI,
        MOBILE,
        ETHERNET,
        VPN,
        BLUETOOTH,
        UNKNOWN
    }
    
    /**
     * جودة الاتصال / Connection quality
     */
    enum class ConnectionQuality {
        EXCELLENT,
        GOOD,
        MODERATE,
        POOR,
        UNKNOWN
    }
    
    /**
     * معلومات الشبكة / Network info
     */
    data class NetworkInfo(
        val isConnected: Boolean,
        val connectionType: ConnectionType,
        val typeName: String,
        val quality: ConnectionQuality,
        val signalStrength: Int,      // 0-100
        val downloadSpeed: Int?,      // Mbps
        val uploadSpeed: Int?,        // Mbps
        val wifiName: String?,
        val wifiBssid: String?,
        val carrierName: String?,
        val ipAddress: String?,
        val isRoaming: Boolean
    )
    
    /**
     * مستمع الشبكة / Network listener
     */
    interface NetworkListener {
        fun onNetworkChanged(info: NetworkInfo)
        fun onNetworkLost()
        fun onNetworkAvailable()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        connectivityManager = ctx.getSystemService(Context.CONNECTIVITY_SERVICE) as? ConnectivityManager
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addNetworkListener(listener: NetworkListener) {
        if (!networkListeners.contains(listener)) {
            networkListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeNetworkListener(listener: NetworkListener) {
        networkListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الشبكة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات الشبكة الحالية / Get current network info
     */
    fun getCurrentNetworkInfo(): NetworkInfo {
        val ctx = context ?: return createNoNetworkInfo()
        val cm = connectivityManager ?: return createNoNetworkInfo()
        
        val network = cm.activeNetwork ?: return createNoNetworkInfo()
        val capabilities = cm.getNetworkCapabilities(network) ?: return createNoNetworkInfo()
        
        val connectionType = getConnectionType(capabilities)
        val quality = getConnectionQuality(capabilities)
        val signalStrength = getSignalStrength(ctx, connectionType)
        
        val wifiInfo = getWifiInfo(ctx)
        val carrierInfo = getCarrierInfo(ctx)
        
        return NetworkInfo(
            isConnected = true,
            connectionType = connectionType,
            typeName = getTypeName(connectionType),
            quality = quality,
            signalStrength = signalStrength,
            downloadSpeed = getDownloadSpeed(capabilities),
            uploadSpeed = getUploadSpeed(capabilities),
            wifiName = wifiInfo?.first,
            wifiBssid = wifiInfo?.second,
            carrierName = carrierInfo?.first,
            ipAddress = getIpAddress(),
            isRoaming = carrierInfo?.second ?: false
        )
    }
    
    private fun createNoNetworkInfo(): NetworkInfo {
        return NetworkInfo(
            isConnected = false,
            connectionType = ConnectionType.NONE,
            typeName = "غير متصل",
            quality = ConnectionQuality.UNKNOWN,
            signalStrength = 0,
            downloadSpeed = null,
            uploadSpeed = null,
            wifiName = null,
            wifiBssid = null,
            carrierName = null,
            ipAddress = null,
            isRoaming = false
        )
    }
    
    private fun getConnectionType(capabilities: NetworkCapabilities): ConnectionType {
        return when {
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) -> ConnectionType.WIFI
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) -> ConnectionType.MOBILE
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) -> ConnectionType.ETHERNET
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_VPN) -> ConnectionType.VPN
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_BLUETOOTH) -> ConnectionType.BLUETOOTH
            else -> ConnectionType.UNKNOWN
        }
    }
    
    private fun getConnectionQuality(capabilities: NetworkCapabilities): ConnectionQuality {
        val downSpeed = capabilities.linkDownstreamBandwidthKbps
        val upSpeed = capabilities.linkUpstreamBandwidthKbps
        
        return when {
            downSpeed >= 20000 && upSpeed >= 5000 -> ConnectionQuality.EXCELLENT
            downSpeed >= 10000 && upSpeed >= 2000 -> ConnectionQuality.GOOD
            downSpeed >= 5000 && upSpeed >= 1000 -> ConnectionQuality.MODERATE
            downSpeed > 0 -> ConnectionQuality.POOR
            else -> ConnectionQuality.UNKNOWN
        }
    }
    
    private fun getSignalStrength(ctx: Context, type: ConnectionType): Int {
        return when (type) {
            ConnectionType.WIFI -> {
                val wifiManager = ctx.applicationContext.getSystemService(Context.WIFI_SERVICE) as? WifiManager
                val wifiInfo = wifiManager?.connectionInfo
                val rssi = wifiInfo?.rssi ?: -100
                WifiManager.calculateSignalLevel(rssi, 100)
            }
            ConnectionType.MOBILE -> {
                // تقريبي
                50
            }
            else -> 0
        }
    }
    
    private fun getDownloadSpeed(capabilities: NetworkCapabilities): Int? {
        val kbps = capabilities.linkDownstreamBandwidthKbps
        return if (kbps > 0) kbps / 1000 else null
    }
    
    private fun getUploadSpeed(capabilities: NetworkCapabilities): Int? {
        val kbps = capabilities.linkUpstreamBandwidthKbps
        return if (kbps > 0) kbps / 1000 else null
    }
    
    private fun getTypeName(type: ConnectionType): String {
        return when (type) {
            ConnectionType.WIFI -> "واي فاي"
            ConnectionType.MOBILE -> "بيانات الجوال"
            ConnectionType.ETHERNET -> "إيثرنت"
            ConnectionType.VPN -> "VPN"
            ConnectionType.BLUETOOTH -> "بلوتوث"
            ConnectionType.NONE -> "غير متصل"
            ConnectionType.UNKNOWN -> "غير معروف"
        }
    }
    
    @Suppress("DEPRECATION")
    private fun getWifiInfo(ctx: Context): Pair<String, String>? {
        val wifiManager = ctx.applicationContext.getSystemService(Context.WIFI_SERVICE) as? WifiManager
        val wifiInfo = wifiManager?.connectionInfo ?: return null
        
        val ssid = wifiInfo.ssid?.removePrefix("\"")?.removeSuffix("\"") ?: return null
        val bssid = wifiInfo.bssid ?: ""
        
        return if (ssid != "<unknown ssid>") Pair(ssid, bssid) else null
    }
    
    private fun getCarrierInfo(ctx: Context): Pair<String, Boolean>? {
        val telephonyManager = ctx.getSystemService(Context.TELEPHONY_SERVICE) as? TelephonyManager
        val carrierName = telephonyManager?.networkOperatorName
        val isRoaming = telephonyManager?.isNetworkRoaming ?: false
        
        return if (!carrierName.isNullOrEmpty()) Pair(carrierName, isRoaming) else null
    }
    
    private fun getIpAddress(): String? {
        return try {
            val interfaces = java.net.NetworkInterface.getNetworkInterfaces()
            while (interfaces.hasMoreElements()) {
                val iface = interfaces.nextElement()
                val addresses = iface.inetAddresses
                while (addresses.hasMoreElements()) {
                    val addr = addresses.nextElement()
                    if (!addr.isLoopbackAddress && addr is java.net.Inet4Address) {
                        return addr.hostAddress
                    }
                }
            }
            null
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المراقبة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء مراقبة الشبكة / Start network monitoring
     */
    fun startMonitoring(): Boolean {
        val cm = connectivityManager ?: return false
        
        if (networkCallback != null) return true
        
        val request = NetworkRequest.Builder()
            .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
            .build()
        
        networkCallback = object : ConnectivityManager.NetworkCallback() {
            override fun onAvailable(network: Network) {
                mainHandler.post {
                    networkListeners.forEach { it.onNetworkAvailable() }
                    val info = getCurrentNetworkInfo()
                    networkListeners.forEach { it.onNetworkChanged(info) }
                }
            }
            
            override fun onLost(network: Network) {
                mainHandler.post {
                    networkListeners.forEach { it.onNetworkLost() }
                }
            }
            
            override fun onCapabilitiesChanged(
                network: Network,
                networkCapabilities: NetworkCapabilities
            ) {
                mainHandler.post {
                    val info = getCurrentNetworkInfo()
                    networkListeners.forEach { it.onNetworkChanged(info) }
                }
            }
        }
        
        cm.registerNetworkCallback(request, networkCallback!!)
        return true
    }
    
    /**
     * إيقاف المراقبة / Stop monitoring
     */
    fun stopMonitoring() {
        networkCallback?.let {
            connectivityManager?.unregisterNetworkCallback(it)
        }
        networkCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على أيقونة الشبكة / Get network icon
     */
    fun getNetworkIconName(type: ConnectionType, signalStrength: Int): String {
        return when (type) {
            ConnectionType.WIFI -> when {
                signalStrength >= 75 -> "wifi_4"
                signalStrength >= 50 -> "wifi_3"
                signalStrength >= 25 -> "wifi_2"
                else -> "wifi_1"
            }
            ConnectionType.MOBILE -> when {
                signalStrength >= 75 -> "signal_4"
                signalStrength >= 50 -> "signal_3"
                signalStrength >= 25 -> "signal_2"
                else -> "signal_1"
            }
            ConnectionType.ETHERNET -> "ethernet"
            ConnectionType.VPN -> "vpn"
            ConnectionType.NONE -> "no_signal"
            else -> "network_unknown"
        }
    }
    
    /**
     * الحصول على لون الجودة / Get quality color
     */
    fun getQualityColor(quality: ConnectionQuality): Int {
        return when (quality) {
            ConnectionQuality.EXCELLENT -> 0xFF4CAF50.toInt()
            ConnectionQuality.GOOD -> 0xFF8BC34A.toInt()
            ConnectionQuality.MODERATE -> 0xFFFFC107.toInt()
            ConnectionQuality.POOR -> 0xFFFF5722.toInt()
            ConnectionQuality.UNKNOWN -> 0xFF9E9E9E.toInt()
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopMonitoring()
        networkListeners.clear()
        connectivityManager = null
        context = null
    }
}
