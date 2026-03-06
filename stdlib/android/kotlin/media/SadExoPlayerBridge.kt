// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadExoPlayerBridge.kt
// الوصف: جسر Kotlin لمشغل الوسائط المتقدم ExoPlayer
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.net.Uri
import android.view.SurfaceView
import android.view.TextureView
import androidx.media3.common.*
import androidx.media3.exoplayer.ExoPlayer
import androidx.media3.exoplayer.source.DefaultMediaSourceFactory
import androidx.media3.exoplayer.trackselection.DefaultTrackSelector
import androidx.media3.ui.PlayerView

/**
 * جسر ExoPlayer — SadExoPlayerBridge
 */
object SadExoPlayerBridge {
    
    private var context: Context? = null
    private val players = mutableMapOf<String, ExoPlayer>()
    private var activePlayerId: String? = null
    
    private var onStateChangedCallback: ((String, Int, String) -> Unit)? = null
    private var onErrorCallback: ((String, String) -> Unit)? = null
    private var onMediaItemTransitionCallback: ((String, Int) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Player Lifecycle
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createPlayer(playerId: String = "default"): Boolean {
        val ctx = context ?: return false
        
        if (players.containsKey(playerId)) {
            return true // Already exists
        }
        
        return try {
            val trackSelector = DefaultTrackSelector(ctx).apply {
                setParameters(buildUponParameters().setMaxVideoSizeSd())
            }
            
            val player = ExoPlayer.Builder(ctx)
                .setTrackSelector(trackSelector)
                .build()
            
            setupPlayerListener(player, playerId)
            
            players[playerId] = player
            if (activePlayerId == null) {
                activePlayerId = playerId
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun releasePlayer(playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        try {
            player.release()
            players.remove(playerId)
            
            if (activePlayerId == playerId) {
                activePlayerId = players.keys.firstOrNull()
            }
            return true
        } catch (e: Exception) {
            return false
        }
    }
    
    @JvmStatic
    fun releaseAllPlayers() {
        players.values.forEach { it.release() }
        players.clear()
        activePlayerId = null
    }
    
    @JvmStatic
    fun setActivePlayer(playerId: String): Boolean {
        if (players.containsKey(playerId)) {
            activePlayerId = playerId
            return true
        }
        return false
    }
    
    @JvmStatic
    fun getActivePlayer(): ExoPlayer? = activePlayerId?.let { players[it] }
    
    @JvmStatic
    fun getPlayer(playerId: String): ExoPlayer? = players[playerId]
    
    private fun setupPlayerListener(player: ExoPlayer, playerId: String) {
        player.addListener(object : Player.Listener {
            override fun onPlaybackStateChanged(playbackState: Int) {
                val stateLabel = when (playbackState) {
                    Player.STATE_IDLE -> "خامل"
                    Player.STATE_BUFFERING -> "تحميل"
                    Player.STATE_READY -> "جاهز"
                    Player.STATE_ENDED -> "انتهى"
                    else -> "غير معروف"
                }
                onStateChangedCallback?.invoke(playerId, playbackState, stateLabel)
            }
            
            override fun onPlayerError(error: PlaybackException) {
                val errorMessage = when (error.errorCode) {
                    PlaybackException.ERROR_CODE_IO_NETWORK_CONNECTION_FAILED -> "فشل الاتصال بالشبكة"
                    PlaybackException.ERROR_CODE_IO_NETWORK_CONNECTION_TIMEOUT -> "انتهت مهلة الاتصال"
                    PlaybackException.ERROR_CODE_PARSING_CONTAINER_MALFORMED -> "ملف تالف"
                    PlaybackException.ERROR_CODE_DECODER_INIT_FAILED -> "فشل تهيئة المفكك"
                    PlaybackException.ERROR_CODE_AUDIO_TRACK_INIT_FAILED -> "فشل تهيئة الصوت"
                    else -> error.message ?: "خطأ غير معروف"
                }
                onErrorCallback?.invoke(playerId, errorMessage)
            }
            
            override fun onMediaItemTransition(mediaItem: MediaItem?, reason: Int) {
                onMediaItemTransitionCallback?.invoke(playerId, player.currentMediaItemIndex)
            }
        })
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Media Loading
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setMediaItem(url: String, playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            val mediaItem = MediaItem.fromUri(url)
            player.setMediaItem(mediaItem)
            player.prepare()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun setMediaItemWithTitle(
        url: String,
        title: String,
        artist: String? = null,
        artworkUri: String? = null,
        playerId: String = "default"
    ): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            val metadata = MediaMetadata.Builder()
                .setTitle(title)
                .apply {
                    artist?.let { setArtist(it) }
                    artworkUri?.let { setArtworkUri(Uri.parse(it)) }
                }
                .build()
            
            val mediaItem = MediaItem.Builder()
                .setUri(url)
                .setMediaMetadata(metadata)
                .build()
            
            player.setMediaItem(mediaItem)
            player.prepare()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun addMediaItem(url: String, playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            val mediaItem = MediaItem.fromUri(url)
            player.addMediaItem(mediaItem)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun addMediaItems(urls: List<String>, playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            val mediaItems = urls.map { MediaItem.fromUri(it) }
            player.addMediaItems(mediaItems)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun setPlaylist(
        items: List<Map<String, String>>,
        playerId: String = "default"
    ): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            val mediaItems = items.map { item ->
                val url = item["url"] ?: return false
                val title = item["title"]
                val artist = item["artist"]
                
                MediaItem.Builder()
                    .setUri(url)
                    .apply {
                        if (title != null || artist != null) {
                            setMediaMetadata(
                                MediaMetadata.Builder()
                                    .apply {
                                        title?.let { setTitle(it) }
                                        artist?.let { setArtist(it) }
                                    }
                                    .build()
                            )
                        }
                    }
                    .build()
            }
            
            player.setMediaItems(mediaItems)
            player.prepare()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun removeMediaItem(index: Int, playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            player.removeMediaItem(index)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun clearMediaItems(playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            player.clearMediaItems()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Playback Control
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun play(playerId: String = "default") {
        players[playerId]?.play()
    }
    
    @JvmStatic
    fun pause(playerId: String = "default") {
        players[playerId]?.pause()
    }
    
    @JvmStatic
    fun stop(playerId: String = "default") {
        players[playerId]?.stop()
    }
    
    @JvmStatic
    fun seekTo(positionMs: Long, playerId: String = "default") {
        players[playerId]?.seekTo(positionMs)
    }
    
    @JvmStatic
    fun seekToItem(itemIndex: Int, positionMs: Long = 0, playerId: String = "default") {
        players[playerId]?.seekTo(itemIndex, positionMs)
    }
    
    @JvmStatic
    fun seekForward(ms: Long = 10000, playerId: String = "default") {
        val player = players[playerId] ?: return
        val newPosition = player.currentPosition + ms
        player.seekTo(newPosition.coerceAtMost(player.duration))
    }
    
    @JvmStatic
    fun seekBackward(ms: Long = 10000, playerId: String = "default") {
        val player = players[playerId] ?: return
        val newPosition = player.currentPosition - ms
        player.seekTo(newPosition.coerceAtLeast(0))
    }
    
    @JvmStatic
    fun next(playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return if (player.hasNextMediaItem()) {
            player.seekToNextMediaItem()
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun previous(playerId: String = "default"): Boolean {
        val player = players[playerId] ?: return false
        
        return if (player.hasPreviousMediaItem()) {
            player.seekToPreviousMediaItem()
            true
        } else {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Playback Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setVolume(volume: Float, playerId: String = "default") {
        players[playerId]?.volume = volume.coerceIn(0f, 1f)
    }
    
    @JvmStatic
    fun getVolume(playerId: String = "default"): Float {
        return players[playerId]?.volume ?: 0f
    }
    
    @JvmStatic
    fun setPlaybackSpeed(speed: Float, playerId: String = "default") {
        players[playerId]?.setPlaybackSpeed(speed.coerceIn(0.25f, 4f))
    }
    
    @JvmStatic
    fun getPlaybackSpeed(playerId: String = "default"): Float {
        return players[playerId]?.playbackParameters?.speed ?: 1f
    }
    
    @JvmStatic
    fun setRepeatMode(mode: String, playerId: String = "default") {
        val repeatMode = when (mode.uppercase()) {
            "OFF" -> Player.REPEAT_MODE_OFF
            "ONE" -> Player.REPEAT_MODE_ONE
            "ALL" -> Player.REPEAT_MODE_ALL
            else -> Player.REPEAT_MODE_OFF
        }
        players[playerId]?.repeatMode = repeatMode
    }
    
    @JvmStatic
    fun getRepeatMode(playerId: String = "default"): String {
        return when (players[playerId]?.repeatMode) {
            Player.REPEAT_MODE_OFF -> "OFF"
            Player.REPEAT_MODE_ONE -> "ONE"
            Player.REPEAT_MODE_ALL -> "ALL"
            else -> "OFF"
        }
    }
    
    @JvmStatic
    fun setShuffleEnabled(enabled: Boolean, playerId: String = "default") {
        players[playerId]?.shuffleModeEnabled = enabled
    }
    
    @JvmStatic
    fun isShuffleEnabled(playerId: String = "default"): Boolean {
        return players[playerId]?.shuffleModeEnabled ?: false
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Playback State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isPlaying(playerId: String = "default"): Boolean {
        return players[playerId]?.isPlaying ?: false
    }
    
    @JvmStatic
    fun isLoading(playerId: String = "default"): Boolean {
        return players[playerId]?.isLoading ?: false
    }
    
    @JvmStatic
    fun getCurrentPosition(playerId: String = "default"): Long {
        return players[playerId]?.currentPosition ?: 0L
    }
    
    @JvmStatic
    fun getDuration(playerId: String = "default"): Long {
        return players[playerId]?.duration ?: 0L
    }
    
    @JvmStatic
    fun getBufferedPosition(playerId: String = "default"): Long {
        return players[playerId]?.bufferedPosition ?: 0L
    }
    
    @JvmStatic
    fun getBufferedPercentage(playerId: String = "default"): Int {
        return players[playerId]?.bufferedPercentage ?: 0
    }
    
    @JvmStatic
    fun getCurrentMediaItemIndex(playerId: String = "default"): Int {
        return players[playerId]?.currentMediaItemIndex ?: -1
    }
    
    @JvmStatic
    fun getMediaItemCount(playerId: String = "default"): Int {
        return players[playerId]?.mediaItemCount ?: 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Video Surface
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setPlayerView(playerView: PlayerView, playerId: String = "default") {
        playerView.player = players[playerId]
    }
    
    @JvmStatic
    fun setVideoSurfaceView(surfaceView: SurfaceView, playerId: String = "default") {
        players[playerId]?.setVideoSurfaceView(surfaceView)
    }
    
    @JvmStatic
    fun setVideoTextureView(textureView: TextureView, playerId: String = "default") {
        players[playerId]?.setVideoTextureView(textureView)
    }
    
    @JvmStatic
    fun clearVideoSurface(playerId: String = "default") {
        players[playerId]?.clearVideoSurface()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnStateChangedListener(callback: (String, Int, String) -> Unit) {
        onStateChangedCallback = callback
    }
    
    @JvmStatic
    fun setOnErrorListener(callback: (String, String) -> Unit) {
        onErrorCallback = callback
    }
    
    @JvmStatic
    fun setOnMediaItemTransitionListener(callback: (String, Int) -> Unit) {
        onMediaItemTransitionCallback = callback
    }
    
    @JvmStatic
    fun removeAllListeners() {
        onStateChangedCallback = null
        onErrorCallback = null
        onMediaItemTransitionCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Player Info
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getPlayerInfo(playerId: String = "default"): Map<String, Any?> {
        val player = players[playerId] ?: return emptyMap()
        
        return mapOf(
            "isPlaying" to player.isPlaying,
            "isLoading" to player.isLoading,
            "currentPosition" to player.currentPosition,
            "duration" to player.duration,
            "bufferedPosition" to player.bufferedPosition,
            "bufferedPercentage" to player.bufferedPercentage,
            "volume" to player.volume,
            "playbackSpeed" to player.playbackParameters.speed,
            "repeatMode" to getRepeatMode(playerId),
            "shuffleEnabled" to player.shuffleModeEnabled,
            "currentItemIndex" to player.currentMediaItemIndex,
            "mediaItemCount" to player.mediaItemCount,
            "playWhenReady" to player.playWhenReady
        )
    }
    
    @JvmStatic
    fun getPlayerIds(): List<String> = players.keys.toList()
}
