---
title: مدونة لغة ص
description: آخر أخبار وتحديثات لغة ص — مقالات تقنية وأخبار المجتمع
---

# مدونة لغة ص

آخر المقالات التقنية والتحديثات وأخبار المجتمع.

---

<ul class="blog-list">

<li>
<a class="post-title" href="/blog/v1-release">
  🎉 الإعلان عن الإصدار الرسمي الأول — لغة ص v1.0.0
</a>
<p class="post-meta">9 أبريل 2025 · فريق لغة ص</p>
<p class="post-summary">
  يسعدنا الإعلان عن الإصدار الرسمي الأول للغة ص — أول لغة برمجة عربية حديثة
  تدعم المفسّر والمترجم الأصيل وخادم LSP...
</p>
</li>

<li>
<a class="post-title" href="/blog/llvm-compiler">
  🚀 كيف بنينا مترجم لغة ص باستخدام LLVM 18
</a>
<p class="post-meta">15 مارس 2025 · قسم الهندسة</p>
<p class="post-summary">
  رحلة بناء المترجم sadc من AST إلى SIR ثم LLVM IR ثم ملف تنفيذي أصيل،
  والتحديات التي واجهناها في دعم النصوص العربية RTL...
</p>
</li>

<li>
<a class="post-title" href="/blog/arabic-programming">
  🌍 لماذا تحتاج البرمجة إلى لغة عربية؟
</a>
<p class="post-meta">1 فبراير 2025 · مقال رأي</p>
<p class="post-summary">
  تحليل مفصّل حول أثر اللغة على التفكير البرمجي، وكيف أن البرمجة بلغتك الأم
  تُحسّن القدرة الإبداعية وتُسرّع التعلم...
</p>
</li>

<li>
<a class="post-title" href="/blog/lsp-arabic">
  🛠️ بناء خادم LSP يدعم العربية
</a>
<p class="post-meta">20 ديسمبر 2024 · قسم الأدوات</p>
<p class="post-summary">
  دليل تقني لبناء sad-lsp — خادم بروتوكول اللغة الذي يوفر إكمالاً تلقائياً
  وتشخيصاً للأخطاء في الكود العربي مع الأخذ بعين الاعتبار خصائص RTL...
</p>
</li>

<li>
<a class="post-title" href="/blog/stdlib-design">
  📦 تصميم مكتبة قياسية عربية
</a>
<p class="post-meta">5 نوفمبر 2024 · هندسة المكتبات</p>
<p class="post-summary">
  كيف صمّمنا المكتبة القياسية للغة ص — من core وio وmath إلى network وgraphics،
  مع الحفاظ على اتساق المصطلحات العربية...
</p>
</li>

</ul>

---

## اشترك في النشرة الإخبارية

احصل على آخر تحديثات لغة ص مباشرة في بريدك.

<div class="newsletter">
  <input type="email" placeholder="بريدك الإلكتروني" class="email-input" />
  <button class="subscribe-btn">اشترك</button>
</div>

---

## ساهم بمقال

هل لديك تجربة أو مقال تريد مشاركته مع مجتمع لغة ص؟

[افتح Pull Request في GitHub ←](https://github.com/sad-lang/sad-language/tree/main/website/docs/blog)

<style scoped>
.newsletter {
  display:   flex;
  gap:       8px;
  max-width: 400px;
  margin:    16px 0;
}
.email-input {
  flex:          1;
  padding:       10px 16px;
  border:        1px solid var(--vp-c-border);
  border-radius: 8px;
  background:    var(--vp-c-bg-soft);
  font-size:     0.95rem;
  direction:     ltr;
}
.subscribe-btn {
  padding:       10px 20px;
  background:    var(--vp-c-brand-1);
  color:         #fff;
  border:        none;
  border-radius: 8px;
  font-weight:   700;
  cursor:        pointer;
}
.subscribe-btn:hover { background: var(--vp-c-brand-2); }
</style>
