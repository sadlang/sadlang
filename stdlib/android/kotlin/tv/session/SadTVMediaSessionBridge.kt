package sad.android.tv.session

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر جلسة الوسائط التلفزيونية - MediaSession management
 * TV Media Session Bridge - MediaSession management
 * 
 * @author فريق لغة ص
 */
object SadTVMediaSessionBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * حالة التشغيل / Playback state
     */
    enum class PlaybackState {
        NONE,
        STOPPED,
        PAUSED,
        PLAYING,
        FAST_FORWARDING,
        REWINDING,
        BUFFERING,
        ERROR,
        CONNECTING,
        SKIPPING_TO_PREVIOUS,
        SKIPPING_TO_NEXT,
        SKIPPING_TO_QUEUE_ITEM
    }
    
    /**
     * بيانات الوسائط / Media metadata
     */
    data class MediaMetadata(
        val title: String,
        val subtitle: String = "",
        val artist: String = "",
        val album: String = "",
        val duration: Long = 0,
        val artUri: String = "",
        val artBitmap: Any? = null,
        val genre: String = "",
        val year: Int = 0,
        val trackNumber: Int = 0,
        val totalTracks: Int = 0,
        val extras: Map<String, String> = emptyMap()
    )
    
    /**
     * عنصر قائمة الانتظار / Queue item
     */
    data class QueueItem(
        val id: Long,
        val title: String,
        val subtitle: String = "",
        val artUri: String = "",
        val duration: Long = 0,
        val extras: Map<String, String> = emptyMap()
    )
    
    /**
     * معلومات التشغيل / Playback info
     */
    data class PlaybackInfo(
        val state: PlaybackState,
        val position: Long,
        val bufferedPosition: Long,
        val speed: Float,
        val errorCode: Int = 0,
        val errorMessage: String = ""
    )
    
    /**
     * تكوين الجلسة / Session config
     */
    data class SessionConfig(
        val sessionTag: String = "SadTVMediaSession",
        val isActive: Boolean = true,
        val volumeType: VolumeType = VolumeType.STREAM,
        val maxVolume: Int = 100,
        val currentVolume: Int = 50
    )
    
    /**
     * نوع الصوت / Volume type
     */
    enum class VolumeType {
        STREAM,
        REMOTE
    }
    
    /**
     * أوامر التحكم / Media commands
     */
    enum class MediaCommand {
        PLAY,
        PAUSE,
        STOP,
        SKIP_TO_NEXT,
        SKIP_TO_PREVIOUS,
        FAST_FORWARD,
        REWIND,
        SEEK_TO,
        SET_RATING,
        SET_SHUFFLE_MODE,
        SET_REPEAT_MODE
    }
    
    /**
     * مستمع الجلسة / Session listener
     */
    interface MediaSessionListener {
        fun onCommand(command: MediaCommand, extras: Map<String, Any>?)
        fun onSeekTo(position: Long)
        fun onSkipToQueueItem(id: Long)
        fun onCustomAction(action: String, extras: Map<String, Any>?)
    }
    
    private var sessionListener: MediaSessionListener? = null
    private var config = SessionConfig()
    private var currentMetadata: MediaMetadata? = null
    private var currentPlaybackInfo = PlaybackInfo(
        state = PlaybackState.NONE,
        position = 0,
        bufferedPosition = 0,
        speed = 1f
    )
    private val queue = mutableListOf<QueueItem>()
    private var currentQueueIndex = -1
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setMediaSessionListener(listener: MediaSessionListener?) {
        sessionListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: SessionConfig) {
        config = newConfig
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الجلسة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بدء الجلسة / Start session
     */
    fun startSession() {
        config = config.copy(isActive = true)
    }
    
    /**
     * إيقاف الجلسة / Stop session
     */
    fun stopSession() {
        config = config.copy(isActive = false)
    }
    
    /**
     * هل الجلسة نشطة؟ / Is session active?
     */
    fun isSessionActive(): Boolean = config.isActive
    
    // ═══════════════════════════════════════════════════════════════
    //                         البيانات الوصفية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين البيانات الوصفية / Set metadata
     */
    fun setMetadata(metadata: MediaMetadata) {
        currentMetadata = metadata
    }
    
    /**
     * الحصول على البيانات الوصفية / Get metadata
     */
    fun getMetadata(): MediaMetadata? = currentMetadata
    
    /**
     * مسح البيانات الوصفية / Clear metadata
     */
    fun clearMetadata() {
        currentMetadata = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         حالة التشغيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين حالة التشغيل / Set playback state
     */
    fun setPlaybackState(state: PlaybackState, position: Long = currentPlaybackInfo.position) {
        currentPlaybackInfo = currentPlaybackInfo.copy(
            state = state,
            position = position
        )
    }
    
    /**
     * تحديث الموضع / Update position
     */
    fun updatePosition(position: Long, bufferedPosition: Long = position) {
        currentPlaybackInfo = currentPlaybackInfo.copy(
            position = position,
            bufferedPosition = bufferedPosition
        )
    }
    
    /**
     * تعيين السرعة / Set speed
     */
    fun setSpeed(speed: Float) {
        currentPlaybackInfo = currentPlaybackInfo.copy(speed = speed)
    }
    
    /**
     * تعيين الخطأ / Set error
     */
    fun setError(errorCode: Int, errorMessage: String) {
        currentPlaybackInfo = currentPlaybackInfo.copy(
            state = PlaybackState.ERROR,
            errorCode = errorCode,
            errorMessage = errorMessage
        )
    }
    
    /**
     * الحصول على معلومات التشغيل / Get playback info
     */
    fun getPlaybackInfo(): PlaybackInfo = currentPlaybackInfo
    
    // ═══════════════════════════════════════════════════════════════
    //                         قائمة الانتظار
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين قائمة الانتظار / Set queue
     */
    fun setQueue(items: List<QueueItem>, title: String = "") {
        queue.clear()
        queue.addAll(items)
        currentQueueIndex = if (items.isNotEmpty()) 0 else -1
    }
    
    /**
     * إضافة لقائمة الانتظار / Add to queue
     */
    fun addToQueue(item: QueueItem) {
        queue.add(item)
        if (currentQueueIndex < 0) currentQueueIndex = 0
    }
    
    /**
     * إزالة من قائمة الانتظار / Remove from queue
     */
    fun removeFromQueue(id: Long): Boolean {
        val index = queue.indexOfFirst { it.id == id }
        if (index < 0) return false
        
        queue.removeAt(index)
        if (currentQueueIndex >= queue.size) {
            currentQueueIndex = queue.size - 1
        }
        return true
    }
    
    /**
     * الحصول على قائمة الانتظار / Get queue
     */
    fun getQueue(): List<QueueItem> = queue.toList()
    
    /**
     * العنصر الحالي / Current item
     */
    fun getCurrentQueueItem(): QueueItem? = queue.getOrNull(currentQueueIndex)
    
    /**
     * التالي في قائمة الانتظار / Next in queue
     */
    fun skipToNext(): Boolean {
        if (currentQueueIndex < queue.size - 1) {
            currentQueueIndex++
            mainHandler.post { sessionListener?.onCommand(MediaCommand.SKIP_TO_NEXT, null) }
            return true
        }
        return false
    }
    
    /**
     * السابق في قائمة الانتظار / Previous in queue
     */
    fun skipToPrevious(): Boolean {
        if (currentQueueIndex > 0) {
            currentQueueIndex--
            mainHandler.post { sessionListener?.onCommand(MediaCommand.SKIP_TO_PREVIOUS, null) }
            return true
        }
        return false
    }
    
    /**
     * التخطي لعنصر / Skip to item
     */
    fun skipToQueueItem(id: Long): Boolean {
        val index = queue.indexOfFirst { it.id == id }
        if (index >= 0) {
            currentQueueIndex = index
            mainHandler.post { sessionListener?.onSkipToQueueItem(id) }
            return true
        }
        return false
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأوامر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال أمر / Send command
     */
    fun sendCommand(command: MediaCommand, extras: Map<String, Any>? = null) {
        mainHandler.post { sessionListener?.onCommand(command, extras) }
    }
    
    /**
     * الانتقال لموضع / Seek to
     */
    fun seekTo(position: Long) {
        updatePosition(position)
        mainHandler.post { sessionListener?.onSeekTo(position) }
    }
    
    /**
     * إجراء مخصص / Custom action
     */
    fun sendCustomAction(action: String, extras: Map<String, Any>? = null) {
        mainHandler.post { sessionListener?.onCustomAction(action, extras) }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopSession()
        queue.clear()
        currentMetadata = null
        sessionListener = null
        context = null
    }
}
