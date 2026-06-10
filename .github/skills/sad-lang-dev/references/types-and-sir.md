# نظام الأنواع و SIR (التمثيل الوسيط) في لغة ص

هذا المرجع يغطّي طبقتين متكاملتين: **نوع القيم في وقت التشغيل** (`Value`/`ValueType` —
يستخدمه المفسر)، و**نظام الأنواع الثابت + SIR** (يستخدمه المترجم `sadc`).

---

## أ. نوع القيم في وقت التشغيل — `Value`

الملف: `shared/types/include/value.h`. هذا هو **النوع الموحّد** الذي يمرّره المفسر.

### الخصائص الأساسية

- `Value` يحمل نوعاً داخلياً `ValueType type_` ويُغلِّف القيمة الفعلية.
- **الكائنات تُخزّن كمؤشر مُدار بـ GC:**
  ```cpp
  using ObjectPtr      = ObjectInstance *;                 // مؤشر خام مُدار بمحرّك GC الموحَّد
  using FunctionRefPtr = std::shared_ptr<FunctionRef>;     // مرجع دالة
  ```
  دورة حياة `ObjectInstance` يديرها GC؛ استخدم `Pin<ObjectInstance>` لتثبيت كائن كجذر عند الحاجة.
- تمرير الكائنات **بالمرجع تلقائياً** (لا نسخ) عبر إدارة GC — هذا أساس دلالة المرجعية في اللغة.

### البنّاءات (Constructors)

```cpp
Value();                       // VOID (قيمة فارغة)
explicit Value(int64_t val);   // INTEGER (32-bit يُرقّى تلقائياً إلى 64)
explicit Value(double val);    // DOUBLE
Value(const std::string& val); // STRING (غير explicit — للحرفيات)
explicit Value(bool val);      // BOOLEAN
explicit Value(const ArrayType& val); // ARRAY
explicit Value(const MapType& val);   // MAP
Value(TupleTag, const TupleType& val);// TUPLE (يحتاج tag للتمييز)
explicit Value(ObjectPtr obj);        // OBJECT
explicit Value(FunctionRefPtr funcRef); // FUNCTION
```

### فاحصات النوع (Type Predicates)

```cpp
bool isInteger();  bool isDouble();   bool isString();  bool isBoolean();
bool isArray();    bool isMap();      bool isTuple();   bool isObject();
bool isFunction(); bool isObjectLike();

bool isNumeric();   // = isInteger() || isDouble()
bool isContainer(); // = isArray() || isMap() || isTuple()
```

> **قاعدة CW-15:** افحص النوع (`isObject()`...) **قبل** أي وصول لقيمة — الوصول الخاطئ يطلق استثناء.

### التحويل النصي

```cpp
std::string toString() const;           // لأي نوع
const std::string& toStringRef() const; // STRING فقط (بلا نسخ) — للأنواع الأخرى استخدم toString()
```

### تنبيهات مهمة (من معايير المشروع)

- **تعارض ماكرو `VOID` مع Windows:** الملف يلغي تعريف `VOID` قبل استخدام `ValueType::VOID`.
- `getType()` يُرجع `Types::SadTypeKind` (نظام الأنواع الثابت) — وهو **مختلف** عن `ValueType`
  الداخلي لوقت التشغيل. لا تخلط بينهما.

---

## ب. نظام الأنواع الثابت — `SadTypeKind`

الملف: `shared/types/include/sad_type_system.h` — `enum class SadTypeKind : int`. يصف الأنواع
كما يفهمها المترجم/المدقّق الدلالي (أوسع بكثير من `ValueType`):

| الفئة | القيم |
|------|-------|
| بدائية | `Void`, `Integer` (i64), `Float` (f64), `Boolean`, `String`, `Byte`, `Char` |
| بحجم محدد | `Int8/16/32/64`, `UInt8/16/32/64`, `Float32/64` (للمترجم/FFI/الأنظمة المنخفضة) |
| مركّبة | `Array`, `Map`, `Tuple`, `Slice` |
| كائنية | `Class`, `Struct`, `Enum`, `Trait` |
| وظيفية | `Function`, `Closure` |
| متقدمة | `Union` (T1\|T2), `Intersection` (T1&T2), `Optional` (T?), `Result<T,E>`, `Generic` (T) |

> عند العمل على المدقّق الدلالي أو الأنواع المُعمَّمة (generics)/القيود (`حيث`)، هذا هو التعداد المعنيّ.
> `DataType`/`DT` enum القديم **لا يحتوي `ANY`** — استخدم `DT::OBJECT` بدلاً منه.

---

## ج. SIR — التمثيل الوسيط للمترجم

