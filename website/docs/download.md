---
title: تحميل لغة ص
description: حمّل مثبّت لغة ص لنظام Windows
---

<script setup>
import { ref, onMounted } from 'vue'

const copied = ref('')

function copy(text, key) {
  navigator.clipboard.writeText(text)
  copied.value = key
  setTimeout(() => { copied.value = '' }, 1800)
}

const sha256setup = '9DDCBFD4B6D7E6E8F309AB894408AB00F5EDE0E9EC08D5D3F1E8558BCAD90B8A'
const sha256zip = '0A803F2ED02753C9A7D18B8AA790A960A857E14EF0C0BF87BB94D77918F1BB96'
const sha256linuxtar = '851DCD17D3C5F002CE875BA93850AD3F69CC40D900CC7D783FD090F04A310663'
const sha256linuxinst = '84BFBA971924FA484ED7F00ED271FDE1BFB9C1794B145326DA7ABBC182A959D8'
const sha256deb = '7DC809217DD0EC7888B638FB84C66F6A86C49A5D4B7F266C65FC8FFC05BE1748'
</script>

# تحميل لغة ص

<SadVersion />

---

## ⬇️ تحميل مباشر سريع

<div class="dl-hero">
  <div class="dl-hero-os windows">
    <div class="dl-os-icon">🪟</div>
    <div class="dl-os-info">
      <div class="dl-os-name">Windows x64</div>
      <div class="dl-os-meta">الإصدار 1.0.0 · 41.4 MB · مثبّت GUI رسمي</div>
    </div>
    <a
      class="dl-btn primary"
      href="https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe"
      download
    >⬇️ تحميل sad-setup.exe</a>
  </div>

  <div class="dl-hero-os linux">
    <div class="dl-os-icon">🐧</div>
    <div class="dl-os-info">
      <div class="dl-os-name">Linux x86_64</div>
      <div class="dl-os-meta">الإصدار 1.0.0 · 6.2 MB · مثبّت رسومي تفاعلي</div>
    </div>
    <a
      class="dl-btn primary"
      href="https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-installer-1.0.0-linux-x64.sh"
      download
    >⬇️ تحميل المثبّت</a>
  </div>

  <div class="dl-hero-os macos coming-soon">
    <div class="dl-os-icon">🍎</div>
    <div class="dl-os-info">
      <div class="dl-os-name">macOS Universal</div>
      <div class="dl-os-meta">قريباً — النسخة قيد التحضير</div>
    </div>
    <span class="dl-btn disabled">🔜 قريباً</span>
  </div>
</div>

