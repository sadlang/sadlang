# 📱 بناء تطبيقات iOS بلغة ص

## نظرة عامة

يمكنك بناء تطبيقات iOS أصلية باستخدام **لغة ص فقط** — بدون كتابة أي كود Swift أو Objective-C مباشرة.
المترجم يحول كود واجهة لغة ص إلى مشروع SwiftUI/Xcode كامل جاهز للبناء والنشر.

## كيف تعمل؟

```
مشروعك/
├── التطبيق.ص          # الكود الرئيسي بلغة ص
├── مشروع.صد           # إعدادات المشروع
└── (ملفات .ص أخرى)
        ↓
    sad build-ios مشروعك/
        ↓
    مشروعك.xcodeproj/  # مشروع Xcode جاهز!
    + ملفات .swift
```

## هيكل المشروع

```
مشروعي/
├── التطبيق.ص          # نقطة الدخول الرئيسية (مطلوب)
├── مشروع.صد           # إعدادات المشروع (اختياري)
├── الأيقونة.png       # أيقونة التطبيق (اختياري)
└── *.ص                # ملفات مصدر إضافية
```

## ملف الإعدادات (مشروع.صد)

```
# معلومات التطبيق
الاسم = "تطبيقي"
الحزمة = "com.example.myapp"
الإصدار = "1.0.0"

# متطلبات iOS
المنصة = "ios"
الحد_الأدنى = "16.0"
إصدار_سويفت = "5.9"
```

## مثال: تطبيق ترحيب

```sad
# التطبيق.ص
استورد واجهة

متغير العنوان = جديد نص("مرحباً بالعالم! 🌍")
العنوان.حجم_الخط(28)
العنوان.لون(.أزرق)

متغير الزر = جديد زر("اضغط هنا")
الزر.عند_الضغط(دالة()
    العنوان.غير("تم الضغط! ✅")
نهاية)

# التخطيط
متغير الشاشة = جديد عمود()
الشاشة.أضف(العنوان)
الشاشة.أضف(الزر)
```

هذا الكود يُولّد تلقائياً:

```swift
// ContentView.swift (مولّد تلقائياً)
import SwiftUI

struct ContentView: View {
    @State private var titleText = "مرحباً بالعالم! 🌍"
    
    var body: some View {
        VStack {
            Text(titleText)
                .font(.system(size: 28))
                .foregroundColor(.blue)
            
            Button("اضغط هنا") {
                titleText = "تم الضغط! ✅"
            }
        }
        .environment(\.layoutDirection, .rightToLeft)
    }
}
```

## عناصر الواجهة المتاحة

### العناصر الأساسية

| عنصر لغة ص | SwiftUI المقابل | الوصف |
|-------------|-----------------|-------|
| `نص` | `Text` | عرض نص |
| `زر` | `Button` | زر تفاعلي |
| `حقل_إدخال` | `TextField` | حقل إدخال نص |
| `صورة` | `Image` | عرض صورة |
| `عمود` | `VStack` | ترتيب عمودي |
| `صف` | `HStack` | ترتيب أفقي |
| `قائمة` | `List` | قائمة عناصر |

### المعدّلات (Modifiers)

```sad
# المعدّلات تُطبّق بتسلسل مثل SwiftUI
متغير النص = جديد نص("مرحباً")
النص.لون(.أحمر)
النص.حجم_الخط(24)
النص.حشو(16)
النص.إطار(لون: .رمادي، عرض: 1)
النص.ظل(نصف_قطر: 5)
```

## المتطلبات

- **لغة ص** (sad) مثبتة
- **macOS 13+** (مطلوب لبناء iOS)
- **Xcode 15+** مع iOS SDK
- **Apple Developer Account** (للنشر على App Store)

## الأوامر

```bash
# توليد مشروع Xcode
sad build-ios مشروعي/

# تحديد الإخراج
sad build-ios مشروعي/ -o مشروع_ios/

# بناء IPA مباشرة (يتطلب Xcode)
sad build-ios مشروعي/ --build --config Release
```

---

## 🏗️ المعمارية الداخلية — للمطورين

### مسار التحويل الكامل

```
ملف .ص (واجهة)
    ↓  [المحلل النحوي — shared/parser/]
شجرة AST (عقد UINode)
    ↓  [ios_swiftui.cpp — تحويل مكونات]
كود SwiftUI (دوال View)
    ↓  [ios_swiftui_codegen.cpp — توليد مشروع كامل]
مشروع Swift/SPM كامل:
├── Sources/App.swift          (نقطة الدخول)
├── Sources/*View.swift        (شاشة لكل عقدة)
├── Sources/Theme.swift        (المظهر والألوان)
├── Sources/AppRouter.swift    (التنقل بين الشاشات)
├── Package.swift              (Swift Package Manager)
└── Info.plist                 (بيانات التطبيق)
    ↓  [xcodebuild / swift build]
تطبيق iOS أصلي (.app / .ipa)
```

### الملفات المصدرية الأساسية

| الملف | المسار | الحجم | المسؤولية |
|-------|--------|-------|-----------|
| تحويل المكونات | `compiler_new/src/backend/ios_swiftui.cpp` | 1018+ سطر | AST → SwiftUI Views |
| Header | `compiler_new/include/backend/ios_swiftui.hpp` | — | إعدادات وبنى البيانات |
| توليد المشروع | `compiler_new/src/ui/ios_swiftui_codegen.cpp` | — | مشروع Xcode كامل |
| مصدّر بسيط | `compiler_new/src/backend/swiftui_emitter.cpp` | — | emit_view, emit_text, ... |
| سلسلة الأدوات | `compiler_new/src/targets/ios_toolchain.cpp` | — | cross-compilation, code signing |
| واجهة البناء | `tools/compiler/src/ios_target.cpp` | — | كشف Xcode، بناء |

### دعم RTL العربي

كل مشروع مولّد يتضمن تلقائياً:

```swift
.environment(\.layoutDirection, .rightToLeft)
```

هذا يضمن أن جميع عناصر الواجهة تعرض من اليمين إلى اليسار كما هو متوقع
للنصوص العربية. دعم RTL مدمج في ios_swiftui.cpp ولا يحتاج إعداداً يدوياً.

### الحالات المتعلقة بالتطوير المستقبلي

| الميزة | الحالة | ملاحظات |
|--------|--------|---------|
| توليد SwiftUI | ✅ مكتمل | 20+ مكون مدعوم |
| توليد مشروع كامل | ✅ مكتمل | App + Theme + Router + Package |
| كشف Xcode تلقائي | ✅ مكتمل | عبر xcode-select |
| دعم ARM64 + x86_64 | ✅ مكتمل | أجهزة + محاكي |
| توليد pbxproj | ⚠️ قيد التطوير | TODO في ios_target.cpp |
| Code Signing | ⚠️ أساسي | يحتاج تحسين |
| CoreML / ARKit | ❌ مخطط | الربع الثالث 2026 |

---

*آخر تحديث: مارس 2026*
*المسؤول: أحمد — مهندس منصة iOS*
