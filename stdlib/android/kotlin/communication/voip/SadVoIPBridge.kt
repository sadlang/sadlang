package sad.android.communication.voip

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.media.AudioAttributes
import android.media.AudioDeviceInfo
import android.media.AudioFocusRequest
import android.media.AudioManager
import android.os.Build
import androidx.core.content.ContextCompat

/**
 * جسر VoIP للمكالمات الصوتية/المرئية عبر الإنترنت
 * VoIP Bridge - Voice/Video over IP calls
 * 
 * @author فريق لغة ص
 */
object SadVoIPBridge {
    
    private var audioManager: AudioManager? = null
    private var audioFocusRequest: AudioFocusRequest? = null
    
    /**
     * حالة المكالمة / Call state
     */
    enum class VoIPState {
        IDLE,           // خامل
        CONNECTING,     // جاري الاتصال
        RINGING,        // رنين
        CONNECTED,      // متصل
        ON_HOLD,        // انتظار
        RECONNECTING,   // إعادة الاتصال
        ENDED           // انتهت
    }
    
    /**
     * نوع المكالمة / Call type
     */
    enum class CallType {
        AUDIO,          // صوتي
        VIDEO,          // مرئي
        SCREEN_SHARE    // مشاركة شاشة
    }
    
    /**
     * جهاز الصوت / Audio device
     */
    enum class AudioDevice {
        EARPIECE,       // السماعة
        SPEAKER,        // مكبر الصوت
        BLUETOOTH,      // بلوتوث
        WIRED_HEADSET   // سماعة سلكية
    }
    
    /**
     * معلومات المكالمة / Call info
     */
    data class CallInfo(
        val callId: String,
        val remoteUserId: String,
        val remoteUserName: String,
        val type: CallType,
        val state: VoIPState,
        val startTime: Long = 0,
        val duration: Long = 0,
        val isMuted: Boolean = false,
        val isSpeakerOn: Boolean = false,
        val isVideoEnabled: Boolean = false
    )
    
