---
title: تحميل لغة ص
description: حمّل مثبّت لغة ص لنظامك — Windows · Linux · macOS
---

<script setup>
import { ref, onMounted } from 'vue'

const copied = ref('')

function copy(text, key) {
  navigator.clipboard.writeText(text)
  copied.value = key
  setTimeout(() => { copied.value = '' }, 1800)
}

const sha256win = '52FFBB8D94FFC69128E5D9404965E6F60D2FC0AFF94B7FAEC78F33964A28980A'
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
      href="https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe"
      download
    >⬇️ تحميل sad-setup.exe</a>
  </div>

  <div class="dl-hero-os linux">
    <div class="dl-os-icon">🐧</div>
    <div class="dl-os-info">
      <div class="dl-os-name">Linux x86_64</div>
      <div class="dl-os-meta">الإصدار 1.0.0 · 22 MB · AppImage</div>
    </div>
    <a
      class="dl-btn"
      href="https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-1.0.0-x86_64.AppImage"
      download
    >⬇️ تحميل AppImage</a>
  </div>

  <div class="dl-hero-os macos">
    <div class="dl-os-icon">🍎</div>
    <div class="dl-os-info">
      <div class="dl-os-name">macOS Universal</div>
      <div class="dl-os-meta">الإصدار 1.0.0 · 20 MB · مثبّت PKG</div>
    </div>
    <a
      class="dl-btn"
      href="https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang-1.0.0-macos-universal.pkg"
      download
    >⬇️ تحميل PKG</a>
  </div>
</div>

---

## 🔐 التحقق من الملفات (SHA256)

<div class="sha-block">
  <div class="sha-row">
    <span class="sha-label">Windows x64:</span>
    <code class="sha-val">{{ sha256win }}</code>
    <button class="copy-btn" @click="copy(sha256win, 'sha-win')">
      {{ copied === 'sha-win' ? '✅ تم النسخ' : '📋 نسخ' }}
    </button>
  </div>
</div>

```powershell
# تحقق من SHA256 في PowerShell
Get-FileHash .\sad-setup-1.0.0-windows-x64.exe -Algorithm SHA256
```

---

## ⚡ تثبيت سريع عبر الأوامر

::: code-group

```powershell [PowerShell (Windows)]
# تثبيت بسطر واحد
irm https://sad-lang.org/install.ps1 | iex
```

```powershell [Winget (Windows)]
winget install sad-lang.sad
```

```powershell [Scoop (Windows)]
scoop bucket add sad https://github.com/sad-lang/scoop-bucket
scoop install sad
```

```bash [curl (Linux/macOS)]
curl -fsSL https://sad-lang.org/install.sh | bash
```

```bash [Homebrew (macOS)]
brew install sad-lang/tap/sad
```

```bash [apt (Debian/Ubuntu)]
curl -fsSL https://sad-lang.org/gpg.key | sudo gpg --dearmor \
  -o /usr/share/keyrings/sad-lang.gpg
echo "deb [signed-by=/usr/share/keyrings/sad-lang.gpg] \
  https://apt.sad-lang.org stable main" \
  | sudo tee /etc/apt/sources.list.d/sad-lang.list
sudo apt update && sudo apt install sad-lang
```

:::

---

## 🗃️ جميع الإصدارات

<div class="download-grid">

<DownloadCard
  icon="🪟"
  platform="Windows"
  version="1.0.0"
  arch="x64"
  :files="[
    { name: 'sad-setup-1.0.0-windows-x64.exe', label: 'مثبّت GUI ✅ (موصى به)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe', size: '24.8 MB' },
    { name: 'sad-1.0.0-windows-x64.zip', label: 'نقلة مضغوطة (ZIP)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-1.0.0-windows-x64.zip', size: '21 MB' },
  ]"
  sha256="52FFBB8D94FFC69128E5D9404965E6F60D2FC0AFF94B7FAEC78F33964A28980A"
  installCmd="irm https://sad-lang.org/install.ps1 | iex"
/>

<DownloadCard
  icon="🐧"
  platform="Linux"
  version="1.0.0"
  arch="x86_64"
  :files="[
    { name: 'sad-1.0.0-x86_64.AppImage', label: 'AppImage (شامل)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-1.0.0-x86_64.AppImage', size: '22 MB' },
    { name: 'sad-lang_1.0.0_amd64.deb', label: 'حزمة DEB (Debian/Ubuntu)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang_1.0.0_amd64.deb', size: '18 MB' },
    { name: 'sad-lang-1.0.0.x86_64.rpm', label: 'حزمة RPM (RHEL/Fedora)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang-1.0.0.x86_64.rpm', size: '18 MB' },
  ]"
  installCmd="curl -fsSL https://sad-lang.org/install.sh | bash"
/>

<DownloadCard
  icon="🍎"
  platform="macOS"
  version="1.0.0"
  arch="Universal (arm64 + x86_64)"
  :files="[
    { name: 'sad-lang-1.0.0-macos-universal.pkg', label: 'حزمة PKG (مثبّت)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang-1.0.0-macos-universal.pkg', size: '20 MB' },
    { name: 'sad-lang-1.0.0-macos-universal.dmg', label: 'صورة DMG', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang-1.0.0-macos-universal.dmg', size: '20 MB' },
  ]"
  installCmd="brew install sad-lang/tap/sad"
/>

</div>

---

## 🧩 إضافة VS Code

<div class="vscode-card">
  <div class="vscode-header">
    <span class="vscode-icon">⚡</span>
    <div>
      <div class="vscode-title">sad-language for VS Code</div>
      <div class="vscode-version">الإصدار 2.0.0 · 2 MB</div>
    </div>
    <a
      class="dl-btn primary"
      href="https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-language-2.0.0.vsix"
      download
    >⬇️ تحميل .vsix</a>
  </div>
  <div class="vscode-features">
    <span>✅ تلوين الكود العربي</span>
    <span>✅ إكمال تلقائي</span>
    <span>✅ تشخيص الأخطاء</span>
    <span>✅ تنسيق تلقائي</span>
    <span>✅ Go to Definition</span>
  </div>
</div>

تثبيت الإضافة بعد التحميل:

```bash
code --install-extension sad-language-2.0.0.vsix
```

---

## 📦 محتويات الحزمة

| المكوّن | الوصف | الحجم |
|---------|-------|-------|
| `sad.exe` / `sad` | المفسر الرئيسي | 17.9 MB |
| `sadc.exe` / `sadc` | المترجم LLVM (كود أصيل) | 75 MB |
| `sad-lsp.exe` / `sad-lsp` | خادم LSP | 4.7 MB |
| `sad-pkg.exe` / `sad-pkg` | مدير الحزم | 2 MB |
| `sad-fmt.exe` / `sad-fmt` | منسّق الكود | 977 KB |
| المكتبة القياسية | مكتبات عربية مضمّنة | 5 MB |
| الأمثلة | 50+ مثال جاهز | 500 KB |

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
| Linux | glibc 2.17+ (Ubuntu 18.04+) |
| macOS | macOS 12 Monterey أو أحدث |

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
