---
status: done
slug: adr-006b-1-4-diataxis
created: 2026-07-21
completed: null
scope: Story 1.4 — تكامل Diátaxis في DocIR (doc_type في builder + renderer + tests)
epic: ADR-006b (Document-IR Foundation)
priority: medium
depends_on:
  - adr-006b-1-1-doc-ir-builder   # ✅ DONE
  - adr-006b-1-2-validator         # ✅ DONE (test_doc_ir.py — 23 tests)
  - adr-006b-1-3-versioning        # ✅ DONE (4 schemas + gen_builtins defaults)
---

# Story 1.4 — تكامل Diátaxis في DocIR

## السياق (Context)

### الوضع الحالي بعد Stories 1.1 → 1.3

| الملف | الحالة | ملاحظة |
|-------|--------|---------|
| `doc_ir_builder.py` | ✅ جاهز | يُنتج DocEntry بـ `doc_type="reference"` دائماً (hardcoded) |
| `doc_ir_validator.py` | ✅ جاهز | V-03 يتحقق من `doc_type ∈ {reference,tutorial,how-to,explanation}` |
| `render_vitepress.py` | ⬜ ناقص | لا يستخدم `doc_type` — يُصيِّر كل entries بنفس الأسلوب |
| `data/docs/` | ⬜ غير موجود | سيُنشأ في هذه الستوري |
| `test_doc_ir.py` | ✅ جاهز | 23 اختبار (T-12 يتحقق من doc_type غير صالح) |

### مشكلة Story 1.4

`doc_ir_builder.py` الحالي يُهمل حقل `doc_type` في YAML ويُعيِّنه دائماً `"reference"`.
هذا يمنع إنشاء محتوى Diátaxis من نوع tutorial/how-to/explanation.

**الهدف:** جعل `doc_type` يُقرأ من YAML إذا موجود، وإضافة دعم
`data/docs/` كمصدر للمحتوى غير المرجعي، وتحديث renderer لاستخدام doc_type.

---

## الملفات المُستهدَفة (Files to Touch)

| الملف | نوع التغيير | السبب |
|-------|------------|-------|
| `scripts/codegen/doc_ir/doc_ir_builder.py` | تعديل | قراءة `doc_type` من YAML بدلاً من hardcode |
| `scripts/codegen/renderers/render_vitepress.py` | تعديل | استخدام `doc_type` لتنسيق sections |
| `data/docs/` | إنشاء | مجلد جديد لمحتوى tutorial/how-to |
| `data/docs/lessons/goroutine_basics.yaml` | إنشاء | مثال tutorial من AC |
| `scripts/codegen/test_doc_ir.py` | تعديل | إضافة T-21 → T-25 (Diátaxis tests) |

---

## معايير القبول (ACs)

### AC-1: doc_type الافتراضي = "reference"

**Given** DocIR entry من `data/language/builtins/*.yaml` (بدون حقل doc_type)
**When** يُشغَّل `doc_ir_builder.build_from_builtins_yaml()`
**Then** كل entry يحمل `doc_type = "reference"`
**And** لا يوجد regression في 15 entry من core.yaml

**ملاحظة التنفيذ:**
```python
# في doc_ir_builder.py — السطر الحالي:
doc_type="reference",  # ← الآن يبقى كافتراضي فقط

# يصبح:
doc_type=func.get("doc_type", "reference"),  # ← يقرأ من YAML أو يستخدم افتراضي
```

---

### AC-2: قراءة doc_type من YAML

**Given** ملف `data/docs/lessons/goroutine_basics.yaml` مع `doc_type: "tutorial"`
**When** يُشغَّل `doc_ir_builder.build_from_docs_yaml()` (دالة جديدة)
**Then** يُنتج DocEntry بـ `doc_type = "tutorial"` (وليس "reference")
**And** جميع حقول DocEntry الأخرى (id, canonical_name, description_ar) صحيحة

**تنسيق `data/docs/lessons/goroutine_basics.yaml` المطلوب:**
```yaml
# مثال doc_type=tutorial من AC-2
id: "goroutine_basics"
doc_type: "tutorial"
canonical_name: "أساسيات الخيوط الخفيفة"
namespace: ""
title_ar: "أساسيات الخيوط الخفيفة (goroutines) في لغة ص"
title_en: "Goroutine Basics in Sad Language"
description_ar: "درس تعليمي لاستخدام أطلق وانتظر_الكل"
description_en: "Tutorial on using go and wait_all"
since: "0.1.0"
stability: "stable"
examples: []
see_also: ["Core.WAIT_ALL", "Core.CHANNEL"]
```

---

### AC-3: فشل doc_type غير صالح

