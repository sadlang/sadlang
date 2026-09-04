# لغة ص v1.0.0 — الإصدار الرسمي الأول 🎉

# Sad Programming Language v1.0.0 — First Official Release

---

## 🌟 ما هي لغة ص؟

**لغة ص** هي لغة برمجة عربية حديثة مبنية بـ C++17، تدعم UTF-8 بالكامل وتوفر بنية نحوية عربية أصيلة مع 40 كلمة محجوزة فقط.

**Sad (ص)** is a modern Arabic-first programming language built with C++17, featuring full UTF-8 support and native Arabic syntax with only 40 reserved keywords.

---

## 📦 محتويات الإصدار / Release Contents

### Windows x64

| الملف / File | الوصف / Description | الحجم / Size |
|---|---|---|
| `sad-setup-1.0.0-windows-x64.exe` | مثبّت GUI رسمي / Official GUI Installer | 24.8 MB |
| `sad-1.0.0-windows-x64.zip` | نسخة محمولة / Portable ZIP | 26.6 MB |

### التحقق / Verification (SHA256)

```
المثبّت / Installer: 52FFBB8D94FFC69128E5D9404965E6F60D2FC0AFF94B7FAEC78F33964A28980A
المحمولة / ZIP:      409C1E66734ED0D13F8872D6D6D8F6E27BB313434E209D4856619C63D3A5E5D7
```

```powershell
Get-FileHash .\sad-setup-1.0.0-windows-x64.exe -Algorithm SHA256
```

> ⚠️ **ملاحظة:** نسخ Linux و macOS قيد التحضير وستُنشر قريباً.
> ⚠️ **Note:** Linux and macOS builds are in preparation and will be released soon.

---

## 🧰 المكونات / Components

| المكوّن | الوصف | الحجم |
|---------|-------|-------|
| `sad.exe` | المفسر الرئيسي — تشغيل ملفات `.ص` مباشرة | 4.8 MB |
| `sadc.exe` | المترجم — تحويل `.ص` إلى ملف تنفيذي أصلي عبر LLVM 18 | 75.9 MB |
| `SDL2.dll` | مكتبة الرسوميات | 2.4 MB |
| `stdlib/` | المكتبة القياسية العربية | — |

---

## ✨ أبرز الميزات / Key Features

### البنية النحوية العربية / Arabic Syntax
```sad
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية

متغير النتيجة = جمع(3، 5)
اطبع_سطر("الناتج: " + النتيجة)
```

### البرمجة الكائنية / OOP
- أصناف (`صنف`) مع وراثة (`يرث`) ومُعدّلات وصول (`عام`/`خاص`/`محمي`)
- بواني (`باني`) وهوادم (`هدم`) وخصائص (`خاصية`) مع `احصل`/`عيّن`
- سمات وواجهات (`سمة`/`واجهة`) مع `نفّذ`
- أصناف مجردة (`مجرد`) وتعدادات (`تعداد`) وبنيات (`بنية`)
- تحميل العوامل الزائد (`عامل`)

### مطابقة الأنماط / Pattern Matching
```sad
طابق (عمر)
    عندما 0..13:
        اطبع("طفل")
    عندما 13..20:
        اطبع("مراهق")
    افتراضي:
        اطبع("بالغ")
نهاية
```

### مسار التنفيذ المزدوج / Dual Execution
- **المفسر (`sad`):** تشغيل فوري — مثالي للتعلم والتطوير السريع
- **المترجم (`sadc`):** تحويل إلى كود أصلي عبر LLVM 18 — أداء عالٍ للإنتاج

### ميزات إضافية / More Features
- البرمجة غير المتزامنة: `غير_متزامن`/`انتظر` (async/await)
- دوال مجهولة: `لامدا` (lambda)
- مولدات: `مولد`/`أنتج` (generators/yield)
- معالجة الأخطاء: `حاول`/`امسك`/`ارمي`/`أخيراً` (try/catch/throw/finally)
- نظام الوحدات: `استورد`/`من`/`كـ`/`صدّر` (import/from/as/export)
- توجيهات منخفضة المستوى: `@غير_آمن`، `@ذري`، `@وقت_الترجمة`
- إطار اختبار عربي: `اختبر`/`تأكد`
- 900+ اختبار شامل

---

## ⚡ التثبيت السريع / Quick Install

```powershell
# تثبيت بسطر واحد (PowerShell)
irm https://sad-lang.org/install.ps1 | iex
```

أو حمّل المثبّت من الروابط أعلاه.

---

## 💻 متطلبات النظام / System Requirements

| النظام | الحد الأدنى |
|--------|------------|
| Windows | Windows 10 x64 أو أحدث |

---

## 🔗 روابط مفيدة / Useful Links

- 🌐 الموقع: [sad-lang.org](https://sad-lang.org)
- 📖 التوثيق: [sad-lang.org/docs](https://sad-lang.org/docs)
- 💬 المجتمع: [GitHub Discussions](https://github.com/sadlang/sadlang/discussions)
- 🐛 الإبلاغ عن مشكلة: [GitHub Issues](https://github.com/sadlang/sadlang/issues)

---

## 📜 الترخيص / License

MIT License — مفتوح المصدر بالكامل.
