package sad.android.communication.usb

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.*
import android.os.Build
import java.nio.ByteBuffer
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر USB - الاتصال بأجهزة USB
 * USB Bridge - USB device communication
 * 
 * @author فريق لغة ص
 */
object SadUsbBridge {
    
    private const val ACTION_USB_PERMISSION = "sad.android.USB_PERMISSION"
    
    private var usbManager: UsbManager? = null
    private val openConnections = ConcurrentHashMap<String, UsbDeviceConnection>()
    private var permissionReceiver: BroadcastReceiver? = null
    
    /**
     * معلومات جهاز USB / USB device info
     */
    data class UsbDeviceInfo(
        val name: String,
        val deviceId: Int,
        val vendorId: Int,
        val productId: Int,
        val deviceClass: Int,
        val deviceSubclass: Int,
        val deviceProtocol: Int,
        val manufacturerName: String?,
        val productName: String?,
        val serialNumber: String?,
        val interfaceCount: Int,
        val version: String?
    )
    
    /**
     * معلومات الواجهة / Interface info
     */
    data class UsbInterfaceInfo(
        val id: Int,
        val interfaceClass: Int,
        val interfaceSubclass: Int,
        val interfaceProtocol: Int,
        val endpointCount: Int,
        val alternateSetting: Int
    )
    
    /**
     * معلومات نقطة النهاية / Endpoint info
     */
    data class UsbEndpointInfo(
        val address: Int,
        val direction: Int, // USB_DIR_IN or USB_DIR_OUT
        val type: Int, // BULK, INTERRUPT, ISOCHRONOUS, CONTROL
        val maxPacketSize: Int,
        val interval: Int
    )
    
    /**
     * مستمع USB / USB listener
     */
    interface UsbListener {
        fun onDeviceAttached(device: UsbDeviceInfo)
        fun onDeviceDetached(device: UsbDeviceInfo)
        fun onPermissionGranted(device: UsbDeviceInfo)
        fun onPermissionDenied(device: UsbDeviceInfo)
        fun onDataReceived(deviceId: String, data: ByteArray)
        fun onError(error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة الجسر / Initialize bridge
     */
    fun initialize(context: Context, listener: UsbListener? = null) {
        usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        
        listener?.let { registerListener(context, it) }
    }
    
    private fun registerListener(context: Context, listener: UsbListener) {
        if (permissionReceiver != null) return
        
        permissionReceiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                when (intent?.action) {
                    ACTION_USB_PERMISSION -> {
                        val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                        } else {
                            @Suppress("DEPRECATION")
                            intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                        }
                        
                        val granted = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)
                        
                        device?.let {
                            val info = deviceToInfo(it)
                            if (granted) {
                                listener.onPermissionGranted(info)
                            } else {
                                listener.onPermissionDenied(info)
                            }
                        }
                    }
                    
                    UsbManager.ACTION_USB_DEVICE_ATTACHED -> {
                        val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                        } else {
                            @Suppress("DEPRECATION")
                            intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                        }
                        
                        device?.let { listener.onDeviceAttached(deviceToInfo(it)) }
                    }
                    
                    UsbManager.ACTION_USB_DEVICE_DETACHED -> {
                        val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                        } else {
                            @Suppress("DEPRECATION")
                            intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                        }
                        
