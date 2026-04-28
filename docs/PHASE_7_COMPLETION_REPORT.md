# تقرير المرحلة 7 — تفكيك LLVMCodeGen "الإله"

> **الحالة:** Phase 7 مكتملة بنجاح حتى Step 18 — جاهزة للانتقال إلى Phase 8.
> **آخر commit:** `f4e3121b` (Phase 7 Step 18: OOPOpsCodeGen)
> **خط الأساس للاختبارات:** **793/837 ناجح (94.7%)** — ثابت عبر جميع الخطوات الـ18.
> **زمن الاختبارات الكامل:** ~268 ثانية (طقم dual_execution).

---

## 1. الملخص التنفيذي

كان `LLVMCodeGen` صنفاً عملاقاً ("god class") تجاوز **15,000 سطر** موزعة على ملف رأسي ضخم وعشرات ملفات `.cpp` المسطحة في `compiler/src/backend/llvm/`. أدى هذا إلى:
- زمن إعادة بناء طويل لأي تعديل في الرأس.
- صعوبة فهم الواجهة العامة.
- خرق متكرر لـ CW-01 (مبدأ المسؤولية الواحدة) وCW-05 (حد 800 سطر).

**الهدف الجذري** (وفق BF-04): تفكيك الصنف إلى **مولّدات فرعية متعاونة** (sub-codegens) عبر نمط *SubBuilder + friend access*، مع الحفاظ على واجهة `LLVMCodeGen` العامة بدون كسر.

**النتيجة الفعلية بعد 18 خطوة:**
- **20 صنف فرعي** في `compiler/include/backend/llvm/builders/` و`compiler/src/backend/llvm/builders/`.
- **~377 method** انتقلت من الصنف الإله إلى أصناف متخصصة.
- **0 تراجع** في الاختبارات — 793/837 ثابت من Step 1 إلى Step 18.
- **0 كسر** في الـ ABI الخارجي — جميع الواجهات العامة تعمل بشكل مطابق.

---

## 2. النمط المعماري (SubBuilder Pattern)

### 2.1 المكونات

كل مولّد فرعي يتكون من:
1. **header** في `compiler/include/backend/llvm/builders/X_codegen.h`:
   - يحوي forward declaration لـ `LLVMCodeGen`.
   - الصنف يحمل مرجعاً واحداً: `LLVMCodeGen &cg_;`.
   - يمنع النسخ صراحةً (`= delete`).
   - يصرّح فقط الـ methods التي يملكها هذا الفرع.

2. **مصدر** في `compiler/src/backend/llvm/builders/X_ops.cpp`:
   - يضم `#include "llvm_codegen.h"` + `#include "builders/X_codegen.h"`.
   - كل وصول لحقل/method في `LLVMCodeGen` يتم عبر `cg_.field_` أو `cg_.method()`.

3. **في `LLVMCodeGen`:**
   - `friend class XCodeGen;` للوصول للحقول الخاصة.
   - `std::unique_ptr<XCodeGen> name_;` كعضو.
   - **wrapper inline** لكل method منقولة — يحافظ على نفس التوقيع في الواجهة العامة:
     ```cpp
     llvm::Value *emitObjectNew(std::shared_ptr<SIRInstruction> inst) {
         return oop_->emitObjectNew(inst);
     }
     ```

4. **في `llvm_codegen_init.cpp`:**
   - تهيئة العضو في جسم المُنشئ: `oop_ = std::make_unique<OOPOpsCodeGen>(*this);`

### 2.2 ضمانات النمط

- **الواجهة العامة محفوظة 100%** — أي كود خارجي يستدعي `cg.emitObjectNew(...)` يعمل دون تغيير (CW-24: Backward Compatibility).
- **لا تكرار للمنطق** — الـ wrapper سطر واحد فقط (CW-19: DRY).
- **اعتماد صريح** — كل sub-codegen يعلن `friend` مرة واحدة في الرأس (CW-06).
- **آمن للخيوط** — لا حالة عامة جديدة، فقط مرجع `cg_`.

