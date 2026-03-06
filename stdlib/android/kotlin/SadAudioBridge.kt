// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAudioBridge.kt
// الوصف: جسر Kotlin للصوت في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_صوت_* بـ Android Media APIs
//
// الدوال المُنفّذة:
//   __اندرويد_صوت_شغل(path) -> playerId
//   __اندرويد_صوت_اوقف(playerId) -> bool
//   __اندرويد_صوت_ايقاف_مؤقت(playerId) -> bool
//   __اندرويد_صوت_استمر(playerId) -> bool
//   __اندرويد_صوت_مستوى(playerId, volume) -> bool
//   __اندرويد_صوت_سجل(path, duration) -> bool
//
// المتطلبات في AndroidManifest.xml:
//   <uses-permission android:name="android.permission.RECORD_AUDIO" />
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.media.AudioAttributes
import android.media.MediaPlayer
import android.media.MediaRecorder
import android.os.Build
import java.io.File
import java.io.IOException

/**
 * جسر الصوت — SadAudioBridge
 * 
 * يوفر واجهة لتشغيل وتسجيل الصوت من كود لغة ص.
 */
object SadAudioBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var appContext: Context? = null
    
    // تخزين المشغلات النشطة
    private val players = mutableMapOf<Int, MediaPlayer>()
    private var playerIdCounter = 0
    
    // المسجل
    private var recorder: MediaRecorder? = null
    private var isRecording = false
    
    /**
     * تهيئة الجسر
     */
    @JvmStatic
    fun initialize(context: Context) {
        appContext = context.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_شغل — تشغيل صوت
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تشغيل ملف صوتي
     * 
     * @param path مسار الملف (محلي أو URL)
     * @param loop تكرار؟
     * @param volume مستوى الصوت (0.0 - 1.0)
     * @param onComplete دالة عند الانتهاء
     * @return معرف المشغل للتحكم لاحقاً، أو -1 عند الفشل
     */
    @JvmStatic
    fun play(
        path: String,
        loop: Boolean = false,
        volume: Float = 1.0f,
        onComplete: (() -> Unit)? = null
    ): Int {
        return try {
            val player = MediaPlayer().apply {
                setAudioAttributes(
                    AudioAttributes.Builder()
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .build()
                )
                
                // تحديد المصدر
                if (path.startsWith("http://") || path.startsWith("https://")) {
                    setDataSource(path)
                } else {
                    setDataSource(path)
                }
                
                isLooping = loop
                setVolume(volume, volume)
                
                setOnCompletionListener {
                    onComplete?.invoke()
                    if (!loop) {
                        val id = players.entries.find { it.value == this }?.key
                        id?.let { stop(it) }
                    }
                }
                
                setOnErrorListener { _, what, extra ->
                    println("خطأ في تشغيل الصوت: $what, $extra")
                    false
                }
                
                prepareAsync()
                setOnPreparedListener { start() }
            }
            
            val id = ++playerIdCounter
            players[id] = player
            id
            
        } catch (e: Exception) {
            e.printStackTrace()
            -1
        }
    }
    
    /**
     * تشغيل من الموارد (res/raw/)
     */
    @JvmStatic
    fun playResource(resourceId: Int, loop: Boolean = false): Int {
        val context = appContext ?: return -1
        
        return try {
            val player = MediaPlayer.create(context, resourceId).apply {
                isLooping = loop
                start()
            }
            
            val id = ++playerIdCounter
            players[id] = player
            id
            
        } catch (e: Exception) {
            e.printStackTrace()
            -1
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_اوقف — إيقاف تشغيل
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إيقاف تشغيل صوت
     * 
     * @param playerId معرف المشغل
     * @return true إذا نجحت العملية
     */
    @JvmStatic
    fun stop(playerId: Int): Boolean {
        val player = players.remove(playerId) ?: return false
        
        return try {
            if (player.isPlaying) {
                player.stop()
            }
            player.release()
            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
    
    /**
     * إيقاف جميع المشغلات
     */
    @JvmStatic
    fun stopAll() {
        players.keys.toList().forEach { stop(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_ايقاف_مؤقت — إيقاف مؤقت
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إيقاف مؤقت
     */
    @JvmStatic
    fun pause(playerId: Int): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            if (player.isPlaying) {
                player.pause()
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_استمر — استئناف التشغيل
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * استئناف التشغيل
     */
    @JvmStatic
    fun resume(playerId: Int): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            if (!player.isPlaying) {
                player.start()
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_مستوى — ضبط مستوى الصوت
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * ضبط مستوى الصوت
     * 
     * @param playerId معرف المشغل
     * @param volume مستوى الصوت (0.0 - 1.0)
     */
    @JvmStatic
    fun setVolume(playerId: Int, volume: Float): Boolean {
        val player = players[playerId] ?: return false
        val clampedVolume = volume.coerceIn(0f, 1f)
        
        return try {
            player.setVolume(clampedVolume, clampedVolume)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_موضع — الموضع الحالي
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الموضع الحالي (بالملي ثانية)
     */
    @JvmStatic
    fun getPosition(playerId: Int): Int {
        return players[playerId]?.currentPosition ?: 0
    }
    
    /**
     * الحصول على المدة الإجمالية (بالملي ثانية)
     */
    @JvmStatic
    fun getDuration(playerId: Int): Int {
        return players[playerId]?.duration ?: 0
    }
    
    /**
     * الانتقال إلى موضع
     */
    @JvmStatic
    fun seekTo(playerId: Int, positionMs: Int): Boolean {
        val player = players[playerId] ?: return false
        
        return try {
            player.seekTo(positionMs)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_صوت_سجل — تسجيل صوت
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * بدء تسجيل صوت
     * 
     * @param outputPath مسار الملف الناتج
     * @return true إذا نجح البدء
     */
    @JvmStatic
    fun startRecording(outputPath: String): Boolean {
        if (isRecording) {
            stopRecording()
        }
        
        return try {
            // إنشاء المجلد إذا لم يكن موجوداً
            File(outputPath).parentFile?.mkdirs()
            
            recorder = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                MediaRecorder(appContext!!)
            } else {
                @Suppress("DEPRECATION")
                MediaRecorder()
            }.apply {
                setAudioSource(MediaRecorder.AudioSource.MIC)
                setOutputFormat(MediaRecorder.OutputFormat.MPEG_4)
                setAudioEncoder(MediaRecorder.AudioEncoder.AAC)
                setAudioSamplingRate(44100)
                setAudioEncodingBitRate(128000)
                setOutputFile(outputPath)
                
                prepare()
                start()
            }
            
            isRecording = true
            true
            
        } catch (e: Exception) {
            e.printStackTrace()
            recorder?.release()
            recorder = null
            false
        }
    }
    
    /**
     * إيقاف التسجيل
     * 
     * @return true إذا نجح الإيقاف
     */
    @JvmStatic
    fun stopRecording(): Boolean {
        if (!isRecording) return false
        
        return try {
            recorder?.apply {
                stop()
                release()
            }
            recorder = null
            isRecording = false
            true
        } catch (e: Exception) {
            e.printStackTrace()
            recorder?.release()
            recorder = null
            isRecording = false
            false
        }
    }
    
    /**
     * هل يجري التسجيل؟
     */
    @JvmStatic
    fun isCurrentlyRecording(): Boolean = isRecording
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تنظيف الموارد
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنظيف جميع الموارد
     */
    @JvmStatic
    fun cleanup() {
        stopAll()
        stopRecording()
        appContext = null
    }
}
