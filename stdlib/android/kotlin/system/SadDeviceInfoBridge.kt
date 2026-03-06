// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDeviceInfoBridge.kt
// الوصف: جسر Kotlin لمعلومات الجهاز
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.ActivityManager
import android.content.Context
import android.content.res.Configuration
import android.os.Build
import android.os.Environment
import android.os.StatFs
import android.provider.Settings
import android.util.DisplayMetrics
import android.view.WindowManager
import java.util.*

/**
 * جسر معلومات الجهاز — SadDeviceInfoBridge
 */
object SadDeviceInfoBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // معلومات النظام
    @JvmStatic
    fun getAndroidVersion(): String = Build.VERSION.RELEASE
    
    @JvmStatic
    fun getAndroidSdkInt(): Int = Build.VERSION.SDK_INT
    
    @JvmStatic
    fun getSecurityPatch(): String = 
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) Build.VERSION.SECURITY_PATCH else ""
    
    // معلومات الجهاز
    @JvmStatic
    fun getManufacturer(): String = Build.MANUFACTURER
    
    @JvmStatic
    fun getModel(): String = Build.MODEL
    
    @JvmStatic
    fun getBrand(): String = Build.BRAND
    
    @JvmStatic
    fun getDevice(): String = Build.DEVICE
    
    @JvmStatic
    fun getProduct(): String = Build.PRODUCT
    
    @JvmStatic
    fun getHardware(): String = Build.HARDWARE
    
    @JvmStatic
    fun getDeviceName(): String {
        val ctx = context ?: return getModel()
        return Settings.Global.getString(ctx.contentResolver, Settings.Global.DEVICE_NAME) ?: getModel()
    }
    
    @JvmStatic
    fun getAndroidId(): String {
        val ctx = context ?: return ""
        return Settings.Secure.getString(ctx.contentResolver, Settings.Secure.ANDROID_ID) ?: ""
    }
    
    // معلومات الشاشة
    @JvmStatic
    fun getScreenInfo(): Map<String, Any> {
        val ctx = context ?: return emptyMap()
        val wm = ctx.getSystemService(Context.WINDOW_SERVICE) as? WindowManager ?: return emptyMap()
        val metrics = DisplayMetrics()
        
        @Suppress("DEPRECATION")
        wm.defaultDisplay.getRealMetrics(metrics)
        
        return mapOf(
            "widthPixels" to metrics.widthPixels,
            "heightPixels" to metrics.heightPixels,
            "density" to metrics.density,
            "densityDpi" to metrics.densityDpi,
            "scaledDensity" to metrics.scaledDensity,
            "xdpi" to metrics.xdpi,
            "ydpi" to metrics.ydpi
        )
    }
    
    @JvmStatic
    fun getScreenDensityCategory(): String {
        val ctx = context ?: return "غير معروف"
        return when (ctx.resources.displayMetrics.densityDpi) {
            DisplayMetrics.DENSITY_LOW -> "ldpi"
            DisplayMetrics.DENSITY_MEDIUM -> "mdpi"
            DisplayMetrics.DENSITY_HIGH -> "hdpi"
            DisplayMetrics.DENSITY_XHIGH -> "xhdpi"
            DisplayMetrics.DENSITY_XXHIGH -> "xxhdpi"
            DisplayMetrics.DENSITY_XXXHIGH -> "xxxhdpi"
            else -> "غير معروف"
        }
    }
    
    // معلومات الذاكرة
    @JvmStatic
    fun getMemoryInfo(): Map<String, Long> {
        val ctx = context ?: return emptyMap()
        val am = ctx.getSystemService(Context.ACTIVITY_SERVICE) as? ActivityManager ?: return emptyMap()
        val memInfo = ActivityManager.MemoryInfo()
        am.getMemoryInfo(memInfo)
        
        return mapOf(
            "totalMemory" to memInfo.totalMem,
            "availableMemory" to memInfo.availMem,
            "usedMemory" to (memInfo.totalMem - memInfo.availMem),
            "threshold" to memInfo.threshold,
            "lowMemory" to if (memInfo.lowMemory) 1L else 0L
        )
    }
    
    @JvmStatic
    fun getAvailableMemoryMB(): Long {
        val memInfo = getMemoryInfo()
        return (memInfo["availableMemory"] ?: 0L) / (1024 * 1024)
    }
    
    @JvmStatic
    fun getTotalMemoryMB(): Long {
        val memInfo = getMemoryInfo()
        return (memInfo["totalMemory"] ?: 0L) / (1024 * 1024)
    }
    
    // معلومات التخزين
    @JvmStatic
    fun getStorageInfo(): Map<String, Long> {
        val path = Environment.getDataDirectory()
        val stat = StatFs(path.path)
        
        val blockSize = stat.blockSizeLong
        val totalBlocks = stat.blockCountLong
        val availableBlocks = stat.availableBlocksLong
        
        return mapOf(
            "totalStorage" to (totalBlocks * blockSize),
            "availableStorage" to (availableBlocks * blockSize),
            "usedStorage" to ((totalBlocks - availableBlocks) * blockSize)
        )
    }
    
    @JvmStatic
    fun getAvailableStorageGB(): Float {
        val storageInfo = getStorageInfo()
        return (storageInfo["availableStorage"] ?: 0L) / (1024f * 1024f * 1024f)
    }
    
    @JvmStatic
    fun getTotalStorageGB(): Float {
        val storageInfo = getStorageInfo()
        return (storageInfo["totalStorage"] ?: 0L) / (1024f * 1024f * 1024f)
    }
    
    // معلومات اللغة والمنطقة
    @JvmStatic
    fun getSystemLanguage(): String = Locale.getDefault().language
    
    @JvmStatic
    fun getSystemCountry(): String = Locale.getDefault().country
    
    @JvmStatic
    fun getSystemLocale(): String = Locale.getDefault().toString()
    
    @JvmStatic
    fun isRTL(): Boolean {
        val ctx = context ?: return false
        return ctx.resources.configuration.layoutDirection == Configuration.LAYOUT_DIRECTION_RTL
    }
    
    // معلومات شاملة
    @JvmStatic
    fun getFullDeviceInfo(): Map<String, Any> {
        return mapOf(
            "manufacturer" to getManufacturer(),
            "model" to getModel(),
            "brand" to getBrand(),
            "device" to getDevice(),
            "product" to getProduct(),
            "hardware" to getHardware(),
            "deviceName" to getDeviceName(),
            "androidVersion" to getAndroidVersion(),
            "sdkInt" to getAndroidSdkInt(),
            "securityPatch" to getSecurityPatch(),
            "language" to getSystemLanguage(),
            "country" to getSystemCountry(),
            "locale" to getSystemLocale(),
            "isRTL" to isRTL(),
            "screen" to getScreenInfo(),
            "memory" to getMemoryInfo(),
            "storage" to getStorageInfo()
        )
    }
    
    @JvmStatic
    fun isEmulator(): Boolean {
        return Build.FINGERPRINT.startsWith("generic") ||
               Build.FINGERPRINT.startsWith("unknown") ||
               Build.MODEL.contains("google_sdk") ||
               Build.MODEL.contains("Emulator") ||
               Build.MODEL.contains("Android SDK built for x86") ||
               Build.MANUFACTURER.contains("Genymotion") ||
               Build.BRAND.startsWith("generic") ||
               Build.DEVICE.startsWith("generic") ||
               Build.PRODUCT == "sdk" ||
               Build.PRODUCT == "google_sdk" ||
               Build.PRODUCT == "sdk_x86" ||
               Build.PRODUCT == "vbox86p" ||
               Build.HARDWARE.contains("goldfish") ||
               Build.HARDWARE.contains("ranchu")
    }
    
    @JvmStatic
    fun isTablet(): Boolean {
        val ctx = context ?: return false
        return ctx.resources.configuration.smallestScreenWidthDp >= 600
    }
}
