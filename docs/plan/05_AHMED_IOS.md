# أحمد — منصة آيفون / iOS (iPhone & iPad Platform)

> الدور: مهندس منصة iOS
> المسؤولية: iOS Runtime, Swift Bridge, UI Framework, IPA Builder, App Store
> الملف المرجعي: `docs/plan/01_STRATEGIC_PLAN.md`

---

## الملكية الحصرية

```
platform/ios/                    ← كل كود iOS
├── runtime/
│   ├── swift_bridge.swift       ← جسر Swift ↔ C++ ↔ Sad
│   ├── objc_bridge.mm          ← Objective-C bridge (legacy APIs)
│   ├── ios_runtime.cpp         ← runtime خاص بـ iOS
│   ├── ios_lifecycle.swift     ← UIApplication lifecycle
│   └── memory_bridge.cpp       ← ARC ↔ Sad memory management
├── ui/
│   ├── swiftui_bridge.swift    ← SwiftUI integration
│   ├── uikit_widgets.swift     ← UIKit widgets bridge
│   ├── ios_layout.swift        ← Auto Layout integration
│   ├── ios_navigation.swift    ← NavigationStack
│   └── ios_theme.swift         ← iOS design system
├── services/
│   ├── push_notifications.swift ← APNs
│   ├── background_tasks.swift   ← BGTaskScheduler
│   ├── core_data.swift         ← CoreData bridge
│   └── keychain.swift          ← Keychain access
├── hardware/
│   ├── camera.swift            ← AVFoundation camera
│   ├── location.swift          ← CoreLocation
│   ├── arkit.swift             ← ARKit integration
│   ├── healthkit.swift         ← HealthKit
│   ├── biometric.swift         ← Face ID / Touch ID
│   └── haptics.swift           ← Haptic Engine
├── build/
│   ├── xcode_generator.cpp     ← توليد مشروع Xcode
│   ├── ipa_builder.cpp         ← بناء IPA
│   ├── provisioning.cpp        ← إدارة Provisioning Profiles
│   └── codesign.cpp            ← Code Signing
└── tests/
    ├── xctest_bridge.swift     ← XCTest integration
    └── ui_tests/               ← UI automation tests

stdlib/platform/ios/             ← مكتبات منصة iOS
├── io_ios.swift                ← File I/O عبر Foundation
├── network_ios.swift           ← URLSession bridge
├── storage_ios.swift           ← UserDefaults + CoreData
├── graphics_ios.swift          ← Core Graphics + Metal
└── media_ios.swift             ← AVFoundation

distribution/ios/                ← توزيع iOS
├── app_store/                  ← App Store metadata
├── testflight/                 ← TestFlight automation
└── certificates/               ← إدارة الشهادات

tools/ios-builder/               ← أداة بناء iOS CLI
├── sad-ipa.cpp                 ← `.ص → IPA` أداة سطر أوامر
└── README.md
```

### القاعدة الذهبية
> أحمد لا يعدّل: `compiler_new/`، `tools/`، `graphics/` (desktop)، `platform/android/`
> يستخدم `shared/` (AST, types) للقراءة فقط

### ملاحظة مهمة: بيئة التطوير
> تطوير iOS يتطلب macOS + Xcode. أحمد يحتاج:
> - Mac (فعلي أو cloud: MacStadium / GitHub Actions macOS runners)
> - Xcode 15+ مع iOS 17 SDK
> - Apple Developer Account ($99/سنة)

---

## 2026 — التفصيل الشهري

---

### الربع الأول (مارس — مايو 2026): "بناء الجسر"

#### مارس 2026

| الأسبوع | المهمة | الملفات | المُخرج | معيار القبول |
|---------|--------|---------|---------|-------------|
| 1 | إعداد Xcode project template | `platform/ios/SadApp.xcodeproj/` | مشروع Xcode قالب يبني | `xcodebuild` ينجح |
| 2 | Swift-C++ Interop: أساسي | `platform/ios/runtime/swift_bridge.swift` | استدعاء دوال C++ من Swift | `SadInterpreter.run("اطبع(42)")` يعمل |
| 3 | Swift Bridge: أنواع Value ↔ Swift | `platform/ios/runtime/swift_types.swift` | تحويل أنواع ص ↔ Swift types | Int, String, Array, Dict تتحول |
| 4 | iOS App أولي: Hello World | `platform/ios/runtime/ios_lifecycle.swift` | UIApplication + ViewController | "مرحباً من لغة ص" على iOS Simulator |