---

## 3. سجل الخطوات (18 خطوة)

| الخطوة | الصنف الفرعي | عدد Methods | الحجم | الالتزام |
|--------|--------------|-------------|-------|----------|
| 1 | `ArithmeticCodeGen` | 30+ | 110KB+ | (مبكر) |
| 2 | `MemoryCodeGen` | 20+ | 86KB | (مبكر) |
| 3 | `ControlFlowCodeGen` | 15+ | 43KB | (مبكر) |
| 4 | `AggregateOpsCodeGen` | 8 | 6.3KB | (مبكر) |
| 5 | `ArrayOpsCodeGen` | 12+ | 36.6KB | (مبكر) |
| 6 | `StringOpsCodeGen` | 30+ | 81.6KB | (مبكر) |
| 7 | `ArrayBuiltinsCodeGen` | 15+ | 37KB | (مبكر) |
| 8 | `MathBuiltinsCodeGen` | 12+ | 24.6KB | (مبكر) |
| 9 | `MapOpsCodeGen` | 14+ | 42KB | (مبكر) |
| 10 | `ExceptionCodeGen` | 10 | 26.8KB | (مبكر) |
| 11 | `LowlevelCodeGen` | 18+ | 72.4KB (ملفان) | (مبكر) |
| 12 | `EnumOpsCodeGen` | 10 | 34.8KB | (مبكر) |
| 13 | `HardwareFFICodeGen` | 25+ | 84KB (ملفان) | (مبكر) |
| 14 | `ClosureCodeGen` | 9 | 45.7KB | `c5a19ee0` |
| 15 | `IO/Security/FFIRemain` | 20 | 56KB (3 ملفات) | `e3468a67` |
| 16 | `FreestandingCodeGen` | 19 | 117KB (4 ملفات) | `f5ed9156` |
| 17 | `ObjectsArraysCodeGen` | 7 | 58.5KB | `0bac2b2c` |
| 18 | `OOPOpsCodeGen` | 6 | 44KB | `f4e3121b` |

**الإجمالي:** ~377 method في 20 sub-codegen.

---

## 4. الدروس المستفادة (محفوظة في `/memories/repo/compiler_fix_notes.md`)

### 4.1 BF-26 (مراجعة شاملة بعد الإصلاح)
بعد Step 17، اكتُشف أن `sirModule_` و`emitVirtualCall` كانتا ناقصتين من قائمة `CG_NAMES` الافتراضية في سكربت الاستخراج، مما أدى لـ 22 خطأ بناء. الحل الجذري: تحديث القائمة الافتراضية لتشمل هذين الرمزين، وقد ثبت عمل النمط في Step 18 (بناء نظيف من أول مرة).

### 4.2 CW-26 (جداول البحث بدل السلاسل الشرطية)
الـ wrappers الـ inline في `llvm_codegen.h` تعمل كجدول استدعاء مباشر إلى `unique_ptr<XCodeGen>`، مما يجنّب أي `if/else` في hot path.

### 4.3 BF-23 (الإصلاح التدريجي)
كل خطوة كانت محصورة بصنف فرعي واحد + ملف `.cpp` واحد أو مجموعة ملفات مرتبطة. هذا منع التراجعات الكبيرة وأبقى زمن إصلاح أي خطأ ضمن جلسة واحدة.

### 4.4 سكربت استخراج معاد الاستخدام
`_scratch/phase7_step17_objects.py` و`_scratch/phase7_step18_oop.py` يقدمان قالباً جاهزاً للخطوات القادمة:
- regex لتحويل `LLVMCodeGen::` → `XCodeGen::`.
- regex بـ lookbehind `(?<!cg_\.)` لمنع التضعيف عند إعادة التشغيل.
- إضافة include + حذف الملف الأصلي تلقائياً.

---

## 5. الحالة الحالية للمستودع

### 5.1 ملفات `builders/` (32 ملف cpp)