**Given** DocIR entry بـ `doc_type: "invalid_type"`
**When** يُشغَّل `doc_ir_validator.validate_entries()`
**Then** يُرجع `ok=False`
**And** رسالة الخطأ تذكر القيم الصالحة
**ملاحظة:** هذا الـ AC محقق بالفعل في T-12 (validator V-03) ✅

---

### AC-4: render_vitepress يستخدم doc_type

**Given** `DocNamespaceGroup` يحتوي entries بـ `doc_type = "deprecated"`
**When** يُشغَّل `render_vitepress.render_namespace_page()`
**Then** الصفحة الناتجة تحتوي قسم `## ⚠️ متوقفة` لهذه الدوال

**Given** entries بـ `doc_type = "reference"` (الأغلبية)
**When** تُصيَّر
**Then** تظهر في القسم الرئيسي بدون تصنيف إضافي (backward compatible)

---

### AC-5: لا regression في gen_all.py

**Given** كل التغييرات مُطبَّقة
**When** يُشغَّل `python gen_all.py --repo-root .`
**Then** `[gen_all] OK: 5/5 generator(s) passed.`
**And** `python -m pytest scripts/codegen/test_doc_ir.py -v` → كل الاختبارات تنجح

---

## قائمة التحقق (Implementation Checklist)

### CW-01: تعديل doc_ir_builder.py

- [ ] استبدل `doc_type="reference"` (hardcoded) بـ `doc_type=func.get("doc_type", "reference")`
- [ ] أضف دالة `build_from_docs_yaml(yaml_path: Path) -> List[DocEntry]` لقراءة `data/docs/`
  - تُنتج DocEntry من ملف YAML حر (ليس بنية builtins)
  - تُطبِّق defaults: `doc_type="reference"` إذا غائب
- [ ] تحقق: `build_from_builtins_yaml(core.yaml)` لا يزال يُنتج `doc_type="reference"` لكل entry

### CW-02: إنشاء data/docs/

- [ ] أنشئ مجلد `data/docs/`
- [ ] أنشئ مجلد `data/docs/lessons/`
- [ ] أنشئ `data/docs/lessons/goroutine_basics.yaml` (مثال من AC-2)
- [ ] أنشئ `data/docs/lessons/README.md` يشرح التنسيق المطلوب

### CW-03: تعديل render_vitepress.py

- [ ] أضف ثابت `_DOC_TYPE_HEADERS` لعناوين Diátaxis:
  ```python
  _DOC_TYPE_HEADERS: dict[str, str] = {
      "reference":   "## المرجع",
      "tutorial":    "## دروس تعليمية",
      "how-to":      "## كيفية",
      "explanation": "## شرح مفاهيم",
  }
  ```
- [ ] عدِّل `render_namespace_page()` لتجميع entries حسب `doc_type` قبل التصيير
- [ ] الترتيب المُفضَّل: `reference` أولاً، ثم `tutorial`، ثم `how-to`، ثم `explanation`
- [ ] backward compatible: إذا كل entries بـ `doc_type="reference"` → نفس السلوك القديم

### CW-04: إضافة اختبارات T-21 → T-25 في test_doc_ir.py

- [ ] **T-21:** `build_from_builtins_yaml(core.yaml)` → كل entries بـ `doc_type="reference"`
- [ ] **T-22:** entry مع `doc_type="tutorial"` في YAML → builder يُنتج `doc_type="tutorial"`
- [ ] **T-23:** `build_from_docs_yaml(goroutine_basics.yaml)` → `doc_type="tutorial"` ✅
- [ ] **T-24:** validator V-03 يقبل `doc_type="how-to"` → ok=True
- [ ] **T-25:** render_vitepress مع entries مختلطة → الإخراج يحتوي headers الصحيحة

### CW-05: التحقق النهائي

- [ ] `gen_all.py --repo-root .` → 5/5 ✅
- [ ] `pytest scripts/codegen/test_doc_ir.py -v` → 28+ tests PASSED ✅
- [ ] `pytest scripts/codegen/ -v` → لا regression ✅

---

## مخطط التدفق (Data Flow)

```
YAML sources
    │
    ├── data/language/builtins/*.yaml  (builtins — doc_type defaults to "reference")
    │         │
    │         └── build_from_builtins_yaml()  ──→  [DocEntry(doc_type="reference")]
    │
    └── data/docs/lessons/*.yaml  (tutorials/how-to)
              │
              └── build_from_docs_yaml()  ──→  [DocEntry(doc_type="tutorial")]
                                                       │
                                               validate_entries()  ← V-03 يتحقق
                                                       │
                                               render_namespace_page()
                                                       │
                                               website/docs/docs/stdlib/*.md
```

---

## اختبارات Story 1.4 (تفصيل pytest)

### T-21: builtins دائماً reference

