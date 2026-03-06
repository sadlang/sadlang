package sad.android.communication.mms

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.provider.Telephony

/**
 * جسر رسائل الوسائط المتعددة MMS
 * MMS Bridge - إرسال رسائل الوسائط
 * 
 * @author فريق لغة ص
 */
object SadMmsBridge {
    
    /**
     * بيانات MMS / MMS data
     */
    data class MmsData(
        val recipients: List<String>,
        val subject: String = "",
        val body: String = "",
        val attachments: List<Uri> = emptyList()
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال MMS
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال MMS مع صورة / Send MMS with image
     */
    fun sendWithImage(
        context: Context,
        phoneNumber: String,
        message: String,
        imageUri: Uri
    ) {
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "image/*"
            putExtra("address", phoneNumber)
            putExtra("sms_body", message)
            putExtra(Intent.EXTRA_STREAM, imageUri)
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    /**
     * إرسال MMS مع فيديو / Send MMS with video
     */
    fun sendWithVideo(
        context: Context,
        phoneNumber: String,
        message: String,
        videoUri: Uri
    ) {
        val intent = Intent(Intent.ACTION_SEND).apply {
            type = "video/*"
            putExtra("address", phoneNumber)
            putExtra("sms_body", message)
            putExtra(Intent.EXTRA_STREAM, videoUri)
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    /**
     * إرسال MMS كامل / Send full MMS
     */
    fun send(context: Context, mms: MmsData) {
        val intent = Intent(Intent.ACTION_SEND_MULTIPLE).apply {
            type = "*/*"
            putExtra("address", mms.recipients.joinToString(";"))
            putExtra("subject", mms.subject)
            putExtra("sms_body", mms.body)
            
            if (mms.attachments.isNotEmpty()) {
                putParcelableArrayListExtra(Intent.EXTRA_STREAM, ArrayList(mms.attachments))
            }
            
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    /**
     * فتح تطبيق الرسائل الافتراضي / Open default messaging app
     */
    fun openMessagingApp(context: Context, phoneNumber: String? = null, body: String? = null) {
        val uri = if (phoneNumber != null) {
            Uri.parse("smsto:$phoneNumber")
        } else {
            Uri.parse("smsto:")
        }
        
        val intent = Intent(Intent.ACTION_SENDTO, uri).apply {
            body?.let { putExtra("sms_body", it) }
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    /**
     * الحصول على اسم حزمة تطبيق الرسائل الافتراضي / Get default SMS app
     */
    fun getDefaultSmsApp(context: Context): String? {
        return Telephony.Sms.getDefaultSmsPackage(context)
    }
}
