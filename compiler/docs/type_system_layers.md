# نظام الأنواع في لغة ص — الطبقات والتقسيم

> **الغرض:** توضيح **لماذا** يوجد نظامان منفصلان للأنواع في لغة ص: نظام التشغيل (`shared/types/`) ونظام الترجمة الثابت (`compiler/src/types/`)، وكيف يتفاعلان دون تكرار.
>
> **القاعدة الذهبية:** هذا **ليس تكراراً** — كل نظام يحل مشكلة مختلفة جذرياً. لا تدمجهما.

---

## 1. الخريطة الكلية لنظام الأنواع

```
┌──────────────────────────────────────────────────────────────────────┐
│                           لغة ص                                       │
│                                                                      │
│   ┌──────────────────────────────────┐                               │
│   │  مرحلة الترجمة (Compile-Time)    │   نظام أنواع ثابت            │
│   │  ────────────────────────────    │                               │
│   │  - Type Checking                 │   namespace Sad::TypeSystem  │
│   │  - Type Inference (Hindley-Milner)│   📁 compiler/src/types/    │
│   │  - Generic Instantiation         │   📁 compiler/include/types/ │
│   │  - Trait Resolution              │                               │
│   │  - Constraint Solving            │   ⏰ يعمل قبل توليد الكود     │
│   └──────────────────────────────────┘                               │
│                    ▲                                                 │
│                    │ معلومات النوع تُحفر في AST/SIR                   │
│                    │                                                 │
│                    ▼                                                 │
│   ┌──────────────────────────────────┐                               │
│   │  مرحلة التنفيذ (Run-Time)        │   نظام قيم ديناميكي          │
│   │  ────────────────────────────    │                               │
│   │  - Value Storage (variant)       │   namespace Sad::Data        │
│   │  - Object Instances              │   📁 shared/types/src/       │
│   │  - Dynamic Dispatch              │   📁 shared/types/include/   │
│   │  - Type Bridge (compile→runtime) │                               │
│   └──────────────────────────────────┘   ⏰ يعمل أثناء التنفيذ        │
└──────────────────────────────────────────────────────────────────────┘
```

---

## 2. لماذا نظامان وليس واحد؟

### المشكلة الجذرية

نظام النوع الثابت يحتاج معلومات **هيكلية رياضية**:
- `TypeVariable` (مثل `α`, `β`) للاستنتاج (Hindley-Milner)
- `Constraint` (مثل `α = نص`, `β <: قابل_للمقارنة`)
- `Substitution` (تطبيق `[α ↦ نص]` على شجرة أنواع)
- `Unification` (دمج نوعين متوافقين)

أما نظام التشغيل فيحتاج معلومات **تنفيذية مادية**:
- `Value` (variant يحوي int/double/string/object/...)
- `ObjectInstance` (مثيل كائن مع حقول وميتاتدتا)
- `ClassType` (وصف runtime لصنف، مع vtable)

### لو دمجناهما (سيناريو سيئ)

| المشكلة | التكلفة |
|---|---|
| كل `Value` يحمل `TypeVariable` و`Substitution` | تضخم كل قيمة بحقول لا تحتاجها أثناء التشغيل |
| كل `Constraint` يحمل `ObjectInstance` | معلومات التشغيل تتسرب لطبقة الترجمة |
| `Unification` يفحص قيم runtime | خلط مفاهيم: نوع `int` vs قيمة `42` |
| المُفسِّر يستورد رؤوس Hindley-Milner | تبعية ثقيلة بدون فائدة |
| المترجم يستورد `Value::variant` | تبعية على ABI الـ runtime |

**النتيجة:** نظام واحد ضخم متشابك، صعب التطوير والاختبار.

### الفصل الحالي (CW-02 + CW-03)

| الطبقة | المسؤولية | لا يعرف عن |
|---|---|---|
| `Sad::TypeSystem` (compile) | "هل هذا التعبير له النوع X؟" | `Value`, `ObjectInstance` (لا تحتاج) |
| `Sad::Data` (runtime) | "ماذا تحوي هذه القيمة الآن؟" | `TypeVariable`, `Constraint` (لا تحتاج) |

