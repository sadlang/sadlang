---
status: done
slug: utm-6-4-gen-builtins-stdlib
created: 2026-06-09
completed: 2026-06-09
scope: UTM-6.4 — ترحيل Math + Strings + Arrays + Basics + Assertions + Maps (162 ثابت)
epic: UTM-6 (codegen unification — ADR-006a)
priority: high
---

# UTM-6.4 — ترحيل مكتبات stdlib القياسية (162 ثابت)

## الهدف
ترحيل 6 namespaces من `builtin_registry.h` إلى YAML files مُولَّدة تلقائياً:
- `Math` — 33 ثابت
- `Strings` — 21 ثابت
- `Arrays` — 10 ثوابت
- `Basics` — 10 ثوابت
- `Assertions` — 5 ثوابت
- `Maps` — 83 ثابت
- **المجموع: 162 ثابت**

## معايير القبول (AC)

- [ ] AC-1: 6 ملفات YAML جديدة في `data/language/builtins/` — كل واحد صالح بالـ schema
- [ ] AC-2: `gen_builtins.py` يدعم `--yaml` متعدد (`nargs='+'`) مع cross-file duplicate check
- [ ] AC-3: `builtin_names_generated.h` يحتوي جميع 7 namespaces (3 من core + 6 جديدة = 177 ثابت)
- [ ] AC-4: `builtin_registry.h` — حُذفت 6 namespaces يدوية + أُضيف تعليق "مُرحَّلة"
- [ ] AC-5: `gen_all.py` يمرر كل 7 YAML files لـ gen_builtins.py (4 generators تبقى)
- [ ] AC-6: `cmake/codegen.cmake` — sad_builtins_codegen يُراقب 7 ملفات YAML
- [ ] AC-7: pytest ≥ 70 passed (63 موجودة + اختبارات UTM-6.4 جديدة)
- [ ] AC-8: `cmake --build --target sad` — EXIT_CODE=0

## المهام

### T1: 6 YAML files + تحديث schema (إن لزم)
- إنشاء `stdlib_math.yaml` (33 ثابت)
- إنشاء `stdlib_strings.yaml` (21 ثابت)
- إنشاء `stdlib_arrays.yaml` (10 ثوابت)
- إنشاء `stdlib_basics.yaml` (10 ثوابت)
- إنشاء `stdlib_assertions.yaml` (5 ثوابت)
- إنشاء `stdlib_maps.yaml` (83 ثابت)

### T2: تحديث gen_builtins.py + gen_all.py + cmake
- `--yaml` → `nargs='+'` مع cross-file duplicate validation
- `gen_all.py`: تمرير كل 7 ملفات YAML
- `cmake/codegen.cmake`: قائمة 7 ملفات كـ DEPENDS

### T3: تحديث builtin_registry.h
- حذف 6 namespaces يدوية (Math, Strings, Arrays, Basics, Assertions, Maps)
- إضافة تعليق UTM-6.4

### T4: اختبارات + تحقق
- إضافة اختبارات multi-yaml في `test_gen_builtins.py`
- تشغيل pytest — ≥ 70 passed
- بناء C++ — EXIT_CODE=0
- diff=0 لكل 162 ثابت مُرحَّل

## ملاحظات التنفيذ
- `gen_builtins.py`: بدل `load_yaml()` واحد → loop على list of yamls، merge functions، cross-file duplicate check
- تحقق من عدم تكرار CHAR_CODE (موجود في Strings وMaps) — يجب أن تكون في نفس الـ cpp_id
- Maps::CHAR_CODE != Strings::CHAR_CODE (نفس الاسم لكن namespace مختلف — مسموح)
- Maps::JOIN != Arrays::... (JOIN في Maps بقيمة "ربط" — مختلف عن Strings::JOIN = "دمج")
- cross-file: التحقق من عدم تكرار (cpp_id, namespace) و (canonical, namespace) معاً
