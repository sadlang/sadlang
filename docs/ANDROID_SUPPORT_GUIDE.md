# دليل دعم لغة ص (Sad) لأندرويد — الشرح الموسّع

## بسم الله الرحمن الرحيم

هذا الدليل الشامل يوضح كيفية استخدام لغة ص (Sad) لتطوير تطبيقات أندرويد الأصلية، مع أكثر من **105 جسور Kotlin** مدمجة.

---

## 🎯 نظرة عامة

لغة ص هي لغة برمجة عربية حديثة تدعم تطوير تطبيقات أندرويد من خلال جسور Kotlin مُحسّنة. هذه الجسور توفر واجهة برمجية عربية سهلة الاستخدام لجميع واجهات أندرويد البرمجية (APIs).

### المميزات الرئيسية:
- ✅ **+105 جسور Kotlin** جاهزة للاستخدام
- ✅ **واجهة عربية كاملة** — رسائل الخطأ والتوثيق بالعربية
- ✅ **دعم Jetpack** — Compose, ViewModel, Lifecycle, Paging, WorkManager
- ✅ **معالجة النصوص العربية** — تشكيل، تطبيع، RTL
- ✅ **تكامل أصلي** — لا حاجة لأي مكتبات خارجية إضافية

---

## 📁 هيكل الجسور

```
stdlib/android/kotlin/
├── core/                    # الأساسيات
│   └── SadCoreBridge.kt
├── activity/                # الأنشطة
│   └── SadActivityBridge.kt
├── animation/               # الرسوم المتحركة
│   └── SadAnimationBridge.kt
├── arabicstring/            # معالجة النصوص العربية
│   └── SadArabicStringBridge.kt
├── audio/                   # الصوت
│   └── SadAudioBridge.kt
├── biometric/               # البصمة والتحقق
│   └── SadBiometricBridge.kt
├── bluetooth/               # البلوتوث
│   └── SadBluetoothBridge.kt
├── cache/                   # التخزين المؤقت
│   └── SadCacheBridge.kt
├── calendar/                # التقويم
│   └── SadCalendarBridge.kt
├── camera/                  # الكاميرا
│   └── SadCameraBridge.kt
├── clipboard/               # الحافظة
│   └── SadClipboardBridge.kt
├── collection/              # المجموعات
│   └── SadCollectionBridge.kt
├── compose/                 # Jetpack Compose
│   └── SadComposeBridge.kt
├── connectivity/            # الاتصال
│   └── SadConnectivityBridge.kt
├── contacts/                # جهات الاتصال
│   └── SadContactsBridge.kt
├── crypto/                  # التشفير
│   └── SadCryptoBridge.kt
├── device/                  # معلومات الجهاز
│   └── SadDeviceBridge.kt
├── dialog/                  # مربعات الحوار
│   └── SadDialogBridge.kt
├── download/                # التنزيل
│   └── SadDownloadBridge.kt
├── dragdrop/                # السحب والإفلات
│   └── SadDragDropBridge.kt
├── file/                    # الملفات
│   └── SadFileBridge.kt
├── firebase/                # Firebase
│   └── SadFirebaseBridge.kt
├── formatter/               # التنسيق
│   └── SadFormatterBridge.kt
├── gesture/                 # الإيماءات
│   └── SadGestureBridge.kt
├── graphics/                # الرسوميات
│   └── SadGraphicsBridge.kt
├── http/                    # HTTP
│   └── SadHttpBridge.kt
├── image/                   # معالجة الصور
│   └── SadImageLoaderBridge.kt
├── intent/                  # الـ Intents
│   └── SadIntentBridge.kt
├── json/                    # JSON
│   └── SadJsonBridge.kt
├── keyboard/                # لوحة المفاتيح
│   └── SadKeyboardBridge.kt
├── lifecycle/               # دورة الحياة
│   └── SadLifecycleBridge.kt
├── location/                # الموقع
│   └── SadLocationBridge.kt
├── logging/                 # التسجيل
│   └── SadLoggingBridge.kt
├── math/                    # الرياضيات
│   └── SadMathBridge.kt
├── media/                   # الوسائط
│   └── SadMediaBridge.kt
├── navigation/              # التنقل
│   └── SadNavigationBridge.kt
├── notification/            # الإشعارات
│   └── SadNotificationBridge.kt
├── nfc/                     # NFC
│   └── SadNfcBridge.kt
├── paging/                  # Paging
│   └── SadPagingBridge.kt
├── permission/              # الصلاحيات
│   └── SadPermissionBridge.kt
├── preferences/             # التفضيلات
│   └── SadPreferencesBridge.kt
├── qrcode/                  # QR Code
│   └── SadQrCodeBridge.kt
├── recycler/                # RecyclerView
│   └── SadRecyclerBridge.kt
├── regex/                   # التعبيرات النمطية
│   └── SadRegexBridge.kt
├── sensor/                  # المستشعرات
│   └── SadSensorBridge.kt
├── share/                   # المشاركة
│   └── SadShareBridge.kt
├── sms/                     # الرسائل النصية
│   └── SadSmsBridge.kt
├── speech/                  # التعرف على الكلام
│   └── SadSpeechBridge.kt
├── sqlite/                  # قواعد البيانات
│   └── SadSqliteBridge.kt
├── storage/                 # التخزين
│   └── SadStorageBridge.kt
├── theme/                   # السمات
│   └── SadThemeBridge.kt
├── time/                    # الوقت والتاريخ
│   └── SadTimeBridge.kt
├── toast/                   # رسائل Toast
│   └── SadToastBridge.kt
├── tts/                     # تحويل النص لكلام
│   └── SadTtsBridge.kt
├── ui/                      # عناصر الواجهة
│   └── SadUiBridge.kt
├── usb/                     # USB
│   └── SadUsbBridge.kt
├── validation/              # التحقق
│   └── SadValidationBridge.kt
├── vibration/               # الاهتزاز
│   └── SadVibrationBridge.kt
├── video/                   # الفيديو
│   └── SadVideoBridge.kt
├── viewmodel/               # ViewModel
│   └── SadViewModelBridge.kt
├── webview/                 # WebView
│   └── SadWebViewBridge.kt
├── wifi/                    # Wi-Fi
│   └── SadWifiBridge.kt
└── worker/                  # WorkManager
    └── SadWorkerBridge.kt
```

