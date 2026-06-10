---
story_key: docs-0.0-v2
epic: 0 — Sprint 0: Bootstrap
story_number: 0.0
title: Verify Existing Infrastructure
status: ready-for-dev
priority: critical
estimate_hours: 2
assignee: TBD
created: 2026-05-09
created_by: Amelia (Senior Dev)
inputDocuments:
  - _bmad-output/planning-artifacts/prd-docs-system-v2.md
  - _bmad-output/planning-artifacts/architecture-docs-system-v2.md
  - _bmad-output/planning-artifacts/epics-docs-system-v2.md
  - _bmad-output/planning-artifacts/gap-analysis-docs-system-v2.md
related_stories:
  - 1.1 — sadinfo skeleton (يأتي بعد 0.0)
---

# Story 0.0 v2: Verify Existing Infrastructure

## 🎯 الهدف

التحقق من أن كل البنية التحتية الموجودة تعمل، توثيق إصداراتها، وتحديد أي شيء ناقص قبل بدء بناء `sadinfo.exe` في Story 1.1.

**ملاحظة جوهرية:** هذه القصة **لا تُنشئ شيئاً جديداً تقريباً**. كل شيء موجود — نتأكد فقط ونوثّق.

---

## 👥 User Story

**As a** maintainer للغة ص،
**I want** التأكد أن website + dual_execution tests + docs_emitter + sad/sadc كلها تعمل،
**So that** Story 1.1 (sadinfo) تبدأ بدون مشاكل بنية تحتية.

---

## ✅ Acceptance Criteria (6 فحوصات سريعة)

### AC-1: website يعمل محلياً

```bash
cd C:\s_lang\website
npm install
npm run dev
# يجب أن يفتح localhost:5173 ويعرض "لغة ص"
```

**Pass:** الموقع يعرض الصفحة الرئيسية بدون أخطاء console.

### AC-2: website يبني للإنتاج

```bash
cd C:\s_lang\website
npm run build
```

**Pass:** `.vitepress/dist/` ينشأ، حجم HTML > 0، لا أخطاء.

### AC-3: dual_execution tests تمر

```bash
cd C:\s_lang\s-programming-language
python tests/doc_gen_dual_execution/runner.py --skip-pdf
```

**Pass:** 33/33 ✓ (أو 44/44 مع pdf إذا متوفر Edge).

### AC-4: sad + sadc يُبنيان

```bash
cd C:\s_lang\s-programming-language
cmake -S . -B build
cmake --build build --target sad --config Release
cmake --build build --target sadc --config Release
```

**Pass:** كلا التنفيذيين موجودان في `build/bin/Release/`.

### AC-5: docs_emitter يعمل (smoke test)

```bash
.\build\bin\Release\sad.exe --docs examples\test_simple.ص
.\build\bin\Release\sad.exe --docs-format=json examples\test_simple.ص
.\build\bin\Release\sad.exe --docs-format=html examples\test_simple.ص
```

**Pass:** كل الصيغ الثلاث تنتج output صحيح بدون أخطاء.

### AC-6: ملفات التوثيق تُنشَأ

أنشئ ملفين:

**1. `_bmad-output/implementation-artifacts/infra-verified.md`:**

```markdown
# Infrastructure Verification Report
**Date:** 2026-05-09
**Verified by:** [الاسم]

## Versions
- VitePress: 1.6.3
- Vue: 3.4.0
- Node: [نسخة]
- npm: [نسخة]
- Python: [نسخة]
- CMake: [نسخة]
- LLVM: [نسخة]

## Build Times
- npm install (cold): [X seconds]
- npm run build: [X seconds]
- cmake build sad: [X seconds]
- cmake build sadc: [X seconds]

## Test Results
- doc_gen_dual_execution: 33/33 ✓ (skip-pdf) أو 44/44 ✓
- sad --docs smoke: ✓
- sadc --docs smoke: ✓

## Issues Found
[أي مشاكل اكتُشفت — إن لم توجد، اكتب "None"]

## Sign-off
- [ ] صلاح (Owner)
```

**2. `_bmad-output/implementation-artifacts/secrets-checklist.md`:**

```markdown
# Secrets Checklist — نظام التوثيق

| Secret | الموقع | الصلاحية | rotation |
|---|---|---|---|
| `CLAUDE_API_KEY` | GitHub Repo Secrets | API access | 2026-08-09 |
| `NPM_PUBLISH_TOKEN` | GitHub Repo Secrets | publish @sad-lang | 2026-08-09 |
| `PYPI_API_TOKEN` | GitHub Repo Secrets | publish sad-doctest | 2026-08-09 |

## Owner
- صلاح

## Status
- [ ] CLAUDE_API_KEY محفوظ
- [ ] NPM_PUBLISH_TOKEN محفوظ
- [ ] PYPI_API_TOKEN محفوظ
- [ ] @sad-lang npm org موجود
- [ ] sad-doctest PyPI name متوفر
```

**Pass:** كلا الملفين موجودان وصلاح وقّع على infra-verified.md.

---

## 📁 الملفات المُنشَأة (2 فقط)

| الملف | النوع | الحجم |
|---|---|---|
| `_bmad-output/implementation-artifacts/infra-verified.md` | جديد | ~1 KB |
| `_bmad-output/implementation-artifacts/secrets-checklist.md` | جديد | ~0.5 KB |

