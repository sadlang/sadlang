/**
 * =============================================================================
 * @file SadStorage.kt  
 * @brief (AR) واجهة التخزين لأندرويد — قراءة/كتابة الملفات
 * @brief (EN) Android Storage Interface — File Read/Write
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import android.os.Environment
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.io.IOException

/**
 * نظام التخزين للغة ص على أندرويد
 */
class SadStorage(private val context: Context) {
    
    companion object {
        init {
            System.loadLibrary("sad_android")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Native Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    private external fun nativeReadFile(filePath: String): String
    private external fun nativeWriteFile(filePath: String, content: String): Boolean
    private external fun nativeAppendFile(filePath: String, content: String): Boolean
    private external fun nativeDeleteFile(filePath: String): Boolean
    private external fun nativeFileExists(filePath: String): Boolean
    private external fun nativeFileSize(filePath: String): Long
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Storage Locations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * مجلد التطبيق الداخلي
     */
    val internalDir: File
        get() = context.filesDir
    
    /**
     * مجلد الكاش
     */
    val cacheDir: File
        get() = context.cacheDir
    
    /**
     * مجلد التخزين الخارجي
     */
    val externalDir: File?
        get() = context.getExternalFilesDir(null)
    
    /**
     * مجلد المستندات
     */
    val documentsDir: File?
        get() = context.getExternalFilesDir(Environment.DIRECTORY_DOCUMENTS)
    
    /**
     * مجلد الصور
     */
    val picturesDir: File?
        get() = context.getExternalFilesDir(Environment.DIRECTORY_PICTURES)
    
    /**
     * مجلد التنزيلات
     */
    val downloadsDir: File?
        get() = context.getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS)
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  File Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة ملف نصي
     */
    fun readText(path: String): Result<String> {
        return try {
            val file = resolveFile(path)
            if (!file.exists()) {
                Result.failure(IOException("الملف غير موجود: $path"))
            } else {
                Result.success(file.readText(Charsets.UTF_8))
            }
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * كتابة ملف نصي
     */
    fun writeText(path: String, content: String): Result<Unit> {
        return try {
            val file = resolveFile(path)
            file.parentFile?.mkdirs()
            file.writeText(content, Charsets.UTF_8)
            Result.success(Unit)
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * إضافة للملف
     */
    fun appendText(path: String, content: String): Result<Unit> {
        return try {
            val file = resolveFile(path)
            file.appendText(content, Charsets.UTF_8)
            Result.success(Unit)
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * قراءة ملف ثنائي
     */
    fun readBytes(path: String): Result<ByteArray> {
        return try {
            val file = resolveFile(path)
            Result.success(file.readBytes())
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * كتابة ملف ثنائي
     */
    fun writeBytes(path: String, data: ByteArray): Result<Unit> {
        return try {
            val file = resolveFile(path)
            file.parentFile?.mkdirs()
            file.writeBytes(data)
            Result.success(Unit)
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * حذف ملف
     */
    fun delete(path: String): Boolean {
        return try {
            resolveFile(path).delete()
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * فحص وجود ملف
     */
    fun exists(path: String): Boolean {
        return try {
            resolveFile(path).exists()
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * حجم الملف
     */
    fun size(path: String): Long {
        return try {
            resolveFile(path).length()
        } catch (e: Exception) {
            -1
        }
    }
    
    /**
     * قائمة الملفات في مجلد
     */
    fun listFiles(path: String): List<String> {
        return try {
            resolveFile(path).listFiles()
                ?.map { it.name }
                ?: emptyList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    /**
     * إنشاء مجلد
     */
    fun createDirectory(path: String): Boolean {
        return try {
            resolveFile(path).mkdirs()
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * نسخ ملف
     */
    fun copy(source: String, destination: String): Result<Unit> {
        return try {
            val sourceFile = resolveFile(source)
            val destFile = resolveFile(destination)
            destFile.parentFile?.mkdirs()
            sourceFile.copyTo(destFile, overwrite = true)
            Result.success(Unit)
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    /**
     * نقل ملف
     */
    fun move(source: String, destination: String): Result<Unit> {
        return try {
            val sourceFile = resolveFile(source)
            val destFile = resolveFile(destination)
            destFile.parentFile?.mkdirs()
            if (sourceFile.renameTo(destFile)) {
                Result.success(Unit)
            } else {
                // Fallback: copy then delete
                sourceFile.copyTo(destFile, overwrite = true)
                sourceFile.delete()
                Result.success(Unit)
            }
        } catch (e: Exception) {
            Result.failure(e)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  JSON Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة ملف JSON
     */
    fun readJson(path: String): Result<String> {
        return readText(path)
    }
    
    /**
     * كتابة JSON
     */
    fun writeJson(path: String, json: String): Result<Unit> {
        return writeText(path, json)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  SharedPreferences Helper
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val prefs by lazy {
        context.getSharedPreferences("sad_storage", Context.MODE_PRIVATE)
    }
    
    /**
     * حفظ قيمة في التفضيلات
     */
    fun savePreference(key: String, value: String) {
        prefs.edit().putString(key, value).apply()
    }
    
    /**
     * قراءة قيمة من التفضيلات
     */
    fun getPreference(key: String, default: String = ""): String {
        return prefs.getString(key, default) ?: default
    }
    
    /**
     * حذف قيمة من التفضيلات
     */
    fun removePreference(key: String) {
        prefs.edit().remove(key).apply()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Helper
    // ═══════════════════════════════════════════════════════════════════════════
    
    private fun resolveFile(path: String): File {
        return when {
            path.startsWith("/") -> File(path)
            path.startsWith("internal://") -> File(internalDir, path.removePrefix("internal://"))
            path.startsWith("cache://") -> File(cacheDir, path.removePrefix("cache://"))
            path.startsWith("external://") -> File(externalDir ?: internalDir, path.removePrefix("external://"))
            path.startsWith("documents://") -> File(documentsDir ?: internalDir, path.removePrefix("documents://"))
            else -> File(internalDir, path)
        }
    }
}
