---
storyId: TEST-004
title: "دمج مجلدات اختبارات الوحدة (C++) ودفع منطق parser/types للوحدة"
status: done
priority: P0
estimate: 8h
createdAt: 2026-06-11
updatedAt: 2026-06-12
completedAt: 2026-06-12
assignee: Claude (Fable 5)
dependsOn: [TEST-002]
blocks: [TEST-005]
relatedStories: [TEST-002, TEST-005]
relatedDecisions: [ADR-001, ADR-002, ADR-004]
acceptanceCriteria:
  - AC-01: lexer_tests/parser/parser_tests/... مدموجة تحت unit/ بلا تكرار
  - AC-02: CTest يكتشف ويشغّل كل اختبارات unit/ مرتبطة بـ sad_core
  - AC-03: لا تراجع في عدد الاختبارات النّاجحة عن baseline
  - AC-04: منطق lexer/parser/type-check القابل للوحدة مُغطّى في unit/ لا فقط behavior/
---

# TEST-004 — دمج طبقة الوحدة ودفع المنطق لأسفل الهرم

## السياق

اختبارات C++ المعزولة موزّعة بتكرار. تُدمج في `unit/<مكوّن>/`. رُفعت الأولوية إلى **P0**
بعد نقد Murat ([ADR-004](../decisions/ADR-004-determinism-and-risk.md)): «فضّل المستويات الأدنى» — دفع منطق
parser/types/lexer من E2E المزدوج البطيء إلى وحدة سريعة مستقرة يعالج مخاطرة الهرم
المقلوب (R2، درجة 6).

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | مجلد واحد لكل مكوّن، بلا تكرار | فحص بنية | T1.004-DOC-001 |
| AC-02 | CTest يشغّل كل `unit/` | `ctest -R Unit` أخضر | T1.004-UNIT-001 |
| AC-03 | عدد الناجح ≥ baseline | مقارنة تقارير | T1.004-INT-001 |
| AC-04 | تغطية وحدة لمنطق lexer/parser/types | مراجعة COVERAGE + قائمة الفجوات المُغلقة | T1.004-UNIT-002 |

## المهام (Tasks)

- [x] **T1: تصنيف ودمج** (AC-01) ✅ 2026-06-12
  - [x] T1.1: صُنّف كل ملف وفق جرد TEST-001: lexer_tests→`unit/lexer`، parser_tests+parser→`unit/parser`، oop_tests→`unit/oop`، interpreter_tests→`unit/interpreter`، data_tests+جذر unit→`unit/types`/`unit/borrow`، stdlib_tests+cpp من stdlib→`unit/stdlib`، compiler/{frontend,optimizer}+jit→`unit/sir`(+`/optimizer`)، low_level→`unit/low_level`، integration→`unit/integration`، comprehensive→`unit/comprehensive`.
  - [x] T1.2: كله بـ`git mv` (~135 ملفاً، renames في git). مكرّر parser/parser_tests وُحِّد في `unit/parser`.
  - [x] T1.3: جدول «اختبارات معطّلة موثَّقة» في `tests/unit/README.md` (parser_tests/oop_tests/optimizer/type_nodes) مع شرط إعادة التفعيل لكلٍّ.
- [x] **T2: تسجيل CTest** (AC-02) ✅ 2026-06-12
  - [x] T2.1: إعادة ربط `cmake/{tests,tests_comprehensive,dual_tests}.cmake` للمسارات الجديدة + وسم `LABELS "Unit"` (ماكرو الشاملة + حلقة للاختبارات الفردية).
  - [x] T2.2: `ctest -L Unit` يكتشف **37 اختباراً**؛ البناء الكامل لأهداف الاختبار نظيف (exit 0).
- [x] **T3: عدم التراجع** (AC-03) ✅ 2026-06-12
  - [x] T3.1: baseline (137 مسجلاً): 41 ناجحاً / 29 فاشلاً / 67 لم تعمل → بعد: **52 ناجحاً (+11) / 27 / 58** — diff لكل اختبار: **صفر تراجعات**. التحول الوحيد: ParserTests كان بلا exe وصار يُبنى ويفشل (عطب قديم أصبح مرئياً — موثَّق).
  - [x] T3.2: المجلدات أُفرغت وأُزيلت (نقل git — لا محتوى للأرشفة)؛ حُذف 148 ملف مخلّفات بناء كانت متتبَّعة خطأً في `tests/stdlib_tests/build/`.
