---
status: done
slug: story-5.2-ai-lessons-pipeline
epic: "ADR-006b Epic 5 — enforce_docs_coverage + pipeline AI"
story_id: "5.2"
created: "2026-06-10"
author: "Amelia (bmad-agent-dev)"
priority: low
depends_on:
  - "5.1 — enforce_docs_coverage ✅"
  - "4.3 — 50 دالة مُوثَّقة ✅"
estimated_size: "2–3 أيام"
utm: "UTM-6.12"
---

# Story 5.2 — Pipeline AI لكتابة الدروس

## البيان

بوصفي **كاتب توثيق AI**،  
أريد **workflow واضح** لكتابة دروس YAML بأسلوب بشري طبيعي عبر وكيل AI،  
لكي **تكون دروس لغة ص عالية الجودة وقابلة للصيانة**.

---

## نطاق العمل

هذه القصة تختلف عن القصص الأخرى — معظمها **إنشاء templates ومواصفات workflow** لا كود.

---

## معايير القبول

### AC-5.2-01: AI Prompt Template

**Given** مواصفة YAML لدرس (`data/docs/lessons/goroutine_basics.yaml`)  
**When** يُشغَّل وكيل AI مع prompt من `scripts/codegen/ai_doc_prompt.md`  
**Then** يُنتج Markdown draft في `_drafts/goroutine_basics.md`  
**And** يحتوي:
  - مقدمة (50-100 كلمة)
  - 3 أقسام رئيسية
  - مثال `runnable: true` على الأقل
  - قسم "الأخطاء الشائعة"

### AC-5.2-02: هيكل مجلد الدروس

**Given** YAML lesson file جديد في `data/docs/lessons/`  
**When** يُشغَّل `gen_docs.py --channel vitepress`  
**Then** يُنشئ صفحة في `docs/generated/tutorials/` مع header `AUTO-GENERATED`

### AC-5.2-03: مراجعة بشرية قبل النشر

**Given** draft جاهز في `_drafts/`  
**When** يُوافق عليه محرر بشري (commit بـ "approved: saleh")  
**Then** يُنقل لـ `docs/generated/tutorials/`  
**And** CI يتحقق من أن الأمثلة تعمل (Story 2.3 CI runner)

---

## المهام

### T1: إنشاء ai_doc_prompt.md

**الملف:** `scripts/codegen/ai_doc_prompt.md`

Template موحَّد للـ prompt يحتوي:
```markdown
# تعليمات وكيل AI لكتابة دروس لغة ص

## السياق
- اللغة: عربية فصحى واضحة
- الجمهور: مطورون عرب مبتدئون في لغة ص
- الأسلوب: عملي، مباشر، مع أمثلة تعمل فعلياً

## المدخلات
- YAML lesson file: [path]
- قائمة الدوال ذات الصلة: [من migration_matrix.yaml]

## المخرجات المطلوبة
1. مقدمة (50-100 كلمة)
2. ثلاثة أقسام: المفهوم / الاستخدام الأساسي / الاستخدام المتقدم
3. مثال كامل runnable
4. قسم "الأخطاء الشائعة" (3 أخطاء شائعة + الحل)

## قواعد إلزامية
- لا استخدام لكلمات إنجليزية إلا للكلمات التقنية المحددة
- كل كود في ``` sad ``` block
- لا افتراض بمعرفة مسبقة بلغات أخرى
```

### T2: تحديث lesson_schema في DocIR

**الملف:** `scripts/codegen/doc_ir/doc_ir_builder.py`

إضافة دعم `doc_type: tutorial` مع حقول إضافية:
```yaml
doc_type: tutorial
prompt_hint: "ركز على الفهم قبل الحفظ"
level: beginner | intermediate | advanced
estimated_time: "15 دقيقة"
prereqs: ["المتغيرات", "الدوال"]
```

### T3: مثال أول درس — goroutine_basics

إكمال `data/docs/lessons/goroutine_basics.yaml` بحقول tutorial كاملة.

تشغيل pipeline:
```powershell
python scripts/codegen/gen_docs.py --channel vitepress --lesson goroutine_basics
```

---

## قائمة الملفات

| الملف | الإجراء |
|-------|--------|
| `scripts/codegen/ai_doc_prompt.md` | إنشاء جديد |
| `data/docs/lessons/goroutine_basics.yaml` | تعديل — إكمال الحقول |
| `scripts/codegen/doc_ir/doc_ir_builder.py` | تعديل — دعم tutorial حقول |

---

## ملاحظات

- `_drafts/` مجلد **غير مُتتبَّع** في git (مُضاف لـ .gitignore)
- المراجعة البشرية إلزامية قبل أي commit في `docs/generated/tutorials/`
- هذه القصة تعتمد على عمل Story 5.1 و 4.3

## سجل المطور

| التاريخ | المهمة | الحالة | ملاحظات |
|---------|--------|--------|---------|
| — | T1 | ⬜ | — |
| — | T2 | ⬜ | — |
| — | T3 | ⬜ | — |
