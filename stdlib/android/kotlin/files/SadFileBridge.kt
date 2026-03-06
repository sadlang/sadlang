// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFileBridge.kt
// الوصف: جسر Kotlin للملفات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Environment
import java.io.*

/**
 * جسر الملفات — SadFileBridge
 */
object SadFileBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // مسارات النظام
    @JvmStatic
    fun getInternalFilesDir(): String {
        return context?.filesDir?.absolutePath ?: ""
    }
    
    @JvmStatic
    fun getInternalCacheDir(): String {
        return context?.cacheDir?.absolutePath ?: ""
    }
    
    @JvmStatic
    fun getExternalFilesDir(type: String? = null): String {
        return context?.getExternalFilesDir(type)?.absolutePath ?: ""
    }
    
    @JvmStatic
    fun getExternalCacheDir(): String {
        return context?.externalCacheDir?.absolutePath ?: ""
    }
    
    @JvmStatic
    fun getDownloadsDir(): String {
        return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS)?.absolutePath ?: ""
    }
    
    @JvmStatic
    fun getPicturesDir(): String {
        return Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES)?.absolutePath ?: ""
    }
    
    // عمليات القراءة
    @JvmStatic
    fun readText(path: String): String? {
        return try {
            File(path).readText(Charsets.UTF_8)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun readLines(path: String): List<String> {
        return try {
            File(path).readLines(Charsets.UTF_8)
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    @JvmStatic
    fun readBytes(path: String): ByteArray? {
        return try {
            File(path).readBytes()
        } catch (e: Exception) {
            null
        }
    }
    
    // عمليات الكتابة
    @JvmStatic
    fun writeText(path: String, content: String): Boolean {
        return try {
            File(path).writeText(content, Charsets.UTF_8)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun appendText(path: String, content: String): Boolean {
        return try {
            File(path).appendText(content, Charsets.UTF_8)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun writeBytes(path: String, bytes: ByteArray): Boolean {
        return try {
            File(path).writeBytes(bytes)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // عمليات الملفات
    @JvmStatic
    fun exists(path: String): Boolean = File(path).exists()
    
    @JvmStatic
    fun isFile(path: String): Boolean = File(path).isFile
    
    @JvmStatic
    fun isDirectory(path: String): Boolean = File(path).isDirectory
    
    @JvmStatic
    fun delete(path: String): Boolean = File(path).delete()
    
    @JvmStatic
    fun deleteRecursively(path: String): Boolean = File(path).deleteRecursively()
    
    @JvmStatic
    fun rename(oldPath: String, newPath: String): Boolean {
        return File(oldPath).renameTo(File(newPath))
    }
    
    @JvmStatic
    fun copy(sourcePath: String, destPath: String): Boolean {
        return try {
            File(sourcePath).copyTo(File(destPath), overwrite = true)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun move(sourcePath: String, destPath: String): Boolean {
        return copy(sourcePath, destPath) && delete(sourcePath)
    }
    
    @JvmStatic
    fun createFile(path: String): Boolean {
        return try {
            File(path).createNewFile()
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun createDirectory(path: String): Boolean {
        return File(path).mkdirs()
    }
    
    // معلومات الملف
    @JvmStatic
    fun getSize(path: String): Long = File(path).length()
    
    @JvmStatic
    fun getSizeFormatted(path: String): String {
        val size = getSize(path)
        return when {
            size < 1024 -> "$size B"
            size < 1024 * 1024 -> "${size / 1024} KB"
            size < 1024 * 1024 * 1024 -> "${size / (1024 * 1024)} MB"
            else -> "%.2f GB".format(size / (1024.0 * 1024 * 1024))
        }
    }
    
    @JvmStatic
    fun getLastModified(path: String): Long = File(path).lastModified()
    
    @JvmStatic
    fun getName(path: String): String = File(path).name
    
    @JvmStatic
    fun getExtension(path: String): String = File(path).extension
    
    @JvmStatic
    fun getParent(path: String): String = File(path).parent ?: ""
    
    @JvmStatic
    fun getAbsolutePath(path: String): String = File(path).absolutePath
    
    // قائمة المحتويات
    @JvmStatic
    fun list(path: String): List<String> {
        return File(path).list()?.toList() ?: emptyList()
    }
    
    @JvmStatic
    fun listFiles(path: String): List<Map<String, Any>> {
        return File(path).listFiles()?.map { file ->
            mapOf(
                "name" to file.name,
                "path" to file.absolutePath,
                "isFile" to file.isFile,
                "isDirectory" to file.isDirectory,
                "size" to file.length(),
                "lastModified" to file.lastModified()
            )
        } ?: emptyList()
    }
    
    @JvmStatic
    fun listFilesRecursively(path: String, extension: String? = null): List<String> {
        val files = mutableListOf<String>()
        
        File(path).walkTopDown().forEach { file ->
            if (file.isFile) {
                if (extension == null || file.extension == extension) {
                    files.add(file.absolutePath)
                }
            }
        }
        
        return files
    }
    
    // ملفات داخلية
    @JvmStatic
    fun writeInternal(fileName: String, content: String): Boolean {
        val ctx = context ?: return false
        return try {
            ctx.openFileOutput(fileName, Context.MODE_PRIVATE).use { fos ->
                fos.write(content.toByteArray(Charsets.UTF_8))
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun readInternal(fileName: String): String? {
        val ctx = context ?: return null
        return try {
            ctx.openFileInput(fileName).bufferedReader().use { it.readText() }
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun deleteInternal(fileName: String): Boolean {
        val ctx = context ?: return false
        return ctx.deleteFile(fileName)
    }
    
    @JvmStatic
    fun listInternal(): List<String> {
        return context?.fileList()?.toList() ?: emptyList()
    }
}
