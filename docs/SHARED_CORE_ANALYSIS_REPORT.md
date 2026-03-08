# تقرير تحليل النواة المشتركة (shared/) للغة ص
# Shared Core Analysis Report for Sad Language

**تاريخ التقرير:** 8 مارس 2026  
**إجمالي أسطر الكود:** 65,768 سطر

---

## 📊 ملخص إحصائي

| المكون | الملفات | الأسطر | النسبة |
|--------|---------|--------|--------|
| **lexer/** (المحلل المعجمي) | 8 | 4,718 | 7.2% |
| **parser/** (المحلل النحوي) | 37 | 28,959 | 44.0% |
| **ast/** (شجرة AST) | 26 | 11,383 | 17.3% |
| **types/** (نظام الأنواع) | 11 | 4,137 | 6.3% |
| **أخرى** (errors, modules, utils) | ~14 | ~16,571 | 25.2% |
| **الإجمالي** | 96+ | **65,768** | 100% |

---

## 1️⃣ المحلل المعجمي (Lexer) — 4,718 سطر

### 📁 هيكل الملفات

| الملف | الأسطر | الدور |
|-------|--------|-------|
| `token.h` | 550 | تعريف TokenType enum + Position struct + Token class |
| `lexer_core.h` | 172 | واجهة LexerCore الرئيسية |
| `lexer_keywords.h` | 181 | واجهة KeywordTable |
| `lexer_core.cpp` | 1,620 | تنفيذ المحلل المعجمي |
| `lexer_keywords.cpp` | 496 | جدول الكلمات المفتاحية |
| `macro_tokens.cpp` | 635 | دعم الماكرو |
| `lifetime_tokens.cpp` | 745 | دعم Lifetimes |
| `token.cpp` | 319 | دوال مساعدة للرموز |

### 🔤 قائمة أنواع الرموز الكاملة (TokenType enum)

#### الكلمات المحجوزة (40 كلمة)

| # | الرمز | العربي | الإنجليزي |
|---|-------|--------|-----------|
| 1 | `KEYWORD_FUNCTION` | دالة | function |
| 2 | `KEYWORD_RETURN` | ارجع | return |
| 3 | `KEYWORD_CLASS` | صنف | class |
| 4 | `KEYWORD_STRUCT` | بنية | struct |
| 5 | `KEYWORD_ENUM` | تعداد | enum |
| 6 | `KEYWORD_INHERITS` | يرث | inherits |
| 7 | `KEYWORD_END` | نهاية | end |
| 8 | `KEYWORD_NEW` | جديد | new |
| 9 | `KEYWORD_THIS` | هذا | this |
| 10 | `KEYWORD_CONSTRUCTOR` | باني/منشئ/بناء | constructor |
| 11 | `KEYWORD_SUPER` | الأساس | super |
| 12 | `KEYWORD_IF` | إذا/اذا | if |
| 13 | `KEYWORD_ELSE` | وإلا/والا | else |
| 14 | `KEYWORD_WHILE` | بينما/طالما | while |
| 15 | `KEYWORD_FOR` | لكل | for |
| 16 | `KEYWORD_IN` | في | in |
| 17 | `KEYWORD_BREAK` | توقف | break |
| 18 | `KEYWORD_CONTINUE` | استمر | continue |
| 19 | `KEYWORD_MATCH` | طابق | match |
| 20 | `KEYWORD_WHEN` | عندما | when |
| 21 | `KEYWORD_DEFAULT` | افتراضي | default |
| 22 | `KEYWORD_TRY` | حاول | try |
| 23 | `KEYWORD_CATCH` | امسك | catch |
| 24 | `KEYWORD_THROW` | ارمي | throw |
| 25 | `KEYWORD_FINALLY` | أخيراً/أخيرا | finally |
| 26 | `KEYWORD_PUBLIC` | عام | public |
| 27 | `KEYWORD_PRIVATE` | خاص | private |
| 28 | `KEYWORD_PROTECTED` | محمي | protected |
| 29 | `KEYWORD_ABSTRACT` | مجرد | abstract |
| 30 | `KEYWORD_IMPORT` | استورد | import |
| 31 | `KEYWORD_FROM` | من | from |
| 32 | `KEYWORD_AS` | كـ | as |
| 33 | `KEYWORD_EXPORT` | صدّر/صدر | export |
| 34 | `KEYWORD_VAR` | متغير | var |
| 35 | `KEYWORD_CONST` | ثابت | const |
| 36 | `KEYWORD_STATIC` | ساكن/ثابتة | static |
| 37 | `KEYWORD_EXTERN` | خارجي | extern |
| 38 | `LITERAL_TRUE` | صحيح | true |
| 39 | `LITERAL_FALSE` | خطأ | false |
| 40 | `LITERAL_NULL` | لاشيء | null |

#### العوامل المنطقية (3 عوامل)

| # | الرمز | العربي | الإنجليزي |
|---|-------|--------|-----------|
| 1 | `OP_AND` | و / && | AND |
| 2 | `OP_OR` | أو / \|\| | OR |
| 3 | `OP_NOT` | ليس / ! | NOT |

#### الكلمات السياقية (Contextual Keywords)

| الرمز | العربي | السياق |
|-------|--------|--------|
| `KEYWORD_ASYNC` | غير_متزامن | قبل `دالة` |
| `KEYWORD_AWAIT` | انتظر | قبل تعبير |
| `KEYWORD_LAMBDA` | لامدا | بداية تعبير λ |
| `KEYWORD_YIELD` | أنتج | داخل مولد |
| `KEYWORD_GENERATOR` | مولد | تعريف مولد |
| `KEYWORD_WITH` | باستخدام | context manager |
| `KEYWORD_END_WITH` | نهاية_استخدام | نهاية context |
| `KEYWORD_TRAIT` | سمة | تعريف سمة |
| `KEYWORD_IMPL` | نفّذ/نفذ | تنفيذ سمة |
| `KEYWORD_TEMPLATE` | قالب | تعريف قالب |
| `KEYWORD_NAMESPACE` | فضاء | تعريف namespace |
| `KEYWORD_END_NAMESPACE` | نهاية_فضاء | نهاية namespace |
| `KEYWORD_OPERATOR` | عامل | تحميل عامل |
| `KEYWORD_PROPERTY` | خاصية | داخل صنف |
| `KEYWORD_GET` | احصل | داخل خاصية |
| `KEYWORD_SET` | عيّن | داخل خاصية |
| `KEYWORD_DESTRUCTOR` | هدم | داخل صنف |
| `KEYWORD_MAIN` | رئيسية | نقطة الدخول |
| `KEYWORD_TEST` | اختبر | تعريف اختبار |
| `KEYWORD_CASE` | حالة | في switch |

#### أنواع البيانات المدمجة (Built-in Types)

| الرمز | العربي | الوصف |
|-------|--------|-------|
| `TYPE_INTEGER` | رقم | عدد صحيح 64-bit |
| `TYPE_DOUBLE` | عشري | عدد عشري |
| `TYPE_STRING` | نص | نص |
| `TYPE_BOOLEAN` | منطقي | صحيح/خطأ |
| `TYPE_VOID` | فراغ | لا قيمة |
| `TYPE_NULL` | عدم | null |
| `TYPE_ARRAY` | مصفوفة | مصفوفة |
| `TYPE_MAP` | خريطة | قاموس |
| `TYPE_ANY` | أي | أي نوع |

#### أنواع منخفضة المستوى (Low-Level Types)

| الرمز | العربي | الحجم |
|-------|--------|-------|
| `TYPE_U8` | ط8 / بايت | 8-bit unsigned |
| `TYPE_U16` | ط16 / كلمة | 16-bit unsigned |
| `TYPE_U32` | ط32 | 32-bit unsigned |
| `TYPE_U64` | ط64 | 64-bit unsigned |
| `TYPE_I8` | ص8 | 8-bit signed |
| `TYPE_I16` | ص16 | 16-bit signed |
| `TYPE_I32` | ص32 | 32-bit signed |
| `TYPE_I64` | ص64 | 64-bit signed |
| `TYPE_PTR` | مؤشر | مؤشر خام |
| `TYPE_USIZE` | ط_حجم | عنوان ذاكرة |
| `TYPE_ISIZE` | ص_حجم | عنوان بإشارة |

#### العوامل (Operators)

```
حسابية:    OP_PLUS (+), OP_MINUS (-), OP_MULTIPLY (*), OP_DIVIDE (/), 
          OP_MODULO (%), OP_POWER (**)

مقارنة:    OP_EQUAL (==), OP_NOT_EQUAL (!=), OP_LESS (<), OP_GREATER (>),
          OP_LESS_EQUAL (<=), OP_GREATER_EQUAL (>=)

تعيين:     OP_ASSIGN (=), OP_WALRUS (:=), OP_PLUS_ASSIGN (+=), 
          OP_MINUS_ASSIGN (-=), OP_MULTIPLY_ASSIGN (*=), OP_DIVIDE_ASSIGN (/=)

بتّية:     OP_BITWISE_AND (&), OP_BITWISE_OR (|), OP_XOR (^), 
          OP_BITWISE_NOT (~), OP_SHIFT_LEFT (<<), OP_SHIFT_RIGHT (>>)

زيادة:     OP_INCREMENT (++), OP_DECREMENT (--)

أنبوب:     OP_PIPE_ARROW (|>)
```

#### الرموز الخاصة (Punctuation & Brackets)

```
أقواس:     PAREN_LEFT/RIGHT (), BRACE_LEFT/RIGHT {}, BRACKET_LEFT/RIGHT []

ترقيم:     COMMA (,), ARABIC_COMMA (،), SEMICOLON (;), ARABIC_SEMICOLON (؛),
          COLON (:), DOUBLE_COLON (::), DOT (.), DOT_DOT (..), ELLIPSIS (...)

أسهم:      ARROW (->), FAT_ARROW (=>)

خاصة:      QUESTION (?), AT_SIGN (@), AMPERSAND (&)
```

#### رموز القيم والمعرفات

```
IDENTIFIER          - معرّف (اسم متغير/دالة/صنف)
NUMBER_INTEGER      - رقم صحيح (123, 0xFF)
NUMBER_DOUBLE       - رقم عشري (3.14, 1.5e10)
STRING_LITERAL      - نص حرفي ("نص")
STRING_RAW          - نص خام (r"نص\n")
STRING_FSTRING      - نص منسق (f"مرحباً {اسم}")
STRING_FSTRING_PART - جزء من نص منسق
DOC_COMMENT         - تعليق توثيقي (## أو #** **#)
COMMENT             - تعليق عادي
NEWLINE             - سطر جديد
END_OF_FILE         - نهاية الملف
INVALID             - رمز غير صالح
WHITESPACE          - مسافة
```

### 🌍 دعم Unicode

- **ترميز UTF-8 كامل** — دعم جميع الأحرف العربية والإنجليزية
- **الفاصلة العربية (،)** — مدعومة كبديل للفاصلة اللاتينية
- **الفاصلة المنقوطة العربية (؛)** — مدعومة كبديل
- **أسماء المتغيرات العربية** — مدعومة بالكامل
- **تنسيق RTL** — لا يؤثر على التحليل

---

## 2️⃣ المحلل النحوي (Parser) — 28,959 سطر

### 📁 هيكل الملفات

| الملف | الأسطر | الدور |
|-------|--------|-------|
| `parser_core.h` | 1,266 | واجهة ParserCore الرئيسية |
| `parser_core_main.cpp` | 1,016 | التحليل الرئيسي |
| `parser_core_helpers.cpp` | 1,253 | دوال مساعدة |
| `parser_core_oop.cpp` | 865 | تحليل OOP |
| `parser_expressions.cpp` | 1,378 | تحليل التعابير |
| `parser_statements.cpp` | 769 | تحليل الجمل |
| `parser_declarations.cpp` | 1,199 | تحليل التصريحات |
| `parser_advanced.cpp` | 1,830 | ميزات متقدمة |
| `class_parser.cpp` | 1,319 | تحليل الأصناف |
| `constructor_parser.cpp` | 1,141 | تحليل البواني |
| `async_parser.cpp` | 738 | تحليل async/await |
| `await_parser.cpp` | 758 | تحليل await |
| `generics_parser.cpp` | 509 | تحليل القوالب |
| `extern_parser.cpp` | 933 | تحليل FFI |
| `ui_parser.cpp` | 1,128 | تحليل UI DSL |
| `asm_parser.cpp` | 815 | تحليل التجميع |
| `comprehension_parser.cpp` | 691 | تحليل List Comprehension |
| `yield_parser.cpp` | 549 | تحليل generators |
| آخرون | ~12,802 | ملفات مساعدة |

### 🔧 تقنية التحليل

- **Recursive Descent Parsing** — تحليل تنازلي متكرر
- **Lookahead** — نظرة مسبقة للرموز (current + peek)
- **Error Recovery** — استرداد من الأخطاء
- **Precedence Climbing** — أولويات العوامل

### 📋 ما يُحلل

| الفئة | المكونات |
|-------|----------|
| **التصريحات** | دالة، صنف، بنية، تعداد، سمة، نفّذ، قالب، فضاء |
| **الجمل** | إذا، بينما، لكل، طابق، حاول/امسك، ارجع، توقف، استمر |
| **التعابير** | ثنائية، أحادية، ثلاثية، استدعاء، فهرسة، عضوية، λ |
| **OOP** | أصناف، وراثة متعددة، خصائص، طرق، باني، هدم |
| **متقدم** | async/await، generators، list comprehension، pattern matching |
| **FFI** | دوال خارجية، تجميع مضمن |

---

## 3️⃣ شجرة AST — 11,383 سطر

### 📁 هيكل الملفات

| الملف | الأسطر | الدور |
|-------|--------|-------|
| `ast_node.h` | 214 | الفئة الأساسية ASTNode |
| `ast_visitor.h` | 898 | واجهة نمط الزائر |
| `ast_printer.h/cpp` | 1,595 | طباعة AST |
| `expressions.h/cpp` | 1,402 | عقد التعابير |
| `statements.h/cpp` | 805 | عقد الجمل |
| `declarations.h/cpp` | 1,335 | عقد التصريحات |
| `class_nodes.h/cpp` | 488 | عقد OOP |
| `pattern_nodes.h` | 549 | عقد Pattern Matching |
| `module_nodes.h/cpp` | 538 | عقد الوحدات |
| `type_nodes.h/cpp` | 871 | عقد الأنواع |
| `comprehension_nodes.h` | 250 | عقد List Comprehension |
| آخرون | ~2,438 | ملفات إضافية |

### 🌳 أنواع عقد AST

#### عقد التعابير (Expressions)

| العقدة | الوصف | مثال |
|--------|-------|------|
| `BinaryExpr` | تعبير ثنائي | `أ + ب` |
| `UnaryExpr` | تعبير أحادي | `-س` |
| `TernaryExpr` | تعبير ثلاثي | `شرط ? أ : ب` |
| `LiteralExpr` | قيمة حرفية | `42`, `"نص"` |
| `VariableExpr` | متغير | `اسم` |
| `AssignExpr` | إسناد | `س = 5` |
| `CallExpr` | استدعاء دالة | `دالة(م1، م2)` |
| `IndexExpr` | فهرسة | `مصفوفة[0]` |
| `MemberExpr` | وصول عضو | `كائن.خاصية` |
| `ArrayExpr` | مصفوفة | `[1، 2، 3]` |
| `MapExpr` | قاموس | `{"مفتاح": "قيمة"}` |
| `LambdaExpr` | دالة مجهولة | `لامدا (س) => س * 2` |
| `AwaitExpr` | انتظار async | `انتظر دالة_غير_متزامنة()` |
| `NewExpr` | إنشاء كائن | `جديد صنف()` |
| `ThisExpr` | المرجع الذاتي | `هذا.خاصية` |
| `SuperExpr` | الصنف الأساس | `الأساس.دالة()` |
| `RangeExpr` | مدى | `1..10` |
| `ListComprehensionExpr` | فهم قائمة | `[س * 2 لكل س في قائمة]` |
| `WalrusExpr` | Walrus operator | `(س := 5)` |

#### عقد الجمل (Statements)

| العقدة | الوصف | مثال |
|--------|-------|------|
| `ExprStmt` | جملة تعبير | `اطبع("مرحباً")` |
| `VarDeclStmt` | تصريح متغير | `متغير س = 10` |
| `IfStmt` | شرط if | `إذا (شرط) ... نهاية` |
| `WhileStmt` | حلقة while | `بينما (شرط) ... نهاية` |
| `ForStmt` | حلقة for كلاسيكية | `for (i=0; i<10; i++)` |
| `ForRangeStmt` | حلقة for each | `لكل عنصر في قائمة` |
| `MatchStmt` | مطابقة أنماط | `طابق (قيمة) ... نهاية` |
| `ReturnStmt` | إرجاع | `ارجع قيمة` |
| `YieldStmt` | إنتاج (generator) | `أنتج قيمة` |
| `BreakStmt` | توقف | `توقف` |
| `ContinueStmt` | استمر | `استمر` |
| `BlockStmt` | كتلة | `{ ... }` |
| `TryStmt` | try/catch | `حاول ... امسك ... نهاية` |
| `RaiseStmt` | رمي استثناء | `ارمي خطأ` |
| `WithStmt` | context manager | `باستخدام ملف ... نهاية` |

#### عقد التصريحات (Declarations)

| العقدة | الوصف |
|--------|-------|
| `FunctionDecl` | تصريح دالة |
| `ClassDecl` | تصريح صنف |
| `FieldDecl` | تصريح حقل |
| `MethodDecl` | تصريح طريقة |
| `PropertyDecl` | تصريح خاصية (get/set) |
| `ConstructorDecl` | تصريح باني |
| `DestructorDecl` | تصريح هدام |
| `EnumDecl` | تصريح تعداد |
| `StructDecl` | تصريح بنية |
| `TraitDecl` | تصريح سمة |
| `ImplDecl` | تصريح تنفيذ سمة |
| `TemplateFunctionDecl` | دالة قالب |
| `TemplateClassDecl` | صنف قالب |
| `NamespaceDecl` | فضاء أسماء |
| `OperatorDecl` | تحميل عامل |
| `ImportStmt` | استيراد |
| `FromImportStmt` | استيراد انتقائي |
| `ExportStmt` | تصدير |
| `TestDecl` | تعريف اختبار |

#### عقد OOP

| العقدة | الوصف |
|--------|-------|
| `ClassDeclStmt` | تصريح صنف كامل |
| `NewExpr` | إنشاء كائن جديد |
| `MemberAccessExpr` | وصول لخاصية |
| `MemberAssignExpr` | إسناد لخاصية |
| `MethodCallExpr` | استدعاء طريقة |
| `ThisExpr` | مرجع ذاتي (هذا) |
| `SuperExpr` | مرجع للأساس |

#### عقد Pattern Matching

| العقدة | الوصف | مثال |
|--------|-------|------|
| `LiteralPattern` | نمط حرفي | `عندما 42:` |
| `WildcardPattern` | نمط شامل | `عندما _:` |
| `VariablePattern` | نمط متغير | `عندما س:` (يربط القيمة) |
| `ListPattern` | نمط قائمة | `عندما [أ، ب]:` |
| `OrPattern` | بدائل | `عندما 1 \|\| 2:` |
| `RangePattern` | نطاق | `عندما 1..10:` |
| `BindingPattern` | ربط مع نمط | `عندما ن @ 1..10:` |

### 🎯 نمط الزائر (Visitor Pattern)

```cpp
class ASTVisitor {
public:
    // تعابير
    virtual void visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr& expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr& expr) = 0;
    virtual void visitCallExpr(CallExpr& expr) = 0;
    // ... 40+ دالة visit أخرى
    
    // جمل
    virtual void visitIfStmt(IfStmt& stmt) = 0;
    virtual void visitWhileStmt(WhileStmt& stmt) = 0;
    // ... إلخ
    
    // تصريحات
    virtual void visitFunctionDecl(FunctionDecl& decl) = 0;
    virtual void visitClassDecl(ClassDecl& decl) = 0;
    // ... إلخ
};
```

---

## 4️⃣ نظام الأنواع (Types) — 4,137 سطر

### 📁 هيكل الملفات

| الملف | الأسطر | الدور |
|-------|--------|-------|
| `value.h/cpp` | 1,679 | فئة Value الأساسية |
| `class_type.h/cpp` | 1,164 | تعريف أنواع الأصناف |
| `object_instance.h/cpp` | 699 | نسخة كائن |
| `generator.h/cpp` | 330 | دعم Generators |
| `function_ref.h` | 164 | مرجع دالة |
| `type.h` | 61 | تعريفات أنواع |
| `data_types.h` | 40 | تعداد DataType |

### 📦 ValueType Enum

```cpp
enum class ValueType {
    VOID,       // فراغ (لاشيء)
    INTEGER,    // رقم صحيح (int64_t)
    DOUBLE,     // رقم عشري (double)
    STRING,     // نص (std::string)
    BOOLEAN,    // منطقي (bool)
    ARRAY,      // مصفوفة (vector<Value>)
    MAP,        // قاموس (unordered_map<string, Value>)
    OBJECT,     // كائن (shared_ptr<ObjectInstance>)
    FUNCTION    // دالة (shared_ptr<FunctionRef>)
};
```

### 🔧 فئة Value

**البنية الداخلية:**
```cpp
class Value {
    ValueType type_;
    std::variant<
        std::monostate,              // VOID
        int64_t,                     // INTEGER
        double,                      // DOUBLE
        std::string,                 // STRING
        bool,                        // BOOLEAN
        ArrayType,                   // ARRAY (vector<Value>)
        MapType,                     // MAP (unordered_map<string, Value>)
        ObjectPtr,                   // OBJECT (shared_ptr<ObjectInstance>)
        FunctionRefPtr               // FUNCTION (shared_ptr<FunctionRef>)
    > data_;
};
```

**الميزات الرئيسية:**
- ✅ تمرير الكائنات بالمرجع عبر `shared_ptr`
- ✅ دعم الدوال كقيم من الدرجة الأولى
- ✅ نسخ عميق عبر `clone()`
- ✅ تحويل تلقائي بين الأنواع
- ✅ دعم الـ Generators

### 📊 DataType Enum (للتحليل وقت الترجمة)

```cpp
enum class DataType {
    UNKNOWN,    // غير معروف
    INTEGER,    // رقم
    FLOAT,      // عشري
    STRING,     // نص
    BOOLEAN,    // منطقي
    NONE,       // لاشيء
    ARRAY,      // مصفوفة
    MAP,        // قاموس
    TUPLE,      // ثنائي/ثلاثي
    FUNCTION,   // دالة
    OBJECT,     // كائن
    ENUM,       // تعداد
    BYTE,       // بايت
    ERROR       // خطأ
};
```

---

## 5️⃣ مشاكل محتملة وتحسينات مقترحة

### 🔴 مشاكل محتملة

| # | المشكلة | الموقع | الخطورة |
|---|---------|--------|---------|
| 1 | تعارض `VOID` مع Windows macro | `value.h` | ⚡ متوسطة (معالجة عبر `#undef`) |
| 2 | تعارض `ERROR` مع Windows macro | `data_types.h` | ⚡ متوسطة (معالجة عبر `#undef`) |
| 3 | بعض أنواع الرموز لا تُنتج من Lexer | `token.h` | ⚠️ منخفضة (للتوافق) |
| 4 | تكرار تعريف بعض العقد | `declarations.h` vs `class_nodes.h` | ⚠️ منخفضة |
| 5 | حجم `parser/` كبير (29K سطر) | `parser/` | ⚠️ صيانة |

### 🟢 تحسينات مقترحة

1. **تقسيم المحلل النحوي:**
   - فصل `parser_advanced.cpp` (1,830 سطر) إلى ملفات أصغر
   - إنشاء `pattern_matching_parser.cpp` منفصل

2. **توحيد التعريفات:**
   - نقل جميع تعريفات OOP إلى `class_nodes.h`
   - إزالة التكرار بين `declarations.h` و `class_nodes.h`

3. **تحسين الوثائق:**
   - إضافة أمثلة كود لكل عقدة AST
   - توثيق أولويات العوامل بشكل رسمي

4. **إضافة اختبارات:**
   - اختبارات وحدة لكل نوع رمز
   - اختبارات تكامل للمحلل النحوي

---

## 📈 ملخص

| المقياس | القيمة |
|---------|--------|
| إجمالي الأسطر | **65,768** |
| أنواع الرموز | **~120** |
| الكلمات المحجوزة | **40** |
| العوامل المنطقية | **3** |
| الكلمات السياقية | **~20** |
| أنواع البيانات المدمجة | **9** |
| أنواع منخفضة المستوى | **11** |
| عقد AST | **~70+** |
| أنواع القيم (Runtime) | **9** |

---

---

## 6️⃣ المفسر (interpreter_new/) — 47,103 سطر

### 📁 هيكل المجلد

```
interpreter_new/
├── include/
│   ├── core/
│   │   ├── interpreter_core.h      ← نقطة الدخول الرئيسية
│   │   └── builtin_module_registry.h
│   ├── visitors/
│   │   ├── expression_evaluator.h   ← تقييم التعابير
│   │   └── statement_executor.h     ← تنفيذ الجمل
│   ├── managers/                    ← إدارة المتغيرات والدوال والنطاقات
│   ├── error/                       ← معالجة الأخطاء
│   └── debug/                       ← أدوات التصحيح
└── src/
    ├── core/                        ← التنفيذ الأساسي
    ├── visitors/                    ← تنفيذ الزوار
    ├── builtins/                    ← الدوال المدمجة (~20 ملف)
    ├── oop/                         ← دعم البرمجة الكائنية
    └── managers/                    ← مديري الموارد
```

### 🔧 المكونات الرئيسية

| المكون | الملفات | الأسطر | الدور |
|--------|---------|--------|-------|
| **expression_evaluator_oop.cpp** | 1 | 1,733 | تقييم تعابير OOP |
| **statement_executor.cpp** | 1 | 1,568 | تنفيذ جميع الجمل |
| **expression_evaluator_core.cpp** | 1 | 1,223 | تقييم التعابير الأساسية |
| **expression_evaluator_calls.cpp** | 2 | 2,373 | استدعاء الدوال |
| **expression_evaluator_members.cpp** | 1 | 1,434 | الوصول للأعضاء |
| **builtin_registry_part*.cpp** | ~20 | ~15,000 | الدوال المدمجة |

### 📋 InterpreterOptions

```cpp
struct InterpreterOptions {
    bool enableDebugMode = false;       // وضع التصحيح
    bool enableStrictMode = false;      // الوضع الصارم
    bool printResults = false;          // طباعة النتائج
    size_t maxCallDepth = 1000;         // أقصى عمق استدعاء
    bool enableOwnership = false;       // نظام الملكية
    bool ownershipArabicMessages = true; // رسائل عربية
    bool enableTypeCheck = false;       // فحص الأنواع
    bool enableSecurity = false;        // نظام الأمان
    std::string currentFilePath;        // مسار الملف الحالي
};
```

### 🎯 واجهة الاستخدام

```cpp
using namespace Sad::Interpreter;

// إنشاء مفسر
Interpreter interpreter(InterpreterOptions{.enableDebugMode = true});

// تنفيذ برنامج
ExecutionResult result = interpreter.execute(programAST);

if (result.success) {
    std::cout << "النتيجة: " << result.result.toString() << std::endl;
} else {
    std::cerr << "خطأ: " << result.errorMessage << std::endl;
}
```

---

## 7️⃣ المترجم (compiler_new/) — 214,673 سطر

### 📁 هيكل المجلد

```
compiler_new/
├── src/
│   ├── sir/            ← 6,357 سطر - طبقة SIR الملكية
│   ├── llvm/           ← 28,406 سطر - تكامل LLVM
│   ├── backend/        ← 17,278 سطر - توليد الكود
│   ├── frontend/       ← 17,254 سطر - واجهة أمامية
│   ├── types/          ← 14,360 سطر - نظام أنواع متقدم
│   ├── jit/            ← 6,632 سطر - ترجمة JIT
│   ├── bytecode/       ← 6,449 سطر - تجميع بايت كود
│   ├── semantic/       ← 6,375 سطر - تحليل دلالي
│   ├── middle/         ← 6,043 سطر - تحسينات وسطية
│   ├── targets/        ← 5,971 سطر - منصات متعددة
│   └── ...             ← 99,548 سطر - مكونات أخرى
├── include/            ← ملفات الرؤوس
├── runtime/            ← وقت التشغيل
├── tests/              ← اختبارات
└── benchmarks/         ← اختبارات أداء
```

### 🔄 خط أنابيب الترجمة (Pipeline)

```
مصدر .ص → Lexer → Parser → AST
                              ↓
              SIRBuilder (AST → SIR)
                              ↓
              SIROptimizer (تحسينات)
                              ↓
              SIRValidator (فحص الملكية)
                              ↓
              LLVMCodeGen (SIR → LLVM IR)
                              ↓
              LLVM Backend (LLVM IR → Machine Code)
                              ↓
                         ملف تنفيذي
```

### ⚙️ طبقة SIR (Sad Intermediate Representation)

**12 عملية ملكية أساسية:**

| # | العملية | الوصف | المثال |
|---|---------|-------|--------|
| 1 | `Alloc` | تخصيص ذاكرة | `%r = Alloc(نوع)` |
| 2 | `Borrow` | استعارة ثابتة | `%r = Borrow(%v)` |
| 3 | `BorrowMut` | استعارة متغيرة | `%r = BorrowMut(%v)` |
| 4 | `Move` | نقل الملكية | `%r = Move(%v)` |
| 5 | `Copy` | نسخ القيمة | `%r = Copy(%v)` |
| 6 | `Drop` | تحرير الذاكرة | `Drop(%v)` |
| 7 | `Clone` | استنساخ عميق | `%r = Clone(%v)` |
| 8 | `EndBorrow` | إنهاء الاستعارة | `EndBorrow(%ref)` |
| 9 | `Reborrow` | إعادة استعارة | `%r = Reborrow(%ref)` |
| 10 | `Project` | الوصول لحقل | `%r = Project(%v, "حقل")` |
| 11 | `Deref` | فك المرجع | `%r = Deref(%ref)` |
| 12 | `Take` | أخذ من حاوية | `%r = Take(%arr, 0)` |

**عمليات إضافية:**
- **ذاكرة:** Load, Store, StackAlloc, HeapAlloc, Free
- **حسابية:** Add, Sub, Mul, Div, Mod, Neg
- **مقارنة:** Eq, Ne, Lt, Le, Gt, Ge
- **منطقية:** And, Or, Not, Xor, BitAnd, BitOr, Shl, Shr
- **تحكم:** Jump, Branch, Switch, Return, Unreachable
- **دوال:** Call, CallIndirect, CallMethod
- **أنواع:** Cast, TypeCheck, Sizeof

**عمليات منخفضة المستوى (لنظام التشغيل):**
- **ذاكرة مباشرة:** RawLoad8/16/32/64, RawStore8/16/32/64
- **منافذ I/O:** PortIn8/16/32, PortOut8/16/32
- **المعالج:** Cli, Sti, Hlt, Pause, Nop, Cpuid
- **سجلات:** Rdmsr, Wrmsr, ReadCr0-4, WriteCr0-4
- **واصفات:** Lgdt, Lidt, Lldt, Ltr, Sgdt, Sidt
- **ذرية:** AtomicLoad, AtomicStore, AtomicAdd, AtomicCmpXchg
- **مقاطعات:** Int, Iret, SaveContext, RestoreContext

### 🎯 المنصات المستهدفة

| المنصة | الحالة | الملاحظات |
|--------|--------|----------|
| x86-64 Windows | ✅ كامل | المنصة الرئيسية |
| x86-64 Linux | ✅ كامل | مدعوم |
| ARM64 | 🔄 جزئي | قيد التطوير |
| WebAssembly | 🔄 جزئي | عبر LLVM |
| RISC-V | ⏳ مخطط | للمستقبل |

---

## 📊 ملخص إحصائي شامل

| المكون | الأسطر | النسبة من الإجمالي |
|--------|--------|-------------------|
| **shared/** (النواة) | 65,768 | 20.1% |
| **interpreter_new/** (المفسر) | 47,103 | 14.4% |
| **compiler_new/** (المترجم) | 214,673 | 65.5% |
| **الإجمالي** | **327,544** | 100% |

### 🏗️ البنية العامة للمشروع

```
┌─────────────────────────────────────────────────────────────┐
│                     لغة ص (Sad Language)                    │
├─────────────────────────────────────────────────────────────┤
│  shared/ (65K)          │  نواة مشتركة: lexer, parser, AST │
├─────────────────────────┼───────────────────────────────────┤
│  interpreter_new/ (47K) │  مفسر شجري → تنفيذ مباشر         │
│                         │  ↓ AST → Value                   │
├─────────────────────────┼───────────────────────────────────┤
│  compiler_new/ (215K)   │  مترجم → ملف تنفيذي              │
│                         │  AST → SIR → LLVM → Native       │
└─────────────────────────┴───────────────────────────────────┘
```

---

**تاريخ آخر تحديث:** 8 مارس 2026  
**أُنشئ بواسطة:** GitHub Copilot (Claude Opus 4.5)
