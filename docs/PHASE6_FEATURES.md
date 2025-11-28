# المرحلة 6: Switch/Case و Ternary Operator
# Phase 6: Switch/Case and Ternary Operator

## نظرة عامة / Overview

تضيف المرحلة 6 ميزتين أساسيتين للغة:
Phase 6 adds two essential features to the language:

1. **جملة Switch/Case** - للتحكم متعدد الفروع
   **Switch/Case Statement** - For multi-way branching
   
2. **عامل ثلاثي شرطي** - للتعبيرات الشرطية المختصرة
   **Ternary Conditional Operator** - For compact conditional expressions

---

## 1. جملة Switch/Case Statement

### البنية / Syntax

```
حالة (<expression>)
    عندما <value1>: <statement1>
    عندما <value2>: <statement2>
    ...
    [افتراضي: <default_statement>]
نهاية
```

### القواعد النحوية / Grammar

```
switch_stmt → KEYWORD_CASE "(" expr ")"
              (KEYWORD_WHEN expr ":" stmt)*
              [KEYWORD_DEFAULT ":" stmt]
              KEYWORD_END
```

### الميزات الرئيسية / Key Features

1. **الشرط بين أقواس / Parenthesized Condition**
   - الشرط يجب أن يكون داخل أقواس `()` 
   - Condition must be enclosed in parentheses `()`
   - مثال: `حالة (يوم)` ✅ صحيح
   - Example: `حالة يوم` ❌ خطأ

2. **عدم وجود Fall-through / No Fall-through**
   - ينفذ أول حالة مطابقة فقط ثم يخرج
   - Executes only the first matching case then exits
   - لا حاجة لـ `break` بعد كل حالة
   - No need for `break` after each case

3. **الحالة الافتراضية اختيارية / Optional Default Case**
   - يمكن إضافة `افتراضي:` لمعالجة القيم غير المطابقة
   - Can add `افتراضي:` to handle non-matching values

4. **القيم يمكن أن تكون تعبيرات / Values Can Be Expressions**
   - قيم الحالات يمكن أن تكون أرقام، نصوص، أو تعبيرات
   - Case values can be numbers, strings, or expressions

### أمثلة / Examples

#### مثال بسيط / Simple Example
```s
رقم يوم = 3

حالة (يوم)
    عندما 1: اطبع("الإثنين")
    عندما 2: اطبع("الثلاثاء")
    عندما 3: اطبع("الأربعاء")
    افتراضي: اطبع("يوم آخر")
نهاية
```
**Output:** `الأربعاء`

#### مثال مع النصوص / Example with Strings
```s
نص اللون = "أحمر"

حالة (اللون)
    عندما "أحمر": اطبع("Red")
    عندما "أزرق": اطبع("Blue")
    عندما "أخضر": اطبع("Green")
    افتراضي: اطبع("Unknown color")
نهاية
```
**Output:** `Red`

#### مثال بدون default / Example without Default
```s
رقم القيمة = 5

حالة (القيمة)
    عندما 1: اطبع("واحد")
    عندما 2: اطبع("اثنان")
نهاية

// لا يطبع شيء لأن القيمة = 5 ولا توجد حالة افتراضية
// Prints nothing since value = 5 and no default case
```

#### مثال مع جمل متعددة / Example with Multiple Statements
```s
رقم المستوى = 2

حالة (المستوى)
    عندما 1: {
        اطبع("مبتدئ")
        اطبع("Level 1")
    }
    عندما 2: {
        اطبع("متوسط")
        اطبع("Level 2")
    }
    افتراضي: {
        اطبع("متقدم")
        اطبع("Advanced")
    }
نهاية
```

### التطبيق الداخلي / Internal Implementation

#### AST Layer
- **Class:** `SwitchStmt` في `statements.h`
- **Components:**
  - `ExprPtr expression` - التعبير الذي يُقيّم
  - `std::vector<CaseBranch> cases` - قائمة الحالات
  - `StmtPtr defaultCase` - الحالة الافتراضية (اختيارية)

#### Parser Layer
- **Function:** `parseSwitchStmt()` في `parser_core_impl.cpp`
- **Steps:**
  1. استهلاك `حالة`
  2. استهلاك `(`
  3. تحليل التعبير
  4. استهلاك `)`
  5. تحليل الحالات (`عندما`)
  6. تحليل الحالة الافتراضية (إن وُجدت)
  7. استهلاك `نهاية`

#### Interpreter Layer
- **Function:** `visitSwitchStmt()` في `statement_executor.cpp`
- **Logic:**
  1. تقييم تعبير switch
  2. المقارنة مع كل case حتى إيجاد تطابق
  3. تنفيذ جسم الحالة المطابقة
  4. الخروج (لا fall-through)
  5. إذا لم يوجد تطابق، تنفيذ default (إن وُجد)

---

## 2. العامل الثلاثي الشرطي / Ternary Conditional Operator

### البنية / Syntax

```
<condition> ? <true_value> : <false_value>
```

### القواعد النحوية / Grammar

```
ternary → logical_or ("?" expression ":" ternary)?
```

### الميزات الرئيسية / Key Features

1. **Right-Associative / ارتباط أيمن**
   - `a ? b : c ? d : e` يُقرأ كـ `a ? b : (c ? d : e)`
   - Allows nested ternary expressions

2. **Short-circuit Evaluation / تقييم قصير الدائرة**
   - يُقيّم فقط الفرع الذي سيُستخدم
   - Only evaluates the branch that will be used
   - يحسّن الأداء ويمنع آثار جانبية غير ضرورية
   - Improves performance and prevents unnecessary side effects

