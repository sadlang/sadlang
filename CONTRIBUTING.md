# بسم الله الرحمن الرحيم

# 🤝 دليل المساهمة في لغة ص / Contributing to Sad Language

شكراً لاهتمامك بالمساهمة في لغة ص! هذا الدليل يشرح كيفية المساهمة بشكل فعال.

---

## 📋 جدول المحتويات

1. [استراتيجية Git](#-استراتيجية-git--git-branching-strategy)
2. [قواعد الدمج](#-قواعد-الدمج--merge-rules)
3. [معايير الكود](#-معايير-الكود--code-standards)
4. [تشغيل الاختبارات](#-الاختبارات--testing)
5. [فتح Pull Request](#-فتح-pull-request)

---

## 🌿 استراتيجية Git / Git Branching Strategy

نستخدم **Git Flow مُعدّل** (Modified Git Flow) مُحسّن للتعاون الفعال:

### الفروع الرئيسية

```
main ──────────── الإصدارات المستقرة فقط (v1.0, v1.1, ...)
  │
  └── develop ─── فرع التطوير الرئيسي (كل الميزات الجديدة تُدمج هنا)
        │
        ├── feature/* ─── ميزات جديدة
        ├── fix/* ─────── إصلاحات أخطاء  
        ├── hotfix/* ──── إصلاحات عاجلة (تُدمج في main + develop)
        └── release/* ─── تحضير الإصدارات
```

### مخطط الفروع

```
main ─────────────────────────────────────────────────────── v1.0
  │                                                           │
  └── develop ────────────────────────────────────────────────┘
        │
        ├── feature/ahmad-scoping-fix ──────────────────┐
        │                                                │
        ├── feature/saeed-compiler-fixes ───────────────┤
        │                                                │
        ├── feature/kamel-wasm-backend ─────────────────├── تُدمج في develop
        │                                                │
        ├── feature/reem-stdlib-docs ───────────────────┤
        │                                                │
        └── feature/omar-test-framework ────────────────┘
              │
              └── fix/omar-lsp-completion ────── إصلاح فرعي يُدمج في الميزة
```

### تسمية الفروع

| النوع | الصيغة | مثال |
|-------|--------|------|
| ميزة جديدة | `feature/{اسم_المطور}-{وصف_قصير}` | `feature/ahmad-generics` |
| إصلاح خطأ | `fix/{اسم_المطور}-{رقم_المشكلة}` | `fix/ahmad-p2-while-loop` |
| إصلاح عاجل | `hotfix/v{version}-{وصف}` | `hotfix/v1.0-crash-fix` |
| إصدار | `release/v{version}` | `release/v0.9-beta` |

### سير العمل

```bash
# 1. تأكد أنك على develop محدّث
git checkout develop
git pull origin develop

# 2. أنشئ فرع جديد
git checkout -b feature/omar-test-framework

# 3. اعمل على التغييرات
git add .
git commit -m "feat(tests): إضافة إطار اختبارات موحد"

# 4. ادفع الفرع
git push -u origin feature/omar-test-framework

# 5. افتح Pull Request إلى develop
```

---

## ✅ قواعد الدمج / Merge Rules

قبل دمج أي Pull Request، يجب استيفاء الشروط التالية:

### الشروط الإلزامية

| ✅ | الشرط | التفاصيل |
|----|-------|----------|
| ☐ | **Code Review** | موافقة واحدة على الأقل من مراجع آخر |
| ☐ | **CI Pass** | جميع اختبارات CI تمر على Windows + Linux + macOS |
| ☐ | **لا تحذيرات جديدة** | عدد التحذيرات ≤ العدد السابق |
| ☐ | **توثيق** | كل دالة عامة جديدة موثقة (عربي + إنجليزي) |
| ☐ | **اختبار** | كل تغيير له اختبار واحد على الأقل |

### أزواج المراجعة

```
أحمد (النواة) ↔ سعيد (المترجم)    — يراجع كل منهما كود الآخر
كامل (المنصات) ↔ عمر (الأدوات)    — يراجع كل منهما كود الآخر
ريم (المكتبات + التوثيق)          — يُراجَع من أي مطور متاح
```

### رسائل Commit

استخدم صيغة **Conventional Commits** مع دعم العربية:

```
<type>(<scope>): <description>

[body اختياري]

[footer اختياري]
```

| Type | الاستخدام |
|------|----------|
| `feat` | ميزة جديدة |
| `fix` | إصلاح خطأ |
| `docs` | توثيق |
| `test` | اختبارات |
| `refactor` | إعادة هيكلة بدون تغيير وظيفي |
| `perf` | تحسين أداء |
| `chore` | مهام صيانة |

**أمثلة:**

```
feat(parser): إضافة دعم generics

fix(interpreter): إصلاح المشكلة #2 — متغير x في بينما

docs(stdlib): توثيق وحدة network

test(lexer): إضافة 20 اختبار للكلمات المحجوزة
```

---

## 🏗️ بنية المشروع / Project Architecture

فهم بنية المشروع ضروري قبل المساهمة. لغة ص مشروع كبير يتكون من عدة أنظمة فرعية متكاملة:

### خريطة المجلدات الرئيسية

```
s-programming-language/
├── shared/              ← النواة المشتركة: محلل معجمي، نحوي، AST، أنواع
│   ├── lexer/           ← المحلل المعجمي (40 كلمة محجوزة + كلمات سياقية)
│   ├── parser/          ← المحلل النحوي (يولّد شجرة AST)
│   ├── ast/             ← عقد الشجرة النحوية المجردة
│   └── types/           ← نظام القيم (Value — std::variant موحد)
│
├── interpreter_new/     ← المفسر الشجري (InterpreterCore)
│   ├── core/            ← محرك التنفيذ والنطاقات والمتغيرات
│   └── visitors/        ← زوار AST (كل عقدة لها زائر)
│
├── compiler_new/        ← المترجم sadc (AST → SIR → LLVM IR → native)
│   ├── src/sir/         ← التمثيل الوسيط SIR (12 تعليمة)
│   ├── src/backend/     ← مولدات الكود (LLVM, JIT)
│   └── src/frontend/    ← الواجهة الأمامية للمترجم
│
├── vm/                  ← الآلة الافتراضية (بايت كود مربوط بالمفسر)
├── stdlib/              ← المكتبة القياسية (core, io, math, string, network)
├── graphics/            ← مكتبة الرسوميات (SDL2 + OpenGL)
├── tools/               ← الأدوات: LSP, formatter, pkg, REPL, compiler
├── tests/               ← الاختبارات الشاملة (~900 اختبار)
├── docs/                ← التوثيق والمراجع
└── cmake/               ← وحدات البناء (llvm.cmake, platform.cmake, ...)
```

### مسار تدفق البيانات

```
ملف .ص  →  المحلل المعجمي (LexerCore)
        →  المحلل النحوي (ParserCore)  →  شجرة AST
        ↓                                    ↓
   المفسر / الآلة الافتراضية          SIR → LLVM IR → ملف تنفيذي
```

### فريق التطوير ومسؤولياته

| المهندس | المسؤولية | المجلدات الرئيسية |
|---------|-----------|-------------------|
| خالد | المترجم والنواة | `compiler_new/`, `cmake/`, `shared/` |
| كامل | سطح المكتب والرسوميات | `graphics/`, `stdlib/` |
| سعيد | منصة Android | `compiler_new/src/backend/` |
| أحمد | منصة iOS | `compiler_new/src/backend/` |
| عمر | الأدوات والبنية التحتية | `tools/`, `.github/`, `tests/` |
| ريم | التوثيق والمجتمع | `docs/`, `website/`, `stdlib/` |

---

## 📐 معايير الكود / Code Standards

### C++ Style

```cpp
// 1. فضاءات الأسماء
namespace Sad::Parser {
    // ...
}

// 2. التعليقات ثنائية اللغة
/**
 * @brief (AR) يحلل دالة
 * @brief (EN) Parses a function
 */
std::unique_ptr<FunctionNode> parseFunction();

// 3. الكلمات المفتاحية
// استخدم KEYWORD_* وليس نصوص عربية مباشرة
if (token.type == TokenType::KEYWORD_FUNCTION) { ... }

// 4. تسمية المتغيرات
int wordCount;        // camelCase
std::string fileName; // camelCase
const int MAX_SIZE;   // UPPER_SNAKE_CASE
```

### ملفات .ص (لغة ص)

```sad
# استخدم التعليقات العربية
# كل دالة موثقة

## يجمع عددين
## @param أ العدد الأول
## @param ب العدد الثاني
## @return مجموع العددين
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية
```

---

## 🧪 الاختبارات / Testing

### تشغيل الاختبارات محلياً

```bash
# بناء مع تفعيل الاختبارات
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build --config Debug

# تشغيل كل الاختبارات
ctest --test-dir build --output-on-failure

# تشغيل اختبارات محددة
./build/bin/Debug/unit_tests --filter=Lexer

# تشغيل الاختبارات الشاملة
.\tests\run_comprehensive_tests.ps1
```

### كتابة اختبار جديد

```cpp
#include "tests/framework/sad_test.h"

// xUnit style
TEST(Parser, يحلل_صنف_بسيط) {
    auto ast = parse(u8"صنف سيارة نهاية");
    ASSERT_NOT_NULL(ast);
    ASSERT_EQ(ast->name, "سيارة");
}

// BDD style
describe("المحلل المعجمي", []() {
    it("يتعرف على الكلمات المفتاحية العربية", []() {
        auto tokens = lex(u8"دالة");
        expect(tokens[0].type).to_equal(TokenType::KEYWORD_FUNCTION);
    });
});
```

---

## 🚀 فتح Pull Request

### القالب

عند فتح PR، استخدم هذا القالب:

```markdown
## الوصف
وصف موجز للتغييرات.

## نوع التغيير
- [ ] ميزة جديدة (feat)
- [ ] إصلاح خطأ (fix)
- [ ] توثيق (docs)
- [ ] إعادة هيكلة (refactor)
- [ ] اختبار (test)

## المشاكل المُصلحة
- Fixes #123
- Related to #456

## قائمة التحقق
- [ ] الكود يتبع معايير المشروع
- [ ] أضفت اختبارات للتغييرات
- [ ] وثقت الدوال العامة الجديدة
- [ ] كل الاختبارات تمر محلياً

## لقطات شاشة (إن وجدت)
```

---

## 📞 التواصل

- **GitHub Issues** — للأخطاء والاقتراحات
- **GitHub Discussions** — للأسئلة والنقاشات
- **Discord** — للتواصل الفوري

---

*آخر تحديث: مارس 2026*
*أُعدّ بواسطة: عمر — مهندس البنية التحتية*
