package sad.android.communication.call

import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.telecom.TelecomManager
import android.telephony.PhoneStateListener
import android.telephony.TelephonyCallback
import android.telephony.TelephonyManager
import androidx.annotation.RequiresApi
import androidx.core.content.ContextCompat

/**
 * جسر المكالمات الهاتفية
 * Phone Call Bridge - إجراء واستقبال المكالمات
 * 
 * @author فريق لغة ص
 */
object SadCallBridge {
    
    private var telephonyManager: TelephonyManager? = null
    private var telecomManager: TelecomManager? = null
    
    /**
     * حالة المكالمة / Call state
     */
    enum class CallState {
        IDLE,       // خامل
        RINGING,    // رنين
        OFFHOOK,    // مشغول
        CONNECTING, // جاري الاتصال
        ACTIVE,     // نشط
        HOLDING,    // انتظار
        DISCONNECTED // منتهي
    }
    
    /**
     * بيانات المكالمة / Call data
     */
    data class CallInfo(
        val phoneNumber: String,
        val state: CallState,
        val duration: Long = 0,
        val isOutgoing: Boolean = true,
        val timestamp: Long = System.currentTimeMillis()
    )
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(context: Context) {
        telephonyManager = context.getSystemService(Context.TELEPHONY_SERVICE) as? TelephonyManager
        telecomManager = context.getSystemService(Context.TELECOM_SERVICE) as? TelecomManager
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إجراء مكالمة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إجراء مكالمة / Make call
     * اتصل("0501234567")
     */
    fun makeCall(context: Context, phoneNumber: String) {
        val intent = Intent(Intent.ACTION_CALL).apply {
            data = Uri.parse("tel:$phoneNumber")
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        
        if (hasCallPermission(context)) {
            context.startActivity(intent)
        }
    }
    
    /**
     * فتح برنامج الاتصال / Open dialer
     */
    fun openDialer(context: Context, phoneNumber: String? = null) {
        val intent = Intent(Intent.ACTION_DIAL).apply {
            phoneNumber?.let { data = Uri.parse("tel:$it") }
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    /**
     * إجراء مكالمة طوارئ / Emergency call
     */
    fun emergencyCall(context: Context, number: String = "911") {
        val intent = Intent(Intent.ACTION_CALL).apply {
            data = Uri.parse("tel:$number")
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة المكالمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنهاء المكالمة الحالية / End current call
     */
    @RequiresApi(Build.VERSION_CODES.P)
    fun endCall(context: Context): Boolean {
        return try {
            telecomManager?.endCall() ?: false
        } catch (e: SecurityException) {
            false
        }
    }
    
    /**
     * الرد على مكالمة واردة / Answer incoming call
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun answerCall(context: Context) {
        try {
            telecomManager?.acceptRingingCall()
        } catch (e: SecurityException) {
            // لا يوجد إذن
        }
    }
    
    /**
     * رفض مكالمة واردة / Reject incoming call
     */
    @RequiresApi(Build.VERSION_CODES.P)
    fun rejectCall(context: Context): Boolean {
        return endCall(context)
    }
    
    /**
     * كتم المكالمة / Mute call
     */
    fun muteCall(context: Context, mute: Boolean) {
        // يحتاج InCallService
    }
    
    /**
     * تشغيل مكبر الصوت / Toggle speaker
     */
    fun toggleSpeaker(context: Context, enabled: Boolean) {
        // يحتاج AudioManager
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مراقبة حالة المكالمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على حالة المكالمة الحالية / Get current call state
     */
    fun getCurrentState(): CallState {
        return when (telephonyManager?.callState) {
            TelephonyManager.CALL_STATE_IDLE -> CallState.IDLE
            TelephonyManager.CALL_STATE_RINGING -> CallState.RINGING
            TelephonyManager.CALL_STATE_OFFHOOK -> CallState.OFFHOOK
            else -> CallState.IDLE
        }
    }
    
    /**
     * مراقبة حالة المكالمات / Listen for call state changes
     */
    @RequiresApi(Build.VERSION_CODES.S)
    fun startListening(
        context: Context,
        onStateChange: (CallState, String?) -> Unit
    ) {
        telephonyManager?.registerTelephonyCallback(
            context.mainExecutor,
            object : TelephonyCallback(), TelephonyCallback.CallStateListener {
                override fun onCallStateChanged(state: Int) {
                    val callState = when (state) {
                        TelephonyManager.CALL_STATE_IDLE -> CallState.IDLE
                        TelephonyManager.CALL_STATE_RINGING -> CallState.RINGING
                        TelephonyManager.CALL_STATE_OFFHOOK -> CallState.OFFHOOK
                        else -> CallState.IDLE
                    }
                    onStateChange(callState, null)
                }
            }
        )
    }
    
    /**
     * مراقبة للإصدارات القديمة / Legacy listener
     */
    @Suppress("DEPRECATION")
    fun startListeningLegacy(onStateChange: (CallState, String?) -> Unit) {
        telephonyManager?.listen(
            object : PhoneStateListener() {
                override fun onCallStateChanged(state: Int, phoneNumber: String?) {
                    val callState = when (state) {
                        TelephonyManager.CALL_STATE_IDLE -> CallState.IDLE
                        TelephonyManager.CALL_STATE_RINGING -> CallState.RINGING
                        TelephonyManager.CALL_STATE_OFFHOOK -> CallState.OFFHOOK
                        else -> CallState.IDLE
                    }
                    onStateChange(callState, phoneNumber)
                }
            },
            PhoneStateListener.LISTEN_CALL_STATE
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * التحقق من إذن الاتصال / Check call permission
     */
    fun hasCallPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.CALL_PHONE
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    /**
     * الأذونات المطلوبة / Required permissions
     */
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.CALL_PHONE,
        Manifest.permission.READ_PHONE_STATE,
        Manifest.permission.ANSWER_PHONE_CALLS
    )
}