**لا تعديل على أي كود.** هذه قصة verification فقط.

---

## 🛠️ خطة التنفيذ (بالترتيب)

### Step 1: Verify website (15 دقيقة)

```bash
cd C:\s_lang\website
node --version    # سجّل في infra-verified.md
npm --version     # سجّل
npm install       # قِس الزمن
npm run build     # قِس الزمن
```

### Step 2: Verify dual_execution tests (10 دقائق)

```bash
cd C:\s_lang\s-programming-language
python --version  # سجّل
python tests/doc_gen_dual_execution/runner.py --skip-pdf
```

### Step 3: Verify sad + sadc (15 دقيقة)

```bash
cmake --version   # سجّل
cmake -S . -B build
cmake --build build --target sad --config Release    # قِس الزمن
cmake --build build --target sadc --config Release   # قِس الزمن
```

### Step 4: Smoke test docs_emitter (5 دقائق)

```bash
.\build\bin\Release\sad.exe --docs examples\test_simple.ص
.\build\bin\Release\sad.exe --docs-format=json examples\test_simple.ص > /tmp/docs.json
.\build\bin\Release\sad.exe --docs-format=html examples\test_simple.ص > /tmp/docs.html
# تأكد أن JSON صحيح: python -c "import json; json.load(open('/tmp/docs.json'))"
```

### Step 5: Write reports (30 دقيقة)

أنشئ الملفين وأكمل البيانات.

### Step 6: Sign-off (15 دقيقة)

أرسل `infra-verified.md` لصلاح للموافقة.

---

## 🧪 خطة الاختبار

**هذه قصة verification — الاختبار = ACs نفسها.**

```bash
# scripts/test_story_0_0_v2.sh
#!/bin/bash
set -e

echo "[1/6] Verify website builds..."
cd C:\s_lang\website && npm run build > /dev/null

echo "[2/6] Verify dual_execution tests..."
cd C:\s_lang\s-programming-language
python tests/doc_gen_dual_execution/runner.py --skip-pdf > /dev/null

echo "[3/6] Verify sad builds..."
cmake --build build --target sad --config Release > /dev/null

echo "[4/6] Verify sadc builds..."
cmake --build build --target sadc --config Release > /dev/null

echo "[5/6] Smoke test docs_emitter..."
./build/bin/Release/sad.exe --docs examples/test_simple.ص > /dev/null

echo "[6/6] Verify reports exist..."
test -f _bmad-output/implementation-artifacts/infra-verified.md
test -f _bmad-output/implementation-artifacts/secrets-checklist.md

echo "✅ Story 0.0 v2 verified!"
```

---

## ⚠️ المخاطر

| المخاطرة | احتمال × تأثير | تخفيف |
|---|---|---|
| Node version قديم | 2×3=6 🟡 | اطلب من صلاح ترقية إلى Node 20 LTS |
| LLVM غير مُثبَّت | 2×4=8 🟡 | sadc يُبنى Release فقط؛ Debug يفشل (موثَّق في memory) |
| website dependencies broken | 1×4=4 🟢 | `rm -rf node_modules && npm install` |

---

## 📋 Definition of Done

- [ ] AC-1 يمر (website dev)
- [ ] AC-2 يمر (website build)
- [ ] AC-3 يمر (33/33 ✓)
- [ ] AC-4 يمر (sad + sadc Release)
- [ ] AC-5 يمر (3 صيغ docs)
- [ ] AC-6 يمر (ملفان موجودان)
- [ ] `test_story_0_0_v2.sh` ينجح
- [ ] صلاح وقّع على infra-verified.md
- [ ] commit واحد بـ message: `docs: verify infrastructure for docs system v2`

---

## 🔗 الاعتماديات

**Upstream:** لا اعتماديات.

**Downstream:**
- Story 1.1 (sadinfo skeleton) — يحتاج CMake يعمل (AC-4)
- Story 1.7 (check_keywords.py) — يحتاج Python يعمل (AC-3)
- Story 3.1 (dataLoaders) — يحتاج website يبني (AC-2)
- Story 4.1 (npm publish) — يحتاج NPM_PUBLISH_TOKEN (AC-6)

---

## 📝 ملاحظات Amelia

> **الفرق عن v1:** Story 0.0 v1 كان 1 يوم لإنشاء 13 ملف جديد. v2 هو 2 ساعات للتأكد من 0 ملف جديد (تقريباً).
>
> **السبب:** كل البنية موجودة. v1 كان مبنياً على افتراض خاطئ.
>
> **التوفير:** 6 ساعات (75% أسرع) بدون فقدان جودة.

**جاهزة للتنفيذ. الزمن المتوقع: 2 ساعات.**

---

## 🧪 Test Architect Note (Murat)

> **موافقة بدون تعديل.** هذه قصة verification — لا تحتاج negative tests لأنها لا تنشئ كوداً.
>
> **القلق الوحيد:** AC-6 (sign-off) يدوي. لا أتمتة ممكنة.
>
> **مقبول للأن.** نراجع في retro Sprint 0.

---

> **توقيع Amelia:** "خطة بلا AC = أمنية. هذه القصة 6 AC قابلة للقياس آلياً، باستثناء sign-off اليدوي."