الملف: `compiler/include/frontend/sir_types.h`. SIR طبقة وسيطة بين AST و LLVM IR، مبنية
على **SSA** (تعيين وحيد ثابت) مع عقد `PHI`. التعداد `enum class SIROpcode` يضم **~90 opcode**.

### مسار المترجم

```
AST → SIRBuilder (compiler/src/frontend) → SIR module
    → SIROptimizer (compiler/src/sir_optimizer)
    → LLVMCodeGen (compiler/src/backend/llvm) → LLVM IR → تنفيذي
```

### فئات الـ opcodes (مع العدد)

| # | الفئة | أمثلة |
|---|------|------|
| 1 | حسابية (10) | `ADD_I64`, `ADD_F64`, `SUB_*`, `MUL_*`, `DIV_*`, `FLOOR_DIV_I64`, `MOD_I64`, `NEG` |
| 2 | ثنائية بِتّية (8) | `AND`, `OR`, `XOR`, `NOT`, `SHL`, `SHR`, `SAR`, `ROL` |
| 3 | مقارنات (6) | `EQ`, `NE`, `LT`, `LE`, `GT`, `GE` |
| 4 | تحكم بالتدفق (8) | `BR`, `BR_COND`, `RET`, `RET_VOID`, `CALL`, `CALL_INDIRECT`, `SWITCH`, `PHI` |
| 5 | الذاكرة والمؤشرات (11) | `ALLOC`, `ALLOC_HEAP`, `FREE`, `LOAD`, `STORE`, `MOVE`, `ADDR`, `PTR_ADD`, `PTR_CAST`, `MEMCPY`, `MEMSET` |
| 6 | المصفوفات (6) | `ARRAY_NEW/GET/SET/LEN/APPEND/REMOVE/CONCAT` |
| 6b | الصفوف (3) | `TUPLE_NEW/GET/LEN` |
| 7 | النصوص (8) | `STRING_NEW/LEN/CONCAT/CMP/SUBSTR/FIND/REPLACE`, `STRING_TO_I64/F64` |
| 8 | OOP (10) | `OBJECT_NEW/GET/SET/CALL`, `INSTANCEOF`, `OBJECT_CAST`, `CLASS_DEF`, `METHOD_DEF`, `FIELD_DEF`, `CONSTRUCTOR_CALL` |
| 8b | تعدادات جبرية (4) | `ENUM_CONSTRUCT`, `ENUM_GET_TAG`, `ENUM_GET_PAYLOAD`, `ENUM_IS_VARIANT`, `ENUM_FREE` |
| — | التزامن / SIMD / BUILTIN_* / LOWLEVEL_* / FFI_* / UEFI / Android | عائلات opcodes مخصّصة (دوال مضمنة، أنظمة منخفضة، FFI...) |

> النموذج الكامل في الملف بتعليقات مزدوجة اللغة لكل opcode. **الملكية:** عمليات مثل `ALLOC`,
> `ALLOC_HEAP`, `FREE`, `MOVE` هي عماد نموذج الملكية في المترجم (تخصيص/تحرير/نقل).

### المعاملات — `SIROperand` / `SIROperandType`

```cpp
enum class SIROperandType {
    REGISTER,  // سجل افتراضي %0, %1, %name
    CONSTANT,  // ثابت 42, 3.14, "hello"
    LABEL,     // تسمية (للقفزات)
    GLOBAL,    // متغير عام $name
    FUNCTION   // اسم دالة @name
};
```

دوال مساعدة: `sirOpcodeToString(opcode)`، `sirOpcodeOperandCount(opcode)`.

---

## د. قواعد العمل على هذه الطبقات

- **إضافة opcode جديد مسموحة؛ تغيير معنى موجود ممنوع** (CW-24, BF-15) — التوافق الخلفي مقدّس.
- **عند خطأ تحويل أنواع في الإخراج:** أصلِح في codegen (`compiler/src/backend/`) — ليس في
  مكان الاستعمال (BF-10). لا تُخفِ عدم تطابق نوع بتحويل ضمني (BF-09, CW-14).
- **ترتيب الحقول:** عند أخطاء البنى، تحقّق من تطابق فهارس `getelementptr`/`OBJECT_GET` مع
  ترتيب `FIELD_DEF` الفعلي (BF-19).
- **تسمية الكتل (CW-11):** كتل LLVM IR بأسماء واصفة (`loop_body`, `then_block`) لا `bb1`؛
  واحذر ترتيب الكتل — لا تعتمد ترتيباً أبجدياً (BF-16).
- **اختبر في المفسر والمترجم** — اختلاف السلوك يكشف خطأ SIR/codegen (BF-08).
