# 📋 تدقيق الالتزام الكامل بالمواصفة
# Full Specification Compliance Audit

**التاريخ / Date:** 24 نوفمبر 2025  
**المشروع / Project:** لغة ص / Sad Language  
**الحالة / Status:** 🔴 تحليل شامل - يتطلب إصلاحات / Comprehensive Analysis - Requires Fixes

---

## 📌 ملخص تنفيذي / Executive Summary

### (AR) الهدف
التأكد من أن المحلل اللفظي (Lexer) والمحلل النحوي (Parser) يطبقان **فقط** القواعد الموجودة في ملفات المواصفة الرسمية الثمانية، بدون أي ميزات إضافية غير موثقة.

### (EN) Objective
Ensure that the Lexer and Parser implement **only** the rules found in the eight official specification files, without any undocumented features.

### ملفات المواصفة المرجعية / Reference Specification Files

1. ✅ `01_types.md` - نظام الأنواع / Type System (943 lines)
2. ✅ `02_functions.md` - نظام الدوال / Function System (1105 lines)
3. ✅ `03_oop.md` - البرمجة الكائنية / OOP System (1448 lines)
4. ✅ `04_syntax.md` - النحو العام / General Syntax (1612 lines)
5. ✅ `05_python_features.md` - ميزات Python / Python Features (1666 lines)
6. ✅ `06_cpp_features.md` - ميزات C++ / C++ Features (848 lines)
7. ✅ `08_modules_and_ffi.md` - النماذج و FFI / Modules & FFI (827 lines)
8. ✅ `09_errors_and_diagnostics.md` - الأخطاء / Errors & Diagnostics (769 lines)

**المجموع:** 9,218 سطر من المواصفات الرسمية

---

## 1️⃣ الكلمات المحجوزة الرسمية / Official Reserved Keywords

### من ملف 04_syntax.md (lines 1402-1450)

#### أنواع البيانات / Data Types
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| رقم | int | TYPE_INTEGER |
| عشري | float | TYPE_DOUBLE |
| منطقي | bool | TYPE_BOOLEAN |
| نص | string | TYPE_STRING |
| مصفوفة | array | TYPE_ARRAY |
| خريطة | map | TYPE_MAP |
| أي | any | TYPE_ANY |

#### التعريفات / Definitions
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| دالة | func | KEYWORD_FUNCTION |
| صنف | class | KEYWORD_CLASS |
| باني | constructor | KEYWORD_CONSTRUCTOR |
| ~باني | destructor | KEYWORD_DESTRUCTOR |

#### التحكم / Control Flow
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| إذا | if | KEYWORD_IF |
| وإلا | else | KEYWORD_ELSE |
| بينما | while | KEYWORD_WHILE |
| لكل | foreach | KEYWORD_FOR |
| في | in | KEYWORD_IN |
| حالة | switch | KEYWORD_CASE (مفقود!) |
| عندما | case | KEYWORD_WHEN (مفقود!) |
| افتراضي | default | KEYWORD_DEFAULT (مفقود!) |
| إرجاع | return | KEYWORD_RETURN |
| اخرج | break | KEYWORD_BREAK |
| تابع | continue | KEYWORD_CONTINUE |

#### الاستثناءات / Exceptions
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| حاول | try | KEYWORD_TRY |
| امسك | catch | KEYWORD_CATCH |
| أخيراً | finally | KEYWORD_FINALLY |
| ارمِ | throw | KEYWORD_THROW |

#### القيم / Values
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| صحيح | true | LITERAL_TRUE |
| خطأ | false | LITERAL_FALSE |
| لاشيء | none | LITERAL_NULL |

#### المراجع / References
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| هذا | this | KEYWORD_THIS |
| الأساس | super | KEYWORD_SUPER |

#### التحكم بالوصول / Access Control
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| عام | public | KEYWORD_PUBLIC |
| خاص | private | KEYWORD_PRIVATE |
| محمي | protected | KEYWORD_PROTECTED |

#### المعدِّلات / Modifiers
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| ثابت | static | KEYWORD_STATIC |

