// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadImageLoaderBridge.kt
// الوصف: جسر Kotlin لتحميل الصور (Glide/Coil compatible)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.drawable.Drawable
import android.os.Handler
import android.os.Looper
import android.widget.ImageView
import java.io.File
import java.io.FileOutputStream
import java.io.InputStream
import java.net.HttpURLConnection
import java.net.URL
import java.security.MessageDigest
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.Executors

/**
 * جسر تحميل الصور — SadImageLoaderBridge
 */
object SadImageLoaderBridge {
    
    private var context: Context? = null
    private val executor = Executors.newFixedThreadPool(4)
    private val mainHandler = Handler(Looper.getMainLooper())
    
    // Memory cache
    private val memoryCache = ConcurrentHashMap<String, Bitmap>()
    private var maxMemoryCacheSize = 50 // Max number of cached images
    
    // Disk cache directory
    private var diskCacheDir: File? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        diskCacheDir = File(ctx.cacheDir, "sad_image_cache")
        diskCacheDir?.mkdirs()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Configuration
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setMemoryCacheSize(maxImages: Int) {
        maxMemoryCacheSize = maxImages
    }
    
    @JvmStatic
    fun clearMemoryCache() {
        memoryCache.clear()
    }
    
    @JvmStatic
    fun clearDiskCache(): Boolean {
        return try {
            diskCacheDir?.listFiles()?.forEach { it.delete() }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getCacheSize(): Map<String, Any> {
        val memoryCount = memoryCache.size
        val diskCount = diskCacheDir?.listFiles()?.size ?: 0
        val diskSizeBytes = diskCacheDir?.listFiles()
            ?.sumOf { it.length() } ?: 0L
        
        return mapOf(
            "memoryCacheCount" to memoryCount,
            "diskCacheCount" to diskCount,
            "diskCacheSizeBytes" to diskSizeBytes,
            "diskCacheSizeMB" to String.format("%.2f", diskSizeBytes / (1024.0 * 1024.0))
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Load into ImageView
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun loadUrl(
        url: String,
        imageView: ImageView,
        placeholderResId: Int? = null,
        errorResId: Int? = null,
        callback: ((Boolean, String) -> Unit)? = null
    ) {
        // Set placeholder
        placeholderResId?.let { imageView.setImageResource(it) }
        
        executor.execute {
            try {
                val bitmap = loadBitmapFromUrl(url)
                
                if (bitmap != null) {
                    mainHandler.post {
                        imageView.setImageBitmap(bitmap)
                        callback?.invoke(true, "تم تحميل الصورة بنجاح")
                    }
                } else {
                    mainHandler.post {
                        errorResId?.let { imageView.setImageResource(it) }
                        callback?.invoke(false, "فشل تحميل الصورة")
                    }
                }
            } catch (e: Exception) {
                mainHandler.post {
                    errorResId?.let { imageView.setImageResource(it) }
                    callback?.invoke(false, "خطأ: ${e.message}")
                }
            }
        }
    }
    
    @JvmStatic
    fun loadFile(
        filePath: String,
        imageView: ImageView,
        placeholderResId: Int? = null,
        callback: ((Boolean, String) -> Unit)? = null
    ) {
        placeholderResId?.let { imageView.setImageResource(it) }
        
        executor.execute {
            try {
                val bitmap = loadBitmapFromFile(filePath)
                
                if (bitmap != null) {
                    mainHandler.post {
                        imageView.setImageBitmap(bitmap)
                        callback?.invoke(true, "تم تحميل الصورة من الملف")
                    }
                } else {
                    mainHandler.post {
                        callback?.invoke(false, "فشل تحميل الصورة من الملف")
                    }
                }
            } catch (e: Exception) {
                mainHandler.post {
                    callback?.invoke(false, "خطأ: ${e.message}")
                }
            }
        }
    }
    
    @JvmStatic
    fun loadResource(
        resourceId: Int,
        imageView: ImageView
    ): Boolean {
        return try {
            imageView.setImageResource(resourceId)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Load as Bitmap
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun loadBitmapFromUrl(
        url: String,
        width: Int? = null,
        height: Int? = null
    ): Bitmap? {
        val cacheKey = generateCacheKey(url, width, height)
        
        // Check memory cache
        memoryCache[cacheKey]?.let { return it }
        
        // Check disk cache
        val cachedFile = getCachedFile(cacheKey)
        if (cachedFile.exists()) {
            val bitmap = decodeBitmap(cachedFile.absolutePath, width, height)
            bitmap?.let { addToMemoryCache(cacheKey, it) }
            return bitmap
        }
        
        // Download
        return try {
            val connection = URL(url).openConnection() as HttpURLConnection
            connection.connectTimeout = 15000
            connection.readTimeout = 15000
            connection.doInput = true
            connection.connect()
            
            if (connection.responseCode == HttpURLConnection.HTTP_OK) {
                val inputStream = connection.inputStream
                
                // Save to disk cache
                saveToDiskCache(cachedFile, inputStream)
                
                // Decode
                val bitmap = decodeBitmap(cachedFile.absolutePath, width, height)
                bitmap?.let { addToMemoryCache(cacheKey, it) }
                bitmap
            } else {
                null
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun loadBitmapFromFile(
        filePath: String,
        width: Int? = null,
        height: Int? = null
    ): Bitmap? {
        val cacheKey = generateCacheKey("file://$filePath", width, height)
        
        // Check memory cache
        memoryCache[cacheKey]?.let { return it }
        
        return try {
            val bitmap = decodeBitmap(filePath, width, height)
            bitmap?.let { addToMemoryCache(cacheKey, it) }
            bitmap
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun loadBitmapFromResource(resourceId: Int): Bitmap? {
        val ctx = context ?: return null
        return try {
            BitmapFactory.decodeResource(ctx.resources, resourceId)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun loadBitmapAsync(
        url: String,
        width: Int? = null,
        height: Int? = null,
        callback: (Bitmap?) -> Unit
    ) {
        executor.execute {
            val bitmap = loadBitmapFromUrl(url, width, height)
            mainHandler.post { callback(bitmap) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Bitmap Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun resizeBitmap(bitmap: Bitmap, width: Int, height: Int): Bitmap {
        return Bitmap.createScaledBitmap(bitmap, width, height, true)
    }
    
    @JvmStatic
    fun cropBitmap(bitmap: Bitmap, x: Int, y: Int, width: Int, height: Int): Bitmap? {
        return try {
            Bitmap.createBitmap(bitmap, x, y, width, height)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun cropToCircle(bitmap: Bitmap): Bitmap {
        val size = minOf(bitmap.width, bitmap.height)
        val output = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        
        val canvas = android.graphics.Canvas(output)
        val paint = android.graphics.Paint().apply {
            isAntiAlias = true
            isFilterBitmap = true
        }
        
        val rect = android.graphics.Rect(0, 0, size, size)
        
        canvas.drawARGB(0, 0, 0, 0)
        canvas.drawCircle(size / 2f, size / 2f, size / 2f, paint)
        
        paint.xfermode = android.graphics.PorterDuffXfermode(android.graphics.PorterDuff.Mode.SRC_IN)
        
        val srcRect = android.graphics.Rect(
            (bitmap.width - size) / 2,
            (bitmap.height - size) / 2,
            (bitmap.width + size) / 2,
            (bitmap.height + size) / 2
        )
        
        canvas.drawBitmap(bitmap, srcRect, rect, paint)
        
        return output
    }
    
    @JvmStatic
    fun cropToRoundedRect(bitmap: Bitmap, radius: Float): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = android.graphics.Canvas(output)
        val paint = android.graphics.Paint().apply {
            isAntiAlias = true
            isFilterBitmap = true
        }
        
        val rect = android.graphics.RectF(0f, 0f, bitmap.width.toFloat(), bitmap.height.toFloat())
        
        canvas.drawARGB(0, 0, 0, 0)
        canvas.drawRoundRect(rect, radius, radius, paint)
        
        paint.xfermode = android.graphics.PorterDuffXfermode(android.graphics.PorterDuff.Mode.SRC_IN)
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        
        return output
    }
    
    @JvmStatic
    fun rotateBitmap(bitmap: Bitmap, degrees: Float): Bitmap {
        val matrix = android.graphics.Matrix()
        matrix.postRotate(degrees)
        return Bitmap.createBitmap(bitmap, 0, 0, bitmap.width, bitmap.height, matrix, true)
    }
    
    @JvmStatic
    fun flipBitmap(bitmap: Bitmap, horizontal: Boolean): Bitmap {
        val matrix = android.graphics.Matrix()
        if (horizontal) {
            matrix.preScale(-1f, 1f)
        } else {
            matrix.preScale(1f, -1f)
        }
        return Bitmap.createBitmap(bitmap, 0, 0, bitmap.width, bitmap.height, matrix, true)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Save Bitmap
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun saveBitmap(
        bitmap: Bitmap,
        filePath: String,
        format: String = "PNG",
        quality: Int = 100
    ): Boolean {
        return try {
            val file = File(filePath)
            file.parentFile?.mkdirs()
            
            val compressFormat = when (format.uppercase()) {
                "JPEG", "JPG" -> Bitmap.CompressFormat.JPEG
                "WEBP" -> Bitmap.CompressFormat.WEBP
                else -> Bitmap.CompressFormat.PNG
            }
            
            FileOutputStream(file).use { out ->
                bitmap.compress(compressFormat, quality, out)
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Image Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getImageInfo(filePath: String): Map<String, Any>? {
        return try {
            val options = BitmapFactory.Options().apply {
                inJustDecodeBounds = true
            }
            BitmapFactory.decodeFile(filePath, options)
            
            mapOf(
                "width" to options.outWidth,
                "height" to options.outHeight,
                "mimeType" to (options.outMimeType ?: "غير معروف")
            )
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getBitmapInfo(bitmap: Bitmap): Map<String, Any> {
        return mapOf(
            "width" to bitmap.width,
            "height" to bitmap.height,
            "byteCount" to bitmap.byteCount,
            "hasAlpha" to bitmap.hasAlpha(),
            "config" to (bitmap.config?.name ?: "غير معروف")
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Preloading
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun preload(urls: List<String>, callback: ((Int, Int) -> Unit)? = null) {
        var completed = 0
        val total = urls.size
        
        urls.forEach { url ->
            executor.execute {
                loadBitmapFromUrl(url)
                synchronized(this) {
                    completed++
                    mainHandler.post {
                        callback?.invoke(completed, total)
                    }
                }
            }
        }
    }
    
    @JvmStatic
    fun cancelAll() {
        // Note: For a real implementation, you would track and cancel ongoing tasks
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Internal Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun generateCacheKey(url: String, width: Int?, height: Int?): String {
        val key = "$url-${width ?: 0}-${height ?: 0}"
        val digest = MessageDigest.getInstance("MD5")
        val bytes = digest.digest(key.toByteArray())
        return bytes.joinToString("") { "%02x".format(it) }
    }
    
    private fun getCachedFile(cacheKey: String): File {
        return File(diskCacheDir, cacheKey)
    }
    
    private fun saveToDiskCache(file: File, inputStream: InputStream) {
        FileOutputStream(file).use { output ->
            inputStream.copyTo(output)
        }
    }
    
    private fun addToMemoryCache(key: String, bitmap: Bitmap) {
        // Evict old entries if needed
        while (memoryCache.size >= maxMemoryCacheSize) {
            val oldestKey = memoryCache.keys.firstOrNull()
            oldestKey?.let { memoryCache.remove(it) }
        }
        memoryCache[key] = bitmap
    }
    
    private fun decodeBitmap(filePath: String, targetWidth: Int?, targetHeight: Int?): Bitmap? {
        if (targetWidth == null && targetHeight == null) {
            return BitmapFactory.decodeFile(filePath)
        }
        
        // Get dimensions first
        val options = BitmapFactory.Options().apply {
            inJustDecodeBounds = true
        }
        BitmapFactory.decodeFile(filePath, options)
        
        // Calculate sample size
        options.inSampleSize = calculateInSampleSize(
            options.outWidth,
            options.outHeight,
            targetWidth ?: options.outWidth,
            targetHeight ?: options.outHeight
        )
        options.inJustDecodeBounds = false
        
        return BitmapFactory.decodeFile(filePath, options)
    }
    
    private fun calculateInSampleSize(
        actualWidth: Int,
        actualHeight: Int,
        reqWidth: Int,
        reqHeight: Int
    ): Int {
        var inSampleSize = 1
        
        if (actualHeight > reqHeight || actualWidth > reqWidth) {
            val halfHeight = actualHeight / 2
            val halfWidth = actualWidth / 2
            
            while ((halfHeight / inSampleSize) >= reqHeight &&
                   (halfWidth / inSampleSize) >= reqWidth) {
                inSampleSize *= 2
            }
        }
        
        return inSampleSize
    }
}
