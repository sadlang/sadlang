# Story EM-2 — المولِّد + CMake + Baseline

**Epic**: EPIC-EM | **Story Points**: 5 | **الأولوية**: حرجة | **الحالة**: ✅ مكتملة
**التبعيات**: EM-1 | **المُسنَد إلى**: Amelia

---

## الهدف
بناء `scripts/codegen/gen_error_messages.py` + تكامل CMake + إنتاج baseline files ملتزمة في git.

## التنفيذ

- [x] **T1** — كتابة `scripts/codegen/gen_error_messages.py`
  - [x] استيراد `_lib.loader` و `_lib.emit`
  - [x] `extract_enum_values(error_codes.h)`
  - [x] `validate_semantically(data, enum_values)` — 6 تحقُّقات
  - [x] `sort_entries(data)`
  - [x] `emit_header(data)`
  - [x] `emit_source(data)` مع lookup map (CW-26)
  - [x] CLI args كاملة
  - [x] نص فارغ → `nullptr` (D-3)

- [x] **T2** — `test_gen_error_messages.py` — 13 اختبار تمرّ (غطّى AC-2.5 الثمانية)

- [x] **T3** — baseline files: `.h` (2491B) + `.cpp` (99919B)

- [x] **T4** — تكامل CMake: `sad_error_messages_codegen` + إضافته إلى `sad_all_codegen`
  - ⚠️ ربط `.cpp` بـ `sad_shared` مُؤجّل إلى EM-3 حفاظاً على baseline (BF-29)

- [x] **T5** — `cmake --build build --config Debug --target sad_error_messages_codegen` → نجح

- [x] **T6** — `pytest scripts/codegen/test_gen_error_messages.py` → **13/13 PASSED**

## معايير القبول (AC)

- **AC-2.1**: ✅ `gen_error_messages.py` ينتج `.h` + `.cpp` صحيحَين.
- **AC-2.2**: ✅ المولِّد يفرض كل التحقُّقات الدلالية الـ6.
- **AC-2.3**: ✅ `sad_error_messages_codegen` target يُبنى بنجاح.
- **AC-2.4**: ✅ baseline files ملتزمة في git.
- **AC-2.5**: ✅ كل اختبارات pytest تنجح.
- **AC-2.6**: ✅ ترتيب `kErrorMessages[]` = category ثم id (CW-27).
- **AC-2.7**: ✅ نص فارغ في YAML → `nullptr` في C++ (D-3).
- **AC-2.8**: ✅ المولِّد idempotent (تشغيل مرتين = نفس الإخراج).

## File List
- (سيُملأ)

## Dev Agent Record
- (سيُملأ)
