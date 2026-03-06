---
title: Download Sad Language
description: Download the Sad Language installer for your platform
---

# Download Sad Language

<SadVersion />

## Version 1.0.0 — First Official Release

**Release Date:** April 9, 2025  
**MIT License** · [View Release Notes](https://github.com/sad-lang/sad-language/releases/tag/v1.0.0)

---

## Direct Downloads

<div class="download-grid">

<DownloadCard
  icon="🪟"
  platform="Windows"
  version="1.0.0"
  arch="x64"
  :files="[
    { name: 'sad-setup-1.0.0-windows-x64.exe', label: 'GUI Installer (Recommended)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe', size: '24.8 MB' },
    { name: 'sad-1.0.0-windows-x64.zip', label: 'Portable ZIP', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-1.0.0-windows-x64.zip', size: '21 MB' },
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
    { name: 'sad-1.0.0-x86_64.AppImage', label: 'AppImage (Bundled)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-1.0.0-x86_64.AppImage', size: '22 MB' },
    { name: 'sad-lang_1.0.0_amd64.deb', label: 'DEB Package (Debian/Ubuntu)', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang_1.0.0_amd64.deb', size: '18 MB' },
  ]"
  installCmd="curl -fsSL https://sad-lang.org/install.sh | bash"
/>

<DownloadCard
  icon="🍎"
  platform="macOS"
  version="1.0.0"
  arch="Universal (arm64 + x86_64)"
  :files="[
    { name: 'sad-lang-1.0.0-macos-universal.pkg', label: 'PKG Installer', url: 'https://github.com/sad-lang/sad-language/releases/download/v1.0.0/sad-lang-1.0.0-macos-universal.pkg', size: '20 MB' },
  ]"
  installCmd="brew install sad-lang/tap/sad"
/>

</div>

---

## Package Managers

::: code-group

```powershell [Winget]
winget install sad-lang.sad
```

```powershell [Scoop]
scoop bucket add sad https://github.com/sad-lang/scoop-bucket
scoop install sad
```

```bash [Homebrew]
brew install sad-lang/tap/sad
```

:::

## System Requirements

| OS | Requirement |
|----|-------------|
| Windows | Windows 10 x64 or newer |
| Linux | glibc 2.17+ (Ubuntu 18.04+, Debian Buster+) |
| macOS | macOS 12 Monterey or newer |
