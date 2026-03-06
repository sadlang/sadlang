package sad.android.communication.clipboard

import android.content.ClipData
import android.content.ClipDescription
import android.content.ClipboardManager
import android.content.Context
import android.net.Uri
import android.os.Build

/**
 * جسر الحافظة
 * Clipboard Bridge - نسخ ولصق
 * 
 * @author فريق لغة ص
 */
object SadClipboardBridge {
    
    private var clipboardManager: ClipboardManager? = null
    private var onChangeCallback: ((String?) -> Unit)? = null
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(context: Context) {
        clipboardManager = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         نسخ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نسخ نص / Copy text
     */
    fun copyText(context: Context, text: String, label: String = "نص منسوخ") {
        val cm = getManager(context)
        val clip = ClipData.newPlainText(label, text)
        cm.setPrimaryClip(clip)
    }
    
    /**
     * نسخ HTML / Copy HTML
     */
    fun copyHtml(context: Context, text: String, html: String, label: String = "HTML") {
        val cm = getManager(context)
        val clip = ClipData.newHtmlText(label, text, html)
        cm.setPrimaryClip(clip)
    }
    
    /**
     * نسخ رابط / Copy URI
     */
    fun copyUri(context: Context, uri: Uri, label: String = "رابط") {
        val cm = getManager(context)
        val clip = ClipData.newUri(context.contentResolver, label, uri)
        cm.setPrimaryClip(clip)
    }
    
    /**
     * نسخ رابط نصي / Copy URL string
     */
    fun copyUrl(context: Context, url: String, label: String = "رابط") {
        copyUri(context, Uri.parse(url), label)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         لصق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * لصق نص / Paste text
     */
    fun pasteText(context: Context): String? {
        val cm = getManager(context)
        val clip = cm.primaryClip ?: return null
        
        if (clip.itemCount == 0) return null
        
        return clip.getItemAt(0).coerceToText(context)?.toString()
    }
    
    /**
     * لصق HTML / Paste HTML
     */
    fun pasteHtml(context: Context): String? {
        val cm = getManager(context)
        val clip = cm.primaryClip ?: return null
        
        if (clip.itemCount == 0) return null
        
        return clip.getItemAt(0).htmlText
    }
    
    /**
     * لصق Uri / Paste URI
     */
    fun pasteUri(context: Context): Uri? {
        val cm = getManager(context)
        val clip = cm.primaryClip ?: return null
        
        if (clip.itemCount == 0) return null
        
        return clip.getItemAt(0).uri
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فحص الحافظة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل يوجد محتوى؟ / Has content?
     */
    fun hasContent(context: Context): Boolean {
        return getManager(context).hasPrimaryClip()
    }
    
    /**
     * هل يوجد نص؟ / Has text?
     */
    fun hasText(context: Context): Boolean {
        val cm = getManager(context)
        return cm.hasPrimaryClip() &&
               cm.primaryClipDescription?.hasMimeType(ClipDescription.MIMETYPE_TEXT_PLAIN) == true
    }
    
    /**
     * هل يوجد HTML؟ / Has HTML?
     */
    fun hasHtml(context: Context): Boolean {
        val cm = getManager(context)
        return cm.hasPrimaryClip() &&
               cm.primaryClipDescription?.hasMimeType(ClipDescription.MIMETYPE_TEXT_HTML) == true
    }
    
    /**
     * هل يوجد Uri؟ / Has URI?
     */
    fun hasUri(context: Context): Boolean {
        val cm = getManager(context)
        if (!cm.hasPrimaryClip()) return false
        
        val clip = cm.primaryClip ?: return false
        if (clip.itemCount == 0) return false
        
        return clip.getItemAt(0).uri != null
    }
    
    /**
     * جلب نوع المحتوى / Get content type
     */
    fun getContentType(context: Context): String? {
        val cm = getManager(context)
        return cm.primaryClipDescription?.getMimeType(0)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مسح الحافظة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مسح الحافظة / Clear clipboard
     */
    fun clear(context: Context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            getManager(context).clearPrimaryClip()
        } else {
            // للإصدارات القديمة
            copyText(context, "", "")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مراقبة التغييرات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مراقبة تغييرات الحافظة / Watch clipboard changes
     */
    fun startWatching(context: Context, onChange: (String?) -> Unit) {
        onChangeCallback = onChange
        
        getManager(context).addPrimaryClipChangedListener {
            val text = pasteText(context)
            onChangeCallback?.invoke(text)
        }
    }
    
    /**
     * إيقاف المراقبة / Stop watching
     */
    fun stopWatching() {
        onChangeCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getManager(context: Context): ClipboardManager {
        return clipboardManager ?: (context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager).also {
            clipboardManager = it
        }
    }
    
    /**
     * معلومات الحافظة / Clipboard info
     */
    data class ClipboardInfo(
        val hasContent: Boolean,
        val contentType: String?,
        val itemCount: Int,
        val label: String?
    )
    
    /**
     * جلب معلومات الحافظة / Get clipboard info
     */
    fun getInfo(context: Context): ClipboardInfo {
        val cm = getManager(context)
        val clip = cm.primaryClip
        
        return ClipboardInfo(
            hasContent = cm.hasPrimaryClip(),
            contentType = cm.primaryClipDescription?.getMimeType(0),
            itemCount = clip?.itemCount ?: 0,
            label = clip?.description?.label?.toString()
        )
    }
}
