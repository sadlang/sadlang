# بسم الله الرحمن الرحيم

<div dir="rtl" align="right">

# 🛠️ أدوات التطوير للغة ص

[![الإصدار](https://img.shields.io/badge/الإصدار-1.0.0-blue.svg)](https://github.com/sad-lang/sad-language)
[![الرخصة](https://img.shields.io/badge/الرخصة-MIT-green.svg)](LICENSE)
[![الحالة](https://img.shields.io/badge/الحالة-جاهز-success.svg)]()

---

## 📋 نظرة عامة

هذا المجلد يحتوي على أدوات التطوير الكاملة للغة ص:

| المجلد | الأداة | الوصف |
|--------|-------|--------|
| `pkg/` | **ص-حزم** | مدير الحزم والتبعيات |
| `lsp/` | **ص-خادم** | خادم بروتوكول اللغة (LSP) |
| `compiler/` | **ص-نسق** | منسق الكود وأدوات المترجم |
| `vscode-extension/` | **إضافة VS Code** | دعم كامل في VS Code |
| `docgen/` | **ص-وثق** | مولد التوثيق |
| `repl/` | **ص-تفاعل** | المفسر التفاعلي |
| `templates/` | **قوالب** | قوالب المشاريع |

---

## 🚀 البدء السريع

### البناء

```bash
# من مجلد المشروع الرئيسي
cmake --build build --config Release

# أو بناء أداة محددة
cmake --build build --target sad-pkg
cmake --build build --target sad-lsp
```

### التثبيت

```bash
# نسخ الأدوات للمسار
sudo cp build/bin/sad-* /usr/local/bin/

# أو على Windows
copy build\bin\Release\sad-*.exe C:\sad\bin\
```

---

## 📦 مدير الحزم (pkg/)

مدير حزم متكامل مع:

- ✅ ملفات تكوين عربية (`ص.toml`)
- ✅ حل التبعيات (خوارزمية PubGrub)
- ✅ دعم SemVer 2.0.0
- ✅ سجل حزم مركزي
- ✅ ملفات قفل (`ص.قفل`)

**الملفات:**
- `package_manager.cpp/hpp` - النواة الرئيسية
- `dependency_resolver.cpp/hpp` - محلل التبعيات
- `registry_client.cpp/hpp` - عميل السجل

---

## 🖥️ خادم اللغة (lsp/)

خادم LSP كامل مع:

- ✅ الإكمال التلقائي الذكي
- ✅ التشخيصات الفورية
- ✅ معلومات التمرير بالعربية
- ✅ الانتقال للتعريف
- ✅ إيجاد المراجع
- ✅ التنسيق

**الملفات:**
- `lsp_server.cpp` - نواة الخادم
- `completion.cpp/hpp` - الإكمال التلقائي
- `diagnostics.cpp/hpp` - التشخيصات
- `hover.cpp/hpp` - معلومات التمرير
- `goto_definition.cpp/hpp` - التنقل

---

## ✨ منسق الكود (compiler/)

منسق كود متقدم مع:

- ✅ مسافات بادئة تلقائية
- ✅ مسافات حول العوامل
- ✅ قواعد تنسيق قابلة للتخصيص

**الملفات:**
- `formatter.cpp` - المنسق الرئيسي

---

## 💻 إضافة VS Code (vscode-extension/)

إضافة متكاملة مع:

- ✅ تمييز الصياغة
- ✅ الإكمال التلقائي
- ✅ اختصارات لوحة المفاتيح
- ✅ أوامر إدارة الحزم
- ✅ شريط الحالة

**الملفات:**
- `src/extension.ts` - الكود الرئيسي
- `package.json` - تكوين الإضافة
- `syntaxes/sad.tmLanguage.json` - قواعد التلوين
- `snippets/sad.json` - القصاصات

---

## 📚 التوثيق

- [دليل المستخدم](../docs/developer-tools-guide.md)
- [التوثيق التقني](../docs/developer-tools-technical.md)

---

## 🤝 المساهمة

1. Fork المستودع
2. أنشئ فرع للميزة (`git checkout -b feature/ميزة-جديدة`)
3. Commit التغييرات (`git commit -m 'إضافة ميزة جديدة'`)
4. Push للفرع (`git push origin feature/ميزة-جديدة`)
5. افتح Pull Request

---

## 📄 الرخصة

MIT License - انظر [LICENSE](../LICENSE)

---

<div align="center">

**الحمد لله رب العالمين**

صُنع بـ ❤️ للمبرمج العربي

</div>

</div>