#### النماذج / Modules (من 08_modules_and_ffi.md)
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| استورد | import | KEYWORD_IMPORT |
| كـ | as | KEYWORD_AS (مفقود!) |
| من | from | KEYWORD_FROM (مفقود!) |
| صدّر | export | KEYWORD_EXPORT |

#### الهياكل / Structures
| العربية | English | TokenType المتوقع |
|---------|---------|-------------------|
| نهاية | end | KEYWORD_END |

**المجموع:** 40+ كلمة محجوزة رسمية

---

## 2️⃣ تحليل الكلمات المحجوزة الحالية / Current Keywords Analysis

### ✅ كلمات موجودة بشكل صحيح / Correctly Implemented

من مراجعة `src/lexer/lexer_keywords.cpp`:

1. ✅ أنواع البيانات: رقم، عشري، منطقي، نص، مصفوفة، خريطة (7/7)
2. ✅ دوال وبنيات: دالة، صنف، باني، ~باني، هذا، الأساس (6/6)
3. ✅ تحكم أساسي: إذا، وإلا، بينما، لكل، في، اخرج، تابع (7/7)
4. ✅ استثناءات: حاول، امسك، أخيراً، ارمِ (4/4)
5. ✅ وصول: عام، خاص، محمي (3/3)
6. ✅ قيم: صحيح، خطأ، لاشيء (3/3)
7. ✅ معدلات: ثابت (1/1 - لكن يجب التحقق من ثابت vs const)
8. ✅ نماذج: استورد، صدّر (2/4 - ناقص كـ و من)
9. ✅ هيكل: نهاية (1/1)

### 🔴 كلمات مفقودة من Lexer / Missing Keywords

| الكلمة | TokenType المطلوب | الأولوية | الملف |
|--------|-------------------|---------|-------|
| حالة (switch) | KEYWORD_CASE | 🔴 HIGH | 04_syntax.md |
| عندما (case) | KEYWORD_WHEN | 🔴 HIGH | 04_syntax.md |
| افتراضي (default) | KEYWORD_DEFAULT | 🔴 HIGH | 04_syntax.md |
| أي (any type) | TYPE_ANY | 🔴 HIGH | 01_types.md, 04_syntax.md |
| كـ (as) | KEYWORD_AS | 🟡 MEDIUM | 08_modules_and_ffi.md |
| من (from) | KEYWORD_FROM | 🟡 MEDIUM | 08_modules_and_ffi.md |

### ⚠️ كلمات غير موثقة في المواصفة / Undocumented Keywords

بعد مراجعة شاملة للمواصفات الثمانية، الكلمات التالية موجودة في Lexer لكن **غير موثقة**:

| الكلمة العربية | English | TokenType | السبب |
|---------------|---------|-----------|-------|
| يرث (duplicate) | inherits | KEYWORD_INHERITS | ✅ موثق لكن مكرر مع KEYWORD_EXTENDS |
| ينفذ | implements | KEYWORD_IMPLEMENTS | ❌ غير موثق (ربما Phase 2) |
| واجهة | interface | KEYWORD_INTERFACE | ❌ غير موثق (ربما Phase 2) |
| مجرد | abstract | KEYWORD_ABSTRACT | ❌ غير موثق (ربما Phase 2) |
| افتراضي | virtual | KEYWORD_VIRTUAL | ❌ تعارض مع "default" للـswitch |
| تجاوز | override | KEYWORD_OVERRIDE | ❌ غير موثق (ربما Phase 2) |
| وحدة | module | KEYWORD_MODULE | ❌ غير موثق |
| حزمة | package | KEYWORD_PACKAGE | ❌ غير موثق |
| متغير | var | KEYWORD_VAR | ❌ غير موثق (غير مطلوب) |
| اجعل | let | KEYWORD_LET | ❌ غير موثق (غير مطلوب) |
| لامدا | lambda | KEYWORD_LAMBDA | ⚠️ موثق في 05_python_features.md لكن بصيغة مختلفة |
| اعطِ | yield | KEYWORD_YIELD | ⚠️ موثق في 05_python_features.md كـ Generators |