---

## 3. نظام الترجمة (`compiler/src/types/` + `compiler/include/types/`)

### الموقع والـ Namespace

- **مساحة الاسم:** `Sad::TypeSystem`
- **مكان الكود:** [compiler/src/types/](../src/types/) — 18 ملف `.cpp`
- **مكان الرؤوس:** [compiler/include/types/](../include/types/) — 19 ملف `.h`
- **يُجمَّع في:** `sad_type_system` (مكتبة ساكنة، تُستخدم من `sadc` و`sad`)

### الخريطة المعمارية

```
                    Sad::TypeSystem  (compile-time)
                          │
        ┌─────────────────┼──────────────────┬──────────────────┐
        ▼                 ▼                  ▼                  ▼
   التمثيل         الاستنتاج          القيود              التطبيقات
   Representation   Inference         Constraints        Applications
        │                 │                  │                  │
        ▼                 ▼                  ▼                  ▼
   ┌────────────┐   ┌──────────────┐   ┌─────────────┐   ┌──────────────┐
   │ Type       │   │ TypeInferencer│  │ Constraint  │   │ Generics     │
   │ TypeVariable│  │ Substitution │   │ ConstraintSet│  │ Substitution │
   │ Primitive  │   │ Unification  │   │ ConstraintSolver│ │ TraitSystem  │
   │ Struct     │   └──────────────┘   └─────────────┘   └──────────────┘
   │ Enum       │
   │ Union      │
   │ Future     │
   │ Generator  │
   │ Arabic     │
   └────────────┘
        │
        ▼
   TypeRegistry  (سجل مركزي لجميع الأنواع المعرفة في البرنامج)
```

### الملفات وأدوارها

#### مجموعة 1: تمثيل الأنواع (Type Representation)

| الملف | الصنف الرئيسي | الدور |
|---|---|---|
| [type.cpp](../src/types/type.cpp) | `Type` (base) | جذر هرمية الأنواع |
| [primitive_type.cpp](../src/types/primitive_type.cpp) | `PrimitiveType` | `رقم`, `نص`, `منطقي`, `عشري`, `فراغ` |
| [struct_types.cpp](../src/types/struct_types.cpp) | `StructType` | البنى (`بنية`) |
| [enum_types.cpp](../src/types/enum_types.cpp) | `EnumType` | التعدادات (`تعداد`) |
| [union_types.cpp](../src/types/union_types.cpp) + [union_type.cpp](../src/types/union_type.cpp) | `UnionType` | الاتحادات |
| [future_type.cpp](../src/types/future_type.cpp) | `FutureType<T>` | نوع الـ async return |
| [generator_type.cpp](../src/types/generator_type.cpp) | `GeneratorType<T>` | نوع المولّدات (`yield`) |
| [arabic_types.cpp](../src/types/arabic_types.cpp) | (تعريفات) | تعريفات لأسماء الأنواع العربية |

#### مجموعة 2: الاستنتاج (Type Inference)

| الملف | الصنف | الدور |
|---|---|---|
| [type_variable.cpp](../src/types/type_variable.cpp) | `TypeVariable` | المتغير `α`, `β`, ... للاستنتاج |
| [type_inferencer.cpp](../src/types/type_inferencer.cpp) | `TypeInferencer` | محرك Hindley-Milner |
| [substitution.cpp](../src/types/substitution.cpp) | `Substitution` | تطبيق `[α ↦ T]` على شجرة |
| [unification.cpp](../src/types/unification.cpp) | `Unification` | دمج نوعين (`unify(α, نص)`) |

#### مجموعة 3: القيود (Constraints)

| الملف | الصنف | الدور |
|---|---|---|
| [constraint.cpp](../src/types/constraint.cpp) | `Constraint`, `ConstraintSet` | تمثيل `α = β`, `α: قابل_للمقارنة` |
| [constraint_solver.cpp](../src/types/constraint_solver.cpp) | `ConstraintSolver` | حل مجموعة قيود متبادلة |

#### مجموعة 4: تطبيقات متقدمة

