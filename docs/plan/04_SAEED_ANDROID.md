# سعيد — منصة أندرويد (Android Platform)

> الدور: مهندس منصة أندرويد
> المسؤولية: Android Runtime, JNI Bridge, UI Framework, APK Builder, Play Store
> الملف المرجعي: `docs/plan/01_STRATEGIC_PLAN.md`

---

## الملكية الحصرية

```
platform/android/                ← كل كود أندرويد
├── runtime/
│   ├── jni_bridge.cpp          ← جسر JNI بين C++ و Java/Kotlin
│   ├── android_runtime.cpp     ← runtime خاص بأندرويد
│   ├── android_lifecycle.cpp   ← Activity lifecycle management
│   └── permissions.cpp         ← إدارة صلاحيات Android
├── ui/
│   ├── android_widgets.cpp     ← Material Design widgets
│   ├── android_layout.cpp      ← Layout system (XML → Sad)
│   ├── compose_bridge.cpp      ← Jetpack Compose integration
│   └── navigation.cpp          ← Navigation component
├── services/
│   ├── notifications.cpp       ← Push notifications
│   ├── background_service.cpp  ← خدمات خلفية
│   └── broadcast_receiver.cpp  ← مستقبل البث
├── sensors/
│   ├── camera.cpp              ← Camera API
│   ├── gps.cpp                 ← الموقع الجغرافي
│   ├── accelerometer.cpp       ← مستشعر التسارع
│   └── biometric.cpp           ← بصمة / وجه
└── build/
    ├── gradle_generator.cpp    ← توليد ملفات Gradle
    ├── manifest_generator.cpp  ← توليد AndroidManifest.xml
    ├── apk_builder.cpp         ← بناء APK
    └── signing.cpp             ← توقيع APK

stdlib/platform/android/         ← مكتبات منصة أندرويد
├── io_android.cpp              ← I/O خاص بأندرويد
├── network_android.cpp         ← OkHttp/Volley bridge
├── storage_android.cpp         ← SharedPreferences + Room
├── graphics_android.cpp        ← Canvas + SurfaceView
└── media_android.cpp           ← MediaPlayer + ExoPlayer

distribution/android/            ← حزم التوزيع
├── play_store/                 ← Play Store metadata
├── templates/                  ← قوالب تطبيقات
└── signing_keys/               ← إدارة المفاتيح

build_test_ui/                   ← اختبارات UI أندرويد
```

### القاعدة الذهبية
> سعيد لا يعدّل: `compiler_new/`، `tools/`، `graphics/` (desktop)، `platform/ios/`
> يستخدم `shared/` (AST, types) للقراءة فقط

---

## 2026 — التفصيل الشهري

---

### الربع الأول (مارس — مايو 2026): "بناء الأساس"

#### مارس 2026

| الأسبوع | المهمة | الملفات | المُخرج | معيار القبول |
|---------|--------|---------|---------|-------------|
| 1 | إعداد بيئة NDK + CMake Android | `platform/android/CMakeLists.txt` | مشروع Android NDK يبني | libsad_android.so يُبنى |
| 2 | JNI Bridge: أساسي | `platform/android/runtime/jni_bridge.cpp` | استدعاء دوال ص من Java | `SadInterpreter.execute("اطبع(42)")` يعمل |
| 3 | JNI Bridge: أنواع Value ↔ Java | `platform/android/runtime/jni_types.cpp` | تحويل أنواع ص ↔ Java (int, string, array) | جميع الأنواع الـ 8 تتحول |
| 4 | Android Activity أولي | `platform/android/runtime/android_lifecycle.cpp` | Activity يبدأ interpreter ويطبع | "مرحباً من لغة ص" على Android |

**المُخرج الشهري**: مكتبة .so مع JNI bridge — برنامج ص يعمل على أندرويد

