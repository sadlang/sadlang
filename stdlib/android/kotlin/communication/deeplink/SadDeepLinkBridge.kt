package sad.android.communication.deeplink

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.os.Bundle

/**
 * جسر الروابط العميقة
 * Deep Links Bridge - التعامل مع الروابط العميقة
 * 
 * @author فريق لغة ص
 */
object SadDeepLinkBridge {

    /**
     * بيانات الرابط العميق / Deep link data
     */
    data class DeepLinkData(
        val scheme: String,
        val host: String,
        val path: String,
        val queryParams: Map<String, String>,
        val fragment: String?
    )
    
    /**
     * نتيجة المعالجة / Handle result
     */
    data class HandleResult(
        val handled: Boolean,
        val data: DeepLinkData? = null,
        val error: String? = null
    )
    
    private val handlers = mutableMapOf<String, (DeepLinkData) -> Unit>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         تسجيل المعالجات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل معالج رابط / Register link handler
     */
    fun registerHandler(pattern: String, handler: (DeepLinkData) -> Unit) {
        handlers[pattern] = handler
    }
    
    /**
     * إلغاء تسجيل معالج / Unregister handler
     */
    fun unregisterHandler(pattern: String) {
        handlers.remove(pattern)
    }
    
    /**
     * مسح كل المعالجات / Clear all handlers
     */
    fun clearHandlers() {
        handlers.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة الروابط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة رابط من Intent / Handle link from Intent
     */
    fun handleIntent(intent: Intent?): HandleResult {
        val uri = intent?.data ?: return HandleResult(false, error = "لا يوجد رابط")
        return handleUri(uri)
    }
    
    /**
     * معالجة Uri / Handle Uri
     */
    fun handleUri(uri: Uri): HandleResult {
        return try {
            val data = parseUri(uri)
            
            // البحث عن معالج مطابق
            val matchedPattern = findMatchingPattern(data)
            
            if (matchedPattern != null) {
                handlers[matchedPattern]?.invoke(data)
                HandleResult(true, data)
            } else {
                HandleResult(false, data, "لا يوجد معالج مسجل")
            }
        } catch (e: Exception) {
            HandleResult(false, error = e.message)
        }
    }
    
    /**
     * معالجة نص رابط / Handle link string
     */
    fun handleLink(link: String): HandleResult {
        return try {
            val uri = Uri.parse(link)
            handleUri(uri)
        } catch (e: Exception) {
            HandleResult(false, error = "رابط غير صالح")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحليل الروابط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحليل Uri / Parse Uri
     */
    fun parseUri(uri: Uri): DeepLinkData {
        val queryParams = mutableMapOf<String, String>()
        
        uri.queryParameterNames.forEach { key ->
            uri.getQueryParameter(key)?.let { value ->
                queryParams[key] = value
            }
        }
        
        return DeepLinkData(
            scheme = uri.scheme ?: "",
            host = uri.host ?: "",
            path = uri.path ?: "",
            queryParams = queryParams,
            fragment = uri.fragment
        )
    }
    
    /**
     * البحث عن نمط مطابق / Find matching pattern
     */
    private fun findMatchingPattern(data: DeepLinkData): String? {
        return handlers.keys.find { pattern ->
            matchPattern(pattern, data)
        }
    }
    
    /**
     * مطابقة النمط / Match pattern
     */
    private fun matchPattern(pattern: String, data: DeepLinkData): Boolean {
        val parts = pattern.split("/")
        val pathParts = data.path.split("/").filter { it.isNotEmpty() }
        
        if (parts.size != pathParts.size + 1) return false // +1 for host
        
        return parts.first() == data.host &&
               parts.drop(1).zip(pathParts).all { (p, actual) ->
                   p.startsWith(":") || p == actual
               }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء الروابط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بناء رابط عميق / Build deep link
     */
    fun buildLink(
        scheme: String,
        host: String,
        path: String = "",
        params: Map<String, String> = emptyMap()
    ): String {
        val builder = Uri.Builder()
            .scheme(scheme)
            .authority(host)
            .path(path)
        
        params.forEach { (key, value) ->
            builder.appendQueryParameter(key, value)
        }
        
        return builder.build().toString()
    }
    
    /**
     * إنشاء رابط تطبيق / Create app link
     */
    fun createAppLink(
        context: Context,
        path: String,
        params: Map<String, String> = emptyMap()
    ): String {
        return buildLink(
            scheme = context.packageName,
            host = "app",
            path = path,
            params = params
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنقل بالروابط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح رابط عميق / Open deep link
     */
    fun openLink(context: Context, link: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(link)).apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * التحقق من قدرة فتح الرابط / Check if can open link
     */
    fun canOpenLink(context: Context, link: String): Boolean {
        return try {
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(link))
            intent.resolveActivity(context.packageManager) != null
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         App Links (Android)
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * استخراج البيانات من Bundle / Extract data from Bundle
     */
    fun extractFromBundle(bundle: Bundle?): Map<String, Any?> {
        val result = mutableMapOf<String, Any?>()
        bundle?.keySet()?.forEach { key ->
            result[key] = bundle.get(key)
        }
        return result
    }
}