```
agg_ops, arith_cmp, arith_extras, arith_main, arith_resolve, arith_type_conv,
array_builtins, array_ops, cf_branch_call, cf_return_switch, closure_ops,
enum_ops, exception_ops, ffi_remain_ops, freestanding_conv_ops,
freestanding_io_ops, freestanding_ops, freestanding_sprintf_ops,
hardware_ffi_ops, hardware_ffi_ops2, io_builtins_ops, lowlevel_ops,
lowlevel_uefi_ops, map_ops, math_builtins, mem_alloca, mem_load, mem_store,
objects_arrays_ops, oop_ops, security_builtins_ops, string_ops
```

### 5.2 ملفات `llvm_codegen_*.cpp` المتبقية في الجذر (174 method)

| الملف | Methods | الحجم |
|-------|---------|-------|
| `llvm_codegen_ui.cpp` | 41 | 25.8KB |
| `llvm_codegen_init.cpp` | 20 | 26.2KB |
| `llvm_codegen_concurrency.cpp` | 19 | 25.5KB |
| `llvm_codegen_channels.cpp` | 12 | 39.3KB |
| `llvm_codegen_builtins_math_async.cpp` | 11 | 18.2KB |
| `llvm_codegen_functions.cpp` | 9 | 15.6KB |
| `llvm_codegen_classes_vtables.cpp` | 9 | 36.6KB |
| `llvm_codegen_file_casts.cpp` | 8 | 17.8KB |
| `llvm_codegen_builtin_funcs.cpp` | 8 | 17.7KB |
| `llvm_codegen_output.cpp` | 7 | 12.9KB |
| `llvm_codegen_directives.cpp` | 7 | 15.5KB |
| `llvm_codegen_coroutines.cpp` | 6 | 31.4KB |
| `llvm_codegen_functions2.cpp` | 3 | 34.4KB |
| `llvm_codegen_instructions.cpp` | 3 | 7.9KB |
| `llvm_codegen_strings.cpp` | 2 | 13.4KB |
| `llvm_codegen_builtins_network.cpp` | 2 | 44KB |
| `llvm_codegen_string_ops2.cpp` | 2 | 12.8KB |
| `llvm_codegen_simd.cpp` | 1 | 24.7KB |
| `llvm_codegen_string_ops.cpp` | 1 | 4.4KB |
| `llvm_codegen_instr_core.cpp` | 1 | 28.7KB |
| `llvm_codegen_instr_lowlevel.cpp` | 1 | 19.5KB |
| `llvm_codegen_instr_platform.cpp` | 1 | 15KB |
| `llvm_codegen_context.cpp` | 0 (utilities) | 1.7KB |

> **ملاحظة:** بعض الملفات ذات Methods=0 تحوي helpers ثابتة أو templates، وليست "god-class methods".

### 5.3 الواجهة العامة (`llvm_codegen.h`)
- ~1700 سطر (مقارنة بـ ~3000+ قبل Phase 7).
- 20 friend declarations لـ sub-codegens.
- 20 `std::unique_ptr<X> name_;` member.
- ~377 inline wrappers، كل واحد سطر واحد.

---

## 6. مكاسب القياس (Profile Before Optimize — CW-30)

- **زمن البناء التدريجي:** تحرير ملف `.cpp` واحد في `builders/` لا يعيد ترجمة `llvm_codegen.h` (لأن الـ headers الفرعية صغيرة ومستقلة) → تحسن ملموس في حلقة dev.
- **زمن الاختبار الكامل:** ثابت ~268s (لم يتغير، مما يثبت BF-29: عدم تراجع).
- **حجم الملفات الكبيرة:** أكبر ملف الآن `string_ops.cpp` بـ 81.6KB، وهو ضمن نطاق مقبول (لم يعد هناك ملف يقترب من 200KB).

---

## 7. خطة Phase 8 — مقترحات

