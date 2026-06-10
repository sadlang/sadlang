---
review_key: ux-review-vue-components-docs-system
reviewer: Sally (UX Designer)
created: 2026-05-09
target_stories:
  - 3.2 — Vue Components (KeywordCard / BuiltinPlayground / ErrorRecoveryDemo / DirectiveExample)
status: design-review-complete
inputs_inspected:
  - C:\s_lang\website\.vitepress\theme\style.css (متغيرات + RTL)
  - C:\s_lang\website\.vitepress\theme\components\ui\FeatureGrid.vue (نمط بطاقات)
  - C:\s_lang\website\.vitepress\theme\components\ui\HomeHero.vue
  - C:\s_lang\website\.vitepress\theme\components\playground\CodePlayground.vue
  - C:\s_lang\website\.vitepress\theme\components\playground\CodePlaygroundV2.vue
  - C:\s_lang\website\.vitepress\theme\components\playground\SadCodeRunner.vue
  - C:\s_lang\website\.vitepress\theme\components\modals\QuizModal.vue
  - C:\s_lang\website\.vitepress\theme\components\badges\AchievementBadges.vue
---

# 🎨 UX Review — Vue Components للموقع (Story 3.2)

## 📌 الخلاصة (TL;DR)

**حكم Sally:**
الموقع يحوي بالفعل **نظام تصميم ناضج** مع متغيرات CSS واضحة، RTL محكم، 4 playground components، modals، badges. القرارات في Architecture v2 جيدة لكنها **تتجاهل 60% من الموجود**.

**التوصية الجوهرية:** بدل إنشاء `BuiltinPlayground.vue` جديد، **أعد استخدام** `SadCodeRunner.vue` الموجود (هو بالفعل CodeMirror + executor). نفس المبدأ لباقي الـ3.

**نتيجة:** Story 3.2 تنخفض من **0.5 يوم → 0.25 يوم** (-50%).

---

## 1️⃣ ما اكتشفته أثناء المراجعة

### 1.1 نظام التصميم الموجود (style.css)

✅ **متغيرات CSS متينة:**
- `--sad-primary` `#0ea5e9` (أزرق العلامة)
- `--sad-accent` `#f59e0b` (برتقالي للتأكيد)
- `--sad-green` `#22c55e` (نجاح)
- `--sad-red` `#ef4444` (خطأ/تحذير)
- `--vp-c-bg-soft`, `--vp-c-border`, `--vp-c-text-1/2` (vitepress integration)

✅ **خطوط:**
- نص: `Cairo` (مثالي للعربية)
- كود: `JetBrains Mono`

✅ **RTL/LTR محكم:**
- `html { direction: rtl }` افتراضي
- `pre, code, .shiki { direction: ltr !important }` (الكود LTR دائماً)

✅ **Dark mode كامل** عبر `.dark` selector

### 1.2 Components موجودة قابلة لإعادة الاستخدام

| Component موجود | يُستبدل/يُستخدم لأجل |
|---|---|
| `FeatureGrid.vue` | **نمط بطاقات** لـ KeywordCard (نفس الأسلوب) |
| `SadCodeRunner.vue` | **BuiltinPlayground** (بدل إنشاء جديد) |
| `CodePlaygroundV2.vue` | **fallback** لو SadCodeRunner لم يكفِ |
| `QuizModal.vue` | **DirectiveExample** modal |
| `AchievementBadges.vue` | **نمط شارات** للـ `since` و `category` |

### 1.3 ما لا يوجد ويحتاج إنشاء فعلاً

| Component جديد | السبب |
|---|---|
| `KeywordCard.vue` | لا يوجد component مخصص لكلمة مفتاحية |
| `ErrorRecoveryDemo.vue` | لا يوجد عرض رسائل أخطاء |

**المُختصر:** Story 3.2 تحتاج **2 components جديدة** فقط، وليس 4.

---

## 2️⃣ التصميم المُقترَح لكل Component

### 2.1 `KeywordCard.vue` (جديد)

**Wireframe:**
```
┌────────────────────────────────────────────────┐
│ ┌─────────┐                                    │
│ │  دالة   │   function          [📌 v1.0.0]   │
│ └─────────┘                                    │
│                                                │
│ تعريف دالة قابلة للاستدعاء.                    │
│ ─────────────────────────────────────          │
│  مثال:                                         │
│  ┌──────────────────────────────────┐          │
│  │ دالة جمع(أ، ب)                    │          │
│  │     ارجع أ + ب                    │          │
│  │ نهاية                             │          │
│  └──────────────────────────────────┘          │
│                                                │
│  [#declaration] [#core]    📚 شرح كامل ←       │
└────────────────────────────────────────────────┘
```

**Props:**
```typescript
interface KeywordCardProps {
  ar: string          // 'دالة'
  en: string          // 'function'
  category: string    // 'declaration'
  since: string       // '1.0.0'
  summary: string     // وصف قصير
  example: string     // كود مثال
  docPath: string     // /book/keywords/دالة
}
```

