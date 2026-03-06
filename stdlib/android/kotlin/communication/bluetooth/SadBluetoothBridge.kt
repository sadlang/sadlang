package sad.android.communication.bluetooth

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat
import java.io.IOException
import java.util.UUID

/**
 * جسر البلوتوث - إدارة الاتصال بالبلوتوث
 * Bluetooth Bridge - Bluetooth connectivity management
 * 
 * @author فريق لغة ص
 */
object SadBluetoothBridge {
    
    private var bluetoothAdapter: BluetoothAdapter? = null
    private var discoveryReceiver: BroadcastReceiver? = null
    private var activeSocket: BluetoothSocket? = null
    
    // UUID معياري للاتصال التسلسلي
    private val SPP_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
    
    /**
     * حالة البلوتوث / Bluetooth state
     */
    enum class BluetoothState {
        OFF,            // مغلق
        TURNING_ON,     // جاري التشغيل
        ON,             // مفتوح
        TURNING_OFF,    // جاري الإغلاق
        NOT_SUPPORTED   // غير مدعوم
    }
    
    /**
     * حالة الإقران / Bond state
     */
    enum class BondState {
        NONE,           // غير مقترن
        BONDING,        // جاري الإقران
        BONDED          // مقترن
    }
    
    /**
     * جهاز بلوتوث / Bluetooth device
     */
    data class BtDevice(
        val name: String?,
        val address: String,
        val bondState: BondState,
        val type: Int,
        val rssi: Int? = null
    )
    
    /**
     * مستمع الاكتشاف / Discovery listener
     */
    interface DiscoveryListener {
        fun onDeviceFound(device: BtDevice)
        fun onDiscoveryStarted()
        fun onDiscoveryFinished()
    }
    
    /**
     * مستمع الاتصال / Connection listener
     */
    interface ConnectionListener {
        fun onConnected()
        fun onDisconnected()
        fun onDataReceived(data: ByteArray)
        fun onError(message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة البلوتوث / Initialize Bluetooth
     */
    fun initialize(context: Context): Boolean {
        val bluetoothManager = context.getSystemService(Context.BLUETOOTH_SERVICE) as? BluetoothManager
        bluetoothAdapter = bluetoothManager?.adapter
        return bluetoothAdapter != null
    }
    
    private fun getAdapter(context: Context): BluetoothAdapter? {
        if (bluetoothAdapter == null) {
            initialize(context)
        }
        return bluetoothAdapter
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحالة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل البلوتوث مدعوم؟ / Is Bluetooth supported?
     */
    fun isSupported(context: Context): Boolean {
        return getAdapter(context) != null
    }
    
    /**
     * هل البلوتوث مفعل؟ / Is Bluetooth enabled?
     */
    fun isEnabled(context: Context): Boolean {
        return getAdapter(context)?.isEnabled == true
    }
    
    /**
     * جلب الحالة / Get state
     */
    fun getState(context: Context): BluetoothState {
        val adapter = getAdapter(context) ?: return BluetoothState.NOT_SUPPORTED
        
        return when (adapter.state) {
            BluetoothAdapter.STATE_OFF -> BluetoothState.OFF
            BluetoothAdapter.STATE_TURNING_ON -> BluetoothState.TURNING_ON
            BluetoothAdapter.STATE_ON -> BluetoothState.ON
            BluetoothAdapter.STATE_TURNING_OFF -> BluetoothState.TURNING_OFF
            else -> BluetoothState.OFF
        }
    }
    
    /**
     * فتح إعدادات البلوتوث / Open Bluetooth settings
     */
    fun openSettings(context: Context) {
        val intent = Intent(android.provider.Settings.ACTION_BLUETOOTH_SETTINGS)
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        context.startActivity(intent)
    }
    
    /**
     * طلب تفعيل البلوتوث / Request enabling Bluetooth
     */
    fun requestEnable(context: Context) {
        val intent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأجهزة المقترنة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب الأجهزة المقترنة / Get paired devices
     */
    fun getPairedDevices(context: Context): List<BtDevice> {
        if (!hasPermissions(context)) return emptyList()
        
        val adapter = getAdapter(context) ?: return emptyList()
        
        return try {
            adapter.bondedDevices?.map { device ->
                BtDevice(
                    name = device.name,
                    address = device.address,
                    bondState = BondState.BONDED,
                    type = device.type
                )
            } ?: emptyList()
        } catch (e: SecurityException) {
            emptyList()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاكتشاف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الاكتشاف / Start discovery
     */
    fun startDiscovery(context: Context, listener: DiscoveryListener): Boolean {
        if (!hasPermissions(context)) return false
        
        val adapter = getAdapter(context) ?: return false
        
        // إلغاء اكتشاف سابق
        try {
            adapter.cancelDiscovery()
        } catch (e: SecurityException) {
            return false
        }
        
        // تسجيل المستمع
        discoveryReceiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                when (intent?.action) {
                    BluetoothDevice.ACTION_FOUND -> {
                        val device = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            intent.getParcelableExtra(
                                BluetoothDevice.EXTRA_DEVICE,
                                BluetoothDevice::class.java
                            )
                        } else {
                            @Suppress("DEPRECATION")
                            intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)
                        }
                        
                        device?.let {
                            try {
                                val rssi = intent.getShortExtra(BluetoothDevice.EXTRA_RSSI, Short.MIN_VALUE).toInt()
                                listener.onDeviceFound(BtDevice(
                                    name = it.name,
                                    address = it.address,
                                    bondState = getBondState(it.bondState),
                                    type = it.type,
                                    rssi = if (rssi != Short.MIN_VALUE.toInt()) rssi else null
                                ))
                            } catch (e: SecurityException) {}
                        }
                    }
                    BluetoothAdapter.ACTION_DISCOVERY_STARTED -> {
                        listener.onDiscoveryStarted()
                    }
                    BluetoothAdapter.ACTION_DISCOVERY_FINISHED -> {
                        listener.onDiscoveryFinished()
                        try { context.unregisterReceiver(this) } catch (_: Exception) {}
                    }
                }
            }
        }
        
        val filter = IntentFilter().apply {
            addAction(BluetoothDevice.ACTION_FOUND)
            addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED)
            addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)
        }
        context.registerReceiver(discoveryReceiver, filter)
        
