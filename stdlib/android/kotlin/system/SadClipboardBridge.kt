// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadClipboardBridge.kt
// الوصف: جسر Kotlin للحافظة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.net.Uri
import android.os.Build

/**
 * جسر الحافظة — SadClipboardBridge
 */
object SadClipboardBridge {
    
    private var context: Context? = null
    private var clipboardManager: ClipboardManager? = null
    private var onClipboardChanged: ((String?) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        clipboardManager = ctx.getSystemService(Context.CLIPBOARD_SERVICE) as? ClipboardManager
    }
    
    @JvmStatic
    fun copyText(text: String, label: String = "نص منسوخ") {
        val clipboard = clipboardManager ?: return
        val clip = ClipData.newPlainText(label, text)
        clipboard.setPrimaryClip(clip)
    }
    
    @JvmStatic
    fun copyUri(uri: String, label: String = "رابط منسوخ") {
        val ctx = context ?: return
        val clipboard = clipboardManager ?: return
        val clip = ClipData.newRawUri(label, Uri.parse(uri))
        clipboard.setPrimaryClip(clip)
    }
    
    @JvmStatic
    fun copyHtml(htmlText: String, plainText: String, label: String = "محتوى منسوخ") {
        val clipboard = clipboardManager ?: return
        val clip = ClipData.newHtmlText(label, plainText, htmlText)
        clipboard.setPrimaryClip(clip)
    }
    
    @JvmStatic
    fun paste(): String? {
        val clipboard = clipboardManager ?: return null
        val clip = clipboard.primaryClip ?: return null
        
        if (clip.itemCount == 0) return null
        
        val item = clip.getItemAt(0)
        return item.text?.toString()
    }
    
    @JvmStatic
    fun pasteUri(): String? {
        val clipboard = clipboardManager ?: return null
        val clip = clipboard.primaryClip ?: return null
        
        if (clip.itemCount == 0) return null
        
        val item = clip.getItemAt(0)
        return item.uri?.toString()
    }
    
    @JvmStatic
    fun pasteHtml(): String? {
        val clipboard = clipboardManager ?: return null
        val clip = clipboard.primaryClip ?: return null
        
        if (clip.itemCount == 0) return null
        
        val item = clip.getItemAt(0)
        return item.htmlText
    }
    
    @JvmStatic
    fun hasText(): Boolean {
        val clipboard = clipboardManager ?: return false
        return clipboard.hasPrimaryClip() && 
               clipboard.primaryClipDescription?.hasMimeType("text/*") == true
    }
    
    @JvmStatic
    fun hasUri(): Boolean {
        val clipboard = clipboardManager ?: return false
        val clip = clipboard.primaryClip ?: return false
        
        if (clip.itemCount == 0) return false
        return clip.getItemAt(0).uri != null
    }
    
    @JvmStatic
    fun clear() {
        val clipboard = clipboardManager ?: return
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            clipboard.clearPrimaryClip()
        } else {
            // للإصدارات القديمة، نضع نص فارغ
            val clip = ClipData.newPlainText("", "")
            clipboard.setPrimaryClip(clip)
        }
    }
    
    @JvmStatic
    fun addOnClipboardChangedListener(callback: (String?) -> Unit) {
        val clipboard = clipboardManager ?: return
        onClipboardChanged = callback
        
        clipboard.addPrimaryClipChangedListener {
            val text = paste()
            onClipboardChanged?.invoke(text)
        }
    }
    
    @JvmStatic
    fun getClipDescription(): Map<String, Any>? {
        val clipboard = clipboardManager ?: return null
        val description = clipboard.primaryClipDescription ?: return null
        
        val mimeTypes = mutableListOf<String>()
        for (i in 0 until description.mimeTypeCount) {
            mimeTypes.add(description.getMimeType(i))
        }
        
        return mapOf(
            "label" to (description.label?.toString() ?: ""),
            "mimeTypes" to mimeTypes,
            "hasMimeType_text" to description.hasMimeType("text/*"),
            "hasMimeType_image" to description.hasMimeType("image/*")
        )
    }
}
