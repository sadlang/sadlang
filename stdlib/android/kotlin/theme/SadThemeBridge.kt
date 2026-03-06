// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadThemeBridge.kt
// الوصف: جسر Kotlin للسمات والأنماط البصرية
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import android.content.res.Configuration
import android.graphics.Color
import android.os.Build
import android.view.View
import android.view.Window
import android.view.WindowManager
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsControllerCompat

/**
 * جسر السمات والأنماط — SadThemeBridge
 */
object SadThemeBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // ألوان السمات المحددة مسبقاً
    // ═══════════════════════════════════════════════════════════════════════════════
    
    object ArabicColors {
        // الألوان الأساسية
        const val أبيض = "#FFFFFF"
        const val أسود = "#000000"
        const val أحمر = "#F44336"
        const val أخضر = "#4CAF50"
        const val أزرق = "#2196F3"
        const val أصفر = "#FFEB3B"
        const val برتقالي = "#FF9800"
        const val بنفسجي = "#9C27B0"
        const val وردي = "#E91E63"
        const val رمادي = "#9E9E9E"
        const val بني = "#795548"
        const val فيروزي = "#00BCD4"
        const val نيلي = "#3F51B5"
        const val ليموني = "#CDDC39"
        const val كحلي = "#1A237E"
        const val ذهبي = "#FFD700"
        const val فضي = "#C0C0C0"
        const val عنابي = "#800020"
        const val زيتوني = "#808000"
        const val سماوي = "#87CEEB"
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // سمات جاهزة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    data class Theme(
        val name: String,
        val nameArabic: String,
        val primaryColor: String,
        val primaryDarkColor: String,
        val accentColor: String,
        val backgroundColor: String,
        val surfaceColor: String,
        val textColor: String,
        val textSecondaryColor: String,
        val isDark: Boolean
    )
    
    val THEME_LIGHT = Theme(
        name = "light",
        nameArabic = "فاتح",
        primaryColor = "#6200EE",
        primaryDarkColor = "#3700B3",
        accentColor = "#03DAC5",
        backgroundColor = "#FFFFFF",
        surfaceColor = "#FFFFFF",
        textColor = "#000000",
        textSecondaryColor = "#666666",
        isDark = false
    )
    
    val THEME_DARK = Theme(
        name = "dark",
        nameArabic = "داكن",
        primaryColor = "#BB86FC",
        primaryDarkColor = "#6200EE",
        accentColor = "#03DAC5",
        backgroundColor = "#121212",
        surfaceColor = "#1E1E1E",
        textColor = "#FFFFFF",
        textSecondaryColor = "#B3B3B3",
        isDark = true
    )
    
    val THEME_BLUE = Theme(
        name = "blue",
        nameArabic = "أزرق",
        primaryColor = "#2196F3",
        primaryDarkColor = "#1976D2",
        accentColor = "#FF4081",
        backgroundColor = "#FAFAFA",
        surfaceColor = "#FFFFFF",
        textColor = "#212121",
        textSecondaryColor = "#757575",
        isDark = false
    )
    
    val THEME_GREEN = Theme(
        name = "green",
        nameArabic = "أخضر",
        primaryColor = "#4CAF50",
        primaryDarkColor = "#388E3C",
        accentColor = "#FF5722",
        backgroundColor = "#FAFAFA",
        surfaceColor = "#FFFFFF",
        textColor = "#212121",
        textSecondaryColor = "#757575",
        isDark = false
    )
    
    val THEME_ARABIAN = Theme(
        name = "arabian",
        nameArabic = "عربي تقليدي",
        primaryColor = "#1B5E20",
        primaryDarkColor = "#0D3B12",
        accentColor = "#FFD700",
        backgroundColor = "#FFF8E1",
        surfaceColor = "#FFFFFF",
        textColor = "#3E2723",
        textSecondaryColor = "#5D4037",
        isDark = false
    )
    
    val THEME_NIGHT_ARABIAN = Theme(
        name = "night_arabian",
        nameArabic = "ليلة عربية",
        primaryColor = "#1A237E",
        primaryDarkColor = "#0D1259",
        accentColor = "#FFD700",
        backgroundColor = "#0A0E2E",
        surfaceColor = "#151B4F",
        textColor = "#E8E8E8",
        textSecondaryColor = "#B0B0B0",
        isDark = true
    )
    
    private val themes = listOf(
        THEME_LIGHT, THEME_DARK, THEME_BLUE, THEME_GREEN, THEME_ARABIAN, THEME_NIGHT_ARABIAN
    )
    
    private var currentTheme: Theme = THEME_LIGHT
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إدارة السمات
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCurrentTheme(): Theme = currentTheme
    
    @JvmStatic
    fun setTheme(theme: Theme) {
        currentTheme = theme
    }
    
    @JvmStatic
    fun setThemeByName(name: String): Boolean {
        val theme = themes.find { it.name == name || it.nameArabic == name }
        return if (theme != null) {
            currentTheme = theme
            true
        } else {
            false
        }
    }
    
    @JvmStatic
    fun getAllThemes(): List<Theme> = themes
    
    @JvmStatic
    fun getThemeNames(): List<String> = themes.map { it.name }
    
    @JvmStatic
    fun getThemeNamesArabic(): List<String> = themes.map { it.nameArabic }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // الوضع الداكن
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isDarkMode(context: Context): Boolean {
        return when (context.resources.configuration.uiMode and Configuration.UI_MODE_NIGHT_MASK) {
            Configuration.UI_MODE_NIGHT_YES -> true
            Configuration.UI_MODE_NIGHT_NO -> false
            else -> false
        }
    }
    
    @JvmStatic
    fun getSystemTheme(context: Context): Theme {
        return if (isDarkMode(context)) THEME_DARK else THEME_LIGHT
    }
    
    @JvmStatic
    fun applySystemTheme(context: Context) {
        currentTheme = getSystemTheme(context)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تحويل الألوان
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun parseColor(colorString: String): Int {
        return try {
            Color.parseColor(colorString)
        } catch (e: Exception) {
            Color.BLACK
        }
    }
    
    @JvmStatic
    fun colorToHex(color: Int): String {
        return String.format("#%06X", 0xFFFFFF and color)
    }
    
    @JvmStatic
    fun colorToRgb(color: Int): IntArray {
        return intArrayOf(
            Color.red(color),
            Color.green(color),
            Color.blue(color)
        )
    }
    
    @JvmStatic
    fun rgbToColor(r: Int, g: Int, b: Int): Int {
        return Color.rgb(r, g, b)
    }
    
    @JvmStatic
    fun rgbaToColor(r: Int, g: Int, b: Int, a: Int): Int {
        return Color.argb(a, r, g, b)
    }
    
    @JvmStatic
    fun adjustBrightness(color: Int, factor: Float): Int {
        val a = Color.alpha(color)
        var r = (Color.red(color) * factor).toInt().coerceIn(0, 255)
        var g = (Color.green(color) * factor).toInt().coerceIn(0, 255)
        var b = (Color.blue(color) * factor).toInt().coerceIn(0, 255)
        return Color.argb(a, r, g, b)
    }
    
    @JvmStatic
    fun lighten(color: Int, amount: Float = 0.2f): Int {
        return adjustBrightness(color, 1 + amount)
    }
    
    @JvmStatic
    fun darken(color: Int, amount: Float = 0.2f): Int {
        return adjustBrightness(color, 1 - amount)
    }
    
    @JvmStatic
    fun setAlpha(color: Int, alpha: Int): Int {
        return Color.argb(alpha, Color.red(color), Color.green(color), Color.blue(color))
    }
    
    @JvmStatic
    fun blendColors(color1: Int, color2: Int, ratio: Float): Int {
        val inverseRatio = 1f - ratio
        val a = (Color.alpha(color1) * inverseRatio + Color.alpha(color2) * ratio).toInt()
        val r = (Color.red(color1) * inverseRatio + Color.red(color2) * ratio).toInt()
        val g = (Color.green(color1) * inverseRatio + Color.green(color2) * ratio).toInt()
        val b = (Color.blue(color1) * inverseRatio + Color.blue(color2) * ratio).toInt()
        return Color.argb(a, r, g, b)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // شريط الحالة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setStatusBarColor(activity: Activity, color: Int) {
        activity.window.statusBarColor = color
    }
    
    @JvmStatic
    fun setStatusBarColor(activity: Activity, colorString: String) {
        setStatusBarColor(activity, parseColor(colorString))
    }
    
    @JvmStatic
    fun setStatusBarLightIcons(activity: Activity, light: Boolean) {
        val insetsController = WindowCompat.getInsetsController(activity.window, activity.window.decorView)
        insetsController.isAppearanceLightStatusBars = !light
    }
    
    @JvmStatic
    fun hideStatusBar(activity: Activity) {
        activity.window.setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        )
    }
    
    @JvmStatic
    fun showStatusBar(activity: Activity) {
        activity.window.clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // شريط التنقل
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setNavigationBarColor(activity: Activity, color: Int) {
        activity.window.navigationBarColor = color
    }
    
    @JvmStatic
    fun setNavigationBarColor(activity: Activity, colorString: String) {
        setNavigationBarColor(activity, parseColor(colorString))
    }
    
    @JvmStatic
    fun setNavigationBarLightIcons(activity: Activity, light: Boolean) {
        val insetsController = WindowCompat.getInsetsController(activity.window, activity.window.decorView)
        insetsController.isAppearanceLightNavigationBars = !light
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // تطبيق السمة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun applyThemeToWindow(activity: Activity, theme: Theme = currentTheme) {
        setStatusBarColor(activity, parseColor(theme.primaryDarkColor))
        setStatusBarLightIcons(activity, theme.isDark)
        setNavigationBarColor(activity, parseColor(theme.backgroundColor))
        setNavigationBarLightIcons(activity, theme.isDark)
    }
    
    @JvmStatic
    fun applyThemeToView(view: View, theme: Theme = currentTheme) {
        view.setBackgroundColor(parseColor(theme.backgroundColor))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // إنشاء سمات مخصصة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createTheme(
        name: String,
        nameArabic: String,
        primaryColor: String,
        accentColor: String,
        isDark: Boolean
    ): Theme {
        val backgroundColor = if (isDark) "#121212" else "#FFFFFF"
        val textColor = if (isDark) "#FFFFFF" else "#000000"
        
        return Theme(
            name = name,
            nameArabic = nameArabic,
            primaryColor = primaryColor,
            primaryDarkColor = darkenHex(primaryColor, 0.2f),
            accentColor = accentColor,
            backgroundColor = backgroundColor,
            surfaceColor = if (isDark) "#1E1E1E" else "#FFFFFF",
            textColor = textColor,
            textSecondaryColor = if (isDark) "#B3B3B3" else "#666666",
            isDark = isDark
        )
    }
    
    private fun darkenHex(hex: String, amount: Float): String {
        val color = parseColor(hex)
        val darkened = darken(color, amount)
        return colorToHex(darkened)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // معلومات السمة
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getThemeInfo(): String {
        val theme = currentTheme
        return buildString {
            appendLine("═══════════════════════════════════════")
            appendLine("معلومات السمة الحالية")
            appendLine("═══════════════════════════════════════")
            appendLine("الاسم: ${theme.nameArabic} (${theme.name})")
            appendLine("اللون الأساسي: ${theme.primaryColor}")
            appendLine("اللون الثانوي: ${theme.accentColor}")
            appendLine("لون الخلفية: ${theme.backgroundColor}")
            appendLine("لون النص: ${theme.textColor}")
            appendLine("الوضع: ${if (theme.isDark) "داكن" else "فاتح"}")
            appendLine("═══════════════════════════════════════")
        }
    }
    
    @JvmStatic
    fun getColorPalette(): Map<String, String> {
        return mapOf(
            "أبيض" to ArabicColors.أبيض,
            "أسود" to ArabicColors.أسود,
            "أحمر" to ArabicColors.أحمر,
            "أخضر" to ArabicColors.أخضر,
            "أزرق" to ArabicColors.أزرق,
            "أصفر" to ArabicColors.أصفر,
            "برتقالي" to ArabicColors.برتقالي,
            "بنفسجي" to ArabicColors.بنفسجي,
            "وردي" to ArabicColors.وردي,
            "رمادي" to ArabicColors.رمادي,
            "ذهبي" to ArabicColors.ذهبي,
            "فضي" to ArabicColors.فضي,
            "كحلي" to ArabicColors.كحلي,
            "عنابي" to ArabicColors.عنابي
        )
    }
}
