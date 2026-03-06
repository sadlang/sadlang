// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBluetoothBridge.kt
// الوصف: جسر Kotlin للبلوتوث
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import androidx.core.app.ActivityCompat

/**
 * جسر البلوتوث — SadBluetoothBridge
 */
object SadBluetoothBridge {
    
    private var context: Context? = null
    private var bluetoothManager: BluetoothManager? = null
    private var bluetoothAdapter: BluetoothAdapter? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        bluetoothManager = ctx.getSystemService(Context.BLUETOOTH_SERVICE) as? BluetoothManager
        bluetoothAdapter = bluetoothManager?.adapter
    }
    
    @JvmStatic
    fun isEnabled(): Boolean = bluetoothAdapter?.isEnabled == true
    
    @JvmStatic
    fun isSupported(): Boolean = bluetoothAdapter != null
    
    @JvmStatic
    fun getPairedDevices(): List<Map<String, String>> {
        val ctx = context ?: return emptyList()
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(ctx, Manifest.permission.BLUETOOTH_CONNECT) 
                != PackageManager.PERMISSION_GRANTED) {
                return emptyList()
            }
        }
        
        return bluetoothAdapter?.bondedDevices?.map { device ->
            mapOf(
                "name" to (device.name ?: "غير معروف"),
                "address" to device.address,
                "type" to when (device.type) {
                    BluetoothDevice.DEVICE_TYPE_CLASSIC -> "classic"
                    BluetoothDevice.DEVICE_TYPE_LE -> "ble"
                    BluetoothDevice.DEVICE_TYPE_DUAL -> "dual"
                    else -> "unknown"
                }
            )
        } ?: emptyList()
    }
    
    @JvmStatic
    fun getDeviceName(): String {
        val ctx = context ?: return ""
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(ctx, Manifest.permission.BLUETOOTH_CONNECT)
                != PackageManager.PERMISSION_GRANTED) {
                return ""
            }
        }
        return bluetoothAdapter?.name ?: ""
    }
}
