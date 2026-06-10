# S-016: Legacy Removal

> **Phase:** 8 | **Effort:** M | **Risk:** Med
> **Depends on:** S-015a, S-015b, S-015c, S-015d, S-015e | **Blocks:** None
> **Arch ref:** §14 (Phase 8)

---

## User Story

**As** maintainer،
**I want** إزالة الكود القديم (hardcoded builtins، yaml-cpp leak في APIs، commands القديمة)،
**So that** الـcodebase نظيفة، لا duplication، v2 هي الـsole source of truth.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `tools/sadinfo/src/builtin_data.cpp` محذوف |
| AC-2 | `tools/sadinfo/include/builtin_data.h` محذوف |
| AC-3 | `data/language/keywords.yaml`، `data/language/error_messages.yaml` → `archived/legacy_data/` |
| AC-4 | commands القديمة `--dump-keywords`، `--dump-builtins` تبقى كـbackward compat لكن تستخدم v2 internally |
| AC-5 | كل LSP/Formatter integration يستخدم Reader API فقط |
| AC-6 | لا include لـyaml-cpp في tools/lsp أو tools/formatter |
| AC-7 | كل comprehensive_tests تمر بدون regression |
| AC-8 | website يبني ويستهلك v2 exports |
| AC-9 | CHANGELOG.md موثَّق بكل التغييرات breaking |
| AC-10 | Golden tests للـcommands القديمة (`--dump-keywords`، `--dump-builtins`): الـoutput v1 أصلي محفوظ → v2 يجب أن يُنتج نفس الـbytes |
| AC-11 | `/memories/repo/keywords_yaml_sot_v41.md` مُرقَّى إلى v42 (يوثَّق sadinfo v2) والـv41 مُأرشَف |
| AC-12 | Tag `v1-final` على git قبل الحذف (rollback safety) |
| AC-13 | Release note draft في `_bmad-output/releases/sadinfo-v2.md` |
| AC-14 | **MD source files تُحذف من git**: جميع `docs/*.md` و `وثائق/*.md` المغطّاة بـS-015d (lessons) تُنقل إلى untracked |
| AC-15 | `.gitignore` يضيف: `docs/`, `وثائق/` (ستُولّد بـS-015e) |
| AC-16 | أول generate-docs run بعد الحذف تُنتج MD مطابقة بنسبة 100% لما كان في git سابقاً (تحقّق من عدم فقدان محتوى) |
| AC-17 | CI step جديد: عند أي PR → `sadinfo generate-docs --check` يسبق أي build (منع drift) |

## Tasks

- [ ] T1: حذف `builtin_data.cpp/.h`
- [ ] T2: نقل `data/language/*.yaml` إلى `archived/`
- [ ] T3: تحديث `--dump-keywords/--dump-builtins` لاستخدام Reader
- [ ] T4: فحص grep على yaml-cpp في LSP/Formatter
- [ ] T5: تشغيل comprehensive_tests كاملة
- [ ] T6: تحديث docs لإزالة المراجع للـv1
- [ ] T7: تحديث CHANGELOG
- [ ] T8: تحديث `/memories/repo/keywords_yaml_sot_v41.md` إلى v42 (sadinfo v2)

## File List

**محذوف:**
- `tools/sadinfo/src/builtin_data.cpp`
- `tools/sadinfo/include/builtin_data.h`
- (محتوى) `data/language/keywords.yaml`
- (محتوى) `data/language/error_messages.yaml`

**معدَّل:**
- `tools/sadinfo/src/cli.cpp`
- `tools/sadinfo/src/dump_keywords.cpp`
- `tools/sadinfo/src/dump_builtins.cpp`
- `CHANGELOG.md`
- `/memories/repo/keywords_yaml_sot_v41.md` → v42

**جديد:**
- `archived/legacy_data/keywords.yaml` (نسخة احتياطية)
- `archived/legacy_data/error_messages.yaml`

## Definition of Done

- [ ] لا hardcoded builtin/keyword في C++
- [ ] grep على `yaml-cpp` في tools/lsp و tools/formatter → 0 نتائج
- [ ] comprehensive_tests كلها خضراء
- [ ] CHANGELOG كامل
- [ ] release note draft جاهز

## Notes للمطوِّر

- آخر story في الـepic — تأنَّى وافحص بدقَّة
- احتفظ بـrollback plan: tag `v1-final` على git قبل البدء
- breaking changes موثَّقة:
  - `tools/sadinfo/include/sadinfo.h` API تغيَّر
  - exit codes ثابتة الآن (لا تتغيَّر بعد)
  - cache format جديد (v1 caches لن تعمل)

### بديل المعرفة لوكلاء الذكاء الاصطناعي (Copilot / Cursor / Claude)

بعد حذف MD من git، وكلاء الذكاء الاصطناعي يفقدون مصدر استرجاع نصي مهم (كانوا يقرؤون `docs/*.md` و `وثائق/*.md` للسياق). الحل الإلزامي:

1. **منتج release artifact مُجمَّع:** كل release يُرفق بـ:
   - `sadinfo-knowledge-<version>.tar.gz` يحتوي على:
     - `.sadinfo_cache/aggregated/full_export.json` (كل الـYAML مُسطَّحاً + مُترجَماً للـ2 لغة)
     - `docs/` و `وثائق/` المُولَّدة (snapshot زمني للقراءة)
     - `schemas/*.json` (للتحقُّق المحلي)
2. **AC إضافي مقترح (AC-18 مستقبلاً):** CI release pipeline يبني الـartifact ويرفعه إلى GitHub Releases.
3. **توثيق في README الرئيسي للمشروع:** قسم "للذكاء الاصطناعي" يشرح كيف يُحمَّل ويُستخدم الـartifact.
4. **بديل أخفّ:** نشر `full_export.json` فقط (ملف JSON واحد < 5MB متوقَّع) على CDN ثابت لكل release — مفيد لـMCP servers مستقبلاً.

> **تنبيه:** بدون هذا البديل، أي AI agent بعد S-016 سيقترح كود لغة ص خاطئ لأنه فقد مصدر القواعد. هذه ليست رفاهية.
