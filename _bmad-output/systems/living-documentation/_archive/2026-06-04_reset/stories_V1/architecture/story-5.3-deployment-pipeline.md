---
status: done
slug: story-5.3-deployment-pipeline
epic: "ADR-006b Epic 5 — enforce_docs_coverage + pipeline AI"
story_id: "5.3"
created: "2026-06-10"
author: "Amelia (bmad-agent-dev)"
priority: low
depends_on:
  - "5.1 — enforce_docs_coverage ✅"
  - "5.2 — AI Lessons pipeline ✅"
  - "Story 2.3 — CI Examples Runner ✅"
estimated_size: "2–3 أيام"
utm: "UTM-6.12"
---

# Story 5.3 — تكامل كامل + Deployment Pipeline

## البيان

بوصفي **مدير المشروع**،  
أريد **pipeline نشر كامل** (YAML → gen_docs → preview → deploy إلى السيرفر الخاص)،  
لكي **كل commit يُحدِّث التوثيق تلقائياً بدون تدخل يدوي**.

---

## معايير القبول

### AC-5.3-01: Trigger تلقائي عند تغيير YAML

**Given** commit يُغيِّر أي ملف في `data/language/`  
**When** يُدفَع للـ `main` branch  
**Then** CI يُشغِّل `gen_docs.py --channel vitepress`  
**And** يُبني موقع VitePress  
**And** يُبنى ويُرفَع إلى `/opt/sad-lang-website/` على `185.47.174.39` (sad-lang.org) عبر rsync over SSH

### AC-5.3-02: فحص أمثلة CI Runner

**Given** أمثلة بـ `runnable: true` في YAML  
**When** يُشغَّل CI  
**Then** يُشغِّل كل مثال ضد `sad.exe` ويتحقق من `expected_output`  
**And** يفشل برسالة واضحة إذا لم يُطابق

### AC-5.3-03: Coverage Gate

**Given** commit جديد يُقلِّل `description_ar` coverage  
**When** يُشغَّل `check_docs_coverage.py --check --threshold 90`  
**Then** يطبع تحذيراً في CI log  
**And** لا يُوقف البناء (حتى تبلغ 95%)

### AC-5.3-04: توقيت البناء

**Given** pipeline كامل (gen_all + gen_docs + check_coverage + VitePress build)  
**When** يُشغَّل في CI  
**Then** يكتمل خلال ≤ 5 دقائق (NFR-1: gen_docs ≤ 30 ثانية)

### AC-5.3-05: Preview لـ Pull Requests

**Given** pull request على `main`  
**When** يُفتَح PR  
**Then** CI يُنشئ preview URL للتوثيق على Netlify/Vercel (اختياري)

---

## المهام

### T1: GitHub Actions Workflow

**الملف:** `.github/workflows/docs-deploy.yml` (تحديث الموجود)

```yaml
name: Deploy Docs

on:
  push:
    branches: [main]
    paths:
      - 'data/language/**'
      - 'scripts/codegen/**'
      - 'website/**'
  pull_request:
    branches: [main]
    paths:
      - 'data/language/**'

jobs:
  build-and-deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with: {python-version: '3.12'}
      - name: Install Python deps
        run: pip install pyyaml
      - name: Run codegen
        run: python scripts/codegen/gen_all.py
      - name: Check docs coverage
        run: |
          python scripts/codegen/check_docs_coverage.py --check --threshold 90
      - name: Generate docs
        run: python scripts/codegen/gen_docs.py --channel vitepress
      - uses: actions/setup-node@v4
        with: {node-version: '20'}
      - name: Build VitePress
        run: |
          npm ci --prefix website
          npm run build --prefix website
      - name: Deploy to private server (sad-lang.org)
        if: github.event_name == 'push'
        env:
          SSH_PRIVATE_KEY: ${{ secrets.DEPLOY_SSH_KEY }}
          SERVER_HOST: ${{ secrets.DEPLOY_HOST }}        # 185.47.174.39
          SERVER_USER: ${{ secrets.DEPLOY_USER }}        # deploy
          SERVER_PATH: /opt/sad-lang-website
        run: |
          mkdir -p ~/.ssh
          echo "$SSH_PRIVATE_KEY" > ~/.ssh/deploy_key
          chmod 600 ~/.ssh/deploy_key
          ssh-keyscan -H "$SERVER_HOST" >> ~/.ssh/known_hosts
          rsync -avz --delete \
            -e "ssh -i ~/.ssh/deploy_key" \
            website/.vitepress/dist/ \
            "$SERVER_USER@$SERVER_HOST:$SERVER_PATH/"
          ssh -i ~/.ssh/deploy_key "$SERVER_USER@$SERVER_HOST" \
            "sudo systemctl reload nginx"
```

> **ملاحظة:** تُعرَّف أسرار `DEPLOY_SSH_KEY`، `DEPLOY_HOST`، `DEPLOY_USER` في GitHub Repository Secrets. سكريبتات النشر اليدوية موجودة في [`deployment/deploy.ps1`](../../../../../deployment/deploy.ps1) و [`deployment/deploy.sh`](../../../../../deployment/deploy.sh).

### T2: تحديث cmake/codegen.cmake

إضافة target `sad_docs_codegen` يعتمد على YAML files:
```cmake
set(DOCS_YAML_FILES
    "${CMAKE_SOURCE_DIR}/data/language/builtins/core.yaml"
    "${CMAKE_SOURCE_DIR}/data/language/builtins/stdlib_math.yaml"
    # ... etc
)
add_custom_target(sad_docs_codegen
    COMMAND Python3::Interpreter scripts/codegen/gen_docs.py --channel vitepress
    DEPENDS ${DOCS_YAML_FILES}
    COMMENT "توليد توثيق VitePress من YAML"
)
```

### T3: اختبار end-to-end

- [ ] اختبار local: `gen_all.py` + `gen_docs.py` + `check_docs_coverage.py` كلها تمر
- [ ] اختبار VitePress build: `npm run build --prefix website`
- [ ] اختبار deploy (dry-run أو على fork)

---

## الملفات المتوقع تعديلها

| الملف | الإجراء |
|-------|--------|
| `.github/workflows/docs-deploy.yml` | تعديل — إضافة gen_docs + check_coverage |
| `cmake/codegen.cmake` | تعديل — إضافة sad_docs_codegen target |

---

## ملاحظات تقنية

1. **`sad.exe` في CI**: يُبنى كـ step مستقل قبل CI runner للأمثلة
2. **NFR-5**: LSP hover ≤ 100ms — يُقاس في اختبارات منفصلة (story لاحقة)
3. **deploy فقط على main**: PRs تحصل على build check فقط (لا deploy)

## سجل المطور

| التاريخ | المهمة | الحالة | ملاحظات |
|---------|--------|--------|---------|
| — | T1 | ⬜ | — |
| — | T2 | ⬜ | — |
| — | T3 | ⬜ | — |