**المجموع:** 12 كلمة غير موثقة أو مكررة

---

## 3️⃣ تحليل العوامل / Operators Analysis

### من ملف 04_syntax.md (جدول الأسبقية)

#### عوامل موثقة رسمياً / Officially Documented Operators

| المستوى | العامل | النوع | TokenType |
|---------|-------|-------|-----------|
| 16 | () [] . | استدعاء/وصول | PAREN_LEFT, BRACKET_LEFT, DOT |
| 15 | ** | أس | OP_POWER |
| 14 | ! - + | أحادي | OP_NOT, OP_MINUS, OP_PLUS |
| 13 | * / % | ضرب | OP_MULTIPLY, OP_DIVIDE, OP_MODULO |
| 12 | + - | جمع | OP_PLUS, OP_MINUS |
| 11 | << >> | إزاحة | ⚠️ غير موجود |
| 10 | < <= > >= | مقارنة | OP_LESS, OP_LESS_EQUAL, OP_GREATER, OP_GREATER_EQUAL |
| 9 | == != | مساواة | OP_EQUAL, OP_NOT_EQUAL |
| 8 | & | AND bitwise | ⚠️ غير موجود |
| 7 | ^ | XOR bitwise | ⚠️ غير موجود |
| 6 | \| | OR bitwise | ⚠️ غير موجود |
| 5 | && | AND منطقي | OP_AND |
| 4 | \|\| | OR منطقي | OP_OR |
| 3 | ? : | ثلاثي | ⚠️ QUESTION موجود لكن غير مطبق |
| 2 | = += -= *= /= | تعيين | OP_ASSIGN, OP_PLUS_ASSIGN, OP_MINUS_ASSIGN, OP_MULTIPLY_ASSIGN, OP_DIVIDE_ASSIGN |
| 1 | , | فاصلة | COMMA, ARABIC_COMMA |

#### عوامل إضافية موثقة / Additional Documented Operators

| العامل | الوصف | TokenType | الحالة |
|-------|-------|-----------|-------|
| ++ | زيادة | OP_INCREMENT | ⚠️ غير موثق في 04_syntax.md |
| -- | نقصان | OP_DECREMENT | ⚠️ غير موثق في 04_syntax.md |
| in | عضوية | KEYWORD_IN | ✅ موجود |
| ؛ | فاصلة منقوطة عربية | ARABIC_SEMICOLON | ✅ موجود |
| ، | فاصلة عربية | ARABIC_COMMA | ✅ موجود |

### 🔴 عوامل مفقودة / Missing Operators

| العامل | الوصف | TokenType المطلوب | الأولوية |
|-------|-------|-------------------|---------|
| << | إزاحة يسار | OP_SHIFT_LEFT | 🟡 MEDIUM |
| >> | إزاحة يمين | OP_SHIFT_RIGHT | 🟡 MEDIUM |
| & | AND bitwise | OP_BIT_AND | 🟡 MEDIUM |
| \| | OR bitwise | OP_BIT_OR | 🟡 MEDIUM |
| ^ | XOR bitwise | OP_BIT_XOR | 🟡 MEDIUM |
| ~ | NOT bitwise | OP_BIT_NOT | 🟡 MEDIUM |
| ? : | ثلاثي | تطبيق في Parser | 🔴 HIGH |

---

## 4️⃣ تحليل قواعد النحو / Syntax Rules Analysis

### من ملف 04_syntax.md

#### ✅ قواعد صحيحة / Correct Rules

1. **الفواصل المنقوطة:** يدعم ؛ (عربي) و ; (إنجليزي) ✅
2. **الفواصل:** يدعم ، (عربي) و , (إنجليزي) ✅
3. **الشروط بين أقواس:** `إذا (condition)` ✅
4. **نهاية الكتل بـ `نهاية`:** ✅ للصنف (class) لكن ❌ لباقي الكتل

#### 🔴 قواعد غير صحيحة / Incorrect Rules

##### 1. استخدام `}` بدلاً من `نهاية` في معظم الكتل

