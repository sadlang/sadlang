package sad.android.communication.sms

import android.Manifest
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.telephony.SmsManager
import android.telephony.SmsMessage
import androidx.core.content.ContextCompat

/**
 * جسر الرسائل النصية SMS
 * SMS Bridge - إرسال واستقبال الرسائل النصية
 * 
 * @author فريق لغة ص
 */
object SadSmsBridge {
    
    private var smsManager: SmsManager? = null
    private var onReceiveCallback: ((SmsData) -> Unit)? = null
    private var smsReceiver: BroadcastReceiver? = null
    
    /**
     * بيانات الرسالة / Message data
     */
    data class SmsData(
        val sender: String,           // المرسل
        val body: String,             // المحتوى
        val timestamp: Long,          // الوقت
        val isRead: Boolean = false   // مقروءة
    )
    
    /**
     * نتيجة الإرسال / Send result
     */
    data class SendResult(
        val success: Boolean,
        val messageId: String? = null,
        val error: String? = null
    )
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(context: Context) {
        smsManager = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            context.getSystemService(SmsManager::class.java)
        } else {
            @Suppress("DEPRECATION")
            SmsManager.getDefault()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إرسال الرسائل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال رسالة نصية / Send SMS
     * أرسل("0501234567", "مرحباً من لغة ص!")
     */
    fun send(
        context: Context,
        phoneNumber: String,
        message: String,
        onResult: (SendResult) -> Unit
    ) {
        if (!hasPermission(context)) {
            onResult(SendResult(false, error = "لا يوجد إذن إرسال SMS"))
            return
        }
        
        try {
            val sentPI = PendingIntent.getBroadcast(
                context, 0,
                Intent("SMS_SENT"),
                PendingIntent.FLAG_IMMUTABLE
            )
            
            val deliveredPI = PendingIntent.getBroadcast(
                context, 0,
                Intent("SMS_DELIVERED"),
                PendingIntent.FLAG_IMMUTABLE
            )
            
            smsManager?.sendTextMessage(
                phoneNumber,
                null,
                message,
                sentPI,
                deliveredPI
            )
            
            onResult(SendResult(true, messageId = System.currentTimeMillis().toString()))
        } catch (e: Exception) {
            onResult(SendResult(false, error = e.message))
        }
    }
    
    /**
     * إرسال رسالة طويلة (متعددة الأجزاء)
     * Send long message (multipart)
     */
    fun sendLong(
        context: Context,
        phoneNumber: String,
        message: String,
        onResult: (SendResult) -> Unit
    ) {
        if (!hasPermission(context)) {
            onResult(SendResult(false, error = "لا يوجد إذن إرسال SMS"))
            return
        }
        
        try {
            val parts = smsManager?.divideMessage(message) ?: return
            smsManager?.sendMultipartTextMessage(
                phoneNumber,
                null,
                parts,
                null,
                null
            )
            onResult(SendResult(true))
        } catch (e: Exception) {
            onResult(SendResult(false, error = e.message))
        }
    }
    
    /**
     * إرسال رسالة جماعية / Send bulk SMS
     */
    fun sendBulk(
        context: Context,
        phoneNumbers: List<String>,
        message: String,
        onProgress: (sent: Int, total: Int) -> Unit,
        onComplete: (List<SendResult>) -> Unit
    ) {
        val results = mutableListOf<SendResult>()
        phoneNumbers.forEachIndexed { index, number ->
            send(context, number, message) { result ->
                results.add(result)
                onProgress(index + 1, phoneNumbers.size)
                if (results.size == phoneNumbers.size) {
                    onComplete(results)
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         استقبال الرسائل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاستماع للرسائل الواردة / Listen for incoming SMS
     */
    fun startListening(context: Context, onReceive: (SmsData) -> Unit) {
        onReceiveCallback = onReceive
        
        smsReceiver = object : BroadcastReceiver() {
            override fun onReceive(ctx: Context?, intent: Intent?) {
                if (intent?.action == "android.provider.Telephony.SMS_RECEIVED") {
                    val bundle = intent.extras ?: return
                    val pdus = bundle.get("pdus") as? Array<*> ?: return
                    
                    pdus.forEach { pdu ->
                        val message = SmsMessage.createFromPdu(
                            pdu as ByteArray,
                            bundle.getString("format")
                        )
                        onReceiveCallback?.invoke(
                            SmsData(
                                sender = message.originatingAddress ?: "",
                                body = message.messageBody,
                                timestamp = message.timestampMillis
                            )
                        )
                    }
                }
            }
        }
        
        context.registerReceiver(
            smsReceiver,
            IntentFilter("android.provider.Telephony.SMS_RECEIVED")
        )
    }
    
    /**
     * إيقاف الاستماع / Stop listening
     */
    fun stopListening(context: Context) {
        smsReceiver?.let {
            context.unregisterReceiver(it)
            smsReceiver = null
        }
        onReceiveCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فتح تطبيق الرسائل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح تطبيق الرسائل / Open SMS app
     */
    fun openSmsApp(context: Context, phoneNumber: String? = null, body: String? = null) {
        val uri = if (phoneNumber != null) {
            Uri.parse("sms:$phoneNumber")
        } else {
            Uri.parse("sms:")
        }
        
        val intent = Intent(Intent.ACTION_SENDTO, uri).apply {
            body?.let { putExtra("sms_body", it) }
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * التحقق من الإذن / Check permission
     */
    fun hasPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.SEND_SMS
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    /**
     * الأذونات المطلوبة / Required permissions
     */
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.SEND_SMS,
        Manifest.permission.RECEIVE_SMS,
        Manifest.permission.READ_SMS
    )
}
