# S-015d: Migrate Lessons (MD → YAML SSoT)

> **Phase:** 7 | **Effort:** XL | **Risk:** High
> **Depends on:** S-015c, S-013 | **Blocks:** S-015e, S-016
> **Arch ref:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) §2–9 + [YAML_UNIFIED_SCHEMA_DESIGN.md](../../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) v1.0

> ⚠️ **البنية إلزامية (whitelist فقط):** كل lesson = مجلد `data/lessons/lesson_<id_last_segment>/` يحوي:
> - `_index.yaml` (§4.4) — schema_version, id, kind, title, level, order, prerequisites, tags, uses, owners
> - `docs.yaml` (§5) — summary, description (markdown داخل `|`), notes, see_also, related_lessons
> - `examples/{name}.yaml` (§6) — مثال لكل ملف (title, order, runnable, deterministic, code, expected_output|expected_pattern)
> - `exercises/{name}.yaml` (§7) — تمرين لكل ملف (title, difficulty, prompt, starter_code, solution, test_strategy)
> - `i18n/{lang}.yaml` (§8) — overlay فقط (en اختياري في v2)
> **ممنوع نهائياً:** `definition.yaml`, `content.yaml`, `examples.yaml`, `definition.ar.yaml`, `definition.en.yaml` (مهجورة).

---

## User Story

**As** maintainer،
**I want** نقل كل lessons من `docs/*.md` و `وثائق/*.md` إلى `data/lessons/lesson_<id>/` كـYAML مُهيكَل وفق الـwhitelist،
**So that** YAML تصبح SSoT الوحيد، و MD تُولَّد آلياً منها لاحقاً (S-015e).

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `scripts/extract_lessons_to_yaml.py` يحلِّل MD → `data/lessons/lesson_<id>/` |
| AC-2 | كل lesson يلتزم بـwhitelist: `_index.yaml + docs.yaml + examples/** + exercises/** + i18n/**` فقط |
| AC-3 | `_index.yaml`: schema_version=1, id=`lesson.<slug>`, kind=lesson, title, level (beginner/intermediate/advanced), order, prerequisites, tags, uses (reverse-index)، owners |
| AC-4 | `docs.yaml`: summary (سطر واحد إلزامي) + description (markdown block scalar `|`) + see_also (refs مُتحقَّق منها) + related_lessons |
| AC-5 | كل مثال = ملف منفصل `examples/<name>.yaml` بـ`runnable` + `deterministic` + (`expected_output` XOR `expected_pattern`) |
| AC-6 | i18n overlay (اختياري v2): `i18n/en.yaml` يكسر فقط `docs.*` + `examples.<name>.title` + `exercises.<name>.{title,prompt}` (لا يكسر code/expected) |
| AC-7 | كل example تجتاز Tier 3 (S-013) |
| AC-8 | الأمثلة الفاشلة تُسجَّل في `_bmad-output/migration/failed_examples.json` للمراجعة اليدوية |
| AC-9 | scope: `docs/01_*.md` → `docs/15_*.md` + `وثائق/*.md` الأساسية |
| AC-10 | reverse-check محدَّد قياسياً: regenerate MD من YAML → diff structural (heading levels + code blocks + list items) ≤ 5% — يستخدم `scripts/structural_md_diff.py` (مقياس: عدد العقد المختلفة ÷ مجموع العقد) |
| AC-11 | non-deterministic examples مُعلَّمة `deterministic: false` + `expected_pattern` صراحة |
| AC-12 | naming check: كل مجلد lesson يطابق `lesson_<id_last_segment>` (يفشل CI خلاف ذلك) |

## Tasks

- [ ] T1: `scripts/extract_lessons_to_yaml.py` (MD parser → بنية whitelist)
- [ ] T2: `scripts/structural_md_diff.py` (مقياس AC-10 الرسمي)
- [ ] T3: تشغيل الـextractor + تقرير الفاشل في `failed_examples.json`
- [ ] T4: مراجعة يدوية + إصلاح structural mismatches
- [ ] T5: تشغيل Tier 3 على كل `examples/**`
- [ ] T6: إنشاء `i18n/en.yaml` overlays للدروس الأولى (10 دروس فقط في v2)
- [ ] T7: reverse-check (regenerate MD → structural diff ≤ 5%)
- [ ] T8: التحقّق من `lesson.schema.json` (مُنشأ في S-000a) ضد كل lesson

## File List

**جديد (لكل lesson ~50):**
- `data/lessons/lesson_<id>/_index.yaml`
- `data/lessons/lesson_<id>/docs.yaml`
- `data/lessons/lesson_<id>/examples/<name>.yaml` (×N)
- `data/lessons/lesson_<id>/exercises/<name>.yaml` (×M اختياري)
- `data/lessons/lesson_<id>/i18n/en.yaml` (اختياري v2)

**جديد (أدوات):**
- `scripts/extract_lessons_to_yaml.py`
- `scripts/structural_md_diff.py`
- `_bmad-output/migration/failed_examples.json`
- `tests/sadinfo/integration/lessons_tier3_full.cpp`
- `tests/sadinfo/integration/lessons_whitelist_test.cpp` (يرفض أي ملف خارج whitelist)
- `tests/sadinfo/integration/lessons_naming_test.cpp` (AC-12)

## Definition of Done

- [ ] ≥ 90% examples تجتاز Tier 3
- [ ] الفاشلة موثَّقة بأسباب في `failed_examples.json`
- [ ] structural diff ≤ 5% لكل lesson
- [ ] whitelist test يفشل عند إضافة ملف خارجي (negative test)
- [ ] naming test يفشل عند `data/lessons/01_intro/` (بدون `lesson_` prefix)
- [ ] LSP autocomplete على `lesson.<id>` يعمل
- [ ] لا أثر لـ`definition.yaml`/`content.yaml`/`examples.yaml` في `data/lessons/**`

## Notes للمطوِّر

- **اقرأ أولاً:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) §4.4، §5، §6، §7، §8 — الأمثلة الجاهزة
- ابدأ بـ`docs/01_البدء.md` (الأبسط) كنموذج هندسي
- markdown داخل YAML strings: استخدم block scalar `|` (يحافظ على newlines)
- بعد نجاح S-015d، **MD الأصلية تُحذف من git في S-016** وتُضاف إلى `.gitignore`
- توليد MD من YAML سيكون مسؤولية **S-015e** المنفصلة (لكل الكيانات، ليس lessons فقط)
- `structural_md_diff.py` يقارن الأشجار (markdown-it AST): عدد heading levels + code fences + list items + table rows
