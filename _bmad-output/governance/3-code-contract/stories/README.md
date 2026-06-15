# stories/ — القصص (3-code-contract)

ستوريات Epic 1 (clang-tidy + إصلاح المخالفات):

| الملف | Epic | Story | الحالة (الواقع 2026-05-30) |
|---|---|---|---|
| [1-1-code-standards-spec.md](1-1-code-standards-spec.md) | 1 | 1.1 | ✅ `.clang-tidy` موجود (مُدَّعى) |
| [1-2-code-standards-enforcer.md](1-2-code-standards-enforcer.md) | 1 | 1.2 | 🔴 مخالف لتسمية الملف — هذه فعلياً ستوري Epic 3 |
| [1-3-cmake-integration.md](1-3-cmake-integration.md) | 1 | 1.3 | 🔴 CMake target check_standards غير موجود |
| [1-4-fix-violations.md](1-4-fix-violations.md) | 1 | 1.4 | ✅ إصلاح expression_evaluator_ui.cpp (مُدَّعى) |

> ⚠️ تسمية الملفات `1-X-...` مُربكة لأنها تشير لـEpic 1 لكن بعضها يتعلق بـEpic 3. ينصح بإعادة تسمية بصيغة `STORY-<EPIC>-<SEQ>-<name>.md` لاحقاً.

> 🔴 **تنبيه:** ستوريات Epic 2 (SAD_INVARIANT) وEpic 3 وEpic 4 **غير موجودة** كملفات هنا، رغم أن [../status/implementation_status.md](../status/implementation_status.md) يدَّعي إنجاز Epic 2 بالكامل. انظر `../status/` للتحقق.