                        device?.let { 
                            listener.onDeviceDetached(deviceToInfo(it))
                            openConnections.remove(it.deviceId.toString())?.close()
                        }
                    }
                }
            }
        }
        
        val filter = IntentFilter().apply {
            addAction(ACTION_USB_PERMISSION)
            addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED)
            addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
        }
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            context.registerReceiver(permissionReceiver, filter, Context.RECEIVER_NOT_EXPORTED)
        } else {
            context.registerReceiver(permissionReceiver, filter)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اكتشاف الأجهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جميع أجهزة USB / Get all USB devices
     */
    fun getConnectedDevices(): List<UsbDeviceInfo> {
        val devices = usbManager?.deviceList ?: return emptyList()
        return devices.values.map { deviceToInfo(it) }
    }
    
    /**
     * البحث عن جهاز بـ Vendor ID و Product ID / Find device by VID/PID
     */
    fun findDevice(vendorId: Int, productId: Int): UsbDeviceInfo? {
        return getConnectedDevices().find { 
            it.vendorId == vendorId && it.productId == productId 
        }
    }
    
    /**
     * البحث عن أجهزة بفئة معينة / Find devices by class
     */
    fun findDevicesByClass(deviceClass: Int): List<UsbDeviceInfo> {
        return getConnectedDevices().filter { it.deviceClass == deviceClass }
    }
    
    /**
     * الحصول على واجهات الجهاز / Get device interfaces
     */
    fun getInterfaces(deviceId: Int): List<UsbInterfaceInfo> {
        val device = findDeviceById(deviceId) ?: return emptyList()
        
        return (0 until device.interfaceCount).map { i ->
            val iface = device.getInterface(i)
            UsbInterfaceInfo(
                id = iface.id,
                interfaceClass = iface.interfaceClass,
                interfaceSubclass = iface.interfaceSubclass,
                interfaceProtocol = iface.interfaceProtocol,
                endpointCount = iface.endpointCount,
                alternateSetting = iface.alternateSetting
            )
        }
    }
    
    /**
     * الحصول على نقاط النهاية / Get endpoints
     */
    fun getEndpoints(deviceId: Int, interfaceIndex: Int): List<UsbEndpointInfo> {
        val device = findDeviceById(deviceId) ?: return emptyList()
        if (interfaceIndex >= device.interfaceCount) return emptyList()
        
        val iface = device.getInterface(interfaceIndex)
        return (0 until iface.endpointCount).map { i ->
            val endpoint = iface.getEndpoint(i)
            UsbEndpointInfo(
                address = endpoint.address,
                direction = endpoint.direction,
                type = endpoint.type,
                maxPacketSize = endpoint.maxPacketSize,
                interval = endpoint.interval
            )
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الصلاحيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل لدينا صلاحية؟ / Has permission?
     */
    fun hasPermission(deviceId: Int): Boolean {
        val device = findDeviceById(deviceId) ?: return false
        return usbManager?.hasPermission(device) == true
    }
    
    /**
     * طلب صلاحية / Request permission
     */
    fun requestPermission(context: Context, deviceId: Int): Boolean {
        val device = findDeviceById(deviceId) ?: return false
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_MUTABLE
        } else {
            0
        }
        
        val permissionIntent = PendingIntent.getBroadcast(
            context, 0, Intent(ACTION_USB_PERMISSION), flags
        )
        
        usbManager?.requestPermission(device, permissionIntent)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح اتصال / Open connection
     */
    fun openConnection(deviceId: Int): Boolean {
        val device = findDeviceById(deviceId) ?: return false
        
        if (openConnections.containsKey(deviceId.toString())) {
            return true // متصل بالفعل
        }
        
        val connection = usbManager?.openDevice(device) ?: return false
        openConnections[deviceId.toString()] = connection
        
        return true
    }
    
    /**
     * إغلاق الاتصال / Close connection
     */
    fun closeConnection(deviceId: Int): Boolean {
        val connection = openConnections.remove(deviceId.toString()) ?: return false
        connection.close()
        return true
    }
    
    /**
     * المطالبة بالواجهة / Claim interface
     */
    fun claimInterface(deviceId: Int, interfaceIndex: Int, force: Boolean = false): Boolean {
        val device = findDeviceById(deviceId) ?: return false
        val connection = openConnections[deviceId.toString()] ?: return false
        
        if (interfaceIndex >= device.interfaceCount) return false
        
        val iface = device.getInterface(interfaceIndex)
        return connection.claimInterface(iface, force)
    }
    
    /**
     * تحرير الواجهة / Release interface
     */
    fun releaseInterface(deviceId: Int, interfaceIndex: Int): Boolean {
        val device = findDeviceById(deviceId) ?: return false
        val connection = openConnections[deviceId.toString()] ?: return false
        
        if (interfaceIndex >= device.interfaceCount) return false
        
        val iface = device.getInterface(interfaceIndex)
        return connection.releaseInterface(iface)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         القراءة والكتابة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال بيانات bulk / Send bulk data
     */
    fun bulkTransfer(
        deviceId: Int,
        interfaceIndex: Int,
        endpointIndex: Int,
        data: ByteArray,
        timeout: Int = 1000
    ): Int {
        val device = findDeviceById(deviceId) ?: return -1
        val connection = openConnections[deviceId.toString()] ?: return -1
        
        if (interfaceIndex >= device.interfaceCount) return -1
        
        val iface = device.getInterface(interfaceIndex)
        if (endpointIndex >= iface.endpointCount) return -1
        
        val endpoint = iface.getEndpoint(endpointIndex)
        return connection.bulkTransfer(endpoint, data, data.size, timeout)
    }
    
    /**
     * استقبال بيانات bulk / Receive bulk data
     */
    fun bulkReceive(
        deviceId: Int,
        interfaceIndex: Int,
        endpointIndex: Int,
        bufferSize: Int = 1024,
        timeout: Int = 1000
    ): ByteArray? {
        val device = findDeviceById(deviceId) ?: return null
        val connection = openConnections[deviceId.toString()] ?: return null
        
        if (interfaceIndex >= device.interfaceCount) return null
        
        val iface = device.getInterface(interfaceIndex)
        if (endpointIndex >= iface.endpointCount) return null
        
        val endpoint = iface.getEndpoint(endpointIndex)
        val buffer = ByteArray(bufferSize)
        
        val received = connection.bulkTransfer(endpoint, buffer, buffer.size, timeout)
        return if (received > 0) buffer.copyOf(received) else null
    }
    
    /**
     * طلب control / Control request
     */
    fun controlTransfer(
        deviceId: Int,
        requestType: Int,
        request: Int,
        value: Int,
        index: Int,
        data: ByteArray? = null,
        timeout: Int = 1000
    ): Int {
        val connection = openConnections[deviceId.toString()] ?: return -1
        
        return connection.controlTransfer(
            requestType, request, value, index,
            data, data?.size ?: 0, timeout
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun findDeviceById(deviceId: Int): UsbDevice? {
        return usbManager?.deviceList?.values?.find { it.deviceId == deviceId }
    }
    
    private fun deviceToInfo(device: UsbDevice): UsbDeviceInfo {
        return UsbDeviceInfo(
            name = device.deviceName,
            deviceId = device.deviceId,
            vendorId = device.vendorId,
            productId = device.productId,
            deviceClass = device.deviceClass,
            deviceSubclass = device.deviceSubclass,
            deviceProtocol = device.deviceProtocol,
            manufacturerName = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) device.manufacturerName else null,
            productName = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) device.productName else null,
            serialNumber = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) device.serialNumber else null,
            interfaceCount = device.interfaceCount,
            version = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) device.version else null
        )
    }
    
    /**
     * فئات USB / USB classes
     */
    object UsbClass {
        const val AUDIO = 1
        const val CDC_CONTROL = 2
        const val HID = 3
        const val PHYSICAL = 5
        const val STILL_IMAGE = 6
        const val PRINTER = 7
        const val MASS_STORAGE = 8
        const val HUB = 9
        const val CDC_DATA = 10
        const val SMART_CARD = 11
        const val CONTENT_SECURITY = 13
        const val VIDEO = 14
        const val WIRELESS_CONTROLLER = 224
        const val MISC = 239
        const val APP_SPECIFIC = 254
        const val VENDOR_SPECIFIC = 255
    }
    
    /**
     * تنظيف الموارد / Release resources
     */
    fun release(context: Context) {
        permissionReceiver?.let {
            try {
                context.unregisterReceiver(it)
            } catch (e: Exception) {}
        }
        permissionReceiver = null
        
        openConnections.values.forEach { it.close() }
        openConnections.clear()
    }
}