#### أبريل 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Android Runtime: إدارة الحياة | `platform/android/runtime/android_runtime.cpp` | onCreate/onPause/onResume/onDestroy |
| 2 | Android Runtime: إدارة الذاكرة | `platform/android/runtime/android_memory.cpp` | GC bridge مع Android |
| 3 | JNI Bridge: callbacks ثنائية | `platform/android/runtime/jni_callbacks.cpp` | Java → Sad و Sad → Java |
| 4 | واجهة أساسية: نص على الشاشة | `platform/android/ui/android_text.cpp` | عرض نص عربي على الشاشة |

**المُخرج الشهري**: Android Runtime مستقر مع lifecycle + callbacks

#### مايو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Android Runtime: threading | `platform/android/runtime/android_threads.cpp` | Main thread + worker threads |
| 2 | Sad builtins لأندرويد | `platform/android/runtime/android_builtins.cpp` | `اطبع_اندرويد()`, `عرض_رسالة()`, `اقرأ_إدخال()` |
| 3 | APK Builder: تحسين sad-apk | `platform/android/build/apk_builder.cpp` | `.ص → APK` أوتوماتيكي |
| 4 | أول تطبيق كامل: "مرحباً" | `examples/apps/android/hello_android.ص` | APK يُثبّت ويعمل على جهاز حقيقي |

**المُخرج الشهري**: APK أول يعمل على جهاز Android حقيقي

**معايير القبول Q1:**
- [ ] JNI Bridge: أنواع ص ↔ Java تعمل (8 أنواع)
- [ ] Android Activity lifecycle يعمل
- [ ] APK يُثبّت على جهاز حقيقي
- [ ] "مرحباً من لغة ص" يظهر على أندرويد

---

### الربع الثاني (يونيو — أغسطس 2026): "واجهات المستخدم"

#### يونيو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | UI Framework: Widget base | `platform/android/ui/widget_base.cpp` | فئة أساسية لـ widgets |
| 2 | UI: نص + زر + صورة | `platform/android/ui/basic_widgets.cpp` | 3 widgets أساسية |
| 3 | UI: حقل إدخال + قائمة | `platform/android/ui/input_widgets.cpp` | TextInput + ListView |
| 4 | UI: Layout (Column/Row) | `platform/android/ui/layout.cpp` | تخطيط عمودي/أفقي مع RTL |

**المُخرج الشهري**: 6 widgets أساسية مع RTL layout

#### يوليو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | UI: Material Design theme | `platform/android/ui/material_theme.cpp` | ألوان + خطوط + تباعد Material |
| 2 | UI: Navigation (شاشات متعددة) | `platform/android/ui/navigation.cpp` | انتقال بين شاشات |
| 3 | APK Builder v2: Gradle integration | `platform/android/build/gradle_generator.cpp` | توليد Gradle تلقائي |
| 4 | APK Builder: توقيع + minSdk/targetSdk | `platform/android/build/signing.cpp` | APK موقّع للتوزيع |

**المُخرج الشهري**: Navigation + Material Design + APK موقّع

#### أغسطس 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Play Store metadata | `distribution/android/play_store/` | وصف + screenshots + policy |
| 2 | تطبيق نموذجي: قائمة مهام | `examples/apps/android/todo_android.ص` | تطبيق TODO كامل |
| 3 | stdlib/android: SharedPreferences | `stdlib/platform/android/storage_android.cpp` | حفظ/تحميل بيانات محلية |
| 4 | Testing: 5 تطبيقات أندرويد | `platform/android/tests/` | 5 تطبيقات بدون crashes |

**المُخرج الشهري**: تطبيقات أندرويد جاهزة للنشر

**معايير القبول Q2:**
- [ ] 10+ widgets مع Material Design
- [ ] Navigation بين شاشات
- [ ] APK موقّع جاهز للتوزيع
- [ ] SharedPreferences API
- [ ] 5 تطبيقات بدون crashes

---

### الربع الثالث (سبتمبر — نوفمبر 2026): "ميزات متقدمة"

#### سبتمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Sensors: كاميرا | `platform/android/sensors/camera.cpp` | التقاط صور + فيديو |
| 2 | Sensors: GPS | `platform/android/sensors/gps.cpp` | موقع + خريطة |
| 3 | Sensors: مستشعر تسارع + بوصلة | `platform/android/sensors/accelerometer.cpp` | بيانات المستشعرات |
| 4 | Permissions: نظام صلاحيات | `platform/android/runtime/permissions.cpp` | طلب + فحص صلاحيات |

#### أكتوبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Notifications: محلية | `platform/android/services/notifications.cpp` | إشعارات محلية |
| 2 | Background Services | `platform/android/services/background_service.cpp` | خدمات خلفية |
| 3 | Network: OkHttp bridge | `stdlib/platform/android/network_android.cpp` | HTTP requests عبر OkHttp |
| 4 | تطبيق نموذجي: طقس + GPS | `examples/apps/android/weather_android.ص` | تطبيق طقس بالموقع |

#### نوفمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Media: تشغيل صوت/فيديو | `stdlib/platform/android/media_android.cpp` | MediaPlayer API |
| 2 | Biometric: بصمة/وجه | `platform/android/sensors/biometric.cpp` | BiometricPrompt API |
| 3 | 3 تطبيقات كاملة | `examples/apps/android/` | حاسبة + مهام + طقس |
| 4 | اختبار شامل + performance | `platform/android/tests/` | 10 تطبيقات بدون crashes |

**معايير القبول Q3:**
- [ ] Camera + GPS + Sensors APIs
- [ ] Notifications + Background services
- [ ] BiometricPrompt
- [ ] HTTP networking عبر OkHttp
- [ ] 3 تطبيقات كاملة
- [ ] 10 تطبيقات بدون crashes

---

### الربع الرابع (ديسمبر 2026): "v1.0 Android"

#### ديسمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Play Store: أول تطبيق منشور | `distribution/android/play_store/` | تطبيق نموذجي على Play Store |
| 2 | Compatibility testing: API 24-34 | `platform/android/tests/` | يعمل على Android 7.0+ |
| 3 | v1.0 Android: bug fixes | `platform/android/` | 0 أخطاء P0 |
| 4 | v1.0 release | `platform/android/` | ✅ Android v1.0 مستقر |

**معايير القبول Q4:**
- [ ] تطبيق على Play Store
- [ ] يعمل على Android 7.0+ (API 24+)
- [ ] 20+ widget + sensors + services
- [ ] 0 أخطاء P0

---

## 2027 — التفصيل الشهري

---

### الربع الأول (يناير — مارس 2027): "منصات إضافية"

#### يناير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Wear OS: watchface + complications | `platform/android/wearos/` | تطبيق ساعة بلغة ص |
| 3-4 | Android Auto: Dashboard UI | `platform/android/auto/` | واجهة قيادة بسيطة |

#### فبراير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Android Auto: navigation | `platform/android/auto/navigation.cpp` | مسارات + خرائط |
| 3-4 | Health Connect: بيانات صحية | `platform/android/health/` | خطوات + نبض + نوم |

#### مارس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Material You: Dynamic colors | `platform/android/ui/material_you.cpp` | ألوان ديناميكية من خلفية |
| 3-4 | v1.3 Android release | `platform/android/` | Wear OS + Auto + Material You |

**معايير القبول Q1 2027:**
- [ ] Wear OS تطبيق عامل
- [ ] Android Auto واجهة عاملة
- [ ] Health Connect API
- [ ] Material You dynamic colors

---

### الربع الثاني (أبريل — يونيو 2027): "Material 3 + Performance"

#### أبريل 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Material 3: كل العناصر | `platform/android/ui/material3/` | 30+ Material 3 widget |
| 3-4 | Material 3: animations | `platform/android/ui/material3/animations.cpp` | Shared element + transitions |

