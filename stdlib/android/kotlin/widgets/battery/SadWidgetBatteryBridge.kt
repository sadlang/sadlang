package sad.android.widgets.battery

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import android.os.Build
import android.os.Handler
import android.os.Looper

/**
 * جسر ويدجت البطارية - عرض حالة البطارية
 * Battery Widget Bridge - Battery status display
 * 
 * @author فريق لغة ص
 */
object SadWidgetBatteryBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var batteryReceiver: BroadcastReceiver? = null
    private val batteryListeners = mutableListOf<BatteryStatusListener>()
    
    /**
     * حالة البطارية / Battery status
     */
    data class BatteryStatus(
        val level: Int,                      // النسبة المئوية
        val isCharging: Boolean,             // هل تشحن؟
        val chargingSource: ChargingSource,  // مصدر الشحن
        val health: BatteryHealth,           // صحة البطارية
        val temperature: Float,              // الحرارة بالسيلسيوس
        val voltage: Float,                  // الفولتية
        val technology: String,              // نوع البطارية
        val capacity: Int,                   // السعة بـ mAh
        val currentNow: Int,                 // التيار الحالي
        val chargeTimeRemaining: Long,       // الوقت المتبقي للشحن (API 28+)
        val isLow: Boolean                   // هل منخفضة؟
    )
    
    /**
     * مصدر الشحن / Charging source
     */
    enum class ChargingSource {
        NONE,
        AC,
        USB,
        WIRELESS,
        UNKNOWN
    }
    
    /**
     * صحة البطارية / Battery health
     */
    enum class BatteryHealth {
        GOOD,
        OVERHEAT,
        DEAD,
        OVER_VOLTAGE,
        COLD,
        UNKNOWN
    }
    
    /**
     * مستمع حالة البطارية / Battery status listener
     */
    interface BatteryStatusListener {
        fun onBatteryStatusChanged(status: BatteryStatus)
        fun onChargingStateChanged(isCharging: Boolean)
        fun onLowBattery(level: Int)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addBatteryListener(listener: BatteryStatusListener) {
        if (!batteryListeners.contains(listener)) {
            batteryListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeBatteryListener(listener: BatteryStatusListener) {
        batteryListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مراقبة البطارية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء المراقبة / Start monitoring
     */
    fun startMonitoring(): Boolean {
        val ctx = context ?: return false
        
        if (batteryReceiver != null) {
            return true // بالفعل تراقب
        }
        
        batteryReceiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context, intent: Intent) {
                val status = getBatteryStatusFromIntent(intent)
                
                mainHandler.post {
                    batteryListeners.forEach { it.onBatteryStatusChanged(status) }
                    
                    if (status.isLow) {
                        batteryListeners.forEach { it.onLowBattery(status.level) }
                    }
                }
            }
        }
        
        val filter = IntentFilter().apply {
            addAction(Intent.ACTION_BATTERY_CHANGED)
            addAction(Intent.ACTION_BATTERY_LOW)
            addAction(Intent.ACTION_BATTERY_OKAY)
            addAction(Intent.ACTION_POWER_CONNECTED)
            addAction(Intent.ACTION_POWER_DISCONNECTED)
        }
        
        ctx.registerReceiver(batteryReceiver, filter)
        return true
    }
    
    /**
     * إيقاف المراقبة / Stop monitoring
     */
    fun stopMonitoring(): Boolean {
        val ctx = context ?: return false
        
        batteryReceiver?.let {
            ctx.unregisterReceiver(it)
            batteryReceiver = null
            return true
        }
        return false
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الحصول على المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على حالة البطارية الحالية / Get current battery status
     */
    fun getCurrentStatus(): BatteryStatus? {
        val ctx = context ?: return null
        
        val batteryIntent = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        return batteryIntent?.let { getBatteryStatusFromIntent(it) }
    }
    
    /**
     * الحصول على مستوى البطارية / Get battery level
     */
    fun getBatteryLevel(): Int {
        return getCurrentStatus()?.level ?: -1
    }
    
    /**
     * هل البطارية تشحن؟ / Is battery charging?
     */
    fun isCharging(): Boolean {
        return getCurrentStatus()?.isCharging ?: false
    }
    
    /**
     * هل البطارية منخفضة؟ / Is battery low?
     */
    fun isLowBattery(): Boolean {
        return getCurrentStatus()?.isLow ?: false
    }
    
    private fun getBatteryStatusFromIntent(intent: Intent): BatteryStatus {
        val level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1)
        val scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1)
        val batteryPct = if (level >= 0 && scale > 0) (level * 100 / scale) else 0
        
        val status = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1)
        val isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING ||
                status == BatteryManager.BATTERY_STATUS_FULL
        
        val plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1)
        val chargingSource = when (plugged) {
            BatteryManager.BATTERY_PLUGGED_AC -> ChargingSource.AC
            BatteryManager.BATTERY_PLUGGED_USB -> ChargingSource.USB
            BatteryManager.BATTERY_PLUGGED_WIRELESS -> ChargingSource.WIRELESS
            else -> if (isCharging) ChargingSource.UNKNOWN else ChargingSource.NONE
        }
        
        val healthInt = intent.getIntExtra(BatteryManager.EXTRA_HEALTH, -1)
        val health = when (healthInt) {
            BatteryManager.BATTERY_HEALTH_GOOD -> BatteryHealth.GOOD
            BatteryManager.BATTERY_HEALTH_OVERHEAT -> BatteryHealth.OVERHEAT
            BatteryManager.BATTERY_HEALTH_DEAD -> BatteryHealth.DEAD
            BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE -> BatteryHealth.OVER_VOLTAGE
            BatteryManager.BATTERY_HEALTH_COLD -> BatteryHealth.COLD
            else -> BatteryHealth.UNKNOWN
        }
        
        val temperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, 0) / 10f
        val voltage = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, 0) / 1000f
        val technology = intent.getStringExtra(BatteryManager.EXTRA_TECHNOLOGY) ?: ""
        
        // معلومات إضافية (API 21+)
        val ctx = context
        var capacity = 0
        var currentNow = 0
        var chargeTimeRemaining = 0L
        
        if (ctx != null) {
            val batteryManager = ctx.getSystemService(Context.BATTERY_SERVICE) as? BatteryManager
            batteryManager?.let { bm ->
                currentNow = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CURRENT_NOW)
                capacity = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CHARGE_COUNTER) / 1000
                
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                    chargeTimeRemaining = bm.computeChargeTimeRemaining()
                }
            }
        }
        
        return BatteryStatus(
            level = batteryPct,
            isCharging = isCharging,
            chargingSource = chargingSource,
            health = health,
            temperature = temperature,
            voltage = voltage,
            technology = technology,
            capacity = capacity,
            currentNow = currentNow,
            chargeTimeRemaining = chargeTimeRemaining,
            isLow = batteryPct <= 15
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق النسبة المئوية / Format percentage
     */
    fun formatLevel(level: Int): String {
        return "$level%"
    }
    
    /**
     * الحصول على نص حالة الشحن / Get charging status text
     */
    fun getChargingStatusText(status: BatteryStatus): String {
        return when {
            status.isCharging -> when (status.chargingSource) {
                ChargingSource.AC -> "يشحن (محول)"
                ChargingSource.USB -> "يشحن (USB)"
                ChargingSource.WIRELESS -> "يشحن (لاسلكي)"
                else -> "يشحن"
            }
            status.level == 100 -> "مكتملة"
            status.isLow -> "منخفضة"
            else -> "تفريغ"
        }
    }
    
    /**
     * الحصول على نص صحة البطارية / Get battery health text
     */
    fun getHealthText(health: BatteryHealth): String {
        return when (health) {
            BatteryHealth.GOOD -> "جيدة"
            BatteryHealth.OVERHEAT -> "ساخنة جداً"
            BatteryHealth.DEAD -> "تالفة"
            BatteryHealth.OVER_VOLTAGE -> "جهد عالي"
            BatteryHealth.COLD -> "باردة جداً"
            BatteryHealth.UNKNOWN -> "غير معروفة"
        }
    }
    
    /**
     * الحصول على لون البطارية / Get battery color
     */
    fun getBatteryColor(level: Int, isCharging: Boolean): Int {
        return when {
            isCharging -> 0xFF4CAF50.toInt()  // أخضر
            level <= 15 -> 0xFFF44336.toInt() // أحمر
            level <= 30 -> 0xFFFF9800.toInt() // برتقالي
            else -> 0xFF4CAF50.toInt()        // أخضر
        }
    }
    
    /**
     * تنسيق وقت الشحن المتبقي / Format remaining charge time
     */
    fun formatChargeTimeRemaining(milliseconds: Long): String {
        if (milliseconds <= 0) return ""
        
        val minutes = (milliseconds / 1000 / 60).toInt()
        val hours = minutes / 60
        val mins = minutes % 60
        
        return when {
            hours > 0 -> "$hours ساعة و $mins دقيقة"
            else -> "$mins دقيقة"
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopMonitoring()
        batteryListeners.clear()
        context = null
    }
}
