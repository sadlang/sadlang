---
title: Installation
description: How to install Sad Language and run your first program
---

# Installation

## Quick Install

::: code-group

```powershell [Windows — one line]
irm https://sad-lang.org/install.ps1 | iex
```

```bash [Linux/macOS — one line]
curl -fsSL https://sad-lang.org/install.sh | bash
```

```bash [Homebrew (macOS/Linux)]
brew install sad-lang/tap/sad
```

```powershell [Scoop (Windows)]
scoop bucket add sad https://github.com/sad-lang/scoop-bucket
scoop install sad
```

:::

After installation, verify:
```bash
sad --version
# sad version 1.0.0 (interpreter)
sadc --version
# sadc version 1.0.0 (compiler, LLVM 18)
```

## Manual Installation

### Windows

1. Download `sad-setup-1.0.0-windows-x64.exe` from the [Download page](/en/download)
2. Run the installer as Administrator
3. Follow the wizard — `sad` and `sadc` are added to PATH automatically

### Linux

```bash
wget https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-x86_64.AppImage
chmod +x sad-1.0.0-x86_64.AppImage
./sad-1.0.0-x86_64.AppImage my-file.ص
```

### macOS

```bash
open sad-lang-1.0.0-macos-universal.pkg
```

## VS Code Extension

```bash
code --install-extension sad-language-2.0.0.vsix
```

## Verify Installation

```bash
echo 'اطبع_سطر("Installation successful! 🎉")' > test.ص
sad test.ص
# Output: Installation successful! 🎉
```

---

Next: [Your First Program →](/en/docs/first-program)
