# S-015e: MD Documentation Generator

> **Phase:** 7 | **Effort:** M | **Risk:** Low
> **Depends on:** S-015a, S-015b, S-015c, S-015d | **Blocks:** S-016
> **Arch ref:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) (مصدر كل الـtemplates) + [YAML_UNIFIED_SCHEMA_DESIGN.md](../../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) v1.0

> ⚠️ **التغطية:** الـgenerator يقرأ كل عناصر whitelist (`_index.yaml` + `docs.yaml` + `examples/*` + `exercises/*` + `i18n/*`) ويدمج overlay اللغة حسب `--lang`.

---

## User Story

**As** developer/reader،
**I want** أمر `sadinfo generate-docs` يولِّد `docs/*.md` و `وثائق/*.md` من `data/` (YAML SSoT) **لكل الكيانات الأربعة (builtins + keywords + errors + lessons)**،
**So that** الوثائق دائماً متزامنة مع SSoT، و MD لا تُحرَّر يدوياً.

> **النطاق صراحةً:** هذه الـstory تُولِّد MD لكل الـ4 kinds — وليس lessons فقط. AC-3 يغطّي lessons، AC-4 يغطّي keywords/builtins/errors. T3 يُنشئ template لكل kind.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `sadinfo generate-docs [--lang=ar|en] [--out=path]` يولِّد كل MD من YAML |
| AC-2 | output deterministic: نفس YAML → نفس bytes في MD |
| AC-3 | لكل lesson في `data/lessons/{id}/` ← `docs/{id}.md` |
| AC-4 | لكل category (keywords/builtins/errors) ← `docs/reference/{category}.md` (مرجع شامل) |
| AC-5 | code blocks تحفظ syntax highlighting (` ```sad `) |
| AC-6 | links بين الـlessons تُولَّد آلياً من `prerequisites` و `see_also` في YAML |
| AC-7 | TOC مُولَّد لكل ملف |
| AC-8 | template system بسيط (Mustache أو jinja-style) لتخصيص الـoutput |
| AC-9 | `--watch` mode: عند تغيير YAML → regenerate MD المتأثرة |
| AC-10 | CI gate: `sadinfo generate-docs --check` يفشل إذا MD موجودة في git لم تتطابق مع YAML |

## Tasks

- [ ] T1: `tools/sadinfo/src/commands/generate_docs.cpp`
- [ ] T2: template engine integration (مكتبة خفيفة)
- [ ] T3: 4 templates: `lesson.md.tmpl`, `keyword_ref.md.tmpl`, `builtin_ref.md.tmpl`, `error_ref.md.tmpl`
- [ ] T4: TOC generator
- [ ] T5: link resolver (id → relative path)
- [ ] T6: `--check` mode (لـCI)
- [ ] T7: `--watch` mode (يستخدم Watcher من S-011)
- [ ] T8: bilingual output (ar/en)

## File List

**جديد:**
- `tools/sadinfo/src/commands/generate_docs.cpp`
- `tools/sadinfo/templates/lesson.md.tmpl`
- `tools/sadinfo/templates/keyword_ref.md.tmpl`
- `tools/sadinfo/templates/builtin_ref.md.tmpl`
- `tools/sadinfo/templates/error_ref.md.tmpl`
- `tests/sadinfo/e2e/generate_docs_test.py`
- `tests/sadinfo/golden/generated_md/*.md` (snapshots)

## Definition of Done

- [ ] جميع MD المُولَّدة قابلة للقراءة + scannable
- [ ] CI check يمر (لا drift بين YAML و MD)
- [ ] watch mode latency < 500ms لتغيير واحد
- [ ] golden snapshots مُحفوظة + تتجدَّد بـ`--update-golden`

## Notes للمطوِّر

- template engine: استخدم [inja](https://github.com/pantor/inja) (header-only، C++17، Jinja-like) أو [mstch](https://github.com/no1msd/mstch)
- output يجب أن يكون **رابلaty للقراءة** — ليس just YAML dump
- إذا lesson لها multiple examples، MD المُولَّد يعرضها بترتيب YAML
- بعد هذه الـstory: docs/*.md و وثائق/*.md تدخل `.gitignore` (في S-016)
