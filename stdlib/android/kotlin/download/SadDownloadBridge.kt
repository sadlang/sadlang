// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDownloadBridge.kt
// الوصف: جسر Kotlin لتحميل الملفات DownloadManager
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.DownloadManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.database.Cursor
import android.net.Uri
import android.os.Environment
import java.io.File

/**
 * جسر التحميل — SadDownloadBridge
 */
object SadDownloadBridge {
    
    private var context: Context? = null
    private var downloadManager: DownloadManager? = null
    private var downloadReceiver: BroadcastReceiver? = null
    
    private val downloadCallbacks = mutableMapOf<Long, (Long, Boolean, String?) -> Unit>()
    private var onProgressCallback: ((Long, Int, Long, Long) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        downloadManager = ctx.getSystemService(Context.DOWNLOAD_SERVICE) as? DownloadManager
        setupDownloadReceiver()
    }
    
    private fun setupDownloadReceiver() {
        val ctx = context ?: return
        
        downloadReceiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context?, intent: Intent?) {
                val downloadId = intent?.getLongExtra(DownloadManager.EXTRA_DOWNLOAD_ID, -1) ?: return
                
                val callback = downloadCallbacks[downloadId]
                if (callback != null) {
                    val status = getDownloadStatus(downloadId)
                    val success = status["status"] == "ناجح"
                    val error = status["error"] as? String
                    callback(downloadId, success, error)
                    downloadCallbacks.remove(downloadId)
                }
            }
        }
        
        ctx.registerReceiver(downloadReceiver, IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE))
    }
    
    @JvmStatic
    fun cleanup() {
        downloadReceiver?.let {
            try {
                context?.unregisterReceiver(it)
            } catch (e: Exception) {
                // Already unregistered
            }
        }
        downloadReceiver = null
        downloadCallbacks.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Download
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun download(
        url: String,
        title: String,
        description: String = "",
        fileName: String? = null,
        directory: String = Environment.DIRECTORY_DOWNLOADS,
        notificationVisibility: Int = DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED,
        allowedOverMetered: Boolean = true,
        allowedOverRoaming: Boolean = true,
        headers: Map<String, String>? = null,
        callback: ((Long, Boolean, String?) -> Unit)? = null
    ): Long {
        val dm = downloadManager ?: return -1
        
        val actualFileName = fileName ?: url.substringAfterLast("/")
        
        val request = DownloadManager.Request(Uri.parse(url)).apply {
            setTitle(title)
            setDescription(description)
            setNotificationVisibility(notificationVisibility)
            setDestinationInExternalPublicDir(directory, actualFileName)
            setAllowedOverMetered(allowedOverMetered)
            setAllowedOverRoaming(allowedOverRoaming)
            
            headers?.forEach { (key, value) ->
                addRequestHeader(key, value)
            }
        }
        
        val downloadId = dm.enqueue(request)
        
        callback?.let {
            downloadCallbacks[downloadId] = it
        }
        
        return downloadId
    }
    
    @JvmStatic
    fun downloadToPrivateStorage(
        url: String,
        title: String,
        description: String = "",
        fileName: String? = null,
        subDir: String = "downloads",
        callback: ((Long, Boolean, String?) -> Unit)? = null
    ): Long {
        val ctx = context ?: return -1
        val dm = downloadManager ?: return -1
        
        val actualFileName = fileName ?: url.substringAfterLast("/")
        val downloadDir = File(ctx.filesDir, subDir)
        if (!downloadDir.exists()) {
            downloadDir.mkdirs()
        }
        
        val destinationFile = File(downloadDir, actualFileName)
        
        val request = DownloadManager.Request(Uri.parse(url)).apply {
            setTitle(title)
            setDescription(description)
            setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED)
            setDestinationUri(Uri.fromFile(destinationFile))
        }
        
        val downloadId = dm.enqueue(request)
        
        callback?.let {
            downloadCallbacks[downloadId] = it
        }
        
        return downloadId
    }
    
    @JvmStatic
    fun downloadWithMimeType(
        url: String,
        title: String,
        mimeType: String,
        fileName: String? = null,
        directory: String = Environment.DIRECTORY_DOWNLOADS,
        callback: ((Long, Boolean, String?) -> Unit)? = null
    ): Long {
        val dm = downloadManager ?: return -1
        
        val actualFileName = fileName ?: url.substringAfterLast("/")
        
        val request = DownloadManager.Request(Uri.parse(url)).apply {
            setTitle(title)
            setMimeType(mimeType)
            setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED)
            setDestinationInExternalPublicDir(directory, actualFileName)
        }
        
        val downloadId = dm.enqueue(request)
        
        callback?.let {
            downloadCallbacks[downloadId] = it
        }
        
        return downloadId
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cancel & Remove
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun cancelDownload(downloadId: Long): Boolean {
        val dm = downloadManager ?: return false
        
        return try {
            val removed = dm.remove(downloadId)
            downloadCallbacks.remove(downloadId)
            removed > 0
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun cancelDownloads(downloadIds: List<Long>): Int {
        val dm = downloadManager ?: return 0
        
        return try {
            val removed = dm.remove(*downloadIds.toLongArray())
            downloadIds.forEach { downloadCallbacks.remove(it) }
            removed
        } catch (e: Exception) {
            0
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Status
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDownloadStatus(downloadId: Long): Map<String, Any?> {
        val dm = downloadManager ?: return emptyMap()
        
        val query = DownloadManager.Query().setFilterById(downloadId)
        val cursor = dm.query(query)
        
        if (cursor == null || !cursor.moveToFirst()) {
            cursor?.close()
            return mapOf(
                "found" to false,
                "status" to "غير موجود"
            )
        }
        
        return try {
            val status = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_STATUS))
            val bytesDownloaded = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR))
            val bytesTotal = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES))
            val reason = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_REASON))
            val localUri = cursor.getString(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_URI))
            
            val progress = if (bytesTotal > 0) {
                ((bytesDownloaded.toDouble() / bytesTotal) * 100).toInt()
            } else {
                0
            }
            
            mapOf(
                "found" to true,
                "status" to getStatusLabel(status),
                "statusCode" to status,
                "bytesDownloaded" to bytesDownloaded,
                "bytesTotal" to bytesTotal,
                "progress" to progress,
                "localUri" to localUri,
                "error" to getReasonLabel(status, reason)
            )
        } finally {
            cursor.close()
        }
    }
    
    @JvmStatic
    fun getDownloadProgress(downloadId: Long): Int {
        val dm = downloadManager ?: return 0
        
        val query = DownloadManager.Query().setFilterById(downloadId)
        val cursor = dm.query(query)
        
        if (cursor == null || !cursor.moveToFirst()) {
            cursor?.close()
            return 0
        }
        
        return try {
            val bytesDownloaded = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR))
            val bytesTotal = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES))
            
            if (bytesTotal > 0) {
                ((bytesDownloaded.toDouble() / bytesTotal) * 100).toInt()
            } else {
                0
            }
        } finally {
            cursor.close()
        }
    }
    
    @JvmStatic
    fun isDownloadComplete(downloadId: Long): Boolean {
        val status = getDownloadStatus(downloadId)
        return status["statusCode"] == DownloadManager.STATUS_SUCCESSFUL
    }
    
    @JvmStatic
    fun isDownloadRunning(downloadId: Long): Boolean {
        val status = getDownloadStatus(downloadId)
        val statusCode = status["statusCode"] as? Int ?: return false
        return statusCode == DownloadManager.STATUS_RUNNING || 
               statusCode == DownloadManager.STATUS_PENDING
    }
    
    private fun getStatusLabel(status: Int): String {
        return when (status) {
            DownloadManager.STATUS_PENDING -> "في الانتظار"
            DownloadManager.STATUS_RUNNING -> "جاري التحميل"
            DownloadManager.STATUS_PAUSED -> "متوقف مؤقتاً"
            DownloadManager.STATUS_SUCCESSFUL -> "ناجح"
            DownloadManager.STATUS_FAILED -> "فشل"
            else -> "غير معروف"
        }
    }
    
    private fun getReasonLabel(status: Int, reason: Int): String? {
        if (status == DownloadManager.STATUS_SUCCESSFUL) {
            return null
        }
        
        return when (reason) {
            DownloadManager.ERROR_CANNOT_RESUME -> "لا يمكن استئناف التحميل"
            DownloadManager.ERROR_DEVICE_NOT_FOUND -> "الجهاز غير موجود"
            DownloadManager.ERROR_FILE_ALREADY_EXISTS -> "الملف موجود مسبقاً"
            DownloadManager.ERROR_FILE_ERROR -> "خطأ في الملف"
            DownloadManager.ERROR_HTTP_DATA_ERROR -> "خطأ في البيانات"
            DownloadManager.ERROR_INSUFFICIENT_SPACE -> "مساحة غير كافية"
            DownloadManager.ERROR_TOO_MANY_REDIRECTS -> "إعادات توجيه كثيرة"
            DownloadManager.ERROR_UNHANDLED_HTTP_CODE -> "رمز HTTP غير معروف"
            DownloadManager.ERROR_UNKNOWN -> "خطأ غير معروف"
            DownloadManager.PAUSED_QUEUED_FOR_WIFI -> "في انتظار WiFi"
            DownloadManager.PAUSED_WAITING_FOR_NETWORK -> "في انتظار الشبكة"
            DownloadManager.PAUSED_WAITING_TO_RETRY -> "في انتظار إعادة المحاولة"
            DownloadManager.PAUSED_UNKNOWN -> "متوقف (سبب غير معروف)"
            else -> null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Query Downloads
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getAllDownloads(): List<Map<String, Any?>> {
        val dm = downloadManager ?: return emptyList()
        
        val query = DownloadManager.Query()
        val cursor = dm.query(query) ?: return emptyList()
        
        val downloads = mutableListOf<Map<String, Any?>>()
        
        try {
            while (cursor.moveToNext()) {
                val id = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_ID))
                val title = cursor.getString(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TITLE))
                val status = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_STATUS))
                val bytesDownloaded = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR))
                val bytesTotal = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TOTAL_SIZE_BYTES))
                val localUri = cursor.getString(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_LOCAL_URI))
                
                downloads.add(mapOf(
                    "id" to id,
                    "title" to title,
                    "status" to getStatusLabel(status),
                    "bytesDownloaded" to bytesDownloaded,
                    "bytesTotal" to bytesTotal,
                    "localUri" to localUri
                ))
            }
        } finally {
            cursor.close()
        }
        
        return downloads
    }
    
    @JvmStatic
    fun getPendingDownloads(): List<Map<String, Any?>> {
        val dm = downloadManager ?: return emptyList()
        
        val query = DownloadManager.Query()
            .setFilterByStatus(DownloadManager.STATUS_PENDING or DownloadManager.STATUS_RUNNING)
        
        return queryDownloads(dm, query)
    }
    
    @JvmStatic
    fun getCompletedDownloads(): List<Map<String, Any?>> {
        val dm = downloadManager ?: return emptyList()
        
        val query = DownloadManager.Query()
            .setFilterByStatus(DownloadManager.STATUS_SUCCESSFUL)
        
        return queryDownloads(dm, query)
    }
    
    @JvmStatic
    fun getFailedDownloads(): List<Map<String, Any?>> {
        val dm = downloadManager ?: return emptyList()
        
        val query = DownloadManager.Query()
            .setFilterByStatus(DownloadManager.STATUS_FAILED)
        
        return queryDownloads(dm, query)
    }
    
    private fun queryDownloads(dm: DownloadManager, query: DownloadManager.Query): List<Map<String, Any?>> {
        val cursor = dm.query(query) ?: return emptyList()
        val downloads = mutableListOf<Map<String, Any?>>()
        
        try {
            while (cursor.moveToNext()) {
                val id = cursor.getLong(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_ID))
                val title = cursor.getString(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_TITLE))
                val status = cursor.getInt(cursor.getColumnIndexOrThrow(DownloadManager.COLUMN_STATUS))
                
                downloads.add(mapOf(
                    "id" to id,
                    "title" to title,
                    "status" to getStatusLabel(status)
                ))
            }
        } finally {
            cursor.close()
        }
        
        return downloads
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Open Downloaded File
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun openDownloadedFile(downloadId: Long): Boolean {
        val dm = downloadManager ?: return false
        val ctx = context ?: return false
        
        return try {
            val uri = dm.getUriForDownloadedFile(downloadId)
            val mimeType = dm.getMimeTypeForDownloadedFile(downloadId)
            
            val intent = Intent(Intent.ACTION_VIEW).apply {
                setDataAndType(uri, mimeType)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            
            ctx.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getDownloadedFileUri(downloadId: Long): String? {
        val dm = downloadManager ?: return null
        return dm.getUriForDownloadedFile(downloadId)?.toString()
    }
    
    @JvmStatic
    fun getDownloadedFileMimeType(downloadId: Long): String? {
        val dm = downloadManager ?: return null
        return dm.getMimeTypeForDownloadedFile(downloadId)
    }
}
