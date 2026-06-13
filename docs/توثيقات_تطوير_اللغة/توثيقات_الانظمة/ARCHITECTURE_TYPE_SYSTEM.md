# وثيقة المعمارية: نظام الأنواع الموحد (SadTypeKind)

> **حالة التحقّق (نقد أميليا — وكيل فرعي):** هذه الوثيقة صُحّحت لتطابق الكود الفعلي.
> الادّعاء السابق «مكتمل 100% — صالح للإنتاج» **غير دقيق**. التطبيق الحقيقي ≈ **55–60%**:
> النواة (توحيد البدائيات + السجلّ + توحيد المترجم) ≈ 85%، الأنواع المتقدّمة ≈ 15%، وأمان Null = 0%.
> انظر [خطّة الإكمال](./TYPE_SYSTEM_COMPLETION_PLAN.md) للفجوات وأولويّات سدّها.

## 📋 نظرة عامة

**الهدف الأساسي**: توحيد نظام الأنواع في لغة ص من عدّة أنظمة منفصلة إلى نظام واحد موحّد (`Types::SadTypeKind`).

**الحالة الفعلية (مدعومة بالكود):**
- ✅ إزالة `enum Data::ValueType` القديم من المسار النشط (بقي فقط في `archived/` و`sad_ui/`). البديل ثوابت توافقية `constexpr SadTypeKind` في [value.h:83-99](../../../../shared/types/include/value.h#L83-L99).
- ✅ `Value` يخزّن النوع كـ`Types::SadTypeKind` + `SadTypePtr` ([value.h:236-246](../../../../shared/types/include/value.h#L236)).
- ✅ المترجم يستورد `Sad::Types::SadTypeKind` (`compiler/include/types/type.h:33`, `sir_types.h:46`) — أنجح جزء في التوحيد.
- ⚠️ **المدقّق الدلالي لم يُوحَّد بعد**: `type_checker.cpp` يستعمل `DataType` القديم ×18 مقابل `SadTypeKind` ×3.
- ❌ **الأنواع المتقدّمة** (Optional/Result/Future/Generator) هياكل غير موصولة أو قيم enum ميتة.
- ❌ **لا يوجد نوع `Null`** في التعداد — `لاشيء` يُمثَّل كـ`Void`.

---

## 🏛️ المشكلة الأصلية

قبل التوحيد كانت هناك أنظمة نوع مستقلّة متعدّدة (ValueType القديم في المفسّر، DataType في الـAST، SIRValueType في المترجم) أدّت إلى:
- ❌ تحويلات متعدّدة بين الأنظمة = أخطاء محتملة وفقدان معلومات.
- ❌ صعوبة إضافة أنواع جديدة وصيانة الكود.

---

## ✨ الحل: النظام الموحّد

### الطبقة 1️⃣: **`SadTypeKind` enum** — العدد الفعلي **50 قيمة**

**الملف**: [shared/types/generated/sad_type_kind_generated.h](../../../../shared/types/generated/sad_type_kind_generated.h) (مُولَّد من `language-truth/types.yaml`)

> ✅ تصحيح اتّساق (S-TS-P0/P1): العدّ الموحَّد الآن **50 قيمة** (49 + `Null` المُضاف في P1). تعليق الترويسة في `sad_type_system.h:16,55-56` صُحِّح إلى 50، والعدد يُولَّد آليًّا من YAML فلا انحراف بنيويًّا.

**القيم الـ50 الفعلية (مُولَّدة من المصدر):**

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
  Color, Widget, Window, Event
```

> ❌ **لا توجد** القيم التالية رغم ذكرها في النسخة القديمة من الوثيقة: `Null` و`Qubit` و`URL`.
> Qubit مؤرشف فقط (`archived/experimental_types/qubit_type.cpp`).

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
└─ SadGeneratorType   (1217)
```

> ❌ **لا توجد** الأصناف التالية رغم ذكرها سابقًا: `SadIntType`, `SadFloatType`, `SadStringType`, `SadBooleanType` (البدائيات يغطّيها `SadPrimitiveType` الواحد)، ولا `SadResultType`, `SadQubitType`, `SadURLType`.
> ملاحظة: `Result` قيمة enum موجودة لكن **بلا صنف SadType ولا بناء في السجلّ** (قيمة ميتة).

### الطبقة 3️⃣: **`SadTypeRegistry` (Singleton)**

**الملف**: [sad_type_system.h:1272-1476](../../../../shared/types/include/sad_type_system.h#L1272)

المركز المركزي لإنشاء الأنواع.

> ⚠️ **حدّ فعلي**: interning (مقارنة المؤشر `==`) يعمل للأنواع البدائية/الخاصّة المخزّنة مسبقًا فقط. الأنواع المركّبة والمتقدّمة تُنشأ بـ`make_shared` جديدًا كل مرّة → **لا تصحّ فيها مقارنة المؤشر**، يلزم مقارنة بنيوية.

---

## 🌉 طبقة الجسر (Type Bridge)

**الملف**: [shared/types/include/type_bridge.h](../../../../shared/types/include/type_bridge.h)

تحويلات `SadTypeKind ↔ ValueType ↔ DataType`.

> ⚠️ **التحويل مُسقِط (lossy) للأنواع المتقدّمة**: `Optional → Void` و`Result → Void` ([type_bridge.cpp:118-121](../../../../shared/types/src/type_bridge.cpp#L118)). أي أن عبور المتقدّمة خلال الجسر يفقد هويّتها.

### جدول المطابقة (البدائيات فقط — موثوق)

| SadTypeKind | ValueType | DataType |
|-------------|-----------|----------|
| Void | VOID | UNKNOWN |
| Integer | INTEGER | INTEGER |
| Float | DOUBLE | FLOAT |
| String | STRING | STRING |
| Boolean | BOOLEAN | BOOLEAN |
| Array | ARRAY | ARRAY |
| Map | MAP | MAP |
| Class | OBJECT | OBJECT |
| Function | FUNCTION | FUNCTION |

> ❌ أُزيل صفّ `Null` من الجدول القديم — **لا قيمة `Null` في النظام**؛ `لاشيء` يُمثَّل بـ`Void`.

---

## 🔄 مسار البيانات من المصدر إلى التنفيذ

```mermaid
graph LR
    A["📄 مصدر .ص<br/>دالة ف(س: رقم)"]
    B["🔍 Lexer"]
    C["📝 Parser → AST"]
    D["🎯 Type Inference<br/>getDataType()"]
    E["🔄 Type Bridge<br/>DataType → SadTypeKind"]
    F["💾 Value<br/>(SadTypePtr محفوظ)"]
    G["🏃 Runtime / Codegen"]
    A --> B --> C --> D --> E --> F --> G
```

> ملاحظة واقعية: المدقّق الدلالي ما زال يعمل غالبًا على `DataType` قبل العبور إلى `SadTypeKind`.

---

## 📊 حالة الأنواع المتقدّمة (تصنيف صادق)

| النوع | قيمة enum | صنف SadType | بناء في السجلّ | استخدام دلالي/تنفيذي | التصنيف |
|---|:---:|:---:|:---:|---|---|
| Optional | ✅ | ✅ `SadOptionalType` | ✅ `makeOptional` | لا يُبنى من المفسّر؛ يُسقَط إلى Void بالجسر | **هيكل فقط** |
| Result | ✅ | ❌ | ❌ | يُحوَّل إلى Void | **قيمة enum ميتة** |
| Future | ✅ | ✅ `SadFutureType` | ✅ `makeFuture` | غير موصول بـ codegen | **هيكل غير موصول** |
| Generator | ✅ | ✅ `SadGeneratorType` | ✅ `makeGenerator` | لا استخدام دلالي | **هيكل غير موصول** |
| Qubit | ❌ | ❌ (مؤرشف) | ❌ | — | **غير موجود** |
| URL | ❌ | ❌ | ❌ | — | **غير موجود** |

`shared/semantic` لا يذكر Optional/Result/Future/Generator إطلاقًا.

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
| [shared/types/include/sad_type_system.h](../../../../shared/types/include/sad_type_system.h) | `SadTypeKind` (50 قيمة) + هرمية `SadType` (17 صنف) + `SadTypeRegistry` |
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
