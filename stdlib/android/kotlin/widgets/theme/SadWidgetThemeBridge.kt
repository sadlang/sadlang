package sad.android.widgets.theme

import android.content.Context
import android.content.res.Configuration
import android.graphics.Color
import android.os.Handler
import android.os.Looper
import java.util.concurrent.ConcurrentHashMap

/**
 * جسر سمة الويدجت - إدارة المظهر والألوان
 * Widget Theme Bridge - Appearance and color management
 * 
 * @author فريق لغة ص
 */
object SadWidgetThemeBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    private val themes = ConcurrentHashMap<String, WidgetTheme>()
    private val widgetThemeAssignments = ConcurrentHashMap<Int, String>()
    
    /**
     * سمة الويدجت / Widget theme
     */
    data class WidgetTheme(
        val id: String,
        val name: String,
        val colors: ThemeColors,
        val typography: ThemeTypography = ThemeTypography(),
        val dimensions: ThemeDimensions = ThemeDimensions(),
        val followSystem: Boolean = false
    )
    
    /**
     * ألوان السمة / Theme colors
     */
    data class ThemeColors(
        val primary: Int,
        val primaryVariant: Int,
        val secondary: Int,
        val secondaryVariant: Int,
        val background: Int,
        val surface: Int,
        val error: Int,
        val onPrimary: Int,
        val onSecondary: Int,
        val onBackground: Int,
        val onSurface: Int,
        val onError: Int
    )
    
    /**
     * طباعة السمة / Theme typography
     */
    data class ThemeTypography(
        val titleSize: Float = 18f,
        val bodySize: Float = 14f,
        val captionSize: Float = 12f,
        val titleWeight: Int = 700,
        val bodyWeight: Int = 400
    )
    
    /**
     * أبعاد السمة / Theme dimensions
     */
    data class ThemeDimensions(
        val cornerRadius: Int = 8,
        val padding: Int = 16,
        val margin: Int = 8,
        val iconSize: Int = 24,
        val elevation: Int = 4
    )
    
    /**
     * مستمع تغيير السمة / Theme change listener
     */
    interface ThemeChangeListener {
        fun onThemeChanged(widgetId: Int, theme: WidgetTheme)
        fun onSystemThemeChanged(isNightMode: Boolean)
    }
    
    private val themeListeners = mutableListOf<ThemeChangeListener>()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        registerDefaultThemes()
    }
    
    /**
     * إضافة مستمع / Add listener
     */
    fun addThemeChangeListener(listener: ThemeChangeListener) {
        if (!themeListeners.contains(listener)) {
            themeListeners.add(listener)
        }
    }
    
    private fun registerDefaultThemes() {
        // السمة الفاتحة
        registerTheme(
            WidgetTheme(
                id = "light",
                name = "فاتح",
                colors = ThemeColors(
                    primary = Color.parseColor("#6200EE"),
                    primaryVariant = Color.parseColor("#3700B3"),
                    secondary = Color.parseColor("#03DAC6"),
                    secondaryVariant = Color.parseColor("#018786"),
                    background = Color.parseColor("#FFFFFF"),
                    surface = Color.parseColor("#FFFFFF"),
                    error = Color.parseColor("#B00020"),
                    onPrimary = Color.WHITE,
                    onSecondary = Color.BLACK,
                    onBackground = Color.BLACK,
                    onSurface = Color.BLACK,
                    onError = Color.WHITE
                )
            )
        )
        
        // السمة الداكنة
        registerTheme(
            WidgetTheme(
                id = "dark",
                name = "داكن",
                colors = ThemeColors(
                    primary = Color.parseColor("#BB86FC"),
                    primaryVariant = Color.parseColor("#3700B3"),
                    secondary = Color.parseColor("#03DAC6"),
                    secondaryVariant = Color.parseColor("#03DAC6"),
                    background = Color.parseColor("#121212"),
                    surface = Color.parseColor("#1E1E1E"),
                    error = Color.parseColor("#CF6679"),
                    onPrimary = Color.BLACK,
                    onSecondary = Color.BLACK,
                    onBackground = Color.WHITE,
                    onSurface = Color.WHITE,
                    onError = Color.BLACK
                )
            )
        )
        
        // سمة النظام
        registerTheme(
            WidgetTheme(
                id = "system",
                name = "متبع النظام",
                colors = ThemeColors(
                    primary = 0,
                    primaryVariant = 0,
                    secondary = 0,
                    secondaryVariant = 0,
                    background = 0,
                    surface = 0,
                    error = 0,
                    onPrimary = 0,
                    onSecondary = 0,
                    onBackground = 0,
                    onSurface = 0,
                    onError = 0
                ),
                followSystem = true
            )
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة السمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تسجيل سمة / Register theme
     */
    fun registerTheme(theme: WidgetTheme): Boolean {
        themes[theme.id] = theme
        return true
    }
    
    /**
     * الحصول على سمة / Get theme
     */
    fun getTheme(themeId: String): WidgetTheme? {
        return themes[themeId]
    }
    
    /**
     * جميع السمات / All themes
     */
    fun getAllThemes(): List<WidgetTheme> {
        return themes.values.toList()
    }
    
    /**
     * تعيين سمة للويدجت / Set theme for widget
     */
    fun setWidgetTheme(widgetId: Int, themeId: String): Boolean {
        val theme = themes[themeId] ?: return false
        widgetThemeAssignments[widgetId] = themeId
        
        mainHandler.post {
            themeListeners.forEach { it.onThemeChanged(widgetId, theme) }
        }
        
        return true
    }
    
    /**
     * الحصول على سمة الويدجت / Get widget theme
     */
    fun getWidgetTheme(widgetId: Int): WidgetTheme {
        val themeId = widgetThemeAssignments[widgetId] ?: "light"
        var theme = themes[themeId] ?: themes["light"]!!
        
        if (theme.followSystem) {
            theme = if (isNightMode()) themes["dark"]!! else themes["light"]!!
        }
        
        return theme
    }
    
    /**
     * هل الوضع الليلي؟ / Is night mode?
     */
    fun isNightMode(): Boolean {
        val ctx = context ?: return false
        val nightModeFlags = ctx.resources.configuration.uiMode and Configuration.UI_MODE_NIGHT_MASK
        return nightModeFlags == Configuration.UI_MODE_NIGHT_YES
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         بناء السمات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * باني السمة / Theme builder
     */
    class ThemeBuilder(private val id: String, private val name: String) {
        private var primary = Color.parseColor("#6200EE")
        private var primaryVariant = Color.parseColor("#3700B3")
        private var secondary = Color.parseColor("#03DAC6")
        private var secondaryVariant = Color.parseColor("#018786")
        private var background = Color.WHITE
        private var surface = Color.WHITE
        private var error = Color.parseColor("#B00020")
        private var onPrimary = Color.WHITE
        private var onSecondary = Color.BLACK
        private var onBackground = Color.BLACK
        private var onSurface = Color.BLACK
        private var onError = Color.WHITE
        private var typography = ThemeTypography()
        private var dimensions = ThemeDimensions()
        
        fun primary(color: Int): ThemeBuilder { primary = color; return this }
        fun primary(hex: String): ThemeBuilder { primary = Color.parseColor(hex); return this }
        
        fun secondary(color: Int): ThemeBuilder { secondary = color; return this }
        fun secondary(hex: String): ThemeBuilder { secondary = Color.parseColor(hex); return this }
        
        fun background(color: Int): ThemeBuilder { background = color; return this }
        fun background(hex: String): ThemeBuilder { background = Color.parseColor(hex); return this }
        
        fun surface(color: Int): ThemeBuilder { surface = color; return this }
        fun surface(hex: String): ThemeBuilder { surface = Color.parseColor(hex); return this }
        
        fun error(color: Int): ThemeBuilder { error = color; return this }
        fun error(hex: String): ThemeBuilder { error = Color.parseColor(hex); return this }
        
        fun onPrimary(color: Int): ThemeBuilder { onPrimary = color; return this }
        fun onBackground(color: Int): ThemeBuilder { onBackground = color; return this }
        fun onSurface(color: Int): ThemeBuilder { onSurface = color; return this }
        
        fun typography(typo: ThemeTypography): ThemeBuilder { typography = typo; return this }
        fun dimensions(dims: ThemeDimensions): ThemeBuilder { dimensions = dims; return this }
        
        fun build(): WidgetTheme {
            return WidgetTheme(
                id = id,
                name = name,
                colors = ThemeColors(
                    primary, primaryVariant, secondary, secondaryVariant,
                    background, surface, error,
                    onPrimary, onSecondary, onBackground, onSurface, onError
                ),
                typography = typography,
                dimensions = dimensions
            )
        }
    }
    
    /**
     * إنشاء سمة / Create theme
     */
    fun createTheme(id: String, name: String): ThemeBuilder {
        return ThemeBuilder(id, name)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         ألوان مساعدة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تفتيح لون / Lighten color
     */
    fun lightenColor(color: Int, factor: Float): Int {
        val r = ((Color.red(color) * (1 - factor) + 255 * factor)).toInt()
        val g = ((Color.green(color) * (1 - factor) + 255 * factor)).toInt()
        val b = ((Color.blue(color) * (1 - factor) + 255 * factor)).toInt()
        return Color.argb(Color.alpha(color), r.coerceIn(0, 255), g.coerceIn(0, 255), b.coerceIn(0, 255))
    }
    
    /**
     * تعتيم لون / Darken color
     */
    fun darkenColor(color: Int, factor: Float): Int {
        val r = (Color.red(color) * (1 - factor)).toInt()
        val g = (Color.green(color) * (1 - factor)).toInt()
        val b = (Color.blue(color) * (1 - factor)).toInt()
        return Color.argb(Color.alpha(color), r.coerceIn(0, 255), g.coerceIn(0, 255), b.coerceIn(0, 255))
    }
    
    /**
     * تعيين شفافية / Set alpha
     */
    fun setAlpha(color: Int, alpha: Int): Int {
        return Color.argb(alpha.coerceIn(0, 255), Color.red(color), Color.green(color), Color.blue(color))
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        themes.clear()
        widgetThemeAssignments.clear()
        themeListeners.clear()
        context = null
    }
}
