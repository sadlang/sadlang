---
inputDocuments:
  - _bmad-output/planning-artifacts/prd-docs-system.md
  - _bmad-output/planning-artifacts/architecture-docs-system.md
  - _bmad-output/party-sessions/2026-05-09_documentation_system.md
workflowType: ux-design
scope: تجربة قارئ نظام التوثيق
---

# UX Design Specification — موقع توثيق لغة ص

**المُصمّمة:** Sally (UX Designer)
**التاريخ:** 2026-05-09
**الإصدار:** 1.0.0

---

## 1. الفلسفة التصميمية

> **"الوثائق ليست مجرد معلومات — هي الانطباع الأول عن اللغة. صفحة الهبوط أهم من 100 ميزة."**

ثلاث قواعد ذهبية:
1. **ثنائية اللغة هوية لا ميزة** — تظهر من اليوم الأول، في كل صفحة، في كل مكوّن.
2. **RTL أصلي** — لا hacks CSS، VitePress + `dir="rtl"` كامل.
3. **جودة بصرية = ثقة** — الخطوط، التباعد، الألوان كلها مدروسة.

---

## 2. متطلبات تصميم UX (UX-DRs)

### UX-DR-Group A: الهوية البصرية والخطوط

- **UX-DR-001:** الخط العربي الأساسي: **Tajawal** (Google Fonts) — مُحسّن للقراءة الطويلة.
- **UX-DR-002:** الخط الإنجليزي الأساسي: **Inter** — متطابق مع Tajawal في المحاذاة العمودية.
- **UX-DR-003:** الكود (AR + EN): **Fira Code** مع ligatures + دعم العربية.
- **UX-DR-004:** نظام ألوان: Light + Dark mode، توقيع لوني `#1976D2` (أزرق ص) + accent `#00897B`.
- **UX-DR-005:** Logo: لوغو "ص" مدمج في navbar، مرئي في الوضعين.

### UX-DR-Group B: i18n والتنقّل بين اللغات

- **UX-DR-010:** هيكل URLs: `/ar/...` و `/en/...` (لا `/?lang=`).
- **UX-DR-011:** `<LangSwitch>` في navbar، يبدّل بدون فقد الموقع: `/ar/ref/print` ↔ `/en/ref/print`.
- **UX-DR-012:** تفضيل اللغة محفوظ في `localStorage` كـ `sad-docs-lang`.
- **UX-DR-013:** `hreflang` tags في `<head>` لكل صفحة (SEO ثنائي).
- **UX-DR-014:** فحص آلي: ترجمة جزئية تعرض banner: "هذه الصفحة غير مترجمة بعد، شارك في الترجمة".
- **UX-DR-015:** Direction switching سلس بدون layout shift (CLS = 0).

### UX-DR-Group C: صفحة الهبوط (Landing)

- **UX-DR-020:** Hero بسيط: "لغة ص — برمجة بلغتك الأم" + زر "ابدأ الآن" (CTA).
- **UX-DR-021:** أربع بطاقات "أنا..." (Audience-driven entry):
  - 👶 **مبتدئ** — "علّمني من الصفر" → tutorial track
  - 🎓 **معلّم** — "أريد دروس قابلة للتدريس" → curriculum
  - 🛠️ **مختص** — "أعرف البرمجة، أرني المرجع" → reference
  - 🤝 **مساهم** — "أريد المساهمة في اللغة" → contribution guide
- **UX-DR-022:** قسم "ابدأ في 90 ثانية" مع `<CodeBlock>` تفاعلي (نسخ بزر واحد).
- **UX-DR-023:** قسم "لماذا لغة ص؟" مع 3 ميزات بصرية (سرعة، عربية، تزامن).

### UX-DR-Group D: مكونات Vue المطلوبة

- **UX-DR-030:** `<DocLang lang="ar|en">` — يخفي/يظهر محتوى حسب اللغة الحالية.
- **UX-DR-031:** `<CodeBlock dir="rtl|ltr" lang="sad|js|cpp" runnable="true">` — مع زر "▶ تشغيل" (مرحلة 2).
- **UX-DR-032:** `<LangSwitch />` — التبديل بين AR/EN.
- **UX-DR-033:** `<Tip>` — نصيحة (أيقونة 💡 + خلفية صفراء فاتحة).
- **UX-DR-034:** `<Warning>` — تحذير (أيقونة ⚠️ + خلفية برتقالية).
- **UX-DR-035:** `<KnowMore href="/...">` — رابط استكشاف عميق.
- **UX-DR-036:** `<KeywordCard id="function" />` — يقرأ من keywords.json ويعرض البطاقة.

