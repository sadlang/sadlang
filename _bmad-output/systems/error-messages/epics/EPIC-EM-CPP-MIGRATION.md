---
id: EPIC-EM-CPP
title: ترحيل مواقع رمي الأخطاء في C++ إلى نظام الكتالوج (ErrorCode المُولَّد)
system: error-messages
status: READY
date: 2026-06-10
author: Amelia (Dev)
governed_by:
  - ../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md
  - ../docs/ERROR_SYSTEM_GUIDE.md
---

# EPIC-EM-CPP — ترحيل أخطاء C++ إلى نظام الكتالوج

## الهدف
استبدال كل مواقع **الرمي الخام** (`throw std::runtime_error("نص")`) في كود C++ باستخدام
**رموز `ErrorCode` المُولَّدة** عبر النظام القائم (`throwRuntime` / `reportFromCatalog`) —
ليصبح **نص الرسالة في YAML وحده** (القاعدة الذهبية، راجع `ERROR_SYSTEM_GUIDE.md`).

## السياق (لماذا الآن؟)
EM-3 جعل **الكتالوج** مُولَّداً من `language-truth/errors/`، لكن **مواقع الرمي في C++** ما زالت
تكتب نصوصاً خاماً بدل الإشارة للرموز — فالتوحيد ناقص. هذا الإبيك يكمله.

## النطاق (أدلة من مسح الكود، 2026-06-10)
| الطبقة | رمي خام | ملاحظة |
|--------|:------:|--------|
| الدوال المضمنة (`interpreter/src/builtins/`) | **254** | الكتلة الكبرى — تصطدم بعقبة Position |
| `shared/builtins/src/runtime/` | **33** | builtins.cpp(26) + type_functions(4) + array_functions(3) |
| lexer | 4 | لديه location |
| parser | 3 | لديه location |
| interpreter visitors | 6 | لديه Position |
| compiler | 10 | لديه location |
| **الإجمالي** | **~310** | |

## العقبة المعمارية + الحل المعتمَد (ADR-EM-CPP-1)
lambdas الدوال المضمنة **بلا `Position`/سياق**، و`throwRuntime` يتطلب موقعاً. **الحل الأمثل
(معتمَد في [ADR-EM-CPP-1](../decisions/ADR-EM-CPP-1-BUILTIN-CONTEXT.md)):** إعادة هيكلة عقد الدوال
المضمنة لتأخذ **`BuiltinContext&`** بدل `(args)` — يحمل الوسائط + الموقع + **`ctx.error(code, placeholders)`**
(من الكتالوج مباشرةً) + قابلية التوسّع (اسم الدالة/المفسر/المكدس). النمط القياسي (Lua/V8/Python).
**ليس جسراً مؤقتاً — أساس دائم.** **استبدال نظيف بلا توافق خلفي** (اللغة غير منشورة): توقيع
`(BuiltinContext&)` هو الوحيد؛ كل الوحدات تُحوَّل في دفعة الهجرة، الدمج بعد بناء أخضر شامل.

## ⚠️ تسلسل التنفيذ: وحدة ذرّية (قرار المالك 2026-06-10)
**استبدال صارم بلا توافق خلفي.** بحذف توقيع الدوال القديم في EM-CPP-0، لا يُجمَّع المفسر حتى تُحوَّل
**كل** الدوال المضمنة. لذلك **`EM-CPP-0..6` وحدة دمج ذرّية واحدة** — البناء **أحمر** بينها، **أخضر
فقط بعد اكتمال EM-CPP-6**. لا تُدمَج ستوري من هذه المجموعة وحدها.
- **مستقلّتان (تُدمجان منفصلتين):** `EM-CPP-7` (كنس غير المضمّن، لها مواقع) + `EM-CPP-T2` (Tier 2).

## تقسيم الستوريات (مُحجَّم بالأرقام)
| الستوري | النطاق | عدد المواقع | التبعية |
|---------|--------|:-----------:|---------|
| **EM-CPP-0** (مُمكِّن) | **`BuiltinContext`** + تسجيل يدعم التوقيعين + تحديث الموزّع (يضبط الموقع) | — | — |
| **EM-CPP-1** (بايلوت) | ترحيل `builtin_module_math.cpp` للتوقيع الجديد + `ctx.error` | 21 | EM-CPP-0 |
| **EM-CPP-2** | مجموعة maps (arrays/core/text/json_xml/utils) | 97 | EM-CPP-1 |
| **EM-CPP-3** | `builtin_module_assertions.cpp` | 60 | EM-CPP-1 |
| **EM-CPP-4** | basics + async + exceptions | 34 | EM-CPP-1 |
| **EM-CPP-5** | kernel (cpu + acpi) | 42 | EM-CPP-1 |
| **EM-CPP-6** | `shared/builtins/src/runtime/*` | 33 | EM-CPP-0 |
| **EM-CPP-7** (كنس) | غير المضمّنة: lexer+parser+visitors+compiler | 23 | — (مباشر، لها مواقع) |
| **EM-CPP-T2** (ملحق Tier 2) | توحيد أخطاء الكود المُترجَم (`sad_panic` + جدول مُولَّد بلا STL) | — | — (مستقل، أولوية منخفضة) |

## نطاق المترجم (sadc) — الطبقة حسب الهدف لا حسب مفسر/مترجم
المترجم يولّد LLVM IR (لا lambdas) → `BuiltinContext` لا ينطبق عليه. لكن **رسائله غنية كالمفسر
عدا freestanding:**
- **وقت الترجمة** (`reportError` ×11) → `reportFromCatalog` (Tier 1 غني) — **EM-CPP-7**.
- **الكود المُترجَم — هدف مُضيف** → **Tier 1 غني** (runtime يرندر عبر `ErrorCatalog`).
- **الكود المُترجَم — freestanding فقط** → **Tier 2 أدنى** (`sad_panic` + جدول مُولَّد).
- نداء codegen موحَّد `sad_runtime_error(code,...)`؛ الـruntime يقرّر الغنى → ملحق **EM-CPP-T2**.
  راجع `docs/BUILTIN_CONTEXT_DESIGN.md` §9 + الدليل §3-ب.

## معايير القبول للإبيك (DoD)
- كل موقع خام يُستبدل بـ`ErrorCode` مناسب (أو رمز جديد يُضاف لـ YAML عبر `ERROR_SYSTEM_GUIDE` §6).
- **0 `throw std::runtime_error` متبقٍّ** في الطبقات المُرحَّلة (فحص grep).
- البناء أخضر + اختبار سلوكي لكل مجموعة (رسالة الخطأ تُعرَض من الكتالوج).
- لا تراجع في رسائل الأخطاء القائمة.

## قاعدة الرموز الناقصة
كثير من النصوص الخام قد لا يكون لها `ErrorCode` مطابق. لكل نص خام:
1. ابحث عن رمز مناسب موجود (`RUN_*`/`SEM_*`).
2. إن لم يوجد → أضِف رمزاً جديداً لـ `error_codes.h` + `language-truth/errors/` (الإجراء في الدليل §6).
3. وثّق الرموز الجديدة المُضافة في status النظام.

## ملاحظة حوكمة (GR-01)
الأعداد أعلاه **خطة** من مسح فعلي (grep). كل ستوري تُقفَل بدليل: grep=0 خام + بناء أخضر + اختبار.
