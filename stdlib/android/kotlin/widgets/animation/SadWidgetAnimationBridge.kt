package sad.android.widgets.animation

import android.animation.ValueAnimator
import android.content.Context
import android.os.Handler
import android.os.Looper
import android.widget.RemoteViews
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر رسوم الويدجت المتحركة - تأثيرات بصرية
 * Widget Animation Bridge - Visual effects
 * 
 * @author فريق لغة ص
 * 
 * ملاحظة: RemoteViews لديها دعم محدود للرسوم المتحركة
 * لذلك نستخدم تحديثات متتالية لمحاكاة الحركة
 */
object SadWidgetAnimationBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val activeAnimations = ConcurrentHashMap<String, AnimationTask>()
    
    /**
     * مهمة الرسوم المتحركة / Animation task
     */
    data class AnimationTask(
        val id: String,
        val widgetId: Int,
        var isRunning: Boolean = true,
        var currentFrame: Int = 0
    )
    
    /**
     * نوع الرسوم المتحركة / Animation type
     */
    enum class AnimationType {
        FADE,
        PULSE,
        BLINK,
        PROGRESS,
        COUNTER,
        TYPEWRITER
    }
    
    /**
     * خيارات الرسوم المتحركة / Animation options
     */
    data class AnimationOptions(
        val duration: Long = 1000,
        val repeatCount: Int = 0,  // 0 = لا تكرار، -1 = لا نهائي
        val delay: Long = 0,
        val interpolator: Interpolator = Interpolator.LINEAR
    )
    
    /**
     * المُقحم / Interpolator
     */
    enum class Interpolator {
        LINEAR,
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        BOUNCE
    }
    
    /**
     * مستمع الرسوم المتحركة / Animation listener
     */
    interface AnimationListener {
        fun onAnimationStart(animationId: String)
        fun onAnimationEnd(animationId: String)
        fun onAnimationUpdate(animationId: String, progress: Float)
    }
    
    private val animationListeners = ConcurrentHashMap<String, AnimationListener>()
    
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
     * إضافة مستمع / Add listener
     */
    fun setAnimationListener(animationId: String, listener: AnimationListener) {
        animationListeners[animationId] = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الرسوم المتحركة النصية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تأثير الآلة الكاتبة / Typewriter effect
     */
    fun animateTypewriter(
        widgetId: Int,
        text: String,
        intervalMs: Long = 100,
        updateCallback: (String) -> Unit
    ): String {
        val animId = "typewriter:${widgetId}:${System.currentTimeMillis()}"
        val task = AnimationTask(animId, widgetId)
        activeAnimations[animId] = task
        
        var index = 0
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning || index > text.length) {
                    task.isRunning = false
                    activeAnimations.remove(animId)
                    animationListeners[animId]?.onAnimationEnd(animId)
                    return
                }
                
                val currentText = text.substring(0, index)
                mainHandler.post { updateCallback(currentText) }
                
                index++
                mainHandler.postDelayed(this, intervalMs)
            }
        }
        
        animationListeners[animId]?.onAnimationStart(animId)
        mainHandler.post(runnable)
        
        return animId
    }
    
    /**
     * تأثير العداد / Counter effect
     */
    fun animateCounter(
        widgetId: Int,
        from: Int,
        to: Int,
        durationMs: Long = 1000,
        updateCallback: (Int) -> Unit
    ): String {
        val animId = "counter:${widgetId}:${System.currentTimeMillis()}"
        val task = AnimationTask(animId, widgetId)
        activeAnimations[animId] = task
        
        val frames = (durationMs / 16).toInt() // ~60 FPS
        val step = (to - from).toFloat() / frames
        var current = from.toFloat()
        var frame = 0
        
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning || frame >= frames) {
                    mainHandler.post { updateCallback(to) }
                    task.isRunning = false
                    activeAnimations.remove(animId)
                    animationListeners[animId]?.onAnimationEnd(animId)
                    return
                }
                
                current += step
                mainHandler.post { updateCallback(current.toInt()) }
                
                frame++
                mainHandler.postDelayed(this, 16)
            }
        }
        
        animationListeners[animId]?.onAnimationStart(animId)
        mainHandler.post(runnable)
        
        return animId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الرسوم المتحركة للتقدم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحريك شريط التقدم / Animate progress bar
     */
    fun animateProgress(
        widgetId: Int,
        from: Int,
        to: Int,
        durationMs: Long = 1000,
        updateCallback: (Int) -> Unit
    ): String {
        return animateCounter(widgetId, from, to, durationMs, updateCallback)
    }
    
    /**
     * تقدم غير محدد / Indeterminate progress
     */
    fun animateIndeterminateProgress(
        widgetId: Int,
        updateCallback: (Int) -> Unit,
        intervalMs: Long = 50
    ): String {
        val animId = "indeterminate:${widgetId}:${System.currentTimeMillis()}"
        val task = AnimationTask(animId, widgetId)
        activeAnimations[animId] = task
        
        var progress = 0
        var increasing = true
        
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning) {
                    activeAnimations.remove(animId)
                    return
                }
                
                if (increasing) {
                    progress += 5
                    if (progress >= 100) increasing = false
                } else {
                    progress -= 5
                    if (progress <= 0) increasing = true
                }
                
                mainHandler.post { updateCallback(progress) }
                mainHandler.postDelayed(this, intervalMs)
            }
        }
        
        mainHandler.post(runnable)
        return animId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الرسوم المتحركة الوميضية
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تأثير الوميض / Blink effect
     */
    fun animateBlink(
        widgetId: Int,
        intervalMs: Long = 500,
        count: Int = -1,  // -1 = لا نهائي
        updateCallback: (Boolean) -> Unit
    ): String {
        val animId = "blink:${widgetId}:${System.currentTimeMillis()}"
        val task = AnimationTask(animId, widgetId)
        activeAnimations[animId] = task
        
        var visible = true
        var blinkCount = 0
        
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning || (count > 0 && blinkCount >= count * 2)) {
                    task.isRunning = false
                    activeAnimations.remove(animId)
                    mainHandler.post { updateCallback(true) } // إظهار في النهاية
                    return
                }
                
                visible = !visible
                blinkCount++
                mainHandler.post { updateCallback(visible) }
                mainHandler.postDelayed(this, intervalMs)
            }
        }
        
        mainHandler.post(runnable)
        return animId
    }
    
    /**
     * تأثير النبض / Pulse effect
     */
    fun animatePulse(
        widgetId: Int,
        minAlpha: Float = 0.3f,
        maxAlpha: Float = 1f,
        periodMs: Long = 1000,
        updateCallback: (Float) -> Unit
    ): String {
        val animId = "pulse:${widgetId}:${System.currentTimeMillis()}"
        val task = AnimationTask(animId, widgetId)
        activeAnimations[animId] = task
        
        val frames = (periodMs / 16).toInt()
        val halfFrames = frames / 2
        var frame = 0
        
        val runnable = object : Runnable {
            override fun run() {
                if (!task.isRunning) {
                    activeAnimations.remove(animId)
                    return
                }
                
                val progress = if (frame < halfFrames) {
                    frame.toFloat() / halfFrames
                } else {
                    1f - (frame - halfFrames).toFloat() / halfFrames
                }
                
                val alpha = minAlpha + (maxAlpha - minAlpha) * progress
                mainHandler.post { updateCallback(alpha) }
                
                frame = (frame + 1) % frames
                mainHandler.postDelayed(this, 16)
            }
        }
        
        mainHandler.post(runnable)
        return animId
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحكم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إيقاف الرسوم المتحركة / Stop animation
     */
    fun stopAnimation(animationId: String): Boolean {
        val task = activeAnimations[animationId]
        if (task != null) {
            task.isRunning = false
            activeAnimations.remove(animationId)
            animationListeners[animationId]?.onAnimationEnd(animationId)
            return true
        }
        return false
    }
    
    /**
     * إيقاف جميع رسوم الويدجت / Stop all widget animations
     */
    fun stopWidgetAnimations(widgetId: Int): Int {
        var count = 0
        activeAnimations.values.filter { it.widgetId == widgetId }.forEach {
            it.isRunning = false
            activeAnimations.remove(it.id)
            count++
        }
        return count
    }
    
    /**
     * إيقاف جميع الرسوم المتحركة / Stop all animations
     */
    fun stopAllAnimations() {
        activeAnimations.values.forEach { it.isRunning = false }
        activeAnimations.clear()
    }
    
    /**
     * هل الرسوم المتحركة نشطة؟ / Is animation active?
     */
    fun isAnimationActive(animationId: String): Boolean {
        return activeAnimations[animationId]?.isRunning ?: false
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        stopAllAnimations()
        animationListeners.clear()
        context = null
    }
}