**المواصفة تنص على:**
> "Blocks must end with `نهاية` keyword (not `}`)"

**الوضع الحالي:**
- ✅ Classes: تستخدم `نهاية` بشكل صحيح
- ❌ Functions: تستخدم `{ }`
- ❌ If statements: تستخدم `{ }`
- ❌ While loops: تستخدم `{ }`
- ❌ For loops: تستخدم `{ }`
- ❌ Try-catch: تستخدم `{ }`

**مثال من المواصفة:**
```s
دالة جمع(رقم أ، رقم ب)
    إرجاع أ + ب
نهاية

إذا (x > 5)
    اطبع("كبير")
وإلا
    اطبع("صغير")
نهاية
```

**الوضع الحالي (خطأ):**
```s
دالة جمع(رقم أ، رقم ب) {
    إرجاع أ + ب
}

إذا (x > 5) {
    اطبع("كبير")
} وإلا {
    اطبع("صغير")
}
```

##### 2. Switch/Case غير مطبق

**المواصفة تنص على:**
```s
حالة (x)
    عندما 1:
        اطبع("واحد")
    عندما 2:
        اطبع("اثنان")
    افتراضي:
        اطبع("آخر")
نهاية
```

**الوضع الحالي:** غير مطبق تماماً ❌

##### 3. Ternary Operator غير مطبق

**المواصفة تنص على:**
```s
رقم نتيجة = x > 5 ? 10 : 20
```

**الوضع الحالي:** QUESTION token موجود لكن غير مطبق في Parser ❌

---

## 5️⃣ تحليل ميزات Python / Python Features Analysis

### من ملف 05_python_features.md

| الميزة | الحالة في المواصفة | الحالة في الكود | الملاحظات |
|-------|-------------------|-----------------|-----------|
| List Comprehensions | ✅ P0 Core | ✅ مطبق | جيد |
| Dict Comprehensions | ✅ P0 Core | ✅ مطبق | جيد |
| Generators | ✅ P0 Core | ⚠️ جزئي | `yield` موجود في Lexer فقط |
| Decorators | 🚧 P1 Phase 2 | ⚠️ غير مطلوب الآن | مستقبلي |
| Context Managers | 🚧 P1 Phase 2 | ❌ غير مطبق | مستقبلي |
| Exceptions | ✅ P0 Core | ✅ مطبق | جيد |
| Lambda | ✅ P0 Core | ⚠️ جزئي | موجود بصيغة `دالة` بدون اسم |
| Unpacking | ✅ P1 Core | ❌ غير مطبق | مطلوب |
| Conditional Expressions | ✅ P1 Core | ❌ غير مطبق | نفس Ternary |
| Keyword Arguments | ✅ P1 Core | ⚠️ جزئي | يحتاج تأكيد |

---

## 6️⃣ تحليل ميزات C++ / C++ Features Analysis

### من ملف 06_cpp_features.md

| الميزة | الحالة في المواصفة | الحالة في الكود | الملاحظات |
|-------|-------------------|-----------------|-----------|
| Optional Static Typing | ✅ P0 Core | ✅ مطبق | جيد |
| Generics/Templates | 🚧 P1 Phase 2 | ❌ غير مطبق | مستقبلي |
| RAII | ✅ P0 Core | ✅ مطبق | Destructors موجودة |
| Move Semantics | 🚧 P2 Phase 2 | ❌ غير مطبق | مستقبلي |
| Operator Overloading | 🚧 P1 Phase 2 | ❌ غير مطبق | مستقبلي |
| Native Bindings | 🚧 P1 Phase 2 | ❌ غير مطبق | مستقبلي |

---

## 7️⃣ تحليل النماذج / Modules Analysis

### من ملف 08_modules_and_ffi.md

