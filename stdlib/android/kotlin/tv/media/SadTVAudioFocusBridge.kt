package sad.android.tv.media

import android.content.Context
import android.media.AudioManager
import android.os.Handler
import android.os.Looper

/**
 * جسر تركيز الصوت التلفزيوني - AudioFocus management
 * TV Audio Focus Bridge - AudioFocus management
 * 
 * @author فريق لغة ص
 */
object SadTVAudioFocusBridge {
    
    private var context: Context? = null
    private var audioManager: AudioManager? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * حالة تركيز الصوت / Audio focus state
     */
    enum class FocusState {
        NOT_REQUESTED,
        GAINED,
        LOST,
        LOST_TRANSIENT,
        LOST_TRANSIENT_CAN_DUCK
    }
    
    /**
     * نوع الطلب / Request type
     */
    enum class FocusType {
        GAIN,
        GAIN_TRANSIENT,
        GAIN_TRANSIENT_MAY_DUCK,
        GAIN_TRANSIENT_EXCLUSIVE
    }
    
    /**
     * نوع الاستخدام / Usage type
     */
    enum class UsageType {
        MEDIA,
        VOICE_COMMUNICATION,
        ALARM,
        NOTIFICATION,
        GAME
    }
    
    /**
     * تكوين التركيز / Focus config
     */
    data class FocusConfig(
        val focusType: FocusType = FocusType.GAIN,
        val usageType: UsageType = UsageType.MEDIA,
        val contentType: ContentType = ContentType.MOVIE,
        val acceptsDelayedFocus: Boolean = true,
        val willPauseWhenDucked: Boolean = false
    )
    
    /**
     * نوع المحتوى / Content type
     */
    enum class ContentType {
        MOVIE,
        MUSIC,
        SPEECH,
        SONIFICATION
    }
    
    /**
     * مستمع التركيز / Focus listener
     */
    interface AudioFocusListener {
        fun onFocusGained()
        fun onFocusLost()
        fun onFocusLostTransient()
        fun onFocusLostTransientCanDuck()
    }
    
    private var focusListener: AudioFocusListener? = null
    private var currentState = FocusState.NOT_REQUESTED
    private var config = FocusConfig()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        audioManager = ctx.getSystemService(Context.AUDIO_SERVICE) as? AudioManager
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setAudioFocusListener(listener: AudioFocusListener?) {
        focusListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: FocusConfig) {
        config = newConfig
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة التركيز
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * طلب تركيز الصوت / Request audio focus
     */
    fun requestFocus(): Boolean {
        val am = audioManager ?: return false
        
        val result = if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            val attr = android.media.AudioAttributes.Builder()
                .setUsage(mapUsageType(config.usageType))
                .setContentType(mapContentType(config.contentType))
                .build()
            
            val request = android.media.AudioFocusRequest.Builder(mapFocusType(config.focusType))
                .setAudioAttributes(attr)
                .setAcceptsDelayedFocusGain(config.acceptsDelayedFocus)
                .setWillPauseWhenDucked(config.willPauseWhenDucked)
                .setOnAudioFocusChangeListener(focusChangeListener)
                .build()
            
            am.requestAudioFocus(request)
        } else {
            @Suppress("DEPRECATION")
            am.requestAudioFocus(
                legacyFocusChangeListener,
                android.media.AudioManager.STREAM_MUSIC,
                mapFocusType(config.focusType)
            )
        }
        
        return when (result) {
            AudioManager.AUDIOFOCUS_REQUEST_GRANTED -> {
                updateState(FocusState.GAINED)
                true
            }
            AudioManager.AUDIOFOCUS_REQUEST_DELAYED -> true
            else -> false
        }
    }
    
    /**
     * التخلي عن تركيز الصوت / Abandon audio focus
     */
    fun abandonFocus() {
        val am = audioManager ?: return
        
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            val request = android.media.AudioFocusRequest.Builder(mapFocusType(config.focusType))
                .setOnAudioFocusChangeListener(focusChangeListener)
                .build()
            am.abandonAudioFocusRequest(request)
        } else {
            @Suppress("DEPRECATION")
            am.abandonAudioFocus(legacyFocusChangeListener)
        }
        
        updateState(FocusState.NOT_REQUESTED)
    }
    
    /**
     * الحصول على الحالة / Get state
     */
    fun getState(): FocusState = currentState
    
    /**
     * هل لدينا التركيز؟ / Do we have focus?
     */
    fun hasFocus(): Boolean = currentState == FocusState.GAINED
    
    private fun updateState(state: FocusState) {
        currentState = state
        
        mainHandler.post {
            when (state) {
                FocusState.GAINED -> focusListener?.onFocusGained()
                FocusState.LOST -> focusListener?.onFocusLost()
                FocusState.LOST_TRANSIENT -> focusListener?.onFocusLostTransient()
                FocusState.LOST_TRANSIENT_CAN_DUCK -> focusListener?.onFocusLostTransientCanDuck()
                FocusState.NOT_REQUESTED -> {}
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المستمعات
    // ═══════════════════════════════════════════════════════════════
    
    private val focusChangeListener = android.media.AudioManager.OnAudioFocusChangeListener { focusChange ->
        handleFocusChange(focusChange)
    }
    
    private val legacyFocusChangeListener = android.media.AudioManager.OnAudioFocusChangeListener { focusChange ->
        handleFocusChange(focusChange)
    }
    
    private fun handleFocusChange(focusChange: Int) {
        when (focusChange) {
            AudioManager.AUDIOFOCUS_GAIN -> updateState(FocusState.GAINED)
            AudioManager.AUDIOFOCUS_LOSS -> updateState(FocusState.LOST)
            AudioManager.AUDIOFOCUS_LOSS_TRANSIENT -> updateState(FocusState.LOST_TRANSIENT)
            AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK -> updateState(FocusState.LOST_TRANSIENT_CAN_DUCK)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحويل
    // ═══════════════════════════════════════════════════════════════
    
    private fun mapFocusType(type: FocusType): Int {
        return when (type) {
            FocusType.GAIN -> AudioManager.AUDIOFOCUS_GAIN
            FocusType.GAIN_TRANSIENT -> AudioManager.AUDIOFOCUS_GAIN_TRANSIENT
            FocusType.GAIN_TRANSIENT_MAY_DUCK -> AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK
            FocusType.GAIN_TRANSIENT_EXCLUSIVE -> AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE
        }
    }
    
    private fun mapUsageType(type: UsageType): Int {
        return when (type) {
            UsageType.MEDIA -> android.media.AudioAttributes.USAGE_MEDIA
            UsageType.VOICE_COMMUNICATION -> android.media.AudioAttributes.USAGE_VOICE_COMMUNICATION
            UsageType.ALARM -> android.media.AudioAttributes.USAGE_ALARM
            UsageType.NOTIFICATION -> android.media.AudioAttributes.USAGE_NOTIFICATION
            UsageType.GAME -> android.media.AudioAttributes.USAGE_GAME
        }
    }
    
    private fun mapContentType(type: ContentType): Int {
        return when (type) {
            ContentType.MOVIE -> android.media.AudioAttributes.CONTENT_TYPE_MOVIE
            ContentType.MUSIC -> android.media.AudioAttributes.CONTENT_TYPE_MUSIC
            ContentType.SPEECH -> android.media.AudioAttributes.CONTENT_TYPE_SPEECH
            ContentType.SONIFICATION -> android.media.AudioAttributes.CONTENT_TYPE_SONIFICATION
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        abandonFocus()
        focusListener = null
        audioManager = null
        context = null
    }
}
