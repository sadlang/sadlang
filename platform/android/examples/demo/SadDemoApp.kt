/**
 * =============================================================================
 * @file SadDemoApp.kt
 * @brief (AR) تطبيق مثال شامل — عرض جميع مكونات SDK
 * @brief (EN) Comprehensive Demo App — Showcasing all SDK Components
 * =============================================================================
 */

package sad.lang.android.demo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.animation.*
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch
import sad.lang.android.*
import sad.lang.android.ui.*

// ═══════════════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ═══════════════════════════════════════════════════════════════════════════════

class SadDemoActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // تهيئة SDK
        SadSdk.init(this)
        
        setContent {
            SadTheme {
                SadDemoApp()
            }
        }
    }
    
    override fun onDestroy() {
        super.onDestroy()
        SadSdk.shutdown()
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  التطبيق الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SadDemoApp() {
    var selectedSection by remember { mutableStateOf<DemoSection?>(null) }
    
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("عرض Sad SDK") },
                navigationIcon = {
                    if (selectedSection != null) {
                        IconButton(onClick = { selectedSection = null }) {
                            Icon(Icons.Default.ArrowBack, contentDescription = "رجوع")
                        }
                    }
                }
            )
        }
    ) { padding ->
        AnimatedContent(
            targetState = selectedSection,
            modifier = Modifier.padding(padding)
        ) { section ->
            when (section) {
                null -> DemoSectionList { selectedSection = it }
                DemoSection.INTERPRETER -> InterpreterDemo()
                DemoSection.STORAGE -> StorageDemo()
                DemoSection.AUDIO -> AudioDemo()
                DemoSection.NETWORK -> NetworkDemo()
                DemoSection.NOTIFICATIONS -> NotificationsDemo()
                DemoSection.BIOMETRIC -> BiometricDemo()
                DemoSection.NFC -> NfcDemo()
                DemoSection.HEALTH -> HealthDemo()
                DemoSection.FIREBASE -> FirebaseDemo()
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  أقسام العرض
// ═══════════════════════════════════════════════════════════════════════════════

enum class DemoSection(val title: String, val icon: ImageVector, val description: String) {
    INTERPRETER("المفسر", Icons.Default.Code, "تنفيذ كود لغة ص"),
    STORAGE("التخزين", Icons.Default.Storage, "قراءة وكتابة الملفات"),
    AUDIO("الصوت", Icons.Default.AudioFile, "تشغيل وتسجيل الصوت"),
    NETWORK("الشبكة", Icons.Default.Wifi, "طلبات HTTP و WebSocket"),
    NOTIFICATIONS("الإشعارات", Icons.Default.Notifications, "إرسال الإشعارات المحلية"),
    BIOMETRIC("البصمة", Icons.Default.Fingerprint, "المصادقة البيومترية"),
    NFC("NFC", Icons.Default.Nfc, "قراءة وكتابة بطاقات NFC"),
    HEALTH("الصحة", Icons.Default.HealthAndSafety, "بيانات اللياقة والصحة"),
    FIREBASE("Firebase", Icons.Default.Cloud, "التحليلات والرسائل السحابية")
}

@Composable
fun DemoSectionList(onSectionClick: (DemoSection) -> Unit) {
    LazyColumn(
        modifier = Modifier.fillMaxSize(),
        contentPadding = PaddingValues(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp)
    ) {
        items(DemoSection.entries) { section ->
            Card(
                onClick = { onSectionClick(section) },
                modifier = Modifier.fillMaxWidth()
            ) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(16.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = section.icon,
                        contentDescription = null,
                        modifier = Modifier.size(40.dp),
                        tint = MaterialTheme.colorScheme.primary
                    )
                    Spacer(Modifier.width(16.dp))
                    Column(modifier = Modifier.weight(1f)) {
                        Text(
                            text = section.title,
                            style = MaterialTheme.typography.titleMedium
                        )
                        Text(
                            text = section.description,
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant
                        )
                    }
                    Icon(
                        imageVector = Icons.Default.ChevronRight,
                        contentDescription = null
                    )
                }
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض المفسر
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun InterpreterDemo() {
    var code by remember { mutableStateOf("""
# مثال بسيط
دالة مرحبا(الاسم)
    ارجع "مرحباً " + الاسم + "!"
نهاية

متغير النتيجة = مرحبا("العالم")
اطبع_سطر(النتيجة)
    """.trimIndent()) }
    var output by remember { mutableStateOf("") }
    var isRunning by remember { mutableStateOf(false) }
    val scope = rememberCoroutineScope()
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        Text("محرر الكود", style = MaterialTheme.typography.titleMedium)
        Spacer(Modifier.height(8.dp))
        
        SadCodeEditor(
            code = code,
            onCodeChange = { code = it },
            modifier = Modifier
                .weight(1f)
                .fillMaxWidth()
        )
        
        Spacer(Modifier.height(8.dp))
        
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            SadButton(
                text = "▶ تشغيل",
                onClick = {
                    isRunning = true
                    scope.launch {
                        val result = SadSdk.interpreter.evalAsync(code)
                        output = when (result) {
                            is SadResult.Success -> "✅ نجاح: ${result.value}"
                            is SadResult.Error -> "❌ خطأ: ${result.message}"
                        }
                        output += "\n\n📝 المخرجات:\n${SadSdk.interpreter.getOutput()}"
                        isRunning = false
                    }
                },
                loading = isRunning,
                modifier = Modifier.weight(1f)
            )
            
            SadOutlinedButton(
                text = "🗑️ مسح",
                onClick = { output = "" },
                modifier = Modifier.weight(1f)
            )
        }
        
        if (output.isNotEmpty()) {
            Spacer(Modifier.height(8.dp))
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(150.dp)
            ) {
                Text(
                    text = output,
                    modifier = Modifier.padding(8.dp)
                )
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض التخزين
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun StorageDemo() {
    var filename by remember { mutableStateOf("test.txt") }
    var content by remember { mutableStateOf("مرحباً من لغة ص!") }
    var readResult by remember { mutableStateOf("") }
    val scope = rememberCoroutineScope()
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("عمليات الملفات", style = MaterialTheme.typography.titleMedium)
        
        SadTextField(
            value = filename,
            onValueChange = { filename = it },
            label = "اسم الملف"
        )
        
        SadTextField(
            value = content,
            onValueChange = { content = it },
            label = "المحتوى",
            minLines = 3
        )
        
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            SadButton(
                text = "💾 حفظ",
                onClick = {
                    scope.launch {
                        val success = SadSdk.storage.writeFile(filename, content)
                        readResult = if (success) "✅ تم الحفظ بنجاح" else "❌ فشل الحفظ"
                    }
                },
                modifier = Modifier.weight(1f)
            )
            
            SadButton(
                text = "📖 قراءة",
                onClick = {
                    scope.launch {
                        val result = SadSdk.storage.readFile(filename)
                        readResult = result ?: "❌ الملف غير موجود"
                    }
                },
                modifier = Modifier.weight(1f)
            )
            
            SadButton(
                text = "🗑️ حذف",
                onClick = {
                    scope.launch {
                        val success = SadSdk.storage.deleteFile(filename)
                        readResult = if (success) "✅ تم الحذف" else "❌ فشل الحذف"
                    }
                },
                modifier = Modifier.weight(1f)
            )
        }
        
        if (readResult.isNotEmpty()) {
            Card(modifier = Modifier.fillMaxWidth()) {
                Text(
                    text = readResult,
                    modifier = Modifier.padding(16.dp)
                )
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض الصوت
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun AudioDemo() {
    var isPlaying by remember { mutableStateOf(false) }
    var isRecording by remember { mutableStateOf(false) }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("التحكم بالصوت", style = MaterialTheme.typography.titleMedium)
        
        Card(modifier = Modifier.fillMaxWidth()) {
            Column(
                modifier = Modifier.padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text("🎵 تشغيل الموسيقى")
                Spacer(Modifier.height(8.dp))
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    SadButton(
                        text = if (isPlaying) "⏸️ إيقاف مؤقت" else "▶️ تشغيل",
                        onClick = {
                            if (isPlaying) {
                                SadSdk.audio.pauseMusic()
                            } else {
                                // Example: Play from raw resource
                                SadSdk.audio.playMusic("example_music")
                            }
                            isPlaying = !isPlaying
                        }
                    )
                    SadButton(
                        text = "⏹️ إيقاف",
                        onClick = {
                            SadSdk.audio.stopMusic()
                            isPlaying = false
                        }
                    )
                }
            }
        }
        
        Card(modifier = Modifier.fillMaxWidth()) {
            Column(
                modifier = Modifier.padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text("🎙️ التسجيل")
                Spacer(Modifier.height(8.dp))
                SadButton(
                    text = if (isRecording) "⏹️ إيقاف التسجيل" else "🔴 بدء التسجيل",
                    onClick = {
                        if (isRecording) {
                            SadSdk.audio.stopRecording()
                        } else {
                            SadSdk.audio.startRecording("recording.m4a")
                        }
                        isRecording = !isRecording
                    }
                )
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض الشبكة
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun NetworkDemo() {
    var url by remember { mutableStateOf("https://api.github.com/zen") }
    var response by remember { mutableStateOf("") }
    var isLoading by remember { mutableStateOf(false) }
    val scope = rememberCoroutineScope()
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("طلبات HTTP", style = MaterialTheme.typography.titleMedium)
        
        SadTextField(
            value = url,
            onValueChange = { url = it },
            label = "الرابط (URL)"
        )
        
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            SadButton(
                text = "GET",
                onClick = {
                    isLoading = true
                    scope.launch {
                        response = try {
                            val result = SadSdk.network.get(url)
                            "✅ نجاح:\n$result"
                        } catch (e: Exception) {
                            "❌ خطأ: ${e.message}"
                        }
                        isLoading = false
                    }
                },
                loading = isLoading,
                modifier = Modifier.weight(1f)
            )
            
            SadButton(
                text = "POST",
                onClick = {
                    isLoading = true
                    scope.launch {
                        response = try {
                            val result = SadSdk.network.post(url, "{\"test\":true}")
                            "✅ نجاح:\n$result"
                        } catch (e: Exception) {
                            "❌ خطأ: ${e.message}"
                        }
                        isLoading = false
                    }
                },
                loading = isLoading,
                modifier = Modifier.weight(1f)
            )
        }
        
        if (response.isNotEmpty()) {
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1f)
            ) {
                Text(
                    text = response,
                    modifier = Modifier.padding(16.dp)
                )
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض الإشعارات
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun NotificationsDemo() {
    var title by remember { mutableStateOf("إشعار من لغة ص") }
    var body by remember { mutableStateOf("هذا إشعار تجريبي!") }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("الإشعارات المحلية", style = MaterialTheme.typography.titleMedium)
        
        SadTextField(
            value = title,
            onValueChange = { title = it },
            label = "العنوان"
        )
        
        SadTextField(
            value = body,
            onValueChange = { body = it },
            label = "المحتوى",
            minLines = 2
        )
        
        SadButton(
            text = "🔔 إرسال إشعار",
            onClick = {
                SadSdk.notifications.show(
                    id = System.currentTimeMillis().toInt(),
                    title = title,
                    body = body
                )
            },
            modifier = Modifier.fillMaxWidth()
        )
        
        SadButton(
            text = "📊 إشعار مع شريط تقدم",
            onClick = {
                SadSdk.notifications.showProgress(
                    id = 100,
                    title = "جاري التحميل...",
                    progress = 75,
                    max = 100
                )
            },
            modifier = Modifier.fillMaxWidth()
        )
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض البصمة
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun BiometricDemo() {
    var result by remember { mutableStateOf("") }
    val scope = rememberCoroutineScope()
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("المصادقة البيومترية", style = MaterialTheme.typography.titleMedium)
        
        Icon(
            imageVector = Icons.Default.Fingerprint,
            contentDescription = null,
            modifier = Modifier.size(100.dp),
            tint = MaterialTheme.colorScheme.primary
        )
        
        Text("استخدم بصمتك للمصادقة")
        
        SadButton(
            text = "🔐 التحقق",
            onClick = {
                scope.launch {
                    val authResult = SadSdk.biometric.authenticate(
                        title = "التحقق من الهوية",
                        subtitle = "استخدم بصمتك للمتابعة",
                        negativeButtonText = "إلغاء"
                    )
                    result = when (authResult) {
                        is SadBiometric.AuthResult.Success -> "✅ تم التحقق بنجاح!"
                        is SadBiometric.AuthResult.Error -> "❌ خطأ: ${authResult.message}"
                        SadBiometric.AuthResult.Cancelled -> "⚠️ تم الإلغاء"
                        SadBiometric.AuthResult.Failed -> "❌ فشل التحقق"
                    }
                }
            },
            modifier = Modifier.width(200.dp)
        )
        
        if (result.isNotEmpty()) {
            Card(modifier = Modifier.fillMaxWidth()) {
                Text(
                    text = result,
                    modifier = Modifier.padding(16.dp)
                )
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض NFC
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun NfcDemo() {
    val nfc = remember { SadNfc(/* context */) }
    var status by remember { mutableStateOf("") }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("قـارئ NFC", style = MaterialTheme.typography.titleMedium)
        
        Icon(
            imageVector = Icons.Default.Nfc,
            contentDescription = null,
            modifier = Modifier.size(100.dp),
            tint = MaterialTheme.colorScheme.primary
        )
        
        Text("قرّب بطاقة NFC من الجهاز")
        
        Card(modifier = Modifier.fillMaxWidth()) {
            Column(modifier = Modifier.padding(16.dp)) {
                Text("حالة NFC:", style = MaterialTheme.typography.titleSmall)
                Spacer(Modifier.height(8.dp))
                Text(
                    text = if (status.isEmpty()) "في انتظار بطاقة..." else status,
                    color = MaterialTheme.colorScheme.onSurfaceVariant
                )
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض الصحة
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun HealthDemo() {
    var stepsToday by remember { mutableStateOf<Long?>(null) }
    var isLoading by remember { mutableStateOf(false) }
    val scope = rememberCoroutineScope()
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("بيانات الصحة واللياقة", style = MaterialTheme.typography.titleMedium)
        
        Icon(
            imageVector = Icons.Default.HealthAndSafety,
            contentDescription = null,
            modifier = Modifier.size(100.dp),
            tint = MaterialTheme.colorScheme.primary
        )
        
        Card(modifier = Modifier.fillMaxWidth()) {
            Column(
                modifier = Modifier.padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                Text("👟 خطوات اليوم", style = MaterialTheme.typography.titleMedium)
                Spacer(Modifier.height(8.dp))
                Text(
                    text = stepsToday?.toString() ?: "---",
                    style = MaterialTheme.typography.displayMedium,
                    color = MaterialTheme.colorScheme.primary
                )
                Text("خطوة")
            }
        }
        
        SadButton(
            text = "🔄 تحديث",
            onClick = {
                isLoading = true
                scope.launch {
                    // stepsToday = SadSdk.health.readTodaySteps()
                    stepsToday = 8547 // Demo value
                    isLoading = false
                }
            },
            loading = isLoading,
            modifier = Modifier.width(200.dp)
        )
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  عرض Firebase
// ═══════════════════════════════════════════════════════════════════════════════

@Composable
fun FirebaseDemo() {
    var eventName by remember { mutableStateOf("button_click") }
    var logMessage by remember { mutableStateOf("") }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(16.dp)
    ) {
        Text("Firebase Analytics", style = MaterialTheme.typography.titleMedium)
        
        SadTextField(
            value = eventName,
            onValueChange = { eventName = it },
            label = "اسم الحدث"
        )
        
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            SadButton(
                text = "📊 تسجيل حدث",
                onClick = {
                    SadFirebase.Analytics.logEvent(eventName, mapOf(
                        "timestamp" to System.currentTimeMillis(),
                        "source" to "demo_app"
                    ))
                    logMessage = "✅ تم تسجيل الحدث: $eventName"
                },
                modifier = Modifier.weight(1f)
            )
            
            SadButton(
                text = "📱 تسجيل شاشة",
                onClick = {
                    SadFirebase.Analytics.logScreen("DemoScreen", "FirebaseDemo")
                    logMessage = "✅ تم تسجيل الشاشة"
                },
                modifier = Modifier.weight(1f)
            )
        }
        
        Divider()
        
        Text("Crashlytics", style = MaterialTheme.typography.titleMedium)
        
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            SadButton(
                text = "📝 تسجيل رسالة",
                onClick = {
                    SadFirebase.Crashlytics.log("رسالة اختبار من التطبيق")
                    logMessage = "✅ تم تسجيل الرسالة"
                },
                modifier = Modifier.weight(1f)
            )
            
            SadButton(
                text = "⚠️ تسجيل استثناء",
                onClick = {
                    SadFirebase.Crashlytics.logException(Exception("استثناء تجريبي"))
                    logMessage = "✅ تم تسجيل الاستثناء"
                },
                modifier = Modifier.weight(1f)
            )
        }
        
        if (logMessage.isNotEmpty()) {
            Card(modifier = Modifier.fillMaxWidth()) {
                Text(
                    text = logMessage,
                    modifier = Modifier.padding(16.dp)
                )
            }
        }
    }
}