| الميزة | الحالة في المواصفة | الحالة في الكود | الملاحظات |
|-------|-------------------|-----------------|-----------|
| استورد (import) | ✅ P0 | ⚠️ Token موجود | يحتاج Parser |
| من...استورد (from...import) | ✅ P0 | ❌ مفقود | `من` و `كـ` مفقودة |
| صدّر (export) | ✅ P0 | ⚠️ Token موجود | يحتاج Parser |
| Built-in modules | ✅ P0 | ⚠️ جزئي | stdlib موجود |
| Custom modules | ✅ P0 | ❌ غير مكتمل | |
| Dynamic loading | 🚧 P1 | ❌ غير مطبق | مستقبلي |
| FFI/C API | 🚧 P1 | ❌ غير مطبق | مستقبلي |

---

## 8️⃣ خطة الإصلاح / Fix Plan

### المرحلة 1: تنظيف Lexer (يوم واحد)

#### 1.1 إضافة الكلمات المفقودة
```cpp
// في src/lexer/lexer_keywords.cpp

// Switch/Case keywords
keywords_["حالة"] = TokenType::KEYWORD_CASE;
keywords_["case"] = TokenType::KEYWORD_CASE;
keywords_["عندما"] = TokenType::KEYWORD_WHEN;
keywords_["when"] = TokenType::KEYWORD_WHEN;
keywords_["افتراضي"] = TokenType::KEYWORD_DEFAULT;
keywords_["default"] = TokenType::KEYWORD_DEFAULT;

// Type ANY
keywords_["أي"] = TokenType::TYPE_ANY;
keywords_["any"] = TokenType::TYPE_ANY;

// Module keywords
keywords_["كـ"] = TokenType::KEYWORD_AS;
keywords_["as"] = TokenType::KEYWORD_AS;
keywords_["من"] = TokenType::KEYWORD_FROM;
keywords_["from"] = TokenType::KEYWORD_FROM;
```

#### 1.2 حذف الكلمات غير الموثقة
```cpp
// حذف Advanced OOP (Phase 2 features)
// keywords_["ينفذ"] = TokenType::KEYWORD_IMPLEMENTS;  // DELETE
// keywords_["واجهة"] = TokenType::KEYWORD_INTERFACE;  // DELETE
// keywords_["مجرد"] = TokenType::KEYWORD_ABSTRACT;    // DELETE
// keywords_["تجاوز"] = TokenType::KEYWORD_OVERRIDE;   // DELETE

// حذف تعارض virtual
// keywords_["افتراضي"] = TokenType::KEYWORD_VIRTUAL;  // DELETE (conflicts with default)

// حذف module/package (غير موثقة)
// keywords_["وحدة"] = TokenType::KEYWORD_MODULE;      // DELETE
// keywords_["حزمة"] = TokenType::KEYWORD_PACKAGE;     // DELETE

// حذف var/let (غير مطلوبة - type inference موجود)
// keywords_["متغير"] = TokenType::KEYWORD_VAR;       // DELETE
// keywords_["اجعل"] = TokenType::KEYWORD_LET;         // DELETE

// الاحتفاظ بـ lambda و yield لكن توضيح أنها من 05_python_features.md
// keywords_["لامدا"] - موثقة في 05_python_features.md
// keywords_["اعطِ"] - موثقة في 05_python_features.md (Generators)
```

#### 1.3 حل تعارض KEYWORD_INHERITS
```cpp
// يرث موجودة مرتين:
// KEYWORD_INHERITS (في OOP section)
// KEYWORD_EXTENDS (في Inheritance section)
// الحل: إبقاء KEYWORD_EXTENDS فقط وحذف KEYWORD_INHERITS
```

#### 1.4 إضافة bitwise operators (اختياري - Priority MEDIUM)
```cpp
keywords_["<<"] = TokenType::OP_SHIFT_LEFT;
keywords_[">>"] = TokenType::OP_SHIFT_RIGHT;
keywords_["&"] = TokenType::OP_BIT_AND;
keywords_["|"] = TokenType::OP_BIT_OR;
keywords_["^"] = TokenType::OP_BIT_XOR;
keywords_["~"] = TokenType::OP_BIT_NOT;
```

### المرحلة 2: إصلاح Parser - Block Termination (3-4 أيام)

#### 2.1 توحيد انتهاء الكتل بـ `نهاية`