**المُخرج الشهري**: مشروع Xcode مع Swift bridge — برنامج ص يعمل على iOS Simulator

#### أبريل 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Xcode Project Generator | `platform/ios/build/xcode_generator.cpp` | `.ص → .xcodeproj` تلقائي |
| 2 | Xcode Generator: resources + assets | `platform/ios/build/assets_generator.cpp` | Assets.xcassets + Info.plist |
| 3 | iOS Runtime: lifecycle كامل | `platform/ios/runtime/ios_runtime.cpp` | willEnterForeground/didEnterBackground |
| 4 | iOS Runtime: error handling | `platform/ios/runtime/ios_errors.swift` | NSException ↔ أخطاء ص |

**المُخرج الشهري**: Xcode Project Generator — `.ص → مشروع Xcode` تلقائي

#### مايو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | iOS Runtime: threading | `platform/ios/runtime/ios_threads.swift` | Main thread + DispatchQueue |
| 2 | iOS Runtime: memory (ARC bridge) | `platform/ios/runtime/memory_bridge.cpp` | ARC ↔ Sad reference counting |
| 3 | IPA Builder أساسي | `platform/ios/build/ipa_builder.cpp` | `.ص → .ipa` (للـ Simulator) |
| 4 | أول تطبيق Simulator كامل | `examples/apps/ios/hello_ios.ص` | تطبيق يعمل على iPhone Simulator |

**المُخرج الشهري**: IPA Builder + تطبيق يعمل على Simulator

**معايير القبول Q1:**
- [ ] Swift Bridge: 8 أنواع ص ↔ Swift
- [ ] Xcode Project Generator يعمل
- [ ] iOS lifecycle كامل
- [ ] تطبيق يعمل على iPhone Simulator

---

### الربع الثاني (يونيو — أغسطس 2026): "واجهات المستخدم"

#### يونيو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | SwiftUI Bridge: أساسي | `platform/ios/ui/swiftui_bridge.swift` | Text, Image, Button من ص |
| 2 | SwiftUI: VStack/HStack/ZStack | `platform/ios/ui/ios_layout.swift` | تخطيط عربي RTL |
| 3 | SwiftUI: NavigationStack | `platform/ios/ui/ios_navigation.swift` | شاشات متعددة |
| 4 | SwiftUI: List + Form | `platform/ios/ui/swiftui_list.swift` | قوائم + نماذج |

**المُخرج الشهري**: SwiftUI integration— 8 عناصر UI أساسية

#### يوليو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | SwiftUI: Theme + Colors | `platform/ios/ui/ios_theme.swift` | نظام ألوان iOS |
| 2 | SwiftUI: TabView + ToolBar | `platform/ios/ui/swiftui_tabs.swift` | 5 عناصر إضافية |
| 3 | IPA Builder: device signing | `platform/ios/build/codesign.cpp` | توقيع لجهاز حقيقي |
| 4 | أول اختبار على جهاز حقيقي | `platform/ios/tests/` | تطبيق يعمل على iPhone فعلي |

**المُخرج الشهري**: IPA موقّع يعمل على iPhone حقيقي

#### أغسطس 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | SwiftUI: Sheet + Alert + ActionSheet | `platform/ios/ui/swiftui_modals.swift` | Modals + dialogs |
| 2 | SwiftUI: AsyncImage + ProgressView | `platform/ios/ui/swiftui_async.swift` | عناصر async |
| 3 | تطبيق نموذجي: قائمة مهام | `examples/apps/ios/todo_ios.ص` | TODO app كامل |
| 4 | Simulator testing: 5 تطبيقات | `platform/ios/tests/` | 5 تطبيقات بدون crashes |

**المُخرج الشهري**: 15+ عنصر SwiftUI + تطبيق TODO

**معايير القبول Q2:**
- [ ] SwiftUI: 15+ عنصر UI
- [ ] Navigation + Modals + Lists
- [ ] IPA موقّع على جهاز حقيقي
- [ ] RTL layout يعمل بشكل صحيح
- [ ] 5 تطبيقات بدون crashes

---

### الربع الثالث (سبتمبر — نوفمبر 2026): "أجهزة + ميزات متقدمة"

#### سبتمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | CoreML: Machine Learning | `platform/ios/hardware/coreml.swift` | تشغيل ML models من ص |
| 2 | ARKit: واقع معزز أساسي | `platform/ios/hardware/arkit.swift` | عرض 3D objects في AR |
| 3 | Camera: AVFoundation | `platform/ios/hardware/camera.swift` | التقاط صور + فيديو |
| 4 | Location: CoreLocation | `platform/ios/hardware/location.swift` | GPS + Geocoding |

#### أكتوبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | HealthKit: بيانات صحية | `platform/ios/hardware/healthkit.swift` | خطوات + نبض |
| 2 | Biometric: Face ID/Touch ID | `platform/ios/hardware/biometric.swift` | LocalAuthentication |
| 3 | Push Notifications: APNs | `platform/ios/services/push_notifications.swift` | إشعارات push |
| 4 | Background Tasks | `platform/ios/services/background_tasks.swift` | BGTaskScheduler |

#### نوفمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | CoreData: قاعدة بيانات محلية | `platform/ios/services/core_data.swift` | CRUD operations |
| 2 | Keychain: تخزين آمن | `platform/ios/services/keychain.swift` | حفظ كلمات سر/tokens |
| 3 | تطبيق كامل: طقس AR | `examples/apps/ios/weather_ar_ios.ص` | تطبيق طقس + AR |
| 4 | App Store: إعداد وصف + screenshots | `distribution/ios/app_store/` | Metadata جاهز |

**معايير القبول Q3:**
- [ ] CoreML + ARKit + Camera + Location
- [ ] HealthKit + Face ID + Push Notifications
- [ ] CoreData + Keychain
- [ ] Background Tasks
- [ ] 3 تطبيقات كاملة

---

### الربع الرابع (ديسمبر 2026): "v1.0 iOS"

#### ديسمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | TestFlight: أول بناء | `distribution/ios/testflight/` | بناء TestFlight عام |
| 2 | App Store: أول رفع | `distribution/ios/app_store/` | تطبيق على App Store |
| 3 | Compatibility: iOS 16-18 | `platform/ios/tests/` | يعمل على 3 إصدارات |
| 4 | v1.0 iOS release | `platform/ios/` | ✅ iOS v1.0 مستقر |

**معايير القبول Q4:**
- [ ] تطبيق على App Store أو TestFlight
- [ ] يعمل على iOS 16+
- [ ] 20+ عنصر SwiftUI
- [ ] Hardware APIs (camera, GPS, biometric, AR)
- [ ] 0 أخطاء P0

---

## 2027 — التفصيل الشهري

---

### الربع الأول (يناير — مارس 2027): "Apple Ecosystem"

#### يناير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | watchOS: Apple Watch app | `platform/ios/watchos/` | تطبيق ساعة بلغة ص |
| 3-4 | watchOS: complications + glances | `platform/ios/watchos/complications.swift` | Complications على watch face |

#### فبراير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | CarPlay: واجهة سيارة | `platform/ios/carplay/` | CarPlay dashboard |
| 3-4 | CarPlay: navigation | `platform/ios/carplay/navigation.swift` | خرائط + ملاحة |

#### مارس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | HealthKit متقدم | `platform/ios/hardware/healthkit_advanced.swift` | Workouts + sleep + nutrition |
| 3-4 | v1.3 iOS release | `platform/ios/` | watchOS + CarPlay + HealthKit |

**معايير القبول Q1 2027:**
- [ ] watchOS تطبيق مع complications
- [ ] CarPlay dashboard + navigation
- [ ] HealthKit: workouts + sleep

---

### الربع الثاني (أبريل — يونيو 2027): "UIKit + Performance"

#### أبريل 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | UIKit: fallback widgets | `platform/ios/ui/uikit_widgets.swift` | UIKit لـ iOS 15- |
| 3-4 | UIKit: UICollectionView | `platform/ios/ui/uikit_collection.swift` | قوائم كبيرة performant |

#### مايو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Advanced widgets: Charts + Maps | `platform/ios/ui/swiftui_charts.swift` | Swift Charts + MapKit |
| 3-4 | Advanced widgets: Custom Drawing | `platform/ios/ui/canvas_bridge.swift` | Canvas + Core Graphics |