| الملف | الصنف | الدور |
|---|---|---|
| [generics.cpp](../src/types/generics.cpp) | `GenericInstantiator` | `قائمة<رقم>` ← `قائمة<T>` + `[T↦رقم]` |
| [trait_system.cpp](../src/types/trait_system.cpp) | `TraitSystem` | السمات (`سمة`)، حل dispatch ثابت |
| [type_registry.cpp](../src/types/type_registry.cpp) | `TypeRegistry` | سجل مركزي لكل الأنواع في البرنامج |

#### رؤوس مساعدة (دون cpp مرافق)

| الملف | الدور |
|---|---|
| [atomic_types.h](../include/types/atomic_types.h) | تعريفات أنواع ذرية (`@ذري`) للمترجم |
| [c_types.h](../include/types/c_types.h) | جسر لأنواع C في FFI (يُستخدم في `ffi_wrapper.h`) |
| [composite_type_classes.h](../include/types/composite_type_classes.h) | helpers لأنواع مركبة |

### مثال على التدفق (استنتاج نوع)

```sad
دالة جمع(أ، ب)
   ارجع أ + ب
نهاية
```

```
Parser ينتج FunctionDecl(جمع, params=[أ:?, ب:?], body=Return(BinOp(+, أ, ب)))
       │
       ▼
TypeInferencer.infer(funcDecl)
       │
       ├─ ينشئ TypeVariable α لـ أ
       ├─ ينشئ TypeVariable β لـ ب
       ├─ ينشئ TypeVariable γ لنوع الإرجاع
       │
       ▼
ConstraintSolver يجمع:
       │  Constraint: α = β            (لأن + يتطلب نوع متجانس)
       │  Constraint: γ = α            (لأن النتيجة من نفس النوع)
       │  Constraint: α ∈ {رقم, عشري, نص}  (يدعم +)
       │
       ▼
Substitution تنتج: [α ↦ رقم, β ↦ رقم, γ ↦ رقم]  (افتراضي)
       │
       ▼
الـ AST يُحفر بمعلومات النوع المُستنتَجة
```

---

## 4. نظام التشغيل (`shared/types/` تحت namespace `Sad::Data`)

### الموقع والـ Namespace

- **مساحة الاسم:** `Sad::Data`
- **مكان الكود:** [shared/types/src/](../../shared/types/src/) — 6 ملفات `.cpp`
- **مكان الرؤوس:** [shared/types/include/](../../shared/types/include/) — 13 ملف `.h`
- **يُجمَّع في:** يُربط مع `sad_core` (يستخدمه المُفسِّر، الـ VM، والـ runtime للمترجم)

### الخريطة المعمارية

```
                        Sad::Data  (run-time)
                              │
        ┌─────────────────────┼──────────────────────┐
        ▼                     ▼                      ▼
   تخزين القيمة          الكائنات              الجسر للمترجم
   Value Storage         Objects                Bridge
        │                     │                      │
        ▼                     ▼                      ▼
   ┌──────────────┐    ┌──────────────────┐    ┌───────────────┐
   │ Value        │    │ ObjectInstance   │    │ TypeBridge    │
   │ (std::variant)│    │ (حقول + ميتاتدتا) │    │ (يحول بين     │
   │              │    │ ClassType        │    │  TypeSystem    │
   │ - INT        │    │ (وصف الصنف)      │    │  ⇄ Data)      │
   │ - DOUBLE     │    │ Generator        │    └───────────────┘
   │ - STRING     │    │ (state machine)  │
   │ - BOOL       │    └──────────────────┘
   │ - ARRAY      │
   │ - MAP        │           ┌──────────────────┐
   │ - OBJECT     │           │ SadTypeSystem    │
   │ - CHANNEL    │           │ (نظام موحد       │
   │ - ...        │           │  للوصول لـ Value) │
   └──────────────┘           └──────────────────┘
```

### الملفات وأدوارها

