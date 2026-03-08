# Sad Android SDK مثال تطبيق

## الخطوات

### 1. إضافة SDK للمشروع

في `settings.gradle`:
```groovy
include ':sad-sdk'
project(':sad-sdk').projectDir = new File('path/to/sad-sdk')
```

في `app/build.gradle`:
```groovy
dependencies {
    implementation project(':sad-sdk')
}
```

### 2. تهيئة SDK

في `Application`:
```kotlin
class MyApp : SadApplication() {
    override val sdkConfig = SadSdkConfig(
        debugMode = BuildConfig.DEBUG,
        enableStdlib = true
    )
    
    override fun onSdkReady() {
        // SDK جاهز للاستخدام
    }
}
```

أو يدوياً:
```kotlin
SadSdk.init(applicationContext, SadSdkConfig())
```

### 3. استخدام المفسر

```kotlin
// تنفيذ كود
val result = SadSdk.interpreter.eval("اطبع(\"مرحباً\")")

when (result) {
    is SadResult.Success -> println("النتيجة: ${result.value}")
    is SadResult.Error -> println("خطأ: ${result.message}")
}

// تنفيذ غير متزامن
lifecycleScope.launch {
    val result = SadSdk.interpreter.evalAsync("""
        دالة مرحبا(الاسم)
            ارجع "مرحباً " + الاسم
        نهاية
        
        مرحبا("أحمد")
    """)
}
```

### 4. استخدام التخزين

```kotlin
// كتابة ملف
SadSdk.storage.writeText("config.json", jsonContent)

// قراءة ملف
val content = SadSdk.storage.readText("config.json")
content.onSuccess { data ->
    println(data)
}

// التفضيلات
SadSdk.storage.savePreference("theme", "dark")
val theme = SadSdk.storage.getPreference("theme", "light")
```

### 5. استخدام الصوت

```kotlin
// تحميل وتشغيل مؤثر صوتي
val soundId = SadSdk.audio.loadSound("sounds/click.mp3")
SadSdk.audio.playSound(soundId)

// تشغيل موسيقى
SadSdk.audio.playMusic("music/background.mp3", loop = true)

// التحكم بالصوت
SadSdk.audio.setVolume(0.5f)
```

### 6. استخدام الشبكة

```kotlin
// HTTP GET
lifecycleScope.launch {
    val response = SadSdk.network.get("https://api.example.com/data")
    response.onSuccess { httpResponse ->
        if (httpResponse.isSuccess) {
            println(httpResponse.body)
        }
    }
}

// HTTP POST
lifecycleScope.launch {
    val json = JSONObject().apply {
        put("name", "أحمد")
        put("age", 25)
    }
    val response = SadSdk.network.postJson("https://api.example.com/users", json)
}

// WebSocket
val connectionId = SadSdk.network.connectWebSocket(
    url = "wss://echo.websocket.org",
    listener = object : SadNetwork.WebSocketListener {
        override fun onOpen() {
            SadSdk.network.sendWebSocket(connectionId, "مرحباً!")
        }
        override fun onMessage(message: String) {
            println("رسالة: $message")
        }
        // ...
    }
)
```

### 7. استخدام الإشعارات

```kotlin
// إشعار بسيط
SadSdk.notifications.show("مرحباً", "هذا إشعار من لغة ص")

// إشعار مع تقدم
val notifId = SadSdk.notifications.showProgress("تحميل", "جاري التحميل...", 0)
// تحديث التقدم
SadSdk.notifications.updateProgress(notifId, 50)
```

### 8. استخدام المشاركة

```kotlin
// مشاركة نص
SadSdk.share.shareText("مرحباً من لغة ص!")

// مشاركة ملف
SadSdk.share.shareFile("/path/to/file.pdf", "application/pdf")

// فتح رابط
SadSdk.share.openUrl("https://sad-lang.org")

// نسخ للحافظة
SadSdk.share.copyToClipboard("نص للنسخ")
```

### 9. استخدام المصادقة الحيوية

