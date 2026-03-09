---
title: التثبيت والبدء
description: كيفية تثبيت لغة ص وكتابة أول برنامج
---

# التثبيت والبدء

## التثبيت السريع

::: code-group

```powershell [Windows — سطر واحد]
irm https://sad-lang.org/install.ps1 | iex
```

```bash [Linux/macOS — سطر واحد]
curl -fsSL https://sad-lang.org/install.sh | bash
```

:::

بعد التثبيت تحقق بـ:

```bash
sad --version
```

## التثبيت اليدوي

### Windows

1. حمّل `sad-setup-1.0.0-windows-x64.exe` من [صفحة التحميل](/download)
2. شغّل المثبّت كمسؤول (Administrator)
3. اتبع خطوات المعالج

سيُضاف `sad` و`sadc` تلقائياً إلى `PATH`.

### Linux

```bash
# تحميل AppImage
wget https://github.com/SalehKadah/s-programming-language/releases/download/v1.0.0/sad-1.0.0-x86_64.AppImage
chmod +x sad-1.0.0-x86_64.AppImage

# تشغيل مباشر
./sad-1.0.0-x86_64.AppImage برنامجي.ص

# أو تثبيت عبر .deb
sudo dpkg -i sad-lang_1.0.0_amd64.deb
```

### macOS

```bash
# عبر Homebrew
brew install sad-lang/tap/sad

# أو تحميل PKG
open sad-lang-1.0.0-macos-universal.pkg
```

## إضافة VS Code

```bash
code --install-extension sad-language-2.0.0.vsix
```

أو ابحث عن **"لغة ص"** في سوق VS Code Extensions.

## هيكل البيئة

بعد التثبيت ستجد:

```
~/sad/           ← المكتبة القياسية
  stdlib/
    core/
    io/
    math/
    string/
    ...
~/.sad/          ← الحزم المثبتة
  packages/
```

## التحقق الشامل

```bash
# تشغيل برنامج اختبار
echo 'اطبع_سطر("نجح التثبيت! 🎉")' > test.ص
sad test.ص
# الخرج: نجح التثبيت! 🎉
```

---

التالي: [أول برنامج ←](/docs/first-program)
