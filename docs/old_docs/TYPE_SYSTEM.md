**نظام الأنواع في لغة ص (S Language) — شرح للمبرمجين**

مقدمة
- **الهدف:** وثيقة شاملة تشرح نموذج أنواع اللغة "ص" للمبرمجين، مضمنة وصفاً مفصلاً لكيفية تمثيل القيم، قواعد التحقق من النوع وقت التشغيل والوقت التحليلي، وتحويلات الأنواع والسلوك المتوقَّع.
- **نطاق:** تغطي المستويات التالية: مُحلّل اللغة (Parser)، تمثيلات القيم في زمن التنفيذ (Interpreter/VM)، وأنواع الكائنات والـ GC.

1. لمحة عامة
- لغة ص تدعم نظام أنواع مُزدوج الملامح: على مستوى الـ AST/Parser هناك تمثيل لأنواع بيانات صريحة (typed annotations) مثل `رقم`/`int`، `عشري`/`float`، `نص`/`string`، `منطقي`/`bool`، وأنواع مركبة مثل `مصفوفة`/`array` و`قاموس`/`map`، مع دعم لقراءة معاملات عامة (generics) مثل `Array<int>` و `Map<string, float>`.
- زمن التنفيذ يعتمد نموذج قيم مُوسوم (tagged union) — كل قيمة تحمل حقل نوع (enum) وبيانٍ/مرجع يُخزن البيانات الفعلية. هناك فصل واضح بين الأنواع البسيطة (primitives) والأنواع المرجعية (objects) التي تُدار بواسطة GC.

2. تمثيل القيم (Runtime Value Representation)
- كل قيمة بداخل الآلة الافتراضية تمثَّل عبر هيكل `Value` يحتوي على `ValueType` (مثلاً: `VAL_INT`, `VAL_FLOAT`, `VAL_STRING`, `VAL_ARRAY`, `VAL_OBJECT`, `VAL_FUNCTION`, `VAL_CLOSURE`, `VAL_NATIVE`, `VAL_POINTER`, إلخ).
- البيانات الفعلية مخزنة في اتحاد (union) أو `std::variant` عبر الطبقات المختلفة: أعداد صحيحة 64-بت، أعداد عشرية 64-بت، منطقية، مؤشرات لكائنات heap (StringObject, ArrayObject, MapObject, FunctionObject, ClosureObject, NativeObject).
- الأنواع المرجعية تُشتق من `Object` الأساسي الذي يحتوي على بيانات ضرورية للـ GC (لون/علامة، مؤشر للسلسلة المرتبطة، نوع الكائن).

3. النموذج الهرمي للأنواع
- أنواع بدائية: `رقم`/`int`، `عشري`/`float`، `نص`/`string`، `منطقي`/`bool`، `فراغ`/`void` (أو `عدم`/`null`).
- أنواع مركبة: `مصفوفة`/`array<T>` و`قاموس`/`map<K,V>` (يُقرأ المعامل العام حاليًا لكن قد لا يُخزَّن بالكامل حتى الآن في الـ AST).
- أنواع لغة الكائنات/الأصناف: `object` و `class`، مع دعم للوراثة والحقول والدوال.
- أنواع الدوال: `function` و `closure`، و `native` للدوال المعرَّفة في C++.

4. التحقق من النوع (Type Checking)
- التحليل (Parsing/AST): المحلل يقرأ أنماط الأنواع والتعليقات التوضيحية للوسائط وأنواع الإرجاع ويُنشئ تمثيلاً ابتدائيًا (`Data::DataType`) مثل `INTEGER`, `FLOAT`, `STRING`, `BOOLEAN`, `ARRAY`, `MAP`, `OBJECT`, `UNKNOWN`.
- وقت التجميع/التحليل الإضافي: بعض التحققات مثل وجود صنف معرف أو تطابق أسماء الأنواع تتم في مراحل لاحقة (ClassManager و Type resolution). حالياً تحليل معاملات الأنواع العامة يُقرأ لكنه لا يُستخدم بالكامل لفحص صارم (حسب ملاحظات الشيفرة).
- وقت التشغيل: يستخدم الـ VM/Interpreter فحوصًا ديناميكية (مثال: `isInt()`, `isString()`, `isObject()`) ويرمي أخطاء تنفيذية عند محاولة عمليات غير مناسبة (مثل قسمة string أو الوصول لخاصية على non-object).

5. تحويلات الأنواع والسلوك
- توجد طرق تحويل صريحة/ضمنية بين الأنواع الأساسية: تحويل نص إلى رقم يحاول `std::stoi`/`std::stod`، التحويل بين `int` و `float` تلقائيًا في العمليات الحسابية.
- القواعد العامة:
  - عند وجود `int` و `float` معاً في عملية حسابية، النتيجة تكون `float`.
  - عملية جمع بين نص وغير نص تقوم بتحويل الطرف الآخر إلى نص ودمجه.
  - عمليات منطقية تعتمد على `toBool()` الذي يعتبر `null`/`void` كقيمة كاذبة، والكائنات فارغة كمؤشرات صحيحة/خاطئة حسب محتواها.
