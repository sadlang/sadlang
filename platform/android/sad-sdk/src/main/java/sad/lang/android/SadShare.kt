/**
 * =============================================================================
 * @file SadShare.kt
 * @brief (AR) واجهة المشاركة — Share Intent
 * @brief (EN) Share Interface — Share Intent
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import android.content.Intent
import android.net.Uri
import androidx.core.content.FileProvider
import java.io.File

/**
 * نظام المشاركة للغة ص
 */
class SadShare(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Text Sharing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * مشاركة نص
     */
    fun shareText(text: String, title: String = "مشاركة") {
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "text/plain"
            putExtra(Intent.EXTRA_TEXT, text)
        }
        
        startChooser(intent, title)
    }
    
    /**
     * مشاركة نص مع عنوان
     */
    fun shareTextWithSubject(text: String, subject: String, title: String = "مشاركة") {
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "text/plain"
            putExtra(Intent.EXTRA_SUBJECT, subject)
            putExtra(Intent.EXTRA_TEXT, text)
        }
        
        startChooser(intent, title)
    }
    
    /**
     * مشاركة رابط
     */
    fun shareUrl(url: String, title: String = "مشاركة الرابط") {
        shareText(url, title)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  File Sharing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * مشاركة ملف
     */
    fun shareFile(
        filePath: String,
        mimeType: String = "*/*",
        title: String = "مشاركة ملف"
    ) {
        val file = File(filePath)
        if (!file.exists()) return
        
        val uri = getFileUri(file)
        
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = mimeType
            putExtra(Intent.EXTRA_STREAM, uri)
            addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
        }
        
        startChooser(intent, title)
    }
    
    /**
     * مشاركة صورة
     */
    fun shareImage(imagePath: String, title: String = "مشاركة صورة") {
        shareFile(imagePath, "image/*", title)
    }
    
    /**
     * مشاركة ملف PDF
     */
    fun sharePdf(pdfPath: String, title: String = "مشاركة PDF") {
        shareFile(pdfPath, "application/pdf", title)
    }
    
    /**
     * مشاركة عدة ملفات
     */
    fun shareMultipleFiles(
        filePaths: List<String>,
        mimeType: String = "*/*",
        title: String = "مشاركة ملفات"
    ) {
        val uris = ArrayList<Uri>()
        
        for (path in filePaths) {
            val file = File(path)
            if (file.exists()) {
                uris.add(getFileUri(file))
            }
        }
        
        if (uris.isEmpty()) return
        
        val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
            type = mimeType
            putParcelableArrayListExtra(Intent.EXTRA_STREAM, uris)
            addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
        }
        
        startChooser(intent, title)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Email
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إرسال بريد إلكتروني
     */
    fun sendEmail(
        to: Array<String>,
        subject: String,
        body: String,
        cc: Array<String>? = null,
        bcc: Array<String>? = null,
        attachmentPath: String? = null
    ) {
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "message/rfc822"
            putExtra(Intent.EXTRA_EMAIL, to)
            putExtra(Intent.EXTRA_SUBJECT, subject)
            putExtra(Intent.EXTRA_TEXT, body)
            
            cc?.let { putExtra(Intent.EXTRA_CC, it) }
            bcc?.let { putExtra(Intent.EXTRA_BCC, it) }
            
            attachmentPath?.let { path ->
                val file = File(path)
                if (file.exists()) {
                    val uri = getFileUri(file)
                    putExtra(Intent.EXTRA_STREAM, uri)
                    addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                }
            }
        }
        
        startChooser(intent, "إرسال بريد")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Open With
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * فتح رابط في المتصفح
     */
    fun openUrl(url: String) {
        val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url)).apply {
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        context.startActivity(intent)
    }
    
    /**
     * فتح ملف بتطبيق خارجي
     */
    fun openFile(filePath: String, mimeType: String = "*/*") {
        val file = File(filePath)
        if (!file.exists()) return
        
        val uri = getFileUri(file)
        
        val intent = Intent(Intent.ACTION_VIEW).apply {
            setDataAndType(uri, mimeType)
            addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        
        context.startActivity(intent)
    }
    
    /**
     * فتح الخرائط
     */
    fun openMap(latitude: Double, longitude: Double, label: String = "") {
        val uri = if (label.isNotEmpty()) {
            Uri.parse("geo:$latitude,$longitude?q=$latitude,$longitude($label)")
        } else {
            Uri.parse("geo:$latitude,$longitude")
        }
        
        val intent = Intent(Intent.ACTION_VIEW, uri).apply {
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        
        context.startActivity(intent)
    }
    
    /**
     * فتح تطبيق الهاتف
     */
    fun openDialer(phoneNumber: String) {
        val intent = Intent(Intent.ACTION_DIAL, Uri.parse("tel:$phoneNumber")).apply {
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        context.startActivity(intent)
    }
    
    /**
     * فتح تطبيق الرسائل
     */
    fun openSms(phoneNumber: String, message: String = "") {
        val uri = Uri.parse("smsto:$phoneNumber")
        val intent = Intent(Intent.ACTION_SENDTO, uri).apply {
            putExtra("sms_body", message)
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Clipboard
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نسخ نص للحافظة
     */
    fun copyToClipboard(text: String, label: String = "نص") {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) 
            as android.content.ClipboardManager
        val clip = android.content.ClipData.newPlainText(label, text)
        clipboard.setPrimaryClip(clip)
    }
    
    /**
     * لصق من الحافظة
     */
    fun pasteFromClipboard(): String? {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) 
            as android.content.ClipboardManager
        
        return if (clipboard.hasPrimaryClip()) {
            clipboard.primaryClip?.getItemAt(0)?.text?.toString()
        } else {
            null
        }
    }
    
    /**
     * هل الحافظة تحتوي نص؟
     */
    fun hasClipboardText(): Boolean {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) 
            as android.content.ClipboardManager
        return clipboard.hasPrimaryClip() && 
               clipboard.primaryClipDescription?.hasMimeType("text/plain") == true
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Helpers
    // ═══════════════════════════════════════════════════════════════════════════
    
    private fun getFileUri(file: File): Uri {
        return FileProvider.getUriForFile(
            context,
            "${context.packageName}.fileprovider",
            file
        )
    }
    
    private fun startChooser(intent: Intent, title: String) {
        val chooser = Intent.createChooser(intent, title).apply {
            addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
        }
        context.startActivity(chooser)
    }
}
