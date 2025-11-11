# معمارية لغة ص - التصميم التقني / Sad Language Architecture - Technical Design

<div dir="rtl">

## 📐 نظرة عامة على المعمارية

تتبع لغة ص معمارية مفسر كلاسيكية مكونة من عدة طبقات متكاملة:

```
┌─────────────────────────────────────────┐
│         ملف المصدر (.s)                 │
│         Source File (.s)                │
└────────────────┬────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────┐
│      المحلل المعجمي (Lexer)             │
│      Token Stream                       │
└────────────────┬────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────┐
│      المحلل النحوي (Parser)             │
│      Abstract Syntax Tree (AST)         │
└────────────────┬────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────┐
│      المفسر (Interpreter)               │
│      Execution Engine                   │
└────────────────┬────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────┐
│      النتيجة / Output                   │
└─────────────────────────────────────────┘
```

## 🏗️ المكونات الرئيسية

### 1. المحلل المعجمي (Lexer)
**الموقع:** `src/lexer/`, `include/lexer/`

**المسؤولية:**
- تحويل النص الخام إلى رموز (Tokens)
- التعرف على الكلمات المفتاحية العربية
- معالجة الأرقام والنصوص والمعرفات

**الملفات:**
- `token.h/cpp` - تعريف أنواع الرموز
- `lexer_core.h/cpp` - المحلل الرئيسي
- `lexer_keywords.h/cpp` - جدول الكلمات المفتاحية
- `lexer_numbers.cpp` - معالجة الأرقام
- `lexer_strings.cpp` - معالجة النصوص
- `lexer_identifiers.cpp` - معالجة المعرفات

**مثال:**
```
"رقم س = 5"  →  [KEYWORD_TYPE, IDENTIFIER, ASSIGN, NUMBER]
```

---

### 2. المحلل النحوي (Parser)
**الموقع:** `src/parser/`, `include/parser/`

**المسؤولية:**
- تحويل الرموز إلى شجرة تحليل مجردة (AST)
- التحقق من صحة البنية النحوية
- بناء هيكل البرنامج

**الملفات:**
- `ast/ast_node.h/cpp` - العقد الأساسية
- `ast/ast_expressions.h/cpp` - عقد التعبيرات
- `ast/ast_statements.h/cpp` - عقد العبارات
- `parser_core.h/cpp` - المحلل الرئيسي
- `parser_expressions.cpp` - تحليل التعبيرات
- `parser_statements.cpp` - تحليل العبارات
- `parser_functions.cpp` - تحليل الدوال
- `parser_classes.cpp` - تحليل الأصناف

**مثال AST:**
```
Assignment
├── Left: Identifier("س")
└── Right: Literal(5)
```

---

### 3. المفسر (Interpreter)
**الموقع:** `src/interpreter/`, `include/interpreter/`

**المسؤولية:**
- تنفيذ الشجرة المجردة (AST)
- إدارة الذاكرة والمتغيرات
- استدعاء الدوال والأصناف

**الملفات:**
- `core/interpreter_core.h/cpp` - المفسر الرئيسي
- `core/execution_context.h/cpp` - سياق التنفيذ
- `visitors/eval_visitor.h/cpp` - زائر التقييم
- `expressions/expr_evaluator.cpp` - تقييم التعبيرات
- `statements/stmt_executor.cpp` - تنفيذ العبارات
- `oop/class_manager.cpp` - إدارة الأصناف

---

### 4. إدارة البيانات (Data Management)
**الموقع:** `src/data/`, `include/data/`

**المسؤولية:**
- إدارة أنواع البيانات
- تخزين المتغيرات والدوال
- إدارة النطاقات (Scopes)

**الملفات:**
- `types/value.h/cpp` - نظام القيم الموحد
- `types/array.h/cpp` - المصفوفات
- `managers/variable_manager.h/cpp` - مدير المتغيرات
- `managers/function_manager.h/cpp` - مدير الدوال
- `managers/class_manager.h/cpp` - مدير الأصناف
- `scope/scope_manager.h/cpp` - مدير النطاقات

**نظام القيم:**
```cpp
class Value {
    enum Type { INTEGER, DOUBLE, STRING, BOOLEAN, OBJECT, NULL_TYPE };
    Type type;
    union {
        int64_t intValue;
        double doubleValue;
        std::string* stringValue;
        bool boolValue;
        Object* objectValue;
    };
};
```

---

### 5. المكتبات القياسية (Standard Library)
**الموقع:** `src/stdlib/`, `include/stdlib/`

**المسؤولية:**
- توفير دوال مدمجة جاهزة
- مكتبات رياضية، نصوص، إدخال/إخراج

**المكتبات:**
- `math/` - دوال رياضية (جذر، قوة، جيب، جتا)
- `io/` - إدخال وإخراج (اطبع، اقرأ)
- `string/` - معالجة النصوص (طول، قص، استبدل)
- `time/` - الوقت والتاريخ
- `graphics/` - نظام الرسومات

