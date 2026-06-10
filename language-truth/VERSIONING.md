# VERSIONING.md — سياسة إصدار بيانات language-truth/

> **المرجع:** [ARCHITECTURE.md §7](../_bmad-output/systems/living-documentation/ARCHITECTURE.md)

---

## نظرة عامة

تتبع بيانات `language-truth/` نظام **Semantic Versioning (SemVer 2.0)** المُطبَّق على البيانات.
الإصدار الحالي: **`5.0.0`** (موجود في `_meta/_version.yaml`).

---

## قواعد الإصدار

### `MAJOR` — تغيير كاسر (Breaking Change)
يزداد عند:
- حذف حقل موجود من Schema
- تغيير نوع حقل (مثال: `string` → `array`)
- تغيير `id` لعنصر موجود
- حذف كلمة مفتاحية أو دالة مدمجة موجودة

**التأثير:** يتطلب تحديث `gen_*.py` + `libsadlangtruth` + المستهلكين.

### `MINOR` — إضافة متوافقة (Backward Compatible Addition)
يزداد عند:
- إضافة حقل اختياري جديد للـ Schema
- إضافة كلمة مفتاحية أو دالة مدمجة جديدة
- إضافة ملف YAML جديد بنطاق جديد
- توسعة enum بقيمة جديدة

**التأثير:** المستهلكون القدامى يستمرون في العمل.

### `PATCH` — تصحيح (No Schema Change)
يزداد عند:
- تصحيح وصف أو ترجمة
- تصحيح خطأ إملائي في `description_ar` أو `description_en`
- إضافة/تصحيح مثال (example)
- تحديث رابط أو مرجع

**التأثير:** صفر — البنية لم تتغير.

---

## ملف الإصدار

الإصدار الحالي مسجَّل في `_meta/_version.yaml`:

```yaml
version: "5.0.0"
date: "2026-06-05"
schema_version: "draft-2020-12"
breaking_changes: []
migration_notes: "ترقية من data/language/ → language-truth/ (V5 Codegen Integration)"
```

---

## سياسة التوافق

- **V5.x.x** — متوافق مع Codegen Python V5 + `libsadlangtruth` V5.
- **V4.x.x** (السابق: `data/language/`) — يبقى يعمل طوال مرحلة M0–M2.
- عند الوصول لـ M2 (إغلاق S-V5-M2-004)، يُحذف `data/language/` ويصبح `language-truth/` SoT الوحيد.

---

## English Summary

Data in `language-truth/` follows Semantic Versioning.
- **MAJOR**: breaking schema or ID changes.
- **MINOR**: backward-compatible additions.
- **PATCH**: description/translation fixes only.

Current version: `5.0.0` — tracked in `_meta/_version.yaml`.
