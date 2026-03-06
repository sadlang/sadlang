// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAudioRecorderBridge.kt
// الوصف: جسر Kotlin لتسجيل الصوت
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.media.MediaRecorder
import android.os.Build
import java.io.File
import java.text.SimpleDateFormat
import java.util.*

/**
 * جسر تسجيل الصوت — SadAudioRecorderBridge
 */
object SadAudioRecorderBridge {
    
    private var context: Context? = null
    private var recorder: MediaRecorder? = null
    private var isRecording = false
    private var isPaused = false
    private var outputPath: String? = null
    private var startTime: Long = 0
    private var pausedDuration: Long = 0
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun startRecording(outputFile: String, quality: String = "medium"): Boolean {
        if (isRecording) return false
        
        return try {
            val ctx = context ?: return false
            
            recorder = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                MediaRecorder(ctx)
            } else {
                @Suppress("DEPRECATION")
                MediaRecorder()
            }
            
            recorder?.apply {
                setAudioSource(MediaRecorder.AudioSource.MIC)
                setOutputFormat(MediaRecorder.OutputFormat.MPEG_4)
                setAudioEncoder(MediaRecorder.AudioEncoder.AAC)
                
                when (quality.lowercase()) {
                    "low" -> {
                        setAudioEncodingBitRate(64000)
                        setAudioSamplingRate(22050)
                    }
                    "high" -> {
                        setAudioEncodingBitRate(256000)
                        setAudioSamplingRate(48000)
                    }
                    else -> { // medium
                        setAudioEncodingBitRate(128000)
                        setAudioSamplingRate(44100)
                    }
                }
                
                setOutputFile(outputFile)
                prepare()
                start()
            }
            
            outputPath = outputFile
            isRecording = true
            isPaused = false
            startTime = System.currentTimeMillis()
            pausedDuration = 0
            true
        } catch (e: Exception) {
            recorder?.release()
            recorder = null
            false
        }
    }
    
    @JvmStatic
    fun startRecordingWithDefaultPath(quality: String = "medium"): String? {
        val ctx = context ?: return null
        
        val timestamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.getDefault()).format(Date())
        val outputDir = File(ctx.getExternalFilesDir(null), "recordings")
        outputDir.mkdirs()
        val outputFile = File(outputDir, "REC_$timestamp.m4a")
        
        return if (startRecording(outputFile.absolutePath, quality)) {
            outputFile.absolutePath
        } else {
            null
        }
    }
    
    @JvmStatic
    fun stopRecording(): String? {
        if (!isRecording) return null
        
        return try {
            recorder?.apply {
                stop()
                release()
            }
            recorder = null
            isRecording = false
            isPaused = false
            outputPath
        } catch (e: Exception) {
            recorder?.release()
            recorder = null
            isRecording = false
            isPaused = false
            null
        }
    }
    
    @JvmStatic
    fun pauseRecording(): Boolean {
        if (!isRecording || isPaused) return false
        
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                recorder?.pause()
                isPaused = true
                pausedDuration = System.currentTimeMillis() - startTime - pausedDuration
                true
            } else {
                false
            }
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun resumeRecording(): Boolean {
        if (!isRecording || !isPaused) return false
        
        return try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                recorder?.resume()
                isPaused = false
                startTime = System.currentTimeMillis() - pausedDuration
                true
            } else {
                false
            }
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun cancelRecording(): Boolean {
        if (!isRecording) return false
        
        return try {
            recorder?.apply {
                stop()
                release()
            }
            recorder = null
            
            // حذف الملف
            outputPath?.let { File(it).delete() }
            
            isRecording = false
            isPaused = false
            outputPath = null
            true
        } catch (e: Exception) {
            recorder?.release()
            recorder = null
            isRecording = false
            isPaused = false
            false
        }
    }
    
    @JvmStatic
    fun isRecording(): Boolean = isRecording
    
    @JvmStatic
    fun isPaused(): Boolean = isPaused
    
    @JvmStatic
    fun getRecordingDuration(): Long {
        if (!isRecording) return 0
        
        return if (isPaused) {
            pausedDuration
        } else {
            System.currentTimeMillis() - startTime
        }
    }
    
    @JvmStatic
    fun getRecordingDurationFormatted(): String {
        val ms = getRecordingDuration()
        val seconds = (ms / 1000) % 60
        val minutes = (ms / (1000 * 60)) % 60
        val hours = ms / (1000 * 60 * 60)
        
        return if (hours > 0) {
            String.format("%02d:%02d:%02d", hours, minutes, seconds)
        } else {
            String.format("%02d:%02d", minutes, seconds)
        }
    }
    
    @JvmStatic
    fun getMaxAmplitude(): Int {
        return try {
            recorder?.maxAmplitude ?: 0
        } catch (e: Exception) {
            0
        }
    }
    
    @JvmStatic
    fun getAmplitudeLevel(): String {
        val amplitude = getMaxAmplitude()
        return when {
            amplitude < 1000 -> "صامت"
            amplitude < 5000 -> "منخفض"
            amplitude < 15000 -> "متوسط"
            amplitude < 25000 -> "مرتفع"
            else -> "عالي جداً"
        }
    }
    
    @JvmStatic
    fun getOutputPath(): String? = outputPath
    
    @JvmStatic
    fun supportsNoiseSuppression(): Boolean {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN
    }
    
    @JvmStatic
    fun cleanup() {
        try {
            recorder?.release()
        } catch (e: Exception) {
            // تجاهل
        }
        recorder = null
        isRecording = false
        isPaused = false
        outputPath = null
    }
}
