package sad.android.communication.ble

import android.Manifest
import android.bluetooth.*
import android.bluetooth.le.*
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.os.ParcelUuid
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat
import java.util.UUID

/**
 * جسر BLE - البلوتوث منخفض الطاقة
 * BLE Bridge - Bluetooth Low Energy
 * 
 * @author فريق لغة ص
 */
object SadBLEBridge {
    
    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bleScanner: BluetoothLeScanner? = null
    private var gatt: BluetoothGatt? = null
    private var scanCallback: ScanCallback? = null
    private val handler = Handler(Looper.getMainLooper())
    
    /**
     * حالة الاتصال / Connection state
     */
    enum class ConnectionState {
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        DISCONNECTING
    }
    
    /**
     * جهاز BLE / BLE device
     */
    data class BleDevice(
        val name: String?,
        val address: String,
        val rssi: Int,
        val scanRecord: ByteArray?,
        val serviceUuids: List<UUID>,
        val isConnectable: Boolean
    )
    
    /**
     * خدمة GATT / GATT service
     */
    data class GattService(
        val uuid: UUID,
        val isPrimary: Boolean,
        val characteristics: List<GattCharacteristic>
    )
    
    /**
     * خاصية GATT / GATT characteristic
     */
    data class GattCharacteristic(
        val uuid: UUID,
        val properties: Int,
        val isReadable: Boolean,
        val isWritable: Boolean,
        val isNotifiable: Boolean
    )
    
    /**
     * مستمع الفحص / Scan listener
     */
    interface BleScanListener {
        fun onDeviceFound(device: BleDevice)
        fun onScanFailed(errorCode: Int)
    }
    
    /**
     * مستمع GATT / GATT listener
     */
    interface GattListener {
        fun onConnectionStateChange(state: ConnectionState)
        fun onServicesDiscovered(services: List<GattService>)
        fun onCharacteristicRead(uuid: UUID, value: ByteArray)
        fun onCharacteristicWrite(uuid: UUID, success: Boolean)
        fun onCharacteristicChanged(uuid: UUID, value: ByteArray)
        fun onError(message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة BLE / Initialize BLE
     */
    fun initialize(context: Context): Boolean {
        val bluetoothManager = context.getSystemService(Context.BLUETOOTH_SERVICE) as? BluetoothManager
        bluetoothAdapter = bluetoothManager?.adapter
        bleScanner = bluetoothAdapter?.bluetoothLeScanner
        return bluetoothAdapter != null
    }
    
    /**
     * هل BLE مدعوم؟ / Is BLE supported?
     */
    fun isSupported(context: Context): Boolean {
        return context.packageManager.hasSystemFeature(android.content.pm.PackageManager.FEATURE_BLUETOOTH_LE)
    }
    
    /**
     * هل البلوتوث مفعل؟ / Is Bluetooth enabled?
     */
    fun isEnabled(context: Context): Boolean {
        if (bluetoothAdapter == null) initialize(context)
        return bluetoothAdapter?.isEnabled == true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الفحص
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الفحص / Start scan
     */
    fun startScan(
        context: Context,
        listener: BleScanListener,
        timeoutMs: Long = 10000,
        serviceUuids: List<UUID>? = null
    ): Boolean {
        if (!hasPermissions(context)) return false
        if (!isEnabled(context)) return false
        
        val scanner = bleScanner ?: bluetoothAdapter?.bluetoothLeScanner ?: return false
        
        scanCallback = object : ScanCallback() {
            override fun onScanResult(callbackType: Int, result: ScanResult?) {
                result?.let { scanResult ->
                    try {
                        val device = BleDevice(
                            name = scanResult.device.name,
                            address = scanResult.device.address,
                            rssi = scanResult.rssi,
                            scanRecord = scanResult.scanRecord?.bytes,
                            serviceUuids = scanResult.scanRecord?.serviceUuids?.map { it.uuid } ?: emptyList(),
                            isConnectable = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                                scanResult.isConnectable
                            } else true
                        )
                        listener.onDeviceFound(device)
                    } catch (e: SecurityException) {}
                }
            }
            
            override fun onScanFailed(errorCode: Int) {
                listener.onScanFailed(errorCode)
            }
        }
        
        // إعدادات الفحص
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()
        
        // فلاتر الخدمات
        val filters = serviceUuids?.map { uuid ->
            ScanFilter.Builder()
                .setServiceUuid(ParcelUuid(uuid))
                .build()
        }
        
        try {
            if (filters != null) {
                scanner.startScan(filters, settings, scanCallback)
            } else {
                scanner.startScan(null, settings, scanCallback)
            }
            
            // إيقاف تلقائي بعد المهلة
            handler.postDelayed({
                stopScan(context)
            }, timeoutMs)
            
            return true
        } catch (e: SecurityException) {
            return false
        }
    }
    
    /**
     * إيقاف الفحص / Stop scan
     */
    fun stopScan(context: Context) {
        scanCallback?.let { callback ->
            try {
                bleScanner?.stopScan(callback)
            } catch (e: SecurityException) {}
        }
        scanCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بجهاز / Connect to device
     */
    fun connect(
        context: Context,
        address: String,
        listener: GattListener
    ): Boolean {
        if (!hasPermissions(context)) {
            listener.onError("لا توجد صلاحية")
            return false
        }
        
        val adapter = bluetoothAdapter ?: run {
            initialize(context)
            bluetoothAdapter
        } ?: return false
        
        val device = try {
            adapter.getRemoteDevice(address)
        } catch (e: Exception) {
            listener.onError("عنوان غير صالح")
            return false
        }
        
        val gattCallback = object : BluetoothGattCallback() {
            override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
                val state = when (newState) {
                    BluetoothProfile.STATE_CONNECTED -> {
                        try { gatt?.discoverServices() } catch (e: SecurityException) {}
                        ConnectionState.CONNECTED
                    }
                    BluetoothProfile.STATE_DISCONNECTED -> ConnectionState.DISCONNECTED
                    BluetoothProfile.STATE_CONNECTING -> ConnectionState.CONNECTING
                    BluetoothProfile.STATE_DISCONNECTING -> ConnectionState.DISCONNECTING
                    else -> ConnectionState.DISCONNECTED
                }
                listener.onConnectionStateChange(state)
            }
            
            override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    val services = gatt?.services?.map { service ->
                        GattService(
                            uuid = service.uuid,
                            isPrimary = service.type == BluetoothGattService.SERVICE_TYPE_PRIMARY,
                            characteristics = service.characteristics.map { char ->
                                GattCharacteristic(
                                    uuid = char.uuid,
                                    properties = char.properties,
                                    isReadable = (char.properties and BluetoothGattCharacteristic.PROPERTY_READ) != 0,
                                    isWritable = (char.properties and BluetoothGattCharacteristic.PROPERTY_WRITE) != 0,
                                    isNotifiable = (char.properties and BluetoothGattCharacteristic.PROPERTY_NOTIFY) != 0
                                )
                            }
                        )
                    } ?: emptyList()
                    listener.onServicesDiscovered(services)
                }
            }
            
            @Deprecated("Deprecated in API 33")
            override fun onCharacteristicRead(
                gatt: BluetoothGatt?,
                characteristic: BluetoothGattCharacteristic?,
                status: Int
            ) {
                if (status == BluetoothGatt.GATT_SUCCESS && characteristic != null) {
                    listener.onCharacteristicRead(characteristic.uuid, characteristic.value ?: ByteArray(0))
                }
            }
            
            override fun onCharacteristicWrite(
                gatt: BluetoothGatt?,
                characteristic: BluetoothGattCharacteristic?,
                status: Int
            ) {
                characteristic?.let {
                    listener.onCharacteristicWrite(it.uuid, status == BluetoothGatt.GATT_SUCCESS)
                }
            }
            
            @Deprecated("Deprecated in API 33")
            override fun onCharacteristicChanged(
                gatt: BluetoothGatt?,
                characteristic: BluetoothGattCharacteristic?
            ) {
                characteristic?.let {
                    listener.onCharacteristicChanged(it.uuid, it.value ?: ByteArray(0))
                }
            }
        }
        
