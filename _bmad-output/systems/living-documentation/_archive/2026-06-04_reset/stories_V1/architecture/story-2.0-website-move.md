---
storyId: "2.0"
epicId: "2"
feature: "ADR-006b"
status: done
created: 2026-07-22
author: "Amelia (dev agent)"
skill_used: bmad-create-story
---

# Story 2.0: نقل `website/` داخل المستودع + CI deploy إلى السيرفر الخاص

## الهدف

نقل موقع VitePress من `C:\s_lang\website\` (خارج المستودع) إلى `s-programming-language/website/` (داخل المستودع) وربطه بـ CI deploy تلقائي إلى السيرفر الخاص `185.47.174.39` (`sad-lang.org`).

## لماذا هذه القصة مهمة؟

pipeline ADR-006b يكتب ملفات `.md` مُولَّدة إلى `website/docs/` — لذا يجب أن يكون `website/` داخل المستودع حتى يعمل gen_docs.py بشكل صحيح.

## معايير القبول (ACs)

### AC-1: نقل الموقع

**Given** `C:\s_lang\website\` موجود خارج المستودع  
**When** تُنفَّذ Story 2.0  
**Then** المجلد منقول إلى `s-programming-language/website/`  
**And** VitePress build ينجح من الموقع الجديد

### AC-2: .gitignore صحيح

**Given** `website/` داخل المستودع  
**When** يُفحص `.gitignore`  
**Then** يحتوي على:
```
website/node_modules/
website/.vitepress/dist/
website/.vitepress/cache/
```

### AC-3: CI deploy إلى السيرفر الخاص

**Given** push إلى `main`  
**When** `.github/workflows/docs-deploy.yml` يعمل  
**Then** يُبنى `website/.vitepress/dist/` ثم يُرفَع عبر rsync/scp إلى `/opt/sad-lang-website/` على `185.47.174.39`  
**And** Nginx يقدّمه على `https://sad-lang.org`

### AC-4: mkdocs.yml ينتقل للـ archived

**Given** `mkdocs.yml` في جذر المشروع  
**When** تكتمل Story 2.0  
**Then** يُنقل إلى `archived/docs/mkdocs_legacy.yml`

### AC-5: gen_docs.py يكتب في المسار الجديد

**Given** `website/` داخل المستودع  
**When** يُشغَّل `gen_docs.py --repo-root . --channel vitepress`  
**Then** يكتب ملفات `.md` في `website/docs/docs/stdlib/`  
**And** `npm run build --prefix website` ينجح بعد ذلك

## الاختبارات التقنية

| # | الاختبار | المعيار |
|---|---------|--------|
| T-W1 | `Test-Path website/package.json` | يُرجع True |
| T-W2 | `npm run build --prefix website` | exit code 0 |
| T-W3 | `.gitignore` يحتوي `website/node_modules/` | grep ✅ |
| T-W4 | `docs-deploy.yml` موجود في `.github/workflows/` | Test-Path ✅ |
| T-W5 | `mkdocs.yml` في جذر المشروع **لا** يوجد (نُقل) | Test-Path يُرجع False |

## الملفات المُستهدَفة

- `website/` — المجلد الرئيسي (نُقل من خارج المستودع)
- `.gitignore` — إضافة استثناءات website/
- `.github/workflows/docs-deploy.yml` — CI deploy جديد
- `archived/docs/mkdocs_legacy.yml` — mkdocs.yml المنقول
- `scripts/codegen/gen_docs.py` — تحديث المسار إلى `website/docs/`

## الحجم

1–2 يوم

## الاعتماديات

- يُسبق Epic 3 (LSP/REPL/man renderers)
- يعتمد على Story 2.1 ✅ (gen_docs.py موجود)
- يعتمد على Story 2.2 ✅ (render_vitepress.py موجود)