#### مايو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Advanced widgets: RecyclerView API | `platform/android/ui/recycler.cpp` | قوائم كبيرة (10,000+ عنصر) |
| 3-4 | Advanced widgets: ViewPager + TabLayout | `platform/android/ui/pager.cpp` | صفحات + tabs |

#### يونيو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Performance: ProGuard + R8 | `platform/android/build/optimization.cpp` | APK حجم < 5 MB |
| 3-4 | Performance: startup time | `platform/android/runtime/preload.cpp` | Cold start < 2 ثانية |

**معايير القبول Q2 2027:**
- [ ] Material 3: 30+ widget
- [ ] RecyclerView: 10,000+ items بدون lag
- [ ] APK < 5 MB
- [ ] Cold start < 2 ثانية

---

### الربع الثالث (يوليو — سبتمبر 2027): "تطبيقات إنتاجية"

#### يوليو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Android TV: 10-foot UI | `platform/android/tv/` | واجهة TV مع D-pad navigation |
| 3-4 | Android Tablets: adaptive layout | `platform/android/ui/adaptive.cpp` | تخطيط يتكيف مع حجم الشاشة |

#### أغسطس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | 5 تطبيقات إنتاجية | `examples/apps/android/` | 5 تطبيقات متنوعة |
| 3-4 | Firebase integration | `platform/android/firebase/` | Analytics + Crashlytics |

#### سبتمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | 10 تطبيقات على Play Store | `distribution/android/play_store/` | 10 تطبيقات منشورة |
| 3-4 | Performance + stability audit | `platform/android/tests/` | ANR rate < 0.1% |

**معايير القبول Q3 2027:**
- [ ] Android TV + Tablet دعم
- [ ] 10 تطبيقات على Play Store
- [ ] Firebase Analytics + Crashlytics
- [ ] ANR rate < 0.1%

---

### الربع الرابع (أكتوبر — ديسمبر 2027): "v2.0 Android"

#### أكتوبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Performance: memory optimization | `platform/android/runtime/` | Memory leaks = 0 |
| 3-4 | Compatibility: API 24-35 | `platform/android/tests/` | كل API levels |

#### نوفمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Production quality: edge cases | `platform/android/` | 0 أخطاء P0/P1 |
| 3-4 | v2.0 preparation + smoke tests | `platform/android/tests/` | Full regression suite |

#### ديسمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | v2.0 release candidate | `platform/android/` | RC جاهز |
| 3-4 | 🎉 v2.0 Android release | `platform/android/` | إطلاق رسمي |

**معايير القبول Q4 2027:**
- [ ] Memory leaks = 0
- [ ] API 24-35 compatibility
- [ ] 10+ تطبيقات على Play Store
- [ ] ANR < 0.1%, crash-free rate > 99.5%

---

## الأدوات والتقنيات

| الأداة | الغرض | الإصدار |
|--------|--------|---------|
| Android NDK | C++ compilation | r26+ |
| Android SDK | Java/Kotlin APIs | API 34+ |
| Gradle | Build system | 8.x |
| JNI | C++ ↔ Java bridge | standard |
| OkHttp | HTTP networking | 4.x |
| ExoPlayer | Media playback | 2.x |
| Firebase | Analytics + Crashes | latest |

---

## ملخص المُخرجات

```
2026:
  ✅ JNI Bridge كامل (8 أنواع ص ↔ Java)
  ✅ Android Runtime مع lifecycle
  ✅ 20+ Material Design widgets
  ✅ Camera + GPS + Sensors APIs
  ✅ Notifications + Background services
  ✅ APK Builder + Play Store
  ✅ 3 تطبيقات كاملة + 1 على Play Store

2027:
  ✅ Wear OS + Android Auto + TV + Tablet
  ✅ Material 3: 30+ widget
  ✅ Firebase integration
  ✅ 10 تطبيقات على Play Store
  ✅ APK < 5 MB, Cold start < 2s
  ✅ ANR < 0.1%, crash-free > 99.5%
  ✅ v2.0 Android stable
```
