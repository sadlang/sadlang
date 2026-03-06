package sad.android.communication.local

import android.content.Context
import android.net.nsd.NsdManager
import android.net.nsd.NsdServiceInfo
import android.os.Handler
import android.os.Looper
import java.net.*
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * جسر الشبكة المحلية - اكتشاف الخدمات المحلية (mDNS/Bonjour)
 * Local Network Bridge - Local service discovery (mDNS/Bonjour/NSD)
 * 
 * @author فريق لغة ص
 */
object SadLocalNetworkBridge {
    
    private var nsdManager: NsdManager? = null
    private var context: Context? = null
    private val executor: ExecutorService = Executors.newCachedThreadPool()
    private val mainHandler = Handler(Looper.getMainLooper())
    
    private val discoveredServices = ConcurrentHashMap<String, LocalService>()
    private val registeredServices = ConcurrentHashMap<String, NsdServiceInfo>()
    private val discoveryListeners = mutableListOf<ServiceDiscoveryListener>()
    private val activeDiscoveries = ConcurrentHashMap<String, NsdManager.DiscoveryListener>()
    
    /**
     * خدمة محلية / Local service
     */
    data class LocalService(
        val name: String,
        val type: String,
        val host: String?,
        val port: Int,
        val attributes: Map<String, String> = emptyMap(),
        val resolved: Boolean = false
    )
    
    /**
     * أنواع الخدمات الشائعة / Common service types
     */
    object ServiceTypes {
        const val HTTP = "_http._tcp."
        const val HTTPS = "_https._tcp."
        const val FTP = "_ftp._tcp."
        const val SSH = "_ssh._tcp."
        const val PRINTER = "_ipp._tcp."
        const val AIRPLAY = "_airplay._tcp."
        const val HOMEKIT = "_hap._tcp."
        const val GOOGLECAST = "_googlecast._tcp."
        const val SPOTIFY = "_spotify-connect._tcp."
        const val SMB = "_smb._tcp."
        const val WEBDAV = "_webdav._tcp."
        const val CUSTOM = "_sad._tcp."
    }
    
    /**
     * مستمع اكتشاف الخدمات / Service discovery listener
     */
    interface ServiceDiscoveryListener {
        fun onServiceFound(service: LocalService)
        fun onServiceLost(serviceName: String)
        fun onDiscoveryStarted(serviceType: String)
        fun onDiscoveryStopped(serviceType: String)
        fun onError(error: String)
    }
    