::: warning ملاحظة بخصوص macOS
حالياً لغة ص متوفرة على **Windows و Linux**. نسخة macOS قيد التحضير وستُنشر قريباً.
تابع [صفحة الإصدارات على GitHub](https://github.com/SalehKadah/s-programming-language/releases) للاطلاع على آخر المستجدات.
:::

---

## 🔐 التحقق من الملفات (SHA256)

<div class="sha-block">
  <div class="sha-row">
    <span class="sha-label">المثبّت Windows (EXE):</span>
    <code class="sha-val">{{ sha256setup }}</code>
    <button class="copy-btn" @click="copy(sha256setup, 'sha-setup')">
      {{ copied === 'sha-setup' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
  <div class="sha-row" style="margin-top: 8px;">
    <span class="sha-label">النسخة المحمولة Windows (ZIP):</span>
    <code class="sha-val">{{ sha256zip }}</code>
    <button class="copy-btn" @click="copy(sha256zip, 'sha-zip')">
      {{ copied === 'sha-zip' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
  <div class="sha-row" style="margin-top: 8px;">
    <span class="sha-label">المثبّت Linux (SH):</span>
    <code class="sha-val">{{ sha256linuxinst }}</code>
    <button class="copy-btn" @click="copy(sha256linuxinst, 'sha-linst')">
      {{ copied === 'sha-linst' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
  <div class="sha-row" style="margin-top: 8px;">
    <span class="sha-label">Linux TAR.GZ:</span>
    <code class="sha-val">{{ sha256linuxtar }}</code>
    <button class="copy-btn" @click="copy(sha256linuxtar, 'sha-ltar')">
      {{ copied === 'sha-ltar' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
  <div class="sha-row" style="margin-top: 8px;">
    <span class="sha-label">حزمة DEB:</span>
    <code class="sha-val">{{ sha256deb }}</code>
    <button class="copy-btn" @click="copy(sha256deb, 'sha-deb')">
      {{ copied === 'sha-deb' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
</div>

```powershell
# تحقق من SHA256 في PowerShell
Get-FileHash .\sad-setup-1.0.0-windows-x64.exe -Algorithm SHA256
```

---

## ⚡ تثبيت سريع عبر الأوامر

```powershell
# تثبيت بسطر واحد (PowerShell — Windows)
irm https://sad-lang.org/install.ps1 | iex
```

```bash
# تثبيت بسطر واحد (Bash — Linux)
curl -fsSL https://sad-lang.org/install.sh | bash
```

```bash
# تثبيت من حزمة DEB (Debian/Ubuntu)
sudo dpkg -i sad-lang_1.0.0_amd64.deb
```

---

## 🗃️ جميع الإصدارات

<div class="download-grid">

<DownloadCard
  icon="🪟"
  platform="Windows"
  version="1.0.0"
  arch="x64"
  :files="[
    { name: 'sad-setup-1.0.0-windows-x64.exe', label: 'مثبّت GUI ✅ (موصى به)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe', size: '41.4 MB' },
    { name: 'sad-1.0.0-windows-x64.zip', label: 'نسخة محمولة (ZIP)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-windows-x64.zip', size: '68.8 MB' },
  ]"
  sha256="9DDCBFD4B6D7E6E8F309AB894408AB00F5EDE0E9EC08D5D3F1E8558BCAD90B8A"
  installCmd="irm https://sad-lang.org/install.ps1 | iex"
/>

<DownloadCard
  icon="🐧"
  platform="Linux"
  version="1.0.0"
  arch="x86_64"
  :files="[
    { name: 'sad-installer-1.0.0-linux-x64.sh', label: 'مثبّت رسومي ✅ (موصى به)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-installer-1.0.0-linux-x64.sh', size: '6.2 MB' },
    { name: 'sad-1.0.0-linux-x64.tar.gz', label: 'أرشيف TAR.GZ', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-linux-x64.tar.gz', size: '6.2 MB' },
    { name: 'sad-lang_1.0.0_amd64.deb', label: 'حزمة DEB (Debian/Ubuntu)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-lang_1.0.0_amd64.deb', size: '0.6 MB' },
  ]"
  sha256="851DCD17D3C5F002CE875BA93850AD3F69CC40D900CC7D783FD090F04A310663"
  installCmd="curl -fsSL https://sad-lang.org/install.sh | bash"
/>

</div>

::: info نسخة macOS — قريباً
نسخة **macOS** (PKG, DMG) قيد التحضير.
سيتم الإعلان عنها في [صفحة الإصدارات](https://github.com/SalehKadah/s-programming-language/releases).
:::

---

## 📦 محتويات الحزمة

| المكوّن | الوصف | الحجم |
|---------|-------|-------|
| `sad.exe` | المفسر الرئيسي | 3.2 MB |
| `sadc.exe` | المترجم LLVM (كود أصيل) | 83.9 MB |
| `sad-fmt.exe` | أداة التنسيق | 0.2 MB |
| `sad-repl.exe` | سطر الأوامر التفاعلي | 2.7 MB |
| `sad-lsp.exe` | خادم LSP | 1.4 MB |
| `sad-pkg.exe` | مدير الحزم | 0.3 MB |
| `SDL2.dll` | مكتبة الرسوميات | 2.4 MB |
| المكتبة القياسية | مكتبات عربية مضمّنة | stdlib/ |

::: tip مكوّنات اختيارية
عند تشغيل مثبّت Windows، يمكن إلغاء تحديد **المترجم LLVM** (75MB) إذا أردت المفسر فحسب.
:::

---

## ✅ التحقق من التثبيت

```bash
sad --version
# sad version 1.0.0

sadc --version
# sadc version 1.0.0 (LLVM 18)
```

---

## 💻 متطلبات النظام

<div class="system-req">

| المتطلب | Windows | Linux |
|---------|---------|-------|
| **نظام التشغيل** | Windows 10 x64 أو أحدث | Ubuntu 20.04+, Debian 11+, Fedora 38+ |
| **المساحة** | 120 MB (مع المترجم)، 15 MB (المفسر فقط) | 15 MB |
| **الذاكرة** | 256 MB RAM | 256 MB RAM |
| **المعمارية** | x86_64 (64-bit) | x86_64 (64-bit) |

</div>

---

## 📦 مستودع الحزم — سيلا هاب

بعد تثبيت لغة ص، يمكنك الوصول إلى **مستودع الحزم العربي** واستخدام آلاف الحزم البرمجية الجاهزة:

<div class="pkg-registry-card">
  <div class="pkg-registry-header">
    <span class="pkg-registry-icon">📦</span>
    <div>
      <div class="pkg-registry-title">سيلا هاب — مستودع حزم لغة ص</div>
      <div class="pkg-registry-sub">أول مستودع حزم برمجية عربي مفتوح المصدر</div>
    </div>
    <a href="https://sila-hub.dev" target="_blank" class="dl-btn primary">زيارة المستودع ←</a>
  </div>
</div>

```bash
# إضافة حزمة من المستودع
ص-حزم أضف شبكة

# البحث عن حزمة
ص-حزم بحث رياضيات

# نشر حزمتك الخاصة
ص-حزم نشر
```

---

## ❓ الأسئلة الشائعة

::: details ما الفرق بين المثبّت GUI والنسخة المحمولة (ZIP)؟
- **المثبّت GUI** (`sad-setup.exe`): تثبيت كامل مع إضافة `sad` إلى PATH تلقائياً، واختصارات، وإمكانية إلغاء التثبيت.
- **النسخة المحمولة** (`ZIP`): فك الضغط وتشغيل مباشرة. مناسبة إذا أردت تشغيل لغة ص بدون تثبيت أو على فلاش USB.
:::

::: details هل يمكنني استخدام المفسر فقط بدون المترجم LLVM؟
نعم! عند تشغيل المثبّت، يمكنك إلغاء تحديد **المترجم LLVM** (75 MB). المفسر `sad.exe` يعمل بشكل مستقل تماماً.
:::

::: details كيف أتحقق أن التثبيت تم بنجاح؟
افتح PowerShell أو CMD واكتب:
```
sad --version
```
يجب أن يظهر: `sad version 1.0.0`
:::

::: details أحصل على خطأ "Windows protected your PC" — ماذا أفعل؟
هذا تحذير Windows SmartScreen لأن البرنامج جديد. اضغط **More info** ثم **Run anyway**. البرنامج آمن ويمكنك التحقق من SHA256 أعلاه.
:::

<style scoped>
/* ===== Hero Download Bands ===== */
.dl-hero { display: flex; flex-direction: column; gap: 12px; margin: 24px 0; }
.dl-hero-os {
  display: flex; align-items: center; gap: 16px;
  padding: 16px 20px; border-radius: 12px;
  border: 1px solid var(--vp-c-border);
  background: var(--vp-c-bg-soft);
}
.dl-hero-os.windows { border-color: #0078d4; background: rgba(0,120,212,0.06); }
.dl-hero-os.linux   { border-color: #e8a000; background: rgba(232,160,0,0.06); }
.dl-hero-os.macos   { border-color: #888; background: rgba(128,128,128,0.06); }
.dl-hero-os.coming-soon { opacity: 0.6; }
.dl-btn.disabled {
  display: inline-block; padding: 10px 18px; border-radius: 8px; font-size: 0.88rem;
  font-weight: 600; border: 1px solid var(--vp-c-border);
  background: var(--vp-c-bg-soft); color: var(--vp-c-text-3); white-space: nowrap;
  cursor: default;
}
.dl-os-icon  { font-size: 2rem; }
.dl-os-info  { flex: 1; }
.dl-os-name  { font-weight: 700; font-size: 1rem; }
.dl-os-meta  { font-size: 0.8rem; color: var(--vp-c-text-2); }

/* ===== Download Buttons ===== */
.dl-btn {
  display: inline-block; padding: 10px 18px; border-radius: 8px; font-size: 0.88rem;
  font-weight: 600; text-decoration: none; border: 1px solid var(--vp-c-border);
  background: var(--vp-c-bg); color: var(--vp-c-text-1); white-space: nowrap;
  transition: all 0.15s;
}
.dl-btn:hover { background: var(--vp-c-brand-soft); border-color: var(--vp-c-brand-1); }
.dl-btn.primary {
  background: var(--vp-c-brand-1); color: #fff; border-color: var(--vp-c-brand-1);
}
.dl-btn.primary:hover { background: var(--vp-c-brand-2); }

/* ===== SHA Block ===== */
.sha-block { background: var(--vp-c-bg-soft); border-radius: 8px; padding: 14px 16px; margin: 16px 0; }
.sha-row   { display: flex; align-items: center; gap: 10px; flex-wrap: wrap; }
.sha-label { font-weight: 600; font-size: 0.85rem; white-space: nowrap; }
.sha-val   { font-size: 0.78rem; letter-spacing: .02em; word-break: break-all; flex: 1; }
.copy-btn {
  padding: 4px 12px; border-radius: 6px; border: 1px solid var(--vp-c-border);
  background: var(--vp-c-bg); cursor: pointer; font-size: 0.8rem; white-space: nowrap;
}
.copy-btn:hover { background: var(--vp-c-brand-soft); }

/* ===== VS Code Card ===== */
.vscode-card {
  border: 1px solid #007acc; border-radius: 12px;
  background: rgba(0,122,204,0.05); padding: 20px; margin: 16px 0;
}
.vscode-header  { display: flex; align-items: center; gap: 14px; margin-bottom: 14px; }
.vscode-icon    { font-size: 2rem; }
.vscode-title   { font-weight: 700; font-size: 1rem; }
.vscode-version { font-size: 0.8rem; color: var(--vp-c-text-2); }
.vscode-features {
  display: flex; flex-wrap: wrap; gap: 8px; font-size: 0.82rem;
  color: var(--vp-c-text-2);
}

/* ===== Package Registry Card ===== */
.pkg-registry-card {
  border: 1px solid #10b981; border-radius: 12px;
  background: rgba(16,185,129,0.06); padding: 20px; margin: 16px 0;
}
.pkg-registry-header {
  display: flex; align-items: center; gap: 16px; flex-wrap: wrap;
}
.pkg-registry-icon { font-size: 2rem; }
.pkg-registry-title { font-weight: 700; font-size: 1rem; }
.pkg-registry-sub { font-size: 0.8rem; color: var(--vp-c-text-2); }
</style>