---

## 🚀 البدء السريع

### 1. إعداد المشروع

أضف التبعيات التالية في `build.gradle`:

```gradle
dependencies {
    // Jetpack Core
    implementation 'androidx.core:core-ktx:1.12.0'
    implementation 'androidx.appcompat:appcompat:1.6.1'
    
    // Lifecycle & ViewModel
    implementation 'androidx.lifecycle:lifecycle-runtime-ktx:2.7.0'
    implementation 'androidx.lifecycle:lifecycle-viewmodel-ktx:2.7.0'
    implementation 'androidx.lifecycle:lifecycle-process:2.7.0'
    
    // WorkManager
    implementation 'androidx.work:work-runtime-ktx:2.9.0'
    
    // Compose (اختياري)
    implementation platform('androidx.compose:compose-bom:2024.02.00')
    implementation 'androidx.compose.ui:ui'
    implementation 'androidx.compose.material3:material3'
    
    // Paging (اختياري)
    implementation 'androidx.paging:paging-runtime-ktx:3.2.1'
}
```

### 2. استخدام الجسور

```kotlin
// تهيئة الجسور
SadCoreBridge.init(applicationContext)
SadLoggingBridge.setDefaultTag("تطبيقي")

// استخدام التسجيل
SadLoggingBridge.i("تم تشغيل التطبيق بنجاح")

// التحقق من الاتصال
if (SadConnectivityBridge.isConnected(context)) {
    SadLoggingBridge.d("الجهاز متصل بالإنترنت")
}

// عرض إشعار
SadToastBridge.showArabic(context, "مرحباً بك!")
```

---

## 📚 دليل الجسور الرئيسية

### 🔧 الأساسيات (Core)

#### SadCoreBridge — النواة
```kotlin
// تهيئة النواة
SadCoreBridge.init(context)

// الحصول على معلومات التطبيق
val version = SadCoreBridge.getAppVersion()
val packageName = SadCoreBridge.getPackageName()
```

