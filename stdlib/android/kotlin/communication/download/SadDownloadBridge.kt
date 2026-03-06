package sad.android.communication.download

import android.app.DownloadManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.database.Cursor
import android.net.Uri
import android.os.Build
import android.os.Environment
import java.io.File
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر التحميل - إدارة التحميلات من الإنترنت
 * Download Bridge - Manage internet downloads
 * 
 * @author فريق لغة ص
 */
object SadDownloadBridge {
    
    private var downloadManager: DownloadManager? = null
    private val downloadCallbacks = ConcurrentHashMap<Long, DownloadCallback>()
    private var receiver: BroadcastReceiver? = null
    
    /**
     * حالة التحميل / Download status
     */
    enum class DownloadStatus {
        PENDING,
        RUNNING,
        PAUSED,
        SUCCESSFUL,
        FAILED,
        UNKNOWN
    }
    
    /**
     * معلومات التحميل / Download info
     */
    data class DownloadInfo(
        val id: Long,
        val status: DownloadStatus,
        val bytesDownloaded: Long,
        val totalBytes: Long,
        val progress: Int,
        val title: String?,
        val description: String?,
        val localUri: String?,
        val reason: Int?
    )
    
    /**
     * طلب تحميل / Download request
     */
    data class DownloadRequest(
        val url: String,
        val fileName: String,
        val title: String? = null,
        val description: String? = null,
        val destination: DownloadDestination = DownloadDestination.DOWNLOADS,
        val mimeType: String? = null,
        val headers: Map<String, String> = emptyMap(),
        val allowOverMetered: Boolean = true,
        val allowOverRoaming: Boolean = false,
        val requiresCharging: Boolean = false,
        val showNotification: Boolean = true
    )
    
    /**
     * وجهة التحميل / Download destination
     */
    enum class DownloadDestination {
        DOWNLOADS,
        DOCUMENTS,
        PICTURES,
        MUSIC,
        MOVIES,
        CUSTOM
    }
    
    /**
     * مستمع التحميل / Download callback
     */
    interface DownloadCallback {
        fun onProgress(downloadId: Long, progress: Int, bytesDownloaded: Long, totalBytes: Long)
        fun onCompleted(downloadId: Long, localUri: String?)
        fun onFailed(downloadId: Long, reason: String)
        fun onPaused(downloadId: Long)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة الجسر / Initialize bridge
     */
    fun initialize(context: Context) {
        downloadManager = context.getSystemService(Context.DOWNLOAD_SERVICE) as DownloadManager
        registerReceiver(context)
    }
    