### UX-DR-Group E: قابلية الوصول (Accessibility)

- **UX-DR-040:** WCAG 2.1 Level AA — إلزامي.
- **UX-DR-041:** Color contrast ≥ 4.5:1 لكل النصوص (axe-clean).
- **UX-DR-042:** كل المكونات interactive قابلة للوصول بـ keyboard (Tab + Enter + Escape).
- **UX-DR-043:** `lang="ar"` و `dir="rtl"` صحيحة لكل بلوك عربي (يُساعد screen readers).
- **UX-DR-044:** Focus indicators واضحة (outline 2px solid).
- **UX-DR-045:** Skip links: "تخطّي إلى المحتوى" / "Skip to content".
- **UX-DR-046:** Alt text لكل صورة، captions لكل فيديو (مرحلة 2).

### UX-DR-Group F: Responsive Design

- **UX-DR-050:** Breakpoints: mobile (< 768px), tablet (768-1024), desktop (> 1024).
- **UX-DR-051:** Mobile: sidebar يصبح hamburger menu، code blocks scroll أفقياً.
- **UX-DR-052:** Tablet: sidebar مخفي بشكل افتراضي، يظهر بزر.
- **UX-DR-053:** Desktop: sidebar ثابت، TOC على اليمين (RTL) أو اليسار (LTR).
- **UX-DR-054:** Tap targets ≥ 44×44px على mobile (Apple HIG).

### UX-DR-Group G: تجربة القارئ

- **UX-DR-060:** TTFHW (Time To First Hello World) < 90 ثانية (مقاس بـ analytics).
- **UX-DR-061:** كل صفحة Reference لها: signature، example، related links، "Edit this page" link.
- **UX-DR-062:** Search shortcut: `/` يفتح search bar، `Esc` يغلق.
- **UX-DR-063:** Copy-to-clipboard على كل code block (toast confirmation).
- **UX-DR-064:** Reading time estimate في كل صفحة tutorial.
- **UX-DR-065:** Progress indicator في tutorials طويلة (1/8 ، 2/8 ...).

### UX-DR-Group H: SEO وMeta

- **UX-DR-070:** كل صفحة لها `<title>`, `<meta description>`, OpenGraph tags.
- **UX-DR-071:** sitemap.xml يولّد آلياً، يشمل AR + EN.
- **UX-DR-072:** Structured data (JSON-LD) للصفحات Reference (Schema.org `TechArticle`).
- **UX-DR-073:** robots.txt يسمح للجميع، Crawl-delay مناسب.

### UX-DR-Group I: الأداء

- **UX-DR-080:** Lighthouse Performance ≥ 90 على 10 صفحات عيّنة.
- **UX-DR-081:** First Contentful Paint < 1.5s على 3G simulation.
- **UX-DR-082:** Largest Contentful Paint < 2.5s.
- **UX-DR-083:** Cumulative Layout Shift < 0.1.
- **UX-DR-084:** خطوط تُحمّل بـ `font-display: swap` لمنع FOIT.
- **UX-DR-085:** صور بـ `lazy` loading + `srcset` للـ responsive.

### UX-DR-Group J: الحالات الحدّية (Edge Cases UX)

- **UX-DR-090:** صفحة 404: "الصفحة غير موجودة" + اقتراحات + رابط للهبوط.
- **UX-DR-091:** صفحة 500/error: "حدث خطأ" + رابط GitHub issues.
- **UX-DR-092:** صفحة "قيد الترجمة": تظهر بدلاً من 404 إن وُجدت لغة واحدة فقط.
- **UX-DR-093:** Offline mode (Service Worker — مرحلة 2): "أنت غير متصل، عرض النسخة المخزنة".

---

## 3. الـ User Journeys الرئيسية

### Journey 1: المبتدئ يكتب أول برنامج
1. يصل إلى الهبوط → يضغط "👶 مبتدئ"
2. يصل إلى `/ar/learn/first-program/` → يرى "ابدأ في 90 ثانية"
3. ينسخ كود `اطبع_سطر("مرحبا بالعالم")` → يلصقه في playground
4. يرى الإخراج → يشعر بالنجاح
5. **النجاح: TTFHW < 90s** ✅

