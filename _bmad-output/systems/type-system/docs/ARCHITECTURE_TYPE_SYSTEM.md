# وثيقة المعمارية: نظام الأنواع الموحد (SadTypeKind)

> **حالة التحقّق (مكتمل ومدموج في `graphic` — `3dc75af4`، 2026-06-15):** اكتمل القلب المعماريّ كاملًا:
> **الاستبدال الكامل DataType→SadTypeKind** (P2/P2.5 — `DataType` محذوف نهائيًّا، grep=0)؛
> **Null متمايز** (P1)؛ **المدقّق موحَّد** (P2)؛ **Result** (P3)؛ **سطح Optional `رقم?`/`رقم؟`** (P4)؛
> **ربط Future** (P4 — `نوع(مستقبل())`=«مستقبل»)؛ **فرض أمان null في المفسّر** (P9)؛ **interning** (P7)؛
> **codegen-null في المترجم** (`نوع(لاشيء)`=«عدم» مزدوج)؛ **توحيد `نوع()` عبر `typeof_ar`** (مصدر واحد للمحرّكين).
> المُرحَّل: تضييق التدفّق (P10) → نظام null-safety المستقلّ؛ مولّدات Generator الكسولة (وسمٌ مؤجَّل).
> انظر [التقرير النهائي](../status/FINAL_REPORT_2026-06-15.md) و[خطّة الإكمال](./TYPE_SYSTEM_COMPLETION_PLAN.md) و`status/implementation_status.md`.

## 📋 نظرة عامة

**الهدف الأساسي**: توحيد نظام الأنواع في لغة ص من عدّة أنظمة منفصلة إلى نظام واحد موحّد (`Types::SadTypeKind`).