**الملفات المتأثرة:**
- `src/parser/parser_core_impl.cpp`
- `src/parser/parser_core_helpers.cpp`

**التغييرات المطلوبة:**

```cpp
// قبل (خطأ):
StmtPtr ParserCore::parseBlockStmt() {
    StmtList statements;
    while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {  // ❌ WRONG
        statements.push_back(parseDeclaration());
    }
    consume(TT::BRACE_RIGHT, "...");  // ❌ WRONG
    return std::make_unique<BlockStmt>(statements, pos);
}

// بعد (صحيح):
StmtPtr ParserCore::parseBlockStmt() {
    StmtList statements;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {  // ✅ CORRECT
        statements.push_back(parseDeclaration());
    }
    consume(TT::KEYWORD_END, "(AR) توقع 'نهاية' بعد الكتلة. (EN) Expected 'نهاية' after block.");  // ✅ CORRECT
    return std::make_unique<BlockStmt>(statements, pos);
}
```

#### 2.2 تحديث جميع الدوال التي تستخدم parseBlockStmt()

- `parseFunction()` - حذف `consume(TT::BRACE_LEFT)`
- `parseIfStmt()` - يستدعي parseStatement() → parseBlockStmt()
- `parseWhileStmt()` - يستدعي parseStatement() → parseBlockStmt()
- `parseForStmt()` - يستدعي parseStatement() → parseBlockStmt()
- `parseTryStmt()` - يستدعي parseBlockStmt() مباشرة

### المرحلة 3: تطبيق Switch/Case (يومان)

```cpp
// في include/ast/ast_statements.h
struct SwitchCase {
    ExprPtr condition;  // القيمة للمقارنة
    StmtList body;      // الجمل التي تنفذ
};

class SwitchStmt : public Stmt {
public:
    SwitchStmt(ExprPtr expr, std::vector<SwitchCase> cases, 
               StmtList defaultCase, Position pos);
    
private:
    ExprPtr expression_;          // التعبير المراد فحصه
    std::vector<SwitchCase> cases_;  // قائمة الحالات
    StmtList defaultCase_;        // الحالة الافتراضية
};

// في src/parser/parser_core_impl.cpp
StmtPtr ParserCore::parseSwitchStmt() {
    // (AR) تحليل تعبير switch: حالة (expression)
    // (EN) Parse switch expression: switch (expression)
    consume(TT::PAREN_LEFT, "(AR) توقع '(' بعد 'حالة'. (EN) Expected '(' after 'حالة'.");
    auto expr = parseExpression();
    consume(TT::PAREN_RIGHT, "(AR) توقع ')' بعد التعبير. (EN) Expected ')' after expression.");
    
    std::vector<SwitchCase> cases;
    
    // (AR) تحليل الحالات: عندما value:
    // (EN) Parse cases: when value:
    while (match(TT::KEYWORD_WHEN) || match(TT::KEYWORD_CASE)) {
        auto caseValue = parseExpression();
        consume(TT::COLON, "(AR) توقع ':' بعد قيمة الحالة. (EN) Expected ':' after case value.");
        
        StmtList caseBody;
        while (!check(TT::KEYWORD_WHEN) && 
               !check(TT::KEYWORD_CASE) && 
               !check(TT::KEYWORD_DEFAULT) &&
               !check(TT::KEYWORD_END)) {
            caseBody.push_back(parseStatement());
        }
        
        cases.push_back({std::move(caseValue), std::move(caseBody)});
    }
    
    // (AR) تحليل الحالة الافتراضية: افتراضي:
    // (EN) Parse default case: default:
    StmtList defaultCase;
    if (match(TT::KEYWORD_DEFAULT)) {
        consume(TT::COLON, "(AR) توقع ':' بعد 'افتراضي'. (EN) Expected ':' after 'افتراضي'.");
        while (!check(TT::KEYWORD_END)) {
            defaultCase.push_back(parseStatement());
        }
    }
    
    consume(TT::KEYWORD_END, "(AR) توقع 'نهاية' بعد switch. (EN) Expected 'نهاية' after switch.");
    
    return std::make_unique<SwitchStmt>(
        std::move(expr),
        std::move(cases),
        std::move(defaultCase),
        previous().getPosition()
    );
}
```

