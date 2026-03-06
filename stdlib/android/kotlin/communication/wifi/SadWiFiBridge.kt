package sad.android.communication.wifi

import android.Manifest
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.net.wifi.ScanResult
import android.net.wifi.WifiConfiguration
import android.net.wifi.WifiManager
import android.net.wifi.WifiNetworkSpecifier
import android.net.wifi.p2p.WifiP2pManager
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat

/**
 * جسر Wi-Fi - إدارة الاتصال اللاسلكي
 * Wi-Fi Bridge - Wireless connectivity management
 * 
 * @author فريق لغة ص
 */
object SadWiFiBridge {
    
    private var wifiManager: WifiManager? = null
    private var connectivityManager: ConnectivityManager? = null
    private var scanReceiver: BroadcastReceiver? = null
    
    /**
     * حالة Wi-Fi / Wi-Fi state
     */
    enum class WifiState {
        DISABLED,       // معطل
        DISABLING,      // جاري التعطيل
        ENABLED,        // مفعل
        ENABLING,       // جاري التفعيل
        UNKNOWN         // غير معروف
    }
    
    /**
     * معلومات الشبكة / Network info
     */
    data class WifiNetwork(
        val ssid: String,
        val bssid: String,
        val level: Int,         // قوة الإشارة
        val frequency: Int,     // التردد MHz
        val capabilities: String,
        val isSecured: Boolean,
        val is5GHz: Boolean,
        val channelWidth: String?
    )
    
    /**
     * معلومات الاتصال الحالي / Current connection info
     */
    data class ConnectionInfo(
        val ssid: String,
        val bssid: String,
        val ipAddress: String,
        val linkSpeed: Int,     // Mbps
        val rssi: Int,          // قوة الإشارة dBm
        val frequency: Int,
        val isConnected: Boolean
    )
    
    /**
     * مستمع الفحص / Scan listener
     */
    interface WifiScanListener {
        fun onScanResults(networks: List<WifiNetwork>)
        fun onScanFailed()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(context: Context) {
        wifiManager = context.applicationContext
            .getSystemService(Context.WIFI_SERVICE) as WifiManager
        connectivityManager = context.getSystemService(Context.CONNECTIVITY_SERVICE) 
            as ConnectivityManager
    }
    
    private fun getWifiManager(context: Context): WifiManager {
        return wifiManager ?: (context.applicationContext
            .getSystemService(Context.WIFI_SERVICE) as WifiManager).also {
            wifiManager = it
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحالة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل Wi-Fi مفعل؟ / Is Wi-Fi enabled?
     */
    fun isWifiEnabled(context: Context): Boolean {
        return getWifiManager(context).isWifiEnabled
    }
    
    /**
     * جلب حالة Wi-Fi / Get Wi-Fi state
     */
    fun getWifiState(context: Context): WifiState {
        return when (getWifiManager(context).wifiState) {
            WifiManager.WIFI_STATE_DISABLED -> WifiState.DISABLED
            WifiManager.WIFI_STATE_DISABLING -> WifiState.DISABLING
            WifiManager.WIFI_STATE_ENABLED -> WifiState.ENABLED
            WifiManager.WIFI_STATE_ENABLING -> WifiState.ENABLING
            else -> WifiState.UNKNOWN
        }
    }
    
    /**
     * تفعيل/تعطيل Wi-Fi (لا يعمل بعد Android Q) / Enable/Disable Wi-Fi
     */
    @Suppress("DEPRECATION")
    fun setWifiEnabled(context: Context, enabled: Boolean): Boolean {
        return if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            getWifiManager(context).isWifiEnabled = enabled
            true
        } else {
            // في Android 10+ يجب فتح إعدادات Wi-Fi
            false
        }
    }
    
    /**
     * فتح إعدادات Wi-Fi / Open Wi-Fi settings
     */
    fun openWifiSettings(context: Context) {
        val intent = Intent(android.provider.Settings.ACTION_WIFI_SETTINGS)
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الفحص
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء فحص الشبكات / Start scan
     */
    fun startScan(context: Context, listener: WifiScanListener) {
        if (!hasPermissions(context)) {
            listener.onScanFailed()
            return
        }
        
        val wm = getWifiManager(context)
        
        // تسجيل المستمع
        scanReceiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                val success = intent?.getBooleanExtra(
                    WifiManager.EXTRA_RESULTS_UPDATED, false
                ) ?: false
                
                if (success) {
                    val results = parseScanResults(wm.scanResults)
                    listener.onScanResults(results)
                } else {
                    // استخدام النتائج القديمة
                    val results = parseScanResults(wm.scanResults)
                    listener.onScanResults(results)
                }
                
                // إلغاء التسجيل
                try { context.unregisterReceiver(this) } catch (_: Exception) {}
            }
        }
        
        context.registerReceiver(
            scanReceiver,
            IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)
        )
        
        // بدء الفحص
        @Suppress("DEPRECATION")
        if (!wm.startScan()) {
            listener.onScanFailed()
        }
    }
    