```kotlin
if (SadSdk.biometric.isAvailable()) {
    SadSdk.biometric.authenticate(
        activity = this,
        title = "تأكيد الهوية",
        subtitle = "استخدم بصمتك"
    ) { result ->
        when (result) {
            is SadBiometric.AuthResult.Success -> {
                // تمت المصادقة
            }
            is SadBiometric.AuthResult.Error -> {
                // فشلت المصادقة
            }
            is SadBiometric.AuthResult.Cancelled -> {
                // ألغى المستخدم
            }
        }
    }
}
```

### 10. استخدام Compose UI

```kotlin
@Composable
fun MyScreen() {
    SadTheme {
        Column {
            // زر
            SadButton(text = "اضغط هنا", onClick = { })
            
            // حقل نص
            var text by remember { mutableStateOf("") }
            SadTextField(
                value = text,
                onValueChange = { text = it },
                label = "الاسم"
            )
            
            // محرر كود
            var code by remember { mutableStateOf("اطبع(\"مرحباً\")") }
            SadCodeEditor(
                code = code,
                onCodeChange = { code = it }
            )
            
            // REPL
            SadREPL()
        }
    }
}
```

## الأذونات المطلوبة

```xml
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.RECORD_AUDIO" />
<uses-permission android:name="android.permission.CAMERA" />
<uses-permission android:name="android.permission.USE_BIOMETRIC" />
<uses-permission android:name="android.permission.POST_NOTIFICATIONS" />
```

## البنية

```
sad-sdk/
├── src/main/
│   ├── java/sad/lang/android/
│   │   ├── SadSdk.kt              # نقطة الدخول
│   │   ├── SadApplication.kt      # صنف قاعدة
│   │   ├── SadInterpreter.kt      # المفسر
│   │   ├── SadStorage.kt          # التخزين
│   │   ├── SadAudio.kt            # الصوت
│   │   ├── SadNetwork.kt          # الشبكة
│   │   ├── SadNotifications.kt    # الإشعارات
│   │   ├── SadShare.kt            # المشاركة
│   │   ├── SadBiometric.kt        # المصادقة الحيوية
│   │   ├── SadNfc.kt              # قراءة/كتابة NFC
│   │   ├── SadHealth.kt           # Health Connect
│   │   ├── SadMaps.kt             # خرائط Google
│   │   ├── SadBilling.kt          # المشتريات داخل التطبيق
│   │   ├── SadFirebase.kt         # Firebase Suite
│   │   ├── SadWorker.kt           # WorkManager
│   │   ├── SadBluetooth.kt        # البلوتوث BLE
│   │   ├── SadPermissions.kt      # إدارة الأذونات
│   │   └── ui/
│   │       ├── SadComponents.kt   # مكونات UI
│   │       ├── SadTheme.kt        # السمة
│   │       ├── SadCodeEditor.kt   # محرر الكود
│   │       └── SadREPL.kt         # REPL
│   └── cpp/
│       └── *.cpp                  # جسور JNI
```

---

## الوحدات الإضافية

### 11. استخدام NFC

```kotlin
// التحقق من توفر NFC
if (SadSdk.nfc.isAvailable() && SadSdk.nfc.isEnabled()) {
    // تفعيل وضع القارئ
    SadSdk.nfc.enableReaderMode(activity, object : SadNfc.NfcListener {
        override fun onTagDiscovered(tag: Tag) {
            val info = SadSdk.nfc.getTagInfo(tag)
            println("Tag ID: ${info.id}")
        }
        
        override fun onNdefMessage(records: List<SadNfc.NdefRecord>) {
            records.forEach { record ->
                println("${record.type}: ${record.payload}")
            }
        }
        
        override fun onError(message: String) {
            println("خطأ: $message")
        }
    })
}

// كتابة نص على بطاقة NFC
lifecycleScope.launch {
    val result = SadSdk.nfc.writeText(tag, "مرحباً من لغة ص!", "ar")
}
```

### 12. استخدام Health Connect

