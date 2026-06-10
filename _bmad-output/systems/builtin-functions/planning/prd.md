---
id: PRD-BUILTINS-V1
title: "متطلبات نظام الدوال المدمجة (Builtins Codegen)"
version: 1.0
status: APPROVED
date: 2026-06-06
---

# PRD: نظام الدوال المدمجة — Builtins Codegen

## 1. المشكلة

`shared/builtins/include/builtin_registry.h` مكتوب يدوياً ويحتوي على:
- أسماء 107 دالة مدمجة بصيغة `constexpr string_view`
- 4 مصفوفات ثابتة (CORE_IO×3، TYPE_CONSTRUCTOR×4، MODULE_FUNCTION×68، TYPE_METHOD×32)
- كل تغيير في اسم دالة يتطلب تعديل يدوي في ملف C++

**التأثير:** LSP، formatter، compiler، REPL يجب أن تُحدَّث يدوياً عند كل تغيير.

---

## 2. الهدف

**توليد `builtin_registry.h` تلقائياً** من `language-truth/builtins/*.yaml` وقت البناء (cmake),
بحيث:
- إضافة دالة جديدة → تعديل YAML فقط
- الكود المُولَّد يحافظ على نفس الـ namespace structure تماماً (`Sad::Builtins::Names::X`)
- صفر تغيير في باقي كود اللغة

---

## 3. النطاق

### داخل النطاق:
- YAML لكل namespace (Core, TypeCtor, Concurrency, Math, Strings, Arrays, Basics, Assertions, Maps, Sockets, HTTP, WebSocket)
- `gen_builtins_registry.py` (أداة توليد جديدة مختلفة عن `gen_builtins.py`)
- cmake target جديد: `sad_builtin_registry_codegen`
- إنتاج `builtin_registry_generated.h` يحل محل `builtin_registry.h`
- إنتاج `module_definitions_generated.h` من `language-truth/stdlib/modules.yaml`

### خارج النطاق:
- تعديل `expression_evaluator_oop_*.cpp` (يتبع M3-TypeMethods)
- تعديل منطق المفسر/المترجم (يبقى كما هو)

---

## 4. المتطلبات الوظيفية

| الرقم | المتطلب | الأولوية |
|-------|---------|---------|
| FR-001 | كل دالة: `cpp_id` + `canonical` + `namespace` + `category` + `require_import` + `since` + `status` | P0 |
| FR-002 | اسم عربي واحد فقط لكل دالة — لا aliases | P0 |
| FR-003 | الـ namespace في YAML يطابق namespace في C++ (`Names::Core`) | P0 |
| FR-004 | `gen_builtins_registry.py` ينتج نفس بنية `builtin_registry.h` | P0 |
| FR-005 | static_assert يُولَّد تلقائياً للتحقق من الأعداد | P1 |
| FR-006 | cmake يُعيد التوليد عند تغيير أي YAML | P0 |
| FR-007 | بناء يبقى أخضر — لا كسر في أي مكون | P0 |

---

## 5. القيود

- **CW-24:** أسماء الدوال ثابتة — تغيير cpp_id موجود ممنوع
- **CW-08:** كل namespace يحوي تعليق عربي + إنجليزي
- اسم واحد فقط لكل دالة — لا aliases في YAML

---

## 6. تعريف "تم"

- `language-truth/builtins/*.yaml` يحوي جميع الدوال الـ107
- `gen_builtins_registry.py` ينتج `builtin_registry_generated.h` بدلاً من `builtin_registry.h`
- `cmake --build build --target sad` أخضر
- الأعداد: CORE_IO=3، TYPE_CONSTRUCTOR=4، MODULE_FUNCTION=68، TYPE_METHOD=32
- T1 Schema + اختبار diff المخرج

---

## 7. المخاطر

| الخطر | الاحتمال | التأثير | التخفيف |
|-------|---------|---------|---------|
| كسر namespace path | متوسط | عالٍ | اختبار diff صارم |
| فقدان static_assert | منخفض | متوسط | توليد static_assert في الـ header |
| ترتيب غلط للدوال | منخفض | منخفض | ترتيب في YAML يطابق الترتيب الحالي |