**يستخدم:** `--sad-primary` للحدود، `--vp-c-bg-soft` للخلفية، `Cairo` للعربية، `JetBrains Mono` للكود.

**Hover state:** نفس `FeatureGrid.feature-item:hover { transform: translateY(-4px) }`.

### 2.2 `BuiltinPlayground.vue` = wrapper حول `SadCodeRunner.vue`

**بدل إنشاء component كامل جديد:**

```vue
<template>
  <div class="builtin-playground">
    <h4>{{ name }}</h4>
    <p>{{ description }}</p>
    <SadCodeRunner
      :initial-code="example"
      :readonly="false"
      @run="onRun"
    />
    <div class="signature">
      <strong>التوقيع:</strong> <code dir="ltr">{{ signature }}</code>
    </div>
  </div>
</template>

<script setup lang="ts">
import SadCodeRunner from '../playground/SadCodeRunner.vue'
defineProps<{
  name: string         // 'اطبع_سطر'
  description: string
  signature: string    // 'اطبع_سطر(...قيم: أي): فراغ'
  example: string
}>()
</script>
```

**التوفير:** ~150 سطر كود.

### 2.3 `ErrorRecoveryDemo.vue` (جديد)

**Wireframe:**
```
┌────────────────────────────────────────────────┐
│  ❌ E0042 — متغير غير معرّف                    │
│ ─────────────────────────────────────          │
│  ┌──────────────────────────────────┐          │
│  │ اطبع(س)        ← السطر 3         │ (أحمر)  │
│  └──────────────────────────────────┘          │
│                                                │
│  💡 الحل المقترح:                              │
│  ┌──────────────────────────────────┐          │
│  │ متغير س = 5                      │ (أخضر)  │
│  │ اطبع(س)                          │          │
│  └──────────────────────────────────┘          │
│                                                │
│  [انسخ الحل]                                   │
└────────────────────────────────────────────────┘
```

**Props:**
```typescript
interface ErrorRecoveryDemoProps {
  errorCode: string       // 'E0042'
  errorTitle: string      // 'متغير غير معرّف'
  brokenCode: string
  fixedCode: string
  hint: string            // الشرح
  errorLine?: number      // للتظليل
}
```

**Tokens:**
- خلفية الكود المكسور: `rgba(239, 68, 68, 0.1)` (--sad-red شفاف)
- خلفية الحل: `rgba(34, 197, 94, 0.1)` (--sad-green شفاف)

### 2.4 `DirectiveExample.vue` = نمط FeatureGrid مُكيَّف

**بدل modal جديد**، استخدم نفس نمط `FeatureGrid.vue`:

```vue
<template>
  <div class="directive-card" :class="{ unsafe: directive.unsafe }">
    <code class="directive-name" dir="ltr">@{{ directive.name }}</code>
    <p>{{ directive.description }}</p>
    <pre><code class="language-sad" dir="ltr">{{ directive.example }}</code></pre>
    <div v-if="directive.unsafe" class="warning">⚠️ توجيه غير آمن</div>
  </div>
</template>
```

**يستخدم:** `class="warning"` بـ `--sad-accent` (برتقالي).

---

## 3️⃣ مخاوف A11y (Accessibility)

| المخاوف | الحل |
|---|---|
| KeywordCard `<h3>` بدون `aria-label` بالعربية | أضف `aria-label="كلمة مفتاحية: دالة"` |
| ErrorRecoveryDemo color-only meaning (أحمر/أخضر) | أضف ❌/💡 icons + `role="alert"` |
| BuiltinPlayground keyboard navigation | `SadCodeRunner` يدعمه أصلاً (مُختبر) |
| DirectiveExample contrast ratio | تأكد warning بـ `--sad-accent` على dark mode > 4.5:1 |

**Required CI check:** `axe-playwright` على كل صفحة تحوي هذه الـ components — موثَّق في G6 ضمن Architecture v2.

---

## 4️⃣ i18n Considerations

✅ **جميع الـ4 components تدعم RTL/LTR تلقائياً** عبر:
- `direction: rtl` على `<html>` افتراضي
- `dir="ltr"` صريح على عناصر الكود
- لا hard-coded margins (يستخدم `inline-start` / `inline-end`)

⚠️ **تنبيه:** في `/en/` route، VitePress يحول `<html lang="en" dir="ltr">` تلقائياً عبر config.mts الموجود. تأكد أن components لا تكسر في LTR mode.

---

## 5️⃣ Design Tokens Mapping (للتطبيق)

| العنصر | Token | Fallback |
|---|---|---|
| خلفية البطاقة | `var(--vp-c-bg-soft)` | `#161b22` (dark) / `#f6f8fa` (light) |
| الحدود | `var(--vp-c-border)` | `#30363d` / `#d0d7de` |
| نص أساسي | `var(--vp-c-text-1)` | `#e6edf3` / `#1f2328` |
| نص ثانوي | `var(--vp-c-text-2)` | `#8b949e` / `#656d76` |
| تأكيد العلامة | `var(--sad-primary)` | `#0ea5e9` |
| نجاح/حل | `var(--sad-green)` | `#22c55e` |
| خطأ | `var(--sad-red)` | `#ef4444` |
| تحذير | `var(--sad-accent)` | `#f59e0b` |
| خط الكود | `var(--vp-font-family-mono)` | `'JetBrains Mono'` |
| خط النص | `var(--vp-font-family-base)` | `'Cairo'` |