```kotlin
// التحقق من توفر Health Connect
if (SadSdk.health.isAvailable()) {
    // قراءة خطوات اليوم
    lifecycleScope.launch {
        val steps = SadSdk.health.readTodaySteps()
        println("خطوات اليوم: $steps")
    }
    
    // قراءة معدل نبض القلب
    lifecycleScope.launch {
        val startTime = Instant.now().minus(24, ChronoUnit.HOURS)
        val endTime = Instant.now()
        val heartRates = SadSdk.health.readHeartRate(startTime, endTime)
        heartRates.forEach { hr ->
            println("${hr.bpm} bpm at ${hr.time}")
        }
    }
    
    // كتابة الوزن
    lifecycleScope.launch {
        SadSdk.health.writeWeight(75.5)
    }
}
```

### 13. استخدام خرائط Google

```kotlin
// في MapFragment أو MapView
map.getMapAsync { googleMap ->
    SadSdk.maps.attachMap(googleMap, object : SadMaps.MapEventListener {
        override fun onMapClick(point: SadMaps.GeoPoint) {
            println("نقر على: ${point.latitude}, ${point.longitude}")
        }
        // ...
    })
    
    // إضافة علامة
    SadSdk.maps.addMarker(SadMaps.MapMarker(
        id = "marker1",
        position = SadMaps.GeoPoint(24.7136, 46.6753), // الرياض
        title = "الموقع",
        snippet = "وصف الموقع"
    ))
    
    // رسم خط
    SadSdk.maps.addPolyline(SadMaps.MapPolyline(
        id = "route1",
        points = listOf(
            SadMaps.GeoPoint(24.7136, 46.6753),
            SadMaps.GeoPoint(24.7200, 46.6800)
        ),
        color = Color.BLUE,
        width = 5f
    ))
    
    // التركيز على الموقع الحالي
    lifecycleScope.launch {
        SadSdk.maps.focusOnCurrentLocation(zoom = 15f)
    }
}
```

### 14. استخدام المشتريات داخل التطبيق

```kotlin
// الاتصال بخدمة الفوترة
SadSdk.billing.connect(object : SadBilling.PurchaseListener {
    override fun onPurchaseCompleted(purchase: SadBilling.Purchase) {
        println("تم الشراء: ${purchase.productId}")
    }
    
    override fun onPurchaseFailed(code: Int, message: String) {
        println("فشل الشراء: $message")
    }
    
    override fun onPurchaseCancelled() {}
    override fun onPurchasePending() {}
})

// استعلام المنتجات
lifecycleScope.launch {
    val products = SadSdk.billing.queryProducts(
        productIds = listOf("premium", "coins_100"),
        type = SadBilling.ProductType.ONE_TIME
    )
    
    products.forEach { product ->
        println("${product.name}: ${product.price}")
    }
}

// شراء منتج
lifecycleScope.launch {
    val result = SadSdk.billing.purchase(activity, "premium")
    when (result) {
        is SadBilling.BillingResult.Success -> {
            println("تم الشراء!")
        }
        is SadBilling.BillingResult.Error -> {
            println("خطأ: ${result.message}")
        }
        // ...
    }
}
```

### 15. استخدام Firebase

```kotlin
// تهيئة Firebase (تلقائي مع SDK)
SadSdk.init(this, SadSdkConfig(
    enableFirebaseAnalytics = true,
    enableCrashReporting = true
))

// تسجيل حدث
SadFirebase.Analytics.logEvent("button_click", mapOf(
    "button_name" to "submit",
    "screen" to "home"
))

// تسجيل شاشة
SadFirebase.Analytics.logScreen("HomeScreen")

// تسجيل استثناء
try {
    // ...
} catch (e: Exception) {
    SadFirebase.Crashlytics.logException(e)
}

// Remote Config
lifecycleScope.launch {
    SadFirebase.RemoteConfig.fetch()
    val welcomeMessage = SadFirebase.RemoteConfig.getString("welcome_message")
}

// FCM Token
lifecycleScope.launch {
    val token = SadFirebase.Messaging.getToken()
    println("FCM Token: $token")
}

// الاشتراك في موضوع
lifecycleScope.launch {
    SadFirebase.Messaging.subscribeToTopic("news")
}
```

### 16. استخدام WorkManager

