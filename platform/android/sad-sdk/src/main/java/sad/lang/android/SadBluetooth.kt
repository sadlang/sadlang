/**
 * =============================================================================
 * @file SadBluetooth.kt
 * @brief (AR) واجهة البلوتوث لأندرويد
 * @brief (EN) Android Bluetooth Interface
 * =============================================================================
 */

package sad.lang.android

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.*
import android.bluetooth.le.*
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.ParcelUuid
import androidx.core.content.ContextCompat
import java.util.*

/**
 * نظام البلوتوث للغة ص
 */
class SadBluetooth(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * جهاز بلوتوث
     */
    data class BluetoothDevice(
        val name: String?,
        val address: String,
        val type: Int,
        val bondState: Int,
        val rssi: Int = 0
    )
    
    /**
     * مستمع الفحص
     */
    interface ScanListener {
        fun onDeviceFound(device: BluetoothDevice)
        fun onScanFinished()
        fun onError(message: String)
    }
    
    /**
     * مستمع الاتصال
     */
    interface ConnectionListener {
        fun onConnected()
        fun onDisconnected()
        fun onDataReceived(data: ByteArray)
        fun onError(message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val bluetoothManager: BluetoothManager? by lazy {
        context.getSystemService(Context.BLUETOOTH_SERVICE) as? BluetoothManager
    }
    
    private val bluetoothAdapter: BluetoothAdapter? by lazy {
        bluetoothManager?.adapter
    }
    
    private val bleScanner: BluetoothLeScanner? by lazy {
        bluetoothAdapter?.bluetoothLeScanner
    }
    
    private var scanCallback: ScanCallback? = null
    private var connectedGatt: BluetoothGatt? = null
    private var connectionListener: ConnectionListener? = null
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Availability
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل البلوتوث متاح؟
     */
    fun isAvailable(): Boolean {
        return bluetoothAdapter != null
    }
    
    /**
     * هل البلوتوث مُفعّل؟
     */
    fun isEnabled(): Boolean {
        return bluetoothAdapter?.isEnabled == true
    }
    
    /**
     * هل BLE متاح؟
     */
    fun isBleSupported(): Boolean {
        return context.packageManager.hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)
    }
    
    /**
     * هل لدينا الأذونات المطلوبة؟
     */
    fun hasPermissions(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            ContextCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED &&
            ContextCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED
        } else {
            ContextCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH) == PackageManager.PERMISSION_GRANTED &&
            ContextCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_ADMIN) == PackageManager.PERMISSION_GRANTED
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Scanning
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء البحث عن أجهزة BLE
     */
    @SuppressLint("MissingPermission")
    fun startScan(listener: ScanListener, serviceUuids: List<UUID>? = null) {
        if (!hasPermissions()) {
            listener.onError("الأذونات غير متاحة")
            return
        }
        
        if (!isEnabled()) {
            listener.onError("البلوتوث غير مُفعّل")
            return
        }
        
        stopScan()
        
        scanCallback = object : ScanCallback() {
            override fun onScanResult(callbackType: Int, result: ScanResult) {
                val device = BluetoothDevice(
                    name = result.device.name,
                    address = result.device.address,
                    type = result.device.type,
                    bondState = result.device.bondState,
                    rssi = result.rssi
                )
                listener.onDeviceFound(device)
            }
            
            override fun onScanFailed(errorCode: Int) {
                listener.onError("فشل البحث: $errorCode")
            }
        }
        
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()
        
        val filters = serviceUuids?.map { uuid ->
            ScanFilter.Builder()
                .setServiceUuid(ParcelUuid(uuid))
                .build()
        }
        
        bleScanner?.startScan(filters, settings, scanCallback)
    }
    
    /**
     * إيقاف البحث
     */
    @SuppressLint("MissingPermission")
    fun stopScan() {
        scanCallback?.let {
            try {
                bleScanner?.stopScan(it)
            } catch (e: Exception) {
                // Ignore
            }
        }
        scanCallback = null
    }
    
    /**
     * الحصول على الأجهزة المقترنة
     */
    @SuppressLint("MissingPermission")
    fun getBondedDevices(): List<BluetoothDevice> {
        if (!hasPermissions()) return emptyList()
        
        return bluetoothAdapter?.bondedDevices?.map { device ->
            BluetoothDevice(
                name = device.name,
                address = device.address,
                type = device.type,
                bondState = device.bondState
            )
        } ?: emptyList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Connection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بجهاز BLE
     */
    @SuppressLint("MissingPermission")
    fun connect(address: String, listener: ConnectionListener) {
        if (!hasPermissions()) {
            listener.onError("الأذونات غير متاحة")
            return
        }
        
        disconnect()
        
        val device = bluetoothAdapter?.getRemoteDevice(address)
        if (device == null) {
            listener.onError("الجهاز غير موجود")
            return
        }
        
        connectionListener = listener
        
        val gattCallback = object : BluetoothGattCallback() {
            override fun onConnectionStateChange(gatt: BluetoothGatt, status: Int, newState: Int) {
                when (newState) {
                    BluetoothProfile.STATE_CONNECTED -> {
                        connectedGatt = gatt
                        gatt.discoverServices()
                        listener.onConnected()
                    }
                    BluetoothProfile.STATE_DISCONNECTED -> {
                        connectedGatt = null
                        listener.onDisconnected()
                    }
                }
            }
            
            override fun onCharacteristicRead(
                gatt: BluetoothGatt,
                characteristic: BluetoothGattCharacteristic,
                status: Int
            ) {
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    listener.onDataReceived(characteristic.value)
                }
            }
            
            @Deprecated("Deprecated in Java")
            override fun onCharacteristicChanged(
                gatt: BluetoothGatt,
                characteristic: BluetoothGattCharacteristic
            ) {
                listener.onDataReceived(characteristic.value)
            }
        }
        
        device.connectGatt(context, false, gattCallback)
    }
    
    /**
     * قطع الاتصال
     */
    @SuppressLint("MissingPermission")
    fun disconnect() {
        connectedGatt?.disconnect()
        connectedGatt?.close()
        connectedGatt = null
        connectionListener = null
    }
    
    /**
     * هل متصل؟
     */
    fun isConnected(): Boolean {
        return connectedGatt != null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Data Transfer
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * كتابة بيانات
     */
    @SuppressLint("MissingPermission")
    fun write(serviceUuid: UUID, characteristicUuid: UUID, data: ByteArray): Boolean {
        val gatt = connectedGatt ?: return false
        
        val service = gatt.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(characteristicUuid) ?: return false
        
        characteristic.value = data
        return gatt.writeCharacteristic(characteristic)
    }
    
    /**
     * قراءة بيانات
     */
    @SuppressLint("MissingPermission")
    fun read(serviceUuid: UUID, characteristicUuid: UUID): Boolean {
        val gatt = connectedGatt ?: return false
        
        val service = gatt.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(characteristicUuid) ?: return false
        
        return gatt.readCharacteristic(characteristic)
    }
    
    /**
     * تفعيل الإشعارات
     */
    @SuppressLint("MissingPermission")
    fun enableNotifications(serviceUuid: UUID, characteristicUuid: UUID): Boolean {
        val gatt = connectedGatt ?: return false
        
        val service = gatt.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(characteristicUuid) ?: return false
        
        gatt.setCharacteristicNotification(characteristic, true)
        
        // Enable notification descriptor
        val descriptor = characteristic.getDescriptor(
            UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")
        )
        descriptor?.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
        return gatt.writeDescriptor(descriptor)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Cleanup
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنظيف الموارد
     */
    fun release() {
        stopScan()
        disconnect()
    }
}
