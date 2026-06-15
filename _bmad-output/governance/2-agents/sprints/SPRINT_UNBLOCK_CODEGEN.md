# SPRINT: UNBLOCK-CODEGEN — تَفعيل تَوليد Builtins التلقائي

**الـPM:** John (صالح)  
**التاريخ:** 2026-06-01  
**المُكتشف من:** AUDIT-2026-06 + التَحقيق الجنائي post-investigation  
**يَحل المُشكلة:** P-15 (الكود مُنفَّذ لكن غير مُدمج في CMake)  
**يَستبدل الإصلاح الخاطئ لـ:** P-02 (التَباين "النوع" vs "نوع" سيَختفي بنيوياً)

## السياق
- ADR-006a (`_bmad-output/systems/living-documentation/2-architecture/decisions/`) مُعتمدة منذ 2026-05-15
- `scripts/codegen/gen_builtins.py` كامل + مُختبَر (`test_gen_builtins.py`، 10 حالات)
- `scripts/codegen/gen_all.py` Orchestrator جاهز
- ❌ **مَفقود فقط:** `add_custom_target(sad_builtins_codegen)` في `cmake/codegen.cmake`
- ❌ **مَفقود:** استبدال الثَوابت اليَدوية في `shared/builtins/include/builtin_registry.h` بـ`#include "builtin_names_generated.h"`

## النَموذج المَرجعي
`cmake/codegen.cmake` يَحوي بالفعل `sad_keywords_codegen` كنَموذج كامل — يُستنسَخ النَمط.

## الـStories

### S-UCG-01: إضافة `sad_builtins_codegen` target إلى CMake
- **Zone (write):** `cmake/codegen.cmake`
- **Zone (read):** الموجود لـ`sad_keywords_codegen`
- **معايير القبول:**
  - target يَستدعي `python scripts/codegen/gen_builtins.py`
  - يُنتج: `shared/builtins/generated/builtin_names_generated.h`
  - DEPENDS على ملفات YAML المَصدر: `data/language/builtins/*.yaml`, `data/stdlib/*.yaml`
  - يُربَط بـ`sad_check_codegen_env`

### S-UCG-02: تَوسيع `sad_all_codegen` orchestrator
- **Zone:** `cmake/codegen.cmake`
- **القبول:** `sad_all_codegen` يَعتمد على `sad_builtins_codegen` بالإضافة للأَخوات

### S-UCG-03: تَشغيل التَوليد لأول مَرة + تَحقق
- **القبول:**
  - `cmake --build build --target sad_builtins_codegen` يَنجح
  - `shared/builtins/generated/builtin_names_generated.h` مَوجود
  - يَحوي `inline constexpr std::string_view TYPE = "نوع";` (بدون ال)

### S-UCG-04: استبدال الثَوابت اليَدوية في `builtin_registry.h`
- **Zone:** `shared/builtins/include/builtin_registry.h`
- **القبول:**
  - استبدال `namespace Names::Core` المُدوَّن يَدوياً بـ`#include "../generated/builtin_names_generated.h"`
  - البناء يَنجح: `cmake --build build --config Debug --target sad`

### S-UCG-05: إصلاح `builtin_core_io.cpp:118` للاعتماد على الثَابت المُولَّد
- **Zone:** `interpreter/src/builtins/builtin_core_io.cpp`
- **القبول:**
  - تَغيير `registerBuiltinFunction("النوع", ...)` إلى `registerBuiltinFunction(Names::Core::TYPE, ...)`
  - اختبار `.ص` بسيط: `اطبع(نوع(42))` يَعمل ويُخرج "رقم"

### S-UCG-06: تَحديث ADR-006a + تَوثيق نَموذج العَمل
- **Zone (write):** ADR-006a + `_bmad-output/governance/3-code-contract/decisions/ADR-UNBLOCK-CODEGEN-2026-06-01.md`
- **القبول:**
  - تَعليم utm-6.3 الحقيقي "مكتمل" (مع الـCMake integration)
  - ADR جديد يُوثِّق دَرس "Done ≠ Integrated"
  - تَحديث `implementation_status.md`

## معايير قبول السبرنت
1. ✅ البناء يَنجح كاملاً
2. ✅ `sad.exe examples/test_simple.ص` يَعمل
3. ✅ كَتابة `اطبع(نوع(42))` تَطبع "رقم" (وليس خطأ "دالة غير معرفة")
4. ✅ لا تَباين بين YAML وcpp بعد الآن (مَنصة بنيوية)
5. ✅ Retroactive verification لجميع `Names::Core::*` بأنها مُولَّدة وليس مَكتوبة يَدوياً

## التَأثير على سبرنتات أُخرى
- ✅ **يَحل P-02 بنيوياً** (DOCS-FIX يَفقد ستوري S-DOCS-02)
- ✅ **يَستبدل P-14** (لا حاجة لسكريبت `verify_builtins_yaml.py` لأن CMake نَفسه سيَفرض التَطابق)
- ⚠️ **يَكشف ضَرورة AUDIT-DONE-FRAUD** لاحقاً (إذا utm-6.3 كَذبت، كم utm-* أُخرى كَذبت؟)

## الـDev المُعيَّن
`dev_codegen` (instance جديد، zone: `cmake/`, `scripts/codegen/`, `shared/builtins/`, `interpreter/src/builtins/`)

## ADRs المَطلوبة
- `ADR-UNBLOCK-CODEGEN-2026-06-01.md` (سيَسبق الـimplementation وفق OR-04)

## المَوعد
2 أيام (مَنخفض التَعقيد لأن 90% من العَمل مُنجَز)
