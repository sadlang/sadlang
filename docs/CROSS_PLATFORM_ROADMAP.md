# خطة دعم المنصات المتعددة — لغة ص
# Cross-Platform Support Roadmap — Sad Language

**التاريخ:** 8 مارس 2026  
**الهدف:** الوصول إلى جاهزية 100% لـ Android, iOS, Web

---

## 📊 الوضع الحالي

### ✅ ما هو موجود بالفعل

| المكون | الحالة | الموقع |
|--------|--------|--------|
| **نظام Flutter-like** | 90% | `stdlib/ui/flutter/` — 33 ملف header |
| **Android Gradle project** | 60% | `platform/android/` |
| **Android JNI bridges** | 50% | `platform/android/bridges/` |
| **iOS Xcode project** | 50% | `platform/ios/xcode/` |
| **WASM build** | 70% | `build_wasm/` — sad.wasm يعمل |
| **WASM runtime** | 60% | `tools/wasm/sad_wasm_runtime.js` |

### ❌ ما ينقص

| المنصة | الناقص |
|--------|--------|
| **Android** | UI rendering native، APK signing، Play Store metadata |
| **iOS** | UI rendering native، Swift bridge، App Store submission |
| **Web** | DOM integration، Canvas rendering، Event handling |

---

## 🎯 خطة التطوير المرحلية

### المرحلة 1: استكمال WASM/Web (أسبوعان)

**لماذا البدء بالويب؟**
- الأسهل للاختبار
- لا يحتاج أجهزة خاصة
- يخدم كـ "playground" للتجربة

**المهام:**

| # | المهمة | الأولوية | الجهد |
|---|--------|----------|-------|
| 1.1 | إنشاء HTML wrapper | عالية | 2 ساعات |
| 1.2 | DOM integration (اطبع → console) | عالية | 4 ساعات |
| 1.3 | Input handling (اقرأ → prompt) | عالية | 4 ساعات |
| 1.4 | Canvas rendering للرسوميات | متوسطة | 8 ساعات |
| 1.5 | Event system (click, keydown) | متوسطة | 6 ساعات |
| 1.6 | Flutter widgets → DOM elements | متوسطة | 16 ساعات |
| 1.7 | Online playground UI | منخفضة | 8 ساعات |

**الملفات المطلوب إنشاؤها:**
```
tools/wasm/
├── sad_web_runtime.js     ← JavaScript runtime
├── sad_web_dom.cpp        ← DOM bridge (Emscripten)
├── sad_web_canvas.cpp     ← Canvas rendering
├── sad_web_events.cpp     ← Event handling
└── index.html             ← Demo page
```

---

### المرحلة 2: استكمال Android (3 أسابيع)

**المهام:**

| # | المهمة | الأولوية | الجهد |
|---|--------|----------|-------|
| 2.1 | تحديث NDK build | عالية | 4 ساعات |
| 2.2 | JNI bridge للمفسر | عالية | 8 ساعات |
| 2.3 | Android UI backend | عالية | 24 ساعات |
| 2.4 | Flutter widgets → Android Views | عالية | 32 ساعات |
| 2.5 | Permissions handling | متوسطة | 8 ساعات |
| 2.6 | APK signing | متوسطة | 4 ساعات |
| 2.7 | Play Store assets | منخفضة | 4 ساعات |

**البنية المقترحة:**
```
platform/android/
├── app/src/main/
│   ├── java/com/sadlang/
│   │   ├── SadInterpreter.java    ← JNI wrapper
│   │   ├── SadWidget.java         ← Base widget
│   │   ├── SadFlutterBridge.java  ← Flutter→Android mapping
│   │   └── MainActivity.java      ← Entry point
│   ├── cpp/
│   │   ├── sad_android_jni.cpp    ← JNI implementation
│   │   └── CMakeLists.txt
│   └── res/
└── bridges/  ← Camera, Location, Sensors
```

---

### المرحلة 3: استكمال iOS (3 أسابيع)

**المهام:**

| # | المهمة | الأولوية | الجهد |
|---|--------|----------|-------|
| 3.1 | Swift/ObjC bridge | عالية | 8 ساعات |
| 3.2 | iOS UI backend | عالية | 24 ساعات |
| 3.3 | Flutter widgets → UIKit | عالية | 32 ساعات |
| 3.4 | CocoaPods integration | متوسطة | 4 ساعات |
| 3.5 | App Store submission prep | منخفضة | 8 ساعات |

**البنية المقترحة:**
```
platform/ios/
├── SadFramework/
│   ├── SadInterpreter.swift       ← Swift wrapper
│   ├── SadFlutterBridge.swift     ← Flutter→UIKit mapping  
│   ├── SadWidget.swift            ← Base widget
│   └── SadInterpreter-Bridging.h  ← ObjC bridge
├── xcode/
│   └── SadNotes.xcodeproj
└── Podspec/
    └── SadLang.podspec
```

---

## 🔧 خطوات البدء الفورية

### الخيار A: البدء بالويب (موصى به)

```bash
# 1. إنشاء HTML wrapper
# 2. اختبار sad.wasm الموجود
# 3. إضافة DOM integration
```

### الخيار B: البدء بـ Android

```bash
# 1. تحديث CMakeLists.txt للـ NDK
# 2. إنشاء JNI bridge
# 3. اختبار على emulator
```

### الخيار C: البدء بـ iOS

```bash
# 1. إنشاء Swift bridge
# 2. تحديث Xcode project
# 3. اختبار على simulator
```

---

## 📈 الجدول الزمني المقترح

```
الأسبوع 1-2:   WASM/Web (اللبنة الأساسية)
الأسبوع 3-5:   Android (النسخة التجريبية)
الأسبوع 6-8:   iOS (النسخة التجريبية)
الأسبوع 9-10:  اختبار وإصلاح bugs
الأسبوع 11-12: توثيق ونشر
```

**الإجمالي:** ~3 أشهر للوصول إلى 100%

---

## 🎯 من أين نبدأ؟

### الخيار الأسرع للنتائج: **WASM/Web**
- يمكن رؤية النتائج في المتصفح فوراً
- لا يحتاج إعداد معقد
- يخدم كـ demo عبر الإنترنت

### الخيار الأكثر تأثيراً: **Android**
- 70% من مستخدمي الهواتف
- Google Play سهل النشر
- Emulator متاح على Windows/Linux

### الخيار الأصعب: **iOS**
- يحتاج macOS للبناء
- Apple Developer account ($99/سنة)
- مراجعة App Store صارمة

---

## ⚡ نقطة البداية المقترحة

بناءً على البنية الحالية، أقترح البدء بـ:

### 1. إنشاء Web Playground (فوري)

ملف HTML بسيط يستخدم `sad.wasm` الموجود:

```html
<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>ملعب لغة ص</title>
</head>
<body>
    <textarea id="code">اطبع("مرحباً بالعالم")</textarea>
    <button onclick="runCode()">تنفيذ</button>
    <pre id="output"></pre>
    <script src="sad.js"></script>
    <script>
        async function runCode() {
            const module = await SadWasm();
            const result = module.ccall('sad_execute', 'string', ['string', 'string'], 
                [document.getElementById('code').value, '']);
            document.getElementById('output').textContent = JSON.parse(result).output;
        }
    </script>
</body>
</html>
```

---

**هل تريد أن أبدأ بتنفيذ أي من هذه الخطوات؟**
