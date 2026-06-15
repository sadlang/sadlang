---
title: تقدّم EM-CPP — ترحيل أخطاء الدوال المضمنة لـ BuiltinContext
date: 2026-06-11
status: ACTIVE (الجزء الأكبر مكتمل)
---

# تقدّم EM-CPP

## المُنجَز (مدموج)
| PR | المحتوى | المواقع |
|----|---------|:------:|
| #17 | EM-3 تكامل الكتالوج + تنظيف 11 ملفاً ميتاً | — |
| #18 | رفع حاجز codegen (فقدان بيانات) + أساس BuiltinContext (shim) | — |
| #19 | math | 21 |
| #20 | maps arrays/core/utils | 59 |
| #21 | assertions | 54 |
| #22 | basics/async/exceptions | 34 |
| #23 | kernel cpu/acpi | 42 |
| #24 | مراجعة أميليا: {func}/{builtin} placeholders | — |
| **الإجمالي** | **~210 موقع → ctx.error** | **~83%** |

## المتبقّي (الفئة الصعبة — deferred)
| الملف | المتبقّي | السبب | المعالجة المستقبلية |
|------|:---:|------|----------------------|
| maps_text | 20 | regex helpers + HOF lambda داخلي | تحويل lambdas المُسجَّلة فقط |
| maps_json_xml | 17 | محلّلات JSON/XML static (بلا ctx) | تمرير ctx/position للمحلّلات أو إبقاء خام |
| assertions | 6 | hexToBytes helper (2 مقصود) + 4 ديناميكي | تحويل الديناميكي للامبدا |
| maps_utils | 1 | `خطأ` (رمي مستخدم — مقصود) | يبقى خام |
| shared/builtins/runtime | 33 | طبقة أدنى `Sad::StdLib` (بلا ctx) | آلية خطأ منفصلة للطبقة الأدنى |

## النمط المُثبَت (للمتبقّي)
سكربت متين: توقيع مرن (`> ?& ?args`) + حقن مرن (`[^{]*\{`) + تصنيف دلالي
(REQUIRES_ARG/FUNCTION/EMPTY_OPERATION/TYPE_CHECK/FILE_ERROR/ASSERTION_FAILED/PANIC).
القاعدة: تحويل throws **داخل lambdas مُسجَّلة فقط** (ctx متاح)؛ ترك helpers/HOF/lower-layer خام.

## حاجز خارجي مكتشَف
[BUG-IMPORT-BUILTIN-MODULE](../../../bugs/BUG-IMPORT-BUILTIN-MODULE-NOT-LOADING.md):
`استورد <وحدة>` لا يُحمّل الوحدات المُبوّبة → الدوال المُرحَّلة **غير قابلة للوصول للمستخدم حالياً**.
الترحيل صحيح بنيوياً؛ يصبح قابلاً للاختبار السلوكي بعد إصلاح علة الاستيراد.

## حذف النوع القديم (`nativeImplementation_`)
مؤجَّل حتى تحويل **كل** lambdas الدوال المضمنة (المتبقّي + التأكّد من عدم وجود lambdas قديمة في ملفات أخرى).
الـ shim يُبقي البناء أخضر حتى ذلك الحين.