    /**
     * مستمع تسجيل الخدمة / Service registration listener
     */
    interface ServiceRegistrationListener {
        fun onRegistered(serviceName: String)
        fun onUnregistered(serviceName: String)
        fun onRegistrationFailed(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        nsdManager = ctx.getSystemService(Context.NSD_SERVICE) as? NsdManager
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addDiscoveryListener(listener: ServiceDiscoveryListener) {
        if (!discoveryListeners.contains(listener)) {
            discoveryListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeDiscoveryListener(listener: ServiceDiscoveryListener) {
        discoveryListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اكتشاف الخدمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء اكتشاف الخدمات / Start service discovery
     */
    fun startDiscovery(serviceType: String = ServiceTypes.HTTP): Boolean {
        val manager = nsdManager ?: return false
        
        if (activeDiscoveries.containsKey(serviceType)) {
            return true // بالفعل يكتشف
        }
        
        val discoveryListener = object : NsdManager.DiscoveryListener {
            override fun onStartDiscoveryFailed(type: String, errorCode: Int) {
                mainHandler.post {
                    discoveryListeners.forEach {
                        it.onError("فشل بدء الاكتشاف: $errorCode")
                    }
                }
            }
            
            override fun onStopDiscoveryFailed(type: String, errorCode: Int) {
                mainHandler.post {
                    discoveryListeners.forEach {
                        it.onError("فشل إيقاف الاكتشاف: $errorCode")
                    }
                }
            }
            
            override fun onDiscoveryStarted(type: String) {
                mainHandler.post {
                    discoveryListeners.forEach { it.onDiscoveryStarted(type) }
                }
            }
            
            override fun onDiscoveryStopped(type: String) {
                activeDiscoveries.remove(serviceType)
                mainHandler.post {
                    discoveryListeners.forEach { it.onDiscoveryStopped(type) }
                }
            }
            
            override fun onServiceFound(info: NsdServiceInfo) {
                resolveService(info)
            }
            
            override fun onServiceLost(info: NsdServiceInfo) {
                discoveredServices.remove(info.serviceName)
                mainHandler.post {
                    discoveryListeners.forEach { it.onServiceLost(info.serviceName) }
                }
            }
        }
        
        try {
            manager.discoverServices(serviceType, NsdManager.PROTOCOL_DNS_SD, discoveryListener)
            activeDiscoveries[serviceType] = discoveryListener
            return true
        } catch (e: Exception) {
            return false
        }
    }
    
    /**
     * إيقاف الاكتشاف / Stop discovery
     */
    fun stopDiscovery(serviceType: String): Boolean {
        val manager = nsdManager ?: return false
        val listener = activeDiscoveries[serviceType] ?: return false
        
        try {
            manager.stopServiceDiscovery(listener)
            activeDiscoveries.remove(serviceType)
            return true
        } catch (e: Exception) {
            return false
        }
    }
    
    /**
     * إيقاف كل الاكتشافات / Stop all discoveries
     */
    fun stopAllDiscoveries() {
        activeDiscoveries.keys.toList().forEach { stopDiscovery(it) }
    }
    
    private fun resolveService(info: NsdServiceInfo) {
        val manager = nsdManager ?: return
        
        val resolveListener = object : NsdManager.ResolveListener {
            override fun onResolveFailed(serviceInfo: NsdServiceInfo, errorCode: Int) {
                // حفظ الخدمة بدون تفاصيل
                val service = LocalService(
                    name = info.serviceName,
                    type = info.serviceType,
                    host = null,
                    port = 0,
                    resolved = false
                )
                discoveredServices[info.serviceName] = service
                mainHandler.post {
                    discoveryListeners.forEach { it.onServiceFound(service) }
                }
            }
            
            override fun onServiceResolved(serviceInfo: NsdServiceInfo) {
                val attrs = mutableMapOf<String, String>()
                try {
                    serviceInfo.attributes?.forEach { (key, value) ->
                        attrs[key] = value?.toString(Charsets.UTF_8) ?: ""
                    }
                } catch (e: Exception) { }
                
                val service = LocalService(
                    name = serviceInfo.serviceName,
                    type = serviceInfo.serviceType,
                    host = serviceInfo.host?.hostAddress,
                    port = serviceInfo.port,
                    attributes = attrs,
                    resolved = true
                )
                
                discoveredServices[serviceInfo.serviceName] = service
                mainHandler.post {
                    discoveryListeners.forEach { it.onServiceFound(service) }
                }
            }
        }
        
        try {
            manager.resolveService(info, resolveListener)
        } catch (e: Exception) { }
    }
    
    /**
     * الحصول على الخدمات المكتشفة / Get discovered services
     */
    fun getDiscoveredServices(): List<LocalService> {
        return discoveredServices.values.toList()
    }
    
    /**
     * البحث عن خدمة بالاسم / Find service by name
     */
    fun findService(name: String): LocalService? {
        return discoveredServices[name]
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تسجيل الخدمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل خدمة / Register service
     */
    fun registerService(
        name: String,
        type: String,
        port: Int,
        attributes: Map<String, String> = emptyMap(),
        callback: ServiceRegistrationListener? = null
    ): Boolean {
        val manager = nsdManager ?: return false
        
        val serviceInfo = NsdServiceInfo().apply {
            serviceName = name
            serviceType = type
            setPort(port)
            
            attributes.forEach { (key, value) ->
                setAttribute(key, value)
            }
        }
        
        val registrationListener = object : NsdManager.RegistrationListener {
            override fun onRegistrationFailed(info: NsdServiceInfo, errorCode: Int) {
                mainHandler.post {
                    callback?.onRegistrationFailed("فشل التسجيل: $errorCode")
                }
            }
            
            override fun onUnregistrationFailed(info: NsdServiceInfo, errorCode: Int) {
                mainHandler.post {
                    callback?.onRegistrationFailed("فشل إلغاء التسجيل: $errorCode")
                }
            }
            
            override fun onServiceRegistered(info: NsdServiceInfo) {
                registeredServices[info.serviceName] = info
                mainHandler.post {
                    callback?.onRegistered(info.serviceName)
                }
            }
            
            override fun onServiceUnregistered(info: NsdServiceInfo) {
                registeredServices.remove(info.serviceName)
                mainHandler.post {
                    callback?.onUnregistered(info.serviceName)
                }
            }
        }
        
        try {
            manager.registerService(serviceInfo, NsdManager.PROTOCOL_DNS_SD, registrationListener)
            return true
        } catch (e: Exception) {
            return false
        }
    }
    
    /**
     * إلغاء تسجيل خدمة / Unregister service
     */
    fun unregisterService(name: String): Boolean {
        // ملاحظة: NSD لا يدعم إلغاء تسجيل خدمة فردية مباشرة
        // يجب الاحتفاظ بالـ listener عند التسجيل
        return registeredServices.remove(name) != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الشبكة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على عنوان IP المحلي / Get local IP address
     */
    fun getLocalIpAddress(): String? {
        try {
            val interfaces = NetworkInterface.getNetworkInterfaces()
            while (interfaces.hasMoreElements()) {
                val iface = interfaces.nextElement()
                if (iface.isLoopback || !iface.isUp) continue
                
                val addresses = iface.inetAddresses
                while (addresses.hasMoreElements()) {
                    val addr = addresses.nextElement()
                    if (addr is Inet4Address && !addr.isLoopbackAddress) {
                        return addr.hostAddress
                    }
                }
            }
        } catch (e: Exception) { }
        return null
    }
    
    /**
     * الحصول على جميع عناوين IP / Get all IP addresses
     */
    fun getAllIpAddresses(): List<String> {
        val addresses = mutableListOf<String>()
        try {
            val interfaces = NetworkInterface.getNetworkInterfaces()
            while (interfaces.hasMoreElements()) {
                val iface = interfaces.nextElement()
                if (iface.isLoopback || !iface.isUp) continue
                
                val addrs = iface.inetAddresses
                while (addrs.hasMoreElements()) {
                    val addr = addrs.nextElement()
                    if (!addr.isLoopbackAddress) {
                        addr.hostAddress?.let { addresses.add(it) }
                    }
                }
            }
        } catch (e: Exception) { }
        return addresses
    }
    
    /**
     * الحصول على اسم المضيف / Get hostname
     */
    fun getHostname(): String {
        return try {
            InetAddress.getLocalHost().hostName
        } catch (e: Exception) {
            "localhost"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فحص الشبكة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فحص منفذ / Check port
     */
    fun isPortOpen(host: String, port: Int, timeout: Int = 3000, callback: (Boolean) -> Unit) {
        executor.execute {
            val result = try {
                Socket().use { socket ->
                    socket.connect(InetSocketAddress(host, port), timeout)
                    true
                }
            } catch (e: Exception) {
                false
            }
            mainHandler.post { callback(result) }
        }
    }
    
    /**
     * فحص الوصول / Check reachability
     */
    fun isHostReachable(host: String, timeout: Int = 3000, callback: (Boolean) -> Unit) {
        executor.execute {
            val result = try {
                InetAddress.getByName(host).isReachable(timeout)
            } catch (e: Exception) {
                false
            }
            mainHandler.post { callback(result) }
        }
    }
    
    /**
     * DNS lookup
     */
    fun resolveDns(hostname: String, callback: (List<String>) -> Unit) {
        executor.execute {
            val addresses = try {
                InetAddress.getAllByName(hostname).mapNotNull { it.hostAddress }
            } catch (e: Exception) {
                emptyList()
            }
            mainHandler.post { callback(addresses) }
        }
    }
    
    /**
     * Reverse DNS lookup
     */
    fun reverseDns(ip: String, callback: (String?) -> Unit) {
        executor.execute {
            val hostname = try {
                InetAddress.getByName(ip).canonicalHostName
            } catch (e: Exception) {
                null
            }
            mainHandler.post { callback(hostname) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فحص الشبكة المحلية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مستمع فحص الشبكة / Network scan listener
     */
    interface NetworkScanListener {
        fun onHostFound(ip: String, hostname: String?)
        fun onScanProgress(current: Int, total: Int)
        fun onScanComplete(hosts: List<String>)
    }
    
    /**
     * فحص الشبكة المحلية / Scan local network
     */
    fun scanLocalNetwork(
        subnet: String? = null,
        timeout: Int = 1000,
        listener: NetworkScanListener
    ) {
        executor.execute {
            val baseIp = subnet ?: getLocalIpAddress()?.substringBeforeLast(".") ?: "192.168.1"
            val foundHosts = mutableListOf<String>()
            
            for (i in 1..254) {
                val ip = "$baseIp.$i"
                
                mainHandler.post {
                    listener.onScanProgress(i, 254)
                }
                
                try {
                    val addr = InetAddress.getByName(ip)
                    if (addr.isReachable(timeout)) {
                        foundHosts.add(ip)
                        val hostname = try { addr.canonicalHostName } catch (e: Exception) { null }
                        mainHandler.post {
                            listener.onHostFound(ip, hostname)
                        }
                    }
                } catch (e: Exception) { }
            }
            
            mainHandler.post {
                listener.onScanComplete(foundHosts)
            }
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopAllDiscoveries()
        discoveredServices.clear()
        registeredServices.clear()
        discoveryListeners.clear()
        executor.shutdown()
        nsdManager = null
        context = null
    }
}