    /**
     * جلب نتائج الفحص الحالية / Get current scan results
     */
    fun getScanResults(context: Context): List<WifiNetwork> {
        if (!hasPermissions(context)) return emptyList()
        return parseScanResults(getWifiManager(context).scanResults)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بشبكة (Android 10+) / Connect to network
     */
    @RequiresApi(Build.VERSION_CODES.Q)
    fun connectToNetwork(
        context: Context,
        ssid: String,
        password: String?,
        onSuccess: () -> Unit,
        onFailure: () -> Unit
    ) {
        val specifier = WifiNetworkSpecifier.Builder()
            .setSsid(ssid)
            .apply {
                password?.let { setWpa2Passphrase(it) }
            }
            .build()
        
        val request = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
            .setNetworkSpecifier(specifier)
            .build()
        
        val cm = context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        
        val callback = object : ConnectivityManager.NetworkCallback() {
            override fun onAvailable(network: Network) {
                cm.bindProcessToNetwork(network)
                onSuccess()
            }
            
            override fun onUnavailable() {
                onFailure()
            }
        }
        
        cm.requestNetwork(request, callback)
    }
    
    /**
     * الاتصال بشبكة (للأجهزة القديمة) / Connect to network (legacy)
     */
    @Suppress("DEPRECATION")
    fun connectToNetworkLegacy(
        context: Context,
        ssid: String,
        password: String?,
        isWPA: Boolean = true
    ): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) return false
        
        val wm = getWifiManager(context)
        
        val config = WifiConfiguration().apply {
            SSID = "\"$ssid\""
            
            if (password != null) {
                if (isWPA) {
                    preSharedKey = "\"$password\""
                } else {
                    wepKeys[0] = "\"$password\""
                    wepTxKeyIndex = 0
                    allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE)
                    allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40)
                }
            } else {
                allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE)
            }
        }
        
        val networkId = wm.addNetwork(config)
        if (networkId == -1) return false
        
        wm.disconnect()
        val success = wm.enableNetwork(networkId, true)
        wm.reconnect()
        
        return success
    }
    
    /**
     * قطع الاتصال / Disconnect
     */
    @Suppress("DEPRECATION")
    fun disconnect(context: Context): Boolean {
        return getWifiManager(context).disconnect()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب معلومات الاتصال الحالي / Get current connection info
     */
    @Suppress("DEPRECATION")
    fun getConnectionInfo(context: Context): ConnectionInfo? {
        val wm = getWifiManager(context)
        val info = wm.connectionInfo ?: return null
        
        if (info.ssid == null || info.ssid == "<unknown ssid>") return null
        
        return ConnectionInfo(
            ssid = info.ssid.trim('"'),
            bssid = info.bssid ?: "",
            ipAddress = intToIpAddress(info.ipAddress),
            linkSpeed = info.linkSpeed,
            rssi = info.rssi,
            frequency = info.frequency,
            isConnected = info.networkId != -1
        )
    }
    
    /**
     * التحقق من الاتصال بالإنترنت / Check internet connection
     */
    fun hasInternetConnection(context: Context): Boolean {
        val cm = context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val network = cm.activeNetwork ?: return false
            val capabilities = cm.getNetworkCapabilities(network) ?: return false
            capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET) &&
            capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED)
        } else {
            @Suppress("DEPRECATION")
            cm.activeNetworkInfo?.isConnected == true
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    fun hasPermissions(context: Context): Boolean {
        return requiredPermissions().all {
            ContextCompat.checkSelfPermission(context, it) == PackageManager.PERMISSION_GRANTED
        }
    }
    
    fun requiredPermissions(): Array<String> {
        val permissions = mutableListOf(
            Manifest.permission.ACCESS_WIFI_STATE,
            Manifest.permission.ACCESS_FINE_LOCATION
        )
        
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            permissions.add(Manifest.permission.CHANGE_WIFI_STATE)
        }
        
        return permissions.toTypedArray()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         Hotspot (نقطة اتصال)
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح إعدادات نقطة الاتصال / Open hotspot settings
     */
    fun openHotspotSettings(context: Context) {
        val intent = Intent().apply {
            action = "android.settings.TETHER_SETTINGS"
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun parseScanResults(results: List<ScanResult>): List<WifiNetwork> {
        return results.map { result ->
            WifiNetwork(
                ssid = result.SSID,
                bssid = result.BSSID,
                level = WifiManager.calculateSignalLevel(result.level, 5),
                frequency = result.frequency,
                capabilities = result.capabilities,
                isSecured = result.capabilities.contains("WPA") || 
                           result.capabilities.contains("WEP"),
                is5GHz = result.frequency > 4900,
                channelWidth = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    when (result.channelWidth) {
                        ScanResult.CHANNEL_WIDTH_20MHZ -> "20MHz"
                        ScanResult.CHANNEL_WIDTH_40MHZ -> "40MHz"
                        ScanResult.CHANNEL_WIDTH_80MHZ -> "80MHz"
                        ScanResult.CHANNEL_WIDTH_160MHZ -> "160MHz"
                        else -> null
                    }
                } else null
            )
        }.distinctBy { it.ssid }.sortedByDescending { it.level }
    }
    
    private fun intToIpAddress(ip: Int): String {
        return "${ip and 0xFF}.${(ip shr 8) and 0xFF}.${(ip shr 16) and 0xFF}.${(ip shr 24) and 0xFF}"
    }
}
