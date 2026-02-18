# 🤝 دليل المساهمة - لغة ص البرمجية
# Contributing Guide - Sad Programming Language

<div dir="rtl">

شكراً لاهتمامك بالمساهمة في **لغة ص**! هذا الدليل يشرح كيفية المساهمة في المشروع.

</div>

---

## 🚀 البداية السريعة / Quick Start

### المتطلبات / Prerequisites
- **C++17** compiler (MSVC 2022, GCC 11+, Clang 14+)
- **CMake** 3.15+
- **LLVM 18** (اختياري - للمترجم sadc فقط)
- **Git**

### الإعداد / Setup

```bash
# استنساخ المشروع / Clone the project
git clone https://github.com/user/s_language.git
cd s_language

# إعداد البناء / Configure build
cmake -S . -B build

# بناء المفسر / Build interpreter
cmake --build build --config Debug --target sad

# تشغيل اختبار / Run a test
./build/bin/Debug/sad.exe examples/مرحبا.ص
```

---

## 📁 هيكل المشروع / Project Structure

```
s_language/
├── shared/              # المكونات المشتركة (Lexer, Parser, AST, Types, Errors)
├── interpreter_new/     # المفسر الجديد
├── compiler_new/        # المترجم الجديد (SIR + LLVM)
├── vm/                  # الآلة الافتراضية (Bytecode VM)
├── stdlib/              # المكتبة القياسية
│   ├── core/           # الدوال الأساسية
│   ├── io/             # الإدخال/الإخراج
│   ├── math/           # الرياضيات
│   ├── string/         # النصوص
│   ├── network/        # الشبكات
│   ├── graphics/       # الرسومات
│   └── ...
├── graphics/            # مكتبة SDL2 + OpenGL
├── tools/               # أدوات التطوير
│   ├── lsp/            # خادم LSP
│   ├── formatter/      # أداة التنسيق
│   ├── pkg/            # مدير الحزم
│   └── repl/           # REPL
├── tests/               # الاختبارات
│   ├── comprehensive/  # الاختبارات الشاملة (900+)
│   └── ...
├── cmake/               # وحدات CMake
├── docs/                # التوثيق
└── examples/            # أمثلة بلغة ص
```

---

## 🔧 سير العمل / Workflow

### 1. اختر مهمة / Pick a Task
- تفقد [ROADMAP.md](ROADMAP.md) للمهام المتاحة
- ابحث عن Issues مفتوحة في GitHub
- **أولويات حالية:** الاختبارات، التوثيق، رسائل الخطأ

### 2. أنشئ فرع / Create a Branch
```bash
git checkout -b feature/اسم-الميزة
# أو / or
git checkout -b fix/وصف-الإصلاح
```

### 3. طبّق التغييرات / Make Changes
- اتبع معايير الكود (انظر أدناه)
- أضف اختبارات لأي ميزة جديدة
- تأكد من أن البناء يعمل

### 4. اختبر / Test
```bash
# بناء كل شيء
cmake --build build --config Debug

# تشغيل الاختبارات الشاملة
cmake --build build --config Debug --target comprehensive_tests
ctest --test-dir build -R Comprehensive
```

### 5. أرسل Pull Request
- وصف واضح بالعربية أو الإنجليزية
- ربط بأي Issue ذي صلة
- انتظر المراجعة

---

## 📝 معايير الكود / Coding Standards

### التسمية / Naming
```cpp
// الأصناف: PascalCase
class ExpressionEvaluator { };

// الدوال: camelCase
void evaluateExpression();

// المتغيرات: camelCase أو snake_case
int tokenCount;
std::string error_message;

// الثوابت: UPPER_SNAKE_CASE
const int MAX_RECURSION_DEPTH = 1000;

// الكلمات المفتاحية العربية: عربي كامل
// KEYWORD_FUNCTION → "دالة"
// KEYWORD_IF → "إذا"
// KEYWORD_CLASS → "صنف"
```

### قواعد عامة / General Rules
1. **UTF-8:** جميع الملفات يجب أن تكون UTF-8
2. **C++17:** استخدم ميزات C++17 (std::optional, std::variant, if constexpr)
3. **لا `using namespace std;`** في ملفات الرأس (.h)
4. **التعليقات ثنائية اللغة:** عربي + إنجليزي
5. **حجم الملف:** حاول أن لا يتجاوز 500 سطر
6. **لا `std::cout`:** استخدم `debugPrint()` للتسجيل

### مثال تعليق / Comment Example
```cpp
// ═══════════════════════════════════════════════════════
// تقييم التعبيرات الحسابية / Evaluate arithmetic expressions
// ═══════════════════════════════════════════════════════
// (AR) تدعم: الجمع، الطرح، الضرب، القسمة، باقي القسمة
// (EN) Supports: +, -, *, /, %
// ═══════════════════════════════════════════════════════
Value evaluateArithmetic(const BinaryExpr& expr) {
    // ...
}
```

---

## 🧪 كتابة الاختبارات / Writing Tests

### اختبار شامل (Comprehensive Test)
```cpp
#include <iostream>
#include <cassert>

// دالة مساعدة / Helper
#define TEST(name) std::cout << "  اختبار: " << name << "... ";
#define PASS() std::cout << "✅\n"; passed++;
#define FAIL(msg) std::cout << "❌ " << msg << "\n"; failed++;

int main() {
    int passed = 0, failed = 0;

    TEST("جمع عددين / Add two numbers")
    // الاختبار هنا
    PASS()

    std::cout << "\n=== النتائج: " << passed << " نجح، " 
              << failed << " فشل ===\n";
    return failed > 0 ? 1 : 0;
}
```

---

## 📂 وحدات CMake / CMake Modules

عند إضافة مكتبة أو اختبار جديد، عدّل الملف المناسب في `cmake/`:

| الملف | المحتوى |
|-------|---------|
| `cmake/sources.cmake` | تعريفات ملفات المصدر |
| `cmake/libraries.cmake` | المكتبات الأساسية |
| `cmake/network.cmake` | مكتبات الشبكات |
| `cmake/executables.cmake` | البرامج التنفيذية |
| `cmake/tools.cmake` | أدوات التطوير |
| `cmake/tests.cmake` | الاختبارات |
| `cmake/tests_comprehensive.cmake` | الاختبارات الشاملة |

---

## 🌟 أنواع المساهمات المرحب بها

| النوع | الوصف | المستوى |
|-------|-------|---------|
| 🐛 إصلاح أخطاء | إصلاح bugs مُبلّغ عنها | مبتدئ |
| 📖 توثيق | كتابة أو تحسين التوثيق | مبتدئ |
| 🧪 اختبارات | كتابة اختبارات جديدة | متوسط |
| ✨ ميزات | إضافة ميزة جديدة للغة | متقدم |
| 🔧 أدوات | تحسين LSP/Formatter/REPL | متقدم |
| 📦 حزم | إنشاء حزم للمكتبة القياسية | متوسط |
| 🌐 ترجمة | ترجمة رسائل الخطأ والتوثيق | مبتدئ |

---

## 📞 التواصل / Contact

- **GitHub Issues:** للأخطاء والاقتراحات
- **Pull Requests:** للمساهمات

---

<div dir="rtl">

**جزاك الله خيراً على مساهمتك! كل مساهمة مهما صغرت تُحدث فرقاً.**

</div>
