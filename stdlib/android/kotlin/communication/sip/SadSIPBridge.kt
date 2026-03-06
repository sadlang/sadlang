package sad.android.communication.sip

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.net.sip.*
import android.os.Build
import androidx.core.content.ContextCompat

/**
 * جسر SIP - بروتوكول بدء الجلسة
 * SIP Bridge - Session Initiation Protocol
 * 
 * @author فريق لغة ص
 */
@Suppress("DEPRECATION")
object SadSIPBridge {
    
    private var sipManager: SipManager? = null
    private var sipProfile: SipProfile? = null
    private var sipAudioCall: SipAudioCall? = null
    
    /**
     * حالة التسجيل / Registration state
     */
    enum class RegistrationState {
        NOT_REGISTERED,     // غير مسجل
        REGISTERING,        // جاري التسجيل
        REGISTERED,         // مسجل
        FAILED              // فشل
    }
    
    /**
     * حالة المكالمة / Call state
     */
    enum class SipCallState {
        IDLE,               // خامل
        CALLING,            // جاري الاتصال
        INCOMING,           // وارد
        RINGING,            // رنين
        ESTABLISHED,        // متصل
        ENDED,              // انتهت
        BUSY,               // مشغول
        ERROR               // خطأ
    }
    
    /**
     * بيانات حساب SIP / SIP account credentials
     */
    data class SipAccount(
        val username: String,
        val password: String,
        val domain: String,
        val displayName: String? = null,
        val port: Int = 5060,
        val protocol: String = "UDP",    // UDP, TCP, or TLS
        val autoRegistration: Boolean = true,
        val receiveCallsInBackground: Boolean = true
    )
    
