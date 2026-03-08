# اختبارات الانحدار / Regression Tests

> **المسؤول:** سعيد — مهندس المترجم  
> **Sprint:** 1 (أسبوع 1-2)  
> **المصدر:** [مشاكل.md](../../مشاكل.md) + اختبارات موحدة إضافية

## الهدف

اختبار **كل مشكلة موثقة** في ملف `مشاكل.md` (22 مشكلة) + اختبارات موحدة إضافية لضمان عدم ظهور الأعطال بعد الإصلاح.

## الملفات

| # | الملف | المشكلة | الأولوية |
|---|-------|---------|----------|
| 1 | `test_p01_array_key_access.ص` | `arr[i]["key"]` crash داخل الدوال | P0 |
| 2 | `test_p02_while_update.ص` | `متغير x = x + 1` حلقة لانهائية | P0 |
| 3 | `test_p03_length_depth.ص` | `.الطول()` crash عمق ≥ 2 | P0 |
| 4 | `test_p04_hamza_var.ص` | `إ` في اسم متغير SEM001 | P1 |
| 5 | `test_p05_mixed_chars.ص` | خلط لاتيني-عربي SEM004 | P2 |
| 6 | `test_p06_large_numbers.ص` | تدوين علمي للأعداد الكبيرة | P2 |
| 7 | `test_p07_array_pluseq.ص` | `+=` للمصفوفات | P2 |
| 8 | `test_p08_break.ص` | غياب `توقف` في الحلقات | P1 |
| 9 | `test_p09_reserved_words.ص` | `نوع`/`حجم` محجوزتان | P0 |
| 10 | `test_p10_void_return.ص` | `ارجع` بدون قيمة | P0 |
| 11 | `test_p11_custom_class_fields.ص` | أصناف مخصصة كأنواع حقول | P0 |
| 12 | `test_p12_integer_division.ص` | القسمة تنتج float دائماً | P0 |
| 13 | `test_p13_class_prefix.ص` | `صنف` كبادئة معرّف | P1 |
| 14 | `test_p14_cascading_errors.ص` | أخطاء متسلسلة | P1 |
| 15 | `test_p15_utf8_windows.ص` | ترميز UTF-8 في Windows | P2 |
| 16 | `test_p16_compound_field.ص` | `+=` على حقل كائن في مصفوفة | P0 |
| 17 | `test_p17_undocumented_reserved.ص` | `جديد`/`سجل` محجوزتان | P1 |
| 18 | `test_p18_this_method_save.ص` | `هذا.method()` لا يحفظ | P0 |
| 19 | `test_p19_hex_literals.ص` | hex literals `0xFF` | P0 |
| 20 | `test_p20_super_reserved.ص` | `أساس` محجوزة | P1 |
| 21 | `test_p21_template_reserved.ص` | `قالب` محجوزة | P2 |
| 22 | `test_p22_true_literal.ص` | `صحيح` ليس معرّف | P2 |
| 24 | `test_p24_json_unified_builtin.ص` | JSON unified entry points | P1 |
| 25 | `test_p25_xml_unified_builtin.ص` | XML unified entry points | P1 |
| 26 | `test_p26_json_invalid_input.ص` | JSON invalid input (negative test) | P1 |
| 27 | `test_p27_xml_invalid_input.ص` | XML invalid input (negative test) | P1 |
| 28 | `test_p28_filesystem_smoke.ص` | Filesystem read/write/delete smoke | P1 |
| 29 | `test_p29_filesystem_invalid_input.ص` | Filesystem invalid path (negative test) | P1 |
| 30 | `test_p30_database_availability.ص` | Database availability/import contract | P1 |

## التشغيل

### المفسر (sad.exe)
```powershell
.\run_regression_tests.ps1 -SadExe .\build\bin\Debug\sad.exe -Mode interpreter
```

### المترجم (sadc.exe)
```powershell
.\run_regression_tests.ps1 -SadExe .\build\bin\Release\sadc.exe -Mode compiler
```

### Linux/macOS
```bash
./run_regression_tests.sh ./build/bin/sad interpreter
```

## الحالات

- **PASS** — الاختبار نجح (المشكلة مُصلحة)
- **XFAIL** — الاختبار فشل كما هو متوقع (المشكلة لم تُصلح بعد)
- **FAIL** — فشل غير متوقع (regression!)

## المخرجات المتوقعة / Expected Output

- لكل ملف `test_pNN_*.ص`: يجب أن يحتوي الخرج على سطور `PASS:`
- يجب أن ينتهي الخرج بالسطر: `=== انتهى اختبار #NN ===`
- أي سطر `FAIL:` يعني فشل الاختبار

### حالات Green المؤكدة

- `test_p01_array_key_access.ص` ✅
- `test_p02_while_update.ص` ✅ (خطأ #2 أصبح Green رسميًا)
- `test_p03_length_depth.ص` ✅
- `test_p05_mixed_chars.ص` ✅
- `test_p06_large_numbers.ص` ✅
- `test_p15_utf8_windows.ص` ✅
- `test_p22_true_literal.ص` ✅
