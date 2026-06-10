---
status: ACTIVE
lastUpdated: 2026-06-07
completionPercentage: 100
---

# حالة التنفيذ — نظام Builtins Codegen

## المرحلة 1: توليد builtin_registry.h من YAML ✅ (100%)

| المكوّن | الحالة |
|---------|--------|
| 37 ملف YAML في `language-truth/builtins/` | ✅ |
| `gen_builtins_registry.py` | ✅ |
| `builtin_registry_generated.h` (Names + TypeMethods) | ✅ |
| `builtin_registry.h` → wrapper | ✅ |
| cmake target `sad_builtin_registry_codegen` | ✅ |
| `#undef` ماكروز Windows (DELETE/UNION/IN/OUT...) | ✅ |

## المرحلة 2: ربط الكود بالثوابت (اسم واحد = ثابت، بلا أسماء بديلة)

### المفسر (interpreter/) — مكتمل 100% ✅
- `builtin_*.cpp` (جميعها): تسجيل بثوابت `Bn::*`
- `expression_evaluator_oop_*.cpp`: طرق بثوابت `TM::*` **بلا أسماء بديلة**
- `builtin_kernel_*.cpp` (7 ملفات): 219 دالة

### المترجم (compiler/) — مكتمل 100% للدوال المدمجة ✅
| الملف | التحويلات |
|-------|-----------|
| builtins_core.cpp | 8 (Bn::Core/TypeCtor/Basics) |
| method_call_array_basic.cpp | 14 (TM::Array) |
| method_call_array_hof.cpp | 4 (TM::Array HOF) |
| method_call_string_map.cpp | 14 (TM::String/Map) |
| builtins_io_utils.cpp | 19 |
| builtins_os_core.cpp | 21 |
| builtins_os_hardware.cpp | 29 |
| builtins_os_system.cpp | 33 |
| builtins_security.cpp | 14 |
| builtins_simd.cpp | 6 |
| builtins_system.cpp | 24 |
| builtins_embedded.cpp | 18 |
| builtins_uefi.cpp | 70 |
| builtins_ui.cpp | 41 |

**YAML مترجم جديد:** CompilerIo/CpuCtl/Hw/Sys/Sec/Simd/Mem/Embed/Uefi/Ui (10 ملفات)

### إصلاح جوهري: تطبيع الحركات
الـ lexer يحذف الحركات (U+064B-065F). طُبِّعت الأسماء القانونية في YAML:
`قسّم→قسم، عيّن→عين، شفّر→شفر، عدّ→عد، البُعد→البعد` لتطابق مخرجات الـ lexer.
(أصلح علة كامنة: الطرق ذات الشدة لم تكن تعمل قط)

## خارج النطاق (أنظمة language-truth منفصلة)

هذه ليست دوالاً مدمجة — تتبع أنظمة أخرى لها YAML خاص:

| الملف | الفئة | النظام |
|-------|-------|--------|
| backend/llvm/builders/oop/functions2_ops.cpp | سمات LLVM (مضمن_دائماً/بارد) | attributes |
| backend/llvm/builders/oop/classes_vtables_ops.cpp | كلمات (باني/هدم) | keywords.yaml |
| backend/llvm/llvm_atomics.cpp | عمليات ذرية (تحميل) | directives.yaml |
| expression_lowlevel.cpp | أسماء أنواع (رقم/نص) | types.yaml |
| expression_basic.cpp | حرفيات (صحيح) | keywords.yaml |
| backend/pipeline_codegen.cpp | ترميز تالف (علة سابقة) | — |
| tools/lsp/.../completion.cpp | جداول إكمال LSP | M2-Library |

## الآلة الافتراضية (VM) — مكتمل 100% ✅

| البند | الدليل |
|-------|--------|
| تسجيلات بنص عربي | `grep -cE 'سجّل_دالة_أصلية\("[^"]*[؀-ۿ]' vm/src/sad_vm_executor.cpp` → **0** |
| التحويلات | 11 تسجيل → ثوابت `Bn::Core::*` / `Bn::TypeCtor::*` |
| الستوري | [S-BF-R02-VM.md](../stories/S-BF-R02-VM.md) ✅ |
| البناء | `cmake --build build --target sad_vm` → أخضر |
| أسماء VM خاصة | ادخل/لنص/لرقم/لعشري أُضيفت لـ core.yaml (INPUT/TO_STRING_L/…) |

## الإجمالي: المسارات الثلاثة (مفسر + مترجم + VM) مُحوَّلة 100% ✅

> **القاعدة المُطبَّقة:** اسم عربي واحد فقط لكل دالة = الثابت المُولَّد. بلا أسماء بديلة.

## وثائق مرجعية
- [HOW_TO_ADD_BUILTIN.md](../planning/HOW_TO_ADD_BUILTIN.md) — دليل إضافة دالة (مفسر/مترجم/VM)
- [BUILTIN_SYSTEM_DIAGRAM.md](../planning/BUILTIN_SYSTEM_DIAGRAM.md) — مخطط النظام بالرسومات

## أدوات Codegen
- gen_builtins_registry / gen_compiler_builtins_yaml / gen_kernel_yaml
- convert_compiler_builtins / convert_kernel_files
- remove_aliases / collapse_method_conditions / normalize_canonical
- decode_builtins / replace_hex_strings / extract_kernel_names
