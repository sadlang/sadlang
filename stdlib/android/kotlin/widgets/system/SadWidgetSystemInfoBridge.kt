package sad.android.widgets.system

import android.app.ActivityManager
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.os.SystemClock
import java.io.BufferedReader
import java.io.FileReader
import java.text.SimpleDateFormat
import java.util.*

/**
 * جسر ويدجت معلومات النظام - معلومات الجهاز والنظام
 * System Info Widget Bridge - Device and system information
 * 
 * @author فريق لغة ص
 */
object SadWidgetSystemInfoBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * معلومات الجهاز / Device info
     */
    data class DeviceInfo(
        val manufacturer: String,
        val model: String,
        val brand: String,
        val device: String,
        val product: String,
        val board: String,
        val hardware: String,
        val displayName: String
    )
    
    /**
     * معلومات النظام / System info
     */
    data class SystemInfo(
        val androidVersion: String,
        val apiLevel: Int,
        val securityPatch: String,
        val buildNumber: String,
        val bootTime: Long,
        val uptimeMs: Long,
        val uptimeFormatted: String
    )
    
    /**
     * معلومات الذاكرة / Memory info
     */
    data class MemoryInfo(
        val totalRam: Long,
        val availableRam: Long,
        val usedRam: Long,
        val usedPercent: Float,
        val totalFormatted: String,
        val availableFormatted: String,
        val usedFormatted: String,
        val isLowMemory: Boolean
    )
    
    /**
     * معلومات المعالج / CPU info
     */
    data class CpuInfo(
        val cores: Int,
        val maxFreqMHz: Int?,
        val minFreqMHz: Int?,
        val architecture: String,
        val usagePercent: Float?
    )
    
    /**
     * معلومات البطارية / Battery info
     */
    data class BatteryInfo(
        val level: Int,
        val isCharging: Boolean,
        val temperature: Float,
        val voltage: Float,
        val health: String,
        val technology: String
    )
    
    /**
     * ملخص النظام / System summary
     */
    data class SystemSummary(
        val device: DeviceInfo,
        val system: SystemInfo,
        val memory: MemoryInfo,
        val cpu: CpuInfo,
        val battery: BatteryInfo
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الجهاز
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات الجهاز / Get device info
     */
    fun getDeviceInfo(): DeviceInfo {
        return DeviceInfo(
            manufacturer = Build.MANUFACTURER,
            model = Build.MODEL,
            brand = Build.BRAND,
            device = Build.DEVICE,
            product = Build.PRODUCT,
            board = Build.BOARD,
            hardware = Build.HARDWARE,
            displayName = "${Build.MANUFACTURER} ${Build.MODEL}"
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات النظام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات النظام / Get system info
     */
    fun getSystemInfo(): SystemInfo {
        val uptimeMs = SystemClock.elapsedRealtime()
        val bootTime = System.currentTimeMillis() - uptimeMs
        
        return SystemInfo(
            androidVersion = Build.VERSION.RELEASE,
            apiLevel = Build.VERSION.SDK_INT,
            securityPatch = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                Build.VERSION.SECURITY_PATCH
            } else "",
            buildNumber = Build.DISPLAY,
            bootTime = bootTime,
            uptimeMs = uptimeMs,
            uptimeFormatted = formatUptime(uptimeMs)
        )
    }
    
    private fun formatUptime(ms: Long): String {
        val seconds = ms / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        val days = hours / 24
        
        return buildString {
            if (days > 0) append("${days} يوم ")
            append("${hours % 24} ساعة ")
            append("${minutes % 60} دقيقة")
        }.trim()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات الذاكرة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات الذاكرة / Get memory info
     */
    fun getMemoryInfo(): MemoryInfo {
        val ctx = context ?: return createEmptyMemoryInfo()
        val activityManager = ctx.getSystemService(Context.ACTIVITY_SERVICE) as? ActivityManager
            ?: return createEmptyMemoryInfo()
        
        val memInfo = ActivityManager.MemoryInfo()
        activityManager.getMemoryInfo(memInfo)
        
        val totalRam = memInfo.totalMem
        val availableRam = memInfo.availMem
        val usedRam = totalRam - availableRam
        val usedPercent = (usedRam.toFloat() / totalRam) * 100
        
        return MemoryInfo(
            totalRam = totalRam,
            availableRam = availableRam,
            usedRam = usedRam,
            usedPercent = usedPercent,
            totalFormatted = formatBytes(totalRam),
            availableFormatted = formatBytes(availableRam),
            usedFormatted = formatBytes(usedRam),
            isLowMemory = memInfo.lowMemory
        )
    }
    
    private fun createEmptyMemoryInfo(): MemoryInfo {
        return MemoryInfo(
            totalRam = 0,
            availableRam = 0,
            usedRam = 0,
            usedPercent = 0f,
            totalFormatted = "غير معروف",
            availableFormatted = "غير معروف",
            usedFormatted = "غير معروف",
            isLowMemory = false
        )
    }
    
    private fun formatBytes(bytes: Long): String {
        val kb = 1024.0
        val mb = kb * 1024
        val gb = mb * 1024
        
        return when {
            bytes >= gb -> String.format("%.1f جيجا", bytes / gb)
            bytes >= mb -> String.format("%.1f ميجا", bytes / mb)
            bytes >= kb -> String.format("%.1f كيلو", bytes / kb)
            else -> "$bytes بايت"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات المعالج
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات المعالج / Get CPU info
     */
    fun getCpuInfo(): CpuInfo {
        val cores = Runtime.getRuntime().availableProcessors()
        val architecture = System.getProperty("os.arch") ?: "غير معروف"
        
        var maxFreq: Int? = null
        var minFreq: Int? = null
        
        try {
            // محاولة قراءة الترددات من نظام الملفات
            val maxFreqFile = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
            val minFreqFile = "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"
            
            BufferedReader(FileReader(maxFreqFile)).use { reader ->
                maxFreq = reader.readLine()?.toIntOrNull()?.div(1000)
            }
            BufferedReader(FileReader(minFreqFile)).use { reader ->
                minFreq = reader.readLine()?.toIntOrNull()?.div(1000)
            }
        } catch (e: Exception) {
            // قد لا يكون مصرحاً بالوصول
        }
        
        return CpuInfo(
            cores = cores,
            maxFreqMHz = maxFreq,
            minFreqMHz = minFreq,
            architecture = architecture,
            usagePercent = null  // يتطلب مراقبة مستمرة
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات البطارية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات البطارية / Get battery info
     */
    fun getBatteryInfo(): BatteryInfo {
        val ctx = context ?: return createEmptyBatteryInfo()
        
        val intent = ctx.registerReceiver(
            null,
            IntentFilter(Intent.ACTION_BATTERY_CHANGED)
        ) ?: return createEmptyBatteryInfo()
        
        val level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1)
        val scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1)
        val status = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1)
        val temperature = intent.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, -1) / 10f
        val voltage = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, -1) / 1000f
        val health = intent.getIntExtra(BatteryManager.EXTRA_HEALTH, -1)
        val technology = intent.getStringExtra(BatteryManager.EXTRA_TECHNOLOGY) ?: "غير معروف"
        
        val batteryPercent = if (level >= 0 && scale > 0) {
            (level * 100) / scale
        } else 0
        
        val isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING ||
                status == BatteryManager.BATTERY_STATUS_FULL
        
        val healthStr = when (health) {
            BatteryManager.BATTERY_HEALTH_GOOD -> "جيدة"
            BatteryManager.BATTERY_HEALTH_OVERHEAT -> "ساخنة جداً"
            BatteryManager.BATTERY_HEALTH_DEAD -> "تالفة"
            BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE -> "جهد زائد"
            BatteryManager.BATTERY_HEALTH_COLD -> "باردة جداً"
            else -> "غير معروف"
        }
        
        return BatteryInfo(
            level = batteryPercent,
            isCharging = isCharging,
            temperature = temperature,
            voltage = voltage,
            health = healthStr,
            technology = technology
        )
    }
    
    private fun createEmptyBatteryInfo(): BatteryInfo {
        return BatteryInfo(
            level = 0,
            isCharging = false,
            temperature = 0f,
            voltage = 0f,
            health = "غير معروف",
            technology = "غير معروف"
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         ملخص النظام
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على ملخص النظام / Get system summary
     */
    fun getSystemSummary(): SystemSummary {
        return SystemSummary(
            device = getDeviceInfo(),
            system = getSystemInfo(),
            memory = getMemoryInfo(),
            cpu = getCpuInfo(),
            battery = getBatteryInfo()
        )
    }
    
    /**
     * الحصول على وقت التشغيل الحالي بالثواني / Get current uptime in seconds
     */
    fun getUptimeSeconds(): Long {
        return SystemClock.elapsedRealtime() / 1000
    }
    
    /**
     * الحصول على تاريخ آخر إقلاع / Get last boot date
     */
    fun getLastBootDate(): String {
        val bootTime = System.currentTimeMillis() - SystemClock.elapsedRealtime()
        val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm", Locale("ar"))
        return dateFormat.format(Date(bootTime))
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        context = null
    }
}
