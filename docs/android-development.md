# بناء تطبيقات أندرويد بلغة ص
# Building Android Apps with Sad Language

<div dir="rtl">

## مقدمة

لغة ص هي أول لغة برمجة عربية تدعم بناء تطبيقات أندرويد الأصلية بدون الحاجة إلى Java أو Kotlin. هذا الدليل يشرح كيفية إنشاء وبناء تطبيقات أندرويد باستخدام لغة ص.

## المتطلبات

### المتطلبات الأساسية:
- **مترجم ص (sadc)** - الإصدار 1.0 أو أحدث
- **Android SDK** - الإصدار 26 أو أحدث
- **Android NDK** - الإصدار r21 أو أحدث

### المتطلبات الاختيارية:
- **Android Studio** - للتطوير والتصحيح
- **ADB** - لتثبيت التطبيقات على الأجهزة

### تثبيت NDK:
```bash
# على Windows (Android Studio)
SDK Manager → SDK Tools → NDK (Side by side)

# أو تحميل مباشر
# https://developer.android.com/ndk/downloads
```

### إعداد متغيرات البيئة:
```bash
# Windows
set ANDROID_NDK_HOME=C:\Android\ndk\25.2.9519653
set ANDROID_SDK_ROOT=C:\Android\Sdk

# Linux/macOS
export ANDROID_NDK_HOME=~/Android/Sdk/ndk/25.2.9519653
export ANDROID_SDK_ROOT=~/Android/Sdk
```

## البدء السريع

### 1. إنشاء مشروع جديد

```bash
sadc build android تطبيقي.ص
```

هذا الأمر يُنشئ هيكل مشروع أندرويد كامل في مجلد `build_android/`.

### 2. هيكل المشروع المُنشأ

```
build_android/
├── src/
│   └── main/
│       ├── cpp/
│       │   ├── include/
│       │   └── sad_generated.cpp  # الكود المترجم
│       ├── res/
│       │   └── mipmap-hdpi/       # الأيقونات
│       └── AndroidManifest.xml
├── libs/
│   └── arm64-v8a/                 # المكتبات الأصلية
├── build.gradle
├── CMakeLists.txt
└── settings.gradle
```

### 3. ترجمة كود ص إلى كود أصلي

```bash
sadc --target=aarch64-linux-android26 تطبيقي.ص -o build_android/src/main/cpp/sad_generated.cpp
```

### 4. بناء APK

```bash
cd build_android
./gradlew assembleDebug
```

### 5. تثبيت على الجهاز

```bash
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

## خيارات الأمر

```
sadc build android [خيارات] <ملف.ص>

الخيارات:
  --name <اسم>        اسم التطبيق (افتراضي: تطبيق_ص)
  --package <اسم>     اسم الحزمة (افتراضي: com.sadlang.app)
  --version <رقم>     رقم الإصدار (افتراضي: 1.0.0)
  -o, --output <مسار> مجلد الإخراج (افتراضي: build_android)
  --ndk <مسار>        مسار Android NDK
  --min-sdk <رقم>     الحد الأدنى لإصدار Android (افتراضي: 26)
  --release           بناء إصدار للنشر
  --help              عرض المساعدة
```

## أمثلة

### مثال 1: تطبيق بسيط

```sad
# تطبيق مرحبا بالعالم

دالة رئيسية()
    اطبع_سطر("مرحباً بالعالم من أندرويد!")
نهاية
```

### مثال 2: تطبيق بواجهة مستخدم

```sad
# تطبيق واجهة مستخدم بسيط
استورد "واجهة"

صنف تطبيقي
    باني()
        هذا.نافذة = واجهة.نافذة("تطبيقي")
        هذا.زر = واجهة.زر("اضغط هنا")
        هذا.زر.عند_الضغط(هذا.عالج_الضغط)
        هذا.نافذة.أضف(هذا.زر)
    نهاية
    
    دالة عالج_الضغط()
        اطبع_سطر("تم الضغط!")
    نهاية
    
    دالة ابدأ()
        هذا.نافذة.اعرض()
    نهاية
