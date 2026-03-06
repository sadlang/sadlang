// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadComposeBridge.kt
// الوصف: جسر Kotlin لـ Jetpack Compose
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.*
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.ComposeView
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

/**
 * جسر Jetpack Compose — SadComposeBridge
 */
object SadComposeBridge {
    
    private var context: Context? = null
    
    // State holders
    private val stringStates = mutableMapOf<String, MutableState<String>>()
    private val intStates = mutableMapOf<String, MutableState<Int>>()
    private val booleanStates = mutableMapOf<String, MutableState<Boolean>>()
    private val listStates = mutableMapOf<String, MutableState<List<Any>>>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Activity Setup
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setComposeContent(activity: ComponentActivity, content: @Composable () -> Unit) {
        activity.setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    content()
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // State Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Composable
    fun rememberString(key: String, initial: String = ""): MutableState<String> {
        val state = remember { mutableStateOf(initial) }
        stringStates[key] = state
        return state
    }
    
    @JvmStatic
    @Composable
    fun rememberInt(key: String, initial: Int = 0): MutableState<Int> {
        val state = remember { mutableStateOf(initial) }
        intStates[key] = state
        return state
    }
    
    @JvmStatic
    @Composable
    fun rememberBoolean(key: String, initial: Boolean = false): MutableState<Boolean> {
        val state = remember { mutableStateOf(initial) }
        booleanStates[key] = state
        return state
    }
    
    @JvmStatic
    fun setStringState(key: String, value: String) {
        stringStates[key]?.value = value
    }
    
    @JvmStatic
    fun getStringState(key: String): String? {
        return stringStates[key]?.value
    }
    
    @JvmStatic
    fun setIntState(key: String, value: Int) {
        intStates[key]?.value = value
    }
    
    @JvmStatic
    fun getIntState(key: String): Int? {
        return intStates[key]?.value
    }
    
    @JvmStatic
    fun setBooleanState(key: String, value: Boolean) {
        booleanStates[key]?.value = value
    }
    
    @JvmStatic
    fun getBooleanState(key: String): Boolean? {
        return booleanStates[key]?.value
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Basic Composables
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Composable
    fun Text(
        text: String,
        fontSize: Int = 16,
        fontWeight: String = "normal",
        color: Long = 0xFF000000,
        textAlign: String = "start",
        modifier: Modifier = Modifier
    ) {
        val weight = when (fontWeight.lowercase()) {
            "bold" -> FontWeight.Bold
            "light" -> FontWeight.Light
            "medium" -> FontWeight.Medium
            "semibold" -> FontWeight.SemiBold
            "thin" -> FontWeight.Thin
            else -> FontWeight.Normal
        }
        
        val align = when (textAlign.lowercase()) {
            "center", "وسط" -> TextAlign.Center
            "end", "نهاية" -> TextAlign.End
            "justify" -> TextAlign.Justify
            else -> TextAlign.Start
        }
        
        Text(
            text = text,
            fontSize = fontSize.sp,
            fontWeight = weight,
            color = Color(color),
            textAlign = align,
            modifier = modifier
        )
    }
    
    @JvmStatic
    @Composable
    fun Button(
        text: String,
        onClick: () -> Unit,
        enabled: Boolean = true,
        modifier: Modifier = Modifier
    ) {
        Button(
            onClick = onClick,
            enabled = enabled,
            modifier = modifier
        ) {
            Text(text)
        }
    }
    
    @JvmStatic
    @Composable
    fun OutlinedButton(
        text: String,
        onClick: () -> Unit,
        enabled: Boolean = true,
        modifier: Modifier = Modifier
    ) {
        OutlinedButton(
            onClick = onClick,
            enabled = enabled,
            modifier = modifier
        ) {
            Text(text)
        }
    }
    
    @JvmStatic
    @Composable
    fun TextButton(
        text: String,
        onClick: () -> Unit,
        enabled: Boolean = true,
        modifier: Modifier = Modifier
    ) {
        TextButton(
            onClick = onClick,
            enabled = enabled,
            modifier = modifier
        ) {
            Text(text)
        }
    }
    
    @JvmStatic
    @Composable
    fun TextField(
        value: String,
        onValueChange: (String) -> Unit,
        label: String = "",
        placeholder: String = "",
        enabled: Boolean = true,
        singleLine: Boolean = true,
        modifier: Modifier = Modifier
    ) {
        OutlinedTextField(
            value = value,
            onValueChange = onValueChange,
            label = if (label.isNotEmpty()) { { Text(label) } } else null,
            placeholder = if (placeholder.isNotEmpty()) { { Text(placeholder) } } else null,
            enabled = enabled,
            singleLine = singleLine,
            modifier = modifier.fillMaxWidth()
        )
    }
    
    @JvmStatic
    @Composable
    fun Checkbox(
        checked: Boolean,
        onCheckedChange: (Boolean) -> Unit,
        label: String = "",
        enabled: Boolean = true
    ) {
        Row(
            verticalAlignment = Alignment.CenterVertically
        ) {
            Checkbox(
                checked = checked,
                onCheckedChange = onCheckedChange,
                enabled = enabled
            )
            if (label.isNotEmpty()) {
                Spacer(modifier = Modifier.width(8.dp))
                Text(text = label)
            }
        }
    }
    
    @JvmStatic
    @Composable
    fun Switch(
        checked: Boolean,
        onCheckedChange: (Boolean) -> Unit,
        label: String = "",
        enabled: Boolean = true
    ) {
        Row(
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween,
            modifier = Modifier.fillMaxWidth()
        ) {
            if (label.isNotEmpty()) {
                Text(text = label)
            }
            Switch(
                checked = checked,
                onCheckedChange = onCheckedChange,
                enabled = enabled
            )
        }
    }
    
    @JvmStatic
    @Composable
    fun Slider(
        value: Float,
        onValueChange: (Float) -> Unit,
        valueRange: ClosedFloatingPointRange<Float> = 0f..1f,
        steps: Int = 0,
        enabled: Boolean = true,
        modifier: Modifier = Modifier
    ) {
        Slider(
            value = value,
            onValueChange = onValueChange,
            valueRange = valueRange,
            steps = steps,
            enabled = enabled,
            modifier = modifier.fillMaxWidth()
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Layout Composables
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Composable
    fun Column(
        horizontalAlignment: String = "start",
        verticalArrangement: String = "top",
        modifier: Modifier = Modifier,
        content: @Composable ColumnScope.() -> Unit
    ) {
        val hAlign = when (horizontalAlignment.lowercase()) {
            "center", "وسط" -> Alignment.CenterHorizontally
            "end", "نهاية" -> Alignment.End
            else -> Alignment.Start
        }
        
        val vArrange = when (verticalArrangement.lowercase()) {
            "center", "وسط" -> Arrangement.Center
            "bottom", "أسفل" -> Arrangement.Bottom
            "spacebetween" -> Arrangement.SpaceBetween
            "spacearound" -> Arrangement.SpaceAround
            "spaceevenly" -> Arrangement.SpaceEvenly
            else -> Arrangement.Top
        }
        
        Column(
            horizontalAlignment = hAlign,
            verticalArrangement = vArrange,
            modifier = modifier,
            content = content
        )
    }
    
    @JvmStatic
    @Composable
    fun Row(
        verticalAlignment: String = "center",
        horizontalArrangement: String = "start",
        modifier: Modifier = Modifier,
        content: @Composable RowScope.() -> Unit
    ) {
        val vAlign = when (verticalAlignment.lowercase()) {
            "top", "أعلى" -> Alignment.Top
            "bottom", "أسفل" -> Alignment.Bottom
            else -> Alignment.CenterVertically
        }
        
        val hArrange = when (horizontalArrangement.lowercase()) {
            "center", "وسط" -> Arrangement.Center
            "end", "نهاية" -> Arrangement.End
            "spacebetween" -> Arrangement.SpaceBetween
            "spacearound" -> Arrangement.SpaceAround
            "spaceevenly" -> Arrangement.SpaceEvenly
            else -> Arrangement.Start
        }
        
        Row(
            verticalAlignment = vAlign,
            horizontalArrangement = hArrange,
            modifier = modifier,
            content = content
        )
    }
    
    @JvmStatic
    @Composable
    fun Box(
        contentAlignment: String = "topleft",
        modifier: Modifier = Modifier,
        content: @Composable BoxScope.() -> Unit
    ) {
        val align = when (contentAlignment.lowercase()) {
            "center", "وسط" -> Alignment.Center
            "topend", "أعلى_يسار" -> Alignment.TopEnd
            "topcenter" -> Alignment.TopCenter
            "bottomstart", "أسفل_يمين" -> Alignment.BottomStart
            "bottomend", "أسفل_يسار" -> Alignment.BottomEnd
            "bottomcenter" -> Alignment.BottomCenter
            "centerstart" -> Alignment.CenterStart
            "centerend" -> Alignment.CenterEnd
            else -> Alignment.TopStart
        }
        
        Box(
            contentAlignment = align,
            modifier = modifier,
            content = content
        )
    }
    
    @JvmStatic
    @Composable
    fun Spacer(height: Int = 0, width: Int = 0) {
        Spacer(
            modifier = Modifier
                .height(height.dp)
                .width(width.dp)
        )
    }
    
    @JvmStatic
    @Composable
    fun Divider(
        thickness: Int = 1,
        color: Long = 0xFFE0E0E0
    ) {
        HorizontalDivider(
            thickness = thickness.dp,
            color = Color(color)
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Card
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Composable
    fun Card(
        onClick: (() -> Unit)? = null,
        elevation: Int = 4,
        cornerRadius: Int = 8,
        modifier: Modifier = Modifier,
        content: @Composable ColumnScope.() -> Unit
    ) {
        if (onClick != null) {
            Card(
                onClick = onClick,
                elevation = CardDefaults.cardElevation(defaultElevation = elevation.dp),
                shape = RoundedCornerShape(cornerRadius.dp),
                modifier = modifier,
                content = content
            )
        } else {
            Card(
                elevation = CardDefaults.cardElevation(defaultElevation = elevation.dp),
                shape = RoundedCornerShape(cornerRadius.dp),
                modifier = modifier,
                content = content
            )
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Lists
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Composable
    fun <T> LazyColumn(
        items: List<T>,
        modifier: Modifier = Modifier,
        itemContent: @Composable (T) -> Unit
    ) {
        LazyColumn(modifier = modifier) {
            items(items) { item ->
                itemContent(item)
            }
        }
    }
    
    @JvmStatic
    @Composable
    fun <T> LazyRow(
        items: List<T>,
        modifier: Modifier = Modifier,
        itemContent: @Composable (T) -> Unit
    ) {
        LazyRow(modifier = modifier) {
            items(items) { item ->
                itemContent(item)
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Modifier Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createModifier(): Modifier = Modifier
    
    @JvmStatic
    fun Modifier.fullWidth(): Modifier = this.fillMaxWidth()
    
    @JvmStatic
    fun Modifier.fullHeight(): Modifier = this.fillMaxHeight()
    
    @JvmStatic
    fun Modifier.fullSize(): Modifier = this.fillMaxSize()
    
    @JvmStatic
    fun Modifier.padding(all: Int): Modifier = this.padding(all.dp)
    
    @JvmStatic
    fun Modifier.paddingHorizontal(horizontal: Int): Modifier = 
        this.padding(horizontal = horizontal.dp)
    
    @JvmStatic
    fun Modifier.paddingVertical(vertical: Int): Modifier = 
        this.padding(vertical = vertical.dp)
    
    @JvmStatic
    fun Modifier.backgroundColor(color: Long): Modifier = 
        this.background(Color(color))
    
    @JvmStatic
    fun Modifier.roundedCorners(radius: Int): Modifier = 
        this.clip(RoundedCornerShape(radius.dp))
    
    @JvmStatic
    fun Modifier.circleShape(): Modifier = 
        this.clip(CircleShape)
    
    @JvmStatic
    fun Modifier.size(width: Int, height: Int): Modifier = 
        this.size(width.dp, height.dp)
    
    @JvmStatic
    fun Modifier.clickable(onClick: () -> Unit): Modifier = 
        this.clickable { onClick() }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Colors
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun colorFromHex(hex: String): Long {
        val cleanHex = hex.removePrefix("#")
        return when (cleanHex.length) {
            6 -> "FF$cleanHex".toLong(16)
            8 -> cleanHex.toLong(16)
            else -> 0xFF000000
        }
    }
    
    @JvmStatic
    fun colorFromRgb(red: Int, green: Int, blue: Int, alpha: Int = 255): Long {
        return ((alpha and 0xFF).toLong() shl 24) or
               ((red and 0xFF).toLong() shl 16) or
               ((green and 0xFF).toLong() shl 8) or
               (blue and 0xFF).toLong()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cleanup
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun clearAllStates() {
        stringStates.clear()
        intStates.clear()
        booleanStates.clear()
        listStates.clear()
    }
}