        gatt = try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                device.connectGatt(context, false, gattCallback, BluetoothDevice.TRANSPORT_LE)
            } else {
                device.connectGatt(context, false, gattCallback)
            }
        } catch (e: SecurityException) {
            listener.onError("فشل الاتصال")
            null
        }
        
        return gatt != null
    }
    
    /**
     * قطع الاتصال / Disconnect
     */
    fun disconnect() {
        try {
            gatt?.disconnect()
            gatt?.close()
        } catch (e: SecurityException) {}
        gatt = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         القراءة والكتابة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * قراءة خاصية / Read characteristic
     */
    fun readCharacteristic(serviceUuid: UUID, charUuid: UUID): Boolean {
        val service = gatt?.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(charUuid) ?: return false
        
        return try {
            gatt?.readCharacteristic(characteristic) == true
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * كتابة خاصية / Write characteristic
     */
    fun writeCharacteristic(serviceUuid: UUID, charUuid: UUID, value: ByteArray): Boolean {
        val service = gatt?.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(charUuid) ?: return false
        
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                gatt?.writeCharacteristic(
                    characteristic,
                    value,
                    BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
                ) == BluetoothStatusCodes.SUCCESS
            } else {
                @Suppress("DEPRECATION")
                characteristic.value = value
                gatt?.writeCharacteristic(characteristic) == true
            }
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * تفعيل الإشعارات / Enable notifications
     */
    fun enableNotifications(serviceUuid: UUID, charUuid: UUID): Boolean {
        val service = gatt?.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(charUuid) ?: return false
        
        return try {
            gatt?.setCharacteristicNotification(characteristic, true)
            
            // كتابة descriptor
            val descriptor = characteristic.getDescriptor(
                UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")
            )
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                gatt?.writeDescriptor(
                    descriptor,
                    BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                ) == BluetoothStatusCodes.SUCCESS
            } else {
                @Suppress("DEPRECATION")
                descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                gatt?.writeDescriptor(descriptor) == true
            }
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * تعطيل الإشعارات / Disable notifications
     */
    fun disableNotifications(serviceUuid: UUID, charUuid: UUID): Boolean {
        val service = gatt?.getService(serviceUuid) ?: return false
        val characteristic = service.getCharacteristic(charUuid) ?: return false
        
        return try {
            gatt?.setCharacteristicNotification(characteristic, false) == true
        } catch (e: SecurityException) {
            false
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
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            arrayOf(
                Manifest.permission.BLUETOOTH_SCAN,
                Manifest.permission.BLUETOOTH_CONNECT,
                Manifest.permission.ACCESS_FINE_LOCATION
            )
        } else {
            arrayOf(
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN,
                Manifest.permission.ACCESS_FINE_LOCATION
            )
        }
    }
}
