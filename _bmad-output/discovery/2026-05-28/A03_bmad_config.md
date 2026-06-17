# ⚙️ تقرير الاكتشاف — منطقة BMAD Config (W3)

> **تاريخ التقرير:** 2026-05-28
> **النطاق:** `_bmad/` كاملاً
> **الوضع:** READ-ONLY (ملخص)

## 1. ملخص المنطقة

| المقياس | القيمة |
|---|---|
| إجمالي الملفات | 200+ |
| الامتدادات | `.md` (60+), `.yaml` (10+), `.csv` (20+), `.py` (30+), `.json` (5+) |
| آخر تحديث | 2026-04-13 |
| المساحة | ~5-8 MB |

## 2. الوحدات الستة

| الوحدة | الإصدار | الدور | الاستخدام |
|---|---|---|---|
| **_config** | 6.3.0 | SoT المركزي | ✅ إجباري |
| **bmb** | 1.5.0 | بناء المهارات (Builder) | ✅ 85% |
| **bmm** | 6.3.0 | المنهجية (4 مراحل) | ✅ 90% |
| **cis** | 0.1.9 | الأدوات الإبداعية | ⚠️ 40% |
| **core** | 6.3.0 | الأدوات الأساسية | ✅ 100% |
| **tea** | 1.7.2 | معمارية الاختبار | ⚠️ 30% |

## 3. الملفات الرئيسية

| الملف | المسار | الوظيفة |
|---|---|---|
| manifest.yaml | [_bmad/_config/manifest.yaml](../../../_bmad/_config/manifest.yaml) | SoT الرئيسي — تسجيل 5 وحدات مع SHA256 |
| config.yaml | [_bmad/bmm/config.yaml](../../../_bmad/bmm/config.yaml) | إعدادات BMM (user_name=saleh, communication_language=arabic) |
| files-manifest.csv | [_bmad/_config/files-manifest.csv](../../../_bmad/_config/files-manifest.csv) | جدول الملفات بـ SHA256 hashes |
| agents-manifest.csv | [_bmad/_config/agents-manifest.csv](../../../_bmad/_config/agents-manifest.csv) | جدول الوكلاء (16 وكيل) |
| skills-manifest.csv | [_bmad/_config/skills-manifest.csv](../../../_bmad/_config/skills-manifest.csv) | جدول المهارات (100+) |

## 4. الأنظمة المُكتشفة داخل الوحدات

### bmm — المنهجية الرئيسية
- 4 مراحل: 1-analysis, 2-plan-workflows, 3-solutioning, 4-implementation
- 16 وكيل متخصص (Mary, John, Sally, Winston, Amelia, Murat, ...)

### core — الأساسيات
- workflows مشتركة بين كل الوحدات

### bmb — البنّاء
- بناء/تعديل/تحليل skills + workflows

### cis — الأدوات الإبداعية
- 4 مجالات: Brainstorming, Design Thinking, Problem Solving, Innovation Strategy
- Storytelling + Presentation

### tea — معمارية الاختبار
- ATDD, Automate, CI, Framework, NFR, Test Design, Test Review, Trace

## 5. التخصيص

- **user_name:** saleh
- **communication_language:** arabic
- **document_output_language:** arabic
- كل الوحدات معايرة للعربية

## 6. الملاحظات الاستراتيجية

### ✅ نقاط قوة
- مصدر حقيقة مركزي قوي (manifest.yaml + SHA256 hashes)
- تخصيص عربي كامل
- تتبع دقيق للإصدارات

### ⚠️ ملاحظات
- معظم الملفات افتراضية من npm (لا تعديلات جذرية)
- التخصيص محصور على الإعدادات فقط (سهل التحديث)
- وحدتا cis و tea استخدامهما منخفض (40% و 30%)

### 🔴 سؤال مفتوح
- هل ينعكس BMAD config على `.github/skills/`؟ العلاقة بينهما؟
- كيف يتزامن `_bmad/` (المُحدَّث من npm) مع `.github/skills/` (المتطور محلياً)؟

---
**أُنشئ:** 2026-05-28 | **النوع:** READ-ONLY Summary
