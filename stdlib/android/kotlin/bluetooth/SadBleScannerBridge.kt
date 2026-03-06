// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBleScannerBridge.kt
// الوصف: جسر Kotlin لمسح أجهزة BLE
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothManager
import android.bluetooth.le.BluetoothLeScanner
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import androidx.core.app.ActivityCompat

/**
 * جسر مسح BLE — SadBleScannerBridge
 */
object SadBleScannerBridge {
    
    private var context: Context? = null
    private var bluetoothAdapter: BluetoothAdapter? = null
    private var bleScanner: BluetoothLeScanner? = null
    private var isScanning = false
    private var discoveredDevices = mutableListOf<Map<String, Any>>()
    private var scanCallback: ScanCallback? = null
    private val handler = Handler(Looper.getMainLooper())
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        val manager = ctx.getSystemService(Context.BLUETOOTH_SERVICE) as? BluetoothManager
        bluetoothAdapter = manager?.adapter
        bleScanner = bluetoothAdapter?.bluetoothLeScanner
    }
    
    @JvmStatic
    fun startScan(durationMs: Long = 10000): Boolean {
        val ctx = context ?: return false
        val scanner = bleScanner ?: return false
        
        if (isScanning) return false
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(ctx, Manifest.permission.BLUETOOTH_SCAN)
                != PackageManager.PERMISSION_GRANTED) {
                return false
            }
        }
        
        discoveredDevices.clear()
        
        scanCallback = object : ScanCallback() {
            override fun onScanResult(callbackType: Int, result: ScanResult) {
                val device = result.device
                val name = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                    if (ActivityCompat.checkSelfPermission(ctx, Manifest.permission.BLUETOOTH_CONNECT)
                        == PackageManager.PERMISSION_GRANTED) {
                        device.name ?: "غير معروف"
                    } else "غير معروف"
                } else {
                    device.name ?: "غير معروف"
                }
                
                val deviceInfo = mapOf(
                    "name" to name,
                    "address" to device.address,
                    "rssi" to result.rssi,
                    "timestamp" to result.timestampNanos
                )
                
                if (discoveredDevices.none { it["address"] == device.address }) {
                    discoveredDevices.add(deviceInfo)
                }
            }
            
            override fun onScanFailed(errorCode: Int) {
                isScanning = false
            }
        }
        
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
            .build()
        
        scanner.startScan(null, settings, scanCallback)
        isScanning = true
        
        handler.postDelayed({
            stopScan()
        }, durationMs)
        
        return true
    }
    
    @JvmStatic
    fun stopScan(): Boolean {
        val ctx = context ?: return false
        if (!isScanning) return false
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(ctx, Manifest.permission.BLUETOOTH_SCAN)
                != PackageManager.PERMISSION_GRANTED) {
                return false
            }
        }
        
        scanCallback?.let { bleScanner?.stopScan(it) }
        isScanning = false
        return true
    }
    
    @JvmStatic
    fun isScanning(): Boolean = isScanning
    
    @JvmStatic
    fun getDiscoveredDevices(): List<Map<String, Any>> = discoveredDevices.toList()
    
    @JvmStatic
    fun clearDevices() {
        discoveredDevices.clear()
    }
}
