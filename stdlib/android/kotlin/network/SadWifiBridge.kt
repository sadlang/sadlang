// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadWifiBridge.kt
// الوصف: جسر Kotlin للواي فاي
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.wifi.WifiInfo
import android.net.wifi.WifiManager
import android.os.Build
import androidx.core.app.ActivityCompat

/**
 * جسر الواي فاي — SadWifiBridge
 */
object SadWifiBridge {
    
    private var context: Context? = null
    private var wifiManager: WifiManager? = null
    private var connectivityManager: ConnectivityManager? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        wifiManager = ctx.applicationContext.getSystemService(Context.WIFI_SERVICE) as? WifiManager
        connectivityManager = ctx.getSystemService(Context.CONNECTIVITY_SERVICE) as? ConnectivityManager
    }
    
    @JvmStatic
    fun isEnabled(): Boolean = wifiManager?.isWifiEnabled == true
    
    @JvmStatic
    fun isConnected(): Boolean {
        val cm = connectivityManager ?: return false
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val network = cm.activeNetwork ?: return false
            val caps = cm.getNetworkCapabilities(network) ?: return false
            caps.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)
        } else {
            @Suppress("DEPRECATION")
            cm.activeNetworkInfo?.type == ConnectivityManager.TYPE_WIFI
        }
    }
    
    @JvmStatic
    fun getConnectionInfo(): Map<String, Any> {
        val ctx = context ?: return emptyMap()
        val wifi = wifiManager ?: return emptyMap()
        
        if (ActivityCompat.checkSelfPermission(ctx, Manifest.permission.ACCESS_FINE_LOCATION)
            != PackageManager.PERMISSION_GRANTED) {
            return mapOf("error" to "يتطلب إذن الموقع")
        }
        
        @Suppress("DEPRECATION")
        val info = wifi.connectionInfo ?: return emptyMap()
        
        return mapOf(
            "ssid" to (info.ssid?.replace("\"", "") ?: "غير معروف"),
            "bssid" to (info.bssid ?: "غير معروف"),
            "rssi" to info.rssi,
            "linkSpeed" to info.linkSpeed,
            "linkSpeedUnits" to WifiInfo.LINK_SPEED_UNITS,
            "frequency" to if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) info.frequency else 0,
            "ipAddress" to formatIpAddress(info.ipAddress)
        )
    }
    
    @JvmStatic
    fun getSignalStrength(): Int {
        val wifi = wifiManager ?: return -1
        @Suppress("DEPRECATION")
        val rssi = wifi.connectionInfo?.rssi ?: return -1
        return WifiManager.calculateSignalLevel(rssi, 5)
    }
    
    @JvmStatic
    fun getIpAddress(): String {
        val wifi = wifiManager ?: return ""
        @Suppress("DEPRECATION")
        val ip = wifi.connectionInfo?.ipAddress ?: return ""
        return formatIpAddress(ip)
    }
    
    private fun formatIpAddress(ip: Int): String {
        return "${ip and 0xFF}.${ip shr 8 and 0xFF}.${ip shr 16 and 0xFF}.${ip shr 24 and 0xFF}"
    }
}
