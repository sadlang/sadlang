---
id: S-V5-M0-002
title: "جسر keywords.yaml عبر نسخ + إعادة توجيه"
type: implementation-story
milestone: M0-Foundation
status: ready
estimate: M
depends_on: [S-V5-M0-001]
governed_by:
  - ../../../IMPLEMENTATION_PLAN.md
  - ../../../decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md
date: 2026-06-05
---

# S-V5-M0-002 — جسر `keywords.yaml`

## القصة
بصفتي **مطوّر اللغة**، أريد **نقل `keywords.yaml` إلى `language-truth/` دون كسر codegen** حتى **يقرأ البناء من المصدر الجديد مع إثبات تطابق المخرَج**.

## السياق
هذا قلب الجسر الانتقالي (المسار الحرج). `data/language/keywords.yaml` يُغذّي الـ Lexer عبر `gen_keywords.py`. النقل يجب أن يُثبت بـ `--diff` أن `keywords_generated.cpp` لم يتغير (BF-12).

## معايير القبول
- **Given** `data/language/keywords.yaml` هو SoT الحالي (version 4.1)
- **When** ننسخه إلى `language-truth/keywords.yaml` + نُحدِّث مسار codegen
- **Then** البناء يستمر أخضر، `gen_keywords.py` يقرأ من المصدر الجديد
- **And** `keywords_generated.cpp` مُطابق تماماً للسابق (إثبات `--diff`)

## المهام
- [x] نسخ `data/language/keywords.yaml` → `language-truth/keywords.yaml`
- [x] نسخ `data/language/keywords.schema.json` → `language-truth/_schemas/keywords.schema.json`
- [x] تحديث `cmake/codegen.cmake`: `SAD_KW_YAML` يشير إلى `language-truth/keywords.yaml`
- [x] بناء + التقاط `keywords_generated.cpp` قبل/بعد ومقارنتهما (`git diff` = صفر — مُثبَت بـ diff Python)
- [x] إبقاء `data/language/keywords.yaml` كأمان حتى M2-004 (محفوظ كـ `SAD_KW_YAML_LEGACY`)

## المصدر في الكود
- `scripts/codegen/gen_keywords.py`، `scripts/codegen/gen_all.py` (سجل المسارات)
- `cmake/codegen.cmake` (متغير `SAD_KW_YAML`)
- `shared/lexer/generated/keywords_generated.{h,cpp}`

## تعريف "تم"
`cmake --build build --target sad` أخضر + الكلمات الـ91 مُولَّدة بلا تغيير (دليل `git diff`).
