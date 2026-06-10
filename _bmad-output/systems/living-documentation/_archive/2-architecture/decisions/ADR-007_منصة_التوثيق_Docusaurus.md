# ADR-007: اعتماد VitePress منصةً رسمية للتوثيق

**الحالة:** مُعتمَد (مُعدَّل — راجع السجل)  
**التاريخ:** 2026-07-09 | آخر تحديث: 2026-01-01  
**المرتبط بـ:** ADR-006b (pipeline توليد التوثيق), ADR-008 (علاقة الموقع بالمشروع)  
**الموقع في خارطة المشروع:** شرط مسبق قبل Story 2.1 (UTM-6.9)  
**صاحب القرار:** saleh  

---

## 1. السياق

بعد اعتماد ADR-006b (pipeline توليد التوثيق تلقائياً)، يتطلب Epic 2 اختيار منصة عرض واحدة.

**الاكتشاف:** أثناء التخطيط اكتُشف أن موقع `sad-lang.org` مبني بالفعل على **VitePress v1.6.3** في مستودع منفصل (`C:\s_lang\website\`) ومُجهَّز بالكامل:

| الميزة | الحالة |
|--------|--------|
| i18n (ar + en) | ✅ مُعدَّ (locales في config) |
| RTL + خط Cairo | ✅ مُفعَّل |
| Shiki grammar لغة ص | ✅ مكتوب ومُسجَّل |
| sidebar.ts (70+ رابط) | ✅ منظَّم |
| Versioning | 🟡 مُخطَّط لاحقاً |

قرار سابق (أول نسخة من هذا ADR) اختار Docusaurus بافتراض "لا موقع موجود" — وهذا الافتراض خاطئ.

---

## 2. المتطلبات التي تحكم القرار

| المتطلب | الأولوية | VitePress | Docusaurus |
|---------|---------|-----------|-----------|
| i18n عربي/إنجليزي | حرجة | ✅ مُعدَّ بالفعل | ✅ مدمج (لكن يحتاج إعداداً من الصفر) |
| RTL + خط عربي | حرجة | ✅ مُفعَّل | ✅ ممكن (يحتاج config) |
| Syntax highlighting لغة ص | عالية | ✅ موجود (Shiki grammar) | 🟡 يحتاج إعادة كتابة |
| Markdown عادي (لا MDX) | عالية | ✅ Markdown + Vue components | يفرض MDX — يُعقِّد generator |
| pipeline output مباشر | عالية | ✅ يكتب `.md` مباشرة | يحتاج frontmatter خاص بـ MDX |
| البنية التحتية موجودة | عالية | ✅ موقع كامل جاهز | ❌ يحتاج بناء من الصفر |
| Build speed | متوسطة | ✅ سريع (Vite) | أبطأ (webpack) |
| Versioning | منخفضة | plugin مستقبلي | مدمج |

---

## 3. القرار

**المُختار: VitePress v1.6.3 (الموقع الحالي — مُعتمَد رسمياً)**

**السبب الجوهري:**  
الموقع موجود ومجهَّز بالكامل. اختيار Docusaurus كان سيعني تجاهل بنية تحتية ناضجة ومُختبَرة وإعادة بنائها من الصفر لأسباب لا تبرر التكلفة. VitePress يُنتج Markdown عادياً — يُبسِّط `render_vitepress.py` ولا يحتاج MDX أو React components.

البنية التحتية لـi18n عربي/إنجليزي **موجودة بالفعل في `website/`** — التنفيذ الكامل للترجمة مُؤجَّل لـUTM-6.11.

---

## 4. نتائج القرار

### 4.1. التغييرات المطلوبة

| التغيير | الملف / المكان | Story |
|---------|---------------|-------|
| نقل `C:\s_lang\website\` → `s-programming-language/website/` | جذر المشروع | 2.0 |
| إضافة `website/node_modules/`, `website/.vitepress/dist/` إلى `.gitignore` | `.gitignore` | 2.0 |
| إنشاء `.github/workflows/docs-deploy.yml` (gh-pages) | `.github/workflows/` | 2.0 |
| نقل `mkdocs.yml` → `archived/docs/mkdocs_legacy.yml` | جذر المشروع | 2.0 |
| بناء `render_vitepress.py` بدلاً من `render_docusaurus.py` | `scripts/codegen/` | 2.2 |
| output path: `website/docs/docs/stdlib/` و`website/docs/docs/api/` | `gen_docs.py` | 2.1 |

### 4.2. الانعكاسات على ADR-006b

- **Epic 2 (UTM-6.9):** Renderer الأول يُبنى لـ VitePress (`render_vitepress.py`) — يُنتج `.md` بـ VitePress frontmatter فقط (لا MDX)
- **Epic 5 (UTM-6.12):** i18n العربي/الإنجليزي يستفيد من `locales` القائمة في `website/.vitepress/config.mts`
- `--channel vitepress` يستبدل `--channel docusaurus` في جميع الأوامر

### 4.3. المخاطر المقبولة

| الخطر | التخفيف |
|-------|---------|
| VitePress API قد يتغير (breaking change) | pin `"vitepress": "^1.6.3"` في package.json |
| `website/node_modules/` يُبطِّئ `git status` | `.gitignore` صريح + `git config core.untrackedCache true` |
| sidebar.ts يحتاج تحديثاً يدوياً عند إضافة صفحات مُولَّدة | Story 5.1 أو أداة مساعدة لفحص التوافق |

---

## 5. البدائل المرفوضة

### Docusaurus v3

**رُفض لأن:**
- الموقع مبني على VitePress ومُختبَر — استبداله يعني: حذف Shiki grammar، إعادة كتابة sidebar، نقل Cairo font config، تحويل Vue components لـ React
- MDX يُعقِّد `render_docusaurus.py` (يحتاج import statements + JSX components) بينما VitePress يقبل Markdown عادياً
- لا ميزة إضافية حقيقية تبرر التكلفة في هذه المرحلة

### MkDocs (Material Theme)

**رُفض لأن:** i18n العربي plugin غير رسمي، `mkdocs.yml` موجود في `archived/` فقط كـ legacy

---

## 6. المراجعة والنقض

هذا القرار يُراجَع إذا:
- VitePress أسقط دعم RTL العربي أو i18n
- قُرِّر نقل الموقع لمنصة مدارة (Netlify/Vercel hosted docs) تشترط Docusaurus
- احتجنا لـ versioning متعدد الإصدارات (VitePress يدعم ذلك بـ plugin)

---

## 7. السجل

| التاريخ | الحدث |
|---------|------|
| 2026-07-09 | القرار الأولي: **Docusaurus v3** — بافتراض لا موقع موجود + i18n مطلوب |
| 2026-01-01 | مراجعة: اكتُشف موقع VitePress ناضج في `C:\s_lang\website\` مع i18n+RTL+Shiki |
| 2026-01-01 | **القرار المُعدَّل: VitePress** — اعتماد رسمي للموقع الحالي. راجع ADR-008 لتفاصيل النقل |
