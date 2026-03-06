package sad.android.communication.upload

import android.content.Context
import android.net.Uri
import android.webkit.MimeTypeMap
import java.io.*
import java.net.HttpURLConnection
import java.net.URL
import java.util.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.Future

/**
 * جسر الرفع - رفع الملفات إلى الخوادم
 * Upload Bridge - Upload files to servers
 * 
 * @author فريق لغة ص
 */
object SadUploadBridge {
    
    private val executor: ExecutorService = Executors.newFixedThreadPool(4)
    private val activeUploads = mutableMapOf<String, Future<*>>()
    
    /**
     * حالة الرفع / Upload status
     */
    enum class UploadStatus {
        PENDING,
        UPLOADING,
        COMPLETED,
        FAILED,
        CANCELLED
    }
    
    /**
     * نتيجة الرفع / Upload result
     */
    data class UploadResult(
        val uploadId: String,
        val success: Boolean,
        val responseCode: Int,
        val responseBody: String?,
        val uploadedBytes: Long,
        val totalBytes: Long,
        val errorMessage: String? = null
    )
    
    /**
     * طلب رفع / Upload request
     */
    data class UploadRequest(
        val url: String,
        val method: String = "POST",
        val headers: Map<String, String> = emptyMap(),
        val params: Map<String, String> = emptyMap(),
        val timeout: Int = 30000,
        val readTimeout: Int = 60000
    )
    
