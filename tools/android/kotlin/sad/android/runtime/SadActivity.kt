// ═══════════════════════════════════════════════════════════════════════════
// SadActivity.kt - Activity أساسية لتطبيقات ص
// ═══════════════════════════════════════════════════════════════════════════
//
// هذا الملف يوفر Activity أساسية تقوم بتشغيل كود ص وعرض UI
//
// ═══════════════════════════════════════════════════════════════════════════

package sad.android.runtime

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.json.JSONArray
import org.json.JSONObject

/**
 * Activity أساسية لتطبيقات لغة ص
 * ترث منها وحدد ملف main.ص في assets
 */
open class SadActivity : ComponentActivity() {
    
    companion object {
        private const val TAG = "SadActivity"
    }
    
    /**
     * اسم ملف ص الرئيسي (يمكن تغييره)
     */
    protected open val mainFile: String = "main.ص"
    
    // حالة UI
    private val uiState = mutableStateOf("")
    private var uiUpdateCallback: ((String) -> Unit)? = null
    
    // ═══════════════════════════════════════════════════════════════════════
    // دوال JNI لدورة الحياة
    // ═══════════════════════════════════════════════════════════════════════
    
    private external fun nativeOnCreate()
    private external fun nativeOnStart()
    private external fun nativeOnResume()
    private external fun nativeOnPause()
    private external fun nativeOnStop()
    private external fun nativeOnDestroy()
    private external fun nativeOnButtonClick(buttonId: String)
    private external fun nativeOnTextChange(inputId: String, text: String)
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        Log.i(TAG, "onCreate - بدء تشغيل SadActivity")
        
        // تهيئة Runtime
        if (!SadRuntime.initialize(this)) {
            Log.e(TAG, "فشل تهيئة Runtime")
            showErrorUI("فشل تهيئة وقت التشغيل")
            return
        }
        
        // تنفيذ الملف الرئيسي
        val result = SadRuntime.executeFile(mainFile)
        if (!result.success) {
            Log.e(TAG, "فشل تنفيذ $mainFile: ${result.error}")
            showErrorUI("خطأ: ${result.error}")
            return
        }
        
        // إشعار الكود الأصلي
        try {
            nativeOnCreate()
        } catch (e: UnsatisfiedLinkError) {
            Log.w(TAG, "nativeOnCreate غير متاح")
        }
        