---

### 6. إدارة الأخطاء (Error Management)
**الموقع:** `src/errors/`, `include/errors/`

**المسؤولية:**
- التقاط وتوثيق الأخطاء
- رسائل خطأ واضحة بالعربية والإنجليزية
- تتبع موقع الخطأ في الكود

**الملفات:**
- `error_manager.h/cpp` - مدير الأخطاء
- `error_reporter.h/cpp` - مبلغ الأخطاء

**أنواع الأخطاء:**
- Lexical Error (خطأ معجمي)
- Syntax Error (خطأ نحوي)
- Runtime Error (خطأ تنفيذي)
- Type Error (خطأ في النوع)

---

### 7. الأدوات المساعدة (Utilities)
**الموقع:** `src/utils/`, `include/utils/`

**المسؤولية:**
- دوال مساعدة للنصوص
- دوال التنقيح والطباعة

**الملفات:**
- `string_utils.h/cpp` - أدوات معالجة النصوص

---

## 🔄 تدفق التنفيذ

### 1. القراءة
```cpp
std::string sourceCode = readFile("program.s");
```

### 2. التحليل المعجمي
```cpp
Lexer lexer(sourceCode);
std::vector<Token> tokens = lexer.tokenize();
```

### 3. التحليل النحوي
```cpp
Parser parser(tokens);
std::unique_ptr<AST::Program> ast = parser.parse();
```

### 4. التنفيذ
```cpp
Interpreter interpreter;
interpreter.execute(ast.get());
```

---

## 🎨 نظام الرسومات

يعتمد على مفهوم الـ Widgets المشابه لـ Flutter:

```
Widget
├── StatelessWidget (عنصر_بلا_حالة)
│   └── build() → Widget
└── StatefulWidget (عنصر_بحالة)
    └── State
        └── build() → Widget
```

**أمثلة:**
- `نافذة()` - نافذة رئيسية
- `عمود()` - ترتيب عمودي
- `صف()` - ترتيب أفقي
- `زر()` - زر تفاعلي
- `نص()` - عرض نص

---

## 📊 إدارة الذاكرة

### المتغيرات
- تخزن في `VariableManager`
- يدعم النطاقات المتداخلة
- تنظيف تلقائي عند الخروج من النطاق

### الكائنات
- إدارة ذاكرة تلقائية باستخدام `shared_ptr`
- عداد مراجع لتجنب تسريب الذاكرة
- تنظيف عند عدم الاستخدام

---

## 🔒 معايير الأمان

1. **فحص الأنواع في وقت التنفيذ**
   - التحقق من صحة العمليات على الأنواع
   
2. **حماية الذاكرة**
   - استخدام smart pointers
   - فحص حدود المصفوفات

3. **معالجة الأخطاء**
   - استثناءات واضحة
   - عدم توقف مفاجئ

---

## 🚀 التحسينات المستقبلية

### المرحلة 1 (الحالية)
- ✅ المحلل المعجمي الأساسي
- ✅ المحلل النحوي
- ✅ المفسر البسيط

### المرحلة 2
- ⏳ نظام الأصناف الكامل
- ⏳ الوراثة والتعددية الشكلية
- ⏳ نظام الرسومات

### المرحلة 3
- 📅 مُحسِّن الكود (Optimizer)
- 📅 نظام التخزين المؤقت (Cache)
- 📅 نظام الوحدات (Modules)

### المرحلة 4
- 📅 المترجم (Compiler)
- 📅 توليد bytecode
- 📅 آلة افتراضية (VM)

---

## 📝 معايير الكود

### التسمية
- الفئات: `PascalCase` (مثل: `LexerCore`)
- الدوال: `camelCase` (مثل: `tokenize`)
- الثوابت: `UPPER_SNAKE_CASE` (مثل: `MAX_SIZE`)
- المتغيرات: `camelCase` (مثل: `currentToken`)

### التعليقات
- تعليقات ثنائية اللغة (عربي/إنجليزي)
- توثيق Doxygen لكل دالة عامة
- شرح الخوارزميات المعقدة

### الأخطاء
- استخدام exceptions للأخطاء
- رسائل واضحة بالعربية والإنجليزية
- تضمين موقع الخطأ في الكود

---

## 🧪 الاختبارات

### أنواع الاختبارات
1. **Unit Tests** - اختبار الوحدات
2. **Integration Tests** - اختبار التكامل
3. **End-to-End Tests** - اختبار شامل

### تشغيل الاختبارات
```bash
cd build
ctest --output-on-failure
```

---

## 📚 مراجع

- [C++ Reference](https://en.cppreference.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [Crafting Interpreters](https://craftinginterpreters.com/)

---

**آخر تحديث:** نوفمبر 2025

</div>
