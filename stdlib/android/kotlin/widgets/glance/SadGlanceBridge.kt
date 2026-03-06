package sad.android.widgets.glance

import android.content.Context
import android.os.Handler
import android.os.Looper
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.glance.*
import androidx.glance.appwidget.*
import androidx.glance.layout.*
import androidx.glance.text.*
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر Glance - ويدجت تفاعلية حديثة بـ Jetpack Compose
 * Glance Bridge - Modern interactive widgets with Jetpack Compose
 * 
 * @author فريق لغة ص
 */
object SadGlanceBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val widgetDefinitions = ConcurrentHashMap<String, WidgetDefinition>()
    
    /**
     * تعريف الويدجت / Widget definition
     */
    data class WidgetDefinition(
        val id: String,
        val title: String,
        val description: String = "",
        val minWidth: Int = 100,
        val minHeight: Int = 50,
        val targetCellWidth: Int = 2,
        val targetCellHeight: Int = 1
    )
    
    /**
     * عنصر واجهة / UI element
     */
    sealed class GlanceElement {
        data class Text(
            val content: String,
            val fontSize: Int = 14,
            val color: Long = 0xFF000000,
            val bold: Boolean = false,
            val maxLines: Int = Int.MAX_VALUE
        ) : GlanceElement()
        
        data class Image(
            val resourceId: Int,
            val contentDescription: String = "",
            val width: Int? = null,
            val height: Int? = null
        ) : GlanceElement()
        
        data class Button(
            val text: String,
            val actionKey: String,
            val backgroundColor: Long = 0xFF6200EE
        ) : GlanceElement()
        
        data class Row(
            val children: List<GlanceElement>,
            val horizontalAlignment: HAlign = HAlign.START,
            val verticalAlignment: VAlign = VAlign.CENTER,
            val spacing: Int = 8
        ) : GlanceElement()
        
        data class Column(
            val children: List<GlanceElement>,
            val horizontalAlignment: HAlign = HAlign.START,
            val verticalAlignment: VAlign = VAlign.TOP,
            val spacing: Int = 8
        ) : GlanceElement()
        
        data class Box(
            val children: List<GlanceElement>,
            val backgroundColor: Long? = null,
            val cornerRadius: Int = 0,
            val padding: Int = 0
        ) : GlanceElement()
        
        data class Spacer(
            val width: Int = 0,
            val height: Int = 0
        ) : GlanceElement()
        
        data class ProgressBar(
            val progress: Float,
            val color: Long = 0xFF6200EE
        ) : GlanceElement()
        
        data class Switch(
            val checked: Boolean,
            val actionKey: String
        ) : GlanceElement()
        
        data class CheckBox(
            val checked: Boolean,
            val text: String,
            val actionKey: String
        ) : GlanceElement()
    }
    
    /**
     * محاذاة أفقية / Horizontal alignment
     */
    enum class HAlign { START, CENTER, END }
    
    /**
     * محاذاة رأسية / Vertical alignment
     */
    enum class VAlign { TOP, CENTER, BOTTOM }
    
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
    //                         تعريف الويدجت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل تعريف / Register definition
     */
    fun registerWidget(definition: WidgetDefinition): Boolean {
        widgetDefinitions[definition.id] = definition
        return true
    }
    
    /**
     * الحصول على تعريف / Get definition
     */
    fun getWidgetDefinition(id: String): WidgetDefinition? {
        return widgetDefinitions[id]
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بناء العناصر
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * بناء الويدجت / Widget builder
     */
    class WidgetBuilder {
        private val elements = mutableListOf<GlanceElement>()
        
        fun text(
            content: String,
            fontSize: Int = 14,
            color: Long = 0xFF000000,
            bold: Boolean = false,
            maxLines: Int = Int.MAX_VALUE
        ): WidgetBuilder {
            elements.add(GlanceElement.Text(content, fontSize, color, bold, maxLines))
            return this
        }
        
        fun image(
            resourceId: Int,
            contentDescription: String = "",
            width: Int? = null,
            height: Int? = null
        ): WidgetBuilder {
            elements.add(GlanceElement.Image(resourceId, contentDescription, width, height))
            return this
        }
        
        fun button(
            text: String,
            actionKey: String,
            backgroundColor: Long = 0xFF6200EE
        ): WidgetBuilder {
            elements.add(GlanceElement.Button(text, actionKey, backgroundColor))
            return this
        }
        
        fun row(
            horizontalAlignment: HAlign = HAlign.START,
            verticalAlignment: VAlign = VAlign.CENTER,
            spacing: Int = 8,
            builder: WidgetBuilder.() -> Unit
        ): WidgetBuilder {
            val rowBuilder = WidgetBuilder()
            builder(rowBuilder)
            elements.add(GlanceElement.Row(rowBuilder.build(), horizontalAlignment, verticalAlignment, spacing))
            return this
        }
        
        fun column(
            horizontalAlignment: HAlign = HAlign.START,
            verticalAlignment: VAlign = VAlign.TOP,
            spacing: Int = 8,
            builder: WidgetBuilder.() -> Unit
        ): WidgetBuilder {
            val colBuilder = WidgetBuilder()
            builder(colBuilder)
            elements.add(GlanceElement.Column(colBuilder.build(), horizontalAlignment, verticalAlignment, spacing))
            return this
        }
        
        fun box(
            backgroundColor: Long? = null,
            cornerRadius: Int = 0,
            padding: Int = 0,
            builder: WidgetBuilder.() -> Unit
        ): WidgetBuilder {
            val boxBuilder = WidgetBuilder()
            builder(boxBuilder)
            elements.add(GlanceElement.Box(boxBuilder.build(), backgroundColor, cornerRadius, padding))
            return this
        }
        
        fun spacer(width: Int = 0, height: Int = 0): WidgetBuilder {
            elements.add(GlanceElement.Spacer(width, height))
            return this
        }
        
        fun progressBar(progress: Float, color: Long = 0xFF6200EE): WidgetBuilder {
            elements.add(GlanceElement.ProgressBar(progress, color))
            return this
        }
        
        fun switch(checked: Boolean, actionKey: String): WidgetBuilder {
            elements.add(GlanceElement.Switch(checked, actionKey))
            return this
        }
        
        fun checkBox(checked: Boolean, text: String, actionKey: String): WidgetBuilder {
            elements.add(GlanceElement.CheckBox(checked, text, actionKey))
            return this
        }
        
        fun build(): List<GlanceElement> = elements.toList()
    }
    
    /**
     * إنشاء ويدجت / Create widget
     */
    fun createWidget(builder: WidgetBuilder.() -> Unit): List<GlanceElement> {
        val widgetBuilder = WidgetBuilder()
        builder(widgetBuilder)
        return widgetBuilder.build()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أحجام الويدجت
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * أحجام قياسية / Standard sizes
     */
    object StandardSizes {
        val SMALL = Pair(2, 1)      // 2x1 خلايا
        val MEDIUM = Pair(2, 2)     // 2x2 خلايا
        val LARGE = Pair(4, 2)      // 4x2 خلايا
        val WIDE = Pair(4, 1)       // 4x1 خلايا
        val TALL = Pair(2, 3)       // 2x3 خلايا
        val FULL = Pair(4, 4)       // 4x4 خلايا
    }
    
    /**
     * حساب الحجم الفعلي / Calculate actual size
     */
    fun calculateWidgetSize(cellWidth: Int, cellHeight: Int): Pair<Int, Int> {
        // حجم تقريبي للخلية: 70dp عرض، 74dp ارتفاع
        val width = cellWidth * 70
        val height = cellHeight * 74
        return Pair(width, height)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        widgetDefinitions.clear()
        context = null
    }
}
