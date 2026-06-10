---
storyId: "3.1"
epicId: "3"
feature: "ADR-006b"
status: done
created: 2026-07-22
author: "Amelia (dev agent)"
skill_used: bmad-create-story
---

# Story 3.1 + 3.2: render_lsp.py — LSP Hover + Completion Items

## الهدف

إنشاء `render_lsp.py` يُولِّد ملف `docs/generated/lsp/builtins.json` يحتوي:
- `hover_text` لكل دالة (Markdown نظيف يُعرض في VS Code hover)
- `completion_detail` لاقتراحات الإكمال التلقائي

## معايير القبول

### AC-1: بنية builtins.json

**Given** DocIR لـ namespace `Core` مع دوال متعددة  
**When** يُشغَّل `render_lsp.py`  
**Then** يُنشئ JSON بهيكل:
```json
{
  "version": "1.0",
  "entries": {
    "اطبع": {
      "id": "Core.PRINT",
      "hover_text": "**اطبع** — يطبع قيمة على الشاشة\n\nمنذ: 0.1.0",
      "completion_detail": "اطبع(قيمة)",
      "completion_doc": "يطبع قيمة على الشاشة",
      "stability": "stable"
    }
  }
}
```

### AC-2: deprecated يحمل تحذيراً في hover_text

**Given** entry بـ `deprecated_in: "1.2.0"`  
**When** تُولَّد الـ LSP JSON  
**Then** `hover_text` يحتوي `⚠️ متوقف الاستخدام منذ 1.2.0`

### AC-3: experimental يحمل badge

**Given** entry بـ `stability: experimental`  
**When** يُولَّد  
**Then** `completion_detail` يحتوي `🧪 تجريبي`

### AC-4: gen_docs.py يُشغِّل render_lsp

**Given** يُشغَّل `gen_docs.py --channel lsp`  
**When** يكتمل  
**Then** ينشئ `docs/generated/lsp/builtins.json`

### AC-5: write_if_changed

**Given** الملف الناتج لم يتغير  
**When** يُشغَّل render_lsp  
**Then** لا يكتب الملف (يتجنب إعادة بناء VitePress غير الضرورية)

## الاختبارات

| # | الاختبار | المعيار |
|---|---------|--------|
| T-L1 | render_lsp_page يُنتج dict مع مفتاح "entries" | ✅ |
| T-L2 | entry عادي → hover_text يحتوي canonical_name | ✅ |
| T-L3 | deprecated entry → hover_text يحتوي ⚠️ | ✅ |
| T-L4 | experimental entry → completion_detail يحتوي 🧪 | ✅ |
| T-L5 | write_if_changed لا يكتب عند عدم التغيير | ✅ |

## الملفات المُستهدَفة

- `scripts/codegen/renderers/render_lsp.py` — جديد
- `scripts/codegen/test_lsp_renderer.py` — اختبارات T-L1..T-L5
- `scripts/codegen/gen_docs.py` — إضافة LSP channel
- `docs/generated/lsp/` — المجلد الناتج (جديد)

## الحجم

2 يوم
