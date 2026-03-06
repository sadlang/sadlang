// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadVideoBridge.kt
// الوصف: جسر Kotlin للفيديو
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.graphics.Bitmap
import android.media.MediaMetadataRetriever
import android.net.Uri
import android.os.Build
import java.io.File

/**
 * جسر الفيديو — SadVideoBridge
 */
object SadVideoBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun getMetadata(path: String): Map<String, Any?> {
        val retriever = MediaMetadataRetriever()
        
        return try {
            retriever.setDataSource(path)
            
            val duration = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)?.toLongOrNull() ?: 0L
            val width = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH)?.toIntOrNull() ?: 0
            val height = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT)?.toIntOrNull() ?: 0
            val rotation = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION)?.toIntOrNull() ?: 0
            val bitrate = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_BITRATE)?.toIntOrNull() ?: 0
            val frameRate = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_CAPTURE_FRAMERATE)?.toFloatOrNull() ?: 0f
            } else 0f
            val mimeType = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_MIMETYPE) ?: ""
            val hasAudio = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_HAS_AUDIO) == "yes"
            val hasVideo = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_HAS_VIDEO) == "yes"
            val title = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE)
            val artist = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_ARTIST)
            val date = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DATE)
            
            mapOf(
                "duration" to duration,
                "durationFormatted" to formatDuration(duration),
                "width" to width,
                "height" to height,
                "rotation" to rotation,
                "bitrate" to bitrate,
                "frameRate" to frameRate,
                "mimeType" to mimeType,
                "hasAudio" to hasAudio,
                "hasVideo" to hasVideo,
                "title" to title,
                "artist" to artist,
                "date" to date,
                "size" to File(path).length()
            )
        } catch (e: Exception) {
            mapOf("error" to e.message)
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getDuration(path: String): Long {
        val retriever = MediaMetadataRetriever()
        return try {
            retriever.setDataSource(path)
            retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)?.toLongOrNull() ?: 0L
        } catch (e: Exception) {
            0L
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getDurationFromUri(uriString: String): Long {
        val ctx = context ?: return 0L
        val retriever = MediaMetadataRetriever()
        
        return try {
            retriever.setDataSource(ctx, Uri.parse(uriString))
            retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)?.toLongOrNull() ?: 0L
        } catch (e: Exception) {
            0L
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getResolution(path: String): Pair<Int, Int> {
        val retriever = MediaMetadataRetriever()
        return try {
            retriever.setDataSource(path)
            val width = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_WIDTH)?.toIntOrNull() ?: 0
            val height = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_HEIGHT)?.toIntOrNull() ?: 0
            Pair(width, height)
        } catch (e: Exception) {
            Pair(0, 0)
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getThumbnail(path: String): Bitmap? {
        val retriever = MediaMetadataRetriever()
        return try {
            retriever.setDataSource(path)
            retriever.getFrameAtTime(0, MediaMetadataRetriever.OPTION_CLOSEST_SYNC)
        } catch (e: Exception) {
            null
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getFrameAt(path: String, timeMs: Long): Bitmap? {
        val retriever = MediaMetadataRetriever()
        return try {
            retriever.setDataSource(path)
            // التحويل من ميلي ثانية إلى ميكرو ثانية
            retriever.getFrameAtTime(timeMs * 1000, MediaMetadataRetriever.OPTION_CLOSEST)
        } catch (e: Exception) {
            null
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getFramesAtIntervals(path: String, intervalMs: Long, count: Int): List<Bitmap> {
        val retriever = MediaMetadataRetriever()
        val frames = mutableListOf<Bitmap>()
        
        try {
            retriever.setDataSource(path)
            val duration = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)?.toLongOrNull() ?: 0L
            
            for (i in 0 until count) {
                val timeMs = i * intervalMs
                if (timeMs > duration) break
                
                val frame = retriever.getFrameAtTime(timeMs * 1000, MediaMetadataRetriever.OPTION_CLOSEST)
                if (frame != null) {
                    frames.add(frame)
                }
            }
        } catch (e: Exception) {
            // تجاهل الأخطاء
        } finally {
            retriever.release()
        }
        
        return frames
    }
    
    @JvmStatic
    fun extractThumbnailGrid(path: String, rows: Int = 3, cols: Int = 3): List<Bitmap> {
        val retriever = MediaMetadataRetriever()
        val frames = mutableListOf<Bitmap>()
        
        try {
            retriever.setDataSource(path)
            val duration = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION)?.toLongOrNull() ?: 0L
            
            val totalFrames = rows * cols
            val interval = duration / (totalFrames + 1)
            
            for (i in 1..totalFrames) {
                val timeMs = i * interval
                val frame = retriever.getFrameAtTime(timeMs * 1000, MediaMetadataRetriever.OPTION_CLOSEST)
                if (frame != null) {
                    frames.add(frame)
                }
            }
        } catch (e: Exception) {
            // تجاهل الأخطاء
        } finally {
            retriever.release()
        }
        
        return frames
    }
    
    @JvmStatic
    fun isValidVideo(path: String): Boolean {
        val retriever = MediaMetadataRetriever()
        return try {
            retriever.setDataSource(path)
            val hasVideo = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_HAS_VIDEO)
            hasVideo == "yes"
        } catch (e: Exception) {
            false
        } finally {
            retriever.release()
        }
    }
    
    @JvmStatic
    fun getRotation(path: String): Int {
        val retriever = MediaMetadataRetriever()
        return try {
            retriever.setDataSource(path)
            retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_VIDEO_ROTATION)?.toIntOrNull() ?: 0
        } catch (e: Exception) {
            0
        } finally {
            retriever.release()
        }
    }
    
    private fun formatDuration(ms: Long): String {
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
