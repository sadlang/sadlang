package sad.android.tv.player

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر مشغل الفيديو التلفزيوني - VideoSupportFragment و ExoPlayer
 * TV Video Player Bridge - VideoSupportFragment and ExoPlayer
 * 
 * @author فريق لغة ص
 */
object SadTVVideoPlayerBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * مصدر الفيديو / Video source
     */
    data class VideoSource(
        val id: String,
        val url: String,
        val title: String = "",
        val subtitle: String = "",
        val contentType: ContentType = ContentType.MP4,
        val drmType: DRMType? = null,
        val drmLicenseUrl: String? = null,
        val startPosition: Long = 0,
        val headers: Map<String, String> = emptyMap()
    )
    
    /**
     * نوع المحتوى / Content type
     */
    enum class ContentType {
        MP4,
        HLS,
        DASH,
        RTSP,
        RTMP,
        PROGRESSIVE
    }
    
    /**
     * نوع DRM
     */
    enum class DRMType {
        WIDEVINE,
        PLAYREADY,
        FAIRPLAY
    }
    
    /**
     * حالة المشغل / Player state
     */
    enum class PlayerState {
        IDLE,
        BUFFERING,
        READY,
        PLAYING,
        PAUSED,
        ENDED,
        ERROR
    }
    
    /**
     * جودة الفيديو / Video quality
     */
    data class VideoQuality(
        val id: String,
        val label: String,
        val width: Int,
        val height: Int,
        val bitrate: Int,
        val isSelected: Boolean = false
    )
    
    /**
     * مسار الصوت / Audio track
     */
    data class AudioTrack(
        val id: String,
        val language: String,
        val label: String,
        val isSelected: Boolean = false
    )
    
    /**
     * مسار الترجمة / Subtitle track
     */
    data class SubtitleTrack(
        val id: String,
        val language: String,
        val label: String,
        val url: String? = null,
        val isSelected: Boolean = false
    )
    
    /**
     * معلومات المشغل / Player info
     */
    data class PlayerInfo(
        val state: PlayerState,
        val currentPosition: Long,
        val duration: Long,
        val bufferedPosition: Long,
        val playbackSpeed: Float,
        val volume: Float,
        val isMuted: Boolean,
        val currentQuality: VideoQuality?,
        val currentAudioTrack: AudioTrack?,
        val currentSubtitleTrack: SubtitleTrack?
    )
    
    /**
     * تكوين المشغل / Player config
     */
    data class PlayerConfig(
        val autoPlay: Boolean = true,
        val looping: Boolean = false,
        val muteOnStart: Boolean = false,
        val showControls: Boolean = true,
        val controlsTimeout: Long = 5000,
        val seekForwardIncrement: Long = 10000,
        val seekBackwardIncrement: Long = 10000,
        val minBufferMs: Int = 15000,
        val maxBufferMs: Int = 30000,
        val bufferForPlaybackMs: Int = 2500
    )
    
    /**
     * مستمع المشغل / Player listener
     */
    interface PlayerListener {
        fun onStateChanged(state: PlayerState)
        fun onPositionChanged(position: Long, duration: Long)
        fun onBuffering(percent: Int)
        fun onPlaybackEnded()
        fun onError(error: String, errorCode: Int)
        fun onTrackChanged(trackType: String)
    }
    
    private var playerListener: PlayerListener? = null
    private var config = PlayerConfig()
    private var currentSource: VideoSource? = null
    private var currentInfo = PlayerInfo(
        state = PlayerState.IDLE,
        currentPosition = 0,
        duration = 0,
        bufferedPosition = 0,
        playbackSpeed = 1f,
        volume = 1f,
        isMuted = false,
        currentQuality = null,
        currentAudioTrack = null,
        currentSubtitleTrack = null
    )
    
    private val availableQualities = mutableListOf<VideoQuality>()
    private val availableAudioTracks = mutableListOf<AudioTrack>()
    private val availableSubtitleTracks = mutableListOf<SubtitleTrack>()
    
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
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: PlayerConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): PlayerConfig = config
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setPlayerListener(listener: PlayerListener?) {
        playerListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إعداد المصدر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين مصدر الفيديو / Set video source
     */
    fun setSource(source: VideoSource) {
        currentSource = source
        currentInfo = currentInfo.copy(
            state = PlayerState.IDLE,
            currentPosition = source.startPosition,
            duration = 0
        )
    }
    
    /**
     * تعيين مصدر بسيط / Set simple source
     */
    fun setSourceUrl(url: String, title: String = "") {
        val contentType = detectContentType(url)
        setSource(VideoSource(
            id = url.hashCode().toString(),
            url = url,
            title = title,
            contentType = contentType
        ))
    }
    
    private fun detectContentType(url: String): ContentType {
        return when {
            url.contains(".m3u8") -> ContentType.HLS
            url.contains(".mpd") -> ContentType.DASH
            url.startsWith("rtsp://") -> ContentType.RTSP
            url.startsWith("rtmp://") -> ContentType.RTMP
            else -> ContentType.MP4
        }
    }
    
    /**
     * الحصول على المصدر الحالي / Get current source
     */
    fun getCurrentSource(): VideoSource? = currentSource
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحكم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تشغيل / Play
     */
    fun play() {
        updateState(PlayerState.PLAYING)
    }
    
    /**
     * إيقاف مؤقت / Pause
     */
    fun pause() {
        updateState(PlayerState.PAUSED)
    }
    
    /**
     * إيقاف / Stop
     */
    fun stop() {
        updateState(PlayerState.IDLE)
        currentInfo = currentInfo.copy(currentPosition = 0)
    }
    
    /**
     * الانتقال لموضع / Seek to position
     */
    fun seekTo(positionMs: Long) {
        val newPosition = positionMs.coerceIn(0, currentInfo.duration)
        currentInfo = currentInfo.copy(currentPosition = newPosition)
        notifyPositionChanged()
    }
    
    /**
     * تقديم / Seek forward
     */
    fun seekForward() {
        seekTo(currentInfo.currentPosition + config.seekForwardIncrement)
    }
    
    /**
     * ترجيع / Seek backward
     */
    fun seekBackward() {
        seekTo(currentInfo.currentPosition - config.seekBackwardIncrement)
    }
    
    /**
     * تعيين سرعة التشغيل / Set playback speed
     */
    fun setPlaybackSpeed(speed: Float) {
        currentInfo = currentInfo.copy(playbackSpeed = speed.coerceIn(0.25f, 3f))
    }
    
    /**
     * تعيين مستوى الصوت / Set volume
     */
    fun setVolume(volume: Float) {
        currentInfo = currentInfo.copy(volume = volume.coerceIn(0f, 1f))
    }
    
    /**
     * كتم/إلغاء كتم الصوت / Toggle mute
     */
    fun toggleMute() {
        currentInfo = currentInfo.copy(isMuted = !currentInfo.isMuted)
    }
    
    private fun updateState(state: PlayerState) {
        currentInfo = currentInfo.copy(state = state)
        mainHandler.post { playerListener?.onStateChanged(state) }
    }
    
    private fun notifyPositionChanged() {
        mainHandler.post {
            playerListener?.onPositionChanged(currentInfo.currentPosition, currentInfo.duration)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المسارات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جودات الفيديو / Get video qualities
     */
    fun getVideoQualities(): List<VideoQuality> = availableQualities.toList()
    
    /**
     * تعيين الجودة / Set quality
     */
    fun setVideoQuality(qualityId: String): Boolean {
        val quality = availableQualities.find { it.id == qualityId } ?: return false
        availableQualities.forEachIndexed { i, q ->
            availableQualities[i] = q.copy(isSelected = q.id == qualityId)
        }
        currentInfo = currentInfo.copy(currentQuality = quality.copy(isSelected = true))
        mainHandler.post { playerListener?.onTrackChanged("video") }
        return true
    }
    
    /**
     * الحصول على مسارات الصوت / Get audio tracks
     */
    fun getAudioTracks(): List<AudioTrack> = availableAudioTracks.toList()
    
    /**
     * تعيين مسار الصوت / Set audio track
     */
    fun setAudioTrack(trackId: String): Boolean {
        val track = availableAudioTracks.find { it.id == trackId } ?: return false
        availableAudioTracks.forEachIndexed { i, t ->
            availableAudioTracks[i] = t.copy(isSelected = t.id == trackId)
        }
        currentInfo = currentInfo.copy(currentAudioTrack = track.copy(isSelected = true))
        mainHandler.post { playerListener?.onTrackChanged("audio") }
        return true
    }
    
    /**
     * الحصول على مسارات الترجمة / Get subtitle tracks
     */
    fun getSubtitleTracks(): List<SubtitleTrack> = availableSubtitleTracks.toList()
    
    /**
     * تعيين مسار الترجمة / Set subtitle track
     */
    fun setSubtitleTrack(trackId: String?): Boolean {
        if (trackId == null) {
            availableSubtitleTracks.forEachIndexed { i, t ->
                availableSubtitleTracks[i] = t.copy(isSelected = false)
            }
            currentInfo = currentInfo.copy(currentSubtitleTrack = null)
        } else {
            val track = availableSubtitleTracks.find { it.id == trackId } ?: return false
            availableSubtitleTracks.forEachIndexed { i, t ->
                availableSubtitleTracks[i] = t.copy(isSelected = t.id == trackId)
            }
            currentInfo = currentInfo.copy(currentSubtitleTrack = track.copy(isSelected = true))
        }
        mainHandler.post { playerListener?.onTrackChanged("subtitle") }
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات المشغل / Get player info
     */
    fun getPlayerInfo(): PlayerInfo = currentInfo
    
    /**
     * الحصول على نسبة التقدم / Get progress percent
     */
    fun getProgressPercent(): Float {
        if (currentInfo.duration <= 0) return 0f
        return currentInfo.currentPosition.toFloat() / currentInfo.duration
    }
    
    /**
     * تنسيق الوقت / Format time
     */
    fun formatTime(ms: Long): String {
        val seconds = ms / 1000
        val minutes = seconds / 60
        val hours = minutes / 60
        
        return if (hours > 0) {
            String.format("%d:%02d:%02d", hours, minutes % 60, seconds % 60)
        } else {
            String.format("%02d:%02d", minutes, seconds % 60)
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stop()
        currentSource = null
        availableQualities.clear()
        availableAudioTracks.clear()
        availableSubtitleTracks.clear()
        playerListener = null
        context = null
    }
}