```kotlin
// جدولة مهمة لمرة واحدة
SadWorker.scheduleOnce<SyncWorker>(
    context = this,
    tag = "sync-data",
    inputData = mapOf("endpoint" to "https://api.example.com/sync"),
    constraints = SadWorker.WorkConstraints(
        requiresNetwork = true,
        requiresCharging = false
    ),
    initialDelay = 5,
    delayUnit = TimeUnit.MINUTES
)

// جدولة مهمة دورية
SadWorker.schedulePeriodic<CleanupWorker>(
    context = this,
    tag = "cleanup",
    intervalMinutes = 60
)

// التحقق من حالة المهمة
lifecycleScope.launch {
    val state = SadWorker.getWorkState(this@MainActivity, "sync-data")
    println("حالة المهمة: $state")
}

// إلغاء مهمة
SadWorker.cancelByTag(this, "sync-data")

// إنشاء عامل مخصص
class MyWorker(context: Context, params: WorkerParameters) : 
    SadWorker.SadCoroutineWorker(context, params) {
    
    override suspend fun execute(inputData: Map<String, Any?>): SadWorker.WorkResult {
        val code = getString("code") ?: return SadWorker.WorkResult.Failure("لا يوجد كود")
        
        val result = SadSdk.interpreter.evalAsync(code)
        return when (result) {
            is SadResult.Success -> SadWorker.WorkResult.Success
            is SadResult.Error -> SadWorker.WorkResult.Failure(result.message)
        }
    }
}
```

### 17. استخدام البلوتوث

```kotlin
// التحقق من التوفر
if (SadSdk.bluetooth.isAvailable() && SadSdk.bluetooth.isEnabled()) {
    // بدء البحث عن أجهزة
    SadSdk.bluetooth.startScan(object : SadBluetooth.ScanListener {
        override fun onDeviceFound(device: SadBluetooth.BluetoothDevice) {
            println("جهاز: ${device.name ?: device.address}")
        }
        
        override fun onScanFinished() {
            println("انتهى البحث")
        }
        
        override fun onError(message: String) {
            println("خطأ: $message")
        }
    })
    
    // الاتصال بجهاز
    SadSdk.bluetooth.connect("AA:BB:CC:DD:EE:FF", object : SadBluetooth.ConnectionListener {
        override fun onConnected() {
            println("متصل!")
        }
        
        override fun onDataReceived(data: ByteArray) {
            println("بيانات: ${String(data)}")
        }
        
        override fun onDisconnected() {}
        override fun onError(message: String) {}
    })
}
```

### 18. استخدام إدارة الأذونات

```kotlin
// التحقق من الأذونات
if (SadSdk.permissions.hasCameraPermission()) {
    // استخدام الكاميرا
}

// طلب الأذونات
val requester = SadPermissions.PermissionRequester(this)
requester.register() // في onCreate

requester.requestCamera { granted ->
    if (granted) {
        // تم منح الإذن
    } else {
        // تم رفض الإذن
    }
}

// طلب جميع الأذونات الأساسية
requester.requestAllBasic { granted ->
    println("الأذونات الأساسية: $granted")
}
```

---

## الاعتماديات

تضاف تلقائياً مع SDK:

```groovy
// AndroidX Core
implementation 'androidx.core:core-ktx:1.12.0'
implementation 'androidx.appcompat:appcompat:1.6.1'

// Compose
implementation platform('androidx.compose:compose-bom:2024.02.00')
implementation 'androidx.compose.material3:material3'

// Biometric
implementation 'androidx.biometric:biometric:1.1.0'

// Network
implementation 'com.squareup.okhttp3:okhttp:4.12.0'

// Location & Maps
implementation 'com.google.android.gms:play-services-location:21.1.0'
implementation 'com.google.android.gms:play-services-maps:18.2.0'

// WorkManager
implementation 'androidx.work:work-runtime-ktx:2.9.0'

// Health Connect
implementation 'androidx.health.connect:connect-client:1.1.0-alpha07'

// In-App Billing
implementation 'com.android.billingclient:billing-ktx:6.1.0'

// Firebase
implementation platform('com.google.firebase:firebase-bom:32.7.1')
implementation 'com.google.firebase:firebase-analytics-ktx'
implementation 'com.google.firebase:firebase-crashlytics-ktx'
implementation 'com.google.firebase:firebase-messaging-ktx'
implementation 'com.google.firebase:firebase-config-ktx'
```
