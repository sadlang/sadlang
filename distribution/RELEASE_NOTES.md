# 🎉 لغة ص v1.0.0-beta.1 — الإصدار التجريبي الأول

**تاريخ الإصدار:** 8 مارس 2026

---

## ✨ ما الجديد؟

### 🤖 Android SDK كامل
- **35+ ملف Kotlin/JNI** للتكامل السلس مع تطبيقات أندرويد
- مكونات جاهزة: NFC, HealthConnect, Maps, Billing, Firebase, WorkManager, Bluetooth
- واجهة Compose UI مع محرر كود ومحطة REPL

### 🍎 iOS SDK
- دعم SwiftUI مع CoreML و HealthKit
- قوالب Xcode جاهزة للاستخدام

### 🌐 WebAssembly
- شغّل لغة ص في المتصفح مباشرة
- جرّب الـ Playground: https://sad-lang.org/docs/playground

### 📱 نظام الواجهات (UI Pipeline)
- توليد تلقائي للواجهات لجميع المنصات
- دعم: Desktop (SDL2), Android (Compose), iOS (SwiftUI), Web (HTML/CSS/JS)

### 🎓 منهج تعليمي متكامل
- 8 أسابيع للمبتدئين
- أمثلة عملية ومشاريع تطبيقية

---

## 📦 التثبيت

### Windows
```powershell
irm https://sad-lang.org/install.ps1 | iex
```

### Linux / macOS
```bash
curl -fsSL https://sad-lang.org/install.sh | sh
```

### مديرو الحزم

| المنصة | الأمر |
|--------|-------|
| **winget** | `winget install SadLang.Sad --version 1.0.0-beta.1` |
| **scoop** | `scoop install sad` |
| **homebrew** | `brew install sad-lang/tap/sad` |
| **apt (deb)** | `sudo dpkg -i sad-lang_1.0.0-beta.1_amd64.deb` |
| **dnf (rpm)** | `sudo dnf install sad-lang-1.0.0~beta.1.rpm` |

---

## 📁 الملفات المتاحة

| الملف | الوصف | الحجم |
|-------|-------|-------|
| `sad-v1.0.0-beta.1-windows-x86_64.zip` | المفسر فقط (Windows) | ~15 MB |
| `sad-full-v1.0.0-beta.1-windows-x86_64.zip` | المفسر + المترجم + الأدوات | ~45 MB |
| `sad-v1.0.0-beta.1-linux-x86_64.tar.gz` | المفسر فقط (Linux) | ~12 MB |
| `sad-full-v1.0.0-beta.1-linux-x86_64.tar.gz` | الحزمة الكاملة (Linux) | ~40 MB |
| `sad-v1.0.0-beta.1-macos-aarch64.tar.gz` | المفسر (Apple Silicon) | ~13 MB |
| `sad-full-v1.0.0-beta.1-macos-aarch64.tar.gz` | الحزمة الكاملة (Apple Silicon) | ~42 MB |

---

## 🔧 المكونات

| الأداة | الوصف |
|--------|-------|
| `sad` | المفسر — يشغل ملفات `.ص` مباشرة |
| `sadc` | المترجم — يحوّل إلى ملف تنفيذي (يتطلب LLVM 18) |
| `sad-pkg` | مدير الحزم |
| `sad-lsp` | خادم LSP للمحررات |
| `sad-fmt` | أداة تنسيق الكود |
| `sad-repl` | محطة تفاعلية |

---

## ⚠️ ملاحظات البيتا

هذا إصدار تجريبي. قد توجد:
- بعض الأخطاء غير المكتشفة
- تغييرات في الـ API قبل الإصدار المستقر

**شارك ملاحظاتك:** https://github.com/sad-lang/sad-language/issues

---

## 🙏 شكر خاص

شكر لجميع المساهمين ومجتمع لغة ص!

---

**الموقع الرسمي:** https://sad-lang.org  
**التوثيق:** https://sad-lang.org/docs  
**GitHub:** https://github.com/sad-lang/sad-language
