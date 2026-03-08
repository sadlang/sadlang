// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  ثيم التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.notes.ui.theme

import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext

// ─────────────────────────────────────────────────────────────────────
// الألوان
// ─────────────────────────────────────────────────────────────────────

// الألوان الفاتحة
private val LightPrimary = Color(0xFF1976D2)
private val LightOnPrimary = Color(0xFFFFFFFF)
private val LightPrimaryContainer = Color(0xFFBBDEFB)
private val LightOnPrimaryContainer = Color(0xFF0D47A1)

private val LightSecondary = Color(0xFF455A64)
private val LightOnSecondary = Color(0xFFFFFFFF)
private val LightSecondaryContainer = Color(0xFFCFD8DC)
private val LightOnSecondaryContainer = Color(0xFF263238)

private val LightBackground = Color(0xFFFAFAFA)
private val LightOnBackground = Color(0xFF212121)
private val LightSurface = Color(0xFFFFFFFF)
private val LightOnSurface = Color(0xFF212121)

// الألوان الداكنة
private val DarkPrimary = Color(0xFF64B5F6)
private val DarkOnPrimary = Color(0xFF0D47A1)
private val DarkPrimaryContainer = Color(0xFF1565C0)
private val DarkOnPrimaryContainer = Color(0xFFBBDEFB)

private val DarkSecondary = Color(0xFF90A4AE)
private val DarkOnSecondary = Color(0xFF263238)
private val DarkSecondaryContainer = Color(0xFF37474F)
private val DarkOnSecondaryContainer = Color(0xFFCFD8DC)

private val DarkBackground = Color(0xFF121212)
private val DarkOnBackground = Color(0xFFE0E0E0)
private val DarkSurface = Color(0xFF1E1E1E)
private val DarkOnSurface = Color(0xFFE0E0E0)

// ─────────────────────────────────────────────────────────────────────
// مخططات الألوان
// ─────────────────────────────────────────────────────────────────────

private val LightColorScheme = lightColorScheme(
    primary = LightPrimary,
    onPrimary = LightOnPrimary,
    primaryContainer = LightPrimaryContainer,
    onPrimaryContainer = LightOnPrimaryContainer,
    secondary = LightSecondary,
    onSecondary = LightOnSecondary,
    secondaryContainer = LightSecondaryContainer,
    onSecondaryContainer = LightOnSecondaryContainer,
    background = LightBackground,
    onBackground = LightOnBackground,
    surface = LightSurface,
    onSurface = LightOnSurface
)

private val DarkColorScheme = darkColorScheme(
    primary = DarkPrimary,
    onPrimary = DarkOnPrimary,
    primaryContainer = DarkPrimaryContainer,
    onPrimaryContainer = DarkOnPrimaryContainer,
    secondary = DarkSecondary,
    onSecondary = DarkOnSecondary,
    secondaryContainer = DarkSecondaryContainer,
    onSecondaryContainer = DarkOnSecondaryContainer,
    background = DarkBackground,
    onBackground = DarkOnBackground,
    surface = DarkSurface,
    onSurface = DarkOnSurface
)

// ─────────────────────────────────────────────────────────────────────
// الثيم
// ─────────────────────────────────────────────────────────────────────

@Composable
fun SadNotesTheme(
    darkTheme: Boolean = isSystemInDarkTheme(),
    dynamicColor: Boolean = true,
    content: @Composable () -> Unit
) {
    val colorScheme = when {
        // استخدام الألوان الديناميكية على Android 12+
        dynamicColor && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val context = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(context) else dynamicLightColorScheme(context)
        }
        darkTheme -> DarkColorScheme
        else -> LightColorScheme
    }
    
    MaterialTheme(
        colorScheme = colorScheme,
        typography = Typography(),
        content = content
    )
}
