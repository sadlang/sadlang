// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFirebaseStorageBridge.kt
// الوصف: جسر Kotlin لتخزين Firebase
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.net.Uri
import com.google.firebase.ktx.Firebase
import com.google.firebase.storage.FirebaseStorage
import com.google.firebase.storage.StorageReference
import com.google.firebase.storage.ktx.storage
import java.io.File
import java.io.InputStream

/**
 * جسر Firebase Storage — SadFirebaseStorageBridge
 */
object SadFirebaseStorageBridge {
    
    private var context: Context? = null
    private val storage: FirebaseStorage by lazy { Firebase.storage }
    private val activeUploads = mutableMapOf<String, com.google.firebase.storage.UploadTask>()
    private val activeDownloads = mutableMapOf<String, com.google.firebase.storage.FileDownloadTask>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Upload Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun uploadFile(
        taskId: String,
        localPath: String,
        remotePath: String,
        onProgress: (Int) -> Unit,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        val file = File(localPath)
        if (!file.exists()) {
            onError("الملف غير موجود: $localPath")
            return
        }
        
        val uri = Uri.fromFile(file)
        val ref = storage.reference.child(remotePath)
        
        val uploadTask = ref.putFile(uri)
        activeUploads[taskId] = uploadTask
        
        uploadTask
            .addOnProgressListener { snapshot ->
                val progress = (100.0 * snapshot.bytesTransferred / snapshot.totalByteCount).toInt()
                onProgress(progress)
            }
            .addOnSuccessListener {
                activeUploads.remove(taskId)
                ref.downloadUrl.addOnSuccessListener { downloadUri ->
                    onSuccess(downloadUri.toString())
                }.addOnFailureListener { e ->
                    onError(e.localizedMessage ?: "فشل في جلب رابط التحميل")
                }
            }
            .addOnFailureListener { e ->
                activeUploads.remove(taskId)
                onError(e.localizedMessage ?: "فشل في رفع الملف")
            }
    }
    
    @JvmStatic
    fun uploadFromUri(
        taskId: String,
        uriString: String,
        remotePath: String,
        onProgress: (Int) -> Unit,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        val uri = Uri.parse(uriString)
        val ref = storage.reference.child(remotePath)
        
        val uploadTask = ref.putFile(uri)
        activeUploads[taskId] = uploadTask
        
        uploadTask
            .addOnProgressListener { snapshot ->
                val progress = (100.0 * snapshot.bytesTransferred / snapshot.totalByteCount).toInt()
                onProgress(progress)
            }
            .addOnSuccessListener {
                activeUploads.remove(taskId)
                ref.downloadUrl.addOnSuccessListener { downloadUri ->
                    onSuccess(downloadUri.toString())
                }.addOnFailureListener { e ->
                    onError(e.localizedMessage ?: "فشل في جلب رابط التحميل")
                }
            }
            .addOnFailureListener { e ->
                activeUploads.remove(taskId)
                onError(e.localizedMessage ?: "فشل في رفع الملف")
            }
    }
    
    @JvmStatic
    fun uploadBytes(
        taskId: String,
        bytes: ByteArray,
        remotePath: String,
        onProgress: (Int) -> Unit,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        val ref = storage.reference.child(remotePath)
        
        val uploadTask = ref.putBytes(bytes)
        activeUploads[taskId] = uploadTask
        
        uploadTask
            .addOnProgressListener { snapshot ->
                val progress = (100.0 * snapshot.bytesTransferred / snapshot.totalByteCount).toInt()
                onProgress(progress)
            }
            .addOnSuccessListener {
                activeUploads.remove(taskId)
                ref.downloadUrl.addOnSuccessListener { downloadUri ->
                    onSuccess(downloadUri.toString())
                }.addOnFailureListener { e ->
                    onError(e.localizedMessage ?: "فشل في جلب رابط التحميل")
                }
            }
            .addOnFailureListener { e ->
                activeUploads.remove(taskId)
                onError(e.localizedMessage ?: "فشل في رفع البيانات")
            }
    }
    
