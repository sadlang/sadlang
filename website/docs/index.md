---
layout: home
title: لغة ص — أول لغة برمجة عربية حديثة

hero:
  name: "لغة ص"
  text: "أول لغة برمجة عربية حديثة"
  tagline: "مفسّر · مترجم إلى كود أصلي · LSP كامل · مكتبة قياسية عربية"
  image:
    src: /sad-hero.svg
    alt: شعار لغة ص
  actions:
    - theme: brand
      text: ابدأ الآن
      link: /docs/getting-started
    - theme: alt
      text: تحميل v1.0.0
      link: /download
    - theme: alt
      text: Playground ▶
      link: /playground

features:
  - icon: ⚡
    title: مفسّر فوري
    details: شغّل ملفات .ص على الفور دون خطوات بناء. مثالي للتعلم والنمذجة السريعة.
  - icon: 🚀
    title: مترجم أصيل عبر LLVM
    details: حوّل كودك إلى ملف تنفيذي أصيل بأداء عالٍ باستخدام البنية التحتية LLVM 18.
  - icon: 🔤
    title: عربي بالكامل
    details: كلمات مفتاحية عربية، تعليقات عربية، أسماء متغيرات عربية. اكتب بلغتك الأم.
  - icon: 🧩
    title: برمجة كائنية متكاملة
    details: أصناف، وراثة، سمات (Traits)، تحميل العوامل — كل شيء بصياغة عربية سلسة.
  - icon: 🛠️
    title: أدوات متكاملة
    details: مدير حزم (sad-pkg)، منسّق كود (sad-fmt)، خادم LSP مع إكمال تلقائي.
  - icon: 🌐
    title: مكتبة قياسية واسعة
    details: io · math · string · network · graphics · database — كل ما تحتاجه جاهز.
---

<script setup>
import { ref } from 'vue'

const sampleCode = `# متغيراتنا
متغير اسم = "عالم"
متغير عمر = 25

# دالة الترحيب
دالة رحّب(شخص، سنوات)
  اطبع_سطر("مرحباً " + شخص + "! عمرك " + سنوات + " سنة.")
  إذا (سنوات >= 18)
    اطبع_سطر("أنت بالغ ✓")
  وإلا
    اطبع_سطر("أنت قاصر ✗")
  نهاية
نهاية

رحّب(اسم، عمر)
`
</script>

## لماذا لغة ص؟

<FeatureGrid :features="[
  { icon: '🇸🇦', title: 'عربية المولد', desc: 'صُصمّت من الصفر لتكون لغة برمجة عربية حقيقية، لا مجرد تعريب جزئي.' },
  { icon: '📚', title: 'سهلة التعلم', desc: 'بنية نحوية واضحة وبسيطة، مناسبة للمبتدئين والمحترفين على حد سواء.' },
  { icon: '🔒', title: 'آمنة ومتينة', desc: 'نظام أنواع قوي وإدارة أخطاء واضحة تمنع الأخطاء الشائعة.' },
  { icon: '⚙️', title: 'قابلة للتوسع', desc: 'من السكريبتات البسيطة إلى التطبيقات الكاملة والخوادم والرسوميات.' },
  { icon: '🤝', title: 'مجتمع نامٍ', desc: 'مجتمع عربي نشط يُلهم ويتعاون لبناء نظام بيئي متكامل.' },
  { icon: '📦', title: 'مفتوحة المصدر', desc: 'رخصة MIT — حرة للجميع، مفتوحة للمساهمة والتطوير.' },
]" />

## مثال سريع

```sad
# برنامج حساب الفيبوناتشي

دالة فيبوناتشي(ن)
  إذا (ن <= 1)
    ارجع ن
  نهاية
  ارجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
نهاية

# طباعة أول 10 أرقام
لكل ي في [0، 1، 2، 3، 4، 5، 6، 7، 8، 9]
  اطبع_سطر("فيبو(" + ي + ") = " + فيبوناتشي(ي))
نهاية
```

## تثبيت سريع

::: code-group

```powershell [Windows]
# تحميل وتثبيت بسطر واحد
irm https://sad-lang.org/install.ps1 | iex
```

```bash [Linux/macOS]
curl -fsSL https://sad-lang.org/install.sh | bash
```

```bash [عبر Scoop (Windows)]
scoop bucket add sad https://github.com/sad-lang/scoop-bucket
scoop install sad
```

:::

[التوثيق الكامل للتثبيت →](/download)

## الإصدار الحالي

<SadVersion />

**الإصدار 1.0.0** — أول إصدار رسمي لغة ص  
9 أبريل 2025 · [ملاحظات الإصدار](https://github.com/SalehKadah/s-programming-language/releases/tag/v1.0.0)