| الملف | الصنف الرئيسي | الدور |
|---|---|---|
| [value.cpp](../../shared/types/src/value.cpp) | `Value`, `ValueType` | النوع الجامع لكل القيم (`std::variant`). **80KB — أكبر ملف**. |
| [object_instance.cpp](../../shared/types/src/object_instance.cpp) | `ObjectInstance` | مثيل كائن في الذاكرة (حقول + class pointer) |
| [class_type.cpp](../../shared/types/src/class_type.cpp) | `ClassType` | وصف runtime لصنف (الحقول، الدوال، الوراثة) |
| [generator.cpp](../../shared/types/src/generator.cpp) | `Generator` | حالة آلة المولّدات (`yield`) في وقت التشغيل |
| [type_bridge.cpp](../../shared/types/src/type_bridge.cpp) | `TypeBridge` | **الجسر:** يحول من `Sad::TypeSystem::Type` إلى `Sad::Data::ValueType` |
| [sad_type_system.cpp](../../shared/types/src/sad_type_system.cpp) | `SadTypeSystem` | API موحد للتعامل مع `Value` (helpers) |

### مثال على التدفق (تنفيذ قيمة)

```sad
متغير س = 42
اطبع_سطر(س + 1)
```

```
المُفسِّر يصل لتعريف س
      │
      ▼
ينشئ Sad::Data::Value(42)  ← variant<int>(42)
      │
      ▼
يخزنه في scope: variables["س"] = Value(42)
      │
      ▼
لتقييم (س + 1):
      │
      ├─ يجلب Value(42) من scope
      ├─ يبني Value(1)
      ├─ ValueArithmetic::add(Value(42), Value(1))  → Value(43)
      │
      ▼
اطبع_سطر يستلم Value(43) → يطبع "43"
```

---

## 5. الجسر بين الطبقتين (TypeBridge)

```
                Sad::TypeSystem  (compile-time)
                ────────────────────
                 PrimitiveType("رقم")
                 StructType("نقطة", fields=[س:رقم, ص:رقم])
                       │
                       │ TypeBridge::convert()
                       ▼
                Sad::Data  (run-time)
                ──────────────
                 ValueType::INT
                 ObjectInstance(class=ClassType("نقطة"), fields={س:0, ص:0})
```

`TypeBridge` ([shared/types/src/type_bridge.cpp](../../shared/types/src/type_bridge.cpp)) هو **النقطة الوحيدة** التي يلتقي فيها النظامان. يعمل في اتجاه واحد:
- المترجم يقول: "لدي `StructType("نقطة")` بهذه الحقول"
- `TypeBridge` يبني له `ClassType` runtime مكافئ
- المُفسِّر يستخدم `ClassType` لإنشاء `ObjectInstance` عند `جديد`

---

## 6. لماذا `compiler/include/types/` فيه `c_types.h` و`atomic_types.h`؟

هذان الرأسان **ليسا** Hindley-Milner — هما helpers خاصة بمترجم `sadc`:

### `c_types.h` — جسر FFI

```c
// (AR) يصف أنواع C الأساسية لاستخدامها في @خارجي وaspectات FFI
// (EN) Describes basic C types for use in extern declarations and FFI
typedef struct CIntType { /* ... */ };
typedef struct CPointerType { /* ... */ };
```

يستخدمه `ffi_wrapper.h` لتوليد LLVM IR صحيح للاستدعاءات الخارجية. مكانه الصحيح هنا لأنه **خاص بـ codegen** وليس بـ runtime الـ Sad.

### `atomic_types.h` — توجيهات `@ذري`

يحوي تعريفات الأنواع الذرية المستخدمة في توجيه `@ذري(تحميل, س)`. ضروري في codegen لتوليد `LLVM atomic load/store/cmpxchg` الصحيح.

---

## 7. مقارنة جنبية: من يستخدم ماذا؟

