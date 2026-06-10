# S-014b: CI Pipeline + Documentation Generation

> **Phase:** 6 | **Effort:** M | **Risk:** Low
> **Depends on:** S-009, S-014 | **Blocks:** S-015a
> **Arch ref:** §10

---

## User Story

**As** فريق المشروع،
**I want** `.github/workflows/sadinfo.yml` كامل + توليد docs تلقائي + tracking للأداء عبر الـPRs،
**So that** كل PR يفحص تلقائياً وأي regression يُكتشف.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | workflow يجري على push + PR لـmain + الفروع المسبقة `sadinfo-*` |
| AC-2 | jobs: unit → integration → performance → e2e → snapshots (sequential where needed) |
| AC-3 | matrix: Windows + Linux (macOS بعد S-011b) |
| AC-4 | Performance regression check: فشل إذا أي scenario > 1.10 × baseline (10% فقط، لا 20%) |
| AC-5 | عند regression، PR comment بـbefore/after table |
| AC-6 | baseline.json يُحدَّث فقط على merge to main مع commit msg `[perf-baseline]` |
| AC-7 | code coverage report (OpenCppCoverage على Win، gcov على Linux) → Codecov upload |
| AC-8 | `sadinfo docs --generate` يبني MD reference من entities → `docs/sadinfo-reference/` |
| AC-9 | docs generation يجري كل CI run، PR comment إذا diff |
| AC-10 | security scan: CodeQL + dependabot enabled |

## Tasks

- [ ] T1: `.github/workflows/sadinfo.yml` (5 jobs)
- [ ] T2: `scripts/compare_perf.py` (baseline vs current)
- [ ] T3: PR comment action للأداء
- [ ] T4: coverage upload setup
- [ ] T5: `tools/sadinfo/src/commands/docs.cpp` (docs generator)
- [ ] T6: MD templates للـreference
- [ ] T7: CodeQL workflow
- [ ] T8: README.md في `.github/workflows/`

## File List

**جديد:**
- `.github/workflows/sadinfo.yml`
- `.github/workflows/sadinfo-codeql.yml`
- `.github/workflows/README.md`
- `scripts/compare_perf.py`
- `tools/sadinfo/src/commands/docs.cpp`
- `tools/sadinfo/templates/entity_doc.md.template`
- `tests/sadinfo/e2e/docs_generation_test.py`

**معدَّل:**
- `.gitignore` (coverage outputs)

## Definition of Done

- [ ] PR sample يُظهر الـperformance comment
- [ ] coverage report مرئي
- [ ] docs generated مطابق للـentities
- [ ] CodeQL clean

## Notes للمطوِّر

- coverage على Win: OpenCppCoverage مع `--export_type=cobertura` (يدعمه Codecov)
- perf regression threshold = **10%** (الصناعة)، 20% فضفاض جداً
- docs generator: قراءة entities → tmpl filling → write to `docs/sadinfo-reference/{category}/{id}.md`
