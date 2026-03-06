package sad.android.communication.share

import android.content.Context
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import androidx.core.content.FileProvider
import java.io.File
import java.io.FileOutputStream

/**
 * جسر المشاركة المتقدم
 * Advanced Share Bridge - مشاركة المحتوى
 * 
 * @author فريق لغة ص
 */
object SadShareBridge {
    
    /**
     * نتيجة المشاركة / Share result
     */
    data class ShareResult(
        val success: Boolean,
        val error: String? = null
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         مشاركة النص
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مشاركة نص / Share text
     */
    fun shareText(context: Context, text: String, title: String = "مشاركة"): ShareResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/plain"
                putExtra(Intent.EXTRA_TEXT, text)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(Intent.createChooser(intent, title).apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            })
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    /**
     * مشاركة نص مع عنوان / Share text with subject
     */
    fun shareTextWithSubject(
        context: Context,
        text: String,
        subject: String,
        title: String = "مشاركة"
    ): ShareResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/plain"
                putExtra(Intent.EXTRA_TEXT, text)
                putExtra(Intent.EXTRA_SUBJECT, subject)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(Intent.createChooser(intent, title).apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            })
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مشاركة الروابط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مشاركة رابط / Share URL
     */
    fun shareUrl(context: Context, url: String, title: String? = null): ShareResult {
        val text = if (title != null) "$title\n$url" else url
        return shareText(context, text)
    }
    
    /**
     * مشاركة رابط مع وصف / Share URL with description
     */
    fun shareUrlWithDescription(
        context: Context,
        url: String,
        description: String
    ): ShareResult {
        return shareText(context, "$description\n\n$url")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مشاركة الصور
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مشاركة صورة / Share image
     */
    fun shareImage(context: Context, imageUri: Uri, text: String? = null): ShareResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "image/*"
                putExtra(Intent.EXTRA_STREAM, imageUri)
                text?.let { putExtra(Intent.EXTRA_TEXT, it) }
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(Intent.createChooser(intent, "مشاركة صورة").apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            })
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    /**
     * مشاركة صورة من Bitmap / Share image from Bitmap
     */
    fun shareBitmap(context: Context, bitmap: Bitmap, text: String? = null): ShareResult {
        return try {
            val file = File(context.cacheDir, "share_image_${System.currentTimeMillis()}.png")
            FileOutputStream(file).use { out ->
                bitmap.compress(Bitmap.CompressFormat.PNG, 100, out)
            }
            
            val uri = FileProvider.getUriForFile(
                context,
                "${context.packageName}.fileprovider",
                file
            )
            
            shareImage(context, uri, text)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    /**
     * مشاركة عدة صور / Share multiple images
     */
    fun shareMultipleImages(context: Context, imageUris: List<Uri>, text: String? = null): ShareResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
                type = "image/*"
                putParcelableArrayListExtra(Intent.EXTRA_STREAM, ArrayList(imageUris))
                text?.let { putExtra(Intent.EXTRA_TEXT, it) }
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(Intent.createChooser(intent, "مشاركة صور").apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            })
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مشاركة الملفات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مشاركة ملف / Share file
     */
    fun shareFile(context: Context, file: File, mimeType: String = "*/*"): ShareResult {
        return try {
            val uri = FileProvider.getUriForFile(
                context,
                "${context.packageName}.fileprovider",
                file
            )
            shareFileUri(context, uri, mimeType)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    /**
     * مشاركة ملف بـ Uri / Share file by Uri
     */
    fun shareFileUri(context: Context, uri: Uri, mimeType: String = "*/*"): ShareResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = mimeType
                putExtra(Intent.EXTRA_STREAM, uri)
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(Intent.createChooser(intent, "مشاركة ملف").apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            })
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    /**
     * مشاركة عدة ملفات / Share multiple files
     */
    fun shareMultipleFiles(context: Context, files: List<File>, mimeType: String = "*/*"): ShareResult {
        return try {
            val uris = files.map { file ->
                FileProvider.getUriForFile(
                    context,
                    "${context.packageName}.fileprovider",
                    file
                )
            }
            
            val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
                type = mimeType
                putParcelableArrayListExtra(Intent.EXTRA_STREAM, ArrayList(uris))
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(Intent.createChooser(intent, "مشاركة ملفات").apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            })
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مشاركة التطبيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مشاركة رابط التطبيق / Share app link
     */
    fun shareApp(context: Context, customMessage: String? = null): ShareResult {
        val packageName = context.packageName
        val playStoreUrl = "https://play.google.com/store/apps/details?id=$packageName"
        
        val message = customMessage ?: "جرّب هذا التطبيق الرائع!"
        return shareText(context, "$message\n\n$playStoreUrl", "مشاركة التطبيق")
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المشاركة لتطبيق محدد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مشاركة لتطبيق محدد / Share to specific app
     */
    fun shareToApp(
        context: Context,
        packageName: String,
        text: String
    ): ShareResult {
        return try {
            val intent = Intent(Intent.ACTION_SEND).apply {
                type = "text/plain"
                putExtra(Intent.EXTRA_TEXT, text)
                setPackage(packageName)
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
            context.startActivity(intent)
            ShareResult(true)
        } catch (e: Exception) {
            ShareResult(false, e.message)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أنواع MIME
    // ═══════════════════════════════════════════════════════════════
    
    object MimeTypes {
        const val TEXT = "text/plain"
        const val HTML = "text/html"
        const val IMAGE = "image/*"
        const val IMAGE_PNG = "image/png"
        const val IMAGE_JPEG = "image/jpeg"
        const val VIDEO = "video/*"
        const val AUDIO = "audio/*"
        const val PDF = "application/pdf"
        const val ZIP = "application/zip"
        const val ALL = "*/*"
    }
}