#### يونيو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Performance: startup optimization | `platform/ios/runtime/preload.swift` | App launch < 1.5s |
| 3-4 | Performance: memory footprint | `platform/ios/runtime/memory_opt.swift` | < 50 MB RSS |

**معايير القبول Q2 2027:**
- [ ] UIKit fallback لـ iOS 15
- [ ] Charts + Maps + Custom Drawing
- [ ] App launch < 1.5 ثانية
- [ ] Memory < 50 MB

---

### الربع الثالث (يوليو — سبتمبر 2027): "إنتاجي"

#### يوليو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | tvOS: Apple TV app | `platform/ios/tvos/` | تطبيق TV بلغة ص |
| 3-4 | tvOS: focus engine + remote | `platform/ios/tvos/focus.swift` | D-pad navigation |

#### أغسطس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | 5 تطبيقات إنتاجية | `examples/apps/ios/` | 5 تطبيقات متنوعة |
| 3-4 | App Clips + Widget Extensions | `platform/ios/extensions/` | App Clips + Home Screen widgets |

#### سبتمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | 10 تطبيقات على App Store | `distribution/ios/app_store/` | 10 تطبيقات منشورة |
| 3-4 | Performance + stability audit | `platform/ios/tests/` | Crash-free > 99.5% |

**معايير القبول Q3 2027:**
- [ ] tvOS تطبيق عامل
- [ ] App Clips + Widget Extensions
- [ ] 10 تطبيقات على App Store
- [ ] Crash-free > 99.5%

---

### الربع الرابع (أكتوبر — ديسمبر 2027): "v2.0 iOS"

#### أكتوبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | visionOS exploration | `platform/ios/visionos/` | استكشاف Apple Vision Pro |
| 3-4 | Performance: كل أحجام الشاشات | `platform/ios/ui/adaptive.swift` | iPhone + iPad + TV adaptive |

#### نوفمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Production quality: edge cases | `platform/ios/` | 0 أخطاء P0/P1 |
| 3-4 | v2.0 preparation | `platform/ios/tests/` | Full regression suite |

#### ديسمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | v2.0 release candidate | `platform/ios/` | RC جاهز |
| 3-4 | 🎉 v2.0 iOS release | `platform/ios/` | إطلاق رسمي |

**معايير القبول Q4 2027:**
- [ ] كل Apple platforms: iPhone, iPad, Watch, TV, Car
- [ ] visionOS exploration
- [ ] 10+ تطبيقات على App Store
- [ ] Crash-free > 99.5%
- [ ] v2.0 iOS stable

---

## الأدوات والتقنيات

| الأداة | الغرض | الإصدار |
|--------|--------|---------|
| Xcode | IDE + Build | 15+ |
| Swift | Language bridge | 5.9+ |
| SwiftUI | UI Framework | iOS 16+ |
| UIKit | Legacy UI | iOS 15+ |
| CocoaPods/SPM | Dependencies | latest |
| Fastlane | CI/CD automation | latest |
| TestFlight | Beta testing | latest |
| Instruments | Profiling | Xcode included |

---

## المخاطر الخاصة

| المخاطرة | خطة التخفيف |
|----------|-------------|
| macOS مطلوب للتطوير | GitHub Actions macOS runners + MacStadium cloud |
| Apple Developer Account تكلفة | $99/سنة — ميزانية مشروع |
| Swift API changes (5.9→6.0) | Pin Swift version + abstraction layer |
| App Store review rejection | اتباع Human Interface Guidelines بدقة |
| C++ interop complexity | استخدام Swift-C++ interop (Xcode 15+) |

---

## ملخص المُخرجات

```
2026:
  ✅ Swift Bridge كامل (8 أنواع ص ↔ Swift)
  ✅ iOS Runtime مع lifecycle + ARC bridge
  ✅ 20+ عنصر SwiftUI مع RTL
  ✅ Camera + GPS + ARKit + CoreML
  ✅ Push Notifications + CoreData + Keychain
  ✅ IPA Builder + App Store / TestFlight
  ✅ 3 تطبيقات كاملة

2027:
  ✅ watchOS + CarPlay + tvOS
  ✅ UIKit fallback + Charts + Maps
  ✅ App Clips + Widget Extensions
  ✅ 10 تطبيقات على App Store
  ✅ App launch < 1.5s, Crash-free > 99.5%
  ✅ visionOS exploration
  ✅ v2.0 iOS stable
```
