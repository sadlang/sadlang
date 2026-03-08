/**
 * =============================================================================
 * @file SadPlayground
 * @brief (AR) تطبيق مثال — ملعب لغة ص
 * @brief (EN) Example App — Sad Language Playground
 * =============================================================================
 */

package sad.lang.android.examples

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch
import sad.lang.android.SadResult
import sad.lang.android.SadSdk
import sad.lang.android.ui.*

/**
 * نشاط ملعب لغة ص
 */
class PlaygroundActivity : ComponentActivity() {
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        setContent {
            SadTheme {
                PlaygroundScreen()
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PlaygroundScreen() {
    var selectedTab by remember { mutableIntStateOf(0) }
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("ملعب لغة ص") }
            )
        },
        bottomBar = {
            NavigationBar {
                NavigationBarItem(
                    selected = selectedTab == 0,
                    onClick = { selectedTab = 0 },
                    icon = { Text("📝") },
                    label = { Text("محرر") }
                )
                NavigationBarItem(
                    selected = selectedTab == 1,
                    onClick = { selectedTab = 1 },
                    icon = { Text("💬") },
                    label = { Text("REPL") }
                )
                NavigationBarItem(
                    selected = selectedTab == 2,
                    onClick = { selectedTab = 2 },
                    icon = { Text("📚") },
                    label = { Text("أمثلة") }
                )
            }
        }
    ) { padding ->
        when (selectedTab) {
            0 -> EditorTab(Modifier.padding(padding))
            1 -> SadREPL(Modifier.padding(padding))
            2 -> ExamplesTab(Modifier.padding(padding))
        }
    }
}

@Composable
fun EditorTab(modifier: Modifier = Modifier) {
    var code by remember { mutableStateOf("""
# مرحباً بك في لغة ص!

دالة مرحبا(الاسم)
    ارجع "مرحباً " + الاسم + "!"
نهاية

متغير النتيجة = مرحبا("العالم")
اطبع_سطر(النتيجة)
    """.trimIndent()) }
    
    var output by remember { mutableStateOf("") }
    var isRunning by remember { mutableStateOf(false) }
    val scope = rememberCoroutineScope()
    
    Column(modifier = modifier.fillMaxSize()) {
        // Code Editor
        SadCodeEditor(
            code = code,
            onCodeChange = { code = it },
            modifier = Modifier
                .weight(1f)
                .fillMaxWidth()
        )
        
        // Run Button
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(8.dp),
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            SadButton(
                text = if (isRunning) "جاري التنفيذ..." else "▶ تشغيل",
                onClick = {
                    isRunning = true
                    scope.launch {
                        val result = SadSdk.interpreter.evalAsync(code)
                        output = when (result) {
                            is SadResult.Success -> result.value
                            is SadResult.Error -> "خطأ: ${result.message}"
                        }
                        // Add interpreter output
                        output += "\n" + SadSdk.interpreter.getOutput()
                        isRunning = false
                    }
                },
                enabled = !isRunning,
                loading = isRunning
            )
            
            SadOutlinedButton(
                text = "🗑️ مسح",
                onClick = { output = "" }
            )
        }
        
        // Output
        if (output.isNotEmpty()) {
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(150.dp)
                    .padding(8.dp)
            ) {
                Text(
                    text = output,
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(8.dp)
                        .verticalScroll(rememberScrollState())
                )
            }
        }
    }
}

@Composable
fun ExamplesTab(modifier: Modifier = Modifier) {
    val examples = listOf(
        "مرحباً بالعالم" to """
اطبع_سطر("مرحباً بالعالم!")
        """.trim(),
        
        "المتغيرات" to """
متغير الاسم = "أحمد"
متغير العمر = 25
اطبع_سطر("الاسم: " + الاسم)
اطبع_سطر("العمر: " + العمر)
        """.trim(),
        
        "الدوال" to """
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

متغير النتيجة = جمع(10، 5)
اطبع_سطر("النتيجة: " + النتيجة)
        """.trim(),
        
        "الشروط" to """
متغير الدرجة = 85

إذا (الدرجة >= 90)
    اطبع_سطر("ممتاز!")
وإلا إذا (الدرجة >= 80)
    اطبع_سطر("جيد جداً!")
وإلا
    اطبع_سطر("يحتاج تحسين")
نهاية
        """.trim(),
        
        "الحلقات" to """
# حلقة لكل
لكل رقم في [1، 2، 3، 4، 5]
    اطبع_سطر(رقم)
نهاية

# حلقة بينما
متغير عداد = 0
بينما (عداد < 5)
    اطبع(عداد)
    عداد = عداد + 1
نهاية
        """.trim(),
        
        "الصنف" to """
صنف شخص
    متغير الاسم
    متغير العمر
    
    باني(الاسم، العمر)
        هذا.الاسم = الاسم
        هذا.العمر = العمر
    نهاية
    
    دالة تقديم()
        ارجع "أنا " + هذا.الاسم
    نهاية
نهاية

متغير أحمد = جديد شخص("أحمد"، 25)
اطبع_سطر(أحمد.تقديم())
        """.trim()
    )
    
    var selectedExample by remember { mutableStateOf<Pair<String, String>?>(null) }
    
    Column(
        modifier = modifier
            .fillMaxSize()
            .padding(8.dp)
            .verticalScroll(rememberScrollState())
    ) {
        Text(
            text = "أمثلة كود لغة ص",
            style = MaterialTheme.typography.headlineSmall,
            modifier = Modifier.padding(bottom = 16.dp)
        )
        
        examples.forEach { (title, code) ->
            SadCard(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 4.dp),
                onClick = { selectedExample = title to code }
            ) {
                Text(
                    text = title,
                    style = MaterialTheme.typography.titleMedium
                )
            }
        }
    }
    
    // Example Dialog
    selectedExample?.let { (title, code) ->
        AlertDialog(
            onDismissRequest = { selectedExample = null },
            title = { Text(title) },
            text = {
                SadCodeViewer(
                    code = code,
                    modifier = Modifier.height(300.dp)
                )
            },
            confirmButton = {
                SadButton(
                    text = "نسخ",
                    onClick = {
                        SadSdk.share.copyToClipboard(code)
                        selectedExample = null
                    }
                )
            },
            dismissButton = {
                SadTextButton(
                    text = "إغلاق",
                    onClick = { selectedExample = null }
                )
            }
        )
    }
}
