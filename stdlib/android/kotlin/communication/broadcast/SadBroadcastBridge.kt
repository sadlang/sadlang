package sad.android.communication.broadcast

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import android.os.Build
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر البث - إدارة مستقبلات البث
 * Broadcast Bridge - Broadcast receiver management
 * 
 * @author فريق لغة ص
 */
object SadBroadcastBridge {
    
    private val receivers = ConcurrentHashMap<String, BroadcastReceiver>()
    
    /**
     * أنواع البث المعروفة / Known broadcast types
     */
    object BroadcastTypes {
        const val BOOT_COMPLETED = Intent.ACTION_BOOT_COMPLETED
        const val BATTERY_LOW = Intent.ACTION_BATTERY_LOW
        const val BATTERY_OKAY = Intent.ACTION_BATTERY_OKAY
        const val BATTERY_CHANGED = Intent.ACTION_BATTERY_CHANGED
        const val POWER_CONNECTED = Intent.ACTION_POWER_CONNECTED
        const val POWER_DISCONNECTED = Intent.ACTION_POWER_DISCONNECTED
        const val SCREEN_ON = Intent.ACTION_SCREEN_ON
        const val SCREEN_OFF = Intent.ACTION_SCREEN_OFF
        const val USER_PRESENT = Intent.ACTION_USER_PRESENT
        const val AIRPLANE_MODE = Intent.ACTION_AIRPLANE_MODE_CHANGED
        const val CONNECTIVITY_CHANGE = "android.net.conn.CONNECTIVITY_CHANGE"
        const val WIFI_STATE = "android.net.wifi.WIFI_STATE_CHANGED"
        const val HEADSET_PLUG = Intent.ACTION_HEADSET_PLUG
        const val LOCALE_CHANGED = Intent.ACTION_LOCALE_CHANGED
        const val TIMEZONE_CHANGED = Intent.ACTION_TIMEZONE_CHANGED
        const val DATE_CHANGED = Intent.ACTION_DATE_CHANGED
        const val TIME_TICK = Intent.ACTION_TIME_TICK
        const val PACKAGE_ADDED = Intent.ACTION_PACKAGE_ADDED
        const val PACKAGE_REMOVED = Intent.ACTION_PACKAGE_REMOVED
    }
    
    /**
     * مستمع البث / Broadcast listener
     */
    interface BroadcastListener {
        fun onReceive(action: String, extras: Map<String, Any?>)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التسجيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل مستقبل / Register receiver
     */
    fun register(
        context: Context,
        id: String,
        actions: List<String>,
        listener: BroadcastListener
    ): Boolean {
        // إلغاء تسجيل سابق إن وجد
        unregister(context, id)
        
        val receiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                intent?.let {
                    val extras = mutableMapOf<String, Any?>()
                    it.extras?.let { bundle ->
                        for (key in bundle.keySet()) {
                            extras[key] = bundle.get(key)
                        }
                    }
                    listener.onReceive(it.action ?: "", extras)
                }
            }
        }
        
