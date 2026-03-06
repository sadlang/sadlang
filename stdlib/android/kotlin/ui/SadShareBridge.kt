// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadShareBridge.kt
// الوصف: جسر Kotlin للمشاركة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Intent
import android.net.Uri
import androidx.core.content.FileProvider
import java.io.File

/**
 * جسر المشاركة — SadShareBridge
 */
object SadShareBridge {
    
    private var activity: Activity? = null
    private var fileProviderAuthority: String = ""
    
    @JvmStatic
    fun initialize(act: Activity, authority: String = "${act.packageName}.fileprovider") {
        activity = act
        fileProviderAuthority = authority
    }
    
    @JvmStatic
    fun shareText(text: String, title: String = "مشاركة") {
        val act = activity ?: return
        
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "text/plain"
            putExtra(Intent.EXTRA_TEXT, text)
        }
        
        act.startActivity(Intent.createChooser(intent, title))
    }
    
    @JvmStatic
    fun shareTextWithSubject(text: String, subject: String, title: String = "مشاركة") {
        val act = activity ?: return
        
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "text/plain"
            putExtra(Intent.EXTRA_SUBJECT, subject)
            putExtra(Intent.EXTRA_TEXT, text)
        }
        
        act.startActivity(Intent.createChooser(intent, title))
    }
    
    @JvmStatic
    fun shareUrl(url: String, title: String = "مشاركة رابط") {
        shareText(url, title)
    }
    
    @JvmStatic
    fun shareImage(imagePath: String, title: String = "مشاركة صورة"): Boolean {
        val act = activity ?: return false
        
        return try {
            val file = File(imagePath)
            if (!file.exists()) return false
            
            val uri = FileProvider.getUriForFile(act, fileProviderAuthority, file)
            
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "image/*"
                putExtra(Intent.EXTRA_STREAM, uri)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            
            act.startActivity(Intent.createChooser(intent, title))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun shareFile(filePath: String, mimeType: String = "*/*", title: String = "مشاركة ملف"): Boolean {
        val act = activity ?: return false
        
        return try {
            val file = File(filePath)
            if (!file.exists()) return false
            
            val uri = FileProvider.getUriForFile(act, fileProviderAuthority, file)
            
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = mimeType
                putExtra(Intent.EXTRA_STREAM, uri)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            
            act.startActivity(Intent.createChooser(intent, title))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun shareMultipleImages(imagePaths: List<String>, title: String = "مشاركة صور"): Boolean {
        val act = activity ?: return false
        
        return try {
            val uris = ArrayList<Uri>()
            
            for (path in imagePaths) {
                val file = File(path)
                if (file.exists()) {
                    uris.add(FileProvider.getUriForFile(act, fileProviderAuthority, file))
                }
            }
            
            if (uris.isEmpty()) return false
            
            val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
                type = "image/*"
                putParcelableArrayListExtra(Intent.EXTRA_STREAM, uris)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
            }
            
            act.startActivity(Intent.createChooser(intent, title))
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun shareViaEmail(
        to: Array<String>,
        subject: String,
        body: String,
        title: String = "إرسال بريد إلكتروني"
    ) {
        val act = activity ?: return
        
        val intent = Intent(Intent.ACTION_SENDTO).apply {
            data = Uri.parse("mailto:")
            putExtra(Intent.EXTRA_EMAIL, to)
            putExtra(Intent.EXTRA_SUBJECT, subject)
            putExtra(Intent.EXTRA_TEXT, body)
        }
        
        if (intent.resolveActivity(act.packageManager) != null) {
            act.startActivity(Intent.createChooser(intent, title))
        }
    }
    
    @JvmStatic
    fun shareViaSms(phoneNumber: String, message: String) {
        val act = activity ?: return
        
        val intent = Intent(Intent.ACTION_SENDTO).apply {
            data = Uri.parse("smsto:$phoneNumber")
            putExtra("sms_body", message)
        }
        
        if (intent.resolveActivity(act.packageManager) != null) {
            act.startActivity(intent)
        }
    }
}
