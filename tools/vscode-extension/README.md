# لغة ص - Sad Language VS Code Extension

<div dir="rtl">

## نظرة عامة

إضافة VS Code الرسمية للغة ص - لغة البرمجة العربية الحديثة.

تدعم هذه الإضافة جميع ميزات البرمجة المتقدمة بفضل خادم اللغة (LSP) المدمج.

</div>

## Overview

Official VS Code extension for Sad Language - the modern Arabic programming language.

This extension provides full IDE features powered by an integrated Language Server Protocol (LSP) implementation.

---

## ✨ الميزات / Features

<div dir="rtl">

### 🎨 تلوين بناء الجملة / Syntax Highlighting
- دعم كامل للكلمات المفتاحية العربية والإنجليزية
- تمييز الأنواع والثوابت والدوال
- دعم التعليقات والنصوص

### 💡 الإكمال التلقائي / IntelliSense
- إكمال تلقائي للكلمات المفتاحية
- اقتراحات الدوال والمتغيرات
- معلومات عن الأنواع

### 📖 معلومات التلميح / Hover Information
- عرض معلومات الدوال والمتغيرات
- توثيق الكلمات المفتاحية
- نوع المتغير وقيمته

### 🔍 الذهاب للتعريف / Go to Definition
- الانتقال السريع لتعريف الدوال
- إيجاد تعريفات المتغيرات
- التنقل بين الأصناف

### 📍 إيجاد المراجع / Find References
- إيجاد جميع استخدامات الدالة
- عرض المراجع في جميع الملفات
- تسليط الضوء على الاستخدامات

### 🐛 التشخيصات / Diagnostics
- كشف الأخطاء في الوقت الفعلي
- تحذيرات وتلميحات
- رسائل خطأ واضحة بالعربية

### ✨ التنسيق التلقائي / Formatting
- تنسيق الكود تلقائياً
- خيارات قابلة للتخصيص
- دعم المسافات والتابات

### 📦 المقتطفات / Snippets
- مقتطفات جاهزة للدوال والحلقات
- قوالب للأصناف
- اختصارات للعبارات الشائعة

</div>

---

## 📦 التثبيت / Installation

<div dir="rtl">

### الطريقة 1: من VS Code Marketplace

1. افتح VS Code
2. اذهب إلى الإضافات (Ctrl+Shift+X)
3. ابحث عن "Sad Language"
4. اضغط Install

### الطريقة 2: من الملف

```bash
# بناء الإضافة
cd tools/vscode-extension
npm install
npm run compile
npm run package

# تثبيت الإضافة
code --install-extension sad-language-1.0.0.vsix
```

### الطريقة 3: وضع التطوير

```bash
cd tools/vscode-extension
npm install
npm run compile
# اضغط F5 في VS Code لفتح نافذة Extension Development Host
```

</div>

---

## ⚙️ الإعدادات / Settings

<div dir="rtl">

يمكنك تخصيص الإضافة من خلال إعدادات VS Code:

```json
{
  // مسار خادم اللغة (اختياري)
  "sad.server.path": "",
  
  // تفعيل/تعطيل خادم اللغة
  "sad.server.enabled": true,
  
  // مستوى التتبع (off/messages/verbose)
  "sad.trace.server": "off",
  
  // تفعيل التنسيق التلقائي
  "sad.formatting.enabled": true,
  
  // حجم التاب
  "sad.formatting.tabSize": 4,
  
  // استخدام المسافات بدلاً من التاب
  "sad.formatting.insertSpaces": true
}
```

</div>

---

## 🚀 الاستخدام / Usage

<div dir="rtl">

### 1. إنشاء ملف جديد

- أنشئ ملف بامتداد `.s` أو `.sad`
- ابدأ الكتابة واستمتع بالميزات التلقائية!

### 2. مثال بسيط

```sad
# بسم الله الرحمن الرحيم
# برنامج بسيط

دالة جمع(رقم أ, رقم ب) -> رقم
    إرجاع أ + ب
نهاية

دالة main()
    رقم نتيجة = جمع(5, 3)
    اطبع("النتيجة: " + نتيجة)
نهاية

main()
```

### 3. الأوامر المتاحة

- **Sad: إعادة تشغيل خادم اللغة** - لإعادة تشغيل الخادم
- **Sad: عرض سجل الخادم** - لعرض سجلات التصحيح

</div>

---

## 🛠️ متطلبات / Requirements

<div dir="rtl">

- VS Code 1.70.0 أو أحدث
- خادم لغة ص (sad-lsp) مُثبّت ومبني
  - يتم بناؤه تلقائياً مع المشروع
  - أو حدد المسار في الإعدادات

</div>

---

## 🐛 الإبلاغ عن مشاكل / Reporting Issues

<div dir="rtl">

إذا واجهت أي مشكلة:

1. تحقق من سجل خادم اللغة (Sad: Show Server Log)
2. أبلغ عن المشكلة في GitHub Issues
3. أرفق السجلات وخطوات إعادة المشكلة

</div>

---

## 📝 التطوير / Development

<div dir="rtl">

### بنية المشروع

```
vscode-extension/
├── package.json              # البيانات الوصفية
├── src/
│   └── extension.ts          # الكود الرئيسي
├── syntaxes/
│   └── sad.tmLanguage.json   # تعريفات التلوين
├── snippets/
│   └── sad.json              # المقتطفات
└── language-configuration.json # إعدادات اللغة
```

### المساهمة

نرحب بالمساهمات! يرجى:
1. Fork المشروع
2. إنشاء فرع للميزة الجديدة
3. Commit التغييرات
4. Push وإنشاء Pull Request

</div>

---

## 📜 الترخيص / License

MIT License

---

## 👥 الفريق / Team

<div dir="rtl">

فريق لغة ص - Sad Language Team

**الحمد لله رب العالمين**

</div>

---

## 🔗 روابط مفيدة / Useful Links

- [GitHub Repository](https://github.com/sad-lang/sad-language)
- [Documentation](https://sad-lang.org/docs)
- [Examples](https://sad-lang.org/examples)
- [Community](https://sad-lang.org/community)

---

<div dir="rtl">

## ❓ أسئلة شائعة / FAQ

### س: لماذا لا يعمل الإكمال التلقائي؟
ج: تأكد من أن خادم اللغة يعمل. راجع السجلات (Sad: Show Server Log).

### س: كيف أحدث الإضافة؟
ج: يتم التحديث تلقائياً من VS Code Marketplace، أو أعد بناء الإضافة في وضع التطوير.

### س: هل تدعم الإضافة العربية من اليمين لليسار؟
ج: نعم! تدعم الإضافة RTL بشكل كامل في المحرر.

### س: أين يتم حفظ السجلات؟
ج: في ملف `lsp_server.log` في مجلد المشروع، وفي قناة Output بـ VS Code.

</div>

---

**الله يسر ولا تعسر، الله تمم بخير** 🤲
