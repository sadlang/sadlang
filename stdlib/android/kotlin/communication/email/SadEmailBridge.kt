package sad.android.communication.email

import android.content.Context
import android.content.Intent
import android.net.Uri
import androidx.core.content.FileProvider
import java.io.File

/**
 * جسر البريد الإلكتروني
 * Email Bridge - إرسال رسائل البريد الإلكتروني
 * 
 * @author فريق لغة ص
 */
object SadEmailBridge {
    
    /**
     * بيانات البريد / Email data
     */
    data class EmailData(
        val to: List<String>,
        val cc: List<String> = emptyList(),
        val bcc: List<String> = emptyList(),
        val subject: String = "",
        val body: String = "",
        val attachments: List<Uri> = emptyList(),
        val isHtml: Boolean = false
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال البريد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال بريد بسيط / Send simple email
     * أرسل_بريد("test@example.com", "عنوان", "محتوى")
     */
    fun send(
        context: Context,
        to: String,
        subject: String,
        body: String
    ) {
        send(context, EmailData(
            to = listOf(to),
            subject = subject,
            body = body
        ))
    }
    
    /**
     * إرسال بريد متقدم / Send advanced email
     */
    fun send(context: Context, email: EmailData) {
        val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
            type = if (email.isHtml) "text/html" else "text/plain"
            
            putExtra(Intent.EXTRA_EMAIL, email.to.toTypedArray())
            
            if (email.cc.isNotEmpty()) {
                putExtra(Intent.EXTRA_CC, email.cc.toTypedArray())
            }
            
            if (email.bcc.isNotEmpty()) {
                putExtra(Intent.EXTRA_BCC, email.bcc.toTypedArray())
            }
            
            putExtra(Intent.EXTRA_SUBJECT, email.subject)
            putExtra(Intent.EXTRA_TEXT, email.body)
            
            if (email.attachments.isNotEmpty()) {
                putParcelableArrayListExtra(
                    Intent.EXTRA_STREAM,
                    ArrayList(email.attachments)
                )
            }
            
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        
        context.startActivity(
            Intent.createChooser(intent, "إرسال عبر").apply {
                flags = Intent.FLAG_ACTIVITY_NEW_TASK
            }
        )
    }
    
    /**
     * إرسال بريد مع مرفق / Send email with attachment
     */
    fun sendWithAttachment(
        context: Context,
        to: String,
        subject: String,
        body: String,
        attachmentFile: File
    ) {
        val uri = FileProvider.getUriForFile(
            context,
            "${context.packageName}.fileprovider",
            attachmentFile
        )
        
        send(context, EmailData(
            to = listOf(to),
            subject = subject,
            body = body,
            attachments = listOf(uri)
        ))
    }
    
    /**
     * إرسال بريد جماعي / Send bulk email
     */
    fun sendBulk(
        context: Context,
        recipients: List<String>,
        subject: String,
        body: String
    ) {
        send(context, EmailData(
            to = recipients,
            subject = subject,
            body = body
        ))
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فتح تطبيق البريد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح تطبيق البريد / Open email app
     */
    fun openEmailApp(context: Context) {
        val intent = Intent(Intent.ACTION_MAIN).apply {
            addCategory(Intent.CATEGORY_APP_EMAIL)
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    /**
     * فتح Gmail / Open Gmail
     */
    fun openGmail(context: Context) {
        try {
            val intent = context.packageManager
                .getLaunchIntentForPackage("com.google.android.gm")
                ?.apply { flags = Intent.FLAG_ACTIVITY_NEW_TASK }
            
            if (intent != null) {
                context.startActivity(intent)
            } else {
                openEmailApp(context)
            }
        } catch (e: Exception) {
            openEmailApp(context)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء رابط mailto / Create mailto link
     */
    fun createMailtoUri(
        to: String,
        subject: String? = null,
        body: String? = null
    ): Uri {
        val builder = StringBuilder("mailto:$to")
        val params = mutableListOf<String>()
        
        subject?.let { params.add("subject=${Uri.encode(it)}") }
        body?.let { params.add("body=${Uri.encode(it)}") }
        
        if (params.isNotEmpty()) {
            builder.append("?${params.joinToString("&")}")
        }
        
        return Uri.parse(builder.toString())
    }
    
    /**
     * التحقق من صحة البريد / Validate email
     */
    fun isValidEmail(email: String): Boolean {
        return android.util.Patterns.EMAIL_ADDRESS.matcher(email).matches()
    }
    
    /**
     * التحقق من قائمة البريد / Validate email list
     */
    fun validateEmails(emails: List<String>): Pair<List<String>, List<String>> {
        val valid = mutableListOf<String>()
        val invalid = mutableListOf<String>()
        
        emails.forEach { email ->
            if (isValidEmail(email)) {
                valid.add(email)
            } else {
                invalid.add(email)
            }
        }
        
        return valid to invalid
    }
}