#### SadActivityBridge — الأنشطة
```kotlin
// بدء نشاط جديد
SadActivityBridge.start<SecondActivity>(context)

// بدء نشاط مع بيانات
SadActivityBridge.startWithData<DetailActivity>(context, mapOf(
    "id" to 123,
    "title" to "عنوان المقال"
))

// الانتهاء من النشاط
SadActivityBridge.finish(activity)
```

### 📱 واجهة المستخدم (UI)

#### SadComposeBridge — Jetpack Compose
```kotlin
// إنشاء نص
SadComposeBridge.createText("مرحباً بالعالم", fontSize = 18f)

// إنشاء زر
SadComposeBridge.createButton("اضغط هنا") {
    // ...
}

// إنشاء حقل إدخال
SadComposeBridge.createTextField(
    value = text,
    onValueChange = { text = it },
    label = "أدخل اسمك"
)
```

#### SadDialogBridge — مربعات الحوار
```kotlin
// رسالة تأكيد
SadDialogBridge.showConfirm(
    context,
    title = "تأكيد الحذف",
    message = "هل تريد حذف هذا العنصر؟",
    positiveButton = "نعم، احذف",
    negativeButton = "إلغاء"
) { confirmed ->
    if (confirmed) {
        // تم التأكيد
    }
}

// قائمة اختيار
SadDialogBridge.showList(
    context,
    title = "اختر البلد",
    items = listOf("السعودية", "مصر", "الإمارات")
) { index, item ->
    // تم الاختيار
}
```

#### SadThemeBridge — السمات
```kotlin
// الحصول على السمة الحالية
val theme = SadThemeBridge.getCurrentTheme()

// تغيير السمة
SadThemeBridge.setThemeByName("ليلة عربية")

// تطبيق السمة على النافذة
SadThemeBridge.applyThemeToWindow(activity)

// الألوان العربية المحددة مسبقاً
val color = SadThemeBridge.ArabicColors.ذهبي
```

### 🌐 الشبكة والاتصال

#### SadHttpBridge — طلبات HTTP
```kotlin
// GET طلب
SadHttpBridge.getAsync("https://api.example.com/data") { response ->
    if (response.isSuccess) {
        val data = response.body
    } else {
        val error = response.errorArabic
    }
}

// POST طلب مع JSON
SadHttpBridge.postJsonAsync(
    "https://api.example.com/users",
    mapOf("name" to "أحمد", "email" to "ahmed@example.com")
) { response ->
    // ...
}

// تحميل ملف
SadHttpBridge.downloadFileAsync(
    "https://example.com/file.pdf",
    outputFile
) { progress ->
    // تحديث شريط التقدم
}
```

#### SadConnectivityBridge — الاتصال
```kotlin
// التحقق من الاتصال
val connected = SadConnectivityBridge.isConnected(context)
val wifi = SadConnectivityBridge.isWifiConnected(context)
val mobile = SadConnectivityBridge.isMobileConnected(context)

// مراقبة تغييرات الاتصال
SadConnectivityBridge.registerCallback(context) { isConnected ->
    if (isConnected) {
        SadToastBridge.show(context, "تم الاتصال بالإنترنت")
    } else {
        SadToastBridge.show(context, "تم قطع الاتصال")
    }
}
```

### 📁 التخزين والبيانات

#### SadPreferencesBridge — التفضيلات
```kotlin
// حفظ قيمة
SadPreferencesBridge.putString(context, "username", "أحمد")
SadPreferencesBridge.putInt(context, "age", 25)

// استرجاع قيمة
val username = SadPreferencesBridge.getString(context, "username", "")
val age = SadPreferencesBridge.getInt(context, "age", 0)
```

#### SadSqliteBridge — قواعد البيانات
```kotlin
// إنشاء قاعدة بيانات
val db = SadSqliteBridge.open(context, "myapp.db")

// إنشاء جدول
SadSqliteBridge.execute(db, """
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY,
        name TEXT,
        email TEXT
    )
""")

// إدراج بيانات
SadSqliteBridge.insert(db, "users", mapOf(
    "name" to "أحمد",
    "email" to "ahmed@example.com"
))

// استعلام
val users = SadSqliteBridge.query(db, "SELECT * FROM users")
```