    private fun registerReceiver(context: Context) {
        if (receiver != null) return
        
        receiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context?, intent: Intent?) {
                val downloadId = intent?.getLongExtra(DownloadManager.EXTRA_DOWNLOAD_ID, -1) ?: return
                if (downloadId == -1L) return
                
                val callback = downloadCallbacks[downloadId] ?: return
                val info = getDownloadInfo(downloadId)
                
                when (info?.status) {
                    DownloadStatus.SUCCESSFUL -> {
                        callback.onCompleted(downloadId, info.localUri)
                        downloadCallbacks.remove(downloadId)
                    }
                    DownloadStatus.FAILED -> {
                        callback.onFailed(downloadId, getFailureReason(info.reason))
                        downloadCallbacks.remove(downloadId)
                    }
                    DownloadStatus.PAUSED -> callback.onPaused(downloadId)
                    else -> {}
                }
            }
        }
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            context.registerReceiver(
                receiver,
                IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE),
                Context.RECEIVER_NOT_EXPORTED
            )
        } else {
            context.registerReceiver(
                receiver,
                IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE)
            )
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بدء التحميل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء تحميل ملف / Start file download
     */
    fun download(request: DownloadRequest, callback: DownloadCallback? = null): Long {
        val dm = downloadManager ?: return -1
        
        val downloadRequest = DownloadManager.Request(Uri.parse(request.url)).apply {
            setTitle(request.title ?: request.fileName)
            setDescription(request.description)
            
            // إضافة الترويسات
            request.headers.forEach { (key, value) ->
                addRequestHeader(key, value)
            }
            
            // تعيين نوع MIME
            request.mimeType?.let { setMimeType(it) }
            
            // تعيين الوجهة
            val dir = when (request.destination) {
                DownloadDestination.DOWNLOADS -> Environment.DIRECTORY_DOWNLOADS
                DownloadDestination.DOCUMENTS -> Environment.DIRECTORY_DOCUMENTS
                DownloadDestination.PICTURES -> Environment.DIRECTORY_PICTURES
                DownloadDestination.MUSIC -> Environment.DIRECTORY_MUSIC
                DownloadDestination.MOVIES -> Environment.DIRECTORY_MOVIES
                DownloadDestination.CUSTOM -> Environment.DIRECTORY_DOWNLOADS
            }
            setDestinationInExternalPublicDir(dir, request.fileName)
            
            // إعدادات الشبكة
            val networkTypes = if (request.allowOverMetered) {
                DownloadManager.Request.NETWORK_WIFI or DownloadManager.Request.NETWORK_MOBILE
            } else {
                DownloadManager.Request.NETWORK_WIFI
            }
            setAllowedNetworkTypes(networkTypes)
            setAllowedOverRoaming(request.allowOverRoaming)
            
            // إظهار الإشعار
            if (request.showNotification) {
                setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED)
            } else {
                setNotificationVisibility(DownloadManager.Request.VISIBILITY_HIDDEN)
            }
        }
        
        val downloadId = dm.enqueue(downloadRequest)
        callback?.let { downloadCallbacks[downloadId] = it }
        
        return downloadId
    }
    
    /**
     * تحميل سريع / Quick download
     */
    fun downloadFile(url: String, fileName: String, callback: DownloadCallback? = null): Long {
        return download(DownloadRequest(url = url, fileName = fileName), callback)
    }
    
    /**
     * تحميل مع ترويسات / Download with headers
     */
    fun downloadWithAuth(url: String, fileName: String, authToken: String, callback: DownloadCallback? = null): Long {
        return download(
            DownloadRequest(
                url = url,
                fileName = fileName,
                headers = mapOf("Authorization" to "Bearer $authToken")
            ),
            callback
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة التحميلات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء تحميل / Cancel download
     */
    fun cancel(downloadId: Long): Boolean {
        val removed = downloadManager?.remove(downloadId) ?: 0
        downloadCallbacks.remove(downloadId)
        return removed > 0
    }
    
    /**
     * إلغاء تحميلات متعددة / Cancel multiple downloads
     */
    fun cancelAll(downloadIds: List<Long>): Int {
        var count = 0
        downloadIds.forEach { id ->
            if (cancel(id)) count++
        }
        return count
    }
    
    /**
     * الحصول على معلومات التحميل / Get download info
     */
    fun getDownloadInfo(downloadId: Long): DownloadInfo? {
        val dm = downloadManager ?: return null
        
        val query = DownloadManager.Query().setFilterById(downloadId)
        val cursor = dm.query(query) ?: return null
        
        return cursor.use {
            if (it.moveToFirst()) {
                parseDownloadInfo(it)
            } else null
        }
    }
    
    /**
     * جميع التحميلات / Get all downloads
     */
    fun getAllDownloads(): List<DownloadInfo> {
        val dm = downloadManager ?: return emptyList()
        val cursor = dm.query(DownloadManager.Query()) ?: return emptyList()
        
        return cursor.use {
            val downloads = mutableListOf<DownloadInfo>()
            while (it.moveToNext()) {
                parseDownloadInfo(it)?.let { info -> downloads.add(info) }
            }
            downloads
        }
    }
    
    /**
     * التحميلات النشطة / Get active downloads
     */
    fun getActiveDownloads(): List<DownloadInfo> {
        return getAllDownloads().filter { 
            it.status == DownloadStatus.RUNNING || it.status == DownloadStatus.PENDING 
        }
    }
    
    /**
     * التحميلات المكتملة / Get completed downloads
     */
    fun getCompletedDownloads(): List<DownloadInfo> {
        return getAllDownloads().filter { it.status == DownloadStatus.SUCCESSFUL }
    }
    
    /**
     * التحميلات الفاشلة / Get failed downloads
     */
    fun getFailedDownloads(): List<DownloadInfo> {
        return getAllDownloads().filter { it.status == DownloadStatus.FAILED }
    }
    
    /**
     * فتح الملف المحمل / Open downloaded file
     */
    fun openDownloadedFile(context: Context, downloadId: Long): Boolean {
        return try {
            val uri = downloadManager?.getUriForDownloadedFile(downloadId) ?: return false
            val mimeType = downloadManager?.getMimeTypeForDownloadedFile(downloadId) ?: "*/*"
            
            val intent = Intent(Intent.ACTION_VIEW).apply {
                setDataAndType(uri, mimeType)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * فتح قائمة التحميلات / Open downloads list
     */
    fun openDownloadsApp(context: Context): Boolean {
        return try {
            val intent = Intent(DownloadManager.ACTION_VIEW_DOWNLOADS)
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التقدم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تتبع تقدم التحميل / Track download progress
     */
    fun trackProgress(downloadId: Long, callback: DownloadCallback, intervalMs: Long = 500) {
        downloadCallbacks[downloadId] = callback
        
        Thread {
            while (downloadCallbacks.containsKey(downloadId)) {
                val info = getDownloadInfo(downloadId) ?: break
                
                when (info.status) {
                    DownloadStatus.RUNNING -> {
                        callback.onProgress(downloadId, info.progress, info.bytesDownloaded, info.totalBytes)
                    }
                    DownloadStatus.SUCCESSFUL -> {
                        callback.onCompleted(downloadId, info.localUri)
                        downloadCallbacks.remove(downloadId)
                        break
                    }
                    DownloadStatus.FAILED -> {
                        callback.onFailed(downloadId, getFailureReason(info.reason))
                        downloadCallbacks.remove(downloadId)
                        break
                    }
                    DownloadStatus.PAUSED -> {
                        callback.onPaused(downloadId)
                    }
                    else -> {}
                }
                
                Thread.sleep(intervalMs)
            }
        }.start()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنظيف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء تسجيل المستقبل / Unregister receiver
     */
    fun release(context: Context) {
        receiver?.let {
            try {
                context.unregisterReceiver(it)
            } catch (e: Exception) {}
        }
        receiver = null
        downloadCallbacks.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun parseDownloadInfo(cursor: Cursor): DownloadInfo? {
        return try {
            val idIndex = cursor.getColumnIndex(DownloadManager.COLUMN_ID)
            val statusIndex = cursor.getColumnIndex(DownloadManager.COLUMN_STATUS)
            val bytesIndex = cursor.getColumnIndex(DownloadManager.COLUMN_BYTES_DOWNLOADED_SO_FAR)
            val totalIndex = cursor.getColumnIndex(DownloadManager.COLUMN_TOTAL_SIZE_BYTES)
            val titleIndex = cursor.getColumnIndex(DownloadManager.COLUMN_TITLE)
            val descIndex = cursor.getColumnIndex(DownloadManager.COLUMN_DESCRIPTION)
            val localIndex = cursor.getColumnIndex(DownloadManager.COLUMN_LOCAL_URI)
            val reasonIndex = cursor.getColumnIndex(DownloadManager.COLUMN_REASON)
            
            val bytesDownloaded = cursor.getLong(bytesIndex)
            val totalBytes = cursor.getLong(totalIndex)
            val progress = if (totalBytes > 0) ((bytesDownloaded * 100) / totalBytes).toInt() else 0
            
            DownloadInfo(
                id = cursor.getLong(idIndex),
                status = parseStatus(cursor.getInt(statusIndex)),
                bytesDownloaded = bytesDownloaded,
                totalBytes = totalBytes,
                progress = progress,
                title = cursor.getString(titleIndex),
                description = cursor.getString(descIndex),
                localUri = cursor.getString(localIndex),
                reason = cursor.getInt(reasonIndex)
            )
        } catch (e: Exception) {
            null
        }
    }
    
    private fun parseStatus(status: Int): DownloadStatus {
        return when (status) {
            DownloadManager.STATUS_PENDING -> DownloadStatus.PENDING
            DownloadManager.STATUS_RUNNING -> DownloadStatus.RUNNING
            DownloadManager.STATUS_PAUSED -> DownloadStatus.PAUSED
            DownloadManager.STATUS_SUCCESSFUL -> DownloadStatus.SUCCESSFUL
            DownloadManager.STATUS_FAILED -> DownloadStatus.FAILED
            else -> DownloadStatus.UNKNOWN
        }
    }
    
    private fun getFailureReason(reason: Int?): String {
        return when (reason) {
            DownloadManager.ERROR_CANNOT_RESUME -> "لا يمكن الاستئناف"
            DownloadManager.ERROR_DEVICE_NOT_FOUND -> "الجهاز غير موجود"
            DownloadManager.ERROR_FILE_ALREADY_EXISTS -> "الملف موجود"
            DownloadManager.ERROR_FILE_ERROR -> "خطأ في الملف"
            DownloadManager.ERROR_HTTP_DATA_ERROR -> "خطأ بيانات HTTP"
            DownloadManager.ERROR_INSUFFICIENT_SPACE -> "مساحة غير كافية"
            DownloadManager.ERROR_TOO_MANY_REDIRECTS -> "إعادات توجيه كثيرة"
            DownloadManager.ERROR_UNHANDLED_HTTP_CODE -> "كود HTTP غير معالج"
            DownloadManager.ERROR_UNKNOWN -> "خطأ غير معروف"
            DownloadManager.PAUSED_QUEUED_FOR_WIFI -> "في الانتظار (واي فاي)"
            DownloadManager.PAUSED_UNKNOWN -> "موقوف مؤقتاً"
            DownloadManager.PAUSED_WAITING_FOR_NETWORK -> "في انتظار الشبكة"
            DownloadManager.PAUSED_WAITING_TO_RETRY -> "في انتظار إعادة المحاولة"
            else -> "سبب غير معروف"
        }
    }
}
