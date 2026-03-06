// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadSmsBridge.kt
// الوصف: جسر Kotlin للرسائل القصيرة SMS
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.provider.Telephony
import android.telephony.SmsManager
import android.telephony.SmsMessage
import androidx.core.content.ContextCompat
import java.util.Date

/**
 * جسر الرسائل القصيرة — SadSmsBridge
 */
object SadSmsBridge {
    
    private var context: Context? = null
    private var smsReceiver: BroadcastReceiver? = null
    private var onSmsReceivedCallback: ((String, String, Long) -> Unit)? = null
    private var onSmsSentCallback: ((Boolean, String?) -> Unit)? = null
    
    private const val ACTION_SMS_SENT = "com.sad.bridges.SMS_SENT"
    private const val ACTION_SMS_DELIVERED = "com.sad.bridges.SMS_DELIVERED"
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Permission Check
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun hasSmsSendPermission(): Boolean {
        val ctx = context ?: return false
        return ContextCompat.checkSelfPermission(ctx, Manifest.permission.SEND_SMS) == 
            PackageManager.PERMISSION_GRANTED
    }
    
    @JvmStatic
    fun hasSmsReadPermission(): Boolean {
        val ctx = context ?: return false
        return ContextCompat.checkSelfPermission(ctx, Manifest.permission.READ_SMS) == 
            PackageManager.PERMISSION_GRANTED
    }
    
    @JvmStatic
    fun hasSmsReceivePermission(): Boolean {
        val ctx = context ?: return false
        return ContextCompat.checkSelfPermission(ctx, Manifest.permission.RECEIVE_SMS) == 
            PackageManager.PERMISSION_GRANTED
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Send SMS
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Suppress("DEPRECATION")
    fun sendSms(phoneNumber: String, message: String): Map<String, Any?> {
        val ctx = context ?: return mapOf(
            "success" to false,
            "error" to "السياق غير متاح"
        )
        
        if (!hasSmsSendPermission()) {
            return mapOf(
                "success" to false,
                "error" to "لا يوجد إذن لإرسال الرسائل"
            )
        }
        
        return try {
            val smsManager = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                ctx.getSystemService(SmsManager::class.java)
            } else {
                SmsManager.getDefault()
            }
            
            smsManager.sendTextMessage(phoneNumber, null, message, null, null)
            
            mapOf(
                "success" to true,
                "phoneNumber" to phoneNumber,
                "messageLength" to message.length
            )
        } catch (e: Exception) {
            mapOf(
                "success" to false,
                "error" to "فشل إرسال الرسالة: ${e.message}"
            )
        }
    }
    