    @JvmStatic
    fun uploadStream(
        taskId: String,
        stream: InputStream,
        remotePath: String,
        onProgress: (Int) -> Unit,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        val ref = storage.reference.child(remotePath)
        
        val uploadTask = ref.putStream(stream)
        activeUploads[taskId] = uploadTask
        
        uploadTask
            .addOnProgressListener { snapshot ->
                val progress = (100.0 * snapshot.bytesTransferred / snapshot.totalByteCount).toInt()
                onProgress(progress)
            }
            .addOnSuccessListener {
                activeUploads.remove(taskId)
                ref.downloadUrl.addOnSuccessListener { downloadUri ->
                    onSuccess(downloadUri.toString())
                }.addOnFailureListener { e ->
                    onError(e.localizedMessage ?: "فشل في جلب رابط التحميل")
                }
            }
            .addOnFailureListener { e ->
                activeUploads.remove(taskId)
                onError(e.localizedMessage ?: "فشل في رفع التدفق")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Download Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun downloadFile(
        taskId: String,
        remotePath: String,
        localPath: String,
        onProgress: (Int) -> Unit,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val ref = storage.reference.child(remotePath)
        val localFile = File(localPath)
        
        localFile.parentFile?.mkdirs()
        
        val downloadTask = ref.getFile(localFile)
        activeDownloads[taskId] = downloadTask
        
        downloadTask
            .addOnProgressListener { snapshot ->
                val progress = (100.0 * snapshot.bytesTransferred / snapshot.totalByteCount).toInt()
                onProgress(progress)
            }
            .addOnSuccessListener {
                activeDownloads.remove(taskId)
                onSuccess()
            }
            .addOnFailureListener { e ->
                activeDownloads.remove(taskId)
                onError(e.localizedMessage ?: "فشل في تحميل الملف")
            }
    }
    
    @JvmStatic
    fun downloadBytes(
        remotePath: String,
        maxSize: Long = 10 * 1024 * 1024, // 10 MB
        onSuccess: (ByteArray) -> Unit,
        onError: (String) -> Unit
    ) {
        val ref = storage.reference.child(remotePath)
        
        ref.getBytes(maxSize)
            .addOnSuccessListener { bytes ->
                onSuccess(bytes)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تحميل البيانات")
            }
    }
    
    @JvmStatic
    fun getDownloadUrl(
        remotePath: String,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        storage.reference.child(remotePath).downloadUrl
            .addOnSuccessListener { uri ->
                onSuccess(uri.toString())
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في جلب الرابط")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // File Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun deleteFile(
        remotePath: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        storage.reference.child(remotePath).delete()
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في حذف الملف")
            }
    }
    
    @JvmStatic
    fun getMetadata(
        remotePath: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        storage.reference.child(remotePath).metadata
            .addOnSuccessListener { metadata ->
                onSuccess(mapOf(
                    "name" to metadata.name,
                    "path" to metadata.path,
                    "bucket" to metadata.bucket,
                    "contentType" to metadata.contentType,
                    "size" to metadata.sizeBytes,
                    "createdAt" to metadata.creationTimeMillis,
                    "updatedAt" to metadata.updatedTimeMillis,
                    "md5Hash" to metadata.md5Hash,
                    "cacheControl" to metadata.cacheControl,
                    "contentDisposition" to metadata.contentDisposition,
                    "contentEncoding" to metadata.contentEncoding,
                    "contentLanguage" to metadata.contentLanguage,
                    "customMetadata" to metadata.customMetadataKeys.associateWith { 
                        metadata.getCustomMetadata(it) 
                    }
                ))
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في جلب البيانات الوصفية")
            }
    }
    
    @JvmStatic
    fun updateMetadata(
        remotePath: String,
        contentType: String? = null,
        cacheControl: String? = null,
        customMetadata: Map<String, String>? = null,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val metadata = com.google.firebase.storage.StorageMetadata.Builder().apply {
            contentType?.let { setContentType(it) }
            cacheControl?.let { setCacheControl(it) }
            customMetadata?.forEach { (key, value) ->
                setCustomMetadata(key, value)
            }
        }.build()
        
        storage.reference.child(remotePath).updateMetadata(metadata)
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تحديث البيانات الوصفية")
            }
    }
    
    @JvmStatic
    fun listFiles(
        remotePath: String,
        maxResults: Int = 100,
        onSuccess: (List<Map<String, String>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val ref = if (remotePath.isEmpty()) storage.reference else storage.reference.child(remotePath)
        
        ref.listAll()
            .addOnSuccessListener { result ->
                val items = result.items.map { item ->
                    mapOf(
                        "name" to item.name,
                        "path" to item.path,
                        "bucket" to item.bucket
                    )
                }
                onSuccess(items)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في سرد الملفات")
            }
    }
    
    @JvmStatic
    fun listFolders(
        remotePath: String,
        onSuccess: (List<String>) -> Unit,
        onError: (String) -> Unit
    ) {
        val ref = if (remotePath.isEmpty()) storage.reference else storage.reference.child(remotePath)
        
        ref.listAll()
            .addOnSuccessListener { result ->
                val folders = result.prefixes.map { it.path }
                onSuccess(folders)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في سرد المجلدات")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Task Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun pauseUpload(taskId: String): Boolean {
        return activeUploads[taskId]?.pause() == true
    }
    
    @JvmStatic
    fun resumeUpload(taskId: String): Boolean {
        return activeUploads[taskId]?.resume() == true
    }
    
    @JvmStatic
    fun cancelUpload(taskId: String): Boolean {
        val task = activeUploads.remove(taskId)
        return task?.cancel() == true
    }
    
    @JvmStatic
    fun pauseDownload(taskId: String): Boolean {
        return activeDownloads[taskId]?.pause() == true
    }
    
    @JvmStatic
    fun resumeDownload(taskId: String): Boolean {
        return activeDownloads[taskId]?.resume() == true
    }
    
    @JvmStatic
    fun cancelDownload(taskId: String): Boolean {
        val task = activeDownloads.remove(taskId)
        return task?.cancel() == true
    }
    
    @JvmStatic
    fun getActiveUploads(): List<String> = activeUploads.keys.toList()
    
    @JvmStatic
    fun getActiveDownloads(): List<String> = activeDownloads.keys.toList()
    
    @JvmStatic
    fun cancelAllUploads() {
        activeUploads.values.forEach { it.cancel() }
        activeUploads.clear()
    }
    
    @JvmStatic
    fun cancelAllDownloads() {
        activeDownloads.values.forEach { it.cancel() }
        activeDownloads.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getReference(path: String): StorageReference {
        return storage.reference.child(path)
    }
    
    @JvmStatic
    fun getReferenceFromUrl(url: String): StorageReference {
        return storage.getReferenceFromUrl(url)
    }
    
    @JvmStatic
    fun setMaxUploadRetryTime(timeoutMs: Long) {
        storage.maxUploadRetryTimeMillis = timeoutMs
    }
    
    @JvmStatic
    fun setMaxDownloadRetryTime(timeoutMs: Long) {
        storage.maxDownloadRetryTimeMillis = timeoutMs
    }
    
    @JvmStatic
    fun setMaxOperationRetryTime(timeoutMs: Long) {
        storage.maxOperationRetryTimeMillis = timeoutMs
    }
    
    @JvmStatic
    fun formatFileSize(bytes: Long): String {
        return when {
            bytes < 1024 -> "$bytes بايت"
            bytes < 1024 * 1024 -> String.format("%.2f كيلوبايت", bytes / 1024.0)
            bytes < 1024 * 1024 * 1024 -> String.format("%.2f ميجابايت", bytes / (1024.0 * 1024))
            else -> String.format("%.2f جيجابايت", bytes / (1024.0 * 1024 * 1024))
        }
    }
}
