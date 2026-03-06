// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadImageBridge.kt
// الوصف: جسر Kotlin لمعالجة الصور
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.graphics.*
import android.media.ExifInterface
import android.net.Uri
import android.os.Build
import android.util.Base64
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream

/**
 * جسر الصور — SadImageBridge
 */
object SadImageBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // التحميل
    @JvmStatic
    fun loadFromFile(path: String): Bitmap? {
        return try {
            BitmapFactory.decodeFile(path)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun loadFromUri(uriString: String): Bitmap? {
        val ctx = context ?: return null
        return try {
            val uri = Uri.parse(uriString)
            ctx.contentResolver.openInputStream(uri)?.use { stream ->
                BitmapFactory.decodeStream(stream)
            }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun loadFromBase64(base64String: String): Bitmap? {
        return try {
            val bytes = Base64.decode(base64String, Base64.DEFAULT)
            BitmapFactory.decodeByteArray(bytes, 0, bytes.size)
        } catch (e: Exception) {
            null
        }
    }
    
    // الحفظ
    @JvmStatic
    fun saveToFile(bitmap: Bitmap, path: String, format: String = "PNG", quality: Int = 100): Boolean {
        return try {
            val compressFormat = when (format.uppercase()) {
                "JPEG", "JPG" -> Bitmap.CompressFormat.JPEG
                "WEBP" -> if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) 
                    Bitmap.CompressFormat.WEBP_LOSSLESS else Bitmap.CompressFormat.WEBP
                else -> Bitmap.CompressFormat.PNG
            }
            
            FileOutputStream(path).use { out ->
                bitmap.compress(compressFormat, quality, out)
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun toBase64(bitmap: Bitmap, format: String = "PNG", quality: Int = 100): String {
        val outputStream = ByteArrayOutputStream()
        val compressFormat = when (format.uppercase()) {
            "JPEG", "JPG" -> Bitmap.CompressFormat.JPEG
            else -> Bitmap.CompressFormat.PNG
        }
        bitmap.compress(compressFormat, quality, outputStream)
        val bytes = outputStream.toByteArray()
        return Base64.encodeToString(bytes, Base64.DEFAULT)
    }
    
    @JvmStatic
    fun toBytes(bitmap: Bitmap, format: String = "PNG", quality: Int = 100): ByteArray {
        val outputStream = ByteArrayOutputStream()
        val compressFormat = when (format.uppercase()) {
            "JPEG", "JPG" -> Bitmap.CompressFormat.JPEG
            else -> Bitmap.CompressFormat.PNG
        }
        bitmap.compress(compressFormat, quality, outputStream)
        return outputStream.toByteArray()
    }
    
    // المعلومات
    @JvmStatic
    fun getWidth(bitmap: Bitmap): Int = bitmap.width
    
    @JvmStatic
    fun getHeight(bitmap: Bitmap): Int = bitmap.height
    
    @JvmStatic
    fun getInfo(path: String): Map<String, Any>? {
        val options = BitmapFactory.Options().apply {
            inJustDecodeBounds = true
        }
        BitmapFactory.decodeFile(path, options)
        
        if (options.outWidth == -1) return null
        
        val file = File(path)
        
        return mapOf(
            "width" to options.outWidth,
            "height" to options.outHeight,
            "mimeType" to (options.outMimeType ?: "unknown"),
            "size" to file.length()
        )
    }
    
    // التحويلات
    @JvmStatic
    fun resize(bitmap: Bitmap, width: Int, height: Int): Bitmap {
        return Bitmap.createScaledBitmap(bitmap, width, height, true)
    }
    
    @JvmStatic
    fun resizeKeepAspectRatio(bitmap: Bitmap, maxWidth: Int, maxHeight: Int): Bitmap {
        val ratio = minOf(
            maxWidth.toFloat() / bitmap.width,
            maxHeight.toFloat() / bitmap.height
        )
        
        val newWidth = (bitmap.width * ratio).toInt()
        val newHeight = (bitmap.height * ratio).toInt()
        
        return resize(bitmap, newWidth, newHeight)
    }
    
    @JvmStatic
    fun crop(bitmap: Bitmap, x: Int, y: Int, width: Int, height: Int): Bitmap {
        return Bitmap.createBitmap(bitmap, x, y, width, height)
    }
    
    @JvmStatic
    fun cropCenter(bitmap: Bitmap, width: Int, height: Int): Bitmap {
        val x = (bitmap.width - width) / 2
        val y = (bitmap.height - height) / 2
        return crop(bitmap, x, y, width, height)
    }
    
    @JvmStatic
    fun cropCircle(bitmap: Bitmap): Bitmap {
        val size = minOf(bitmap.width, bitmap.height)
        val output = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888)
        
        val canvas = Canvas(output)
        val paint = Paint().apply {
            isAntiAlias = true
        }
        
        val rect = Rect(0, 0, size, size)
        canvas.drawCircle(size / 2f, size / 2f, size / 2f, paint)
        
        paint.xfermode = PorterDuffXfermode(PorterDuff.Mode.SRC_IN)
        
        val srcRect = Rect(
            (bitmap.width - size) / 2,
            (bitmap.height - size) / 2,
            (bitmap.width + size) / 2,
            (bitmap.height + size) / 2
        )
        
        canvas.drawBitmap(bitmap, srcRect, rect, paint)
        
        return output
    }
    
    @JvmStatic
    fun rotate(bitmap: Bitmap, degrees: Float): Bitmap {
        val matrix = Matrix().apply {
            postRotate(degrees)
        }
        return Bitmap.createBitmap(bitmap, 0, 0, bitmap.width, bitmap.height, matrix, true)
    }
    
    @JvmStatic
    fun flip(bitmap: Bitmap, horizontal: Boolean = true): Bitmap {
        val matrix = Matrix().apply {
            if (horizontal) {
                postScale(-1f, 1f, bitmap.width / 2f, bitmap.height / 2f)
            } else {
                postScale(1f, -1f, bitmap.width / 2f, bitmap.height / 2f)
            }
        }
        return Bitmap.createBitmap(bitmap, 0, 0, bitmap.width, bitmap.height, matrix, true)
    }
    
    // الفلاتر
    @JvmStatic
    fun toGrayscale(bitmap: Bitmap): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        val colorMatrix = ColorMatrix().apply {
            setSaturation(0f)
        }
        
        val paint = Paint().apply {
            colorFilter = ColorMatrixColorFilter(colorMatrix)
        }
        
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        return output
    }
    
    @JvmStatic
    fun adjustBrightness(bitmap: Bitmap, brightness: Float): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        val colorMatrix = ColorMatrix(floatArrayOf(
            1f, 0f, 0f, 0f, brightness,
            0f, 1f, 0f, 0f, brightness,
            0f, 0f, 1f, 0f, brightness,
            0f, 0f, 0f, 1f, 0f
        ))
        
        val paint = Paint().apply {
            colorFilter = ColorMatrixColorFilter(colorMatrix)
        }
        
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        return output
    }
    
    @JvmStatic
    fun adjustContrast(bitmap: Bitmap, contrast: Float): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        val t = (1f - contrast) / 2f * 255f
        val colorMatrix = ColorMatrix(floatArrayOf(
            contrast, 0f, 0f, 0f, t,
            0f, contrast, 0f, 0f, t,
            0f, 0f, contrast, 0f, t,
            0f, 0f, 0f, 1f, 0f
        ))
        
        val paint = Paint().apply {
            colorFilter = ColorMatrixColorFilter(colorMatrix)
        }
        
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        return output
    }
    
    @JvmStatic
    fun adjustSaturation(bitmap: Bitmap, saturation: Float): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        val colorMatrix = ColorMatrix().apply {
            setSaturation(saturation)
        }
        
        val paint = Paint().apply {
            colorFilter = ColorMatrixColorFilter(colorMatrix)
        }
        
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        return output
    }
    
    @JvmStatic
    fun invert(bitmap: Bitmap): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        val colorMatrix = ColorMatrix(floatArrayOf(
            -1f, 0f, 0f, 0f, 255f,
            0f, -1f, 0f, 0f, 255f,
            0f, 0f, -1f, 0f, 255f,
            0f, 0f, 0f, 1f, 0f
        ))
        
        val paint = Paint().apply {
            colorFilter = ColorMatrixColorFilter(colorMatrix)
        }
        
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        return output
    }
    
    @JvmStatic
    fun sepia(bitmap: Bitmap): Bitmap {
        val output = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        val colorMatrix = ColorMatrix(floatArrayOf(
            0.393f, 0.769f, 0.189f, 0f, 0f,
            0.349f, 0.686f, 0.168f, 0f, 0f,
            0.272f, 0.534f, 0.131f, 0f, 0f,
            0f, 0f, 0f, 1f, 0f
        ))
        
        val paint = Paint().apply {
            colorFilter = ColorMatrixColorFilter(colorMatrix)
        }
        
        canvas.drawBitmap(bitmap, 0f, 0f, paint)
        return output
    }
    
    // الدمج
    @JvmStatic
    fun overlay(base: Bitmap, overlay: Bitmap, x: Int = 0, y: Int = 0, alpha: Int = 255): Bitmap {
        val output = Bitmap.createBitmap(base.width, base.height, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(output)
        
        canvas.drawBitmap(base, 0f, 0f, null)
        
        val paint = Paint().apply {
            this.alpha = alpha
        }
        
        canvas.drawBitmap(overlay, x.toFloat(), y.toFloat(), paint)
        
        return output
    }
    
    // تصحيح الاتجاه
    @JvmStatic
    fun fixOrientation(path: String): Bitmap? {
        val bitmap = loadFromFile(path) ?: return null
        
        return try {
            val exif = ExifInterface(path)
            val orientation = exif.getAttributeInt(
                ExifInterface.TAG_ORIENTATION,
                ExifInterface.ORIENTATION_NORMAL
            )
            
            val matrix = Matrix()
            when (orientation) {
                ExifInterface.ORIENTATION_ROTATE_90 -> matrix.postRotate(90f)
                ExifInterface.ORIENTATION_ROTATE_180 -> matrix.postRotate(180f)
                ExifInterface.ORIENTATION_ROTATE_270 -> matrix.postRotate(270f)
                ExifInterface.ORIENTATION_FLIP_HORIZONTAL -> matrix.postScale(-1f, 1f)
                ExifInterface.ORIENTATION_FLIP_VERTICAL -> matrix.postScale(1f, -1f)
                else -> return bitmap
            }
            
            Bitmap.createBitmap(bitmap, 0, 0, bitmap.width, bitmap.height, matrix, true)
        } catch (e: Exception) {
            bitmap
        }
    }
    
    // تنظيف الذاكرة
    @JvmStatic
    fun recycle(bitmap: Bitmap) {
        if (!bitmap.isRecycled) {
            bitmap.recycle()
        }
    }
}
