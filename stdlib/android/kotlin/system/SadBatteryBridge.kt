// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBatteryBridge.kt
// الوصف: جسر Kotlin لمعلومات البطارية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import android.os.Build

/**
 * جسر البطارية — SadBatteryBridge
 */
object SadBatteryBridge {
    
    private var context: Context? = null
    private var batteryManager: BatteryManager? = null
    private var batteryReceiver: BroadcastReceiver? = null
    
    private var onBatteryChanged: ((Map<String, Any>) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        batteryManager = ctx.getSystemService(Context.BATTERY_SERVICE) as? BatteryManager
    }
    
    @JvmStatic
    fun getLevel(): Int {
        val ctx = context ?: return -1
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        
        val level = intent?.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) ?: -1
        val scale = intent?.getIntExtra(BatteryManager.EXTRA_SCALE, 100) ?: 100
        
        return if (level >= 0 && scale > 0) (level * 100) / scale else -1
    }
    
    @JvmStatic
    fun isCharging(): Boolean {
        val ctx = context ?: return false
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        val status = intent?.getIntExtra(BatteryManager.EXTRA_STATUS, -1) ?: -1
        
        return status == BatteryManager.BATTERY_STATUS_CHARGING ||
               status == BatteryManager.BATTERY_STATUS_FULL
    }
    
    @JvmStatic
    fun getChargingType(): String {
        val ctx = context ?: return "غير معروف"
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        val plugged = intent?.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1) ?: -1
        
        return when (plugged) {
            BatteryManager.BATTERY_PLUGGED_AC -> "شاحن كهربائي"
            BatteryManager.BATTERY_PLUGGED_USB -> "USB"
            BatteryManager.BATTERY_PLUGGED_WIRELESS -> "لاسلكي"
            else -> "غير متصل"
        }
    }
    
    @JvmStatic
    fun getHealth(): String {
        val ctx = context ?: return "غير معروف"
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        val health = intent?.getIntExtra(BatteryManager.EXTRA_HEALTH, -1) ?: -1
        
        return when (health) {
            BatteryManager.BATTERY_HEALTH_GOOD -> "جيدة"
            BatteryManager.BATTERY_HEALTH_OVERHEAT -> "سخونة زائدة"
            BatteryManager.BATTERY_HEALTH_DEAD -> "تالفة"
            BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE -> "جهد زائد"
            BatteryManager.BATTERY_HEALTH_COLD -> "باردة جداً"
            BatteryManager.BATTERY_HEALTH_UNSPECIFIED_FAILURE -> "فشل غير محدد"
            else -> "غير معروف"
        }
    }
    
    @JvmStatic
    fun getTemperature(): Float {
        val ctx = context ?: return 0f
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        val temp = intent?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0) ?: 0
        return temp / 10f  // درجة مئوية
    }
    
    @JvmStatic
    fun getVoltage(): Float {
        val ctx = context ?: return 0f
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        val voltage = intent?.getIntExtra(BatteryManager.EXTRA_VOLTAGE, 0) ?: 0
        return voltage / 1000f  // فولت
    }
    
    @JvmStatic
    fun getTechnology(): String {
        val ctx = context ?: return ""
        val intent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        return intent?.getStringExtra(BatteryManager.EXTRA_TECHNOLOGY) ?: ""
    }
    
    @JvmStatic
    fun getBatteryInfo(): Map<String, Any> {
        return mapOf(
            "level" to getLevel(),
            "isCharging" to isCharging(),
            "chargingType" to getChargingType(),
            "health" to getHealth(),
            "temperature" to getTemperature(),
            "voltage" to getVoltage(),
            "technology" to getTechnology()
        )
    }
    
    @JvmStatic
    fun isLowBattery(): Boolean = getLevel() <= 15
    
    @JvmStatic
    fun isCriticalBattery(): Boolean = getLevel() <= 5
    
    @JvmStatic
    fun registerBatteryChangeListener(callback: (Map<String, Any>) -> Unit) {
        val ctx = context ?: return
        onBatteryChanged = callback
        
        batteryReceiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context, intent: Intent) {
                onBatteryChanged?.invoke(getBatteryInfo())
            }
        }
        
        val filter = IntentFilter(Intent.ACTION_BATTERY_CHANGED)
        ctx.registerReceiver(batteryReceiver, filter)
    }
    
    @JvmStatic
    fun unregisterBatteryChangeListener() {
        batteryReceiver?.let {
            try {
                context?.unregisterReceiver(it)
            } catch (e: Exception) { /* تجاهل */ }
        }
        batteryReceiver = null
        onBatteryChanged = null
    }
}
