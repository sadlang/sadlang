---
title: Download Sad Language
description: Download the Sad Language installer for Windows and Linux
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
    { name: 'sad-setup-1.0.0-windows-x64.exe', label: 'GUI Installer (Recommended)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-setup-1.0.0-windows-x64.exe', size: '41.4 MB' },
    { name: 'sad-1.0.0-windows-x64.zip', label: 'Portable ZIP', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-windows-x64.zip', size: '68.8 MB' },
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
    { name: 'sad-installer-1.0.0-linux-x64.sh', label: 'GUI Installer (Recommended)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-installer-1.0.0-linux-x64.sh', size: '6.2 MB' },
    { name: 'sad-1.0.0-linux-x64.tar.gz', label: 'TAR.GZ Archive', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-linux-x64.tar.gz', size: '6.2 MB' },
    { name: 'sad-lang_1.0.0_amd64.deb', label: 'DEB Package (Debian/Ubuntu)', url: 'https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-lang_1.0.0_amd64.deb', size: '0.6 MB' },
  ]"
  sha256="851DCD17D3C5F002CE875BA93850AD3F69CC40D900CC7D783FD090F04A310663"
  installCmd="curl -fsSL https://sad-lang.org/install.sh | bash"
/>

</div>

::: info macOS — Coming Soon
macOS builds are in preparation and will be released soon.
Follow [GitHub Releases](https://github.com/SalehKadah/s-programming-language/releases) for updates.
:::

---

## Quick Install

```powershell
# One-line install (PowerShell — Windows)
irm https://sad-lang.org/install.ps1 | iex
```

```bash
# One-line install (Bash — Linux)
curl -fsSL https://sad-lang.org/install.sh | bash
```

```bash
# DEB package (Debian/Ubuntu)
sudo dpkg -i sad-lang_1.0.0_amd64.deb
```

---

## System Requirements

| Requirement | Windows | Linux |
|-------------|---------|-------|
| **OS** | Windows 10 x64+ | Ubuntu 20.04+, Debian 11+, Fedora 38+ |
| **Disk Space** | 120 MB (with compiler), 15 MB (interpreter only) | 15 MB |
| **RAM** | 256 MB | 256 MB |
| **Architecture** | x86_64 (64-bit) | x86_64 (64-bit) |