    /**
     * مستمع SIP / SIP listener
     */
    interface SipListener {
        fun onRegistrationState(state: RegistrationState, message: String?)
        fun onCallState(state: SipCallState, message: String?)
        fun onIncomingCall(callerUri: String)
        fun onError(errorCode: Int, message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل SIP مدعوم؟ / Is SIP supported?
     */
    fun isSupported(context: Context): Boolean {
        return SipManager.isApiSupported(context) && SipManager.isVoipSupported(context)
    }
    
    /**
     * تهيئة SIP / Initialize SIP
     */
    fun initialize(context: Context): Boolean {
        if (!isSupported(context)) return false
        sipManager = SipManager.newInstance(context)
        return sipManager != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التسجيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل حساب SIP / Register SIP account
     */
    fun register(
        context: Context,
        account: SipAccount,
        listener: SipListener
    ): Boolean {
        if (!hasPermissions(context)) {
            listener.onError(-1, "لا توجد صلاحية")
            return false
        }
        
        val manager = sipManager ?: SipManager.newInstance(context) ?: run {
            listener.onError(-2, "فشل إنشاء SipManager")
            return false
        }
        sipManager = manager
        
        try {
            // إنشاء ملف SIP
            val builder = SipProfile.Builder(account.username, account.domain).apply {
                setPassword(account.password)
                setPort(account.port)
                account.displayName?.let { setDisplayName(it) }
                setAutoRegistration(account.autoRegistration)
                
                when (account.protocol.uppercase()) {
                    "TCP" -> setProtocol("TCP")
                    "TLS" -> setProtocol("TLS")
                    else -> setProtocol("UDP")
                }
            }
            
            sipProfile = builder.build()
            
            // تسجيل المستمع
            val registrationListener = object : SipRegistrationListener {
                override fun onRegistering(localProfileUri: String?) {
                    listener.onRegistrationState(RegistrationState.REGISTERING, null)
                }
                
                override fun onRegistrationDone(localProfileUri: String?, expiryTime: Long) {
                    listener.onRegistrationState(RegistrationState.REGISTERED, "Expires: $expiryTime")
                }
                
                override fun onRegistrationFailed(
                    localProfileUri: String?,
                    errorCode: Int,
                    errorMessage: String?
                ) {
                    listener.onRegistrationState(RegistrationState.FAILED, errorMessage)
                    listener.onError(errorCode, errorMessage ?: "فشل التسجيل")
                }
            }
            
            // فتح للمكالمات الواردة
            if (account.receiveCallsInBackground) {
                manager.open(sipProfile, pendingIntentForIncomingCalls(context), registrationListener)
            } else {
                manager.open(sipProfile)
                manager.setRegistrationListener(sipProfile?.uriString, registrationListener)
            }
            
            return true
        } catch (e: Exception) {
            listener.onError(-3, e.message ?: "خطأ غير معروف")
            return false
        }
    }
    
    /**
     * إلغاء التسجيل / Unregister
     */
    fun unregister(): Boolean {
        return try {
            sipProfile?.let {
                sipManager?.close(it.uriString)
            }
            sipProfile = null
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المكالمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إجراء مكالمة / Make call
     */
    fun makeCall(sipUri: String, listener: SipListener): Boolean {
        val profile = sipProfile ?: return false
        val manager = sipManager ?: return false
        
        return try {
            sipAudioCall = manager.makeAudioCall(
                profile.uriString,
                sipUri,
                createCallListener(listener),
                30 // timeout
            )
            true
        } catch (e: Exception) {
            listener.onError(-1, e.message ?: "فشل الاتصال")
            false
        }
    }
    
    /**
     * الرد على مكالمة / Answer call
     */
    fun answerCall(timeout: Int = 30): Boolean {
        return try {
            sipAudioCall?.answerCall(timeout)
            sipAudioCall?.startAudio()
            sipAudioCall?.setSpeakerMode(false)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إنهاء المكالمة / End call
     */
    fun endCall(): Boolean {
        return try {
            sipAudioCall?.endCall()
            sipAudioCall?.close()
            sipAudioCall = null
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * كتم الصوت / Mute
     */
    fun setMute(muted: Boolean) {
        sipAudioCall?.toggleMute()
    }
    
    /**
     * تفعيل مكبر الصوت / Enable speaker
     */
    fun setSpeaker(enabled: Boolean) {
        sipAudioCall?.setSpeakerMode(enabled)
    }
    
    /**
     * إرسال DTMF / Send DTMF
     */
    fun sendDtmf(code: Int, duration: Int = 100) {
        sipAudioCall?.sendDtmf(code, null)
    }
    
    /**
     * هل في مكالمة؟ / Is in call?
     */
    fun isInCall(): Boolean {
        return sipAudioCall?.isInCall == true
    }
    
    /**
     * هل المكالمة قيد الانتظار؟ / Is on hold?
     */
    fun isOnHold(): Boolean {
        return sipAudioCall?.isOnHold == true
    }
    
    /**
     * وضع المكالمة قيد الانتظار / Hold call
     */
    fun holdCall(): Boolean {
        return try {
            sipAudioCall?.holdCall(30)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * استئناف المكالمة / Continue call
     */
    fun resumeCall(): Boolean {
        return try {
            sipAudioCall?.continueCall(30)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة المكالمات الواردة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة المكالمة الواردة / Handle incoming call
     */
    fun handleIncomingCall(
        intent: android.content.Intent,
        listener: SipListener
    ): Boolean {
        val manager = sipManager ?: return false
        
        return try {
            sipAudioCall = manager.takeAudioCall(intent, createCallListener(listener))
            true
        } catch (e: Exception) {
            listener.onError(-1, e.message ?: "فشل معالجة المكالمة")
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    fun hasPermissions(context: Context): Boolean {
        return requiredPermissions().all {
            ContextCompat.checkSelfPermission(context, it) == PackageManager.PERMISSION_GRANTED
        }
    }
    
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.USE_SIP,
        Manifest.permission.INTERNET,
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.ACCESS_WIFI_STATE,
        Manifest.permission.WAKE_LOCK
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنظيف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إغلاق كل شيء / Close everything
     */
    fun close() {
        try {
            sipAudioCall?.close()
            sipProfile?.let { sipManager?.close(it.uriString) }
        } catch (e: Exception) {}
        
        sipAudioCall = null
        sipProfile = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun createCallListener(listener: SipListener): SipAudioCall.Listener {
        return object : SipAudioCall.Listener() {
            override fun onCalling(call: SipAudioCall?) {
                listener.onCallState(SipCallState.CALLING, null)
            }
            
            override fun onRinging(call: SipAudioCall?, caller: SipProfile?) {
                listener.onCallState(SipCallState.RINGING, null)
            }
            
            override fun onRingingBack(call: SipAudioCall?) {
                listener.onCallState(SipCallState.RINGING, "Ringing back")
            }
            
            override fun onCallEstablished(call: SipAudioCall?) {
                call?.startAudio()
                call?.setSpeakerMode(false)
                listener.onCallState(SipCallState.ESTABLISHED, null)
            }
            
            override fun onCallEnded(call: SipAudioCall?) {
                listener.onCallState(SipCallState.ENDED, null)
            }
            
            override fun onCallBusy(call: SipAudioCall?) {
                listener.onCallState(SipCallState.BUSY, null)
            }
            
            override fun onError(call: SipAudioCall?, errorCode: Int, errorMessage: String?) {
                listener.onCallState(SipCallState.ERROR, errorMessage)
                listener.onError(errorCode, errorMessage ?: "خطأ في المكالمة")
            }
            
            override fun onReadyToCall(call: SipAudioCall?) {
                listener.onCallState(SipCallState.IDLE, "Ready")
            }
        }
    }
    
    private fun pendingIntentForIncomingCalls(context: Context): android.app.PendingIntent {
        val intent = android.content.Intent("sad.sip.INCOMING_CALL")
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            android.app.PendingIntent.FLAG_UPDATE_CURRENT or android.app.PendingIntent.FLAG_MUTABLE
        } else {
            android.app.PendingIntent.FLAG_UPDATE_CURRENT
        }
        return android.app.PendingIntent.getBroadcast(context, 0, intent, flags)
    }
}
