/**
 * =============================================================================
 * @file SadTheme.kt
 * @brief (AR) سمة التصميم للغة ص
 * @brief (EN) Sad Language Design Theme
 * =============================================================================
 */

package sad.lang.android.ui

import android.app.Activity
import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.SideEffect
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalView
import androidx.core.view.WindowCompat

// ═══════════════════════════════════════════════════════════════════════════════
//  Colors
// ═══════════════════════════════════════════════════════════════════════════════

private val SadPrimary = Color(0xFF2196F3)
private val SadPrimaryDark = Color(0xFF1976D2)
private val SadSecondary = Color(0xFF03DAC6)
private val SadAccent = Color(0xFFFF5722)
private val SadBackground = Color(0xFFF5F5F5)
private val SadSurface = Color(0xFFFFFFFF)
private val SadError = Color(0xFFB00020)

private val SadDarkPrimary = Color(0xFF90CAF9)
private val SadDarkSecondary = Color(0xFF03DAC6)
private val SadDarkBackground = Color(0xFF121212)
private val SadDarkSurface = Color(0xFF1E1E1E)

// Arabic Green Theme
private val ArabicGreen = Color(0xFF009688)
private val ArabicGreenLight = Color(0xFF4DB6AC)
private val ArabicGreenDark = Color(0xFF00796B)

// Light Theme
private val LightColorScheme = lightColorScheme(
    primary = SadPrimary,
    onPrimary = Color.White,
    primaryContainer = SadPrimaryDark,
    onPrimaryContainer = Color.White,
    secondary = SadSecondary,
    onSecondary = Color.Black,
    tertiary = SadAccent,
    onTertiary = Color.White,
    background = SadBackground,
    onBackground = Color.Black,
    surface = SadSurface,
    onSurface = Color.Black,
    error = SadError,
    onError = Color.White
)

// Dark Theme
private val DarkColorScheme = darkColorScheme(
    primary = SadDarkPrimary,
    onPrimary = Color.Black,
    primaryContainer = SadPrimaryDark,
    onPrimaryContainer = Color.White,
    secondary = SadDarkSecondary,
    onSecondary = Color.Black,
    tertiary = SadAccent,
    onTertiary = Color.White,
    background = SadDarkBackground,
    onBackground = Color.White,
    surface = SadDarkSurface,
    onSurface = Color.White,
    error = SadError,
    onError = Color.White
)

// Arabic Theme (Green)
private val ArabicLightColorScheme = lightColorScheme(
    primary = ArabicGreen,
    onPrimary = Color.White,
    primaryContainer = ArabicGreenLight,
    onPrimaryContainer = Color.Black,
    secondary = ArabicGreenDark,
    onSecondary = Color.White,
    background = SadBackground,
    onBackground = Color.Black,
    surface = SadSurface,
    onSurface = Color.Black,
    error = SadError,
    onError = Color.White
)

private val ArabicDarkColorScheme = darkColorScheme(
    primary = ArabicGreenLight,
    onPrimary = Color.Black,
    primaryContainer = ArabicGreenDark,
    onPrimaryContainer = Color.White,
    secondary = ArabicGreen,
    onSecondary = Color.White,
    background = SadDarkBackground,
    onBackground = Color.White,
    surface = SadDarkSurface,
    onSurface = Color.White,
    error = SadError,
    onError = Color.White
)

// ═══════════════════════════════════════════════════════════════════════════════
//  Theme Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * أنواع السمات المتاحة
 */
enum class SadThemeType {
    DEFAULT,    // سمة افتراضية (أزرق)
    ARABIC,     // سمة عربية (أخضر)
    DYNAMIC     // سمة ديناميكية (Android 12+)
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Theme Composable
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * سمة لغة ص
 */
@Composable
fun SadTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    themeType: SadThemeType = SadThemeType.DEFAULT,
    dynamicColor: Boolean = true,
    content: @Composable () -> Unit
) {
    val colorScheme = when {
        // Dynamic colors on Android 12+
        dynamicColor && themeType == SadThemeType.DYNAMIC && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val context = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(context) else dynamicLightColorScheme(context)
        }
        
        // Arabic theme
        themeType == SadThemeType.ARABIC -> {
            if (darkTheme) ArabicDarkColorScheme else ArabicLightColorScheme
        }
        
        // Default theme
        else -> {
            if (darkTheme) DarkColorScheme else LightColorScheme
        }
    }
    
    // Update status bar color
    val view = LocalView.current
    if (!view.isInEditMode) {
        SideEffect {
            val window = (view.context as Activity).window
            window.statusBarColor = colorScheme.primary.toArgb()
            WindowCompat.getInsetsController(window, view).isAppearanceLightStatusBars = !darkTheme
        }
    }
    
    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography(),
        content = content
    )
}

/**
 * سمة لغة ص العربية
 */
@Composable
fun SadArabicTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    content: @Composable () -> Unit
) {
    SadTheme(
        darkTheme = darkTheme,
        themeType = SadThemeType.ARABIC,
        dynamicColor = false,
        content = content
    )
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Color Extensions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ألوان إضافية
 */
object SadColors {
    val Success = Color(0xFF4CAF50)
    val Warning = Color(0xFFFFC107)
    val Info = Color(0xFF2196F3)
    val Error = Color(0xFFF44336)
    
    val Gray50 = Color(0xFFFAFAFA)
    val Gray100 = Color(0xFFF5F5F5)
    val Gray200 = Color(0xFFEEEEEE)
    val Gray300 = Color(0xFFE0E0E0)
    val Gray400 = Color(0xFFBDBDBD)
    val Gray500 = Color(0xFF9E9E9E)
    val Gray600 = Color(0xFF757575)
    val Gray700 = Color(0xFF616161)
    val Gray800 = Color(0xFF424242)
    val Gray900 = Color(0xFF212121)
}
