// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadGestureBridge.kt
// الوصف: جسر Kotlin للإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.view.GestureDetector
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.View
import kotlin.math.abs

/**
 * جسر الإيماءات — SadGestureBridge
 */
object SadGestureBridge {
    
    private var context: Context? = null
    
    // Callbacks
    private var onTapCallback: ((Float, Float) -> Unit)? = null
    private var onDoubleTapCallback: ((Float, Float) -> Unit)? = null
    private var onLongPressCallback: ((Float, Float) -> Unit)? = null
    private var onSwipeCallback: ((String, Float, Float) -> Unit)? = null
    private var onFlingCallback: ((Float, Float, Float, Float) -> Unit)? = null
    private var onScrollCallback: ((Float, Float, Float, Float) -> Unit)? = null
    private var onScaleCallback: ((Float, Float, Float) -> Unit)? = null
    private var onRotateCallback: ((Float) -> Unit)? = null
    
    // Settings
    private var swipeThreshold = 100f
    private var swipeVelocityThreshold = 100f
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Settings
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setSwipeThreshold(threshold: Float) {
        swipeThreshold = threshold
    }
    
    @JvmStatic
    fun setSwipeVelocityThreshold(velocity: Float) {
        swipeVelocityThreshold = velocity
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Setup Gesture Detectors
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun attachGestureDetector(view: View): Boolean {
        val ctx = context ?: return false
        
        val gestureDetector = GestureDetector(ctx, object : GestureDetector.SimpleOnGestureListener() {
            
            override fun onSingleTapConfirmed(e: MotionEvent): Boolean {
                onTapCallback?.invoke(e.x, e.y)
                return true
            }
            
            override fun onDoubleTap(e: MotionEvent): Boolean {
                onDoubleTapCallback?.invoke(e.x, e.y)
                return true
            }
            
            override fun onLongPress(e: MotionEvent) {
                onLongPressCallback?.invoke(e.x, e.y)
            }
            
            override fun onFling(
                e1: MotionEvent?,
                e2: MotionEvent,
                velocityX: Float,
                velocityY: Float
            ): Boolean {
                if (e1 == null) return false
                
                onFlingCallback?.invoke(velocityX, velocityY, e2.x, e2.y)
                
                val diffX = e2.x - e1.x
                val diffY = e2.y - e1.y
                
                // Detect swipe direction
                if (abs(diffX) > abs(diffY)) {
                    // Horizontal swipe
                    if (abs(diffX) > swipeThreshold && abs(velocityX) > swipeVelocityThreshold) {
                        val direction = if (diffX > 0) "يمين" else "يسار"
                        onSwipeCallback?.invoke(direction, e2.x, e2.y)
                    }
                } else {
                    // Vertical swipe
                    if (abs(diffY) > swipeThreshold && abs(velocityY) > swipeVelocityThreshold) {
                        val direction = if (diffY > 0) "أسفل" else "أعلى"
                        onSwipeCallback?.invoke(direction, e2.x, e2.y)
                    }
                }
                
                return true
            }
            
            override fun onScroll(
                e1: MotionEvent?,
                e2: MotionEvent,
                distanceX: Float,
                distanceY: Float
            ): Boolean {
                onScrollCallback?.invoke(distanceX, distanceY, e2.x, e2.y)
                return true
            }
        })
        
        val scaleDetector = ScaleGestureDetector(ctx, object : ScaleGestureDetector.SimpleOnScaleGestureListener() {
            override fun onScale(detector: ScaleGestureDetector): Boolean {
                onScaleCallback?.invoke(detector.scaleFactor, detector.focusX, detector.focusY)
                return true
            }
        })
        
        view.setOnTouchListener { _, event ->
            gestureDetector.onTouchEvent(event)
            scaleDetector.onTouchEvent(event)
            true
        }
        
        return true
    }
    
    @JvmStatic
    fun createGestureDetector(): GestureDetector? {
        val ctx = context ?: return null
        
        return GestureDetector(ctx, object : GestureDetector.SimpleOnGestureListener() {
            
            override fun onSingleTapConfirmed(e: MotionEvent): Boolean {
                onTapCallback?.invoke(e.x, e.y)
                return true
            }
            
            override fun onDoubleTap(e: MotionEvent): Boolean {
                onDoubleTapCallback?.invoke(e.x, e.y)
                return true
            }
            
            override fun onLongPress(e: MotionEvent) {
                onLongPressCallback?.invoke(e.x, e.y)
            }
            
            override fun onFling(
                e1: MotionEvent?,
                e2: MotionEvent,
                velocityX: Float,
                velocityY: Float
            ): Boolean {
                if (e1 == null) return false
                
                onFlingCallback?.invoke(velocityX, velocityY, e2.x, e2.y)
                
                val diffX = e2.x - e1.x
                val diffY = e2.y - e1.y
                
                if (abs(diffX) > abs(diffY)) {
                    if (abs(diffX) > swipeThreshold && abs(velocityX) > swipeVelocityThreshold) {
                        val direction = if (diffX > 0) "يمين" else "يسار"
                        onSwipeCallback?.invoke(direction, e2.x, e2.y)
                    }
                } else {
                    if (abs(diffY) > swipeThreshold && abs(velocityY) > swipeVelocityThreshold) {
                        val direction = if (diffY > 0) "أسفل" else "أعلى"
                        onSwipeCallback?.invoke(direction, e2.x, e2.y)
                    }
                }
                
                return true
            }
        })
    }
    
    @JvmStatic
    fun createScaleGestureDetector(): ScaleGestureDetector? {
        val ctx = context ?: return null
        
        return ScaleGestureDetector(ctx, object : ScaleGestureDetector.SimpleOnScaleGestureListener() {
            override fun onScale(detector: ScaleGestureDetector): Boolean {
                onScaleCallback?.invoke(detector.scaleFactor, detector.focusX, detector.focusY)
                return true
            }
        })
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnTapListener(callback: (Float, Float) -> Unit) {
        onTapCallback = callback
    }
    
    @JvmStatic
    fun setOnDoubleTapListener(callback: (Float, Float) -> Unit) {
        onDoubleTapCallback = callback
    }
    
    @JvmStatic
    fun setOnLongPressListener(callback: (Float, Float) -> Unit) {
        onLongPressCallback = callback
    }
    
    @JvmStatic
    fun setOnSwipeListener(callback: (String, Float, Float) -> Unit) {
        onSwipeCallback = callback
    }
    
    @JvmStatic
    fun setOnFlingListener(callback: (Float, Float, Float, Float) -> Unit) {
        onFlingCallback = callback
    }
    
    @JvmStatic
    fun setOnScrollListener(callback: (Float, Float, Float, Float) -> Unit) {
        onScrollCallback = callback
    }
    
    @JvmStatic
    fun setOnScaleListener(callback: (Float, Float, Float) -> Unit) {
        onScaleCallback = callback
    }
    
    @JvmStatic
    fun setOnRotateListener(callback: (Float) -> Unit) {
        onRotateCallback = callback
    }
    
    @JvmStatic
    fun removeAllListeners() {
        onTapCallback = null
        onDoubleTapCallback = null
        onLongPressCallback = null
        onSwipeCallback = null
        onFlingCallback = null
        onScrollCallback = null
        onScaleCallback = null
        onRotateCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Swipe Detection Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun detectSwipeDirection(
        startX: Float,
        startY: Float,
        endX: Float,
        endY: Float
    ): String {
        val diffX = endX - startX
        val diffY = endY - startY
        
        return when {
            abs(diffX) > abs(diffY) -> {
                if (diffX > 0) "يمين" else "يسار"
            }
            else -> {
                if (diffY > 0) "أسفل" else "أعلى"
            }
        }
    }
    
    @JvmStatic
    fun detectSwipeDirectionEnglish(
        startX: Float,
        startY: Float,
        endX: Float,
        endY: Float
    ): String {
        val diffX = endX - startX
        val diffY = endY - startY
        
        return when {
            abs(diffX) > abs(diffY) -> {
                if (diffX > 0) "right" else "left"
            }
            else -> {
                if (diffY > 0) "down" else "up"
            }
        }
    }
    
    @JvmStatic
    fun getSwipeDirections(): Map<String, String> {
        return mapOf(
            "right" to "يمين",
            "left" to "يسار",
            "up" to "أعلى",
            "down" to "أسفل"
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Rotation Detection (Two-finger rotation)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private var previousAngle = 0f
    private var isRotating = false
    
    @JvmStatic
    fun attachRotationDetector(view: View) {
        view.setOnTouchListener { _, event ->
            if (event.pointerCount == 2) {
                val currentAngle = calculateAngle(
                    event.getX(0), event.getY(0),
                    event.getX(1), event.getY(1)
                )
                
                when (event.actionMasked) {
                    MotionEvent.ACTION_POINTER_DOWN -> {
                        previousAngle = currentAngle
                        isRotating = true
                    }
                    MotionEvent.ACTION_MOVE -> {
                        if (isRotating) {
                            val rotation = currentAngle - previousAngle
                            onRotateCallback?.invoke(rotation)
                            previousAngle = currentAngle
                        }
                    }
                    MotionEvent.ACTION_POINTER_UP -> {
                        isRotating = false
                    }
                }
            }
            false // Allow other touch events
        }
    }
    
    private fun calculateAngle(x1: Float, y1: Float, x2: Float, y2: Float): Float {
        return Math.toDegrees(
            kotlin.math.atan2((y2 - y1).toDouble(), (x2 - x1).toDouble())
        ).toFloat()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun calculateDistance(x1: Float, y1: Float, x2: Float, y2: Float): Float {
        val dx = x2 - x1
        val dy = y2 - y1
        return kotlin.math.sqrt(dx * dx + dy * dy)
    }
    
    @JvmStatic
    fun calculateVelocity(
        startTime: Long,
        endTime: Long,
        distance: Float
    ): Float {
        val duration = (endTime - startTime).toFloat() / 1000f // seconds
        return if (duration > 0) distance / duration else 0f
    }
}
