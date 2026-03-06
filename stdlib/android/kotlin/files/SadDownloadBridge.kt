// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDownloadBridge.kt
// الوصف: جسر Kotlin لتحميل الملفات
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

/**
 * جسر التحميل — SadDownloadBridge
 */
object SadDownloadBridge {
    
    private var context: Context? = null
    private var downloadManager: DownloadManager? = null
    private val downloadCallbacks = mutableMapOf<Long, (Boolean, String?) -> Unit>()
    private var receiver: BroadcastReceiver? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        downloadManager = ctx.getSystemService(Context.DOWNLOAD_SERVICE) as? DownloadManager
        registerReceiver()
    }
    
    private fun registerReceiver() {
        val ctx = context ?: return
        
        receiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context, intent: Intent) {
                val downloadId = intent.getLongExtra(DownloadManager.EXTRA_DOWNLOAD_ID, -1)
                if (downloadId != -1L) {
                    val callback = downloadCallbacks.remove(downloadId)
                    val status = getStatus(downloadId)
                    if (status == "مكتمل") {
                        val uri = getDownloadedFile(downloadId)
                        callback?.invoke(true, uri)
                    } else {
                        callback?.invoke(false, status)
                    }
                }
            }
        }
        
        ctx.registerReceiver(receiver, IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE))
    }
    
    @JvmStatic
    fun download(
        url: String,
        fileName: String,
        title: String = fileName,
        description: String = "",
        destinationDir: Int = DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED,
        onComplete: ((Boolean, String?) -> Unit)? = null
    ): Long {
        val dm = downloadManager ?: return -1
        
        try {
            val request = DownloadManager.Request(Uri.parse(url))
                .setTitle(title)
                .setDescription(description)
                .setNotificationVisibility(destinationDir)
                .setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, fileName)
                .setAllowedOverMetered(true)
                .setAllowedOverRoaming(true)
            
            val downloadId = dm.enqueue(request)
            
            if (onComplete != null) {
                downloadCallbacks[downloadId] = onComplete
            }
            
            return downloadId
        } catch (e: Exception) {
            onComplete?.invoke(false, e.message)
            return -1
        }
    }
    
    @JvmStatic
    fun downloadToInternal(
        url: String,
        fileName: String,
        title: String = fileName,
        onComplete: ((Boolean, String?) -> Unit)? = null
    ): Long {
        val dm = downloadManager ?: return -1
        val ctx = context ?: return -1
        
        try {
            val file = java.io.File(ctx.filesDir, fileName)
            
            val request = DownloadManager.Request(Uri.parse(url))
                .setTitle(title)
                .setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE)
                .setDestinationUri(Uri.fromFile(file))
            
            val downloadId = dm.enqueue(request)
            
            if (onComplete != null) {
                downloadCallbacks[downloadId] = onComplete
            }
            
            return downloadId
        } catch (e: Exception) {
            onComplete?.invoke(false, e.message)
            return -1
        }
    }
    
    @JvmStatic
    fun cancel(downloadId: Long): Int {
        return downloadManager?.remove(downloadId) ?: 0
    }
    
    @JvmStatic
    fun getStatus(downloadId: Long): String {
        val dm = downloadManager ?: return "غير معروف"
        
        val query = DownloadManager.Query().setFilterById(downloadId)
        val cursor: Cursor = dm.query(query) ?: return "غير معروف"
        
        return cursor.use {
            if (it.moveToFirst()) {
                val statusIndex = it.getColumnIndex(DownloadManager.COLUMN_STATUS)
                when (it.getInt(statusIndex)) {
                    DownloadManager.STATUS_PENDING -> "قيد الانتظار"
                    DownloadManager.STATUS_RUNNING -> "جاري التحميل"
                    DownloadManager.STATUS_PAUSED -> "متوقف مؤقتاً"
                    DownloadManager.STATUS_SUCCESSFUL -> "مكتمل"
                    DownloadManager.STATUS_FAILED -> "فشل"
                    else -> "غير معروف"
                }
            } else {
                "غير موجود"
            }
        }
    }
    
    @JvmStatic
    fun getProgress(downloadId: Long): Map<String, Long> {
        val dm = downloadManager ?: return emptyMap()
        
        val query = DownloadManager.Query().setFilterById(downloadId)
        val cursor = dm.query(query) ?: return emptyMap()
        
        return cursor.use {
            if (it.moveToFirst()) {
                val downloadedIndex = it.getColumnIndex(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR)
                val totalIndex = it.getColumnIndex(DownloadManager.COLUMN_TOTAL_SIZE_BYTES)
                
                val downloaded = it.getLong(downloadedIndex)
                val total = it.getLong(totalIndex)
                
                mapOf(
                    "downloaded" to downloaded,
                    "total" to total,
                    "percent" to if (total > 0) (downloaded * 100 / total) else 0L
                )
            } else {
                emptyMap()
            }
        }
    }
    
    @JvmStatic
    fun getDownloadedFile(downloadId: Long): String? {
        val dm = downloadManager ?: return null
        return dm.getUriForDownloadedFile(downloadId)?.toString()
    }
    
    @JvmStatic
    fun getDownloadInfo(downloadId: Long): Map<String, Any?> {
        val dm = downloadManager ?: return emptyMap()
        
        val query = DownloadManager.Query().setFilterById(downloadId)
        val cursor = dm.query(query) ?: return emptyMap()
        
        return cursor.use {
            if (it.moveToFirst()) {
                mapOf(
                    "id" to downloadId,
                    "status" to getStatus(downloadId),
                    "progress" to getProgress(downloadId),
                    "uri" to it.getString(it.getColumnIndex(DownloadManager.COLUMN_URI)),
                    "localUri" to it.getString(it.getColumnIndex(DownloadManager.COLUMN_LOCAL_URI)),
                    "title" to it.getString(it.getColumnIndex(DownloadManager.COLUMN_TITLE)),
                    "description" to it.getString(it.getColumnIndex(DownloadManager.COLUMN_DESCRIPTION)),
                    "mimeType" to it.getString(it.getColumnIndex(DownloadManager.COLUMN_MEDIA_TYPE)),
                    "reason" to it.getInt(it.getColumnIndex(DownloadManager.COLUMN_REASON))
                )
            } else {
                emptyMap()
            }
        }
    }
    
    @JvmStatic
    fun cleanup() {
        receiver?.let {
            try {
                context?.unregisterReceiver(it)
            } catch (e: Exception) { /* تجاهل */ }
        }
        downloadCallbacks.clear()
    }
}