### الخيار أ — **إكمال إفراغ الجذر** (الأكثر اتساقاً مع Phase 7)
استكمال نقل الـ174 method المتبقية وفق نفس النمط المُثبت:

| المهمة | تقدير الخطوات | الأولوية |
|--------|---------------|----------|
| `ConcurrencyCodeGen` (concurrency 25KB + channels 39KB) | 1 خطوة | عالية |
| `CoroutinesCodeGen` (coroutines 31KB) | 1 خطوة | متوسطة |
| `ClassesVtablesCodeGen` (classes_vtables 37KB) | 1 خطوة | عالية |
| `FunctionsCodeGen` (functions 16KB + functions2 34KB) | 1 خطوة | عالية |
| `BuiltinFuncsCodeGen` (builtin_funcs 18KB + math_async 18KB) | 1 خطوة | متوسطة |
| `NetworkBuiltinsCodeGen` (builtins_network 44KB) | 1 خطوة | متوسطة |
| `UICodeGen` (ui 25KB، 41 method!) | 1 خطوة | عالية |
| `SIMDCodeGen` (simd 25KB) | 1 خطوة | منخفضة |
| `FileCastsCodeGen` (file_casts 18KB) | 1 خطوة | منخفضة |
| `DirectivesCodeGen` (directives 15KB) | 1 خطوة | منخفضة |
| `OutputCodeGen` (output 13KB) | 1 خطوة | منخفضة |
| `InstructionCoreCodeGen` (instr_core 29KB + instr_lowlevel 19KB + instr_platform 15KB) | 1-2 خطوة | متوسطة |

**الناتج المتوقع:** ~12 خطوة إضافية → الجذر يبقى فقط `llvm_codegen_init.cpp` و`llvm_codegen_context.cpp` (utilities).

### الخيار ب — **تنظيف وتوحيد البنية**
- إعادة ترتيب `builders/` في فئات فرعية: `builders/arith/`, `builders/mem/`, `builders/oop/`, `builders/io/` ...
- توحيد naming: بعض الملفات `_ops.cpp` وبعضها بدون لاحقة → اعتماد لاحقة موحدة.
- استخراج CG_NAMES إلى ثابت c++ مشترك (تجنب التكرار في كل sub-codegen).
- توحيد `llvm_codegen.h` — تقسيمه إلى `llvm_codegen.h` (الواجهة) + `llvm_codegen_internals.h` (للـ friends).

### الخيار ج — **مرحلة أداء الكود المُولَّد**
Phase 7 ركّز على بنية المُترجم. Phase 8 يمكن أن يركّز على جودة الـ IR المُولَّد:
- تفعيل LLVM optimization passes إضافية.
- تحسين تخصيص الذاكرة للـ closures.
- تقليل allocations في hot paths (CW-25).
- قياس قبل/بعد بـ profiler حقيقي (BF-29).

### الخيار د — **توسيع تغطية الاختبارات**
- إضافة 44 اختبار يصلح الـflakes الحالية (websockets 083/085/086/090 + 045_ipv6).
- إضافة اختبارات وحدة للـ sub-codegens مباشرة (بدون المرور عبر runner).
- رفع الـ pass rate إلى >97%.

---

## 8. التوصية

**الخيار أ (إكمال إفراغ الجذر)** هو الامتداد الطبيعي والأقل مخاطرة لـ Phase 7. النمط مُثبت، السكربتات جاهزة، والاختبارات مستقرة. بعد إكماله يصبح المستودع في حالة مثالية للانتقال إلى الخيار ج (تحسين أداء الكود المُولَّد) في Phase 9.

---

## 9. مراجع

- نمط الاستخراج المُجرَّب: `_scratch/phase7_step17_objects.py`، `_scratch/phase7_step18_oop.py`.
- ملاحظات الإصلاح والدروس: `/memories/repo/compiler_fix_notes.md`.
- خط الأساس للاختبارات: `python tests/dual_execution/runner.py` → 793/837.
- آخر بناء نظيف: commit `f4e3121b`، sadc Release + sad Debug.
