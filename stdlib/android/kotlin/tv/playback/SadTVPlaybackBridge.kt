package sad.android.tv.playback

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر تشغيل التلفزيون - PlaybackFragment للتحكم بالوسائط
 * TV Playback Bridge - PlaybackFragment for media control
 * 
 * @author فريق لغة ص
 */
object SadTVPlaybackBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var playbackHandler: Handler? = null
    private var positionUpdateRunnable: Runnable? = null
    
    /**
     * حالة التشغيل / Playback state
     */
    enum class PlaybackState {
        IDLE,
        BUFFERING,
        PLAYING,
        PAUSED,
        ENDED,
        ERROR
    }
    
    /**
     * معلومات الوسائط / Media info
     */
    data class MediaInfo(
        val id: String,
        val title: String,
        val subtitle: String = "",
        val description: String = "",
        val imageUrl: String = "",
        val videoUrl: String = "",
        val duration: Long = 0,         // بالميلي ثانية
        val contentType: String = "video/mp4",
        val subtitles: List<SubtitleTrack> = emptyList(),
        val audioTracks: List<AudioTrack> = emptyList(),
        val qualities: List<VideoQuality> = emptyList()
    )
    
    /**
     * مسار الترجمة / Subtitle track
     */
    data class SubtitleTrack(
        val id: String,
        val language: String,
        val languageCode: String,
        val url: String,
        val isDefault: Boolean = false
    )
    
    /**
     * مسار الصوت / Audio track
     */
    data class AudioTrack(
        val id: String,
        val language: String,
        val languageCode: String,
        val isDefault: Boolean = false
    )
    
    /**
     * جودة الفيديو / Video quality
     */
    data class VideoQuality(
        val id: String,
        val label: String,           // مثال: "1080p", "720p"
        val width: Int,
        val height: Int,
        val bitrate: Int,            // بالكيلوبت
        val url: String
    )
    
    /**
     * حالة المشغل / Player state
     */
    data class PlayerState(
        val state: PlaybackState,
        val currentPosition: Long,   // بالميلي ثانية
        val duration: Long,
        val bufferedPosition: Long,
        val playbackSpeed: Float = 1f,
        val volume: Float = 1f,
        val isMuted: Boolean = false,
        val currentSubtitleTrack: String? = null,
        val currentAudioTrack: String? = null,
        val currentQuality: String? = null
    )
    
    /**
     * أوامر التشغيل / Playback commands
     */
    sealed class PlaybackCommand {
        object Play : PlaybackCommand()
        object Pause : PlaybackCommand()
        object Stop : PlaybackCommand()
        data class SeekTo(val positionMs: Long) : PlaybackCommand()
        data class SeekRelative(val deltaMs: Long) : PlaybackCommand()
        data class SetSpeed(val speed: Float) : PlaybackCommand()
        data class SetVolume(val volume: Float) : PlaybackCommand()
        object ToggleMute : PlaybackCommand()
        data class SetSubtitle(val trackId: String?) : PlaybackCommand()
        data class SetAudio(val trackId: String) : PlaybackCommand()
        data class SetQuality(val qualityId: String) : PlaybackCommand()
    }
    
    /**
     * مستمع التشغيل / Playback listener
     */
    interface PlaybackListener {
        fun onStateChanged(state: PlaybackState)
        fun onPositionChanged(position: Long, duration: Long)
        fun onBufferingChanged(bufferedPosition: Long)
        fun onPlaybackEnded()
        fun onError(error: String)
        fun onTrackChanged(trackType: String, trackId: String)
    }
    
    private var currentMedia: MediaInfo? = null
    private var playerState = PlayerState(
        state = PlaybackState.IDLE,
        currentPosition = 0,
        duration = 0,
        bufferedPosition = 0
    )
    private var playbackListener: PlaybackListener? = null
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        playbackHandler = Handler(Looper.getMainLooper())
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setPlaybackListener(listener: PlaybackListener?) {
        playbackListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إعداد الوسائط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين الوسائط / Set media
     */
    fun setMedia(media: MediaInfo) {
        currentMedia = media
        playerState = playerState.copy(
            state = PlaybackState.IDLE,
            currentPosition = 0,
            duration = media.duration,
            bufferedPosition = 0
        )
    }
    
    /**
     * الحصول على الوسائط الحالية / Get current media
     */
    fun getCurrentMedia(): MediaInfo? = currentMedia
    
    /**
     * الحصول على حالة المشغل / Get player state
     */
    fun getPlayerState(): PlayerState = playerState
    
    // ═══════════════════════════════════════════════════════════════
    //                         أوامر التشغيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنفيذ أمر / Execute command
     */
    fun executeCommand(command: PlaybackCommand): Boolean {
        when (command) {
            is PlaybackCommand.Play -> {
                updateState(PlaybackState.PLAYING)
                startPositionUpdates()
            }
            is PlaybackCommand.Pause -> {
                updateState(PlaybackState.PAUSED)
                stopPositionUpdates()
            }
            is PlaybackCommand.Stop -> {
                updateState(PlaybackState.IDLE)
                stopPositionUpdates()
                playerState = playerState.copy(currentPosition = 0)
            }
            is PlaybackCommand.SeekTo -> {
                val newPosition = command.positionMs.coerceIn(0, playerState.duration)
                playerState = playerState.copy(currentPosition = newPosition)
                notifyPositionChanged()
            }
            is PlaybackCommand.SeekRelative -> {
                val newPosition = (playerState.currentPosition + command.deltaMs)
                    .coerceIn(0, playerState.duration)
                playerState = playerState.copy(currentPosition = newPosition)
                notifyPositionChanged()
            }
            is PlaybackCommand.SetSpeed -> {
                playerState = playerState.copy(playbackSpeed = command.speed)
            }
            is PlaybackCommand.SetVolume -> {
                playerState = playerState.copy(volume = command.volume.coerceIn(0f, 1f))
            }
            is PlaybackCommand.ToggleMute -> {
                playerState = playerState.copy(isMuted = !playerState.isMuted)
            }
            is PlaybackCommand.SetSubtitle -> {
                playerState = playerState.copy(currentSubtitleTrack = command.trackId)
                command.trackId?.let { notifyTrackChanged("subtitle", it) }
            }
            is PlaybackCommand.SetAudio -> {
                playerState = playerState.copy(currentAudioTrack = command.trackId)
                notifyTrackChanged("audio", command.trackId)
            }
            is PlaybackCommand.SetQuality -> {
                playerState = playerState.copy(currentQuality = command.qualityId)
                notifyTrackChanged("quality", command.qualityId)
            }
        }
        return true
    }
    
    // اختصارات سريعة
    fun play() = executeCommand(PlaybackCommand.Play)
    fun pause() = executeCommand(PlaybackCommand.Pause)
    fun stop() = executeCommand(PlaybackCommand.Stop)
    fun seekTo(positionMs: Long) = executeCommand(PlaybackCommand.SeekTo(positionMs))
    fun seekForward(ms: Long = 10000) = executeCommand(PlaybackCommand.SeekRelative(ms))
    fun seekBackward(ms: Long = 10000) = executeCommand(PlaybackCommand.SeekRelative(-ms))
    
    private fun updateState(newState: PlaybackState) {
        playerState = playerState.copy(state = newState)
        mainHandler.post { playbackListener?.onStateChanged(newState) }
        
        if (newState == PlaybackState.ENDED) {
            mainHandler.post { playbackListener?.onPlaybackEnded() }
        }
    }
    
    private fun notifyPositionChanged() {
        mainHandler.post {
            playbackListener?.onPositionChanged(playerState.currentPosition, playerState.duration)
        }
    }
    
    private fun notifyTrackChanged(type: String, trackId: String) {
        mainHandler.post { playbackListener?.onTrackChanged(type, trackId) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحديث الموضع
    // ═══════════════════════════════════════════════════════════════
    
    private fun startPositionUpdates() {
        stopPositionUpdates()
        
        positionUpdateRunnable = object : Runnable {
            override fun run() {
                if (playerState.state == PlaybackState.PLAYING) {
                    val increment = (1000 * playerState.playbackSpeed).toLong()
                    var newPosition = playerState.currentPosition + increment
                    
                    if (newPosition >= playerState.duration) {
                        newPosition = playerState.duration
                        updateState(PlaybackState.ENDED)
                    } else {
                        playerState = playerState.copy(currentPosition = newPosition)
                        notifyPositionChanged()
                        playbackHandler?.postDelayed(this, 1000)
                    }
                }
            }
        }
        
        playbackHandler?.postDelayed(positionUpdateRunnable!!, 1000)
    }
    
    private fun stopPositionUpdates() {
        positionUpdateRunnable?.let { playbackHandler?.removeCallbacks(it) }
        positionUpdateRunnable = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
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
     * الحصول على نسبة التقدم / Get progress percent
     */
    fun getProgressPercent(): Float {
        if (playerState.duration <= 0) return 0f
        return playerState.currentPosition.toFloat() / playerState.duration
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopPositionUpdates()
        currentMedia = null
        playbackListener = null
        playbackHandler = null
        context = null
    }
}