- بعض التحويلات تثير استثناءات (مثلاً: محاولة تحويل `void` إلى عدد، أو قسمة على صفر).

6. استدعاء الدوال والقابلية للاستدعاء
- القيم القابلة للاستدعاء: `function`, `closure`, `native`.
- الدوال المجمعة تحتوي على بايتكود وعداد معاملات (arity). الإغلاقات تحتفظ بمؤشرات `upvalue` لالتقاط المتغيرات الخارجية.

7. إدارة الذاكرة وGC
- الأنواع المرجعية تُخزن في heap وتُدير بواسطة جامع قمامة ثلاثي الألوان (tri-color mark-and-sweep).
- كل كائن يرث `Object` ويحتفظ بحالة اللون/العلامة أثناء عملية الجمع.

8. التوافق بين طبقات النظام
- هناك فصل بين نموذج البيانات في الطبقة العليا (`Data::Value`، `Data::DataType`, `Type` class) وبين تمثيل القيم في الـ VM (`VM::Value`, `VM::ValueType`, `ObjectType`). عند تحويل AST إلى بايتكود أو تنفيذ تفسيري، تتم خرائط بين أنواع الـ AST ووسوم الـ VM.

9. نصائح للمبرمجين
- افترض أن التحقق الصارم للأنواع العامّة (generics) غير مكتمل؛ لا تعتمد على فحص compile-time كامل لهيئات `Array<T>`/`Map<K,V>` حتى إشعار آخر.
- استخدم الحرس (guards) في وقت التشغيل: افحص `isString()`, `isInt()`, `isObject()` قبل العمليات الحرجة.
- عند إضافة نوع جديد: حدّث كلا من `parser::parseType()`, `include/data/types/*`، و `vm/include/value.h` مع خرائط التحويل الملائمة واسم النوع في دوال الطباعة.

-- أمثلة (15 مثالاً مع شرح موجز):

1) إعلان متغير عدد صحيح:

```s
let x: رقم = 42;    // Arabic: رقم == int
```

شرح: `x` مُعلَن بنوع `رقم` ويخزن قيمة صحيحة 64-بت.

2) تحويل ضمني بين `رقم` و `عشري`:

```s
let a: رقم = 3;
let b: عشري = 2.5;
let c = a + b; // نتيجة: عشري (5.5)
```

3) نصوص وعمليات الربط:

```s
let s = نص("مرحبا");
let t = " عالم";
let r = s + t; // "مرحبا عالم"
```

4) مصفوفة عامة وقراءة معامل النوع (syntax):

```s
let arr: مصفوفة<رقم> = [1,2,3];
let x = arr[0]; // 1
```

5) قاموس/خريطة مع مفاتيح نصية:

```s
let m: قاموس<نص, رقم> = { "a": 10, "b": 20 };
let v = m["a"]; // 10
```

6) دالة مع توقيع منوع:

```s
func add(x: رقم, y: رقم) -> رقم {
    return x + y;
}
let z = add(5, 7); // 12
```

7) إغلاق (closure) يلتقط متغيراً:

```s
func makeAdder(n: رقم) -> function {
    return (x: رقم) => x + n;
}
let inc = makeAdder(1);
let v = inc(10); // 11
```

8) استدعاء دالة أصلية (native):

```s
// printf-like native registered by runtime
native print(msg: نص);
print("مرحبا من native");
```

9) مقارنة بين int و float:

```s
let a = 2;     // رقم
let b = 2.0;   // عشري
let eq = (a == b); // true (numeric comparison)
```

10) خطأ وقت التنفيذ عند تحويل غير صالح:

```s
let s = "abc";
let n: رقم = s; // محاولة تحويل string->int: ستُطرَح استثناء في وقت التنفيذ
```

11) فحص نوع قبل الوصول لخاصية:

```s
let v = maybeGetValue();
if (v.isObject()) {
    // آمن الحصول على خاصية
    let p = v.prop;
}
```

12) استخدام null/void:

```s
let n = عدم; // null
if (!n) { /* يعتبر false */ }
```

13) تمرير مصفوفة كمرجع (runtime semantics):

```s
let a = [1,2,3];
let b = a; // يشير إلى نفس الكائن المرجعي (shared) ما لم يتم نسخها صراحة
b[0] = 9; // a[0] == 9
```

14) تعريف صنف وتحقّق النوعية البسيطة:

```s
class Point { x: رقم; y: رقم; }
let p = new Point();
p.x = 10;
```

15) مثال على نوع عام (توضيحي):

```s
func length<T>(arr: مصفوفة<T>) -> رقم {
    return arr.length();
}
let l = length([1,2,3]); // 3
```

ملاحظة: دعم التحقق الكامل للـ generics قد لا يكون مكتملًا في هذه المرحلة؛ يُقرأ المعامل العام لكن قد لا يُطبق فحص صارم في كل الأماكن.

=========================================================

**Type System in S Language — Developer Guide (English)**

