# خطة موقع لغة ص v2.0

## نظرة عامة

إعادة تصميم كاملة لموقع sad-lang.org مع تجربة مستخدم محسّنة ومحتوى تعليمي موسّع.

## الهيكل الجديد

```
sad-lang.org/
├── / (الصفحة الرئيسية)
├── /docs/ (التوثيق)
│   ├── /quickstart/
│   ├── /reference/
│   ├── /api/
│   └── /tutorials/
├── /learn/ (التعلم)
│   ├── /beginners/
│   ├── /intermediate/
│   ├── /advanced/
│   └── /courses/
├── /playground/ (ملعب تفاعلي)
├── /download/ (التحميل)
├── /community/ (المجتمع)
│   ├── /discord/
│   ├── /forum/
│   └── /showcase/
├── /blog/ (المدونة)
├── /en/ (English version)
└── /about/ (عن المشروع)
```

## الصفحة الرئيسية الجديدة

### Hero Section
```html
&lt;section class="hero rtl"&gt;
  &lt;h1&gt;لغة ص&lt;/h1&gt;
  &lt;p class="tagline"&gt;البرمجة بلغتك الأم&lt;/p&gt;
  &lt;div class="cta-buttons"&gt;
    &lt;a href="/docs/quickstart" class="btn primary"&gt;ابدأ الآن&lt;/a&gt;
    &lt;a href="/playground" class="btn secondary"&gt;جرّب مباشرة&lt;/a&gt;
  &lt;/div&gt;
&lt;/section&gt;
```

### Code Example (Animated)
```html
&lt;section class="code-example"&gt;
  &lt;pre&gt;&lt;code class="language-sad"&gt;
دالة مرحبا(الاسم)
    ارجع "مرحباً، " + الاسم + "!"
نهاية

متغير رسالة = مرحبا("العالم")
اطبع_سطر(رسالة)
  &lt;/code&gt;&lt;/pre&gt;
  &lt;div class="output"&gt;
    &lt;span class="output-label"&gt;المخرجات:&lt;/span&gt;
    &lt;span class="output-text"&gt;مرحباً، العالم!&lt;/span&gt;
  &lt;/div&gt;
&lt;/section&gt;
```

### Features Grid
| الميزة | الوصف | الأيقونة |
|--------|-------|----------|
| عربية 100% | كلمات مفتاحية ورسائل خطأ | 🌐 |
| سريعة | مترجم LLVM | ⚡ |
| سهلة | للمبتدئين والمحترفين | 📚 |
| متعددة المنصات | Windows/macOS/Linux/Web | 💻 |

## الملعب التفاعلي (Playground)

### المتطلبات
- محرر Monaco مع دعم اللغة العربية
- WebAssembly backend للتنفيذ الفوري
- مشاركة الكود via URL
- قوالب جاهزة

### الواجهة
```
┌─────────────────────────────────────────────────┐
│ [قوالب ▼] [مشاركة] [تنفيذ ▶]    [وضع ليلي 🌙] │
├─────────────────────────────────────────────────┤
│                    │                            │
│   محرر الكود      │     المخرجات               │
│   (Monaco RTL)    │     (Console)              │
│                    │                            │
│                    │                            │
├─────────────────────────────────────────────────┤
│ [الأمثلة الشائعة] [حفظ] [تحميل ملف]            │
└─────────────────────────────────────────────────┘
```

## صفحة التحميل

### Detection Script
```javascript
function detectOS() {
  const ua = navigator.userAgent;
  if (ua.includes('Windows')) return 'windows';
  if (ua.includes('Mac')) return 'macos';
  if (ua.includes('Linux')) return 'linux';
  return 'other';
}

// عرض زر التحميل المناسب تلقائياً
const os = detectOS();
document.querySelector('.download-primary').href = downloads[os];
```