        // عرض UI
        val uiJson = SadRuntime.getUIDescription()
        setupComposeUI(uiJson)
    }
    
    override fun onStart() {
        super.onStart()
        try { nativeOnStart() } catch (_: UnsatisfiedLinkError) {}
    }
    
    override fun onResume() {
        super.onResume()
        try { nativeOnResume() } catch (_: UnsatisfiedLinkError) {}
    }
    
    override fun onPause() {
        super.onPause()
        try { nativeOnPause() } catch (_: UnsatisfiedLinkError) {}
    }
    
    override fun onStop() {
        super.onStop()
        try { nativeOnStop() } catch (_: UnsatisfiedLinkError) {}
    }
    
    override fun onDestroy() {
        super.onDestroy()
        try { nativeOnDestroy() } catch (_: UnsatisfiedLinkError) {}
        SadRuntime.shutdown()
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // إعداد Compose UI
    // ═══════════════════════════════════════════════════════════════════════
    
    private fun setupComposeUI(initialJson: String) {
        uiState.value = initialJson
        
        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    val json by uiState
                    SadUI(json)
                }
            }
        }
    }
    
    private fun showErrorUI(message: String) {
        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.errorContainer
                ) {
                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(24.dp),
                        horizontalAlignment = Alignment.CenterHorizontally,
                        verticalArrangement = Arrangement.Center
                    ) {
                        Text(
                            text = "⚠️",
                            fontSize = 48.sp
                        )
                        Spacer(modifier = Modifier.height(16.dp))
                        Text(
                            text = "خطأ",
                            fontSize = 24.sp,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.onErrorContainer
                        )
                        Spacer(modifier = Modifier.height(8.dp))
                        Text(
                            text = message,
                            fontSize = 16.sp,
                            textAlign = TextAlign.Center,
                            color = MaterialTheme.colorScheme.onErrorContainer
                        )
                    }
                }
            }
        }
    }
    
    /**
     * تحديث UI من كود ص
     * يُستدعى من الكود الأصلي عبر JNI
     */
    fun updateUI(json: String) {
        runOnUiThread {
            uiState.value = json
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // معالجات الأحداث
    // ═══════════════════════════════════════════════════════════════════════
    
    private fun handleButtonClick(id: String) {
        Log.d(TAG, "Button clicked: $id")
        try {
            nativeOnButtonClick(id)
            // تحديث UI بعد معالجة الحدث
            val newUi = SadRuntime.getUIDescription()
            uiState.value = newUi
        } catch (e: Exception) {
            Log.e(TAG, "Error handling button click: ${e.message}")
        }
    }
    
    private fun handleTextChange(id: String, text: String) {
        Log.d(TAG, "Text changed: $id = $text")
        try {
            nativeOnTextChange(id, text)
        } catch (e: Exception) {
            Log.e(TAG, "Error handling text change: ${e.message}")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // Compose UI من JSON
    // ═══════════════════════════════════════════════════════════════════════
    
    @Composable
    private fun SadUI(json: String) {
        if (json.isBlank()) {
            LoadingUI()
            return
        }
        
        try {
            val root = JSONObject(json)
            RenderWidget(root)
        } catch (e: Exception) {
            Log.e(TAG, "Error parsing UI JSON: ${e.message}")
            ErrorWidget("خطأ في تحليل UI")
        }
    }
    
    @Composable
    private fun LoadingUI() {
        Box(
            modifier = Modifier.fillMaxSize(),
            contentAlignment = Alignment.Center
        ) {
            CircularProgressIndicator()
        }
    }
    
    @Composable
    private fun ErrorWidget(message: String) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = message,
                color = MaterialTheme.colorScheme.error
            )
        }
    }
    
    @Composable
    private fun RenderWidget(obj: JSONObject) {
        val type = obj.optString("type", "")
        
        when (type) {
            "column" -> RenderColumn(obj)
            "row" -> RenderRow(obj)
            "box" -> RenderBox(obj)
            "text" -> RenderText(obj)
            "button" -> RenderButton(obj)
            "input" -> RenderTextField(obj)
            "image" -> RenderImage(obj)
            "spacer" -> RenderSpacer(obj)
            "divider" -> RenderDivider(obj)
            "card" -> RenderCard(obj)
            "progress" -> RenderProgress(obj)
            "circularProgress" -> RenderCircularProgress(obj)
            "checkbox" -> RenderCheckbox(obj)
            "switch" -> RenderSwitch(obj)
            "slider" -> RenderSlider(obj)
            else -> {
                Log.w(TAG, "Unknown widget type: $type")
            }
        }
    }
    
    @Composable
    private fun RenderColumn(obj: JSONObject) {
        val spacing = obj.optInt("spacing", 0)
        val alignment = parseHorizontalAlignment(obj.optString("alignment", "start"))
        val children = obj.optJSONArray("children") ?: JSONArray()
        
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .applyPadding(obj)
                .then(if (obj.optBoolean("scroll", false)) 
                    Modifier.verticalScroll(rememberScrollState()) 
                else Modifier),
            horizontalAlignment = alignment,
            verticalArrangement = Arrangement.spacedBy(spacing.dp)
        ) {
            for (i in 0 until children.length()) {
                RenderWidget(children.getJSONObject(i))
            }
        }
    }
    
    @Composable
    private fun RenderRow(obj: JSONObject) {
        val spacing = obj.optInt("spacing", 0)
        val alignment = parseVerticalAlignment(obj.optString("alignment", "center"))
        val children = obj.optJSONArray("children") ?: JSONArray()
        
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .applyPadding(obj),
            verticalAlignment = alignment,
            horizontalArrangement = Arrangement.spacedBy(spacing.dp)
        ) {
            for (i in 0 until children.length()) {
                RenderWidget(children.getJSONObject(i))
            }
        }
    }
    
    @Composable
    private fun RenderBox(obj: JSONObject) {
        val children = obj.optJSONArray("children") ?: JSONArray()
        
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .applyPadding(obj),
            contentAlignment = parseBoxAlignment(obj.optString("alignment", "center"))
        ) {
            for (i in 0 until children.length()) {
                RenderWidget(children.getJSONObject(i))
            }
        }
    }
    
    @Composable
    private fun RenderText(obj: JSONObject) {
        val content = obj.optString("content", "")
        val fontSize = obj.optInt("fontSize", 16)
        val fontWeight = parseFontWeight(obj.optString("fontWeight", "normal"))
        val color = parseColor(obj.optJSONObject("color"))
        
        Text(
            text = content,
            fontSize = fontSize.sp,
            fontWeight = fontWeight,
            color = color,
            modifier = Modifier.applyPadding(obj)
        )
    }
    
    @Composable
    private fun RenderButton(obj: JSONObject) {
        val label = obj.optString("label", "زر")
        val id = obj.optString("id", "")
        val style = obj.optString("style", "filled")
        val enabled = obj.optBoolean("enabled", true)
        val color = parseColor(obj.optJSONObject("color"))
        
        when (style) {
            "filled" -> Button(
                onClick = { handleButtonClick(id) },
                enabled = enabled,
                colors = ButtonDefaults.buttonColors(containerColor = color),
                modifier = Modifier.applyPadding(obj)
            ) {
                Text(label)
            }
            "outlined" -> OutlinedButton(
                onClick = { handleButtonClick(id) },
                enabled = enabled,
                modifier = Modifier.applyPadding(obj)
            ) {
                Text(label)
            }
            "text" -> TextButton(
                onClick = { handleButtonClick(id) },
                enabled = enabled,
                modifier = Modifier.applyPadding(obj)
            ) {
                Text(label)
            }
        }
    }
    
    @Composable
    private fun RenderTextField(obj: JSONObject) {
        val id = obj.optString("id", "")
        val placeholder = obj.optString("placeholder", "")
        val label = obj.optString("label", "")
        val initialValue = obj.optString("value", "")
        
        var text by remember { mutableStateOf(initialValue) }
        
        OutlinedTextField(
            value = text,
            onValueChange = { newText ->
                text = newText
                handleTextChange(id, newText)
            },
            label = if (label.isNotEmpty()) {{ Text(label) }} else null,
            placeholder = if (placeholder.isNotEmpty()) {{ Text(placeholder) }} else null,
            modifier = Modifier
                .fillMaxWidth()
                .applyPadding(obj)
        )
    }
    
    @Composable
    private fun RenderImage(obj: JSONObject) {
        // TODO: تحميل الصور
        val source = obj.optString("source", "")
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(200.dp)
                .applyPadding(obj),
            contentAlignment = Alignment.Center
        ) {
            Text("صورة: $source")
        }
    }
    
    @Composable
    private fun RenderSpacer(obj: JSONObject) {
        val height = parseSize(obj.optJSONObject("height"))
        val width = parseSize(obj.optJSONObject("width"))
        
        Spacer(
            modifier = Modifier
                .height(height.dp)
                .width(width.dp)
        )
    }
    
    @Composable
    private fun RenderDivider(obj: JSONObject) {
        val color = parseColor(obj.optJSONObject("color"))
        val thickness = obj.optDouble("thickness", 1.0)
        
        Divider(
            color = color,
            thickness = thickness.dp,
            modifier = Modifier.applyPadding(obj)
        )
    }
    
    @Composable
    private fun RenderCard(obj: JSONObject) {
        val elevation = obj.optInt("elevation", 4)
        val children = obj.optJSONArray("children") ?: JSONArray()
        
        Card(
            elevation = CardDefaults.cardElevation(defaultElevation = elevation.dp),
            modifier = Modifier
                .fillMaxWidth()
                .applyPadding(obj)
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                for (i in 0 until children.length()) {
                    RenderWidget(children.getJSONObject(i))
                }
            }
        }
    }
    
    @Composable
    private fun RenderProgress(obj: JSONObject) {
        val progress = obj.optDouble("progress", 0.0).toFloat()
        val indeterminate = obj.optBoolean("indeterminate", false)
        
        if (indeterminate) {
            LinearProgressIndicator(modifier = Modifier.fillMaxWidth().applyPadding(obj))
        } else {
            LinearProgressIndicator(
                progress = progress,
                modifier = Modifier.fillMaxWidth().applyPadding(obj)
            )
        }
    }
    
    @Composable
    private fun RenderCircularProgress(obj: JSONObject) {
        val progress = obj.optDouble("progress", 0.0).toFloat()
        val indeterminate = obj.optBoolean("indeterminate", false)
        
        if (indeterminate) {
            CircularProgressIndicator(modifier = Modifier.applyPadding(obj))
        } else {
            CircularProgressIndicator(
                progress = progress,
                modifier = Modifier.applyPadding(obj)
            )
        }
    }
    
    @Composable
    private fun RenderCheckbox(obj: JSONObject) {
        val id = obj.optString("id", "")
        val label = obj.optString("label", "")
        val initialChecked = obj.optBoolean("checked", false)
        
        var checked by remember { mutableStateOf(initialChecked) }
        
        Row(
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier.applyPadding(obj)
        ) {
            Checkbox(
                checked = checked,
                onCheckedChange = { newValue ->
                    checked = newValue
                    SadRuntime.setVariable(id, newValue.toString())
                }
            )
            Text(label)
        }
    }
    
    @Composable
    private fun RenderSwitch(obj: JSONObject) {
        val id = obj.optString("id", "")
        val label = obj.optString("label", "")
        val initialChecked = obj.optBoolean("checked", false)
        
        var checked by remember { mutableStateOf(initialChecked) }
        
        Row(
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween,
            modifier = Modifier
                .fillMaxWidth()
                .applyPadding(obj)
        ) {
            Text(label)
            Switch(
                checked = checked,
                onCheckedChange = { newValue ->
                    checked = newValue
                    SadRuntime.setVariable(id, newValue.toString())
                }
            )
        }
    }
    
    @Composable
    private fun RenderSlider(obj: JSONObject) {
        val id = obj.optString("id", "")
        val min = obj.optDouble("min", 0.0).toFloat()
        val max = obj.optDouble("max", 100.0).toFloat()
        val initialValue = obj.optDouble("value", 0.0).toFloat()
        
        var value by remember { mutableStateOf(initialValue) }
        
        Slider(
            value = value,
            onValueChange = { newValue ->
                value = newValue
                SadRuntime.setVariable(id, newValue.toString())
            },
            valueRange = min..max,
            modifier = Modifier.applyPadding(obj)
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // مساعدات التحليل
    // ═══════════════════════════════════════════════════════════════════════
    
    private fun Modifier.applyPadding(obj: JSONObject): Modifier {
        val padding = obj.optJSONObject("padding") ?: return this
        val top = padding.optInt("top", 0)
        val right = padding.optInt("right", 0)
        val bottom = padding.optInt("bottom", 0)
        val left = padding.optInt("left", 0)
        return this.padding(left.dp, top.dp, right.dp, bottom.dp)
    }
    
    private fun parseColor(obj: JSONObject?): Color {
        if (obj == null) return Color.Black
        val r = obj.optInt("r", 0)
        val g = obj.optInt("g", 0)
        val b = obj.optInt("b", 0)
        val a = obj.optInt("a", 255)
        return Color(r, g, b, a)
    }
    
    private fun parseSize(obj: JSONObject?): Int {
        if (obj == null) return 16
        val type = obj.optString("type", "fixed")
        return when (type) {
            "fixed" -> obj.optInt("value", 16)
            else -> 16
        }
    }
    
    private fun parseFontWeight(weight: String): FontWeight {
        return when (weight.lowercase()) {
            "thin" -> FontWeight.Thin
            "light" -> FontWeight.Light
            "normal" -> FontWeight.Normal
            "medium" -> FontWeight.Medium
            "semibold" -> FontWeight.SemiBold
            "bold" -> FontWeight.Bold
            "extrabold" -> FontWeight.ExtraBold
            "black" -> FontWeight.Black
            else -> FontWeight.Normal
        }
    }
    
    private fun parseHorizontalAlignment(alignment: String): Alignment.Horizontal {
        return when (alignment.lowercase()) {
            "start", "topstart", "centerstart", "bottomstart" -> Alignment.Start
            "center", "topcenter", "centercenter", "bottomcenter" -> Alignment.CenterHorizontally
            "end", "topend", "centerend", "bottomend" -> Alignment.End
            else -> Alignment.Start
        }
    }
    
    private fun parseVerticalAlignment(alignment: String): Alignment.Vertical {
        return when (alignment.lowercase()) {
            "top", "topstart", "topcenter", "topend" -> Alignment.Top
            "center", "centerstart", "centercenter", "centerend" -> Alignment.CenterVertically
            "bottom", "bottomstart", "bottomcenter", "bottomend" -> Alignment.Bottom
            else -> Alignment.CenterVertically
        }
    }
    
    private fun parseBoxAlignment(alignment: String): Alignment {
        return when (alignment.lowercase()) {
            "topstart" -> Alignment.TopStart
            "topcenter" -> Alignment.TopCenter
            "topend" -> Alignment.TopEnd
            "centerstart" -> Alignment.CenterStart
            "center", "centercenter" -> Alignment.Center
            "centerend" -> Alignment.CenterEnd
            "bottomstart" -> Alignment.BottomStart
            "bottomcenter" -> Alignment.BottomCenter
            "bottomend" -> Alignment.BottomEnd
            else -> Alignment.Center
        }
    }
}