```python
def test_T21_builtins_default_doc_type_is_reference():
    """T-21: جميع entries من core.yaml تحمل doc_type=reference."""
    core_yaml = repo_root / "data/language/builtins/core.yaml"
    if not core_yaml.exists():
        pytest.skip("core.yaml not found")
    entries = build_from_builtins_yaml(core_yaml)
    assert all(e.doc_type == "reference" for e in entries), \
        "بعض entries من builtins لا تحمل doc_type=reference"
```

### T-22: YAML مع doc_type صريح

```python
def test_T22_explicit_doc_type_from_yaml(tmp_path):
    """T-22: حقل doc_type في YAML يُقرأ (لا يُتجاهل)."""
    yaml_file = tmp_path / "test.yaml"
    yaml_file.write_text(
        "version: 1\nnamespace_group: Test\nfunctions:\n"
        "  - cpp_id: FOO\n    canonical: 'فو'\n    namespace: Test\n"
        "    doc_type: 'tutorial'\n    description_ar: 'اختبار'\n    description_en: 'test'\n",
        encoding="utf-8"
    )
    entries = build_from_builtins_yaml(yaml_file)
    assert entries[0].doc_type == "tutorial"
```

### T-23: build_from_docs_yaml

```python
def test_T23_build_from_docs_yaml_tutorial(tmp_path):
    """T-23: build_from_docs_yaml يُنتج DocEntry بـ doc_type=tutorial."""
    yaml_file = tmp_path / "goroutine_basics.yaml"
    yaml_file.write_text(
        "id: goroutine_basics\ndoc_type: tutorial\n"
        "canonical_name: 'أساسيات الخيوط'\ndescription_ar: 'درس'\n"
        "description_en: 'Goroutine tutorial'\nsince: '0.1.0'\nstability: stable\n",
        encoding="utf-8"
    )
    from doc_ir.doc_ir_builder import build_from_docs_yaml
    entries = build_from_docs_yaml(yaml_file)
    assert len(entries) == 1
    assert entries[0].doc_type == "tutorial"
    assert entries[0].id == "goroutine_basics"
```

### T-24: validator يقبل how-to

```python
def test_T24_validator_accepts_how_to():
    """T-24: doc_type='how-to' مقبول من validator."""
    e = _entry(doc_type="how-to")
    result = validate_entries([e])
    assert result.ok is True
```

### T-25: renderer يُنتج headers صحيحة

```python
def test_T25_renderer_uses_doc_type_headers(tmp_path):
    """T-25: render_vitepress يُولِّد section headers حسب doc_type."""
    import sys; sys.path.insert(0, str(Path(__file__).parent))
    from renderers.render_vitepress import render_namespace_page
    from doc_ir.doc_ir_builder import DocEntry, DocNamespaceGroup

    group = DocNamespaceGroup(
        namespace="Test",
        entries=[
            DocEntry(id="Test.A", doc_type="reference",   canonical_name="أ"),
            DocEntry(id="Test.B", doc_type="tutorial",    canonical_name="ب"),
        ]
    )
    md_content = render_namespace_page(group)
    # (AR) يجب أن يحتوي على قسم tutorial
    assert "tutorial" in md_content.lower() or "درس" in md_content
```

---

## ملاحظات تقنية

### منع التراجع (Regression Prevention)

1. `build_from_builtins_yaml()` موجودة — التغيير محدود جداً (سطر واحد)
2. `validate_entries()` لا تتغير — V-03 موجود بالفعل ✅
3. `render_vitepress.py` يحتاج تغيير مُحاط بـ backward compatibility check:
   - إذا كل entries = "reference" → نفس الخرج القديم
   - فقط عند وجود types أخرى → يُضيف sections

### مسار `data/docs/`

```
data/
  language/          ← YAML للدوال المدمجة (موجود)
  docs/              ← محتوى وثائقي (جديد في 1.4)
    lessons/         ← doc_type: "tutorial"
    guides/          ← doc_type: "how-to"
    concepts/        ← doc_type: "explanation"
```

### لا يُنفَّذ في هذه الستوري (Out of Scope)

- تكامل `data/docs/` مع `gen_docs.py` (يُؤجَّل لـ Epic 4)
- Navigation sidebar في VitePress بـ Diátaxis sections (يُؤجَّل لـ Story 2.2 تحديث)
- توليد صفحات docs منفصلة لكل doc_type (Epic 2-3)

---

## التأثير على Epics اللاحقة

| Epic | الاستفادة من Story 1.4 |
|------|----------------------|
| Epic 2 Story 2.2 | render_vitepress يستخدم doc_type sections بالفعل |
| Epic 3 Story 3.1 | LSP renderer يستخدم doc_type للـ hover context |
| Epic 4 | مصفوفة الترحيل تشمل doc_type migration status |
| Epic 5 | enforce_docs_coverage يتحقق من doc_type consistency |