    @JvmStatic
    @Suppress("DEPRECATION")
    fun sendSmsWithCallback(
        phoneNumber: String, 
        message: String,
        callback: (Boolean, String?) -> Unit
    ) {
        val ctx = context ?: run {
            callback(false, "السياق غير متاح")
            return
        }
        
        if (!hasSmsSendPermission()) {
            callback(false, "لا يوجد إذن لإرسال الرسائل")
            return
        }
        
        try {
            val sentPI = PendingIntent.getBroadcast(
                ctx, 0, Intent(ACTION_SMS_SENT),
                PendingIntent.FLAG_IMMUTABLE
            )
            
            val sentReceiver = object : BroadcastReceiver() {
                override fun onReceive(context: Context?, intent: Intent?) {
                    when (resultCode) {
                        android.app.Activity.RESULT_OK -> callback(true, null)
                        SmsManager.RESULT_ERROR_GENERIC_FAILURE -> callback(false, "خطأ عام")
                        SmsManager.RESULT_ERROR_NO_SERVICE -> callback(false, "لا توجد خدمة")
                        SmsManager.RESULT_ERROR_NULL_PDU -> callback(false, "خطأ PDU")
                        SmsManager.RESULT_ERROR_RADIO_OFF -> callback(false, "الراديو مغلق")
                        else -> callback(false, "خطأ غير معروف")
                    }
                    ctx.unregisterReceiver(this)
                }
            }
            
            ctx.registerReceiver(sentReceiver, IntentFilter(ACTION_SMS_SENT))
            
            val smsManager = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                ctx.getSystemService(SmsManager::class.java)
            } else {
                SmsManager.getDefault()
            }
            
            smsManager.sendTextMessage(phoneNumber, null, message, sentPI, null)
            
        } catch (e: Exception) {
            callback(false, "فشل إرسال الرسالة: ${e.message}")
        }
    }
    
    @JvmStatic
    @Suppress("DEPRECATION")
    fun sendMultipartSms(phoneNumber: String, message: String): Map<String, Any?> {
        val ctx = context ?: return mapOf(
            "success" to false,
            "error" to "السياق غير متاح"
        )
        
        if (!hasSmsSendPermission()) {
            return mapOf(
                "success" to false,
                "error" to "لا يوجد إذن لإرسال الرسائل"
            )
        }
        
        return try {
            val smsManager = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                ctx.getSystemService(SmsManager::class.java)
            } else {
                SmsManager.getDefault()
            }
            
            val parts = smsManager.divideMessage(message)
            smsManager.sendMultipartTextMessage(phoneNumber, null, parts, null, null)
            
            mapOf(
                "success" to true,
                "phoneNumber" to phoneNumber,
                "totalParts" to parts.size
            )
        } catch (e: Exception) {
            mapOf(
                "success" to false,
                "error" to "فشل إرسال الرسالة: ${e.message}"
            )
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Open SMS App
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun openSmsApp(phoneNumber: String?, message: String?): Boolean {
        val ctx = context ?: return false
        
        return try {
            val intent = Intent(Intent.ACTION_SENDTO).apply {
                data = Uri.parse("smsto:${phoneNumber ?: ""}")
                message?.let { putExtra("sms_body", it) }
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            ctx.startActivity(intent)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Read SMS
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getInboxMessages(limit: Int = 50): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        
        if (!hasSmsReadPermission()) {
            return emptyList()
        }
        
        val messages = mutableListOf<Map<String, Any?>>()
        
        try {
            val cursor = ctx.contentResolver.query(
                Telephony.Sms.Inbox.CONTENT_URI,
                arrayOf(
                    Telephony.Sms._ID,
                    Telephony.Sms.ADDRESS,
                    Telephony.Sms.BODY,
                    Telephony.Sms.DATE,
                    Telephony.Sms.READ
                ),
                null,
                null,
                "${Telephony.Sms.DATE} DESC"
            )
            
            cursor?.use {
                var count = 0
                while (it.moveToNext() && count < limit) {
                    val id = it.getLong(it.getColumnIndexOrThrow(Telephony.Sms._ID))
                    val address = it.getString(it.getColumnIndexOrThrow(Telephony.Sms.ADDRESS))
                    val body = it.getString(it.getColumnIndexOrThrow(Telephony.Sms.BODY))
                    val date = it.getLong(it.getColumnIndexOrThrow(Telephony.Sms.DATE))
                    val read = it.getInt(it.getColumnIndexOrThrow(Telephony.Sms.READ))
                    
                    messages.add(mapOf(
                        "id" to id,
                        "address" to address,
                        "body" to body,
                        "date" to date,
                        "dateFormatted" to Date(date).toString(),
                        "read" to (read == 1),
                        "type" to "inbox"
                    ))
                    count++
                }
            }
        } catch (e: Exception) {
            // Ignore errors
        }
        
        return messages
    }
    
    @JvmStatic
    fun getSentMessages(limit: Int = 50): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        
        if (!hasSmsReadPermission()) {
            return emptyList()
        }
        
        val messages = mutableListOf<Map<String, Any?>>()
        
        try {
            val cursor = ctx.contentResolver.query(
                Telephony.Sms.Sent.CONTENT_URI,
                arrayOf(
                    Telephony.Sms._ID,
                    Telephony.Sms.ADDRESS,
                    Telephony.Sms.BODY,
                    Telephony.Sms.DATE
                ),
                null,
                null,
                "${Telephony.Sms.DATE} DESC"
            )
            
            cursor?.use {
                var count = 0
                while (it.moveToNext() && count < limit) {
                    val id = it.getLong(it.getColumnIndexOrThrow(Telephony.Sms._ID))
                    val address = it.getString(it.getColumnIndexOrThrow(Telephony.Sms.ADDRESS))
                    val body = it.getString(it.getColumnIndexOrThrow(Telephony.Sms.BODY))
                    val date = it.getLong(it.getColumnIndexOrThrow(Telephony.Sms.DATE))
                    
                    messages.add(mapOf(
                        "id" to id,
                        "address" to address,
                        "body" to body,
                        "date" to date,
                        "dateFormatted" to Date(date).toString(),
                        "type" to "sent"
                    ))
                    count++
                }
            }
        } catch (e: Exception) {
            // Ignore errors
        }
        
        return messages
    }
    
    @JvmStatic
    fun searchMessages(query: String, limit: Int = 50): List<Map<String, Any?>> {
        val ctx = context ?: return emptyList()
        
        if (!hasSmsReadPermission()) {
            return emptyList()
        }
        
        val messages = mutableListOf<Map<String, Any?>>()
        
        try {
            val cursor = ctx.contentResolver.query(
                Telephony.Sms.CONTENT_URI,
                arrayOf(
                    Telephony.Sms._ID,
                    Telephony.Sms.ADDRESS,
                    Telephony.Sms.BODY,
                    Telephony.Sms.DATE,
                    Telephony.Sms.TYPE
                ),
                "${Telephony.Sms.BODY} LIKE ? OR ${Telephony.Sms.ADDRESS} LIKE ?",
                arrayOf("%$query%", "%$query%"),
                "${Telephony.Sms.DATE} DESC"
            )
            
            cursor?.use {
                var count = 0
                while (it.moveToNext() && count < limit) {
                    val id = it.getLong(it.getColumnIndexOrThrow(Telephony.Sms._ID))
                    val address = it.getString(it.getColumnIndexOrThrow(Telephony.Sms.ADDRESS))
                    val body = it.getString(it.getColumnIndexOrThrow(Telephony.Sms.BODY))
                    val date = it.getLong(it.getColumnIndexOrThrow(Telephony.Sms.DATE))
                    val type = it.getInt(it.getColumnIndexOrThrow(Telephony.Sms.TYPE))
                    
                    messages.add(mapOf(
                        "id" to id,
                        "address" to address,
                        "body" to body,
                        "date" to date,
                        "dateFormatted" to Date(date).toString(),
                        "type" to getSmsTypeLabel(type)
                    ))
                    count++
                }
            }
        } catch (e: Exception) {
            // Ignore errors
        }
        
        return messages
    }
    
    private fun getSmsTypeLabel(type: Int): String {
        return when (type) {
            Telephony.Sms.MESSAGE_TYPE_INBOX -> "inbox"
            Telephony.Sms.MESSAGE_TYPE_SENT -> "sent"
            Telephony.Sms.MESSAGE_TYPE_DRAFT -> "draft"
            Telephony.Sms.MESSAGE_TYPE_OUTBOX -> "outbox"
            Telephony.Sms.MESSAGE_TYPE_FAILED -> "failed"
            Telephony.Sms.MESSAGE_TYPE_QUEUED -> "queued"
            else -> "unknown"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // SMS Receiver
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun startSmsListener(callback: (String, String, Long) -> Unit) {
        val ctx = context ?: return
        
        if (!hasSmsReceivePermission()) {
            return
        }
        
        onSmsReceivedCallback = callback
        
        smsReceiver = object : BroadcastReceiver() {
            override fun onReceive(context: Context?, intent: Intent?) {
                if (intent?.action == Telephony.Sms.Intents.SMS_RECEIVED_ACTION) {
                    val bundle = intent.extras ?: return
                    
                    val pdus = bundle.get("pdus") as? Array<*> ?: return
                    val format = bundle.getString("format") ?: return
                    
                    for (pdu in pdus) {
                        val smsMessage = SmsMessage.createFromPdu(pdu as ByteArray, format)
                        val sender = smsMessage.displayOriginatingAddress
                        val body = smsMessage.messageBody
                        val timestamp = smsMessage.timestampMillis
                        
                        callback(sender, body, timestamp)
                    }
                }
            }
        }
        
        val filter = IntentFilter(Telephony.Sms.Intents.SMS_RECEIVED_ACTION)
        filter.priority = IntentFilter.SYSTEM_HIGH_PRIORITY
        ctx.registerReceiver(smsReceiver, filter)
    }
    
    @JvmStatic
    fun stopSmsListener() {
        val ctx = context ?: return
        
        smsReceiver?.let {
            try {
                ctx.unregisterReceiver(it)
            } catch (e: Exception) {
                // Already unregistered
            }
        }
        smsReceiver = null
        onSmsReceivedCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getMessageCount(): Map<String, Int> {
        val ctx = context ?: return emptyMap()
        
        if (!hasSmsReadPermission()) {
            return emptyMap()
        }
        
        var inbox = 0
        var sent = 0
        
        try {
            ctx.contentResolver.query(
                Telephony.Sms.Inbox.CONTENT_URI,
                arrayOf("count(*) AS count"),
                null, null, null
            )?.use {
                if (it.moveToFirst()) {
                    inbox = it.getInt(0)
                }
            }
            
            ctx.contentResolver.query(
                Telephony.Sms.Sent.CONTENT_URI,
                arrayOf("count(*) AS count"),
                null, null, null
            )?.use {
                if (it.moveToFirst()) {
                    sent = it.getInt(0)
                }
            }
        } catch (e: Exception) {
            // Ignore
        }
        
        return mapOf(
            "inbox" to inbox,
            "sent" to sent,
            "total" to (inbox + sent)
        )
    }
    
    @JvmStatic
    fun getDefaultSmsApp(): String? {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            Telephony.Sms.getDefaultSmsPackage(context)
        } else {
            null
        }
    }
    
    @JvmStatic
    fun isDefaultSmsApp(): Boolean {
        val ctx = context ?: return false
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            ctx.packageName == Telephony.Sms.getDefaultSmsPackage(ctx)
        } else {
            false
        }
    }
}
