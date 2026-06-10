---
title: "نظام الدوال المدمجة (Builtins System)"
englishName: "builtin-functions"
date: 2026-06-07
status: ACTIVE
owner: فريق لغة ص
relatedDocuments:
  - planning/prd.md
  - planning/architecture.md
  - planning/HOW_TO_ADD_BUILTIN.md
  - planning/BUILTIN_SYSTEM_DIAGRAM.md
  - status/implementation_status.md
purpose: |
  توليد builtin_registry.h تلقائياً من YAML بدل الكتابة اليدوية،
  بحيث تستخدم جميع مكونات اللغة (مفسر/مترجم/VM/LSP) ثوابت من مصدر واحد،
  مع قاعدة صارمة: اسم عربي واحد فقط لكل دالة (الثابت) بلا أسماء بديلة.
---

# نظام الدوال المدمجة (Builtins System)

## نظرة عامة

`shared/builtins/include/builtin_registry.h` أصبح **wrapper** يَضمّ ملفاً مُولَّداً تلقائياً (`generated/builtin_registry_generated.h`) من `language-truth/builtins/*.yaml`. إضافة دالة مدمجة جديدة أو تعديل اسمها تتم **بتعديل YAML فقط**، ثم يُعيد CMake التوليد عند البناء.

**القاعدة الذهبية:** اسم عربي واحد فقط لكل دالة = الثابت المُولَّد. **بلا أسماء بديلة** (لا `الثابت || "نص"`).

## الحالة الحالية (مُحقَّقة بالأدلة — GR-01)

| البند | القيمة | الدليل (أمر تحقق) |
|---|---|---|
| النسبة المنجَزة | **100%** | راجع الجدول أدناه |
| ملفات YAML للدوال | 41 ملف | `ls language-truth/builtins/*.yaml \| wc -l` → 41 |
| ثوابت مُولَّدة | 1211 ثابت | `grep -c 'inline constexpr std::string_view' …_generated.h` → 1211 |
| المفسر — نصوص عربية متبقية | 0 (كود) | `grep -rlE 'registerBuiltinFunction\("[؀-ۿ]' interpreter/src/builtins/` → MD فقط |
| المترجم — builders بنصوص | 0 | `grep -rlE 'funcName == "[؀-ۿ]' compiler/src/frontend/builders/` → 0 |
| البناء | أخضر | `cmake --build build --target sad` → نجح |

### تفصيل النسبة (100%)

| المكوّن | النسبة | ملاحظة |
|---|---|---|
| توليد الـ header من YAML | 100% | `gen_builtins_registry.py` + cmake target |
| المفسر (interpreter) | 100% | جميع `builtin_*.cpp` + `expression_evaluator_oop_*` |
| المترجم (compiler/frontend) | 100% | جميع `builders/builtins_*` + `method_call_*` |
| **الآلة الافتراضية (VM)** | **100%** | مُحوَّلة لثوابت `Bn::Core::*` ✅ |

> **النسبة 100%** = المسارات الثلاثة (مفسر + مترجم + VM) تستخدم الثوابت المُولَّدة بالكامل.
>
> **ملاحظة (خارج نطاق دوال البناء):** جداول إكمال LSP تتبع نطاق M2-Library (طبقة عرض، تستهلك البيانات المُولَّدة لاحقاً).

## روابط سريعة

- 📋 [PRD](planning/prd.md)
- 🏗️ [Architecture](planning/architecture.md)
- 🖼️ [مخطط النظام بالرسومات](planning/BUILTIN_SYSTEM_DIAGRAM.md)
- ➕ [كيفية إضافة دالة مدمجة](planning/HOW_TO_ADD_BUILTIN.md)
- 🔧 [خطة إزالة النصوص](planning/REFACTORING_PLAN.md)
- 📊 [Implementation Status](status/implementation_status.md)
- 📁 [Stories](stories/)
- 📐 [Decisions (ADRs)](decisions/)

## كيفية المساهمة

1. اقرأ [PRD](planning/prd.md) لفهم المتطلبات.
2. راجع [Architecture](planning/architecture.md) و[المخطط](planning/BUILTIN_SYSTEM_DIAGRAM.md).
3. لإضافة دالة: اتبع [HOW_TO_ADD_BUILTIN.md](planning/HOW_TO_ADD_BUILTIN.md).
4. اتبع قواعد المساهمة في copilot-instructions.md.
