# Secrets Checklist — نظام التوثيق

**Date:** 2026-05-09
**Owner:** صلاح
**Last Reviewed:** 2026-05-09

---

## 🔐 المفاتيح المطلوبة

| Secret | الموقع | الصلاحية | Used By | Rotation |
|---|---|---|---|---|
| `CLAUDE_API_KEY` | GitHub Repo Secrets | API access (Anthropic) | `tools/sad-ai-docgen/` (Story 2.1) | كل 90 يوم — أول rotation: **2026-08-09** |
| `NPM_PUBLISH_TOKEN` | GitHub Repo Secrets | publish to `@sad-lang` | `tools/npm-publish/` (Story 4.1) | كل 90 يوم |
| `PYPI_API_TOKEN` | GitHub Repo Secrets | publish `sad-doctest` | `tools/sad-doctest/` (Story 4.2) | كل 90 يوم |
| `GH_PAGES_DEPLOY_KEY` | GitHub Repo Secrets | gh-pages deploy | `.github/workflows/deploy-docs.yml` (Story 4.3) | غير محدودة (SSH key) |

---

## ✅ Status Checklist

- [ ] `CLAUDE_API_KEY` محفوظ في GitHub Secrets
- [ ] `NPM_PUBLISH_TOKEN` محفوظ
- [ ] `PYPI_API_TOKEN` محفوظ
- [ ] `GH_PAGES_DEPLOY_KEY` محفوظ
- [ ] `@sad-lang` npm organization موجودة
- [ ] `sad-doctest` PyPI name متوفر (مُحجوز)
- [ ] gh-pages branch موجود في GitHub repo

---

## 🛡️ Best Practices

1. **لا تكشف المفاتيح في:**
   - logs (CI logs مرئية للعموم في open source)
   - error messages
   - commit messages
   - issue comments
2. **استخدم masking تلقائي في GitHub Actions** (مفعَّل افتراضياً للـ secrets)
3. **rotate أي مفتاح فوراً** إن:
   - شُكّ في تسريبه
   - مر عليه > 90 يوم
   - غادر مطوّر له صلاحية
4. **لا تنسخ المفاتيح** إلى ملفات `.env` محلية مُنشَرة في git

---

## 🚫 Anti-Patterns ممنوعة

```yaml
# ❌ ممنوع — hardcoded في workflow
env:
  CLAUDE_API_KEY: "sk-ant-xxxxx"

# ❌ ممنوع — print في logs
- run: echo "$CLAUDE_API_KEY"

# ✅ صحيح — secret reference
env:
  CLAUDE_API_KEY: ${{ secrets.CLAUDE_API_KEY }}

# ✅ صحيح — masked usage
- run: ./tools/sad-ai-docgen --keyword=دالة
  env:
    CLAUDE_API_KEY: ${{ secrets.CLAUDE_API_KEY }}
```

---

## 📋 Pre-Wave 2 Checklist

قبل بدء Wave 2 (AI Drafts Pipeline):
- [ ] CLAUDE_API_KEY مُولَّد ومحفوظ
- [ ] @sad-lang npm org مسجَّل
- [ ] PyPI sad-doctest حجز اسم
- [ ] صلاح وقّع على هذه القائمة

---

**Next Review:** 2026-08-09 (rotation deadline)
