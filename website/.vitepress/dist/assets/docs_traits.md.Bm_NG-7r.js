import{_ as a,o as n,c as i,am as l}from"./chunks/framework.ZxETiGz0.js";const b=JSON.parse('{"title":"السمات (Traits)","description":"السمات واجهات المعالجة في لغة ص","frontmatter":{"title":"السمات (Traits)","description":"السمات واجهات المعالجة في لغة ص"},"headers":[],"relativePath":"docs/traits.md","filePath":"docs/traits.md","lastUpdated":null}'),p={name:"docs/traits.md"};function e(r,s,t,h,E,c){return n(),i("div",null,[...s[0]||(s[0]=[l(`<h1 id="السمات-traits" tabindex="-1">السمات (Traits) <a class="header-anchor" href="#السمات-traits" aria-label="Permalink to &quot;السمات (Traits)&quot;">​</a></h1><p>السمة هي عقد يُلزم الصنف بتنفيذ مجموعة من الدوال.</p><div class="language-sad vp-adaptive-theme line-numbers-mode"><button title="Copy Code" class="copy"></button><span class="lang">sad</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">سمة قابل_للسرد</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة طول()</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة العنصر_في(فهرس)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">نهاية</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">سمة قابل_للمقارنة</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة يساوي(آخر)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة أقل_من(آخر)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">نهاية</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">صنف قائمة_مرتّبة نفّذ قابل_للسرد، قابل_للمقارنة</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  باني()</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">    هذا.البيانات = []</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  نهاية</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة أضف(عنصر)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">    هذا.البيانات.أضف(عنصر)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  نهاية</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  # تنفيذ سمة القابل للسرد</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة طول()          ارجع هذا.البيانات.طول()        نهاية</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة العنصر_في(فهرس) ارجع هذا.البيانات[فهرس]       نهاية</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  # تنفيذ سمة القابل للمقارنة</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة يساوي(آخر)  ارجع هذا.طول() == آخر.طول()     نهاية</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة أقل_من(آخر) ارجع هذا.طول() &lt; آخر.طول()      نهاية</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">نهاية</span></span></code></pre><div class="line-numbers-wrapper" aria-hidden="true"><span class="line-number">1</span><br><span class="line-number">2</span><br><span class="line-number">3</span><br><span class="line-number">4</span><br><span class="line-number">5</span><br><span class="line-number">6</span><br><span class="line-number">7</span><br><span class="line-number">8</span><br><span class="line-number">9</span><br><span class="line-number">10</span><br><span class="line-number">11</span><br><span class="line-number">12</span><br><span class="line-number">13</span><br><span class="line-number">14</span><br><span class="line-number">15</span><br><span class="line-number">16</span><br><span class="line-number">17</span><br><span class="line-number">18</span><br><span class="line-number">19</span><br><span class="line-number">20</span><br><span class="line-number">21</span><br><span class="line-number">22</span><br><span class="line-number">23</span><br><span class="line-number">24</span><br><span class="line-number">25</span><br><span class="line-number">26</span><br><span class="line-number">27</span><br></div></div><h2 id="السمات-مع-تنفيذ-افتراضي" tabindex="-1">السمات مع تنفيذ افتراضي <a class="header-anchor" href="#السمات-مع-تنفيذ-افتراضي" aria-label="Permalink to &quot;السمات مع تنفيذ افتراضي&quot;">​</a></h2><div class="language-sad vp-adaptive-theme line-numbers-mode"><button title="Copy Code" class="copy"></button><span class="lang">sad</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">سمة مسجّل</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة احصل_على_الاسم()   # مجردة، يجب تنفيذها</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  دالة سجّل(رسالة)        # لها تنفيذ افتراضي</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">    اطبع_سطر(&quot;[&quot; + هذا.احصل_على_الاسم() + &quot;] &quot; + رسالة)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  نهاية</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">نهاية</span></span></code></pre><div class="line-numbers-wrapper" aria-hidden="true"><span class="line-number">1</span><br><span class="line-number">2</span><br><span class="line-number">3</span><br><span class="line-number">4</span><br><span class="line-number">5</span><br><span class="line-number">6</span><br><span class="line-number">7</span><br></div></div><hr><p>السابق: <a href="/docs/inheritance">الوراثة ←</a></p>`,7)])])}const d=a(p,[["render",e]]);export{b as __pageData,d as default};
