package sad.android.widgets.music

import android.content.ComponentName
import android.content.Context
import android.graphics.Bitmap
import android.media.MediaMetadata
import android.media.session.MediaController
import android.media.session.MediaSessionManager
import android.media.session.PlaybackState
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر ويدجت الموسيقى - التحكم بمشغل الوسائط
 * Music Widget Bridge - Media player control
 * 
 * @author فريق لغة ص
 */
object SadWidgetMusicBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private var mediaSessionManager: MediaSessionManager? = null
    private var activeController: MediaController? = null
    private val mediaListeners = mutableListOf<MediaStateListener>()
    private var mediaCallback: MediaController.Callback? = null
    
    /**
     * معلومات المسار / Track info
     */
    data class TrackInfo(
        val title: String,
        val artist: String,
        val album: String,
        val duration: Long,      // بالمللي ثانية
        val position: Long,      // الموضع الحالي
        val albumArt: Bitmap?,
        val albumArtUri: String?
    )
    
    /**
     * حالة التشغيل / Playback state
     */
    data class PlaybackInfo(
        val state: PlayState,
        val position: Long,
        val speed: Float,
        val shuffleMode: ShuffleMode,
        val repeatMode: RepeatMode
    )
    
    /**
     * حالة التشغيل / Play state
     */
    enum class PlayState {
        PLAYING,
        PAUSED,
        STOPPED,
        BUFFERING,
        ERROR,
        NONE
    }
    
    /**
     * وضع الخلط / Shuffle mode
     */
    enum class ShuffleMode {
        OFF,
        ON
    }
    
    /**
     * وضع التكرار / Repeat mode
     */
    enum class RepeatMode {
        NONE,
        ONE,
        ALL
    }
    
    /**
     * مستمع حالة الوسائط / Media state listener
     */
    interface MediaStateListener {
        fun onTrackChanged(track: TrackInfo)
        fun onPlaybackStateChanged(playback: PlaybackInfo)
        fun onSessionConnected(packageName: String)
        fun onSessionDisconnected()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        mediaSessionManager = ctx.getSystemService(Context.MEDIA_SESSION_SERVICE) as? MediaSessionManager
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addMediaListener(listener: MediaStateListener) {
        if (!mediaListeners.contains(listener)) {
            mediaListeners.add(listener)
        }
    }
    
    /**
     * إزالة مستمع / Remove listener
     */
    fun removeMediaListener(listener: MediaStateListener) {
        mediaListeners.remove(listener)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الاتصال بالجلسة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الاتصال بجلسة نشطة / Connect to active session
     * يتطلب إذن MEDIA_CONTENT_CONTROL أو NotificationListenerService
     */
    fun connectToActiveSession(listenerComponent: ComponentName): Boolean {
        val manager = mediaSessionManager ?: return false
        
        try {
            val controllers = manager.getActiveSessions(listenerComponent)
            if (controllers.isEmpty()) return false
            
            // الاتصال بأول جلسة نشطة
            return connectToController(controllers[0])
        } catch (e: SecurityException) {
            return false
        }
    }
    
    /**
     * الاتصال بمتحكم / Connect to controller
     */
    private fun connectToController(controller: MediaController): Boolean {
        // إلغاء الاتصال السابق
        disconnect()
        
        activeController = controller
        
        mediaCallback = object : MediaController.Callback() {
            override fun onMetadataChanged(metadata: MediaMetadata?) {
                metadata?.let {
                    val track = extractTrackInfo(it)
                    mainHandler.post {
                        mediaListeners.forEach { l -> l.onTrackChanged(track) }
                    }
                }
            }
            
            override fun onPlaybackStateChanged(state: PlaybackState?) {
                state?.let {
                    val playback = extractPlaybackInfo(it)
                    mainHandler.post {
                        mediaListeners.forEach { l -> l.onPlaybackStateChanged(playback) }
                    }
                }
            }
            
            override fun onSessionDestroyed() {
                mainHandler.post {
                    mediaListeners.forEach { it.onSessionDisconnected() }
                }
            }
        }
        
        controller.registerCallback(mediaCallback!!, mainHandler)
        
        mainHandler.post {
            mediaListeners.forEach { it.onSessionConnected(controller.packageName) }
        }
        
        return true
    }
    
    /**
     * قطع الاتصال / Disconnect
     */
    fun disconnect() {
        mediaCallback?.let { callback ->
            activeController?.unregisterCallback(callback)
        }
        activeController = null
        mediaCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحكم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تشغيل / Play
     */
    fun play(): Boolean {
        activeController?.transportControls?.play()
        return activeController != null
    }
    
    /**
     * إيقاف مؤقت / Pause
     */
    fun pause(): Boolean {
        activeController?.transportControls?.pause()
        return activeController != null
    }
    
    /**
     * تشغيل/إيقاف / Toggle play/pause
     */
    fun togglePlayPause(): Boolean {
        val controller = activeController ?: return false
        val state = controller.playbackState?.state
        
        if (state == PlaybackState.STATE_PLAYING) {
            controller.transportControls.pause()
        } else {
            controller.transportControls.play()
        }
        
        return true
    }
    
    /**
     * إيقاف / Stop
     */
    fun stop(): Boolean {
        activeController?.transportControls?.stop()
        return activeController != null
    }
    
    /**
     * التالي / Next
     */
    fun next(): Boolean {
        activeController?.transportControls?.skipToNext()
        return activeController != null
    }
    
    /**
     * السابق / Previous
     */
    fun previous(): Boolean {
        activeController?.transportControls?.skipToPrevious()
        return activeController != null
    }
    
    /**
     * التقديم / Seek forward
     */
    fun seekForward(milliseconds: Long = 10000): Boolean {
        val controller = activeController ?: return false
        val currentPosition = controller.playbackState?.position ?: 0
        controller.transportControls.seekTo(currentPosition + milliseconds)
        return true
    }
    
    /**
     * الترجيع / Seek backward
     */
    fun seekBackward(milliseconds: Long = 10000): Boolean {
        val controller = activeController ?: return false
        val currentPosition = controller.playbackState?.position ?: 0
        controller.transportControls.seekTo(maxOf(0, currentPosition - milliseconds))
        return true
    }
    
    /**
     * الانتقال إلى موضع / Seek to position
     */
    fun seekTo(position: Long): Boolean {
        activeController?.transportControls?.seekTo(position)
        return activeController != null
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على معلومات المسار الحالي / Get current track info
     */
    fun getCurrentTrack(): TrackInfo? {
        val metadata = activeController?.metadata ?: return null
        return extractTrackInfo(metadata)
    }
    
    /**
     * الحصول على حالة التشغيل / Get playback state
     */
    fun getPlaybackState(): PlaybackInfo? {
        val state = activeController?.playbackState ?: return null
        return extractPlaybackInfo(state)
    }
    
    /**
     * هل يشغل؟ / Is playing?
     */
    fun isPlaying(): Boolean {
        return activeController?.playbackState?.state == PlaybackState.STATE_PLAYING
    }
    
    /**
     * الحصول على اسم التطبيق / Get app name
     */
    fun getConnectedAppPackage(): String? {
        return activeController?.packageName
    }
    
    private fun extractTrackInfo(metadata: MediaMetadata): TrackInfo {
        return TrackInfo(
            title = metadata.getString(MediaMetadata.METADATA_KEY_TITLE) ?: "",
            artist = metadata.getString(MediaMetadata.METADATA_KEY_ARTIST) ?: "",
            album = metadata.getString(MediaMetadata.METADATA_KEY_ALBUM) ?: "",
            duration = metadata.getLong(MediaMetadata.METADATA_KEY_DURATION),
            position = activeController?.playbackState?.position ?: 0,
            albumArt = metadata.getBitmap(MediaMetadata.METADATA_KEY_ALBUM_ART),
            albumArtUri = metadata.getString(MediaMetadata.METADATA_KEY_ALBUM_ART_URI)
        )
    }
    
    private fun extractPlaybackInfo(state: PlaybackState): PlaybackInfo {
        val playState = when (state.state) {
            PlaybackState.STATE_PLAYING -> PlayState.PLAYING
            PlaybackState.STATE_PAUSED -> PlayState.PAUSED
            PlaybackState.STATE_STOPPED -> PlayState.STOPPED
            PlaybackState.STATE_BUFFERING -> PlayState.BUFFERING
            PlaybackState.STATE_ERROR -> PlayState.ERROR
            else -> PlayState.NONE
        }
        
        return PlaybackInfo(
            state = playState,
            position = state.position,
            speed = state.playbackSpeed,
            shuffleMode = ShuffleMode.OFF,  // يحتاج API 28+
            repeatMode = RepeatMode.NONE
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنسيق
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنسيق الوقت / Format time
     */
    fun formatDuration(milliseconds: Long): String {
        val totalSeconds = milliseconds / 1000
        val minutes = totalSeconds / 60
        val seconds = totalSeconds % 60
        return String.format("%d:%02d", minutes, seconds)
    }
    
    /**
     * حساب نسبة التقدم / Calculate progress percentage
     */
    fun calculateProgress(position: Long, duration: Long): Float {
        if (duration <= 0) return 0f
        return (position.toFloat() / duration.toFloat()).coerceIn(0f, 1f)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        disconnect()
        mediaListeners.clear()
        mediaSessionManager = null
        context = null
    }
}
