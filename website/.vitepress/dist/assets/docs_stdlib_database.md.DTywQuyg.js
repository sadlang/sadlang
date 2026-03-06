import{_ as a,o as n,c as i,am as e}from"./chunks/framework.ZxETiGz0.js";const E=JSON.parse('{"title":"مكتبة database — قواعد البيانات","description":"","frontmatter":{"title":"مكتبة database — قواعد البيانات"},"headers":[],"relativePath":"docs/stdlib/database.md","filePath":"docs/stdlib/database.md","lastUpdated":null}'),l={name:"docs/stdlib/database.md"};function p(t,s,r,d,c,b){return n(),i("div",null,[...s[0]||(s[0]=[e(`<h1 id="مكتبة-database" tabindex="-1">مكتبة database <a class="header-anchor" href="#مكتبة-database" aria-label="Permalink to &quot;مكتبة database&quot;">​</a></h1><p>دعم SQLite3 مدمج.</p><div class="language-sad vp-adaptive-theme line-numbers-mode"><button title="Copy Code" class="copy"></button><span class="lang">sad</span><pre class="shiki shiki-themes github-light github-dark vp-code" tabindex="0"><code><span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">استورد قاعدة_بيانات من &quot;stdlib/database&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># الاتصال بـ SQLite</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">متغير ق = جديد قاعدة_بيانات.sqlite(&quot;app.db&quot;)</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># إنشاء جدول</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">ق.نفّذ(&quot;إنشاء_جدول إذا_لم_يكن_موجود مستخدمون (id رقم، اسم نص، عمر رقم)&quot;)</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># إدراج بيانات</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">ق.نفّذ(&quot;إدراج_في مستخدمون (اسم، عمر) قيم (؟، ؟)&quot;، [&quot;أحمد&quot;، 25])</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;"># استعلام</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">متغير نتائج = ق.استعلم(&quot;اختر * من مستخدمون حيث عمر &gt; 20&quot;)</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">لكل صف في نتائج</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">  اطبع_سطر(صف[&quot;اسم&quot;] + &quot; — &quot; + صف[&quot;عمر&quot;])</span></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">نهاية</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#24292E;--shiki-dark:#E1E4E8;">ق.أغلق()</span></span></code></pre><div class="line-numbers-wrapper" aria-hidden="true"><span class="line-number">1</span><br><span class="line-number">2</span><br><span class="line-number">3</span><br><span class="line-number">4</span><br><span class="line-number">5</span><br><span class="line-number">6</span><br><span class="line-number">7</span><br><span class="line-number">8</span><br><span class="line-number">9</span><br><span class="line-number">10</span><br><span class="line-number">11</span><br><span class="line-number">12</span><br><span class="line-number">13</span><br><span class="line-number">14</span><br><span class="line-number">15</span><br><span class="line-number">16</span><br><span class="line-number">17</span><br><span class="line-number">18</span><br></div></div>`,3)])])}const o=a(l,[["render",p]]);export{E as __pageData,o as default};
