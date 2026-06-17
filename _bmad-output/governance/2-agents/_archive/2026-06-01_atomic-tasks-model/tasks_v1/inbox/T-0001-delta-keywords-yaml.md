---
task_id: T-0001
title: "إنشاء data/_schemas/keywords.yaml من 65 كلمة (40 محجوزة + 25 سياقية)"
assigned_to: agent_delta
created_by: PM (John)
created_at: 2026-06-01T08:50:00Z
deadline: 2026-06-08
priority: P0
story_id: S-015b
milestone: M1
depends_on: []
guarded_files: []
status: queued
estimated_hours: 3
---

# 📋 T-0001 — إنشاء `data/_schemas/keywords.yaml`

## السياق

هذه أساس M1 (Foundation). بدون YAML SoT للكلمات، لا يُمكن أن نَبدأ Loader PoC (T-0007) ولا Validator (M2). الستوري الكامل: [S-015b](../../../../systems/living-documentation/3-implementation/stories/S-015b-migrate-keywords.md).

## المُدخلات (اقرأها أولاً)

1. **مَصدر الحقيقة الحالي:** [shared/lexer/src/lexer_keywords.cpp](../../../../../shared/lexer/src/lexer_keywords.cpp) — قائمة `KeywordTable::initialize()`
2. **شَكل الـSchema المُتفق عليه:** [2-architecture/planning/YAML_UNIFIED_SCHEMA_DESIGN.md](../../../../systems/living-documentation/2-architecture/planning/) (إن وجد)
3. **القواعد:** [copilot-instructions.md §نظام الكلمات المفتاحية](../../../../../.github/copilot-instructions.md)

## المخرجات المطلوبة (DoD)

- [ ] ملف `data/_schemas/keywords.yaml` يَحوي **65 كلمة بالضبط**:
  - 40 كلمة محجوزة (مع `category: reserved`, `since_version: 1.0`)
  - 25 كلمة سياقية (مع `category: contextual`, `context: "after_dalla|expression_start|..."`)
- [ ] كل كلمة تَحوي: `arabic`, `english`, `token_type`, `category`, `since_version`, `description_ar`, `description_en`, `example_code`
- [ ] الـ3 العوامل المنطقية (`و`, `أو`, `ليس`) في قسم منفصل `logical_operators:`
- [ ] أسماء الأنواع المدمجة (`رقم`, `عشري`, `نص`, إلخ) في قسم `builtin_types:` مع `is_identifier: true`
- [ ] `yq eval '.reserved | length' data/_schemas/keywords.yaml` → `40`
- [ ] `yq eval '.contextual | length' data/_schemas/keywords.yaml` → `25`

## القواعد الحَوكمية

- **CW-04 (التَناسق):** اتبع نَمط الـschema المُتفق عليه — لا تَخترع حقولاً
- **BF-25 (Pre-Code Analysis):** اقرأ `lexer_keywords.cpp` كاملاً قبل الكتابة
- **GR-01:** أي عدد تَكتبه (40, 25) يَجب أن يَكون مدعوماً بـgrep على `lexer_keywords.cpp`
- لا تَلمس `shared/lexer/src/lexer_keywords.cpp` (ملف محروس Saleh)

## كيفية الإبلاغ

1. **عند البدء:**
   ```powershell
   Move-Item .../tasks/inbox/T-0001-*.md .../tasks/in-progress/
   "agent_delta_1`n2026-06-01T09:00:00Z`n$PID" | Set-Content .../tasks/locks/T-0001.lock
   ```

2. **عند الانتهاء:** اكتب `outbox/T-0001-delta-keywords-yaml-RESULT.md` بصيغة §4 في [AGENT_ORCHESTRATION.md](../../AGENT_ORCHESTRATION.md)

3. **حدِّث `tasks/index.yaml`:** أضف الإدخال `T-0001:` مع `status: done` + `completed_at`

4. **احذف القفل:** `Remove-Item .../locks/T-0001.lock`

## ملاحظات إضافية

- اللغة في الـYAML: عربية للمحتوى، إنجليزية للمفاتيح
- العَرضي: 0 ملفات `data/_schemas/*.yaml` موجودة حالياً — أنت أوَّل من يَكتب هنا
