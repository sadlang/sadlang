package sad.android.tv.focus

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر رسوم التركيز التلفزيونية - Focus animations and effects
 * TV Focus Animation Bridge - Focus animations and effects
 * 
 * @author فريق لغة ص
 */
object SadTVFocusAnimationBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع الرسوم المتحركة / Animation type
     */
    enum class AnimationType {
        SCALE,
        SCALE_WITH_SHADOW,
        GLOW,
        BORDER,
        COLOR_SHIFT,
        RIPPLE,
        CUSTOM
    }
    
    /**
     * منحنى التسارع / Easing curve
     */
    enum class EasingCurve {
        LINEAR,
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        BOUNCE,
        ELASTIC,
        OVERSHOOT
    }
    
    /**
     * تكوين الرسوم المتحركة / Animation config
     */
    data class FocusAnimationConfig(
        val type: AnimationType = AnimationType.SCALE_WITH_SHADOW,
        val duration: Long = 200,
        val focusScale: Float = 1.1f,
        val unfocusScale: Float = 1.0f,
        val shadowRadius: Float = 12f,
        val shadowColor: Int = 0x40000000,
        val glowColor: Int = 0xFF1E88E5.toInt(),
        val glowRadius: Float = 16f,
        val borderWidth: Float = 3f,
        val borderColor: Int = 0xFF1E88E5.toInt(),
        val easingCurve: EasingCurve = EasingCurve.EASE_OUT,
        val enableParallax: Boolean = false,
        val parallaxFactor: Float = 0.1f
    )
    
    /**
     * حالة التركيز / Focus state
     */
    data class FocusState(
        val viewId: Int,
        val isFocused: Boolean,
        val scale: Float,
        val alpha: Float,
        val elevation: Float,
        val translationZ: Float
    )
    
    /**
     * مستمع الرسوم المتحركة / Animation listener
     */
    interface FocusAnimationListener {
        fun onAnimationStarted(viewId: Int, isFocusing: Boolean)
        fun onAnimationEnded(viewId: Int, isFocusing: Boolean)
        fun onAnimationCancelled(viewId: Int)
    }
    
    private var animationListener: FocusAnimationListener? = null
    private var config = FocusAnimationConfig()
    private val focusStates = mutableMapOf<Int, FocusState>()
    
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
     * تعيين المستمع / Set listener
     */
    fun setAnimationListener(listener: FocusAnimationListener?) {
        animationListener = listener
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: FocusAnimationConfig) {
        config = newConfig
    }
    
    /**
     * الحصول على التكوين / Get config
     */
    fun getConfig(): FocusAnimationConfig = config
    
    // ═══════════════════════════════════════════════════════════════
    //                         الرسوم المتحركة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تشغيل رسوم التركيز / Animate focus
     */
    fun animateFocus(viewId: Int) {
        val targetScale = config.focusScale
        val targetElevation = config.shadowRadius
        
        val newState = FocusState(
            viewId = viewId,
            isFocused = true,
            scale = targetScale,
            alpha = 1f,
            elevation = targetElevation,
            translationZ = targetElevation / 2
        )
        
        focusStates[viewId] = newState
        mainHandler.post { animationListener?.onAnimationStarted(viewId, true) }
        
        // محاكاة نهاية الرسوم المتحركة
        mainHandler.postDelayed({
            animationListener?.onAnimationEnded(viewId, true)
        }, config.duration)
    }
    
    /**
     * تشغيل رسوم إلغاء التركيز / Animate unfocus
     */
    fun animateUnfocus(viewId: Int) {
        val targetScale = config.unfocusScale
        
        val newState = FocusState(
            viewId = viewId,
            isFocused = false,
            scale = targetScale,
            alpha = 1f,
            elevation = 0f,
            translationZ = 0f
        )
        
        focusStates[viewId] = newState
        mainHandler.post { animationListener?.onAnimationStarted(viewId, false) }
        
        mainHandler.postDelayed({
            animationListener?.onAnimationEnded(viewId, false)
        }, config.duration)
    }
    
    /**
     * إلغاء الرسوم المتحركة / Cancel animation
     */
    fun cancelAnimation(viewId: Int) {
        mainHandler.post { animationListener?.onAnimationCancelled(viewId) }
    }
    
    /**
     * الحصول على حالة التركيز / Get focus state
     */
    fun getFocusState(viewId: Int): FocusState? = focusStates[viewId]
    
    // ═══════════════════════════════════════════════════════════════
    //                         قيم الرسوم المتحركة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * حساب قيمة التسارع / Calculate easing value
     */
    fun calculateEasing(progress: Float, curve: EasingCurve = config.easingCurve): Float {
        val t = progress.coerceIn(0f, 1f)
        
        return when (curve) {
            EasingCurve.LINEAR -> t
            EasingCurve.EASE_IN -> t * t
            EasingCurve.EASE_OUT -> t * (2 - t)
            EasingCurve.EASE_IN_OUT -> {
                if (t < 0.5f) 2 * t * t
                else -1 + (4 - 2 * t) * t
            }
            EasingCurve.BOUNCE -> {
                val n1 = 7.5625f
                val d1 = 2.75f
                var t2 = t
                when {
                    t2 < 1 / d1 -> n1 * t2 * t2
                    t2 < 2 / d1 -> {
                        t2 -= 1.5f / d1
                        n1 * t2 * t2 + 0.75f
                    }
                    t2 < 2.5 / d1 -> {
                        t2 -= 2.25f / d1
                        n1 * t2 * t2 + 0.9375f
                    }
                    else -> {
                        t2 -= 2.625f / d1
                        n1 * t2 * t2 + 0.984375f
                    }
                }
            }
            EasingCurve.ELASTIC -> {
                if (t == 0f || t == 1f) t
                else {
                    val p = 0.3f
                    kotlin.math.sin((t - p / 4) * (2 * kotlin.math.PI.toFloat()) / p) *
                            kotlin.math.pow(2f, -10 * t) + 1
                }
            }
            EasingCurve.OVERSHOOT -> {
                val c = 1.70158f
                (t - 1) * (t - 1) * ((c + 1) * (t - 1) + c) + 1
            }
        }
    }
    
    /**
     * حساب المقياس الحالي / Calculate current scale
     */
    fun calculateScale(progress: Float, isFocusing: Boolean): Float {
        val from = if (isFocusing) config.unfocusScale else config.focusScale
        val to = if (isFocusing) config.focusScale else config.unfocusScale
        val easedProgress = calculateEasing(progress)
        
        return from + (to - from) * easedProgress
    }
    
    /**
     * حساب الارتفاع الحالي / Calculate current elevation
     */
    fun calculateElevation(progress: Float, isFocusing: Boolean): Float {
        val from = if (isFocusing) 0f else config.shadowRadius
        val to = if (isFocusing) config.shadowRadius else 0f
        val easedProgress = calculateEasing(progress)
        
        return from + (to - from) * easedProgress
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تكوينات جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نمط Netflix / Netflix style
     */
    fun createNetflixConfig(): FocusAnimationConfig {
        return FocusAnimationConfig(
            type = AnimationType.SCALE_WITH_SHADOW,
            duration = 150,
            focusScale = 1.08f,
            shadowRadius = 16f,
            shadowColor = 0x60000000,
            easingCurve = EasingCurve.EASE_OUT
        )
    }
    
    /**
     * نمط YouTube / YouTube style
     */
    fun createYouTubeConfig(): FocusAnimationConfig {
        return FocusAnimationConfig(
            type = AnimationType.SCALE,
            duration = 100,
            focusScale = 1.05f,
            easingCurve = EasingCurve.LINEAR
        )
    }
    
    /**
     * نمط متوهج / Glow style
     */
    fun createGlowConfig(glowColor: Int = 0xFF1E88E5.toInt()): FocusAnimationConfig {
        return FocusAnimationConfig(
            type = AnimationType.GLOW,
            duration = 200,
            focusScale = 1.02f,
            glowColor = glowColor,
            glowRadius = 20f,
            easingCurve = EasingCurve.EASE_IN_OUT
        )
    }
    
    /**
     * نمط الحدود / Border style
     */
    fun createBorderConfig(borderColor: Int = 0xFF1E88E5.toInt()): FocusAnimationConfig {
        return FocusAnimationConfig(
            type = AnimationType.BORDER,
            duration = 150,
            focusScale = 1.0f,
            borderWidth = 4f,
            borderColor = borderColor,
            easingCurve = EasingCurve.EASE_OUT
        )
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        focusStates.clear()
        animationListener = null
        context = null
    }
}
