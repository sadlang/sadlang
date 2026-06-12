# الطبقة 1 — اختبارات الوحدة (Unit / C++)

> اختبارات تتحقق من **مكوّن C++ معزول** (Lexer/Parser/AST/SIR/Value/Interpreter) دون
> تشغيل برنامج `.ص` كامل. سريعة، مستقرة، ترتبط بمكتبة `sad_core`، وتُشغَّل عبر CTest.

## متى يقع الاختبار هنا؟

- منطق صافٍ يمكن عزله: قواعد المحلل النحوي، استدلال الأنواع، رفض الكلمات المحجوزة،
  تحويلات SIR، عمليات `Value`.
- **القاعدة (ADR-004 / test-levels):** «فضّل المستويات الأدنى». أي منطق يمكن إثباته
  هنا **لا** يُترك للتنفيذ المزدوج البطيء في `behavior/`.

## البنية (بعد دمج TEST-004 — 2026-06-12)

| المجلد | المكوّن | أصله قبل الدمج |
|---|---|---|
| `lexer/` | المحلل المعجمي + الكلمات المفتاحية | `tests/lexer_tests` |
| `parser/` | المحلل النحوي | `tests/parser_tests` + `tests/parser` |
| `oop/` | إقرارات الأصناف وسكّر OOP | `tests/oop_tests` |
| `ast/` | عقد الشجرة | — (فارغ — ينتظر T4) |
| `types/` | `Value` / مدراء البيانات / الجسر | `tests/data_tests` + جذر unit |
| `sir/` | بناء SIR + LLVM + المحسّن (`sir/optimizer/`) | `tests/compiler/{frontend,optimizer}` + `tests/jit` |
| `interpreter/` | نواة المفسر | `tests/interpreter_tests` |
| `stdlib/` | المكتبة القياسية (C++) | `tests/stdlib_tests` + cpp من `tests/stdlib` |
| `low_level/` | عمليات ثنائية ومنخفضة المستوى | `tests/low_level` |
| `integration/` | تكامل C++ (مراجعة: بعضها e2e → TEST-005) | `tests/integration` |
| `comprehensive/` | **الحزمة الشاملة (~900 تأكيد)** — منظمة بملف-لكل-مكوّن | `tests/comprehensive` |
| `errors/`, `borrow/`, `macros/`, `patterns/`, ... | أنظمة أخرى | كانت هنا أصلاً |

## ⚠️ اختبارات معطّلة موثَّقة (لا تُحذف — GR-04)

| الاختبار | المكان | سبب التعطيل | شرط إعادة التفعيل |
|---|---|---|---|
| `parser_tests` (test_runner) | `parser/` | حارس `EXISTS test_lambda_simple.cpp` لا يتحقق — الملف في `interpreter/` | إصلاح الحارس + includes |
| `test_type_nodes`, `test_module_parsing` | `parser/` | `if(FALSE)` — includes قديمة تحتاج إعادة كتابة | إعادة كتابة للمسارات الجديدة |
| `oop_tests` | `oop/` | `if(FALSE)` — includes تشير لبنية ما قبل `shared/` | إعادة كتابة كاملة |
| محسّن SIR (3 أهداف) | `sir/optimizer/` | `if(FALSE)` — `sir_test_utils.cpp` يحتاج opcodes الحالية (SUB_I64/SUB_F64) | إصلاح sir_test_utils |

## التشغيل

```
ctest --test-dir build -C Debug -R Comprehensive   # الحزمة الشاملة
ctest --test-dir build -C Debug                     # كل المسجَّل
cmake --build build --config Debug --target comprehensive_tests
```

*جزء من نظام testing-system (الطبقة 1). راجع `_bmad-output/systems/testing-system/stories/TEST-004-consolidate-unit.md`.*
