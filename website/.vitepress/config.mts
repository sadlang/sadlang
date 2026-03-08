// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════
// VitePress Configuration — sad-lang.org
// ═══════════════════════════════════════════════════════════════
import { defineConfig } from 'vitepress'

export default defineConfig({
  // ── البيانات الأساسية
  title:       'لغة ص',
  titleTemplate: '%s — لغة ص البرمجية',
  description: 'لغة برمجة عربية حديثة — تدعم البرمجة الكائنية والمتزامنة والتحويل إلى كود أصلي',
  lang:        'ar',
  base:        '/',

  // ── Head (meta tags, icons, fonts)
  head: [
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/sad-icon.svg' }],
    ['link', { rel: 'icon', type: 'image/png',     href: '/sad-icon.png' }],
    ['meta', { name: 'theme-color',    content: '#0070f3' }],
    ['meta', { name: 'og:type',        content: 'website' }],
    ['meta', { name: 'og:site_name',   content: 'لغة ص' }],
    ['meta', { name: 'og:image',       content: 'https://sad-lang.org/og-image.png' }],
    ['meta', { name: 'twitter:card',   content: 'summary_large_image' }],
    ['meta', { name: 'twitter:site',   content: '@sadlang_official' }],
    // خط عربي
    ['link', {
      rel:  'preconnect',
      href: 'https://fonts.googleapis.com'
    }],
    ['link', {
      rel:  'stylesheet',
      href: 'https://fonts.googleapis.com/css2?family=Cairo:wght@300;400;600;700;900&family=JetBrains+Mono:wght@400;600&display=swap'
    }],
    // ── Umami Analytics (استبدل data-website-id بقيمتك من umami.is)
    ['script', {
      defer:           'true',
      src:             'https://analytics.umami.is/script.js',
      'data-website-id': 'YOUR_UMAMI_WEBSITE_ID',
    }],
  ],

  // ── ملفات Markdown
  srcDir:   './docs',
  outDir:   './.vitepress/dist',
  cacheDir: './.vitepress/cache',

  // ── معالجة Markdown
  markdown: {
    theme: {
      light: 'github-light',
      dark:  'github-dark',
    },
    lineNumbers: true,
    toc:         { level: [2, 3] },
    math:        false,
    // تعريف لغة ص كـ Shiki grammar مبسّط
    languages: [
      {
        name:     'sad',
        aliases:  ['ص'],
        scopeName: 'source.sad',
        grammar: {
          scopeName: 'source.sad',
          patterns: [
            // تعليقات
            { name: 'comment.line.sad',  match: '#.*$' },
            { name: 'comment.block.sad', begin: '#\\*', end: '\\*#' },
            // كلمات مفتاحية
            {
              name: 'keyword.control.sad',
              match: '\\b(دالة|ارجع|إذا|اذا|وإلا|والا|بينما|لكل|في|توقف|استمر|طابق|عندما|افتراضي|حاول|امسك|ارمي|أخيراً|اخيرا|صنف|بنية|يرث|جديد|هذا|باني|الأساس|متغير|ثابت|ساكن|عام|خاص|محمي|مجرد|استورد|من|كـ|صدّر|صدر|لامدا|غير_متزامن|انتظر|أنتج|انتج|مولد|نفّذ|نفذ|سمة|واجهة|صحيح|خطأ|خطا|لاشيء)\\b',
            },
            // نصوص
            { name: 'string.quoted.double.sad', begin: '"', end: '"', patterns: [{ name: 'constant.character.escape.sad', match: '\\\\.' }] },
            { name: 'string.quoted.single.sad', begin: "'", end: "'" },
            // أرقام
            { name: 'constant.numeric.sad', match: '\\b\\d+(\\.\\d+)?\\b' },
            // دوال المدمجة
            { name: 'support.function.sad', match: '\\b(اطبع|اطبع_سطر|مدى|طول|أضف|أزل|حجم)\\b' },
          ],
        },
      },
    ],
  },

  // ── VitePress Appearance
  appearance:    'dark',
  lastUpdated:   true,
  cleanUrls:     true,

  // ══════════════════════════════════════════════════════════════
  // إعدادات السمة
  // ══════════════════════════════════════════════════════════════
  themeConfig: {
    // ── الشعار والعنوان
    logo:      '/sad-icon.svg',
    siteTitle: 'لغة ص',

    // ── شريط التنقل العلوي
    nav: [
      { text: 'الرئيسية',  link: '/' },
      { text: 'التوثيق',   link: '/docs/getting-started' },
      { text: 'المسار التعليمي', link: '/learn' },
      { text: 'الدروس',    link: '/tutorials' },
      { text: 'Playground', link: '/playground' },
      { text: 'التحميل',   link: '/download' },
      {
        text: 'المجتمع',
        items: [
          { text: 'GitHub',    link: 'https://github.com/SalehKadah/s-programming-language/tree/graphic' },
          { text: 'المنتدى',   link: 'https://github.com/SalehKadah/s-programming-language/discussions' },
          { text: 'Telegram',  link: 'https://t.me/s_prog_lang' },
          { text: 'المدونة',   link: '/blog' },
          { text: 'مقارنة اللغات', link: '/compare' },
        ]
      },
    ],

    // ── الشريط الجانبي
    sidebar: {
      '/docs/': [
        {
          text:  'البداية',
          items: [
            { text: 'مقدمة',         link: '/docs/introduction' },
            { text: 'التثبيت',        link: '/docs/getting-started' },
            { text: 'أول برنامج',    link: '/docs/first-program' },
          ],
        },
        {
          text:  'أساسيات اللغة',
          items: [
            { text: 'المتغيرات والأنواع', link: '/docs/variables' },
            { text: 'الدوال',            link: '/docs/functions' },
            { text: 'التحكم في التدفق',  link: '/docs/control-flow' },
            { text: 'الأنماط والمطابقة', link: '/docs/pattern-matching' },
          ],
        },
        {
          text:  'البرمجة الكائنية',
          items: [
            { text: 'الصنف والبنية',  link: '/docs/classes' },
            { text: 'الوراثة',        link: '/docs/inheritance' },
            { text: 'السمات',         link: '/docs/traits' },
          ],
        },
        {
          text:  'ميزات متقدمة',
          items: [
            { text: 'async/await',  link: '/docs/async' },
            { text: 'إدارة الأخطاء', link: '/docs/errors' },
            { text: 'المولدات',     link: '/docs/generators' },
          ],
        },
        {
          text:  'المكتبة القياسية',
          items: [
            { text: 'io — الإدخال/الإخراج',    link: '/docs/stdlib/io' },
            { text: 'math — الرياضيات',         link: '/docs/stdlib/math' },
            { text: 'string — النصوص',          link: '/docs/stdlib/string' },
            { text: 'network — الشبكة',         link: '/docs/stdlib/network' },
            { text: 'graphics — الرسوميات',     link: '/docs/stdlib/graphics' },
            { text: 'database — قواعد البيانات', link: '/docs/stdlib/database' },
          ],
        },
        {
          text:  'الأدوات',
          items: [
            { text: 'sad-pkg — مدير الحزم',  link: '/docs/tools/pkg' },
            { text: 'sad-fmt — المنسّق',      link: '/docs/tools/fmt' },
            { text: 'sadc — المترجم',         link: '/docs/tools/compiler' },
            { text: 'خادم LSP',               link: '/docs/tools/lsp' },
          ],
        },
        {
          text: 'الكتاب — الأساسيات',
          collapsed: true,
          items: [
            { text: '1. المتغيرات والأنواع', link: '/docs/book/01-variables-types' },
            { text: '2. العوامل والتعبيرات', link: '/docs/book/02-operators-expressions' },
            { text: '3. التحكم في التدفق', link: '/docs/book/03-control-flow' },
            { text: '4. الدوال', link: '/docs/book/04-functions' },
            { text: '5. المصفوفات والخرائط', link: '/docs/book/05-arrays-maps' },
            { text: '6. البرمجة الكائنية', link: '/docs/book/06-oop-basics' },
            { text: '7. معالجة الأخطاء', link: '/docs/book/07-error-handling' },
            { text: '8. الوحدات والاستيراد', link: '/docs/book/08-modules' },
            { text: '9. النصوص والمعالجة', link: '/docs/book/09-strings' },
            { text: '10. المكتبة القياسية', link: '/docs/book/10-stdlib' },
          ],
        },
        {
          text: 'الكتاب — المتوسط',
          collapsed: true,
          items: [
            { text: '11. OOP المتقدمة', link: '/docs/book/11-oop-advanced' },
            { text: '12. البرمجة غير المتزامنة', link: '/docs/book/12-async-await' },
            { text: '13. الوحدات المتقدمة', link: '/docs/book/13-modules-advanced' },
            { text: '14. القوالب والمفاهيم', link: '/docs/book/14-generics' },
            { text: '15. مشاريع عملية', link: '/docs/book/15-projects-practice' },
            { text: '16. مشروع تخرج', link: '/docs/book/16-capstone-roadmap' },
          ],
        },
        {
          text: 'الكتاب — المتقدم',
          collapsed: true,
          items: [
            { text: '17. أنماط التصميم', link: '/docs/book/17-design-patterns' },
            { text: '18. البرمجة الوظيفية', link: '/docs/book/18-functional-programming' },
            { text: '19. المولدات والتكرار', link: '/docs/book/19-generators-iterators' },
            { text: '20. البرمجة الدفاعية', link: '/docs/book/20-defensive-programming' },
            { text: '21. هياكل البيانات', link: '/docs/book/21-data-structures' },
            { text: '22. الخوارزميات', link: '/docs/book/22-algorithms' },
            { text: '23. مطابقة الأنماط المتقدمة', link: '/docs/book/23-pattern-matching-advanced' },
            { text: '24. إدارة الملفات', link: '/docs/book/24-file-io' },
            { text: '25. الشبكات', link: '/docs/book/25-networking' },
            { text: '26. قواعد البيانات', link: '/docs/book/26-database' },
            { text: '27. واجهات المستخدم', link: '/docs/book/27-gui' },
            { text: '28. رسوميات ثنائية البعد', link: '/docs/book/28-2d-graphics' },
            { text: '29. تطوير الويب', link: '/docs/book/29-web-development' },
            { text: '30. اختبار البرمجيات', link: '/docs/book/30-testing' },
          ],
        },
        {
          text: 'الكتاب — الاحترافي',
          collapsed: true,
          items: [
            { text: '31. تصحيح الأخطاء', link: '/docs/book/31-debugging' },
            { text: '32. الأداء والتحسين', link: '/docs/book/32-performance' },
            { text: '33. البرمجة المتزامنة', link: '/docs/book/33-concurrency' },
            { text: '34. أمان البرمجيات', link: '/docs/book/34-security' },
            { text: '35. بنية المشاريع', link: '/docs/book/35-architecture' },
            { text: '36. إدارة الحزم', link: '/docs/book/36-package-management' },
            { text: '37. أدوات التطوير', link: '/docs/book/37-developer-tools' },
            { text: '38. المترجم sadc', link: '/docs/book/38-compiler' },
            { text: '39. البرمجة منخفضة المستوى', link: '/docs/book/39-low-level' },
            { text: '40. تطوير تطبيقات الهاتف', link: '/docs/book/40-mobile' },
            { text: '41. التوثيق والتعليق', link: '/docs/book/41-documentation' },
            { text: '42. المصدر المفتوح', link: '/docs/book/42-open-source' },
            { text: '43. بناء مكتبة خاصة', link: '/docs/book/43-building-library' },
            { text: '44. مشاريع متقدمة', link: '/docs/book/44-advanced-projects' },
            { text: '45. الذكاء الاصطناعي', link: '/docs/book/45-ai-ml' },
            { text: '46. مستقبل لغة ص', link: '/docs/book/46-future' },
          ],
        },
      ],
    },

    // ── روابط اجتماعية
    socialLinks: [
      { icon: 'github',   link: 'https://github.com/SalehKadah/s-programming-language/tree/graphic' },
      { 
        icon: { svg: '<svg role="img" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><title>Telegram</title><path d="M11.944 0A12 12 0 0 0 0 12a12 12 0 0 0 12 12 12 12 0 0 0 12-12A12 12 0 0 0 12 0a12 12 0 0 0-.056 0zm4.962 7.224c.1-.002.321.023.465.14a.506.506 0 0 1 .171.325c.016.093.036.306.02.472-.18 1.898-.962 6.502-1.36 8.627-.168.9-.499 1.201-.82 1.23-.696.065-1.225-.46-1.9-.895-1.056-.693-1.653-1.124-2.678-1.8-1.185-.78-.417-1.21.258-1.91.177-.184 3.247-2.977 3.307-3.23.007-.032.014-.15-.056-.212s-.174-.041-.249-.024c-.106.024-1.793 1.14-5.061 3.345-.48.33-.913.49-1.302.48-.428-.008-1.252-.241-1.865-.44-.752-.245-1.349-.374-1.297-.789.027-.216.325-.437.893-.663 3.498-1.524 5.83-2.529 6.998-3.014 3.332-1.386 4.025-1.627 4.476-1.635z"/></svg>' }, 
        link: 'https://t.me/s_prog_lang' 
      },
    ],

    // ── بحث Algolia DocSearch
    // لتفعيل Algolia: اطلب فهرسة مجانية على https://docsearch.algolia.com
    // ثم استبدل القيم التالية بقيمك الحقيقية
    search: {
      provider: 'algolia',
      options: {
        appId:     'YOUR_ALGOLIA_APP_ID',
        apiKey:    'YOUR_ALGOLIA_SEARCH_KEY',
        indexName: 'sad-lang',
        locales: {
          root: {
            placeholder: 'ابحث في التوثيق...',
            translations: {
              button: {
                buttonText:        'بحث',
                buttonAriaLabel:   'بحث',
              },
              modal: {
                searchBox: {
                  resetButtonTitle: 'مسح البحث',
                  resetButtonAriaLabel: 'مسح البحث',
                  cancelButtonText: 'إلغاء',
                  cancelButtonAriaLabel: 'إلغاء',
                },
                startScreen: {
                  recentSearchesTitle: 'عمليات بحث أخيرة',
                  noRecentSearchesText: 'لا توجد عمليات بحث أخيرة',
                  saveRecentSearchButtonTitle: 'حفظ',
                  removeRecentSearchButtonTitle: 'حذف',
                  favoriteSearchesTitle: 'المفضلة',
                  removeFavoriteSearchButtonTitle: 'إزالة من المفضلة',
                },
                errorScreen: {
                  titleText: 'لا يمكن جلب النتائج',
                  helpText: 'تحقق من اتصالك بالإنترنت',
                },
                footer: {
                  selectText:       'اختر',
                  navigateText:     'تنقل',
                  closeText:        'إغلاق',
                  searchByText:     'البحث بواسطة',
                },
                noResultsScreen: {
                  noResultsText: 'لا توجد نتائج لـ',
                  suggestedQueryText: 'جرّب البحث عن',
                  reportMissingResultsText: 'أرسل ملاحظة',
                  reportMissingResultsLinkText: 'إرسال',
                },
              },
            },
          },
          en: {
            placeholder: 'Search docs...',
            translations: { button: { buttonText: 'Search' } },
          },
        },
      },
    },

    // ── أزرار التحرير
    editLink: {
      pattern: 'https://github.com/sad-lang/sad-language/edit/main/website/docs/:path',
      text:    'تحرير هذه الصفحة على GitHub',
    },

    // ── التنقل في الصفحات
    docFooter: {
      prev: 'الصفحة السابقة',
      next: 'الصفحة التالية',
    },

    // ── التذييل
    footer: {
      message:   'مُرخَّص بموجب رخصة MIT',
      copyright: 'حقوق محفوظة © 2025-2026 فريق لغة ص',
    },

    // ── نص جانب الخريطة
    outline: {
      label: 'في هذه الصفحة',
      level: [2, 3],
    },

    // ── أزرار الإجراءات
    lastUpdated: {
      text:       'آخر تحديث',
      formatOptions: {
        dateStyle: 'long',
        timeStyle: 'short',
      },
    },

    // ── شريط Dark Mode
    darkModeSwitchLabel:  'المظهر',
    lightModeSwitchTitle: 'تبديل للمضيء',
    darkModeSwitchTitle:  'تبديل للمُعتم',
    sidebarMenuLabel:     'القائمة',
    returnToTopLabel:     'العودة للأعلى',
    langMenuLabel:        'تغيير اللغة',
  },

  // ══════════════════════════════════════════════════════════════
  // دعم متعدد اللغات (i18n) — العربية (root) + الإنجليزية (/en/)
  // ══════════════════════════════════════════════════════════════
  locales: {
    root: {
      label:    'العربية',
      lang:     'ar',
      dir:      'rtl',
      title:    'لغة ص',
      description: 'لغة برمجة عربية حديثة — مفسر · مترجم · LSP',
    },
    en: {
      label:    'English',
      lang:     'en',
      dir:      'ltr',
      link:     '/en/',
      title:    'Sad Language',
      description: 'A modern Arabic programming language — interpreter · compiler · LSP',
      themeConfig: {
        nav: [
          { text: 'Home',       link: '/en/' },
          { text: 'Docs',       link: '/en/docs/introduction' },
          { text: 'Playground', link: '/playground' },
          { text: 'Download',   link: '/en/download' },
          {
            text: 'Community',
            items: [
              { text: 'GitHub',    link: 'https://github.com/sad-lang/sad-language' },
              { text: 'Discord',   link: 'https://discord.gg/sadlang' },
              { text: 'Blog',      link: '/blog' },
            ],
          },
        ],
        sidebar: {
          '/en/docs/': [
            {
              text: 'Getting Started',
              items: [
                { text: 'Introduction',    link: '/en/docs/introduction' },
                { text: 'Installation',    link: '/en/docs/getting-started' },
                { text: 'First Program',   link: '/en/docs/first-program' },
              ],
            },
            {
              text: 'Language Basics',
              items: [
                { text: 'Variables & Types', link: '/en/docs/variables' },
                { text: 'Functions',          link: '/en/docs/functions' },
                { text: 'Control Flow',       link: '/en/docs/control-flow' },
              ],
            },
            {
              text: 'OOP',
              items: [
                { text: 'Classes',     link: '/en/docs/classes' },
                { text: 'Inheritance', link: '/en/docs/inheritance' },
                { text: 'Traits',      link: '/en/docs/traits' },
              ],
            },
          ],
        },
        editLink: {
          pattern: 'https://github.com/sad-lang/sad-language/edit/main/website/docs/:path',
          text:    'Edit this page on GitHub',
        },
        docFooter:    { prev: 'Previous Page', next: 'Next Page' },
        footer:       { message: 'Released under MIT License', copyright: 'Copyright © 2025-2026 Sad Language Team' },
        outline:      { label: 'On This Page', level: [2, 3] },
        lastUpdated:  { text: 'Last Updated' },
        darkModeSwitchLabel:  'Appearance',
        sidebarMenuLabel:     'Menu',
        returnToTopLabel:     'Return to Top',
        langMenuLabel:        'Change Language',
      },
    },
  },

  // ── Vite options
  vite: {
    define: {
      __SAD_VERSION__:     JSON.stringify('1.0.0'),
      __SAD_RELEASE_DATE__: JSON.stringify('2026-03-04'),
    },
    build: {
      chunkSizeWarningLimit: 2048,
    },
  },
})
