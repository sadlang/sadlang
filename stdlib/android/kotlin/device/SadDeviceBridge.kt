// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDeviceBridge.kt
// الوصف: جسر Kotlin لمعلومات الجهاز
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.ActivityManager
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.res.Configuration
import android.os.BatteryManager
import android.os.Build
import android.os.Environment
import android.os.StatFs
import android.provider.Settings
import android.util.DisplayMetrics
import android.view.WindowManager
import java.io.File
import java.io.RandomAccessFile
import java.util.*

/**
 * جسر معلومات الجهاز — SadDeviceBridge
 */
object SadDeviceBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Device Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDeviceInfo(): Map<String, Any> {
        return mapOf(
            "manufacturer" to Build.MANUFACTURER,
            "brand" to Build.BRAND,
            "model" to Build.MODEL,
            "device" to Build.DEVICE,
            "product" to Build.PRODUCT,
            "board" to Build.BOARD,
            "hardware" to Build.HARDWARE,
            "id" to Build.ID,
            "display" to Build.DISPLAY,
            "type" to Build.TYPE,
            "tags" to Build.TAGS,
            "fingerprint" to Build.FINGERPRINT,
            "bootloader" to Build.BOOTLOADER,
            "host" to Build.HOST,
            "time" to Build.TIME,
            "user" to Build.USER
        )
    }
    
    @JvmStatic
    fun getManufacturer(): String = Build.MANUFACTURER
    
    @JvmStatic
    fun getBrand(): String = Build.BRAND
    
    @JvmStatic
    fun getModel(): String = Build.MODEL
    
    @JvmStatic
    fun getDeviceName(): String = "${Build.MANUFACTURER} ${Build.MODEL}"
    
    @JvmStatic
    fun getHardware(): String = Build.HARDWARE
    
    @JvmStatic
    fun getSerial(): String {
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                Build.getSerial()
            } else {
                @Suppress("DEPRECATION")
                Build.SERIAL
            }
        } catch (e: SecurityException) {
            "غير متاح"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Android Version
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAndroidVersion(): Map<String, Any> {
        return mapOf(
            "sdkInt" to Build.VERSION.SDK_INT,
            "release" to Build.VERSION.RELEASE,
            "codename" to Build.VERSION.CODENAME,
            "incremental" to Build.VERSION.INCREMENTAL,
            "baseOs" to (if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) Build.VERSION.BASE_OS else ""),
            "securityPatch" to (if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) Build.VERSION.SECURITY_PATCH else ""),
            "previewSdkInt" to (if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) Build.VERSION.PREVIEW_SDK_INT else 0)
        )
    }
    
    @JvmStatic
    fun getSdkVersion(): Int = Build.VERSION.SDK_INT
    
    @JvmStatic
    fun getAndroidRelease(): String = Build.VERSION.RELEASE
    
    @JvmStatic
    fun getAndroidCodename(): String {
        return when (Build.VERSION.SDK_INT) {
            21, 22 -> "Lollipop"
            23 -> "Marshmallow"
            24, 25 -> "Nougat"
            26, 27 -> "Oreo"
            28 -> "Pie"
            29 -> "Q"
            30 -> "R"
            31, 32 -> "S"
            33 -> "Tiramisu"
            34 -> "Upside Down Cake"
            35 -> "Vanilla Ice Cream"
            else -> Build.VERSION.CODENAME
        }
    }
    
    @JvmStatic
    fun isAtLeast(sdkInt: Int): Boolean = Build.VERSION.SDK_INT >= sdkInt
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Display
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDisplayInfo(): Map<String, Any> {
        val ctx = context ?: return emptyMap()
        
        val windowManager = ctx.getSystemService(Context.WINDOW_SERVICE) as WindowManager
        val displayMetrics = DisplayMetrics()
        
        @Suppress("DEPRECATION")
        windowManager.defaultDisplay.getRealMetrics(displayMetrics)
        
        return mapOf(
            "widthPixels" to displayMetrics.widthPixels,
            "heightPixels" to displayMetrics.heightPixels,
            "density" to displayMetrics.density,
            "densityDpi" to displayMetrics.densityDpi,
            "scaledDensity" to displayMetrics.scaledDensity,
            "xdpi" to displayMetrics.xdpi,
            "ydpi" to displayMetrics.ydpi,
            "densityClass" to getDensityClass(displayMetrics.densityDpi),
            "widthDp" to (displayMetrics.widthPixels / displayMetrics.density).toInt(),
            "heightDp" to (displayMetrics.heightPixels / displayMetrics.density).toInt(),
            "diagonalInches" to calculateDiagonalInches(displayMetrics)
        )
    }
    
    @JvmStatic
    fun getScreenWidthPixels(): Int {
        val ctx = context ?: return 0
        val displayMetrics = ctx.resources.displayMetrics
        return displayMetrics.widthPixels
    }
    
    @JvmStatic
    fun getScreenHeightPixels(): Int {
        val ctx = context ?: return 0
        val displayMetrics = ctx.resources.displayMetrics
        return displayMetrics.heightPixels
    }
    
    @JvmStatic
    fun getScreenDensity(): Float {
        val ctx = context ?: return 1f
        return ctx.resources.displayMetrics.density
    }
    
    @JvmStatic
    fun isTablet(): Boolean {
        val ctx = context ?: return false
        return ctx.resources.configuration.screenLayout and
               Configuration.SCREENLAYOUT_SIZE_MASK >= Configuration.SCREENLAYOUT_SIZE_LARGE
    }
    
    @JvmStatic
    fun isPortrait(): Boolean {
        val ctx = context ?: return true
        return ctx.resources.configuration.orientation == Configuration.ORIENTATION_PORTRAIT
    }
    
    @JvmStatic
    fun isLandscape(): Boolean {
        val ctx = context ?: return false
        return ctx.resources.configuration.orientation == Configuration.ORIENTATION_LANDSCAPE
    }
    
    private fun getDensityClass(dpi: Int): String {
        return when {
            dpi <= DisplayMetrics.DENSITY_LOW -> "ldpi"
            dpi <= DisplayMetrics.DENSITY_MEDIUM -> "mdpi"
            dpi <= DisplayMetrics.DENSITY_HIGH -> "hdpi"
            dpi <= DisplayMetrics.DENSITY_XHIGH -> "xhdpi"
            dpi <= DisplayMetrics.DENSITY_XXHIGH -> "xxhdpi"
            else -> "xxxhdpi"
        }
    }
    
    private fun calculateDiagonalInches(dm: DisplayMetrics): Double {
        val widthInches = dm.widthPixels / dm.xdpi.toDouble()
        val heightInches = dm.heightPixels / dm.ydpi.toDouble()
        return kotlin.math.sqrt(widthInches * widthInches + heightInches * heightInches)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Memory
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getMemoryInfo(): Map<String, Any> {
        val ctx = context ?: return emptyMap()
        
        val activityManager = ctx.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val memInfo = ActivityManager.MemoryInfo()
        activityManager.getMemoryInfo(memInfo)
        
        val runtime = Runtime.getRuntime()
        
        return mapOf(
            "totalMemoryBytes" to memInfo.totalMem,
            "availableMemoryBytes" to memInfo.availMem,
            "usedMemoryBytes" to (memInfo.totalMem - memInfo.availMem),
            "lowMemory" to memInfo.lowMemory,
            "threshold" to memInfo.threshold,
            "totalMemoryMB" to (memInfo.totalMem / (1024 * 1024)),
            "availableMemoryMB" to (memInfo.availMem / (1024 * 1024)),
            "jvmMaxMemory" to runtime.maxMemory(),
            "jvmTotalMemory" to runtime.totalMemory(),
            "jvmFreeMemory" to runtime.freeMemory()
        )
    }
    
    @JvmStatic
    fun getAvailableMemoryMB(): Long {
        val ctx = context ?: return 0
        val activityManager = ctx.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val memInfo = ActivityManager.MemoryInfo()
        activityManager.getMemoryInfo(memInfo)
        return memInfo.availMem / (1024 * 1024)
    }
    
    @JvmStatic
    fun isLowMemory(): Boolean {
        val ctx = context ?: return false
        val activityManager = ctx.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val memInfo = ActivityManager.MemoryInfo()
        activityManager.getMemoryInfo(memInfo)
        return memInfo.lowMemory
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Storage
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getStorageInfo(): Map<String, Any> {
        val internal = getInternalStorageInfo()
        val external = getExternalStorageInfo()
        
        return mapOf(
            "internal" to internal,
            "external" to external
        )
    }
    
    @JvmStatic
    fun getInternalStorageInfo(): Map<String, Any> {
        val path = Environment.getDataDirectory()
        val stat = StatFs(path.path)
        
        val blockSize = stat.blockSizeLong
        val totalBlocks = stat.blockCountLong
        val availableBlocks = stat.availableBlocksLong
        
        val totalBytes = totalBlocks * blockSize
        val availableBytes = availableBlocks * blockSize
        val usedBytes = totalBytes - availableBytes
        
        return mapOf(
            "totalBytes" to totalBytes,
            "availableBytes" to availableBytes,
            "usedBytes" to usedBytes,
            "totalGB" to String.format("%.2f", totalBytes / (1024.0 * 1024.0 * 1024.0)),
            "availableGB" to String.format("%.2f", availableBytes / (1024.0 * 1024.0 * 1024.0)),
            "usagePercent" to ((usedBytes.toDouble() / totalBytes.toDouble()) * 100).toInt()
        )
    }
    
    @JvmStatic
    fun getExternalStorageInfo(): Map<String, Any> {
        val path = Environment.getExternalStorageDirectory()
        
        return try {
            val stat = StatFs(path.path)
            
            val blockSize = stat.blockSizeLong
            val totalBlocks = stat.blockCountLong
            val availableBlocks = stat.availableBlocksLong
            
            val totalBytes = totalBlocks * blockSize
            val availableBytes = availableBlocks * blockSize
            
            mapOf(
                "available" to true,
                "totalBytes" to totalBytes,
                "availableBytes" to availableBytes,
                "totalGB" to String.format("%.2f", totalBytes / (1024.0 * 1024.0 * 1024.0)),
                "availableGB" to String.format("%.2f", availableBytes / (1024.0 * 1024.0 * 1024.0)),
                "state" to Environment.getExternalStorageState()
            )
        } catch (e: Exception) {
            mapOf("available" to false)
        }
    }
    
    @JvmStatic
    fun isExternalStorageWritable(): Boolean {
        return Environment.getExternalStorageState() == Environment.MEDIA_MOUNTED
    }
    
    @JvmStatic
    fun isExternalStorageReadable(): Boolean {
        val state = Environment.getExternalStorageState()
        return state == Environment.MEDIA_MOUNTED || state == Environment.MEDIA_MOUNTED_READ_ONLY
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Battery
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getBatteryInfo(): Map<String, Any> {
        val ctx = context ?: return emptyMap()
        
        val batteryStatus = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        
        val level = batteryStatus?.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) ?: -1
        val scale = batteryStatus?.getIntExtra(BatteryManager.EXTRA_SCALE, -1) ?: -1
        val batteryPct = if (level >= 0 && scale > 0) (level * 100 / scale) else -1
        
        val status = batteryStatus?.getIntExtra(BatteryManager.EXTRA_STATUS, -1) ?: -1
        val isCharging = status == BatteryManager.BATTERY_STATUS_CHARGING ||
                        status == BatteryManager.BATTERY_STATUS_FULL
        
        val chargePlug = batteryStatus?.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1) ?: -1
        val usbCharge = chargePlug == BatteryManager.BATTERY_PLUGGED_USB
        val acCharge = chargePlug == BatteryManager.BATTERY_PLUGGED_AC
        val wirelessCharge = chargePlug == BatteryManager.BATTERY_PLUGGED_WIRELESS
        
        val health = batteryStatus?.getIntExtra(BatteryManager.EXTRA_HEALTH, -1) ?: -1
        val temperature = batteryStatus?.getIntExtra(BatteryManager.EXTRA_TEMPERATURE, -1) ?: -1
        val voltage = batteryStatus?.getIntExtra(BatteryManager.EXTRA_VOLTAGE, -1) ?: -1
        val technology = batteryStatus?.getStringExtra(BatteryManager.EXTRA_TECHNOLOGY) ?: "غير معروف"
        
        return mapOf(
            "levelPercent" to batteryPct,
            "isCharging" to isCharging,
            "usbCharge" to usbCharge,
            "acCharge" to acCharge,
            "wirelessCharge" to wirelessCharge,
            "health" to getBatteryHealthString(health),
            "temperature" to (temperature / 10.0), // Convert to Celsius
            "voltage" to voltage,
            "technology" to technology,
            "status" to getBatteryStatusString(status)
        )
    }
    
    @JvmStatic
    fun getBatteryLevel(): Int {
        val ctx = context ?: return -1
        val batteryStatus = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        
        val level = batteryStatus?.getIntExtra(BatteryManager.EXTRA_LEVEL, -1) ?: -1
        val scale = batteryStatus?.getIntExtra(BatteryManager.EXTRA_SCALE, -1) ?: -1
        
        return if (level >= 0 && scale > 0) (level * 100 / scale) else -1
    }
    
    @JvmStatic
    fun isCharging(): Boolean {
        val ctx = context ?: return false
        val batteryStatus = ctx.registerReceiver(null, IntentFilter(Intent.ACTION_BATTERY_CHANGED))
        
        val status = batteryStatus?.getIntExtra(BatteryManager.EXTRA_STATUS, -1) ?: -1
        return status == BatteryManager.BATTERY_STATUS_CHARGING ||
               status == BatteryManager.BATTERY_STATUS_FULL
    }
    
    private fun getBatteryHealthString(health: Int): String {
        return when (health) {
            BatteryManager.BATTERY_HEALTH_GOOD -> "جيدة"
            BatteryManager.BATTERY_HEALTH_OVERHEAT -> "حرارة زائدة"
            BatteryManager.BATTERY_HEALTH_DEAD -> "ميتة"
            BatteryManager.BATTERY_HEALTH_OVER_VOLTAGE -> "جهد زائد"
            BatteryManager.BATTERY_HEALTH_COLD -> "باردة"
            BatteryManager.BATTERY_HEALTH_UNSPECIFIED_FAILURE -> "فشل غير محدد"
            else -> "غير معروف"
        }
    }
    
    private fun getBatteryStatusString(status: Int): String {
        return when (status) {
            BatteryManager.BATTERY_STATUS_CHARGING -> "يشحن"
            BatteryManager.BATTERY_STATUS_DISCHARGING -> "يفرغ"
            BatteryManager.BATTERY_STATUS_FULL -> "ممتلئ"
            BatteryManager.BATTERY_STATUS_NOT_CHARGING -> "لا يشحن"
            else -> "غير معروف"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // CPU
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCpuInfo(): Map<String, Any> {
        val processors = Runtime.getRuntime().availableProcessors()
        
        val cpuAbi = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            Build.SUPPORTED_ABIS.toList()
        } else {
            @Suppress("DEPRECATION")
            listOf(Build.CPU_ABI, Build.CPU_ABI2)
        }
        
        return mapOf(
            "processors" to processors,
            "supportedAbis" to cpuAbi,
            "is64Bit" to (cpuAbi.any { it.contains("64") }),
            "isArm" to (cpuAbi.any { it.contains("arm", ignoreCase = true) }),
            "isX86" to (cpuAbi.any { it.contains("x86", ignoreCase = true) })
        )
    }
    
    @JvmStatic
    fun getProcessorCount(): Int = Runtime.getRuntime().availableProcessors()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Device ID
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAndroidId(): String {
        val ctx = context ?: return ""
        return Settings.Secure.getString(ctx.contentResolver, Settings.Secure.ANDROID_ID)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // System Features
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun hasSystemFeature(feature: String): Boolean {
        val ctx = context ?: return false
        return ctx.packageManager.hasSystemFeature(feature)
    }
    
    @JvmStatic
    fun hasCamera(): Boolean = hasSystemFeature("android.hardware.camera.any")
    
    @JvmStatic
    fun hasBluetooth(): Boolean = hasSystemFeature("android.hardware.bluetooth")
    
    @JvmStatic
    fun hasNfc(): Boolean = hasSystemFeature("android.hardware.nfc")
    
    @JvmStatic
    fun hasTelephony(): Boolean = hasSystemFeature("android.hardware.telephony")
    
    @JvmStatic
    fun hasGps(): Boolean = hasSystemFeature("android.hardware.location.gps")
    
    @JvmStatic
    fun hasFingerprint(): Boolean = hasSystemFeature("android.hardware.fingerprint")
    
    @JvmStatic
    fun hasSensor(sensorType: String): Boolean {
        return hasSystemFeature("android.hardware.sensor.$sensorType")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Locale
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getLocaleInfo(): Map<String, Any> {
        val locale = Locale.getDefault()
        
        return mapOf(
            "language" to locale.language,
            "country" to locale.country,
            "displayLanguage" to locale.displayLanguage,
            "displayCountry" to locale.displayCountry,
            "languageTag" to locale.toLanguageTag(),
            "isRtl" to (locale.language in listOf("ar", "he", "fa", "ur")),
            "script" to locale.script,
            "variant" to locale.variant
        )
    }
    
    @JvmStatic
    fun getLanguage(): String = Locale.getDefault().language
    
    @JvmStatic
    fun getCountry(): String = Locale.getDefault().country
    
    @JvmStatic
    fun isRtlLocale(): Boolean {
        return Locale.getDefault().language in listOf("ar", "he", "fa", "ur")
    }
    
    @JvmStatic
    fun getTimeZone(): String = TimeZone.getDefault().id
    
    @JvmStatic
    fun getTimeZoneOffset(): Int = TimeZone.getDefault().rawOffset / (1000 * 60 * 60)
}
