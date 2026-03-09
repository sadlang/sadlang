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

const sha256setup = '52FFBB8D94FFC69128E5D9404965E6F60D2FC0AFF94B7FAEC78F33964A28980A'
const sha256zip = '409C1E66734ED0D13F8872D6D6D8F6E27BB313434E209D4856619C63D3A5E5D7'
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
      <div class="dl-os-meta">الإصدار 1.0.0 · 24.8 MB · مثبّت GUI رسمي</div>
    </div>
    <a
      class="dl-btn primary"
      href="https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe"
      download
    >⬇️ تحميل sad-setup.exe</a>
  </div>

  <div class="dl-hero-os linux coming-soon">
    <div class="dl-os-icon">🐧</div>
    <div class="dl-os-info">
      <div class="dl-os-name">Linux x86_64</div>
      <div class="dl-os-meta">قريباً — النسخة قيد التحضير</div>
    </div>
    <span class="dl-btn disabled">🔜 قريباً</span>
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

::: warning ملاحظة بخصوص المنصات الأخرى
حالياً لغة ص متوفرة على **نظام Windows فقط**. نسخ Linux و macOS قيد التحضير وستُنشر قريباً.
تابع [صفحة الإصدارات على GitHub](https://github.com/SalehKadah/s-programming-language/releases) للاطلاع على آخر المستجدات.
:::

---

## 🔐 التحقق من الملفات (SHA256)

<div class="sha-block">
  <div class="sha-row">
    <span class="sha-label">المثبّت (EXE):</span>
    <code class="sha-val">{{ sha256setup }}</code>
    <button class="copy-btn" @click="copy(sha256setup, 'sha-setup')">
      {{ copied === 'sha-setup' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
  <div class="sha-row" style="margin-top: 8px;">
    <span class="sha-label">النسخة المحمولة (ZIP):</span>
    <code class="sha-val">{{ sha256zip }}</code>
    <button class="copy-btn" @click="copy(sha256zip, 'sha-zip')">
      {{ copied === 'sha-zip' ? '✅ تم النسخ' : '📋 نسخ' }}
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

---

## 🗃️ جميع الإصدارات

<div class="download-grid">

<DownloadCard
  icon="🪟"
  platform="Windows"
  version="1.0.0"
  arch="x64"
  :files="[
    { name: 'sad-setup-1.0.0-windows-x64.exe', label: 'مثبّت GUI ✅ (موصى به)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe', size: '24.8 MB' },
    { name: 'sad-1.0.0-windows-x64.zip', label: 'نسخة محمولة (ZIP)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-windows-x64.zip', size: '26.6 MB' },
  ]"
  sha256="52FFBB8D94FFC69128E5D9404965E6F60D2FC0AFF94B7FAEC78F33964A28980A"
  installCmd="irm https://sad-lang.org/install.ps1 | iex"
/>

</div>

::: info منصات أخرى — قريباً
نسخة **Linux** (AppImage, DEB, RPM) ونسخة **macOS** (PKG, DMG) قيد التحضير.
سيتم الإعلان عنها في [صفحة الإصدارات](https://github.com/SalehKadah/s-programming-language/releases).
:::

---

## 📦 محتويات الحزمة

| المكوّن | الوصف | الحجم |
|---------|-------|-------|
| `sad.exe` | المفسر الرئيسي | 4.8 MB |
| `sadc.exe` | المترجم LLVM (كود أصيل) | 75.9 MB |
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

| النظام | الحد الأدنى |
|--------|------------|
| Windows | Windows 10 x64 أو أحدث |

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
</style>
