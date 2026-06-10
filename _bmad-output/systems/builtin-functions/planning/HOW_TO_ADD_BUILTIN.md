---
id: GUIDE-ADD-BUILTIN
title: "دليل إضافة دالة مدمجة (المفسر + المترجم + الآلة الافتراضية)"
version: 1.0
status: ACTIVE
date: 2026-06-07
---

# دليل: كيف تُضيف دالة مدمجة جديدة؟

> **القاعدة الذهبية:** كل دالة لها **اسم عربي واحد فقط** = الثابت المُولَّد من YAML.
> يُمنع منعاً باتاً استخدام `الثابت || "نص_بديل"`. لا أسماء إنجليزية ولا تهجئات بديلة في الكود.

هذا الدليل يشرح إضافة دالة مدمجة عبر **المسارات الثلاثة** التي تُنفِّذ اللغة:
المفسر (`sad`)، المترجم (`sadc`)، والآلة الافتراضية (`sad_vm`).

---

## ⚠️ مصيدة مهمة: الحركات (التشكيل)

الـ Lexer **يحذف الحركات العربية** (U+064B–U+065F: فتحة/ضمة/كسرة/**شدة**/سكون) من المُعرِّفات
(انظر `shared/lexer/src/lexer_core.cpp:1280`). لذلك:

```
المستخدم يكتب:  عيّن   →   الـ Lexer يُنتج:  عين   (بلا شدة)
```

**لذا: الاسم القانوني في YAML يجب أن يكون بلا تشكيل** (`عين` لا `عيّن`، `قسم` لا `قسّم`).
استخدم `scripts/codegen/normalize_canonical.py` للتأكد. التطويل (ـ) **يبقى** (لا يُحذف).

---

## الخطوة المشتركة (إلزامية للجميع): تعريف الاسم في YAML

كل دالة مدمجة تُعرَّف **مرة واحدة** في `language-truth/builtins/<module>.yaml`:

```yaml
# language-truth/builtins/math.yaml
- cpp_id: CUBE_ROOT          # معرّف C++ (UPPER_SNAKE_CASE، فريد داخل النطاق)
  canonical: "جذر_تكعيبي"    # الاسم العربي الوحيد — بلا تشكيل
  namespace: Math            # النطاق C++ (Names::Math)
  category: MODULE_FUNCTION   # CORE_IO | TYPE_CONSTRUCTOR | MODULE_FUNCTION | TYPE_METHOD
  require_import: true        # هل تحتاج `استورد`؟
  module: MATH               # NONE أو اسم الوحدة (MATH/STRINGS/…)
  compiler_strategy: LLVM_INTRINSIC  # INLINE | RUNTIME_CALL | LLVM_INTRINSIC
  since: "0.1.0"
  status: stable
  description_ar: "الجذر التكعيبي"
  description_en: "Cube root"
  params: ["العدد"]
  lookup_table: name_only    # name_only | CORE_IO | TYPE_CONSTRUCTOR | MODULE_FUNCTION
```

ثم أعد توليد الـ header:

```bash
python scripts/codegen/gen_builtins_registry.py \
  --yaml-dir language-truth/builtins/ \
  --index    language-truth/builtins/_index.yaml \
  --out-h    shared/builtins/generated/builtin_registry_generated.h \
  --quiet
```

> أو ببساطة `cmake --build build` — الـ target `sad_builtin_registry_codegen` يُعيد التوليد تلقائياً.

الآن أصبح الثابت متاحاً: `Sad::Builtins::Names::Math::CUBE_ROOT` (= `"جذر_تكعيبي"`).

---

## 1️⃣ المفسر (interpreter/ → الملف التنفيذي `sad`)

### أ) التسجيل (دالة عامة، تُستدعى بالاسم)

في ملف الوحدة المناسب `interpreter/src/builtins/builtin_module_math.cpp`:

```cpp
#include "builtin_registry.h"
namespace Bm = Sad::Builtins::Names::Math;   // اختصار أعلى الملف

// داخل registerBuiltinsMath(...):
auto cbrt_func = [](const std::vector<std::shared_ptr<Data::Value>> &args)
    -> std::shared_ptr<Data::Value>
{
    double x = args[0]->toDouble();
    return std::make_shared<Data::Value>(std::cbrt(x));
};
interpreter.getFunctionManager().registerBuiltinFunction(
    std::string(Bm::CUBE_ROOT), cbrt_func);   // ← الثابت فقط، لا نص
```

### ب) طريقة نوع (تُستدعى بـ `.`)