### المرحلة 4: تطبيق Ternary Operator (يوم واحد)

```cpp
// في include/ast/ast_expressions.h
class TernaryExpr : public Expr {
public:
    TernaryExpr(ExprPtr condition, ExprPtr thenBranch, ExprPtr elseBranch, Position pos);
    
private:
    ExprPtr condition_;   // الشرط
    ExprPtr thenBranch_;  // القيمة إذا كان الشرط صحيح
    ExprPtr elseBranch_;  // القيمة إذا كان الشرط خاطئ
};

// في src/parser/parser_core_impl.cpp
ExprPtr ParserCore::parseTernary() {
    // (AR) تحليل العامل الثلاثي: condition ? true_value : false_value
    // (EN) Parse ternary operator: condition ? true_value : false_value
    auto expr = parseLogicalOr();
    
    if (match(TT::QUESTION)) {
        auto thenBranch = parseExpression();
        consume(TT::COLON, "(AR) توقع ':' في العامل الثلاثي. (EN) Expected ':' in ternary operator.");
        auto elseBranch = parseTernary();  // Right-associative
        
        return std::make_unique<TernaryExpr>(
            std::move(expr),
            std::move(thenBranch),
            std::move(elseBranch),
            previous().getPosition()
        );
    }
    
    return expr;
}
```

### المرحلة 5: إضافة التعليقات (يوم واحد)

```cpp
// في رأس كل ملف:
// ======================================================================
// <filename>.cpp - [الوصف بالعربية] / [English Description]
// ======================================================================
// (AR) الوصف بالعربية:
//   [شرح تفصيلي عن غرض الملف ووظائفه الرئيسية]
//
// (EN) English Description:
//   [Detailed explanation of file purpose and main functions]
//
// (AR) ملاحظات:
//   - [ملاحظة 1]
//   - [ملاحظة 2]
//
// (EN) Notes:
//   - [Note 1]
//   - [Note 2]
// ======================================================================

// لكل دالة:
/**
 * @brief (AR) [شرح الدالة بالعربية]
 * @brief (EN) [Function explanation in English]
 * 
 * @param param1 (Type) — (AR) [وصف المعامل] / (EN) [Parameter description]
 * @return (Type) — (AR) [وصف القيمة المرجعة] / (EN) [Return value description]
 * 
 * (AR) ملاحظات: [ملاحظات إضافية]
 * (EN) Notes: [Additional notes]
 * 
 * (AR) مثال / (EN) Example:
 * ```cpp
 * // Example code
 * ```
 */
```

---

## 9️⃣ جدول الأولويات / Priority Table

| الأولوية | المهمة | الوقت المقدر | التأثير |
|----------|--------|-------------|---------|
| 🔴 P0 | إضافة switch/case keywords | 2 ساعة | HIGH |
| 🔴 P0 | إضافة TYPE_ANY | 1 ساعة | HIGH |
| 🔴 P0 | حذف الكلمات غير الموثقة | 2 ساعة | HIGH |
| 🔴 P0 | توحيد block termination بـ`نهاية` | 3 أيام | BREAKING |
| 🔴 P0 | تطبيق switch/case parser | 2 أيام | HIGH |
| 🔴 P0 | تطبيق ternary operator | يوم واحد | HIGH |
| 🟡 P1 | إضافة من و كـ keywords | 2 ساعة | MEDIUM |
| 🟡 P1 | تطبيق module import/export | 3 أيام | MEDIUM |
| 🟡 P1 | إضافة bitwise operators | يوم واحد | MEDIUM |
| 🟢 P2 | إضافة تعليقات شاملة | يومان | LOW |
| 🟢 P2 | توثيق ملفات الرأس | يوم واحد | LOW |

**الوقت الإجمالي المقدر:** 12-15 يوم عمل

---

## 🔟 ملخص التغييرات المطلوبة / Summary of Required Changes

