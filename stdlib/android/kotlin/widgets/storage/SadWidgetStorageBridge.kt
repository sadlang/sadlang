package sad.android.widgets.storage

import android.content.Context
import android.os.Environment
import android.os.StatFs
import android.os.Handler
import android.os.Looper
import java.io.File

/**
 * جسر ويدجت التخزين - عرض مساحة التخزين
 * Storage Widget Bridge - Display storage space
 * 
 * @author فريق لغة ص
 */
object SadWidgetStorageBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * معلومات التخزين / Storage info
     */
    data class StorageInfo(
        val totalBytes: Long,
        val usedBytes: Long,
        val freeBytes: Long,
        val usagePercent: Float,
        val totalFormatted: String,
        val usedFormatted: String,
        val freeFormatted: String
    )
    
    /**
     * معلومات التخزين التفصيلية / Detailed storage info
     */
    data class DetailedStorageInfo(
        val internal: StorageInfo,
        val external: StorageInfo?,
        val appSize: Long,
        val cacheSize: Long,
        val dataSize: Long
    )
    
    /**
     * مستمع التخزين / Storage listener
     */
    interface StorageListener {
        fun onStorageUpdated(info: StorageInfo)
        fun onLowStorage(freePercent: Float)
    }
    
    private val storageListeners = mutableListOf<StorageListener>()
    
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
    fun addStorageListener(listener: StorageListener) {
        if (!storageListeners.contains(listener)) {
            storageListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeStorageListener(listener: StorageListener) {
        storageListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات التخزين
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات التخزين الداخلي / Get internal storage info
     */
    fun getInternalStorageInfo(): StorageInfo {
        val path = Environment.getDataDirectory()
        return getStorageInfo(path)
    }
    
    /**
     * الحصول على معلومات التخزين الخارجي / Get external storage info
     */
    fun getExternalStorageInfo(): StorageInfo? {
        val state = Environment.getExternalStorageState()
        if (state != Environment.MEDIA_MOUNTED && state != Environment.MEDIA_MOUNTED_READ_ONLY) {
            return null
        }
        
        val path = Environment.getExternalStorageDirectory()
        return getStorageInfo(path)
    }
    
    /**
     * الحصول على معلومات التخزين التفصيلية / Get detailed storage info
     */
    fun getDetailedStorageInfo(): DetailedStorageInfo {
        val ctx = context
        
        return DetailedStorageInfo(
            internal = getInternalStorageInfo(),
            external = getExternalStorageInfo(),
            appSize = ctx?.let { getAppSize(it) } ?: 0,
            cacheSize = ctx?.let { getCacheSize(it) } ?: 0,
            dataSize = ctx?.let { getDataSize(it) } ?: 0
        )
    }
    
    private fun getStorageInfo(path: File): StorageInfo {
        val statFs = StatFs(path.absolutePath)
        
        val totalBytes = statFs.blockCountLong * statFs.blockSizeLong
        val freeBytes = statFs.availableBlocksLong * statFs.blockSizeLong
        val usedBytes = totalBytes - freeBytes
        val usagePercent = (usedBytes.toFloat() / totalBytes) * 100
        
        return StorageInfo(
            totalBytes = totalBytes,
            usedBytes = usedBytes,
            freeBytes = freeBytes,
            usagePercent = usagePercent,
            totalFormatted = formatBytes(totalBytes),
            usedFormatted = formatBytes(usedBytes),
            freeFormatted = formatBytes(freeBytes)
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         حجم التطبيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حجم التطبيق / App size
     */
    fun getAppSize(ctx: Context): Long {
        val appDir = ctx.applicationInfo.sourceDir
        return File(appDir).length()
    }
    
    /**
     * حجم الكاش / Cache size
     */
    fun getCacheSize(ctx: Context): Long {
        return getDirSize(ctx.cacheDir) + (ctx.externalCacheDir?.let { getDirSize(it) } ?: 0)
    }
    
    /**
     * حجم البيانات / Data size
     */
    fun getDataSize(ctx: Context): Long {
        return getDirSize(ctx.filesDir)
    }
    
    private fun getDirSize(dir: File): Long {
        var size: Long = 0
        if (dir.exists()) {
            dir.listFiles()?.forEach { file ->
                size += if (file.isDirectory) getDirSize(file) else file.length()
            }
        }
        return size
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تنظيف الكاش
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مسح الكاش / Clear cache
     */
    fun clearCache(): Boolean {
        val ctx = context ?: return false
        
        var success = deleteDir(ctx.cacheDir)
        ctx.externalCacheDir?.let {
            success = success && deleteDir(it)
        }
        
        return success
    }
    
    private fun deleteDir(dir: File): Boolean {
        if (dir.isDirectory) {
            dir.listFiles()?.forEach { file ->
                if (!deleteDir(file)) return false
            }
        }
        return dir.delete()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق البايت / Format bytes
     */
    fun formatBytes(bytes: Long): String {
        val kb = 1024.0
        val mb = kb * 1024
        val gb = mb * 1024
        val tb = gb * 1024
        
        return when {
            bytes >= tb -> String.format("%.2f تيرا", bytes / tb)
            bytes >= gb -> String.format("%.2f جيجا", bytes / gb)
            bytes >= mb -> String.format("%.2f ميجا", bytes / mb)
            bytes >= kb -> String.format("%.2f كيلو", bytes / kb)
            else -> "$bytes بايت"
        }
    }
    
    /**
     * الحصول على لون الحالة / Get status color
     */
    fun getStatusColor(usagePercent: Float): Int {
        return when {
            usagePercent >= 90 -> 0xFFE53935.toInt()  // أحمر
            usagePercent >= 75 -> 0xFFFF9800.toInt()  // برتقالي
            usagePercent >= 50 -> 0xFFFFC107.toInt()  // أصفر
            else -> 0xFF4CAF50.toInt()                // أخضر
        }
    }
    
    /**
     * الحصول على رسالة الحالة / Get status message
     */
    fun getStatusMessage(usagePercent: Float): String {
        return when {
            usagePercent >= 90 -> "⚠️ المساحة شارفت على النفاد!"
            usagePercent >= 75 -> "تحذير: المساحة منخفضة"
            usagePercent >= 50 -> "المساحة متوسطة"
            else -> "✓ المساحة جيدة"
        }
    }
    
    /**
     * تحديث ويدجت التخزين / Update storage widget
     */
    fun updateWidget() {
        val info = getInternalStorageInfo()
        val freePercent = 100 - info.usagePercent
        
        mainHandler.post {
            storageListeners.forEach { it.onStorageUpdated(info) }
            
            if (freePercent < 10) {
                storageListeners.forEach { it.onLowStorage(freePercent) }
            }
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        storageListeners.clear()
        context = null
    }
}
