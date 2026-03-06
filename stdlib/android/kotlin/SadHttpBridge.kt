// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadHttpBridge.kt
// الوصف: جسر Kotlin للدوال الأصلية لمكتبة HTTP في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_http_* المستخدمة في stdlib/android/شبكة.ص
// بـ Android APIs الفعلية (OkHttp).
//
// الدوال المُنفّذة:
//   __اندرويد_http_get(url, headers, timeout) -> استجابة
//   __اندرويد_http_post(url, body, headers, timeout) -> استجابة
//   __اندرويد_http_download(url, path) -> bool
//
// المتطلبات في build.gradle.kts:
//   implementation("com.squareup.okhttp3:okhttp:4.12.0")
//   implementation("org.jetbrains.kotlinx:kotlinx-coroutines-android:1.7.3")
//
// الاستخدام في لغة ص:
//   استورد "شبكة/http"
//   متغير استجابة = http.احصل("https://api.example.com/data")
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.os.Handler
import android.os.Looper
import kotlinx.coroutines.*
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.File
import java.io.IOException
import java.util.concurrent.TimeUnit

/**
 * جسر HTTP — SadHttpBridge
 * 
 * يوفر واجهة لتنفيذ طلبات HTTP من كود لغة ص.
 * يستخدم OkHttp لتنفيذ الطلبات بكفاءة عالية.
 */
object SadHttpBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد OkHttp Client
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var defaultTimeout: Long = 30000 // 30 ثانية افتراضياً
    
    /**
     * إنشاء عميل OkHttp مع المهلة المحددة
     */
    private fun createClient(timeoutMs: Long): OkHttpClient {
        return OkHttpClient.Builder()
            .connectTimeout(timeoutMs, TimeUnit.MILLISECONDS)
            .readTimeout(timeoutMs, TimeUnit.MILLISECONDS)
            .writeTimeout(timeoutMs, TimeUnit.MILLISECONDS)
            .build()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // نتيجة الاستجابة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هيكل نتيجة HTTP
     * يُمثّل الاستجابة من الخادم
     */
    data class HttpResult(
        val success: Boolean,          // هل نجح الطلب؟
        val statusCode: Int,           // رمز الحالة (200, 404, 500...)
        val body: String,              // محتوى الاستجابة
        val headers: Map<String, String>, // الرؤوس
        val error: String? = null      // رسالة الخطأ إن وجدت
    ) {
        /**
         * تحويل إلى Map للاستخدام في لغة ص
         */
        fun toMap(): Map<String, Any?> = mapOf(
            "نجح" to success,
            "حالة" to statusCode,
            "محتوى" to body,
            "رؤوس" to headers,
            "خطأ" to error
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_http_get — طلب GET
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ طلب GET
     * 
     * @param url العنوان المطلوب
     * @param headers الرؤوس (Map<String, String>)
     * @param timeoutMs المهلة بالملي ثانية
     * @return نتيجة HTTP كـ Map
     */
    @JvmStatic
    fun httpGet(
        url: String,
        headers: Map<String, String> = emptyMap(),
        timeoutMs: Long = defaultTimeout
    ): Map<String, Any?> {
        return try {
            val client = createClient(timeoutMs)
            
            val requestBuilder = Request.Builder().url(url)
            headers.forEach { (key, value) ->
                requestBuilder.addHeader(key, value)
            }
            
            val request = requestBuilder.build()
            val response = client.newCall(request).execute()
            
            val responseHeaders = mutableMapOf<String, String>()
            response.headers.forEach { (name, value) ->
                responseHeaders[name] = value
            }
            
            HttpResult(
                success = response.isSuccessful,
                statusCode = response.code,
                body = response.body?.string() ?: "",
                headers = responseHeaders
            ).toMap()
            
        } catch (e: IOException) {
            HttpResult(
                success = false,
                statusCode = 0,
                body = "",
                headers = emptyMap(),
                error = "خطأ في الاتصال: ${e.message}"
            ).toMap()
        } catch (e: Exception) {
            HttpResult(
                success = false,
                statusCode = 0,
                body = "",
                headers = emptyMap(),
                error = "خطأ: ${e.message}"
            ).toMap()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_http_post — طلب POST
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ طلب POST
     * 
     * @param url العنوان المطلوب
     * @param body محتوى الطلب
     * @param headers الرؤوس
     * @param timeoutMs المهلة بالملي ثانية
     * @return نتيجة HTTP كـ Map
     */
    @JvmStatic
    fun httpPost(
        url: String,
        body: String,
        headers: Map<String, String> = emptyMap(),
        timeoutMs: Long = defaultTimeout
    ): Map<String, Any?> {
        return try {
            val client = createClient(timeoutMs)
            
            // تحديد نوع المحتوى
            val contentType = headers["Content-Type"] ?: "application/json"
            val requestBody = body.toRequestBody(contentType.toMediaType())
            
            val requestBuilder = Request.Builder()
                .url(url)
                .post(requestBody)
            
            headers.forEach { (key, value) ->
                requestBuilder.addHeader(key, value)
            }
            
            val request = requestBuilder.build()
            val response = client.newCall(request).execute()
            
            val responseHeaders = mutableMapOf<String, String>()
            response.headers.forEach { (name, value) ->
                responseHeaders[name] = value
            }
            
            HttpResult(
                success = response.isSuccessful,
                statusCode = response.code,
                body = response.body?.string() ?: "",
                headers = responseHeaders
            ).toMap()
            
        } catch (e: IOException) {
            HttpResult(
                success = false,
                statusCode = 0,
                body = "",
                headers = emptyMap(),
                error = "خطأ في الاتصال: ${e.message}"
            ).toMap()
        } catch (e: Exception) {
            HttpResult(
                success = false,
                statusCode = 0,
                body = "",
                headers = emptyMap(),
                error = "خطأ: ${e.message}"
            ).toMap()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_http_download — تنزيل ملف
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنزيل ملف من URL
     * 
     * @param url عنوان الملف
     * @param filePath مسار الحفظ
     * @param onProgress دالة التقدم (اختياري)
     * @return true إذا نجح التنزيل
     */
    @JvmStatic
    fun httpDownload(
        url: String,
        filePath: String,
        onProgress: ((Long, Long) -> Unit)? = null
    ): Boolean {
        return try {
            val client = createClient(60000) // دقيقة للتنزيل
            
            val request = Request.Builder().url(url).build()
            val response = client.newCall(request).execute()
            
            if (!response.isSuccessful) {
                return false
            }
            
            val body = response.body ?: return false
            val contentLength = body.contentLength()
            
            val file = File(filePath)
            file.parentFile?.mkdirs()
            
            file.outputStream().use { output ->
                body.byteStream().use { input ->
                    val buffer = ByteArray(8192)
                    var bytesRead: Int
                    var totalBytesRead: Long = 0
                    
                    while (input.read(buffer).also { bytesRead = it } != -1) {
                        output.write(buffer, 0, bytesRead)
                        totalBytesRead += bytesRead
                        onProgress?.invoke(totalBytesRead, contentLength)
                    }
                }
            }
            
            true
            
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Coroutines — طلبات غير متزامنة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * طلب GET غير متزامن (Async)
     * 
     * @param url العنوان
     * @param headers الرؤوس
     * @param callback دالة الاستجابة
     */
    @JvmStatic
    fun httpGetAsync(
        url: String,
        headers: Map<String, String> = emptyMap(),
        callback: (Map<String, Any?>) -> Unit
    ) {
        CoroutineScope(Dispatchers.IO).launch {
            val result = httpGet(url, headers)
            withContext(Dispatchers.Main) {
                callback(result)
            }
        }
    }
    
    /**
     * طلب POST غير متزامن (Async)
     * 
     * @param url العنوان
     * @param body المحتوى
     * @param headers الرؤوس
     * @param callback دالة الاستجابة
     */
    @JvmStatic
    fun httpPostAsync(
        url: String,
        body: String,
        headers: Map<String, String> = emptyMap(),
        callback: (Map<String, Any?>) -> Unit
    ) {
        CoroutineScope(Dispatchers.IO).launch {
            val result = httpPost(url, body, headers)
            withContext(Dispatchers.Main) {
                callback(result)
            }
        }
    }
}
