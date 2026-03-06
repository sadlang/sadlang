// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadNetworkStateBridge.kt
// الوصف: جسر Kotlin لحالة الشبكة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.os.Build

/**
 * جسر حالة الشبكة — SadNetworkStateBridge
 */
object SadNetworkStateBridge {
    
    private var context: Context? = null
    private var connectivityManager: ConnectivityManager? = null
    private var networkCallback: ConnectivityManager.NetworkCallback? = null
    private var onNetworkAvailable: (() -> Unit)? = null
    private var onNetworkLost: (() -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        connectivityManager = ctx.getSystemService(Context.CONNECTIVITY_SERVICE) as? ConnectivityManager
    }
    
    @JvmStatic
    fun isConnected(): Boolean {
        val cm = connectivityManager ?: return false
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val network = cm.activeNetwork ?: return false
            val caps = cm.getNetworkCapabilities(network) ?: return false
            caps.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
        } else {
            @Suppress("DEPRECATION")
            cm.activeNetworkInfo?.isConnected == true
        }
    }
    
    @JvmStatic
    fun getConnectionType(): String {
        val cm = connectivityManager ?: return "غير متصل"
        
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val network = cm.activeNetwork ?: return "غير متصل"
            val caps = cm.getNetworkCapabilities(network) ?: return "غير متصل"
            
            when {
                caps.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) -> "wifi"
                caps.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) -> "cellular"
                caps.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) -> "ethernet"
                caps.hasTransport(NetworkCapabilities.TRANSPORT_BLUETOOTH) -> "bluetooth"
                caps.hasTransport(NetworkCapabilities.TRANSPORT_VPN) -> "vpn"
                else -> "غير معروف"
            }
        } else {
            @Suppress("DEPRECATION")
            when (cm.activeNetworkInfo?.type) {
                ConnectivityManager.TYPE_WIFI -> "wifi"
                ConnectivityManager.TYPE_MOBILE -> "cellular"
                ConnectivityManager.TYPE_ETHERNET -> "ethernet"
                ConnectivityManager.TYPE_BLUETOOTH -> "bluetooth"
                ConnectivityManager.TYPE_VPN -> "vpn"
                else -> "غير معروف"
            }
        }
    }
    
    @JvmStatic
    fun isMetered(): Boolean {
        val cm = connectivityManager ?: return true
        return cm.isActiveNetworkMetered
    }
    
    @JvmStatic
    fun getNetworkInfo(): Map<String, Any> {
        val cm = connectivityManager ?: return emptyMap()
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val network = cm.activeNetwork ?: return mapOf("connected" to false)
            val caps = cm.getNetworkCapabilities(network) ?: return mapOf("connected" to false)
            
            return mapOf(
                "connected" to true,
                "type" to getConnectionType(),
                "metered" to cm.isActiveNetworkMetered,
                "hasInternet" to caps.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET),
                "validated" to caps.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED),
                "downloadBandwidth" to caps.linkDownstreamBandwidthKbps,
                "uploadBandwidth" to caps.linkUpstreamBandwidthKbps
            )
        } else {
            @Suppress("DEPRECATION")
            val info = cm.activeNetworkInfo
            return mapOf(
                "connected" to (info?.isConnected == true),
                "type" to getConnectionType(),
                "metered" to cm.isActiveNetworkMetered
            )
        }
    }
    
    @JvmStatic
    fun registerNetworkCallback(onAvailable: () -> Unit, onLost: () -> Unit) {
        val cm = connectivityManager ?: return
        unregisterNetworkCallback()
        
        onNetworkAvailable = onAvailable
        onNetworkLost = onLost
        
        val request = NetworkRequest.Builder()
            .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
            .build()
        
        networkCallback = object : ConnectivityManager.NetworkCallback() {
            override fun onAvailable(network: Network) {
                onNetworkAvailable?.invoke()
            }
            
            override fun onLost(network: Network) {
                onNetworkLost?.invoke()
            }
        }
        
        cm.registerNetworkCallback(request, networkCallback!!)
    }
    
    @JvmStatic
    fun unregisterNetworkCallback() {
        networkCallback?.let {
            try {
                connectivityManager?.unregisterNetworkCallback(it)
            } catch (e: Exception) { /* تجاهل */ }
        }
        networkCallback = null
        onNetworkAvailable = null
        onNetworkLost = null
    }
}