**الحالة الفعلية (مدعومة بالكود):**
- ✅ إزالة `enum Data::ValueType` القديم من المسار النشط (بقي فقط في `archived/` و`sad_ui/`). البديل ثوابت توافقية `constexpr SadTypeKind` في [value.h:83-99](../../../../shared/types/include/value.h#L83-L99).
- ✅ `Value` يخزّن النوع كـ`Types::SadTypeKind` + `SadTypePtr` ([value.h:236-246](../../../../shared/types/include/value.h#L236)).
- ✅ المترجم يستورد `Sad::Types::SadTypeKind` (`compiler/include/types/type.h:33`, `sir_types.h:46`) — أنجح جزء في التوحيد.
- ✅ **المدقّق الدلالي موحَّد** على SadTypeKind (S-TS-P2): `grep "Data::DataType::"` = 0؛ و`DataType` محذوف من المشروع كلّه (P2.5a/b).
- ◑ **الأنواع المتقدّمة**: Result مطبَّق+مختبَر (P3)؛ Optional له سطح `رقم?`+إسناد (P4)؛ Future/Generator أصنافها موجودة (ربط runtime جزئيّ — متبقٍّ).
- ✅ **نوع `Null` متمايز** (S-TS-P1): `نوع(لاشيء)`=«عدم»؛ وفرض أمان null في المفسّر (P9).

---

## 🏛️ المشكلة الأصلية

قبل التوحيد كانت هناك أنظمة نوع مستقلّة متعدّدة (ValueType القديم في المفسّر، DataType في الـAST، SIRValueType في المترجم) أدّت إلى:
- ❌ تحويلات متعدّدة بين الأنظمة = أخطاء محتملة وفقدان معلومات.
- ❌ صعوبة إضافة أنواع جديدة وصيانة الكود.

---

## ✨ الحل: النظام الموحّد

### الطبقة 1️⃣: **`SadTypeKind` enum** — العدد الفعلي **52 قيمة**

**الملف**: [shared/types/generated/sad_type_kind_generated.h](../../../../shared/types/generated/sad_type_kind_generated.h) (مُولَّد من `language-truth/types.yaml`)

> ✅ تصحيح اتّساق (S-TS-P0/P1): العدّ الموحَّد الآن **52 قيمة** (49 + Null في P1 + Point/Rect في P11). تعليق الترويسة في `sad_type_system.h:16,55-56` صُحِّح إلى 52، والعدد يُولَّد آليًّا من YAML فلا انحراف بنيويًّا.

**القيم الـ52 الفعلية (مُولَّدة من المصدر):**

```
البدائيات والعددية:
  Void, Integer, Float, Boolean, String, Byte,
  Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64,
  Float32, Float64, Char

المركّبة:
  Array, Map, Tuple, Slice, Vector

الكائنية/التصنيفية:
  Class, Struct, Enum, Trait, Function, Closure

الجبرية/العامة:
  Union, Intersection, Optional, Result, Generic,
  TypeParameter, TypeAlias

المؤشّرات والمراجع:
  Pointer, Reference, MutableRef

الخاصّة:
  Any, Never, Unknown, Error, Null

غير المتزامن/المولّدات:
  Future, Generator, Comprehension

واجهة المستخدم (GUI):
  Color, Widget, Window, Event, Point, Rect
```

> `Null` مُضاف في S-TS-P1 (عدم متمايز عن فراغ). `Qubit` مؤرشف فقط
> (`archived/experimental_types/qubit_type.cpp`) ولا قيمة enum له؛ `URL` غير موجود.

### تدقيق الاستهلاك (S-TS-P6) — لا قيمة ميتة

تدقيق grep لكل القيم الـ52 عبر `interpreter`/`compiler`/`shared`/`vm`/`tools`
(عدا الترويسة المُولَّدة). **النتيجة: كل قيمة مُستهلَكة (الحدّ الأدنى 4 مواضع) — صفر قيمة ميتة، فلا إزالة (AC2).**

| الفئة | القيم | الاستهلاك |
|------|------|:---------:|
| مُطبَّق بكثافة | Integer(1058)، Boolean(471)، Void(424)، String(420)، Float(303)، Array(183)، Pointer(173)، Unknown(101)، Class(85)، Function(67)، Map(62) | >60 |
| مُطبَّق | Struct(46)، Tuple(38)، Byte(23)، Any(23)، Error(19)، Union(17)، Enum/Never(16)، Optional(13)، Null(12)، Trait(12)، Closure(11) | 11–46 |
| مُطبَّق (SIMD) | **Vector(24)** — SIMD، **ليس ميتًا** (`builtins_simd.cpp`) | 24 |
| مُستهلَك (بنيوي/قليل) | Future(10)، Generator(9)، Generic(9)، Int8(8)، TypeParameter(8)، Float64/Char/Result/Reference/MutableRef/TypeAlias/Intersection/Float32/Int16-64(6)، Slice/UInt*(5) | 5–10 |
| مُستهلَك (واجهات/استيعاب) | Comprehension، Color، Widget، Window، Event | 4 لكلٍّ |

**ملاحظات تطبيق (تقاطع P3/P4):** `Result` نوعٌ مطبَّق + مختبَر (S-TS-P3)؛ `Optional` له سطح `رقم?` + إسناد (S-TS-P4)؛ `Future`/`Generator` أصنافها موجودة لكن **ربط الـruntime جزئيّ** (مخطّط — S-TS-P4 المتبقّي)، لا تُزال (مستهلَكة). لا قيمة تحتاج وسم «ميت».

### الطبقة 2️⃣: **هرمية `SadType`** — **17 صنفًا فرعيًّا فعليًّا** (لا 15)

**الأصناف الموجودة فعلًا** (من [sad_type_system.h](../../../../shared/types/include/sad_type_system.h)):

```cpp
SadType (الأساس)
├─ SadPrimitiveType   (517)  // يغطّي كل البدائيات والعددية — لا توجد IntType/FloatType منفصلة
├─ SadSpecialType     (555)  // Any/Never/Unknown/Error
├─ SadArrayType       (583)
├─ SadMapType         (630)
├─ SadTupleType       (684)
├─ SadFunctionType    (744)
├─ SadClassType       (832)
├─ SadEnumType        (889)
├─ SadTraitType       (925)
├─ SadUnionType       (959)
├─ SadOptionalType    (1018)
├─ SadGenericType     (1062)
├─ SadReferenceType   (1125)
├─ SadFutureType      (1174)
├─ SadGeneratorType   (1217)
└─ SadResultType      (S-TS-P3)  // نتيجة<T, E> — مُضاف
```

> البدائيات يغطّيها `SadPrimitiveType` الواحد (لا `SadIntType`/`SadFloatType`... منفصلة). `Qubit`/`URL` غير موجودة.
> **تحديث (S-TS-P3):** `Result` أصبح له صنف `SadResultType` + `makeResult` + إسناد تغايُري — لم يعد «قيمة ميتة».

### الطبقة 3️⃣: **`SadTypeRegistry` (Singleton)**

**الملف**: [sad_type_system.h:1272-1476](../../../../shared/types/include/sad_type_system.h#L1272)

المركز المركزي لإنشاء الأنواع.

> ✅ **تحديث (S-TS-P7):** interning صار يشمل الأنواع المركّبة (Array/Map/Tuple/Optional/Result)
> عبر `SadTypeRegistry::intern()` (مفتاح = التوقيع البنيوي) → **مقارنة المؤشر `==` صحيحة** للمتماثلة بنيويًّا.

---

## 🌉 طبقة الجسر (Type Bridge)

**الملف**: [shared/types/include/type_bridge.h](../../../../shared/types/include/type_bridge.h)

تحويلات `SadTypeKind ↔ ValueType` (+ runtime: `inferSadType`/`isValueCompatible`).

> ✅ **تحديث (S-TS-P2.5b):** أُزيلت دوال `DataType` الأربع من الجسر و`data_types.h` كاملًا
> (`grep DataType` في المشروع = 0). لم يعد ثمّة تحويل مُسقِط `Optional/Result → Void`؛
> النظام الأفقر `DataType` (14 قيمة) أُزيل نهائيًّا، والمحور الوحيد `SadTypeKind`.

### جدول المطابقة (SadTypeKind ↔ ValueType — البدائيات)

| SadTypeKind | ValueType |
|-------------|-----------|
| Void | VOID |
| Null | NULL (عدم — متمايز، S-TS-P1) |
| Integer | INTEGER |
| Float | DOUBLE |
| String | STRING |
| Boolean | BOOLEAN |
| Array | ARRAY |
| Map | MAP |
| Class | OBJECT |
| Function | FUNCTION |

---

## 🔄 مسار البيانات من المصدر إلى التنفيذ

```mermaid
graph LR
    A["📄 مصدر .ص<br/>دالة ف(س: رقم)"]
    B["🔍 Lexer"]
    C["📝 Parser → AST<br/>(SadTypeKind مباشرة)"]
    D["🎯 Type Inference<br/>getTypeKind()"]
    F["💾 Value<br/>(SadTypePtr محفوظ)"]
    G["🏃 Runtime / Codegen"]
    A --> B --> C --> D --> F --> G
```

> تحديث (S-TS-P2.5a): المحلّل يُنتج `SadTypeKind` مباشرة؛ لا عبور `DataType` (محذوف). المدقّق موحَّد على SadTypeKind (P2).

---

## 📊 حالة الأنواع المتقدّمة (تصنيف صادق)

| النوع | قيمة enum | صنف SadType | بناء في السجلّ | استخدام دلالي/تنفيذي | التصنيف |
|---|:---:|:---:|:---:|---|---|
| Optional | ✅ | ✅ `SadOptionalType` | ✅ `makeOptional` (interned P7) | سطح `رقم?` يُحلَّل ويعمل في المفسّر (P4)؛ `Null<:T?`؛ codegen مؤجَّل | **مطبَّق (سطح kind-level)** |
| Result | ✅ | ✅ `SadResultType` (P3) | ✅ `makeResult` (interned P7) | إسناد تغايُري + اختبار وحدة 47/0 (P3) | **مطبَّق** |
| Future | ✅ | ✅ `SadFutureType` | ✅ `makeFuture` | الصنف موجود؛ ربط async runtime — **متبقٍّ** | **هيكل (ربط runtime متبقٍّ)** |
| Generator | ✅ | ✅ `SadGeneratorType` | ✅ `makeGenerator` | الصنف موجود؛ ربط المولّدات — **متبقٍّ** | **هيكل (ربط runtime متبقٍّ)** |
| Qubit | ❌ | ❌ (مؤرشف) | ❌ | — | **غير موجود** |

> ملاحظة (محدَّثة): فرض أمان null يحدث في **طبقة المفسّر** (`statement_executor`، P9) لا المدقّق الدلالي (الأخير لا يُفعَّل في sad-run الافتراضي).

---

## 🛡️ ضمانات الأمان — الوضع الفعلي

### 1. Type Safety (البدائيات)
يعمل فعليًّا: `نوع(42)`→«رقم»، `نوع(3.14)`→«عشري»، `نوع("نص")`→«نص»، `نوع(صحيح)`→«منطقي»، `نوع([..])`→«مصفوفة». ✅

### 2. ❌ Null Safety — **غير مطبَّق (0%)** — مع أن الـSoT يطلبه
لا توجد قيمة `SadTypeKind::Null`. `LITERAL_NULL → Value()` أي `Void` ([expression_evaluator_core.cpp:278-279](../../../../interpreter/src/visitors/expression_evaluator_core.cpp#L278)). تأكيدات الاختبار نفسها تطابق `لاشيء → VOID` (`builtin_module_assertions.cpp:533`). نتيجة `نوع(لاشيء)` = «فراغ».
> ⚠️ **مخالفة SoT:** `language-truth/types.yaml` يُعرّف `type.null` بكلمة **«عدم»** منفصلًا عن `type.void` («فراغ») — فالتطبيق يخالف مصدر الحقيقة. الإصلاح (S-TS-P1): `نوع(لاشيء)` يجب أن يُرجع **«عدم»**.

### 3. Inheritance Type Checking
يعمل للبدائيات/الكائنات؛ لم يُتحقّق من حالات الأنواع المتقدّمة.

---

## 🧪 الاختبارات — الوضع الفعلي

> ⚠️ الأرقام في النسخة القديمة («34/34 P3 + 20 Type Safety + 10/10») **غير قابلة للإثبات ومتضاربة داخليًّا**:
> - `tests/safety/` **غير موجود**.
> - ملف الأمان النوعي الوحيد **مؤرشف**: `tests/_archive/safety/type_safety_regression.ص`.
> - لا يوجد suite باسم «P3» بـ34 اختبارًا (فقط `test_p30..p36` متفرّقة).
>
> يلزم إنشاء suite حقيقي ومُشغَّل قبل أي ادّعاء «تمرّ 100%».

---

## 📚 الملفات الرئيسية

| الملف | الوصف |
|------|-------|
| [shared/types/include/sad_type_system.h](../../../../shared/types/include/sad_type_system.h) | `SadTypeKind` (52 قيمة) + هرمية `SadType` (17 صنف) + `SadTypeRegistry` |
| [shared/types/include/type_bridge.h](../../../../shared/types/include/type_bridge.h) | واجهات التحويل (مُسقِطة للمتقدّمة) |
| [shared/types/include/value.h](../../../../shared/types/include/value.h) | `Value` يخزّن `SadTypePtr` + ثوابت `ValueType` التوافقية |
| [shared/semantic/src/semantic/type_checker.cpp](../../../../shared/semantic/src/semantic/type_checker.cpp) | المدقّق الدلالي — **لا يزال غالبًا على DataType** |

---

## 📋 الخلاصة الصادقة

| الجانب | الحالة | الملاحظة |
|-------|--------|---------|
| **توحيد البدائيات (Value→SadTypeKind)** | ✅ ~85% | صلب وحيّ؛ النقص: المدقّق الدلالي |
| **توحيد المترجم على SadTypeKind** | ✅ | أنجح جزء |
| **المدقّق الدلالي** | ⚠️ ~15% | DataType ×18 مقابل SadTypeKind ×3 |
| **الأنواع المتقدّمة (Optional/Result/Future/Generator)** | ❌ ~15% | هياكل ميتة/غير موصولة |
| **Null Safety** | ❌ 0% | لا قيمة Null؛ `لاشيء`=Void |
| **Qubit / URL** | ❌ غير موجود | كانت ادّعاءً في النسخة القديمة |
| **الاختبارات** | ⚠️ | المُدّعاة مؤرشفة/غير موجودة |

**الحالة النهائية الواقعية**: نواة توحيد قويّة (~85%) فوقها طبقة متقدّمة غير مكتملة. **التطبيق المجمّع ≈ 55–60%، لا 100%.**

انظر [خطّة الإكمال](./TYPE_SYSTEM_COMPLETION_PLAN.md).

---

*صُحِّحت هذه الوثيقة بناءً على نقد أميليا (وكيل فرعي) — مطابقة الكود الفعلي بـملف:سطر.*