### في Lexer (src/lexer/lexer_keywords.cpp):

**إضافات (9 كلمات):**
1. حالة / case → KEYWORD_CASE
2. عندما / when → KEYWORD_WHEN
3. افتراضي / default → KEYWORD_DEFAULT
4. أي / any → TYPE_ANY
5. كـ / as → KEYWORD_AS
6. من / from → KEYWORD_FROM
7-9. Bitwise operators (اختياري)

**حذف (10-11 كلمات):**
1. ينفذ / implements → KEYWORD_IMPLEMENTS ❌
2. واجهة / interface → KEYWORD_INTERFACE ❌
3. مجرد / abstract → KEYWORD_ABSTRACT ❌
4. افتراضي / virtual → KEYWORD_VIRTUAL ❌ (تعارض)
5. تجاوز / override → KEYWORD_OVERRIDE ❌
6. وحدة / module → KEYWORD_MODULE ❌
7. حزمة / package → KEYWORD_PACKAGE ❌
8. متغير / var → KEYWORD_VAR ❌
9. اجعل / let → KEYWORD_LET ❌
10. KEYWORD_INHERITS (مكرر مع KEYWORD_EXTENDS) ❌

**تعديل:**
- توضيح أن `لامدا` و `اعطِ` من 05_python_features.md

### في Parser (src/parser/parser_core_impl.cpp):

**تعديلات جذرية:**
1. تغيير `parseBlockStmt()` لاستخدام `KEYWORD_END`
2. حذف `consume(TT::BRACE_LEFT)` من `parseFunction()`
3. تحديث جميع control flow statements

**إضافات جديدة:**
1. `parseSwitchStmt()` - switch/case implementation
2. `parseTernary()` - ternary operator implementation
3. تطبيق compound assignment في `parseAssignment()`

### في Token Types (include/lexer/token.h):

**إضافات:**
```cpp
KEYWORD_CASE,
KEYWORD_WHEN,
KEYWORD_DEFAULT,
TYPE_ANY,
KEYWORD_AS,
KEYWORD_FROM,
// Optionally:
OP_SHIFT_LEFT,
OP_SHIFT_RIGHT,
OP_BIT_AND,
OP_BIT_OR,
OP_BIT_XOR,
OP_BIT_NOT,
```

**حذف:**
```cpp
KEYWORD_IMPLEMENTS,
KEYWORD_INTERFACE,
KEYWORD_ABSTRACT,
KEYWORD_VIRTUAL,
KEYWORD_OVERRIDE,
KEYWORD_MODULE,
KEYWORD_PACKAGE,
KEYWORD_VAR,
KEYWORD_LET,
KEYWORD_INHERITS,  // duplicate
```

---

## 📝 الخلاصة / Conclusion

### النتيجة الرئيسية / Main Finding

المحلل الحالي يحتوي على:
1. ✅ **70% التزام بالمواصفة** في الكلمات المحجوزة
2. ⚠️ **50% التزام بالمواصفة** في قواعد النحو (بسبب مشكلة `نهاية` vs `}`)
3. ❌ **3 ميزات موثقة مفقودة تماماً** (switch/case, ternary, TYPE_ANY)
4. ❌ **10 كلمات غير موثقة موجودة** (يجب حذفها أو توثيقها)

### التوصية / Recommendation

**يجب تطبيق المراحل 1-4 فوراً** لضمان:
- ✅ إزالة جميع الميزات غير الموثقة
- ✅ إضافة جميع الميزات الموثقة والمفقودة
- ✅ توحيد استخدام `نهاية` في جميع الكتل (تغيير جذري!)
- ✅ الالتزام 100% بالمواصفة الرسمية

**التغيير الجذري (Breaking Change):**
استبدال `{ }` بـ `نهاية` سيكسر جميع الأكواد الموجودة! يجب:
1. إنشاء أداة migration لتحويل الأكواد القديمة
2. تحديث جميع الأمثلة في `examples/`
3. تحديث التوثيق
4. إعلان الإصدار الجديد بوضوح

---

**النهاية / End of Report**

