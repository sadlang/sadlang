package sad.android.widgets.layout

import android.content.Context
import android.graphics.Color
import android.os.Handler
import android.os.Looper
import android.util.TypedValue
import android.widget.RemoteViews
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر تخطيط الويدجت - بناء واجهات الويدجت
 * Widget Layout Bridge - Building widget interfaces
 * 
 * @author فريق لغة ص
 */
object SadWidgetLayoutBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val layoutTemplates = ConcurrentHashMap<String, LayoutTemplate>()
    
    /**
     * قالب التخطيط / Layout template
     */
    data class LayoutTemplate(
        val id: String,
        val layoutResId: Int,
        val viewBindings: Map<String, Int>,  // اسم -> viewId
        val theme: WidgetTheme = WidgetTheme.LIGHT
    )
    
    /**
     * سمة الويدجت / Widget theme
     */
    enum class WidgetTheme {
        LIGHT,
        DARK,
        TRANSPARENT,
        CUSTOM
    }
    
    /**
     * خصائص النص / Text properties
     */
    data class TextStyle(
        val color: Int = Color.BLACK,
        val size: Float = 14f,
        val bold: Boolean = false,
        val italic: Boolean = false,
        val maxLines: Int = 0
    )
    
    /**
     * خصائص الصورة / Image properties
     */
    data class ImageStyle(
        val tintColor: Int? = null,
        val alpha: Float = 1f,
        val scaleType: ScaleType = ScaleType.FIT_CENTER
    )
    
    /**
     * نوع التحجيم / Scale type
     */
    enum class ScaleType {
        FIT_CENTER,
        CENTER_CROP,
        CENTER_INSIDE,
        FIT_XY
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         قوالب التخطيط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل قالب / Register template
     */
    fun registerTemplate(template: LayoutTemplate): Boolean {
        layoutTemplates[template.id] = template
        return true
    }
    
    /**
     * الحصول على قالب / Get template
     */
    fun getTemplate(templateId: String): LayoutTemplate? {
        return layoutTemplates[templateId]
    }
    
    /**
     * إنشاء RemoteViews من قالب / Create RemoteViews from template
     */
    fun createFromTemplate(templateId: String): RemoteViews? {
        val ctx = context ?: return null
        val template = layoutTemplates[templateId] ?: return null
        return RemoteViews(ctx.packageName, template.layoutResId)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بناء التخطيط
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * باني التخطيط / Layout builder
     */
    class LayoutBuilder(private val context: Context, private val views: RemoteViews) {
        
        // ═══════ النص ═══════
        
        /**
         * تعيين نص / Set text
         */
        fun setText(viewId: Int, text: String): LayoutBuilder {
            views.setTextViewText(viewId, text)
            return this
        }
        
        /**
         * تعيين نص مع نمط / Set styled text
         */
        fun setStyledText(viewId: Int, text: String, style: TextStyle): LayoutBuilder {
            views.setTextViewText(viewId, text)
            views.setTextColor(viewId, style.color)
            views.setTextViewTextSize(viewId, TypedValue.COMPLEX_UNIT_SP, style.size)
            return this
        }
        
        /**
         * تعيين لون النص / Set text color
         */
        fun setTextColor(viewId: Int, color: Int): LayoutBuilder {
            views.setTextColor(viewId, color)
            return this
        }
        
        /**
         * تعيين لون النص من hex / Set text color from hex
         */
        fun setTextColorHex(viewId: Int, hexColor: String): LayoutBuilder {
            views.setTextColor(viewId, Color.parseColor(hexColor))
            return this
        }
        
        /**
         * تعيين حجم النص / Set text size
         */
        fun setTextSize(viewId: Int, sizeSp: Float): LayoutBuilder {
            views.setTextViewTextSize(viewId, TypedValue.COMPLEX_UNIT_SP, sizeSp)
            return this
        }
        
        // ═══════ الصور ═══════
        
        /**
         * تعيين صورة من مورد / Set image from resource
         */
        fun setImageResource(viewId: Int, resId: Int): LayoutBuilder {
            views.setImageViewResource(viewId, resId)
            return this
        }
        
        /**
         * تعيين تلوين الصورة / Set image tint
         */
        fun setImageTint(viewId: Int, color: Int): LayoutBuilder {
            views.setInt(viewId, "setColorFilter", color)
            return this
        }
        
        /**
         * تعيين شفافية الصورة / Set image alpha
         */
        fun setImageAlpha(viewId: Int, alpha: Int): LayoutBuilder {
            views.setInt(viewId, "setImageAlpha", alpha)
            return this
        }
        
        // ═══════ الرؤية ═══════
        
        /**
         * تعيين رؤية / Set visibility
         */
        fun setVisibility(viewId: Int, visible: Boolean): LayoutBuilder {
            views.setViewVisibility(viewId, 
                if (visible) android.view.View.VISIBLE else android.view.View.GONE)
            return this
        }
        
        /**
         * إظهار / Show view
         */
        fun show(viewId: Int): LayoutBuilder {
            views.setViewVisibility(viewId, android.view.View.VISIBLE)
            return this
        }
        
        /**
         * إخفاء / Hide view
         */
        fun hide(viewId: Int): LayoutBuilder {
            views.setViewVisibility(viewId, android.view.View.GONE)
            return this
        }
        
        /**
         * إخفاء مع حفظ المساحة / Hide but keep space
         */
        fun invisible(viewId: Int): LayoutBuilder {
            views.setViewVisibility(viewId, android.view.View.INVISIBLE)
            return this
        }
        
        // ═══════ الخلفية ═══════
        
        /**
         * تعيين لون الخلفية / Set background color
         */
        fun setBackgroundColor(viewId: Int, color: Int): LayoutBuilder {
            views.setInt(viewId, "setBackgroundColor", color)
            return this
        }
        
        /**
         * تعيين خلفية من مورد / Set background resource
         */
        fun setBackgroundResource(viewId: Int, resId: Int): LayoutBuilder {
            views.setInt(viewId, "setBackgroundResource", resId)
            return this
        }
        
        // ═══════ شريط التقدم ═══════
        
        /**
         * تعيين شريط تقدم / Set progress bar
         */
        fun setProgress(viewId: Int, progress: Int, max: Int = 100): LayoutBuilder {
            views.setProgressBar(viewId, max, progress, false)
            return this
        }
        
        /**
         * تعيين تقدم غير محدد / Set indeterminate progress
         */
        fun setIndeterminateProgress(viewId: Int): LayoutBuilder {
            views.setProgressBar(viewId, 0, 0, true)
            return this
        }
        
        // ═══════ عناصر أخرى ═══════
        
        /**
         * تعيين تفعيل / Set enabled
         */
        fun setEnabled(viewId: Int, enabled: Boolean): LayoutBuilder {
            views.setBoolean(viewId, "setEnabled", enabled)
            return this
        }
        
        /**
         * تعيين حالة checkbox / Set checkbox state
         */
        fun setChecked(viewId: Int, checked: Boolean): LayoutBuilder {
            views.setBoolean(viewId, "setChecked", checked)
            return this
        }
        
        /**
         * الحصول على RemoteViews / Get RemoteViews
         */
        fun build(): RemoteViews = views
    }
    
    /**
     * إنشاء باني / Create builder
     */
    fun createBuilder(layoutResId: Int): LayoutBuilder? {
        val ctx = context ?: return null
        val views = RemoteViews(ctx.packageName, layoutResId)
        return LayoutBuilder(ctx, views)
    }
    
    /**
     * إنشاء باني من قالب / Create builder from template
     */
    fun createBuilderFromTemplate(templateId: String): LayoutBuilder? {
        val ctx = context ?: return null
        val template = layoutTemplates[templateId] ?: return null
        val views = RemoteViews(ctx.packageName, template.layoutResId)
        return LayoutBuilder(ctx, views)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الألوان المساعدة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * ألوان قياسية / Standard colors
     */
    object Colors {
        val WHITE = Color.WHITE
        val BLACK = Color.BLACK
        val RED = Color.RED
        val GREEN = Color.GREEN
        val BLUE = Color.BLUE
        val YELLOW = Color.YELLOW
        val CYAN = Color.CYAN
        val MAGENTA = Color.MAGENTA
        val GRAY = Color.GRAY
        val LIGHT_GRAY = Color.LTGRAY
        val DARK_GRAY = Color.DKGRAY
        val TRANSPARENT = Color.TRANSPARENT
        
        // Material colors
        val PRIMARY = Color.parseColor("#6200EE")
        val PRIMARY_DARK = Color.parseColor("#3700B3")
        val SECONDARY = Color.parseColor("#03DAC6")
        val ERROR = Color.parseColor("#B00020")
        val SURFACE = Color.parseColor("#FFFFFF")
        val BACKGROUND = Color.parseColor("#F5F5F5")
    }
    
    /**
     * تحليل لون / Parse color
     */
    fun parseColor(colorString: String): Int {
        return try {
            Color.parseColor(colorString)
        } catch (e: Exception) {
            Color.BLACK
        }
    }
    
    /**
     * إنشاء لون / Create color
     */
    fun createColor(red: Int, green: Int, blue: Int, alpha: Int = 255): Int {
        return Color.argb(alpha, red, green, blue)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        layoutTemplates.clear()
        context = null
    }
}