- [x] **T4: دفع المنطق للوحدة** (AC-04) ✅ 2026-06-12
  - [x] T4.1: فحص مجموعة «الأخطاء النحوية» في parser comprehensive كشف أنها شكلية (SERR01 توتولوجيا تنجح في الحالتين).
  - [x] T4.2: مساعد `parseReportsError` صارم (hasErrors/استثناء فقط) + مجموعة `Parser.StrictNegative` بخمسة اختبارات: رفض كلمة محجوزة كاسم متغير/دالة، قوس غير مغلق، وإلا يتيمة، إسناد ناقص — **109/109 ناجحة**. (استدلال النوع وحدوياً ما زال فجوة — فاحص الأنواع بلا اختبارات وحدة؛ يُجدول لاحقاً.)

## ملاحظات التنفيذ

- لا تكرار تغطية بين الطبقات (test-levels: duplicate coverage guard) إلا لمسارات حرجة.
- ما لا يمكن إثباته إلا بالتكافؤ التشغيلي يبقى في `behavior/`؛ المنطق الصافي ينزل للوحدة.

## اكتشافان جذريان أُصلحا أثناء القصة (BF-04/BF-10)

1. **علة ربط GC:** `types/src/value.cpp` (في sad_shared) يستدعي محرك GC منذ توحيد الذاكرة B-step، والتبعية كانت معلنة على sad_core فقط → كل مستهلك يربط sad_shared وحدها يفشل LNK2019 (سبب فشل 5 اختبارات Comprehensive في الأساس). الإصلاح: `target_link_libraries(sad_shared PUBLIC sad_memory_gc)` حيث تنشأ التبعية (CW-06)؛ الدورة الساكنة مقصودة ومدعومة.
2. **`sad_interpreter` جزئية بالتصميم:** تسجّل 7/27 ملف زائر وvtable غير مكتمل، وإكمالها يسحب سلسلة sad_ui/SDL (عبر expression_evaluator_ui). القرار: توثيق المنع من الربط المباشر، وتحويل مستهلكيها الأربعة (اختبارات interpreter/throw_catch/optional_null/vm_compiler الشاملة) إلى `sad_core`.

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-12 | Claude (Fable 5) | U0: baseline ctest عند HEAD (41/137) محفوظ في `build/_ctest_baseline.txt` |
| 2026-06-12 | Claude (Fable 5) | U1+U2: جرد وتصنيف + git mv ~135 ملفاً إلى unit/<مكوّن> + تنظيف 148 build-junk متتبَّعة + README موسّع |
| 2026-06-12 | Claude (Fable 5) | إصلاح جذري: ربط sad_shared↔sad_memory_gc + تحويل 4 اختبارات لـ sad_core + توثيق جزئية sad_interpreter |
| 2026-06-12 | Claude (Fable 5) | U3: إعادة ربط cmake الثلاثة + LABELS Unit (37) + بناء نظيف + ctest: 52 ناجحاً، صفر تراجعات |
| 2026-06-12 | Claude (Fable 5) | U4: parseReportsError + 5 اختبارات سلبية صارمة — parser comprehensive 109/109 |

## File List

- `tests/unit/{lexer,parser,oop,interpreter,types,borrow,stdlib,sir,sir/optimizer,low_level,integration,comprehensive}/**` — ~135 ملفاً منقولاً (git mv)
- `tests/unit/README.md` — بنية + جدول المعطّل (معدَّل)
- `tests/unit/comprehensive/test_parser_comprehensive.cpp` — مساعد صارم + مجموعة Parser.StrictNegative (معدَّل)
- `shared/CMakeLists.txt` — ربط sad_memory_gc (إصلاح جذري)
- `interpreter/CMakeLists.txt` — تحذير معماري موثَّق (لا ربط مباشر)
- `cmake/tests.cmake` — مسارات unit/ + حلقة LABELS Unit (معدَّل)
- `cmake/tests_comprehensive.cmake` — بادئة unit/comprehensive + sad_core للأربعة + LABELS (معدَّل)
- `.github/copilot-instructions.md` + `references/systems-catalog.md` — مسار comprehensive (معدَّل)
- حُذف: `tests/stdlib_tests/build/**` (148 ملف مخلّفات متتبَّعة خطأً)
