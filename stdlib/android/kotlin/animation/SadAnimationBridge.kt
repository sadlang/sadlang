// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAnimationBridge.kt
// الوصف: جسر Kotlin للرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.animation.*
import android.content.Context
import android.os.Build
import android.view.View
import android.view.animation.*

/**
 * جسر الرسوم المتحركة — SadAnimationBridge
 */
object SadAnimationBridge {
    
    private var context: Context? = null
    private val runningAnimations = mutableMapOf<String, Animator>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Property Animations - Alpha
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun fadeIn(
        view: View,
        duration: Long = 300,
        startDelay: Long = 0,
        onEnd: (() -> Unit)? = null
    ): String {
        val animator = ObjectAnimator.ofFloat(view, View.ALPHA, 0f, 1f).apply {
            this.duration = duration
            this.startDelay = startDelay
            interpolator = AccelerateDecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "fade_in_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        view.visibility = View.VISIBLE
        animator.start()
        return id
    }
    
    @JvmStatic
    fun fadeOut(
        view: View,
        duration: Long = 300,
        startDelay: Long = 0,
        hideOnComplete: Boolean = true,
        onEnd: (() -> Unit)? = null
    ): String {
        val animator = ObjectAnimator.ofFloat(view, View.ALPHA, 1f, 0f).apply {
            this.duration = duration
            this.startDelay = startDelay
            interpolator = AccelerateDecelerateInterpolator()
            addListener(object : AnimatorListenerAdapter() {
                override fun onAnimationEnd(animation: Animator) {
                    if (hideOnComplete) {
                        view.visibility = View.GONE
                    }
                    onEnd?.invoke()
                }
            })
        }
        
        val id = "fade_out_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun crossFade(
        viewOut: View,
        viewIn: View,
        duration: Long = 300
    ): String {
        viewIn.alpha = 0f
        viewIn.visibility = View.VISIBLE
        
        viewIn.animate()
            .alpha(1f)
            .setDuration(duration)
            .setListener(null)
        
        viewOut.animate()
            .alpha(0f)
            .setDuration(duration)
            .setListener(object : AnimatorListenerAdapter() {
                override fun onAnimationEnd(animation: Animator) {
                    viewOut.visibility = View.GONE
                }
            })
        
        return "cross_fade_${System.currentTimeMillis()}"
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Property Animations - Translation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun slideInFromLeft(
        view: View,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        view.translationX = -view.width.toFloat()
        view.visibility = View.VISIBLE
        
        val animator = ObjectAnimator.ofFloat(view, View.TRANSLATION_X, 0f).apply {
            this.duration = duration
            interpolator = DecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "slide_in_left_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun slideInFromRight(
        view: View,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        view.translationX = view.width.toFloat()
        view.visibility = View.VISIBLE
        
        val animator = ObjectAnimator.ofFloat(view, View.TRANSLATION_X, 0f).apply {
            this.duration = duration
            interpolator = DecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "slide_in_right_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun slideInFromTop(
        view: View,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        view.translationY = -view.height.toFloat()
        view.visibility = View.VISIBLE
        
        val animator = ObjectAnimator.ofFloat(view, View.TRANSLATION_Y, 0f).apply {
            this.duration = duration
            interpolator = DecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "slide_in_top_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun slideInFromBottom(
        view: View,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        view.translationY = view.height.toFloat()
        view.visibility = View.VISIBLE
        
        val animator = ObjectAnimator.ofFloat(view, View.TRANSLATION_Y, 0f).apply {
            this.duration = duration
            interpolator = DecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "slide_in_bottom_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun translate(
        view: View,
        toX: Float,
        toY: Float,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        val animatorX = ObjectAnimator.ofFloat(view, View.TRANSLATION_X, toX)
        val animatorY = ObjectAnimator.ofFloat(view, View.TRANSLATION_Y, toY)
        
        val animatorSet = AnimatorSet().apply {
            playTogether(animatorX, animatorY)
            this.duration = duration
            interpolator = AccelerateDecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "translate_${System.currentTimeMillis()}"
        runningAnimations[id] = animatorSet
        animatorSet.start()
        return id
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Property Animations - Scale
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun scaleIn(
        view: View,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        view.scaleX = 0f
        view.scaleY = 0f
        view.visibility = View.VISIBLE
        
        val scaleX = ObjectAnimator.ofFloat(view, View.SCALE_X, 1f)
        val scaleY = ObjectAnimator.ofFloat(view, View.SCALE_Y, 1f)
        
        val animatorSet = AnimatorSet().apply {
            playTogether(scaleX, scaleY)
            this.duration = duration
            interpolator = OvershootInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "scale_in_${System.currentTimeMillis()}"
        runningAnimations[id] = animatorSet
        animatorSet.start()
        return id
    }
    
    @JvmStatic
    fun scaleOut(
        view: View,
        duration: Long = 300,
        hideOnComplete: Boolean = true,
        onEnd: (() -> Unit)? = null
    ): String {
        val scaleX = ObjectAnimator.ofFloat(view, View.SCALE_X, 0f)
        val scaleY = ObjectAnimator.ofFloat(view, View.SCALE_Y, 0f)
        
        val animatorSet = AnimatorSet().apply {
            playTogether(scaleX, scaleY)
            this.duration = duration
            interpolator = AnticipateInterpolator()
            addListener(object : AnimatorListenerAdapter() {
                override fun onAnimationEnd(animation: Animator) {
                    if (hideOnComplete) {
                        view.visibility = View.GONE
                    }
                    onEnd?.invoke()
                }
            })
        }
        
        val id = "scale_out_${System.currentTimeMillis()}"
        runningAnimations[id] = animatorSet
        animatorSet.start()
        return id
    }
    
    @JvmStatic
    fun scale(
        view: View,
        toScaleX: Float,
        toScaleY: Float,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        val scaleX = ObjectAnimator.ofFloat(view, View.SCALE_X, toScaleX)
        val scaleY = ObjectAnimator.ofFloat(view, View.SCALE_Y, toScaleY)
        
        val animatorSet = AnimatorSet().apply {
            playTogether(scaleX, scaleY)
            this.duration = duration
            interpolator = AccelerateDecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "scale_${System.currentTimeMillis()}"
        runningAnimations[id] = animatorSet
        animatorSet.start()
        return id
    }
    
    @JvmStatic
    fun pulse(
        view: View,
        scaleFactor: Float = 1.1f,
        duration: Long = 200,
        repeatCount: Int = 1
    ): String {
        val scaleUpX = ObjectAnimator.ofFloat(view, View.SCALE_X, 1f, scaleFactor)
        val scaleUpY = ObjectAnimator.ofFloat(view, View.SCALE_Y, 1f, scaleFactor)
        val scaleDownX = ObjectAnimator.ofFloat(view, View.SCALE_X, scaleFactor, 1f)
        val scaleDownY = ObjectAnimator.ofFloat(view, View.SCALE_Y, scaleFactor, 1f)
        
        val scaleUp = AnimatorSet().apply { playTogether(scaleUpX, scaleUpY) }
        val scaleDown = AnimatorSet().apply { playTogether(scaleDownX, scaleDownY) }
        
        val animatorSet = AnimatorSet().apply {
            playSequentially(scaleUp, scaleDown)
            this.duration = duration
        }
        
        val wrapper = ObjectAnimator().apply {
            // Repeat the whole sequence
        }
        
        val id = "pulse_${System.currentTimeMillis()}"
        runningAnimations[id] = animatorSet
        
        // Manual repeat
        var currentRepeat = 0
        animatorSet.addListener(object : AnimatorListenerAdapter() {
            override fun onAnimationEnd(animation: Animator) {
                currentRepeat++
                if (currentRepeat < repeatCount) {
                    animatorSet.start()
                }
            }
        })
        
        animatorSet.start()
        return id
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Property Animations - Rotation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun rotate(
        view: View,
        toDegrees: Float,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        val animator = ObjectAnimator.ofFloat(view, View.ROTATION, toDegrees).apply {
            this.duration = duration
            interpolator = AccelerateDecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "rotate_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun rotateBy(
        view: View,
        byDegrees: Float,
        duration: Long = 300,
        onEnd: (() -> Unit)? = null
    ): String {
        val currentRotation = view.rotation
        return rotate(view, currentRotation + byDegrees, duration, onEnd)
    }
    
    @JvmStatic
    fun spin(
        view: View,
        duration: Long = 1000,
        repeatCount: Int = ValueAnimator.INFINITE,
        clockwise: Boolean = true
    ): String {
        val endDegrees = if (clockwise) 360f else -360f
        
        val animator = ObjectAnimator.ofFloat(view, View.ROTATION, 0f, endDegrees).apply {
            this.duration = duration
            this.repeatCount = repeatCount
            repeatMode = ValueAnimator.RESTART
            interpolator = LinearInterpolator()
        }
        
        val id = "spin_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Special Effects
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun shake(
        view: View,
        intensity: Float = 10f,
        duration: Long = 500
    ): String {
        val animator = ObjectAnimator.ofFloat(
            view, View.TRANSLATION_X,
            0f, intensity, -intensity, intensity, -intensity, intensity, -intensity, 0f
        ).apply {
            this.duration = duration
            interpolator = LinearInterpolator()
        }
        
        val id = "shake_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun bounce(
        view: View,
        height: Float = 50f,
        duration: Long = 500
    ): String {
        val animator = ObjectAnimator.ofFloat(
            view, View.TRANSLATION_Y,
            0f, -height, 0f, -height * 0.5f, 0f, -height * 0.25f, 0f
        ).apply {
            this.duration = duration
            interpolator = BounceInterpolator()
        }
        
        val id = "bounce_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    @JvmStatic
    fun flip(
        view: View,
        horizontal: Boolean = false,
        duration: Long = 500,
        onEnd: (() -> Unit)? = null
    ): String {
        val property = if (horizontal) View.ROTATION_Y else View.ROTATION_X
        
        val animator = ObjectAnimator.ofFloat(view, property, 0f, 180f).apply {
            this.duration = duration
            interpolator = AccelerateDecelerateInterpolator()
            addListener(createAnimatorListener(onEnd))
        }
        
        val id = "flip_${System.currentTimeMillis()}"
        runningAnimations[id] = animator
        animator.start()
        return id
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Control
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun cancelAnimation(animationId: String): Boolean {
        val animator = runningAnimations[animationId]
        if (animator != null) {
            animator.cancel()
            runningAnimations.remove(animationId)
            return true
        }
        return false
    }
    
    @JvmStatic
    fun cancelAllAnimations() {
        runningAnimations.values.forEach { it.cancel() }
        runningAnimations.clear()
    }
    
    @JvmStatic
    fun pauseAnimation(animationId: String): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            val animator = runningAnimations[animationId]
            if (animator != null) {
                animator.pause()
                return true
            }
        }
        return false
    }
    
    @JvmStatic
    fun resumeAnimation(animationId: String): Boolean {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            val animator = runningAnimations[animationId]
            if (animator != null) {
                animator.resume()
                return true
            }
        }
        return false
    }
    
    @JvmStatic
    fun isAnimationRunning(animationId: String): Boolean {
        return runningAnimations[animationId]?.isRunning == true
    }
    
    @JvmStatic
    fun getRunningAnimationsCount(): Int {
        return runningAnimations.size
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Interpolators
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getInterpolator(name: String): Interpolator {
        return when (name.lowercase()) {
            "linear" -> LinearInterpolator()
            "accelerate" -> AccelerateInterpolator()
            "decelerate" -> DecelerateInterpolator()
            "accelerate_decelerate" -> AccelerateDecelerateInterpolator()
            "anticipate" -> AnticipateInterpolator()
            "overshoot" -> OvershootInterpolator()
            "anticipate_overshoot" -> AnticipateOvershootInterpolator()
            "bounce" -> BounceInterpolator()
            "cycle" -> CycleInterpolator(1f)
            else -> AccelerateDecelerateInterpolator()
        }
    }
    
    @JvmStatic
    fun getAvailableInterpolators(): List<String> {
        return listOf(
            "linear",
            "accelerate",
            "decelerate",
            "accelerate_decelerate",
            "anticipate",
            "overshoot",
            "anticipate_overshoot",
            "bounce",
            "cycle"
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Helper
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun createAnimatorListener(onEnd: (() -> Unit)?): AnimatorListenerAdapter {
        return object : AnimatorListenerAdapter() {
            override fun onAnimationEnd(animation: Animator) {
                onEnd?.invoke()
            }
        }
    }
}