        return try {
            adapter.startDiscovery()
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * إيقاف الاكتشاف / Stop discovery
     */
    fun stopDiscovery(context: Context) {
        try {
            getAdapter(context)?.cancelDiscovery()
            discoveryReceiver?.let {
                try { context.unregisterReceiver(it) } catch (_: Exception) {}
            }
            discoveryReceiver = null
        } catch (e: SecurityException) {}
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
        uuid: UUID = SPP_UUID,
        listener: ConnectionListener
    ) {
        if (!hasPermissions(context)) {
            listener.onError("لا توجد صلاحية")
            return
        }
        
        val adapter = getAdapter(context)
        if (adapter == null) {
            listener.onError("البلوتوث غير متاح")
            return
        }
        
        // إيقاف الاكتشاف أولاً
        try { adapter.cancelDiscovery() } catch (e: SecurityException) {}
        
        Thread {
            try {
                val device = adapter.getRemoteDevice(address)
                activeSocket = device.createRfcommSocketToServiceRecord(uuid)
                activeSocket?.connect()
                listener.onConnected()
                
                // بدء قراءة البيانات
                val buffer = ByteArray(1024)
                val inputStream = activeSocket?.inputStream
                
                while (activeSocket?.isConnected == true) {
                    try {
                        val bytesRead = inputStream?.read(buffer) ?: -1
                        if (bytesRead > 0) {
                            val data = buffer.copyOf(bytesRead)
                            listener.onDataReceived(data)
                        }
                    } catch (e: IOException) {
                        break
                    }
                }
                
                listener.onDisconnected()
            } catch (e: SecurityException) {
                listener.onError("لا توجد صلاحية")
            } catch (e: IOException) {
                listener.onError(e.message ?: "فشل الاتصال")
            }
        }.start()
    }
    
    /**
     * قطع الاتصال / Disconnect
     */
    fun disconnect() {
        try {
            activeSocket?.close()
        } catch (e: IOException) {}
        activeSocket = null
    }
    
    /**
     * إرسال بيانات / Send data
     */
    fun send(data: ByteArray): Boolean {
        return try {
            activeSocket?.outputStream?.write(data)
            activeSocket?.outputStream?.flush()
            true
        } catch (e: IOException) {
            false
        }
    }
    
    /**
     * إرسال نص / Send text
     */
    fun sendText(text: String): Boolean {
        return send(text.toByteArray(Charsets.UTF_8))
    }
    
    /**
     * هل متصل؟ / Is connected?
     */
    fun isConnected(): Boolean {
        return activeSocket?.isConnected == true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإقران
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب الإقران / Request pairing
     */
    fun requestPairing(context: Context, address: String): Boolean {
        if (!hasPermissions(context)) return false
        
        val adapter = getAdapter(context) ?: return false
        
        return try {
            val device = adapter.getRemoteDevice(address)
            device.createBond()
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * إلغاء الإقران / Remove pairing
     */
    fun removePairing(context: Context, address: String): Boolean {
        if (!hasPermissions(context)) return false
        
        val adapter = getAdapter(context) ?: return false
        
        return try {
            val device = adapter.getRemoteDevice(address)
            val method = device.javaClass.getMethod("removeBond")
            method.invoke(device) as? Boolean ?: false
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الجهاز
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * اسم الجهاز / Device name
     */
    fun getDeviceName(context: Context): String? {
        return try {
            getAdapter(context)?.name
        } catch (e: SecurityException) {
            null
        }
    }
    
    /**
     * عنوان الجهاز / Device address
     */
    fun getDeviceAddress(context: Context): String? {
        return try {
            getAdapter(context)?.address
        } catch (e: SecurityException) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         قابلية الاكتشاف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب جعل الجهاز قابل للاكتشاف / Request discoverability
     */
    fun requestDiscoverability(context: Context, durationSeconds: Int = 300) {
        val intent = Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE).apply {
            putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, durationSeconds)
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        context.startActivity(intent)
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
                Manifest.permission.BLUETOOTH_ADVERTISE,
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
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getBondState(state: Int): BondState {
        return when (state) {
            BluetoothDevice.BOND_NONE -> BondState.NONE
            BluetoothDevice.BOND_BONDING -> BondState.BONDING
            BluetoothDevice.BOND_BONDED -> BondState.BONDED
            else -> BondState.NONE
        }
    }
}