**قاعدة:** يُمنع hardcoded colors. كل لون = متغير CSS.

---

## 6️⃣ تأثير على Story 3.2

### قبل المراجعة (Epics v2 الأصلي)
- 4 components جديدة من الصفر
- 0.5 يوم
- ~600 سطر كود

### بعد المراجعة (Sally توصية)
- **2 components جديدة** (`KeywordCard.vue` + `ErrorRecoveryDemo.vue`)
- **2 wrappers** (`BuiltinPlayground.vue` + `DirectiveExample.vue`)
- **0.25 يوم** (-50%)
- **~250 سطر كود** (-58%)

### تحديث AC جديد لـ Story 3.2

```yaml
ACs (مُحدَّث):
  AC-1: KeywordCard.vue يعرض ar/en/category/since/summary/example
  AC-2: BuiltinPlayground.vue يلتف حول SadCodeRunner.vue (لا CodeMirror مباشر)
  AC-3: ErrorRecoveryDemo.vue يعرض broken→fixed مع color tokens
  AC-4: DirectiveExample.vue يستخدم نمط feature-card الموجود
  AC-5: كلها تستخدم متغيرات CSS من style.css (zero hardcoded colors)
  AC-6: كلها تجاوز axe a11y (zero critical)
  AC-7: visual regression test في Playwright لكل component (4 snapshots)
```

---

## 7️⃣ تحذيرات Sally — **مُحدَّث بعد فحص SadCodeRunner.vue**

### ✅ تأكيد: SadCodeRunner.vue ناضج جداً (مفاجأة سارّة)

بعد الفحص الفعلي للملف (~350 سطر) و `composables/useSadWasm.ts`:

| الميزة الموجودة | الحالة |
|---|---|
| WASM executor محمَّل من `/sad.wasm` + `/sad.js` | ✅ موجود ومُختبر |
| CodeMirror 6 كامل (state/view/commands/language/autocomplete) | ✅ موجود |
| Fallback إلى `<textarea>` عند فشل CodeMirror | ✅ موجود (auto-fallback) |
| كشف اتجاه الكود تلقائياً (RTL/LTR حسب نسبة عربي/لاتيني) | ✅ موجود |
| Ctrl+Enter للتشغيل | ✅ موجود |
| نسخ كود + نسخ مخرجات + reset | ✅ موجود |
| تبويبات: مخرجات / خطأ / canvas (رسوميات) | ✅ موجود |
| Loading state + 15-second timeout للـ WASM | ✅ موجود |
| stdout/stderr capture من Emscripten | ✅ موجود |
| oneDark theme | ✅ موجود |

### ❌ تحذير Sally الأصلي (مُلغى):
> ~~"إن فشل `SadCodeRunner.vue` (يعتمد على wasm executor) → fallback إلى `CodePlaygroundV2.vue`"~~

**الواقع:** `SadCodeRunner.vue` لديه **fallback مدمج** بالفعل (CodeMirror → textarea)، ويتعامل مع فشل WASM برسالة عربية واضحة. لا حاجة fallback خارجي.

### ⚠️ تحذيرات Sally المتبقية (سارية):

1. **خط Cairo والكلمات العربية الطويلة** (`غير_متزامن`، `وقت_الترجمة`): يحتاج `min-width: 0; word-break: break-word` على `KeywordCard`.
2. **VitePress 1.6.3 hot reload** على JSON: يحتاج اختبار يدوي مع `npm run dev` + تعديل `_generated/keywords.json`.
3. **ألوان dark mode للأخطاء**: `rgba(239, 68, 68, 0.1)` على dark BG قد ينخفض contrast. اختبر بـ axe-playwright.
4. **سعة CSS bundle**: 4 components جديدة لا تُضيف > 5 KB CSS gzip.

---

## 8️⃣ Definition of Done (UX)

- [ ] 4 components مكتوبة بالـ tokens
- [ ] zero hardcoded colors (grep check)
- [ ] axe a11y zero critical على 4 صفحات اختبار
- [ ] Playwright visual snapshots لكل component
- [ ] tested في dark + light mode
- [ ] tested في RTL (`/`) + LTR (`/en/`)
- [ ] tested على mobile viewport (375px)
- [ ] Lighthouse score ≥ 95 على صفحة keyword

---

> **توقيع Sally:** "أفضل تصميم = استخدام الموجود. الموقع جاهز معمارياً، 60% من الـ4 components موجودة بالفعل، نحن فقط نُكسِيها سياقاً جديداً (keywords/builtins/errors/directives)."