### Journey 2: المختص يبحث عن signature لـ builtin
1. يضغط `/` لفتح search → يكتب "اطبع"
2. يصل مباشرة إلى `/ar/ref/print/`
3. يرى: signature، params، example، related
4. ينسخ المثال → يرجع للعمل
5. **النجاح: < 30 ثانية من البحث للحل** ✅

### Journey 3: مساهم يصلح خطأ في الوثائق
1. يقرأ صفحة → يرى خطأ
2. يضغط "Edit this page" (في footer كل صفحة)
3. يفتح GitHub editor → يصلح → يفتح PR
4. CI يفحص (G2, G3, G4) → يمر → merge
5. **النجاح: PR ينتهي خلال 10 دقائق** ✅

---

## 4. wireframes نصية (low-fidelity)

### Landing Page
```
┌─────────────────────────────────────────┐
│ [ص] لغة ص    [بحث /]    [AR|EN] [🌙]    │  ← navbar
├─────────────────────────────────────────┤
│                                         │
│         لغة ص — برمجة بلغتك الأم        │  ← hero
│              [ ابدأ الآن ▶ ]             │
│                                         │
├─────────────────────────────────────────┤
│  ┌───┐  ┌───┐  ┌───┐  ┌───┐            │  ← 4 cards
│  │👶 │  │🎓 │  │🛠️│  │🤝 │            │
│  │مبت│  │معل│  │مخت│  │مسا│            │
│  └───┘  └───┘  └───┘  └───┘            │
├─────────────────────────────────────────┤
│       ابدأ في 90 ثانية                  │
│  ┌─────────────────────────────┐       │
│  │ اطبع_سطر("مرحبا بالعالم")    │ [📋] │
│  └─────────────────────────────┘       │
└─────────────────────────────────────────┘
```

### Reference Page (مثال: /ar/ref/print/)
```
┌──────────────────────────────────────────┐
│ [navbar]                                 │
├──────────┬────────────────────┬──────────┤
│          │                    │          │
│ [sidebar]│  # اطبع            │  [TOC]   │
│          │                    │          │
│ - مرجع   │  ## التوقيع         │  - وصف   │
│   - اطبع │  ```sad            │  - أمث.  │
│   - اقرأ │  اطبع(...قيم)       │  - ذو ص. │
│          │  ```               │          │
│          │                    │          │
│          │  ## مثال           │          │
│          │  ```sad            │          │
│          │  اطبع("مرحبا")      │          │
│          │  ```               │          │
│          │                    │          │
│          │  [Edit this page]  │          │
└──────────┴────────────────────┴──────────┘
```

---

## 5. القرارات التصميمية الحاسمة

| القرار | الخيار | السبب |
|---|---|---|
| إطار الموقع | **VitePress** | i18n أنضج من MkDocs، مكونات Vue مرنة |
| الخط العربي | **Tajawal** | أوضح من Cairo للقراءة الطويلة |
| Layout | **3-column desktop, single mobile** | معيار صناعي |
| Code highlighting | **Shiki** (مدمج VitePress) | تطابق مع GitHub |
| Search | **Local search** (مدمج) في v1، Algolia في v2 | تجنّب dependency early |
| Theme | **Light + Dark + Auto** | احترام تفضيل النظام |
| Animations | **Minimal** (تقليل CLS) | الأداء > البهرجة |

---

## 6. ما لا نفعله (Anti-patterns)

- ❌ Carousels تلقائية (يكره المستخدمون)
- ❌ Modals فجائية (CTA يكفي)
- ❌ Newsletter popup (مزعج)
- ❌ Tracking عدواني (privacy first)
- ❌ Auto-play videos
- ❌ Hamburger menu على desktop (المساحة كافية)
- ❌ Infinite scroll في tutorials (pagination أوضح)

---

## 7. الموافقة

- ✅ Sally: التصميم محقق للرؤية البصرية كاملة
- ✅ Winston: قابل للتنفيذ بـ VitePress + Vue components
- ✅ Murat: A11y + Performance gates مدمجة
- ✅ Quinn: bilingual_check يفرض UX-DR-014
