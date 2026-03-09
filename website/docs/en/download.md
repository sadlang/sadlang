---
title: Download Sad Language
description: Download the Sad Language installer for Windows
---

# Download Sad Language

<SadVersion />

## Version 1.0.0 — First Official Release

**Release Date:** April 9, 2025  
**MIT License** · [View Release Notes](https://github.com/SalehKadah/s-programming-language/releases/tag/v1.0.0)

---

## Direct Downloads

<div class="download-grid">

<DownloadCard
  icon="🪟"
  platform="Windows"
  version="1.0.0"
  arch="x64"
  :files="[
    { name: 'sad-setup-1.0.0-windows-x64.exe', label: 'GUI Installer (Recommended)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe', size: '24.8 MB' },
    { name: 'sad-1.0.0-windows-x64.zip', label: 'Portable ZIP', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-windows-x64.zip', size: '26.6 MB' },
  ]"
  sha256="52FFBB8D94FFC69128E5D9404965E6F60D2FC0AFF94B7FAEC78F33964A28980A"
  installCmd="irm https://sad-lang.org/install.ps1 | iex"
/>

</div>

::: warning Other Platforms — Coming Soon
Sad Language is currently available for **Windows only**. Linux and macOS builds are in preparation and will be released soon.
Follow [GitHub Releases](https://github.com/SalehKadah/s-programming-language/releases) for updates.
:::

---

## Quick Install

```powershell
# One-line install (PowerShell — Windows)
irm https://sad-lang.org/install.ps1 | iex
```

---

## System Requirements

| OS | Requirement |
|----|-------------|
| Windows | Windows 10 x64 or newer |