#### SadCacheBridge — التخزين المؤقت
```kotlin
// تهيئة الكاش
SadCacheBridge.initDiskCache(context)

// تخزين في الذاكرة
SadCacheBridge.createSimpleCache("myCache")
SadCacheBridge.put("myCache", "key", "value", ttlMillis = 60000)

// استرجاع
val value = SadCacheBridge.get("myCache", "key")

// التخزين الذكي (ذاكرة + قرص)
SadCacheBridge.smartPut("userData", jsonString, persistToDisk = true)
val data = SadCacheBridge.smartGet("userData")
```

### 📝 معالجة النصوص العربية

#### SadArabicStringBridge — النصوص العربية
```kotlin
// إزالة التشكيل
val clean = SadArabicStringBridge.removeTashkeel("مُحَمَّد")
// النتيجة: "محمد"

// تطبيع الألف
val normalized = SadArabicStringBridge.normalizeAlef("أحمد إبراهيم آل علي")
// النتيجة: "احمد ابراهيم ال علي"

// تحويل الأرقام
val arabicNums = SadArabicStringBridge.toArabicNumerals("123")
// النتيجة: "١٢٣"

// الجمع العربي
val plural = SadArabicStringBridge.arabicPlural(5, "كتاب", "كتابان", "كتب", "كتاباً")
// النتيجة: "كتب"
```

#### SadFormatterBridge — التنسيق
```kotlin
// تنسيق الأرقام
val formatted = SadFormatterBridge.formatNumberArabic(1234567.89)
// النتيجة: "١٬٢٣٤٬٥٦٧٫٨٩"

// تنسيق العملات
val price = SadFormatterBridge.formatSAR(199.99)
// النتيجة: "199.99 ر.س"

// الوقت النسبي
val relative = SadFormatterBridge.formatRelativeTimeArabic(timestamp)
// النتيجة: "منذ ساعتين"

// الأرقام بالكلمات
val words = SadFormatterBridge.numberToArabicWords(1985)
// النتيجة: "ألف وتسعمائة وخمسة وثمانون"
```

### 🔒 الأمان والتحقق

#### SadValidationBridge — التحقق
```kotlin
// التحقق من البريد الإلكتروني
val emailResult = SadValidationBridge.isEmail("test@example.com")
if (!emailResult.isValid) {
    showError(emailResult.errorMessage) // "البريد الإلكتروني غير صالح"
}

// التحقق من رقم الهاتف السعودي
val isValid = SadValidationBridge.isSaudiPhone("0501234567")

// قوة كلمة المرور
val strength = SadValidationBridge.getPasswordStrength("MyP@ss123")
val label = SadValidationBridge.getPasswordStrengthLabel(strength)
// النتيجة: "قوية"

// التحقق من الاسم العربي
val nameResult = SadValidationBridge.isArabicName("محمد أحمد")
```

#### SadCryptoBridge — التشفير
```kotlin
// تشفير AES
val encrypted = SadCryptoBridge.encryptAesGcm(data, key)
val decrypted = SadCryptoBridge.decryptAesGcm(encrypted, key)

// Hash
val hash = SadCryptoBridge.sha256("النص")

// توليد مفاتيح RSA
val keyPair = SadCryptoBridge.generateRsaKeyPair()

// Android Keystore
SadCryptoBridge.generateKeyInKeystore("myKey")
val encryptedData = SadCryptoBridge.encryptWithKeystore("myKey", data)
```

### 📊 Jetpack المتقدم

#### SadViewModelBridge — ViewModel
```kotlin
// الحصول على ViewModel
val viewModel = SadViewModelBridge.get(activity, "mainViewModel")

// تخزين واسترجاع البيانات
SadViewModelBridge.setString(viewModel, "username", "أحمد")
val username = SadViewModelBridge.getString(viewModel, "username")

// مراقبة التغييرات
SadViewModelBridge.observe(viewModel, "counter", activity) { value ->
    // تحديث الواجهة
}
```

#### SadLifecycleBridge — دورة الحياة
```kotlin
// تسجيل مراقب دورة الحياة
SadLifecycleBridge.observe(activity) { event ->
    when (event) {
        Lifecycle.Event.ON_RESUME -> {
            // النشاط في الواجهة
        }
        Lifecycle.Event.ON_PAUSE -> {
            // النشاط في الخلفية
        }
    }
}

// التحقق من حالة التطبيق
val isInForeground = SadLifecycleBridge.isAppInForeground()
```