إن كانت طريقة (مثل `مصفوفة.رتب()`) تُضاف في
`interpreter/src/visitors/expression_evaluator_oop_array_methods.cpp`:

```cpp
namespace TM = Sad::Builtins::Names::TypeMethods;

// داخل handleArrayMethodCall:
if (m == TM::Array::CUBE_ROOT)   // ← ثابت واحد فقط، بلا || "بديل"
{
    /* ... */
    return;
}
```

والطريقة تُضاف في `language-truth/type_methods.yaml` بدل `builtins/`.

---

## 2️⃣ المترجم (compiler/ → `sadc`)

المترجم يُولِّد SIR ثم LLVM IR. يُطابق اسم الدالة لاختيار التعليمة.

في `compiler/src/frontend/builders/builtins_math.cpp`:

```cpp
#include "builtin_registry.h"
namespace Bm = Sad::Builtins::Names::Math;

// داخل buildMathBuiltin(...):
if (funcName == Bm::CUBE_ROOT)   // ← الثابت فقط
{
    // استراتيجية INLINE/LLVM_INTRINSIC/RUNTIME_CALL حسب YAML
    std::string r = b_.newTempRegister();
    SIRInstruction inst(SIROpcode::BUILTIN_MATH_CBRT);
    inst.result = SIROperand::Register(r, SadTypeKind::Double);
    inst.operands.push_back(args[0]);
    b_.currentBlock_->instructions.push_back(inst);
    return BuildResult(r, SadTypeKind::Double);
}
```

> لطرق الأنواع: `compiler/src/frontend/builders/method_call_*.cpp` تستخدم `TM::Array::*` إلخ.
> إن كانت الدالة جديدة على المترجم فقط (نواة/SIMD)، أضِفها لملف `compiler_*.yaml` بنطاق `CompilerXxx`.

---

## 3️⃣ الآلة الافتراضية (vm/ → `sad_vm`)

> **الحالة:** الـ VM **لم تُحوَّل بعد** للثوابت المُولَّدة (ضمن الخطة التالية).
> حالياً تُسجِّل بنصوص عربية مباشرة في `vm/src/sad_vm_executor.cpp`.

### الوضع الحالي (يدوي):

```cpp
// vm/src/sad_vm_executor.cpp — داخل سجّل_الدوال_الأصلية()
سجّل_دالة_أصلية("اطبع", [](const std::vector<قيمة> &م) -> قيمة { /* ... */ });
```

### الوضع الهدف (بعد التحويل — نفس نمط المفسر):

```cpp
#include "builtin_registry.h"
namespace Bn = Sad::Builtins::Names;

سجّل_دالة_أصلية(std::string(Bn::Core::PRINT), [](const std::vector<قيمة> &م) -> قيمة { /* ... */ });
```

لإضافة دالة جديدة للـ VM الآن: أضِف سطر `سجّل_دالة_أصلية(std::string(Bn::Math::CUBE_ROOT), …)`
بعد `#include "builtin_registry.h"`. (الدوال ذات الـ opcode المخصص مثل `عملية_اطبع` تُضاف في
`vm/include/sad_vm_opcodes.h` + معالجتها في `executor`.)

---

## ✅ قائمة التحقق النهائية

- [ ] أضفت الإدخال في `language-truth/builtins/<module>.yaml` (أو `type_methods.yaml`)
- [ ] الاسم القانوني **بلا تشكيل** (شغّل `normalize_canonical.py`)
- [ ] `cpp_id` فريد داخل النطاق
- [ ] أعدت التوليد (أو `cmake --build`) — الـ header مُحدَّث
- [ ] المفسر: `registerBuiltinFunction(std::string(Bn::X::Y), …)` — ثابت فقط
- [ ] المترجم: `funcName == Bn::X::Y` — ثابت فقط، بلا `||`
- [ ] الـ VM: `سجّل_دالة_أصلية(std::string(Bn::X::Y), …)` (عند التحويل)
- [ ] `cmake --build build --config Debug --target sad` أخضر
- [ ] اختبار وظيفي: ملف `.ص` يستدعي الدالة وينجح

---

## مراجع
- توليد الـ header: `scripts/codegen/gen_builtins_registry.py`
- ربط cmake: `cmake/codegen.cmake` (السطر 76+)
- التصنيفات: `shared/builtins/include/builtin_categories.h`
- الوحدات: `shared/builtins/include/module_definitions.h`
- المخطط الكامل: [BUILTIN_SYSTEM_DIAGRAM.md](BUILTIN_SYSTEM_DIAGRAM.md)
