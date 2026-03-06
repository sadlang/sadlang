import{_ as n,o as a,c as i,am as e}from"./chunks/framework.ZxETiGz0.js";const o=JSON.parse('{"title":"مكتبة network — الشبكة","description":"","frontmatter":{"title":"مكتبة network — الشبكة"},"headers":[],"relativePath":"docs/stdlib/network.md","filePath":"docs/stdlib/network.md","lastUpdated":null}'),l={name:"docs/stdlib/network.md"};function p(t,s,r,c,E,h){return a(),i("div",null,[...s[0]||(s[0]=[e(`<h1 id="مكتبة-network" tabindex="-1">مكتبة network <a class="header-anchor" href="#مكتبة-network" aria-label="Permalink to &quot;مكتبة network&quot;">​</a></h1><div class="language-sad vp-adaptive-theme line-numbers-mode"><button title="Copy Code" class="copy"></button><span class="lang">sad</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">استورد شبكة من &quot;stdlib/network&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># طلب GET</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">متغير رد = انتظر شبكة.احصل(&quot;https://api.example.com/data&quot;)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">اطبع(رد.الحالة)     # 200</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">اطبع(رد.نص())</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># طلب POST</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">متغير رد2 = انتظر شبكة.أرسل(&quot;https://api.example.com/users&quot;، {</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  &quot;الاسم&quot;: &quot;أحمد&quot;,</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  &quot;العمر&quot;: 25</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">})</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># خادم HTTP بسيط</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">متغير خادم = جديد شبكة.خادم_http(8080)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">خادم.على(&quot;/&quot;، لامدا(طلب، رد)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  رد.أرسل(&quot;مرحباً من لغة ص!&quot;)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">نهاية)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">خادم.شغّل()</span></span></code></pre><div class="line-numbers-wrapper" aria-hidden="true"><span class="line-number">1</span><br><span class="line-number">2</span><br><span class="line-number">3</span><br><span class="line-number">4</span><br><span class="line-number">5</span><br><span class="line-number">6</span><br><span class="line-number">7</span><br><span class="line-number">8</span><br><span class="line-number">9</span><br><span class="line-number">10</span><br><span class="line-number">11</span><br><span class="line-number">12</span><br><span class="line-number">13</span><br><span class="line-number">14</span><br><span class="line-number">15</span><br><span class="line-number">16</span><br><span class="line-number">17</span><br><span class="line-number">18</span><br><span class="line-number">19</span><br></div></div>`,2)])])}const d=n(l,[["render",p]]);export{o as __pageData,d as default};