    /**
     * مستمع الرفع / Upload listener
     */
    interface UploadListener {
        fun onProgress(uploadId: String, progress: Int, uploadedBytes: Long, totalBytes: Long)
        fun onCompleted(result: UploadResult)
        fun onFailed(uploadId: String, error: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         رفع ملف واحد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * رفع ملف / Upload file
     */
    fun uploadFile(
        context: Context,
        fileUri: Uri,
        request: UploadRequest,
        fieldName: String = "file",
        listener: UploadListener? = null
    ): String {
        val uploadId = UUID.randomUUID().toString()
        
        val future = executor.submit {
            try {
                val result = performUpload(context, fileUri, request, fieldName, uploadId, listener)
                listener?.onCompleted(result)
            } catch (e: Exception) {
                listener?.onFailed(uploadId, e.message ?: "خطأ غير معروف")
            } finally {
                activeUploads.remove(uploadId)
            }
        }
        
        activeUploads[uploadId] = future
        return uploadId
    }
    
    /**
     * رفع ملف من مسار / Upload file from path
     */
    fun uploadFile(
        filePath: String,
        request: UploadRequest,
        fieldName: String = "file",
        listener: UploadListener? = null
    ): String {
        val uploadId = UUID.randomUUID().toString()
        val file = File(filePath)
        
        val future = executor.submit {
            try {
                val result = performUpload(file, request, fieldName, uploadId, listener)
                listener?.onCompleted(result)
            } catch (e: Exception) {
                listener?.onFailed(uploadId, e.message ?: "خطأ غير معروف")
            } finally {
                activeUploads.remove(uploadId)
            }
        }
        
        activeUploads[uploadId] = future
        return uploadId
    }
    
    /**
     * رفع بايتات / Upload bytes
     */
    fun uploadBytes(
        data: ByteArray,
        fileName: String,
        request: UploadRequest,
        fieldName: String = "file",
        listener: UploadListener? = null
    ): String {
        val uploadId = UUID.randomUUID().toString()
        
        val future = executor.submit {
            try {
                val result = performUploadBytes(data, fileName, request, fieldName, uploadId, listener)
                listener?.onCompleted(result)
            } catch (e: Exception) {
                listener?.onFailed(uploadId, e.message ?: "خطأ غير معروف")
            } finally {
                activeUploads.remove(uploadId)
            }
        }
        
        activeUploads[uploadId] = future
        return uploadId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         رفع ملفات متعددة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * رفع ملفات متعددة / Upload multiple files
     */
    fun uploadFiles(
        filePaths: List<String>,
        request: UploadRequest,
        fieldName: String = "files",
        listener: UploadListener? = null
    ): String {
        val uploadId = UUID.randomUUID().toString()
        
        val future = executor.submit {
            try {
                val result = performMultiUpload(filePaths, request, fieldName, uploadId, listener)
                listener?.onCompleted(result)
            } catch (e: Exception) {
                listener?.onFailed(uploadId, e.message ?: "خطأ غير معروف")
            } finally {
                activeUploads.remove(uploadId)
            }
        }
        
        activeUploads[uploadId] = future
        return uploadId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال بيانات JSON
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال JSON / Send JSON
     */
    fun uploadJson(
        json: String,
        request: UploadRequest,
        listener: UploadListener? = null
    ): String {
        val uploadId = UUID.randomUUID().toString()
        
        val future = executor.submit {
            try {
                val result = performJsonUpload(json, request, uploadId, listener)
                listener?.onCompleted(result)
            } catch (e: Exception) {
                listener?.onFailed(uploadId, e.message ?: "خطأ غير معروف")
            } finally {
                activeUploads.remove(uploadId)
            }
        }
        
        activeUploads[uploadId] = future
        return uploadId
    }
    
    /**
     * إرسال Form Data / Send Form Data
     */
    fun uploadFormData(
        formData: Map<String, String>,
        request: UploadRequest,
        listener: UploadListener? = null
    ): String {
        val uploadId = UUID.randomUUID().toString()
        
        val future = executor.submit {
            try {
                val result = performFormDataUpload(formData, request, uploadId, listener)
                listener?.onCompleted(result)
            } catch (e: Exception) {
                listener?.onFailed(uploadId, e.message ?: "خطأ غير معروف")
            } finally {
                activeUploads.remove(uploadId)
            }
        }
        
        activeUploads[uploadId] = future
        return uploadId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الرفع
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إلغاء رفع / Cancel upload
     */
    fun cancel(uploadId: String): Boolean {
        return activeUploads[uploadId]?.cancel(true) ?: false
    }
    
    /**
     * إلغاء جميع الرفعات / Cancel all uploads
     */
    fun cancelAll() {
        activeUploads.values.forEach { it.cancel(true) }
        activeUploads.clear()
    }
    
    /**
     * هل الرفع نشط؟ / Is upload active?
     */
    fun isActive(uploadId: String): Boolean {
        return activeUploads[uploadId]?.isDone == false
    }
    
    /**
     * عدد الرفعات النشطة / Active uploads count
     */
    fun activeCount(): Int {
        return activeUploads.count { !it.value.isDone }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنفيذ الفعلي
    // ═══════════════════════════════════════════════════════════════
    
    private fun performUpload(
        context: Context,
        fileUri: Uri,
        request: UploadRequest,
        fieldName: String,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        val inputStream = context.contentResolver.openInputStream(fileUri)
            ?: throw IOException("تعذر فتح الملف")
        
        val fileName = getFileName(context, fileUri)
        val mimeType = context.contentResolver.getType(fileUri) ?: "application/octet-stream"
        val data = inputStream.readBytes()
        inputStream.close()
        
        return uploadWithMultipart(data, fileName, mimeType, request, fieldName, uploadId, listener)
    }
    
    private fun performUpload(
        file: File,
        request: UploadRequest,
        fieldName: String,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        if (!file.exists()) throw IOException("الملف غير موجود: ${file.path}")
        
        val mimeType = getMimeType(file.name)
        val data = file.readBytes()
        
        return uploadWithMultipart(data, file.name, mimeType, request, fieldName, uploadId, listener)
    }
    
    private fun performUploadBytes(
        data: ByteArray,
        fileName: String,
        request: UploadRequest,
        fieldName: String,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        val mimeType = getMimeType(fileName)
        return uploadWithMultipart(data, fileName, mimeType, request, fieldName, uploadId, listener)
    }
    
    private fun uploadWithMultipart(
        data: ByteArray,
        fileName: String,
        mimeType: String,
        request: UploadRequest,
        fieldName: String,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        val boundary = "----WebKitFormBoundary${UUID.randomUUID()}"
        val connection = URL(request.url).openConnection() as HttpURLConnection
        
        return try {
            connection.apply {
                requestMethod = request.method
                doOutput = true
                doInput = true
                useCaches = false
                connectTimeout = request.timeout
                readTimeout = request.readTimeout
                setRequestProperty("Content-Type", "multipart/form-data; boundary=$boundary")
                
                request.headers.forEach { (key, value) ->
                    setRequestProperty(key, value)
                }
            }
            
            val outputStream = DataOutputStream(connection.outputStream)
            val totalBytes = data.size.toLong()
            
            // إضافة البارامترات
            request.params.forEach { (key, value) ->
                outputStream.writeBytes("--$boundary\r\n")
                outputStream.writeBytes("Content-Disposition: form-data; name=\"$key\"\r\n\r\n")
                outputStream.writeBytes("$value\r\n")
            }
            
            // إضافة الملف
            outputStream.writeBytes("--$boundary\r\n")
            outputStream.writeBytes("Content-Disposition: form-data; name=\"$fieldName\"; filename=\"$fileName\"\r\n")
            outputStream.writeBytes("Content-Type: $mimeType\r\n\r\n")
            
            // رفع البيانات مع إرسال التقدم
            var uploadedBytes = 0L
            val bufferSize = 4096
            var offset = 0
            
            while (offset < data.size) {
                val length = minOf(bufferSize, data.size - offset)
                outputStream.write(data, offset, length)
                offset += length
                uploadedBytes += length
                
                val progress = ((uploadedBytes * 100) / totalBytes).toInt()
                listener?.onProgress(uploadId, progress, uploadedBytes, totalBytes)
            }
            
            outputStream.writeBytes("\r\n--$boundary--\r\n")
            outputStream.flush()
            outputStream.close()
            
            val responseCode = connection.responseCode
            val responseBody = if (responseCode in 200..299) {
                connection.inputStream.bufferedReader().readText()
            } else {
                connection.errorStream?.bufferedReader()?.readText()
            }
            
            UploadResult(
                uploadId = uploadId,
                success = responseCode in 200..299,
                responseCode = responseCode,
                responseBody = responseBody,
                uploadedBytes = uploadedBytes,
                totalBytes = totalBytes
            )
        } catch (e: Exception) {
            UploadResult(
                uploadId = uploadId,
                success = false,
                responseCode = -1,
                responseBody = null,
                uploadedBytes = 0,
                totalBytes = data.size.toLong(),
                errorMessage = e.message
            )
        } finally {
            connection.disconnect()
        }
    }
    
    private fun performMultiUpload(
        filePaths: List<String>,
        request: UploadRequest,
        fieldName: String,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        val boundary = "----WebKitFormBoundary${UUID.randomUUID()}"
        val connection = URL(request.url).openConnection() as HttpURLConnection
        
        return try {
            connection.apply {
                requestMethod = request.method
                doOutput = true
                doInput = true
                useCaches = false
                connectTimeout = request.timeout
                readTimeout = request.readTimeout
                setRequestProperty("Content-Type", "multipart/form-data; boundary=$boundary")
                
                request.headers.forEach { (key, value) ->
                    setRequestProperty(key, value)
                }
            }
            
            val outputStream = DataOutputStream(connection.outputStream)
            var totalBytes = 0L
            var uploadedBytes = 0L
            
            // حساب الحجم الكلي
            filePaths.forEach { path ->
                totalBytes += File(path).length()
            }
            
            // رفع كل ملف
            filePaths.forEachIndexed { index, path ->
                val file = File(path)
                val mimeType = getMimeType(file.name)
                
                outputStream.writeBytes("--$boundary\r\n")
                outputStream.writeBytes("Content-Disposition: form-data; name=\"${fieldName}[$index]\"; filename=\"${file.name}\"\r\n")
                outputStream.writeBytes("Content-Type: $mimeType\r\n\r\n")
                
                val fileInputStream = FileInputStream(file)
                val buffer = ByteArray(4096)
                var bytesRead: Int
                
                while (fileInputStream.read(buffer).also { bytesRead = it } != -1) {
                    outputStream.write(buffer, 0, bytesRead)
                    uploadedBytes += bytesRead
                    
                    val progress = ((uploadedBytes * 100) / totalBytes).toInt()
                    listener?.onProgress(uploadId, progress, uploadedBytes, totalBytes)
                }
                
                fileInputStream.close()
                outputStream.writeBytes("\r\n")
            }
            
            outputStream.writeBytes("--$boundary--\r\n")
            outputStream.flush()
            outputStream.close()
            
            val responseCode = connection.responseCode
            val responseBody = if (responseCode in 200..299) {
                connection.inputStream.bufferedReader().readText()
            } else {
                connection.errorStream?.bufferedReader()?.readText()
            }
            
            UploadResult(
                uploadId = uploadId,
                success = responseCode in 200..299,
                responseCode = responseCode,
                responseBody = responseBody,
                uploadedBytes = uploadedBytes,
                totalBytes = totalBytes
            )
        } catch (e: Exception) {
            UploadResult(
                uploadId = uploadId,
                success = false,
                responseCode = -1,
                responseBody = null,
                uploadedBytes = 0,
                totalBytes = 0,
                errorMessage = e.message
            )
        } finally {
            connection.disconnect()
        }
    }
    
    private fun performJsonUpload(
        json: String,
        request: UploadRequest,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        val connection = URL(request.url).openConnection() as HttpURLConnection
        
        return try {
            connection.apply {
                requestMethod = request.method
                doOutput = true
                doInput = true
                connectTimeout = request.timeout
                readTimeout = request.readTimeout
                setRequestProperty("Content-Type", "application/json; charset=utf-8")
                
                request.headers.forEach { (key, value) ->
                    setRequestProperty(key, value)
                }
            }
            
            val bytes = json.toByteArray(Charsets.UTF_8)
            val totalBytes = bytes.size.toLong()
            
            connection.outputStream.use { it.write(bytes) }
            listener?.onProgress(uploadId, 100, totalBytes, totalBytes)
            
            val responseCode = connection.responseCode
            val responseBody = if (responseCode in 200..299) {
                connection.inputStream.bufferedReader().readText()
            } else {
                connection.errorStream?.bufferedReader()?.readText()
            }
            
            UploadResult(
                uploadId = uploadId,
                success = responseCode in 200..299,
                responseCode = responseCode,
                responseBody = responseBody,
                uploadedBytes = totalBytes,
                totalBytes = totalBytes
            )
        } catch (e: Exception) {
            UploadResult(
                uploadId = uploadId,
                success = false,
                responseCode = -1,
                responseBody = null,
                uploadedBytes = 0,
                totalBytes = json.length.toLong(),
                errorMessage = e.message
            )
        } finally {
            connection.disconnect()
        }
    }
    
    private fun performFormDataUpload(
        formData: Map<String, String>,
        request: UploadRequest,
        uploadId: String,
        listener: UploadListener?
    ): UploadResult {
        val connection = URL(request.url).openConnection() as HttpURLConnection
        
        return try {
            connection.apply {
                requestMethod = request.method
                doOutput = true
                doInput = true
                connectTimeout = request.timeout
                readTimeout = request.readTimeout
                setRequestProperty("Content-Type", "application/x-www-form-urlencoded")
                
                request.headers.forEach { (key, value) ->
                    setRequestProperty(key, value)
                }
            }
            
            val postData = formData.entries.joinToString("&") { (k, v) ->
                "${java.net.URLEncoder.encode(k, "UTF-8")}=${java.net.URLEncoder.encode(v, "UTF-8")}"
            }
            
            val bytes = postData.toByteArray(Charsets.UTF_8)
            val totalBytes = bytes.size.toLong()
            
            connection.outputStream.use { it.write(bytes) }
            listener?.onProgress(uploadId, 100, totalBytes, totalBytes)
            
            val responseCode = connection.responseCode
            val responseBody = if (responseCode in 200..299) {
                connection.inputStream.bufferedReader().readText()
            } else {
                connection.errorStream?.bufferedReader()?.readText()
            }
            
            UploadResult(
                uploadId = uploadId,
                success = responseCode in 200..299,
                responseCode = responseCode,
                responseBody = responseBody,
                uploadedBytes = totalBytes,
                totalBytes = totalBytes
            )
        } catch (e: Exception) {
            UploadResult(
                uploadId = uploadId,
                success = false,
                responseCode = -1,
                responseBody = null,
                uploadedBytes = 0,
                totalBytes = 0,
                errorMessage = e.message
            )
        } finally {
            connection.disconnect()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getFileName(context: Context, uri: Uri): String {
        val cursor = context.contentResolver.query(uri, null, null, null, null)
        return cursor?.use {
            val nameIndex = it.getColumnIndex(android.provider.OpenableColumns.DISPLAY_NAME)
            it.moveToFirst()
            it.getString(nameIndex)
        } ?: "file_${System.currentTimeMillis()}"
    }
    
    private fun getMimeType(fileName: String): String {
        val extension = MimeTypeMap.getFileExtensionFromUrl(fileName)
        return MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension) ?: "application/octet-stream"
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        cancelAll()
        executor.shutdown()
    }
}