#### SadWorkerBridge — المهام الخلفية
```kotlin
// إنشاء قيود
val constraints = SadWorkerBridge.createConstraints(
    requiresNetwork = true,
    requiresCharging = true
)

// إنشاء مهمة
val request = SadWorkerBridge.createOneTimeWork(
    MyWorker::class.java,
    tag = "sync_data",
    constraints = constraints,
    inputData = SadWorkerBridge.createData(
        "userId" to 123,
        "action" to "sync"
    )
)

// تنفيذ المهمة
val id = SadWorkerBridge.enqueue(request, "syncTask")

// مراقبة الحالة
val state = SadWorkerBridge.getWorkState(id)
// النتيجة: "قيد التنفيذ" أو "نجحت" أو "فشلت"
```

### 📱 الأجهزة والمستشعرات

#### SadDeviceBridge — معلومات الجهاز
```kotlin
// معلومات الجهاز
val info = SadDeviceBridge.getDeviceInfo()
val model = info["model"]
val manufacturer = info["manufacturer"]

// حالة البطارية
val battery = SadDeviceBridge.getBatteryLevel(context)
val charging = SadDeviceBridge.isCharging(context)
val healthArabic = SadDeviceBridge.getBatteryHealthArabic(context)

// الذاكرة
val memoryInfo = SadDeviceBridge.getMemoryInfo(context)
val usedMemoryMB = memoryInfo["usedMemoryMB"]
```

#### SadSensorBridge — المستشعرات
```kotlin
// مستشعر التسارع
SadSensorBridge.startAccelerometer(context) { x, y, z ->
    // حركة الجهاز
}

// مستشعر الضوء
SadSensorBridge.startLightSensor(context) { lux ->
    if (lux < 10) {
        // البيئة مظلمة
    }
}

// البوصلة
SadSensorBridge.startCompass(context) { degrees ->
    // اتجاه الجهاز
}
```

### 🎨 الوسائط والرسوميات

#### SadImageLoaderBridge — تحميل الصور
```kotlin
// تحميل صورة
SadImageLoaderBridge.loadImage(context, url, imageView)

// تحميل مع تحويلات
SadImageLoaderBridge.loadWithTransformations(
    context, url, imageView,
    resize = Pair(200, 200),
    circle = true
)

// تحميل غير متزامن
SadImageLoaderBridge.loadBitmapAsync(context, url) { bitmap ->
    // استخدام الصورة
}
```

#### SadAnimationBridge — الرسوم المتحركة
```kotlin
// تأثير التلاشي
SadAnimationBridge.fadeIn(view, duration = 500)
SadAnimationBridge.fadeOut(view, duration = 500)

// تأثير الانزلاق
SadAnimationBridge.slideInFromBottom(view)
SadAnimationBridge.slideOutToTop(view)

// تأثير التكبير
SadAnimationBridge.scaleIn(view)
SadAnimationBridge.bounce(view)
```

---

## 🧪 التسجيل والتصحيح

#### SadLoggingBridge — التسجيل
```kotlin
// إعداد التسجيل
SadLoggingBridge.setDefaultTag("تطبيقي")
SadLoggingBridge.setMinLevel(SadLoggingBridge.LogLevel.DEBUG)

// التسجيل
SadLoggingBridge.v("رسالة تفصيلية")
SadLoggingBridge.d("رسالة تصحيح")
SadLoggingBridge.i("معلومات")
SadLoggingBridge.w("تحذير")
SadLoggingBridge.e("خطأ")
SadLoggingBridge.e("خطأ", exception)

// قياس الأداء
SadLoggingBridge.startTimer("loadData")
// ... العمليات
val duration = SadLoggingBridge.stopTimer("loadData")

// تقرير الانهيار
val report = SadLoggingBridge.getCrashReport(exception)

// إحصائيات
val stats = SadLoggingBridge.getLogStatsArabic()
```

---

## 📋 قائمة الجسور الكاملة (105+)

