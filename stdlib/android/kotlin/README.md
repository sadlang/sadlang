# جسور Kotlin لمكتبة لغة ص (Sad Kotlin Bridges)

## نظرة عامة

هذا المجلد يحتوي على جسور Kotlin التي تربط دوال `__اندرويد_*` الأصلية في لغة ص بـ Android APIs الفعلية.

## الملفات

| الملف | الدور | الدوال الأصلية |
|-------|------|----------------|
| `SadBridgeManager.kt` | مدير التهيئة | - |
| `SadHttpBridge.kt` | طلبات HTTP | `__اندرويد_http_get/post/download` |
| `SadNotificationBridge.kt` | الإشعارات | `__اندرويد_اشعار_ارسل/الغ` |
| `SadStorageBridge.kt` | التخزين المحلي | `__اندرويد_تخزين_احفظ/اقرأ/احذف` |
| `SadLocationBridge.kt` | الموقع الجغرافي | `__اندرويد_موقع_الحالي/تتبع` |
| `SadCameraBridge.kt` | الكاميرا | `__اندرويد_كاميرا_افتح/التقط` |
| `SadPermissionBridge.kt` | الأذونات | `__اندرويد_اذونات_تحقق/اطلب` |
| `SadJsonBridge.kt` | معالجة JSON | `__اندرويد_json_parse/stringify/get/set` |
| `SadAudioBridge.kt` | الصوت والتسجيل | `__اندرويد_صوت_شغل/اوقف/سجل` |
| `SadCryptoBridge.kt` | التشفير | `__اندرويد_تشفير_hash/encrypt/decrypt` |

## التثبيت

### 1. إضافة الملفات

انسخ هذا المجلد إلى مشروع Android في:
```
app/src/main/java/com/sad/bridges/
```

### 2. إضافة الاعتماديات في `build.gradle.kts`

```kotlin
dependencies {
    // HTTP (OkHttp)
    implementation("com.squareup.okhttp3:okhttp:4.12.0")
    
    // Coroutines
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-android:1.7.3")
    
    // DataStore للتخزين
    implementation("androidx.datastore:datastore-preferences:1.0.0")
    
    // Location
    implementation("com.google.android.gms:play-services-location:21.0.1")
    
    // CameraX
    implementation("androidx.camera:camera-core:1.3.0")
    implementation("androidx.camera:camera-camera2:1.3.0")
    implementation("androidx.camera:camera-lifecycle:1.3.0")
    implementation("androidx.camera:camera-view:1.3.0")
    
    // JSON (Gson)
    implementation("com.google.code.gson:gson:2.10.1")
}
```

### 3. إضافة الأذونات في `AndroidManifest.xml`

```xml
<!-- الإنترنت -->
<uses-permission android:name="android.permission.INTERNET" />

<!-- الإشعارات (Android 13+) -->
<uses-permission android:name="android.permission.POST_NOTIFICATIONS" />

<!-- الموقع -->
<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
<uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />

<!-- الكاميرا -->
<uses-permission android:name="android.permission.CAMERA" />

<!-- التخزين (للإصدارات القديمة) -->
<uses-permission 
    android:name="android.permission.READ_EXTERNAL_STORAGE" 
    android:maxSdkVersion="32" />
<uses-permission 
    android:name="android.permission.WRITE_EXTERNAL_STORAGE" 
    android:maxSdkVersion="32" />
```

### 4. التهيئة في MainActivity

```kotlin
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        // تهيئة جسور لغة ص
        SadBridgeManager.initialize(this)
        
        setContent {
            // ...
        }
    }
    
    override fun onDestroy() {
        super.onDestroy()
        SadBridgeManager.cleanup()
    }
}
```

## الاستخدام من لغة ص

### HTTP

```sad
استورد "شبكة/http"

# طلب GET
متغير استجابة = http.احصل("https://api.example.com/data")
إذا (استجابة.نجح)
    اطبع(استجابة.محتوى)
نهاية

# طلب POST
متغير بيانات = '{"اسم": "محمد"}'
متغير نتيجة = http.ارسل("https://api.example.com/users", بيانات)
```

### الإشعارات

```sad
استورد "نظام/اشعارات"

اشعارات.ارسل("عنوان", "نص الإشعار")
```

### التخزين

```sad
استورد "تخزين/محلي"

محلي.احفظ("اسم_المستخدم", "محمد")
متغير اسم = محلي.اقرأ("اسم_المستخدم")
```

### الموقع

```sad
استورد "موقع/gps"

gps.الحالي(دالة(موقع)
    إذا (موقع.نجح)
        اطبع("خط العرض: " + موقع.خط_العرض)
    نهاية
نهاية)
```

### الأذونات

```sad
استورد "نظام/اذونات"

إذا (اذونات.تحقق("كاميرا"))
    # فتح الكاميرا
وإلا
    اذونات.اطلب(["كاميرا"])
نهاية
```

## الربط مع الكود المُولّد

عند استخدام `sadc ui generate`، يتم تلقائياً:

1. نسخ الجسور المطلوبة إلى المشروع
2. إضافة الاعتماديات في `build.gradle.kts`
3. إضافة الأذونات في `AndroidManifest.xml`
4. استدعاء `SadBridgeManager.initialize()` في MainActivity

## ملاحظات تقنية

### الأداء
- جميع طلبات HTTP تُنفذ على `Dispatchers.IO`
- التخزين يدعم `SharedPreferences` (متزامن) و `DataStore` (غير متزامن)
- الكاميرا تستخدم `CameraX` لأفضل توافق

### التوافق
- الحد الأدنى: Android API 21 (Android 5.0)
- الموصى به: Android API 33 (Android 13)

### الأخطاء
- جميع الدوال تُرجع `Map` مع حقل `خطأ` عند الفشل
- استخدم `نجح` للتحقق من نجاح العملية

## الترخيص

MIT License - مفتوح المصدر