        val filter = IntentFilter().apply {
            actions.forEach { addAction(it) }
        }
        
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                context.registerReceiver(receiver, filter, Context.RECEIVER_NOT_EXPORTED)
            } else {
                context.registerReceiver(receiver, filter)
            }
            receivers[id] = receiver
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إلغاء التسجيل / Unregister
     */
    fun unregister(context: Context, id: String): Boolean {
        return receivers.remove(id)?.let { receiver ->
            try {
                context.unregisterReceiver(receiver)
                true
            } catch (e: Exception) {
                false
            }
        } ?: false
    }
    
    /**
     * إلغاء تسجيل الكل / Unregister all
     */
    fun unregisterAll(context: Context) {
        receivers.keys.forEach { id ->
            unregister(context, id)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مستقبلات محددة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مراقبة البطارية / Monitor battery
     */
    fun monitorBattery(context: Context, listener: BatteryListener): String {
        val id = "battery_monitor_${System.currentTimeMillis()}"
        
        register(context, id, listOf(
            Intent.ACTION_BATTERY_CHANGED,
            Intent.ACTION_BATTERY_LOW,
            Intent.ACTION_BATTERY_OKAY,
            Intent.ACTION_POWER_CONNECTED,
            Intent.ACTION_POWER_DISCONNECTED
        ), object : BroadcastListener {
            override fun onReceive(action: String, extras: Map<String, Any?>) {
                when (action) {
                    Intent.ACTION_BATTERY_CHANGED -> {
                        val level = (extras["level"] as? Int) ?: 0
                        val scale = (extras["scale"] as? Int) ?: 100
                        val percentage = (level * 100) / scale
                        val status = extras["status"] as? Int ?: BatteryManager.BATTERY_STATUS_UNKNOWN
                        
                        listener.onBatteryChanged(BatteryInfo(
                            percentage = percentage,
                            isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING ||
                                        status == BatteryManager.BATTERY_STATUS_FULL,
                            status = getBatteryStatus(status),
                            health = getBatteryHealth(extras["health"] as? Int),
                            temperature = ((extras["temperature"] as? Int) ?: 0) / 10f,
                            voltage = ((extras["voltage"] as? Int) ?: 0) / 1000f
                        ))
                    }
                    Intent.ACTION_BATTERY_LOW -> listener.onBatteryLow()
                    Intent.ACTION_BATTERY_OKAY -> listener.onBatteryOkay()
                    Intent.ACTION_POWER_CONNECTED -> listener.onPowerConnected()
                    Intent.ACTION_POWER_DISCONNECTED -> listener.onPowerDisconnected()
                }
            }
        })
        
        return id
    }
    
    /**
     * مراقبة الشاشة / Monitor screen
     */
    fun monitorScreen(context: Context, listener: ScreenListener): String {
        val id = "screen_monitor_${System.currentTimeMillis()}"
        
        register(context, id, listOf(
            Intent.ACTION_SCREEN_ON,
            Intent.ACTION_SCREEN_OFF,
            Intent.ACTION_USER_PRESENT
        ), object : BroadcastListener {
            override fun onReceive(action: String, extras: Map<String, Any?>) {
                when (action) {
                    Intent.ACTION_SCREEN_ON -> listener.onScreenOn()
                    Intent.ACTION_SCREEN_OFF -> listener.onScreenOff()
                    Intent.ACTION_USER_PRESENT -> listener.onUserPresent()
                }
            }
        })
        
        return id
    }
    
    /**
     * مراقبة التطبيقات / Monitor packages
     */
    fun monitorPackages(context: Context, listener: PackageListener): String {
        val id = "package_monitor_${System.currentTimeMillis()}"
        
        val receiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                intent?.data?.schemeSpecificPart?.let { packageName ->
                    when (intent.action) {
                        Intent.ACTION_PACKAGE_ADDED -> {
                            val isUpdate = intent.getBooleanExtra(Intent.EXTRA_REPLACING, false)
                            if (isUpdate) {
                                listener.onPackageUpdated(packageName)
                            } else {
                                listener.onPackageAdded(packageName)
                            }
                        }
                        Intent.ACTION_PACKAGE_REMOVED -> {
                            val isReplacing = intent.getBooleanExtra(Intent.EXTRA_REPLACING, false)
                            if (!isReplacing) {
                                listener.onPackageRemoved(packageName)
                            }
                        }
                    }
                }
            }
        }
        
        val filter = IntentFilter().apply {
            addAction(Intent.ACTION_PACKAGE_ADDED)
            addAction(Intent.ACTION_PACKAGE_REMOVED)
            addDataScheme("package")
        }
        
        context.registerReceiver(receiver, filter)
        receivers[id] = receiver
        
        return id
    }
    
    /**
     * مراقبة الاتصال / Monitor connectivity
     */
    fun monitorConnectivity(context: Context, listener: ConnectivityListener): String {
        val id = "connectivity_monitor_${System.currentTimeMillis()}"
        
        register(context, id, listOf(
            "android.net.conn.CONNECTIVITY_CHANGE",
            Intent.ACTION_AIRPLANE_MODE_CHANGED
        ), object : BroadcastListener {
            override fun onReceive(action: String, extras: Map<String, Any?>) {
                when (action) {
                    "android.net.conn.CONNECTIVITY_CHANGE" -> {
                        listener.onConnectivityChanged()
                    }
                    Intent.ACTION_AIRPLANE_MODE_CHANGED -> {
                        val isOn = extras["state"] as? Boolean ?: false
                        listener.onAirplaneModeChanged(isOn)
                    }
                }
            }
        })
        
        return id
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال البث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال بث محلي / Send local broadcast
     */
    fun sendLocalBroadcast(context: Context, action: String, extras: Map<String, Any?> = emptyMap()) {
        val intent = Intent(action)
        extras.forEach { (key, value) ->
            when (value) {
                is String -> intent.putExtra(key, value)
                is Int -> intent.putExtra(key, value)
                is Long -> intent.putExtra(key, value)
                is Boolean -> intent.putExtra(key, value)
                is Float -> intent.putExtra(key, value)
                is Double -> intent.putExtra(key, value)
            }
        }
        
        androidx.localbroadcastmanager.content.LocalBroadcastManager
            .getInstance(context)
            .sendBroadcast(intent)
    }
    
    /**
     * تسجيل مستقبل محلي / Register local receiver
     */
    fun registerLocalReceiver(
        context: Context,
        id: String,
        actions: List<String>,
        listener: BroadcastListener
    ): Boolean {
        val receiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                intent?.let {
                    val extras = mutableMapOf<String, Any?>()
                    it.extras?.let { bundle ->
                        for (key in bundle.keySet()) {
                            extras[key] = bundle.get(key)
                        }
                    }
                    listener.onReceive(it.action ?: "", extras)
                }
            }
        }
        
        val filter = IntentFilter().apply {
            actions.forEach { addAction(it) }
        }
        
        androidx.localbroadcastmanager.content.LocalBroadcastManager
            .getInstance(context)
            .registerReceiver(receiver, filter)
        
        receivers[id] = receiver
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأنواع
    // ═══════════════════════════════════════════════════════════════
    
    data class BatteryInfo(
        val percentage: Int,
        val isCharging: Boolean,
        val status: String,
        val health: String,
        val temperature: Float,
        val voltage: Float
    )
    
    interface BatteryListener {
        fun onBatteryChanged(info: BatteryInfo)
        fun onBatteryLow()
        fun onBatteryOkay()
        fun onPowerConnected()
        fun onPowerDisconnected()
    }
    
    interface ScreenListener {
        fun onScreenOn()
        fun onScreenOff()
        fun onUserPresent()
    }
    
    interface PackageListener {
        fun onPackageAdded(packageName: String)
        fun onPackageRemoved(packageName: String)
        fun onPackageUpdated(packageName: String)
    }
    
    interface ConnectivityListener {
        fun onConnectivityChanged()
        fun onAirplaneModeChanged(isOn: Boolean)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getBatteryStatus(status: Int): String {
        return when (status) {
            BatteryManager.BATTERY_STATUS_CHARGING -> "شحن"
            BatteryManager.BATTERY_STATUS_DISCHARGING -> "تفريغ"
            BatteryManager.BATTERY_STATUS_FULL -> "ممتلئ"
            BatteryManager.BATTERY_STATUS_NOT_CHARGING -> "غير يشحن"
            else -> "غير معروف"
        }
    }
    
    private fun getBatteryHealth(health: Int?): String {
        return when (health) {
            BatteryManager.BATTERY_HEALTH_GOOD -> "جيد"
            BatteryManager.BATTERY_HEALTH_OVERHEAT -> "ساخن"
            BatteryManager.BATTERY_HEALTH_DEAD -> "ميت"
            BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE -> "جهد عالي"
            BatteryManager.BATTERY_HEALTH_COLD -> "بارد"
            else -> "غير معروف"
        }
    }
}