| المكوّن | يستخدم `Sad::TypeSystem` | يستخدم `Sad::Data` |
|---|:---:|:---:|
| Lexer | ❌ | ❌ |
| Parser | ❌ (يبني AST عام) | ❌ |
| Type Checker (sema) | ✅ | ❌ |
| Interpreter (sad-run) | ❌ (محدود — يعتمد على معلومات AST) | ✅✅ كل شيء |
| VM | ❌ | ✅✅ كل شيء |
| SIR Builder | ✅ (يحفر معلومات النوع) | ❌ |
| SIR Optimizer | ✅ (للتحسينات النوعية) | ❌ |
| LLVM Codegen | ✅ + `c_types.h` + `atomic_types.h` | ❌ |
| FFI Wrapper | `c_types.h` | ❌ |
| Stdlib (built-ins) | ❌ | ✅ |
| Tests | ✅ (للأنواع) أو ✅ (للقيم) حسب الحاجة | ✅ |

---

## 8. القواعد الصارمة (لا تكسرها)

> **هذه القواعد تمنع تكرار مشكلة "المحسن المكرر" في نظام الأنواع.**

1. **CW-02 (Layered):** `Sad::TypeSystem` لا يستورد أبداً من `Sad::Data` (واتجاه واحد فقط: عبر `TypeBridge`).
2. **CW-19 (DRY):** أي نوع جديد يُضاف **مرة واحدة فقط**. حدد أولاً إن كان compile-time (`compiler/src/types/`) أم runtime (`shared/types/`):
   - هل يحتاج `unify()`, `substitute()`, `infer()`؟ ⇒ compile-time
   - هل يُحفظ كقيمة في متغير وقت التشغيل؟ ⇒ runtime
   - كلاهما؟ ⇒ نوع compile-time + entry في `TypeBridge`
3. **لا runtime types في compiler/:** يُمنع وضع `ObjectInstance`-like في `compiler/src/types/`.
4. **لا compile-time inference في shared/:** يُمنع وضع `TypeInferencer`-like في `shared/types/`.
5. **TypeBridge هي البوابة الوحيدة:** أي تحويل بين النظامين يمر عبره. لا تستورد رؤوس النظام الآخر مباشرةً.
6. **`c_types.h` و`atomic_types.h` استثناءات موثقة:** ليست نظام أنواع — هي helpers لـ codegen. لا تنقلهما لـ shared.

---

## 9. تخطيط المجلدات النهائي

```
s-programming-language/
├── shared/                          ← مكتبات مشتركة بين المُفسِّر والمُترجم
│   └── types/                       ← Sad::Data — نظام القيم وقت التشغيل
│       ├── include/                 (Value, ObjectInstance, ClassType, Generator, TypeBridge)
│       └── src/
│
└── compiler/                        ← خاص بمُترجم sadc
    ├── include/
    │   ├── types/                   ← Sad::TypeSystem — نظام الأنواع وقت الترجمة
    │   │   ├── (Hindley-Milner: Type, TypeVariable, Substitution, Unification, ...)
    │   │   ├── (Constraint solving: Constraint, ConstraintSolver, ...)
    │   │   ├── (Type representations: Primitive, Struct, Enum, Union, Future, Generator)
    │   │   ├── (Generics + Traits + TypeRegistry + TypeInferencer)
    │   │   ├── c_types.h            ← helper لـ FFI codegen
    │   │   └── atomic_types.h       ← helper لتوجيهات @ذري
    │   └── sir_optimizer/           ← passes تحسين SIR (انظر optimization_layers.md)
    └── src/
        ├── types/                   ← تنفيذ Sad::TypeSystem (18 .cpp)
        ├── sir_optimizer/           ← تنفيذ passes التحسين (16 .cpp)
        └── backend/llvm/            ← LLVM codegen + arabic_passes
```

---

## 10. مراجع داخلية

- طبقات المحسن: [compiler/docs/optimization_layers.md](optimization_layers.md)
- خريطة المشروع: [/memories/repo/project_full_map.md](/memories/repo/project_full_map.md)
- نظام الأنواع الموحد (تاريخي): [/memories/repo/unified_type_system.md](/memories/repo/unified_type_system.md)
- جسر النوعين: [shared/types/src/type_bridge.cpp](../../shared/types/src/type_bridge.cpp)
- نقطة دخول الاستنتاج: [compiler/src/types/type_inferencer.cpp](../src/types/type_inferencer.cpp)
- نقطة دخول القيم: [shared/types/include/value.h](../../shared/types/include/value.h)