3. **Type Inference / استنتاج النوع**
   - النوع النهائي يُحدد من أنواع الفرعين
   - Final type determined from both branches
   - إذا أحدهما `FLOAT`، النتيجة `FLOAT`
   - If either is `FLOAT`, result is `FLOAT`

### أمثلة / Examples

#### مثال بسيط / Simple Example
```s
رقم س = 10
نص النتيجة = س > 5 ? "كبير" : "صغير"
اطبع(النتيجة)
```
**Output:** `كبير`

#### مثال مع الأعمار / Example with Ages
```s
رقم العمر = 16
نص الحالة = العمر >= 18 ? "بالغ" : "قاصر"
اطبع(الحالة)
```
**Output:** `قاصر`

#### مثال متداخل / Nested Example
```s
رقم القيمة = 0
نص الوصف = القيمة > 0 ? "موجب" : القيمة < 0 ? "سالب" : "صفر"
اطبع(الوصف)
```
**Output:** `صفر`

**تحليل التداخل / Nesting Analysis:**
```
القيمة > 0 ? "موجب" : (القيمة < 0 ? "سالب" : "صفر")
     false           →  (0 < 0 ? "سالب" : "صفر")
                            false     →  "صفر"
```

#### مثال مع أرقام / Example with Numbers
```s
رقم أ = 15
رقم ب = 20
رقم الأكبر = أ > ب ? أ : ب
اطبع("الأكبر =", الأكبر)
```
**Output:** `الأكبر = 20`

#### مثال في تعبير / Example in Expression
```s
رقم السعر = 100
رقم الخصم = السعر > 50 ? 0.2 : 0.1
رقم السعر_النهائي = السعر * (1 - الخصم)
اطبع("السعر النهائي:", السعر_النهائي)
```
**Output:** `السعر النهائي: 80`

### التطبيق الداخلي / Internal Implementation

#### Lexer Layer
- **Token:** `QUESTION` (`?`) في `token.h`
- **Added:** دعم رمز `?` في `lexer_core.cpp`

#### AST Layer
- **Class:** `TernaryExpr` في `expressions.h`
- **Components:**
  - `ExprPtr condition` - الشرط
  - `ExprPtr trueExpr` - القيمة عند الصحة
  - `ExprPtr falseExpr` - القيمة عند الخطأ

#### Parser Layer
- **Function:** `parseTernary()` في `parser_core_impl.cpp`
- **Steps:**
  1. تحليل logical OR
  2. إذا وُجد `?`
  3. تحليل true expression
  4. استهلاك `:`
  5. تحليل false expression بشكل تكراري (للتداخل)

#### Interpreter Layer
- **Function:** `visitTernaryExpr()` في `expression_evaluator.cpp`
- **Logic:**
  1. تقييم condition
  2. إذا `true`: تقييم trueExpr فقط
  3. إذا `false`: تقييم falseExpr فقط
  4. النتيجة في `lastResult_`

---

## الاختلافات عن C/C++ / Differences from C/C++

### Switch/Case

| الميزة / Feature | لغة ص / Sad | C/C++ |
|---|---|---|
| الأقواس / Parentheses | إلزامي `حالة (x)` | إلزامي `switch(x)` |
| Fall-through | ❌ لا يوجد / No | ✅ موجود / Yes |
| Break | 🚫 غير مطلوب / Not needed | ✅ مطلوب / Required |
| نهاية الكتلة / Block end | `نهاية` keyword | `}` brace |
| Default | اختياري `افتراضي:` | اختياري `default:` |

### Ternary Operator

| الميزة / Feature | لغة ص / Sad | C/C++ |
|---|---|---|
| البنية / Syntax | `cond ? true : false` | `cond ? true : false` |
| التداخل / Nesting | ✅ مدعوم / Supported | ✅ مدعوم / Supported |
| الارتباط / Associativity | أيمن / Right | أيمن / Right |
| Short-circuit | ✅ نعم / Yes | ✅ نعم / Yes |

---

## ملفات الاختبار / Test Files

### Switch Tests
- `test_simple_switch.s` - اختبار switch بسيط
- `test_switch_default.s` - اختبار مع default
- `test_switch.s` - اختبار شامل

### Ternary Tests
- `test_ternary_num.s` - اختبار مع أرقام
- `test_nested_ternary.s` - اختبار متداخل

---

## الأداء / Performance

### Switch/Case
- **المقارنة:** تستخدم `Value::equals()` للمقارنة
- **التعقيد:** O(n) حيث n عدد الحالات
- **التحسين المستقبلي:** يمكن استخدام hash table للقيم الثابتة

### Ternary
- **Short-circuit:** يُقيّم فرع واحد فقط
- **التعقيد:** O(1) للتقييم
- **الذاكرة:** لا توجد ذاكرة إضافية

---

## المراجع / References

- **Specification:** `docs/language_spec/rules/04_syntax.md`
- **AST Definitions:** `include/parser/ast/expressions.h`, `statements.h`
- **Parser Implementation:** `src/parser/parser_core_impl.cpp`
- **Interpreter:** `src/interpreter/visitors/`

---

## تاريخ الإنجاز / Completion History

- **تاريخ البدء / Start Date:** 2025-11-25
- **تاريخ الإنجاز / Completion Date:** 2025-11-25
- **الحالة / Status:** ✅ مكتمل / Complete
- **الاختبارات / Tests:** ✅ تم الاختبار بنجاح / Successfully tested

---

## الخطوات التالية / Next Steps

المرحلة 7: التوثيق الشامل والأمثلة
Phase 7: Comprehensive documentation and examples
