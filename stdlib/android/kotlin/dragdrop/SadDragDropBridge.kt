// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDragDropBridge.kt
// الوصف: جسر Kotlin للسحب والإفلات
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.ClipData
import android.content.ClipDescription
import android.content.Context
import android.graphics.Canvas
import android.graphics.Point
import android.os.Build
import android.view.DragEvent
import android.view.View

/**
 * جسر السحب والإفلات — SadDragDropBridge
 */
object SadDragDropBridge {
    
    private var context: Context? = null
    
    // Callbacks
    private var onDragStartedCallback: ((View, String) -> Unit)? = null
    private var onDragEndedCallback: ((View, Boolean) -> Unit)? = null
    private var onDragEnteredCallback: ((View) -> Unit)? = null
    private var onDragExitedCallback: ((View) -> Unit)? = null
    private var onDropCallback: ((View, String, Float, Float) -> Boolean)? = null
    private var onDragLocationCallback: ((View, Float, Float) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Start Drag
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun startDrag(
        view: View,
        data: String,
        label: String = "drag_data",
        flags: Int = 0
    ): Boolean {
        return try {
            val clipData = ClipData.newPlainText(label, data)
            val shadowBuilder = View.DragShadowBuilder(view)
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                view.startDragAndDrop(clipData, shadowBuilder, view, flags)
            } else {
                @Suppress("DEPRECATION")
                view.startDrag(clipData, shadowBuilder, view, flags)
            }
            
            onDragStartedCallback?.invoke(view, data)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun startDragWithCustomShadow(
        view: View,
        data: String,
        shadowWidth: Int,
        shadowHeight: Int,
        touchPointX: Int,
        touchPointY: Int,
        label: String = "drag_data"
    ): Boolean {
        return try {
            val clipData = ClipData.newPlainText(label, data)
            
            val shadowBuilder = object : View.DragShadowBuilder(view) {
                override fun onProvideShadowMetrics(outShadowSize: Point, outShadowTouchPoint: Point) {
                    outShadowSize.set(shadowWidth, shadowHeight)
                    outShadowTouchPoint.set(touchPointX, touchPointY)
                }
                
                override fun onDrawShadow(canvas: Canvas) {
                    view.draw(canvas)
                }
            }
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                view.startDragAndDrop(clipData, shadowBuilder, view, 0)
            } else {
                @Suppress("DEPRECATION")
                view.startDrag(clipData, shadowBuilder, view, 0)
            }
            
            onDragStartedCallback?.invoke(view, data)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun startDragNoShadow(view: View, data: String): Boolean {
        return try {
            val clipData = ClipData.newPlainText("drag_data", data)
            
            val shadowBuilder = object : View.DragShadowBuilder(view) {
                override fun onProvideShadowMetrics(outShadowSize: Point, outShadowTouchPoint: Point) {
                    outShadowSize.set(1, 1)
                    outShadowTouchPoint.set(0, 0)
                }
                
                override fun onDrawShadow(canvas: Canvas) {
                    // Empty - no shadow
                }
            }
            
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                view.startDragAndDrop(clipData, shadowBuilder, view, 0)
            } else {
                @Suppress("DEPRECATION")
                view.startDrag(clipData, shadowBuilder, view, 0)
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Drop Target
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun makeDropTarget(
        view: View,
        acceptedMimeTypes: List<String>? = null
    ) {
        view.setOnDragListener { targetView, event ->
            when (event.action) {
                DragEvent.ACTION_DRAG_STARTED -> {
                    // Check if we should accept this drag
                    if (acceptedMimeTypes != null) {
                        val description = event.clipDescription
                        val accepted = acceptedMimeTypes.any { mimeType ->
                            description?.hasMimeType(mimeType) == true
                        }
                        accepted
                    } else {
                        true
                    }
                }
                
                DragEvent.ACTION_DRAG_ENTERED -> {
                    onDragEnteredCallback?.invoke(targetView)
                    true
                }
                
                DragEvent.ACTION_DRAG_LOCATION -> {
                    onDragLocationCallback?.invoke(targetView, event.x, event.y)
                    true
                }
                
                DragEvent.ACTION_DRAG_EXITED -> {
                    onDragExitedCallback?.invoke(targetView)
                    true
                }
                
                DragEvent.ACTION_DROP -> {
                    val data = extractData(event)
                    onDropCallback?.invoke(targetView, data, event.x, event.y) ?: true
                }
                
                DragEvent.ACTION_DRAG_ENDED -> {
                    val draggedView = event.localState as? View
                    draggedView?.let {
                        onDragEndedCallback?.invoke(it, event.result)
                    }
                    true
                }
                
                else -> false
            }
        }
    }
    
    @JvmStatic
    fun makeDropTargetWithCallbacks(
        view: View,
        onEnter: (() -> Unit)? = null,
        onExit: (() -> Unit)? = null,
        onDrop: ((String, Float, Float) -> Boolean)? = null,
        onLocation: ((Float, Float) -> Unit)? = null
    ) {
        view.setOnDragListener { _, event ->
            when (event.action) {
                DragEvent.ACTION_DRAG_STARTED -> true
                
                DragEvent.ACTION_DRAG_ENTERED -> {
                    onEnter?.invoke()
                    true
                }
                
                DragEvent.ACTION_DRAG_LOCATION -> {
                    onLocation?.invoke(event.x, event.y)
                    true
                }
                
                DragEvent.ACTION_DRAG_EXITED -> {
                    onExit?.invoke()
                    true
                }
                
                DragEvent.ACTION_DROP -> {
                    val data = extractData(event)
                    onDrop?.invoke(data, event.x, event.y) ?: true
                }
                
                DragEvent.ACTION_DRAG_ENDED -> true
                
                else -> false
            }
        }
    }
    
    @JvmStatic
    fun removeDropTarget(view: View) {
        view.setOnDragListener(null)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Global Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnDragStartedListener(callback: (View, String) -> Unit) {
        onDragStartedCallback = callback
    }
    
    @JvmStatic
    fun setOnDragEndedListener(callback: (View, Boolean) -> Unit) {
        onDragEndedCallback = callback
    }
    
    @JvmStatic
    fun setOnDragEnteredListener(callback: (View) -> Unit) {
        onDragEnteredCallback = callback
    }
    
    @JvmStatic
    fun setOnDragExitedListener(callback: (View) -> Unit) {
        onDragExitedCallback = callback
    }
    
    @JvmStatic
    fun setOnDropListener(callback: (View, String, Float, Float) -> Boolean) {
        onDropCallback = callback
    }
    
    @JvmStatic
    fun setOnDragLocationListener(callback: (View, Float, Float) -> Unit) {
        onDragLocationCallback = callback
    }
    
    @JvmStatic
    fun removeAllListeners() {
        onDragStartedCallback = null
        onDragEndedCallback = null
        onDragEnteredCallback = null
        onDragExitedCallback = null
        onDropCallback = null
        onDragLocationCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Drag Data Types
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createTextClipData(label: String, text: String): ClipData {
        return ClipData.newPlainText(label, text)
    }
    
    @JvmStatic
    fun createUriClipData(label: String, uri: String): ClipData {
        return ClipData.newRawUri(label, android.net.Uri.parse(uri))
    }
    
    @JvmStatic
    fun createIntentClipData(label: String, action: String): ClipData {
        val intent = android.content.Intent(action)
        return ClipData.newIntent(label, intent)
    }
    
    @JvmStatic
    fun createHtmlClipData(label: String, text: String, html: String): ClipData {
        return ClipData.newHtmlText(label, text, html)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun extractData(event: DragEvent): String {
        return try {
            val item = event.clipData?.getItemAt(0)
            item?.text?.toString() ?: item?.uri?.toString() ?: ""
        } catch (e: Exception) {
            ""
        }
    }
    
    @JvmStatic
    fun getClipDataText(event: DragEvent): String? {
        return try {
            event.clipData?.getItemAt(0)?.text?.toString()
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getClipDataUri(event: DragEvent): String? {
        return try {
            event.clipData?.getItemAt(0)?.uri?.toString()
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun getClipDataMimeTypes(event: DragEvent): List<String> {
        val description = event.clipDescription ?: return emptyList()
        return (0 until description.mimeTypeCount).map { i ->
            description.getMimeType(i)
        }
    }
    
    @JvmStatic
    fun hasMimeType(event: DragEvent, mimeType: String): Boolean {
        return event.clipDescription?.hasMimeType(mimeType) == true
    }
    
    @JvmStatic
    fun getActionName(action: Int): String {
        return when (action) {
            DragEvent.ACTION_DRAG_STARTED -> "بدأ السحب"
            DragEvent.ACTION_DRAG_ENTERED -> "دخول منطقة الإفلات"
            DragEvent.ACTION_DRAG_LOCATION -> "موقع السحب"
            DragEvent.ACTION_DRAG_EXITED -> "خروج من منطقة الإفلات"
            DragEvent.ACTION_DROP -> "إفلات"
            DragEvent.ACTION_DRAG_ENDED -> "انتهى السحب"
            else -> "غير معروف"
        }
    }
    
    @JvmStatic
    fun getActionNameEnglish(action: Int): String {
        return when (action) {
            DragEvent.ACTION_DRAG_STARTED -> "DRAG_STARTED"
            DragEvent.ACTION_DRAG_ENTERED -> "DRAG_ENTERED"
            DragEvent.ACTION_DRAG_LOCATION -> "DRAG_LOCATION"
            DragEvent.ACTION_DRAG_EXITED -> "DRAG_EXITED"
            DragEvent.ACTION_DROP -> "DROP"
            DragEvent.ACTION_DRAG_ENDED -> "DRAG_ENDED"
            else -> "UNKNOWN"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // View Effects
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setDragHighlight(
        view: View,
        highlightOnEnter: Boolean = true,
        enterAlpha: Float = 0.7f,
        exitAlpha: Float = 1.0f
    ) {
        view.setOnDragListener { v, event ->
            when (event.action) {
                DragEvent.ACTION_DRAG_STARTED -> true
                
                DragEvent.ACTION_DRAG_ENTERED -> {
                    if (highlightOnEnter) {
                        v.alpha = enterAlpha
                    }
                    true
                }
                
                DragEvent.ACTION_DRAG_EXITED, DragEvent.ACTION_DRAG_ENDED -> {
                    v.alpha = exitAlpha
                    true
                }
                
                DragEvent.ACTION_DROP -> {
                    v.alpha = exitAlpha
                    val data = extractData(event)
                    onDropCallback?.invoke(v, data, event.x, event.y) ?: true
                }
                
                else -> true
            }
        }
    }
    
    @JvmStatic
    fun makeDraggableWithLongPress(view: View, data: String) {
        view.setOnLongClickListener { v ->
            startDrag(v, data)
        }
    }
}