| الرقم | الجسر | الوصف |
|-------|-------|-------|
| 1 | SadCoreBridge | النواة والتهيئة |
| 2 | SadActivityBridge | إدارة الأنشطة |
| 3 | SadAnimationBridge | الرسوم المتحركة |
| 4 | SadArabicStringBridge | معالجة النصوص العربية |
| 5 | SadAudioBridge | تشغيل الصوت |
| 6 | SadBiometricBridge | المصادقة الحيوية |
| 7 | SadBluetoothBridge | البلوتوث |
| 8 | SadCacheBridge | التخزين المؤقت |
| 9 | SadCalendarBridge | التقويم |
| 10 | SadCameraBridge | الكاميرا |
| 11 | SadClipboardBridge | الحافظة |
| 12 | SadCollectionBridge | المجموعات |
| 13 | SadComposeBridge | Jetpack Compose |
| 14 | SadConnectivityBridge | الاتصال بالإنترنت |
| 15 | SadContactsBridge | جهات الاتصال |
| 16 | SadCryptoBridge | التشفير |
| 17 | SadDeviceBridge | معلومات الجهاز |
| 18 | SadDialogBridge | مربعات الحوار |
| 19 | SadDownloadBridge | التنزيل |
| 20 | SadDragDropBridge | السحب والإفلات |
| 21 | SadFileBridge | إدارة الملفات |
| 22 | SadFirebaseBridge | Firebase |
| 23 | SadFormatterBridge | التنسيق |
| 24 | SadGestureBridge | الإيماءات |
| 25 | SadGraphicsBridge | الرسوميات |
| 26 | SadHttpBridge | طلبات HTTP |
| 27 | SadImageLoaderBridge | تحميل الصور |
| 28 | SadIntentBridge | الـ Intents |
| 29 | SadJsonBridge | JSON |
| 30 | SadKeyboardBridge | لوحة المفاتيح |
| 31 | SadLifecycleBridge | دورة الحياة |
| 32 | SadLocationBridge | الموقع الجغرافي |
| 33 | SadLoggingBridge | التسجيل |
| 34 | SadMathBridge | العمليات الرياضية |
| 35 | SadMediaBridge | الوسائط |
| 36 | SadNavigationBridge | التنقل |
| 37 | SadNotificationBridge | الإشعارات |
| 38 | SadNfcBridge | NFC |
| 39 | SadPagingBridge | Paging |
| 40 | SadPermissionBridge | الصلاحيات |
| 41 | SadPreferencesBridge | التفضيلات |
| 42 | SadQrCodeBridge | رموز QR |
| 43 | SadRecyclerBridge | RecyclerView |
| 44 | SadRegexBridge | التعبيرات النمطية |
| 45 | SadSensorBridge | المستشعرات |
| 46 | SadShareBridge | المشاركة |
| 47 | SadSmsBridge | الرسائل النصية |
| 48 | SadSpeechBridge | التعرف على الكلام |
| 49 | SadSqliteBridge | قواعد البيانات |
| 50 | SadStorageBridge | التخزين |
| 51 | SadThemeBridge | السمات |
| 52 | SadTimeBridge | الوقت والتاريخ |
| 53 | SadToastBridge | رسائل Toast |
| 54 | SadTtsBridge | تحويل النص لكلام |
| 55 | SadUiBridge | عناصر الواجهة |
| 56 | SadUsbBridge | USB |
| 57 | SadValidationBridge | التحقق من الصحة |
| 58 | SadVibrationBridge | الاهتزاز |
| 59 | SadVideoBridge | الفيديو |
| 60 | SadViewModelBridge | ViewModel |
| 61 | SadWebViewBridge | WebView |
| 62 | SadWifiBridge | Wi-Fi |
| 63 | SadWorkerBridge | WorkManager |
| ... | ... | وأكثر من 40 جسر إضافي |

---

## 🔗 الروابط المفيدة

- [توثيق لغة ص الكامل](../docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md)
- [أمثلة عملية](../examples/)
- [البرمجة الكائنية](../docs/07_البرمجة_الكائنية.md)

---

## 📄 الترخيص

هذا المشروع مرخص بموجب رخصة MIT.

---

**تم الإعداد بواسطة فريق تطوير لغة ص**
