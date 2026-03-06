// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadVideoPlayerBridge.kt
// الوصف: جسر Kotlin لمشغل الفيديو
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.media.AudioAttributes
import android.media.MediaPlayer
import android.net.Uri
import android.view.Surface
import android.view.SurfaceHolder

/**
 * جسر مشغل الفيديو — SadVideoPlayerBridge
 */
object SadVideoPlayerBridge {
    
    private var context: Context? = null
    private var mediaPlayer: MediaPlayer? = null
    private var isPrepared = false
    private var currentPath: String? = null
    
    private var onCompletionCallback: (() -> Unit)? = null
    private var onErrorCallback: ((String) -> Unit)? = null
    private var onPreparedCallback: (() -> Unit)? = null
    private var onBufferingCallback: ((Int) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun setSource(path: String): Boolean {
        release()
        
        return try {
            mediaPlayer = MediaPlayer().apply {
                setDataSource(path)
                setAudioAttributes(
                    AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MOVIE)
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .build()
                )
            }
            currentPath = path
            true
        } catch (e: Exception) {
            onErrorCallback?.invoke(e.message ?: "فشل في تحميل الملف")
            false
        }
    }
    
    @JvmStatic
    fun setSourceUri(uriString: String): Boolean {
        val ctx = context ?: return false
        release()
        
        return try {
            mediaPlayer = MediaPlayer().apply {
                setDataSource(ctx, Uri.parse(uriString))
                setAudioAttributes(
                    AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MOVIE)
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .build()
                )
            }
            currentPath = uriString
            true
        } catch (e: Exception) {
            onErrorCallback?.invoke(e.message ?: "فشل في تحميل الرابط")
            false
        }
    }
    
    @JvmStatic
    fun prepare(): Boolean {
        return try {
            mediaPlayer?.apply {
                setOnPreparedListener {
                    isPrepared = true
                    onPreparedCallback?.invoke()
                }
                setOnCompletionListener {
                    onCompletionCallback?.invoke()
                }
                setOnErrorListener { _, what, extra ->
                    onErrorCallback?.invoke("خطأ: $what, $extra")
                    true
                }
                setOnBufferingUpdateListener { _, percent ->
                    onBufferingCallback?.invoke(percent)
                }
                prepareAsync()
            }
            true
        } catch (e: Exception) {
            onErrorCallback?.invoke(e.message ?: "فشل في التحضير")
            false
        }
    }
    
    @JvmStatic
    fun prepareSync(): Boolean {
        return try {
            mediaPlayer?.prepare()
            isPrepared = true
            true
        } catch (e: Exception) {
            onErrorCallback?.invoke(e.message ?: "فشل في التحضير")
            false
        }
    }
    
    @JvmStatic
    fun setSurface(surface: Surface) {
        mediaPlayer?.setSurface(surface)
    }
    
    @JvmStatic
    fun setSurfaceHolder(holder: SurfaceHolder) {
        mediaPlayer?.setDisplay(holder)
    }
    
    @JvmStatic
    fun play(): Boolean {
        if (!isPrepared) return false
        
        return try {
            mediaPlayer?.start()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun pause(): Boolean {
        return try {
            mediaPlayer?.pause()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun stop(): Boolean {
        return try {
            mediaPlayer?.stop()
            isPrepared = false
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun seekTo(positionMs: Int): Boolean {
        return try {
            mediaPlayer?.seekTo(positionMs)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun seekToPercent(percent: Float): Boolean {
        val duration = getDuration()
        if (duration <= 0) return false
        
        val position = (duration * percent / 100).toInt()
        return seekTo(position)
    }
    
    @JvmStatic
    fun isPlaying(): Boolean = mediaPlayer?.isPlaying == true
    
    @JvmStatic
    fun isPrepared(): Boolean = isPrepared
    
    @JvmStatic
    fun getCurrentPosition(): Int = mediaPlayer?.currentPosition ?: 0
    
    @JvmStatic
    fun getDuration(): Int = mediaPlayer?.duration ?: 0
    
    @JvmStatic
    fun getProgress(): Float {
        val duration = getDuration()
        if (duration <= 0) return 0f
        return (getCurrentPosition().toFloat() / duration) * 100
    }
    
    @JvmStatic
    fun getCurrentPositionFormatted(): String {
        return formatTime(getCurrentPosition())
    }
    
    @JvmStatic
    fun getDurationFormatted(): String {
        return formatTime(getDuration())
    }
    
    @JvmStatic
    fun getRemainingTime(): Int {
        return getDuration() - getCurrentPosition()
    }
    
    @JvmStatic
    fun getRemainingTimeFormatted(): String {
        return formatTime(getRemainingTime())
    }
    
    @JvmStatic
    fun setVolume(left: Float, right: Float) {
        mediaPlayer?.setVolume(left.coerceIn(0f, 1f), right.coerceIn(0f, 1f))
    }
    
    @JvmStatic
    fun setVolume(volume: Float) {
        val v = volume.coerceIn(0f, 1f)
        mediaPlayer?.setVolume(v, v)
    }
    
    @JvmStatic
    fun mute() {
        setVolume(0f, 0f)
    }
    
    @JvmStatic
    fun unmute() {
        setVolume(1f, 1f)
    }
    
    @JvmStatic
    fun setLooping(loop: Boolean) {
        mediaPlayer?.isLooping = loop
    }
    
    @JvmStatic
    fun isLooping(): Boolean = mediaPlayer?.isLooping == true
    
    @JvmStatic
    fun setPlaybackSpeed(speed: Float): Boolean {
        return try {
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
                val playbackParams = mediaPlayer?.playbackParams
                playbackParams?.speed = speed.coerceIn(0.25f, 4f)
                if (playbackParams != null) {
                    mediaPlayer?.playbackParams = playbackParams
                }
                true
            } else {
                false
            }
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getVideoWidth(): Int = mediaPlayer?.videoWidth ?: 0
    
    @JvmStatic
    fun getVideoHeight(): Int = mediaPlayer?.videoHeight ?: 0
    
    @JvmStatic
    fun getAspectRatio(): Float {
        val width = getVideoWidth()
        val height = getVideoHeight()
        if (height <= 0) return 0f
        return width.toFloat() / height
    }
    
    @JvmStatic
    fun setOnCompletionListener(callback: () -> Unit) {
        onCompletionCallback = callback
    }
    
    @JvmStatic
    fun setOnErrorListener(callback: (String) -> Unit) {
        onErrorCallback = callback
    }
    
    @JvmStatic
    fun setOnPreparedListener(callback: () -> Unit) {
        onPreparedCallback = callback
    }
    
    @JvmStatic
    fun setOnBufferingUpdateListener(callback: (Int) -> Unit) {
        onBufferingCallback = callback
    }
    
    @JvmStatic
    fun release() {
        try {
            mediaPlayer?.release()
        } catch (e: Exception) {
            // تجاهل
        }
        mediaPlayer = null
        isPrepared = false
        currentPath = null
    }
    
    @JvmStatic
    fun getState(): String {
        return when {
            mediaPlayer == null -> "غير مهيأ"
            !isPrepared -> "قيد التحضير"
            isPlaying() -> "يعمل"
            else -> "متوقف"
        }
    }
    
    private fun formatTime(ms: Int): String {
        val totalSeconds = ms / 1000
        val hours = totalSeconds / 3600
        val minutes = (totalSeconds % 3600) / 60
        val seconds = totalSeconds % 60
        
        return if (hours > 0) {
            String.format("%02d:%02d:%02d", hours, minutes, seconds)
        } else {
            String.format("%02d:%02d", minutes, seconds)
        }
    }
}