### Download Cards
```html
&lt;div class="download-cards"&gt;
  &lt;div class="card" data-os="windows"&gt;
    &lt;img src="/icons/windows.svg" alt="Windows"&gt;
    &lt;h3&gt;Windows&lt;/h3&gt;
    &lt;a href="/download/sad-1.0.0-win64.msi"&gt;تحميل MSI&lt;/a&gt;
    &lt;a href="/download/sad-1.0.0-win64.zip"&gt;تحميل ZIP&lt;/a&gt;
  &lt;/div&gt;
  &lt;div class="card" data-os="macos"&gt;
    &lt;img src="/icons/apple.svg" alt="macOS"&gt;
    &lt;h3&gt;macOS&lt;/h3&gt;
    &lt;code&gt;brew install sad-lang/tap/sad&lt;/code&gt;
  &lt;/div&gt;
  &lt;div class="card" data-os="linux"&gt;
    &lt;img src="/icons/linux.svg" alt="Linux"&gt;
    &lt;h3&gt;Linux&lt;/h3&gt;
    &lt;code&gt;curl -fsSL https://sad-lang.org/install.sh | sh&lt;/code&gt;
  &lt;/div&gt;
&lt;/div&gt;
```

## التوثيق التفاعلي

### مميزات جديدة
- [ ] بحث فوري (Algolia DocSearch)
- [ ] أمثلة قابلة للتنفيذ مباشرة
- [ ] إصدارات متعددة
- [ ] وضع PDF للطباعة
- [ ] تعليقات المجتمع

### هيكل التوثيق
```yaml
docs:
  - title: البداية السريعة
    items:
      - التثبيت
      - أول برنامج
      - إعداد المحرر
  
  - title: أساسيات اللغة
    items:
      - المتغيرات والثوابت
      - أنواع البيانات
      - العوامل
      - التحكم في التدفق
      - الدوال
  
  - title: البرمجة الكائنية
    items:
      - الأصناف
      - الوراثة
      - السمات والواجهات
  
  - title: المواضيع المتقدمة
    items:
      - البرمجة غير المتزامنة
      - معالجة الأخطاء
      - مطابقة الأنماط
      - المولدات
```

## صفحة التعلم

### مسارات التعلم
1. **المبتدئ الكامل** (0 → مبرمج)
   - 20 درس
   - 4 أسابيع
   - مشاريع: آلة حاسبة، لعبة تخمين

2. **المبرمج المتحول** (Python/JS → ص)
   - 10 دروس
   - أسبوع واحد
   - مقارنات مع لغات أخرى

3. **المتقدم** (→ مساهم)
   - بنية المترجم
   - كتابة مكتبات
   - المساهمة في المشروع

## التقنيات المستخدمة

### Frontend
- **Framework**: Astro 4.0 + React
- **Styling**: Tailwind CSS + Arabic fonts
- **Editor**: Monaco Editor
- **Search**: Algolia DocSearch
- **Analytics**: Plausible (privacy-friendly)

### Backend
- **Static**: Cloudflare Pages
- **API**: Cloudflare Workers
- **Playground**: WebAssembly (sad-wasm)

### CI/CD
```yaml
name: Deploy Website
on:
  push:
    branches: [main]
    paths:
      - 'website/**'

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: pnpm/action-setup@v2
      - run: pnpm install
      - run: pnpm build
      - uses: cloudflare/pages-action@v1
```

## الجدول الزمني

| المرحلة | المدة | المهام |
|---------|-------|--------|
| التصميم | أسبوعان | Figma mockups، user testing |
| التطوير | 4 أسابيع | Frontend، Playground، Docs |
| المحتوى | أسبوعان | التوثيق، الدروس، المدونة |
| الاختبار | أسبوع | QA، performance، a11y |
| الإطلاق | - | DNS migration، monitoring |

## الفريق

- **ريم** - المحتوى والتوثيق
- **مصمم UI** - التصميم (خارجي)
- **عمر** - تكامل Playground
- **كامل** - WebAssembly backend