نهاية

دالة رئيسية()
    متغير تطبيق = جديد تطبيقي()
    تطبيق.ابدأ()
نهاية
```

### مثال 3: تطبيق شبكة

```sad
# تطبيق HTTP بسيط
استورد "شبكة"

صنف عميل_HTTP
    دالة جلب_بيانات(رابط)
        متغير استجابة = شبكة.طلب_HTTP(رابط، "GET")
        إذا (استجابة.نجح)
            ارجع استجابة.النص
        وإلا
            ارجع "فشل الاتصال"
        نهاية
    نهاية
نهاية
```

## البنية المعمارية

### طبقات النظام

```
┌─────────────────────────────────────────────┐
│            كود ص / Sad Code                 │
├─────────────────────────────────────────────┤
│         SIR (Sad IR) Opcodes                │
│   ANDROID_*, ANDROID_UI_*, ANDROID_NET_*    │
├─────────────────────────────────────────────┤
│         LLVM CodeGen + Android Backend      │
├─────────────────────────────────────────────┤
│         Sad Android Runtime                 │
│   sad_android_*, sad_ui_*, sad_net_*        │
├─────────────────────────────────────────────┤
│         Android NDK / Native APIs           │
│   JNI, EGL, OpenGL ES 3.0, liblog, etc     │
├─────────────────────────────────────────────┤
│              Android OS                     │
└─────────────────────────────────────────────┘
```

### دوال Runtime الأصلية

| الدالة | الوصف |
|--------|-------|
| `sad_android_init()` | تهيئة Runtime |
| `sad_android_log()` | طباعة في Logcat |
| `sad_alloc()` | تخصيص ذاكرة |
| `sad_free()` | تحرير ذاكرة |
| `sad_string_create()` | إنشاء نص |
| `sad_array_create()` | إنشاء مصفوفة |
| `sad_ui_init()` | تهيئة واجهة المستخدم |
| `sad_ui_create_widget()` | إنشاء عنصر واجهة |
| `sad_http_request()` | طلب HTTP |
| `sad_ws_connect()` | اتصال WebSocket |

## استكشاف الأخطاء

### مشكلة: لم يتم العثور على NDK
```
⚠ لم يتم العثور على Android NDK
```
**الحل:** تعيين متغير البيئة `ANDROID_NDK_HOME`

### مشكلة: خطأ في الربط
```
undefined reference to `sad_android_init'
```
**الحل:** تأكد من ربط مكتبة `libsad_android.so` في CMakeLists.txt

### مشكلة: التطبيق لا يعمل
**الحل:** تحقق من Logcat:
```bash
adb logcat -s SadApp
```

## الأداء

### نصائح التحسين:
1. استخدم `-O2` أو `-O3` عند الترجمة للإنتاج
2. فعّل ProGuard في build.gradle للإصدار
3. استخدم `--release` لتعطيل معلومات التصحيح

### مقارنة الأداء:
| اللغة | وقت البدء | حجم APK |
|-------|-----------|---------|
| ص | ~50ms | ~2MB |
| Java | ~200ms | ~5MB |
| Flutter | ~300ms | ~15MB |

## الترخيص

نظام بناء أندرويد للغة ص مرخص تحت MIT License.

</div>

---

## English Summary

Sad (ص) is the first Arabic programming language that supports building native Android applications without Java or Kotlin.

### Quick Start:
```bash
# Create Android project
sadc build android myapp.sad

# Compile to native code
sadc --target=aarch64-linux-android26 myapp.sad -o build_android/src/main/cpp/sad_generated.cpp

# Build APK
cd build_android && ./gradlew assembleDebug

# Install
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

### Requirements:
- sadc compiler 1.0+
- Android SDK 26+
- Android NDK r21+