Introduction
- Purpose: This document explains the S language type model for developers, covering value representations, type checking (both parse-time and runtime), conversions and expected behavior.
- Scope: Parser-level typed annotations, runtime VM/Interpreter value representations, object types and GC.

1. Overview
- S supports annotated data types at the AST/Parser level: `int`/`رقم`, `float`/`عشري`, `string`/`نص`, `bool`/`منطقي`, and composite types `array` and `map` with basic parsing for generics like `Array<int>` and `Map<string, float>`.
- Runtime uses a tagged-union value model: each value carries a `ValueType` tag and data. Primitives are distinct from heap-allocated reference types managed by GC.

2. Runtime Value Representation
- VM `Value` structure uses `ValueType` enum (`VAL_INT`, `VAL_FLOAT`, `VAL_STRING`, `VAL_ARRAY`, `VAL_OBJECT`, `VAL_FUNCTION`, `VAL_CLOSURE`, `VAL_NATIVE`, `VAL_POINTER`, etc.).
- Data stored in union/variant: 64-bit ints, 64-bit floats, booleans, and pointers to heap objects (`StringObject`, `ArrayObject`, `MapObject`, `FunctionObject`, `ClosureObject`, `NativeObject`).

3. Type Hierarchy
- Primitive types: `int`, `float`, `string`, `bool`, `void`/`null`.
- Composite types: `array<T>`, `map<K,V>` (parser reads generic parameters; full checking may be partial at this stage).
- Class/object types with inheritance, fields and methods.
- Function types: `function`, `closure`, and `native`.

4. Type Checking
- Parsing/AST: parser reads type annotations into `Data::DataType` (e.g., `INTEGER`, `FLOAT`, `STRING`, `BOOLEAN`, `ARRAY`, `MAP`, `OBJECT`, `UNKNOWN`).
- Semantic analysis: resolves class names and type references via managers (e.g., `ClassManager`). Full generic/type inference is a future enhancement.
- Runtime: interpreter/VM performs dynamic checks using helpers like `isInt()`, `isString()`, `isObject()`, and raises runtime errors for invalid operations.

5. Conversions and Semantics
- Conversions between primitives are supported: string→int/float via `stoi`/`stod`, int/float mixed arithmetic promotes to float.
- Concatenation with strings converts the other operand to string.
- `toBool()` rules: `null`/`void` → false; numbers compare to zero; empty strings/containers are falsey; objects are truthy if non-empty.

6. Calls & Callables
- Callable values: `function`, `closure`, `native`.
- Functions contain bytecode and arity; closures capture upvalues for lexical variables.

7. Memory Management & GC
- Reference types allocated on heap; GC uses tri-color mark-and-sweep; `Object` base stores GC metadata.

8. Cross-layer Consistency
- There exist mappings between AST-level types (`Data::DataType` and `Type` classes) and the VM-level `ValueType`/`ObjectType`. When extending types, keep both layers in sync.

9. Developer Notes
- Do not assume full compile-time generic checking yet.
- Guard runtime operations with `isX()` checks.
- When adding new types, update parser, Data type definitions, and VM `value.h` mappings and type-name utilities.

Examples (same 15 examples in English with short notes):

1) Integer variable declaration:

```s
let x: int = 42;
```

2) Implicit int/float promotion:

```s
let a: int = 3;
let b: float = 2.5;
let c = a + b; // 5.5 (float)
```

3) Strings and concatenation:

```s
let s = string("hello");
let t = " world";
let r = s + t; // "hello world"
```

4) Generic array and indexing:

```s
let arr: array<int> = [1,2,3];
let x = arr[0]; // 1
```

5) Map/dictionary:

```s
let m: map<string, int> = { "a": 10, "b": 20 };
let v = m["a"]; // 10
```

6) Function with types:

```s
func add(x: int, y: int) -> int { return x + y; }
let z = add(5, 7); // 12
```

7) Closure capturing an outer variable:

```s
func makeAdder(n: int) -> function { return (x: int) => x + n; }
let inc = makeAdder(1);
let v = inc(10); // 11
```

8) Native function call:

```s
native print(msg: string);
print("hello from native");
```

9) Numeric equality between int and float:

```s
let a = 2; let b = 2.0;
let eq = (a == b); // true
```

10) Invalid conversion runtime error:

```s
let s = "abc";
let n: int = s; // runtime conversion error
```

11) Type guard before property access:

```s
let v = maybeGetValue();
if (v.isObject()) { let p = v.prop; }
```

12) Null/void usage:

```s
let n = null;
if (!n) { /* false branch */ }
```

13) Array reference semantics:

```s
let a = [1,2,3];
let b = a; // shares reference
b[0] = 9; // a[0] == 9
```

14) Class example:

```s
class Point { x: int; y: int; }
let p = new Point();
p.x = 10;
```

15) Generic function sketch:

```s
func length<T>(arr: array<T>) -> int { return arr.length(); }
let l = length([1,2,3]); // 3
```

Closing remarks
- The document provides a technical summary of the S language type system, based on repository artifacts (parser, data types, VM value). For further extension consider adding a section on type inference and explicit mapping documentation between `Data::Type` and `VM::ValueType`.
