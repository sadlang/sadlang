/**
 * =============================================================================
 * @file SadAudio.kt
 * @brief (AR) واجهة الصوت لأندرويد — تشغيل وتسجيل
 * @brief (EN) Android Audio Interface — Playback and Recording
 * =============================================================================
 */

package sad.lang.android

import android.content.Context
import android.media.AudioAttributes
import android.media.MediaPlayer
import android.media.MediaRecorder
import android.media.SoundPool
import android.os.Build
import java.io.File
import java.io.IOException

/**
 * نظام الصوت للغة ص على أندرويد
 */
class SadAudio(private val context: Context) {
    
    companion object {
        private const val MAX_STREAMS = 10
        
        init {
            System.loadLibrary("sad_android")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Native Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    private external fun nativeInit(): Boolean
    private external fun nativeLoadSound(filePath: String): Int
    private external fun nativePlaySound(soundId: Int, loop: Boolean): Boolean
    private external fun nativeStopSound(soundId: Int)
    private external fun nativeStopAll()
    private external fun nativeSetVolume(volume: Float)
    private external fun nativeStartRecording(outputPath: String): Boolean
    private external fun nativeStopRecording(): Boolean
    private external fun nativeReleaseSound(soundId: Int)
    private external fun nativeShutdown()
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var soundPool: SoundPool? = null
    private var mediaPlayer: MediaPlayer? = null
    private var mediaRecorder: MediaRecorder? = null
    
    private val loadedSounds = mutableMapOf<Int, Int>()  // soundId -> poolLoadId
    private var currentVolume = 1.0f
    private var isRecording = false
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Initialization
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة نظام الصوت
     */
    fun init(): Boolean {
        soundPool = SoundPool.Builder()
            .setMaxStreams(MAX_STREAMS)
            .setAudioAttributes(
                AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_MEDIA)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                    .build()
            )
            .build()
        
        nativeInit()
        return true
    }
    
    /**
     * إنهاء نظام الصوت
     */
    fun shutdown() {
        stopAll()
        stopRecording()
        
        soundPool?.release()
        soundPool = null
        
        mediaPlayer?.release()
        mediaPlayer = null
        
        nativeShutdown()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Sound Effects (SoundPool)
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحميل مؤثر صوتي
     */
    fun loadSound(assetPath: String): Int {
        val pool = soundPool ?: return -1
        
        return try {
            val afd = context.assets.openFd(assetPath)
            val loadId = pool.load(afd, 1)
            val soundId = loadedSounds.size + 1
            loadedSounds[soundId] = loadId
            soundId
        } catch (e: Exception) {
            -1
        }
    }
    
    /**
     * تحميل مؤثر صوتي من ملف
     */
    fun loadSoundFile(filePath: String): Int {
        val pool = soundPool ?: return -1
        
        return try {
            val loadId = pool.load(filePath, 1)
            val soundId = loadedSounds.size + 1
            loadedSounds[soundId] = loadId
            soundId
        } catch (e: Exception) {
            -1
        }
    }
    
    /**
     * تشغيل مؤثر صوتي
     */
    fun playSound(soundId: Int, loop: Boolean = false): Boolean {
        val pool = soundPool ?: return false
        val loadId = loadedSounds[soundId] ?: return false
        
        val loopCount = if (loop) -1 else 0
        pool.play(loadId, currentVolume, currentVolume, 1, loopCount, 1.0f)
        return true
    }
    
    /**
     * إيقاف مؤثر صوتي
     */
    fun stopSound(soundId: Int) {
        val pool = soundPool ?: return
        val loadId = loadedSounds[soundId] ?: return
        pool.stop(loadId)
    }
    
    /**
     * تحرير مؤثر صوتي
     */
    fun releaseSound(soundId: Int) {
        val pool = soundPool ?: return
        val loadId = loadedSounds.remove(soundId) ?: return
        pool.unload(loadId)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Music Playback (MediaPlayer)
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تشغيل موسيقى
     */
    fun playMusic(assetPath: String, loop: Boolean = false): Boolean {
        stopMusic()
        
        return try {
            val afd = context.assets.openFd(assetPath)
            mediaPlayer = MediaPlayer().apply {
                setDataSource(afd.fileDescriptor, afd.startOffset, afd.length)
                isLooping = loop
                setVolume(currentVolume, currentVolume)
                prepare()
                start()
            }
            afd.close()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * تشغيل موسيقى من ملف
     */
    fun playMusicFile(filePath: String, loop: Boolean = false): Boolean {
        stopMusic()
        
        return try {
            mediaPlayer = MediaPlayer().apply {
                setDataSource(filePath)
                isLooping = loop
                setVolume(currentVolume, currentVolume)
                prepare()
                start()
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إيقاف الموسيقى مؤقتاً
     */
    fun pauseMusic() {
        mediaPlayer?.pause()
    }
    
    /**
     * استئناف الموسيقى
     */
    fun resumeMusic() {
        mediaPlayer?.start()
    }
    
    /**
     * إيقاف الموسيقى
     */
    fun stopMusic() {
        mediaPlayer?.apply {
            stop()
            release()
        }
        mediaPlayer = null
    }
    
    /**
     * هل الموسيقى قيد التشغيل؟
     */
    fun isMusicPlaying(): Boolean {
        return mediaPlayer?.isPlaying ?: false
    }
    
    /**
     * موضع الموسيقى الحالي
     */
    fun getMusicPosition(): Int {
        return mediaPlayer?.currentPosition ?: 0
    }
    
    /**
     * الانتقال لموضع
     */
    fun seekMusic(position: Int) {
        mediaPlayer?.seekTo(position)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Recording
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء التسجيل
     */
    fun startRecording(outputPath: String): Boolean {
        if (isRecording) return false
        
        return try {
            val file = File(outputPath)
            file.parentFile?.mkdirs()
            
            mediaRecorder = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                MediaRecorder(context)
            } else {
                @Suppress("DEPRECATION")
                MediaRecorder()
            }.apply {
                setAudioSource(MediaRecorder.AudioSource.MIC)
                setOutputFormat(MediaRecorder.OutputFormat.MPEG_4)
                setAudioEncoder(MediaRecorder.AudioEncoder.AAC)
                setAudioEncodingBitRate(128000)
                setAudioSamplingRate(44100)
                setOutputFile(outputPath)
                prepare()
                start()
            }
            
            isRecording = true
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إيقاف التسجيل
     */
    fun stopRecording(): Boolean {
        if (!isRecording) return false
        
        return try {
            mediaRecorder?.apply {
                stop()
                release()
            }
            mediaRecorder = null
            isRecording = false
            true
        } catch (e: Exception) {
            mediaRecorder = null
            isRecording = false
            false
        }
    }
    
    /**
     * هل التسجيل جاري؟
     */
    fun isRecording(): Boolean = isRecording
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Volume Control
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تعيين مستوى الصوت (0.0 - 1.0)
     */
    fun setVolume(volume: Float) {
        currentVolume = volume.coerceIn(0f, 1f)
        mediaPlayer?.setVolume(currentVolume, currentVolume)
        nativeSetVolume(currentVolume)
    }
    
    /**
     * الحصول على مستوى الصوت
     */
    fun getVolume(): Float = currentVolume
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Utilities
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إيقاف كل الأصوات
     */
    fun stopAll() {
        soundPool?.autoPause()
        stopMusic()
        nativeStopAll()
    }
    
    /**
     * استئناف كل الأصوات
     */
    fun resumeAll() {
        soundPool?.autoResume()
    }
}