    /**
     * إعدادات المكالمة / Call settings
     */
    data class CallSettings(
        val enableVideo: Boolean = false,
        val enableAEC: Boolean = true,      // Echo cancellation
        val enableNS: Boolean = true,       // Noise suppression
        val enableAGC: Boolean = true,      // Auto gain control
        val maxBitrate: Int = 128000,       // الحد الأقصى للـ bitrate
        val preferredCodec: String = "opus"
    )
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(context: Context) {
        audioManager = context.getSystemService(Context.AUDIO_SERVICE) as AudioManager
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الصوت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب تركيز الصوت / Request audio focus
     */
    fun requestAudioFocus(context: Context): Boolean {
        val am = getAudioManager(context)
        
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val attributes = AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_VOICE_COMMUNICATION)
                .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
                .build()
            
            audioFocusRequest = AudioFocusRequest.Builder(AudioManager.AUDIOFOCUS_GAIN_TRANSIENT)
                .setAudioAttributes(attributes)
                .setOnAudioFocusChangeListener { }
                .build()
            
            am.requestAudioFocus(audioFocusRequest!!) == AudioManager.AUDIOFOCUS_REQUEST_GRANTED
        } else {
            @Suppress("DEPRECATION")
            am.requestAudioFocus(
                null,
                AudioManager.STREAM_VOICE_CALL,
                AudioManager.AUDIOFOCUS_GAIN_TRANSIENT
            ) == AudioManager.AUDIOFOCUS_REQUEST_GRANTED
        }
    }
    
    /**
     * تحرير تركيز الصوت / Release audio focus
     */
    fun releaseAudioFocus(context: Context) {
        val am = getAudioManager(context)
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            audioFocusRequest?.let { am.abandonAudioFocusRequest(it) }
        } else {
            @Suppress("DEPRECATION")
            am.abandonAudioFocus(null)
        }
    }
    
    /**
     * تبديل مكبر الصوت / Toggle speaker
     */
    fun setSpeakerOn(context: Context, enabled: Boolean) {
        getAudioManager(context).isSpeakerphoneOn = enabled
    }
    
    /**
     * هل مكبر الصوت مفعل؟ / Is speaker on?
     */
    fun isSpeakerOn(context: Context): Boolean {
        return getAudioManager(context).isSpeakerphoneOn
    }
    
    /**
     * كتم الميكروفون / Mute microphone
     */
    fun setMicrophoneMute(context: Context, muted: Boolean) {
        getAudioManager(context).isMicrophoneMute = muted
    }
    
    /**
     * هل الميكروفون صامت؟ / Is mic muted?
     */
    fun isMicrophoneMuted(context: Context): Boolean {
        return getAudioManager(context).isMicrophoneMute
    }
    
    /**
     * ضبط مستوى الصوت / Set volume
     */
    fun setVolume(context: Context, volume: Int) {
        val am = getAudioManager(context)
        am.setStreamVolume(
            AudioManager.STREAM_VOICE_CALL,
            volume.coerceIn(0, am.getStreamMaxVolume(AudioManager.STREAM_VOICE_CALL)),
            0
        )
    }
    
    /**
     * جلب مستوى الصوت / Get volume
     */
    fun getVolume(context: Context): Int {
        return getAudioManager(context).getStreamVolume(AudioManager.STREAM_VOICE_CALL)
    }
    
    /**
     * جلب الحد الأقصى للصوت / Get max volume
     */
    fun getMaxVolume(context: Context): Int {
        return getAudioManager(context).getStreamMaxVolume(AudioManager.STREAM_VOICE_CALL)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أجهزة الصوت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * جلب أجهزة الصوت المتاحة / Get available audio devices
     */
    fun getAvailableAudioDevices(context: Context): List<AudioDevice> {
        val devices = mutableListOf<AudioDevice>()
        val am = getAudioManager(context)
        
        // السماعة دائماً متاحة
        devices.add(AudioDevice.EARPIECE)
        devices.add(AudioDevice.SPEAKER)
        
        // التحقق من البلوتوث
        if (am.isBluetoothScoAvailableOffCall) {
            devices.add(AudioDevice.BLUETOOTH)
        }
        
        // التحقق من السماعة السلكية
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val audioDevices = am.getDevices(AudioManager.GET_DEVICES_OUTPUTS)
            if (audioDevices.any { it.type == AudioDeviceInfo.TYPE_WIRED_HEADSET }) {
                devices.add(AudioDevice.WIRED_HEADSET)
            }
        }
        
        return devices
    }
    
    /**
     * تحديد جهاز الصوت / Set audio device
     */
    fun setAudioDevice(context: Context, device: AudioDevice) {
        val am = getAudioManager(context)
        
        when (device) {
            AudioDevice.SPEAKER -> {
                am.isSpeakerphoneOn = true
                am.isBluetoothScoOn = false
            }
            AudioDevice.EARPIECE -> {
                am.isSpeakerphoneOn = false
                am.isBluetoothScoOn = false
            }
            AudioDevice.BLUETOOTH -> {
                am.isSpeakerphoneOn = false
                am.startBluetoothSco()
                am.isBluetoothScoOn = true
            }
            AudioDevice.WIRED_HEADSET -> {
                am.isSpeakerphoneOn = false
                am.isBluetoothScoOn = false
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         وضع الاتصال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تفعيل وضع الاتصال / Enable call mode
     */
    fun enableCallMode(context: Context) {
        val am = getAudioManager(context)
        am.mode = AudioManager.MODE_IN_COMMUNICATION
    }
    
    /**
     * إيقاف وضع الاتصال / Disable call mode
     */
    fun disableCallMode(context: Context) {
        val am = getAudioManager(context)
        am.mode = AudioManager.MODE_NORMAL
        am.isSpeakerphoneOn = false
        am.isBluetoothScoOn = false
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأذونات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * التحقق من الأذونات / Check permissions
     */
    fun hasPermissions(context: Context): Boolean {
        return hasAudioPermission(context) && hasCameraPermission(context)
    }
    
    fun hasAudioPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.RECORD_AUDIO
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun hasCameraPermission(context: Context): Boolean {
        return ContextCompat.checkSelfPermission(
            context,
            Manifest.permission.CAMERA
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    fun requiredPermissions(): Array<String> = arrayOf(
        Manifest.permission.RECORD_AUDIO,
        Manifest.permission.CAMERA,
        Manifest.permission.BLUETOOTH,
        Manifest.permission.MODIFY_AUDIO_SETTINGS
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun getAudioManager(context: Context): AudioManager {
        return audioManager ?: (context.getSystemService(Context.AUDIO_SERVICE) as AudioManager).also {
            audioManager = it
        }
    }
}
