// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadUsbBridge.kt
// الوصف: جسر Kotlin لاتصال USB (Host Mode)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.*
import android.os.Build
import java.nio.ByteBuffer

/**
 * جسر USB — SadUsbBridge
 */
object SadUsbBridge {
    
    private const val ACTION_USB_PERMISSION = "com.sad.USB_PERMISSION"
    
    private var context: Context? = null
    private var usbManager: UsbManager? = null
    private var currentConnection: UsbDeviceConnection? = null
    private var currentEndpointIn: UsbEndpoint? = null
    private var currentEndpointOut: UsbEndpoint? = null
    
    // Callbacks
    private var onDeviceAttachedCallback: ((Map<String, Any>) -> Unit)? = null
    private var onDeviceDetachedCallback: ((Map<String, Any>) -> Unit)? = null
    private var onPermissionGrantedCallback: ((Boolean) -> Unit)? = null
    private var onDataReceivedCallback: ((ByteArray) -> Unit)? = null
    
    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                UsbManager.ACTION_USB_DEVICE_ATTACHED -> {
                    val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                    } else {
                        @Suppress("DEPRECATION")
                        intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                    }
                    device?.let {
                        onDeviceAttachedCallback?.invoke(deviceToMap(it))
                    }
                }
                UsbManager.ACTION_USB_DEVICE_DETACHED -> {
                    val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                    } else {
                        @Suppress("DEPRECATION")
                        intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                    }
                    device?.let {
                        onDeviceDetachedCallback?.invoke(deviceToMap(it))
                    }
                }
                ACTION_USB_PERMISSION -> {
                    val granted = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)
                    onPermissionGrantedCallback?.invoke(granted)
                }
            }
        }
    }
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        usbManager = ctx.getSystemService(Context.USB_SERVICE) as? UsbManager
        
        val filter = IntentFilter().apply {
            addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED)
            addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
            addAction(ACTION_USB_PERMISSION)
        }
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            ctx.registerReceiver(usbReceiver, filter, Context.RECEIVER_NOT_EXPORTED)
        } else {
            ctx.registerReceiver(usbReceiver, filter)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Device Discovery
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getConnectedDevices(): List<Map<String, Any>> {
        val manager = usbManager ?: return emptyList()
        return manager.deviceList.values.map { deviceToMap(it) }
    }
    
    @JvmStatic
    fun getDeviceCount(): Int {
        return usbManager?.deviceList?.size ?: 0
    }
    
    @JvmStatic
    fun findDeviceByVendorId(vendorId: Int): Map<String, Any>? {
        val manager = usbManager ?: return null
        return manager.deviceList.values
            .firstOrNull { it.vendorId == vendorId }
            ?.let { deviceToMap(it) }
    }
    
    @JvmStatic
    fun findDeviceByProductId(productId: Int): Map<String, Any>? {
        val manager = usbManager ?: return null
        return manager.deviceList.values
            .firstOrNull { it.productId == productId }
            ?.let { deviceToMap(it) }
    }
    
    @JvmStatic
    fun findDevice(vendorId: Int, productId: Int): Map<String, Any>? {
        val manager = usbManager ?: return null
        return manager.deviceList.values
            .firstOrNull { it.vendorId == vendorId && it.productId == productId }
            ?.let { deviceToMap(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Permissions
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun hasPermission(deviceName: String): Boolean {
        val manager = usbManager ?: return false
        val device = manager.deviceList[deviceName] ?: return false
        return manager.hasPermission(device)
    }
    
    @JvmStatic
    fun requestPermission(deviceName: String): Boolean {
        val ctx = context ?: return false
        val manager = usbManager ?: return false
        val device = manager.deviceList[deviceName] ?: return false
        
        val permissionIntent = PendingIntent.getBroadcast(
            ctx,
            0,
            Intent(ACTION_USB_PERMISSION),
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )
        
        manager.requestPermission(device, permissionIntent)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Connection
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun openDevice(deviceName: String): Boolean {
        val manager = usbManager ?: return false
        val device = manager.deviceList[deviceName] ?: return false
        
        if (!manager.hasPermission(device)) {
            return false
        }
        
        currentConnection = manager.openDevice(device)
        return currentConnection != null
    }
    
    @JvmStatic
    fun closeDevice() {
        currentConnection?.close()
        currentConnection = null
        currentEndpointIn = null
        currentEndpointOut = null
    }
    
    @JvmStatic
    fun isConnected(): Boolean {
        return currentConnection != null
    }
    
    @JvmStatic
    fun claimInterface(deviceName: String, interfaceIndex: Int, force: Boolean = true): Boolean {
        val manager = usbManager ?: return false
        val device = manager.deviceList[deviceName] ?: return false
        val connection = currentConnection ?: return false
        
        if (interfaceIndex >= device.interfaceCount) return false
        
        val usbInterface = device.getInterface(interfaceIndex)
        return connection.claimInterface(usbInterface, force)
    }
    
    @JvmStatic
    fun releaseInterface(deviceName: String, interfaceIndex: Int): Boolean {
        val manager = usbManager ?: return false
        val device = manager.deviceList[deviceName] ?: return false
        val connection = currentConnection ?: return false
        
        if (interfaceIndex >= device.interfaceCount) return false
        
        val usbInterface = device.getInterface(interfaceIndex)
        return connection.releaseInterface(usbInterface)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Endpoints
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setupEndpoints(deviceName: String, interfaceIndex: Int): Boolean {
        val manager = usbManager ?: return false
        val device = manager.deviceList[deviceName] ?: return false
        
        if (interfaceIndex >= device.interfaceCount) return false
        
        val usbInterface = device.getInterface(interfaceIndex)
        
        for (i in 0 until usbInterface.endpointCount) {
            val endpoint = usbInterface.getEndpoint(i)
            
            when (endpoint.direction) {
                UsbConstants.USB_DIR_IN -> currentEndpointIn = endpoint
                UsbConstants.USB_DIR_OUT -> currentEndpointOut = endpoint
            }
        }
        
        return currentEndpointIn != null || currentEndpointOut != null
    }
    
    @JvmStatic
    fun getEndpointInfo(deviceName: String, interfaceIndex: Int): List<Map<String, Any>> {
        val manager = usbManager ?: return emptyList()
        val device = manager.deviceList[deviceName] ?: return emptyList()
        
        if (interfaceIndex >= device.interfaceCount) return emptyList()
        
        val usbInterface = device.getInterface(interfaceIndex)
        val endpoints = mutableListOf<Map<String, Any>>()
        
        for (i in 0 until usbInterface.endpointCount) {
            val endpoint = usbInterface.getEndpoint(i)
            endpoints.add(mapOf(
                "index" to i,
                "address" to endpoint.address,
                "direction" to when (endpoint.direction) {
                    UsbConstants.USB_DIR_IN -> "IN"
                    UsbConstants.USB_DIR_OUT -> "OUT"
                    else -> "غير معروف"
                },
                "directionArabic" to when (endpoint.direction) {
                    UsbConstants.USB_DIR_IN -> "وارد"
                    UsbConstants.USB_DIR_OUT -> "صادر"
                    else -> "غير معروف"
                },
                "type" to when (endpoint.type) {
                    UsbConstants.USB_ENDPOINT_XFER_CONTROL -> "Control"
                    UsbConstants.USB_ENDPOINT_XFER_ISOC -> "Isochronous"
                    UsbConstants.USB_ENDPOINT_XFER_BULK -> "Bulk"
                    UsbConstants.USB_ENDPOINT_XFER_INT -> "Interrupt"
                    else -> "غير معروف"
                },
                "maxPacketSize" to endpoint.maxPacketSize,
                "interval" to endpoint.interval
            ))
        }
        
        return endpoints
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Data Transfer
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun sendData(data: ByteArray, timeout: Int = 1000): Int {
        val connection = currentConnection ?: return -1
        val endpoint = currentEndpointOut ?: return -1
        
        return connection.bulkTransfer(endpoint, data, data.size, timeout)
    }
    
    @JvmStatic
    fun receiveData(maxLength: Int, timeout: Int = 1000): ByteArray? {
        val connection = currentConnection ?: return null
        val endpoint = currentEndpointIn ?: return null
        
        val buffer = ByteArray(maxLength)
        val bytesRead = connection.bulkTransfer(endpoint, buffer, maxLength, timeout)
        
        return if (bytesRead > 0) {
            buffer.copyOf(bytesRead)
        } else {
            null
        }
    }
    
    @JvmStatic
    fun controlTransfer(
        requestType: Int,
        request: Int,
        value: Int,
        index: Int,
        data: ByteArray?,
        timeout: Int = 1000
    ): Int {
        val connection = currentConnection ?: return -1
        
        return connection.controlTransfer(
            requestType,
            request,
            value,
            index,
            data,
            data?.size ?: 0,
            timeout
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnDeviceAttachedListener(callback: (Map<String, Any>) -> Unit) {
        onDeviceAttachedCallback = callback
    }
    
    @JvmStatic
    fun setOnDeviceDetachedListener(callback: (Map<String, Any>) -> Unit) {
        onDeviceDetachedCallback = callback
    }
    
    @JvmStatic
    fun setOnPermissionResultListener(callback: (Boolean) -> Unit) {
        onPermissionGrantedCallback = callback
    }
    
    @JvmStatic
    fun setOnDataReceivedListener(callback: (ByteArray) -> Unit) {
        onDataReceivedCallback = callback
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun deviceToMap(device: UsbDevice): Map<String, Any> {
        return mapOf(
            "deviceName" to device.deviceName,
            "deviceId" to device.deviceId,
            "vendorId" to device.vendorId,
            "productId" to device.productId,
            "deviceClass" to device.deviceClass,
            "deviceSubclass" to device.deviceSubclass,
            "deviceProtocol" to device.deviceProtocol,
            "interfaceCount" to device.interfaceCount,
            "manufacturerName" to (device.manufacturerName ?: "غير معروف"),
            "productName" to (device.productName ?: "غير معروف"),
            "serialNumber" to (device.serialNumber ?: ""),
            "version" to device.version
        )
    }
    
    @JvmStatic
    fun getDeviceClassDescription(deviceClass: Int): String {
        return when (deviceClass) {
            UsbConstants.USB_CLASS_APP_SPEC -> "تطبيقي"
            UsbConstants.USB_CLASS_AUDIO -> "صوت"
            UsbConstants.USB_CLASS_CDC_DATA -> "بيانات CDC"
            UsbConstants.USB_CLASS_COMM -> "اتصالات"
            UsbConstants.USB_CLASS_CONTENT_SEC -> "أمان محتوى"
            UsbConstants.USB_CLASS_CSCID -> "بطاقات ذكية"
            UsbConstants.USB_CLASS_HID -> "واجهة بشرية"
            UsbConstants.USB_CLASS_HUB -> "موزع"
            UsbConstants.USB_CLASS_MASS_STORAGE -> "تخزين كبير"
            UsbConstants.USB_CLASS_MISC -> "متنوع"
            UsbConstants.USB_CLASS_PER_INTERFACE -> "حسب الواجهة"
            UsbConstants.USB_CLASS_PHYSICA -> "فيزيائي"
            UsbConstants.USB_CLASS_PRINTER -> "طابعة"
            UsbConstants.USB_CLASS_STILL_IMAGE -> "صور ثابتة"
            UsbConstants.USB_CLASS_VENDOR_SPEC -> "خاص بالشركة"
            UsbConstants.USB_CLASS_VIDEO -> "فيديو"
            UsbConstants.USB_CLASS_WIRELESS_CONTROLLER -> "تحكم لاسلكي"
            else -> "غير معروف"
        }
    }
    
    @JvmStatic
    fun cleanup() {
        closeDevice()
        try {
            context?.unregisterReceiver(usbReceiver)
        } catch (e: Exception) {
            // Already unregistered
        }
    }
}
