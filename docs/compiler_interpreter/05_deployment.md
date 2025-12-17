# النشر: المفسر مقابل المترجم
# Deployment: Interpreter vs Compiler

## 📦 نظرة عامة على النشر / Deployment Overview

### الفرق الأساسي / Core Difference

```
┌────────────────────────────────────────────────────┐
│  Deployment Comparison                             │
├────────────────────────────────────────────────────┤
│                                                    │
│  Interpreter:                                      │
│  Source Code + Runtime → User Machine              │
│  📄 program.s + 🔧 sad.exe → 💻                   │
│                                                    │
│  Compiler:                                         │
│  Source Code → Compilation → Executable            │
│  📄 program.s → 🏭 → 📦 program.exe → 💻         │
│                                                    │
└────────────────────────────────────────────────────┘
```

---

## 🚀 نشر المفسر / Interpreter Deployment

### الهيكل المطلوب / Required Structure

```
MyApp/
├─ sad.exe                    (5 MB)   # المفسر
├─ vcruntime140.dll           (1 MB)   # C++ Runtime
├─ msvcp140.dll               (500 KB) # C++ STL
├─ src/
│  ├─ main.s                  (10 KB)  # الكود المصدري
│  ├─ utils.s                 (5 KB)
│  └─ config.s                (2 KB)
├─ data/
│  └─ config.json             (1 KB)
└─ run.bat                    (1 KB)   # نص تشغيل
────────────────────────────────────────
Total Size: ~7-8 MB
```

### نص التشغيل / Run Script

**Windows (run.bat):**
```batch
@echo off
REM بسم الله الرحمن الرحيم
REM تشغيل تطبيق لغة ص

cd /d "%~dp0"
sad.exe src\main.s
pause
```

**Linux (run.sh):**
```bash
#!/bin/bash
# بسم الله الرحمن الرحيم
# تشغيل تطبيق لغة ص

cd "$(dirname "$0")"
./sad src/main.s
```

### المميزات / Advantages

✅ **سهولة النشر:**
```
1. نسخ المجلد بالكامل
2. لا حاجة للترجمة
3. الكود المصدري قابل للتعديل
4. تحديثات سهلة (استبدال .s فقط)
```

✅ **التوافق عبر المنصات:**
```
MyApp-Windows/
├─ sad.exe              (Windows)
└─ src/*.s

MyApp-Linux/
├─ sad                  (Linux)
└─ src/*.s

MyApp-macOS/
├─ sad                  (macOS)
└─ src/*.s

# نفس الكود المصدري يعمل على الكل!
```

✅ **التحديثات السريعة:**
```bash
# تحديث التطبيق:
# 1. استبدل ملفات .s فقط
# 2. لا حاجة لإعادة بناء
# 3. المستخدم يحصل على التحديث فوراً
```

### العيوب / Disadvantages

❌ **حجم كبير:**
- المفسر الكامل مطلوب (~5-7 MB)
- كل تطبيق يحتاج نسخة من المفسر

❌ **الكود المصدري مكشوف:**
```
src/main.s  ← يمكن للمستخدم قراءته وتعديله
```

❌ **الأداء:**
- تنفيذ أبطأ 10-50x
- استهلاك ذاكرة أعلى

❌ **الاعتماديات:**
- يحتاج C++ Runtime على النظام
- قد تحتاج تثبيت Visual C++ Redistributable

---

## 📦 نشر المترجم / Compiler Deployment

### الهيكل المطلوب / Required Structure

```
MyApp/
├─ MyApp.exe                  (800 KB)  # الملف التنفيذي
├─ data/
│  └─ config.json             (1 KB)
└─ README.txt                 (1 KB)
────────────────────────────────────────
Total Size: ~1 MB

# Optional: Small runtime
├─ sad_runtime.dll            (200 KB)  # إذا كان dynamically linked
```

### أنواع الربط / Linking Types

#### 1. Static Linking (الربط الثابت)

```bash
$ sad compile --static program.s -o app.exe
```

**المميزات:**
- ✅ ملف واحد فقط
- ✅ لا dependencies
- ✅ نشر بسيط جداً
- ✅ يعمل على أي نظام

**العيوب:**
- ⚠️ حجم أكبر (~1-2 MB)
- ⚠️ لا يستفيد من system libraries

#### 2. Dynamic Linking (الربط الديناميكي)

```bash
$ sad compile --dynamic program.s -o app.exe
```

**المميزات:**
- ✅ حجم أصغر (~500 KB)
- ✅ يستخدم system libraries
- ✅ تحديثات security من النظام

**العيوب:**
- ⚠️ يحتاج runtime DLLs
- ⚠️ مشاكل توافق محتملة

---

## 🌍 النشر عبر المنصات / Cross-Platform Deployment

### المفسر / Interpreter

```
┌────────────────────────────────────────────┐
│  Universal Deployment Strategy             │
└────────────────────────────────────────────┘

MyApp/
├─ bin/
│  ├─ windows/
│  │  └─ sad.exe
│  ├─ linux/
│  │  └─ sad
│  └─ macos/
│     └─ sad
├─ src/
│  └─ *.s              ← نفس الملفات للكل
└─ run.py
```

**run.py (لانتقاء المفسر المناسب):**
```python
#!/usr/bin/env python3
import platform
import subprocess
import sys

system = platform.system().lower()
if system == 'windows':
    interpreter = 'bin/windows/sad.exe'
elif system == 'linux':
    interpreter = 'bin/linux/sad'
elif system == 'darwin':  # macOS
    interpreter = 'bin/macos/sad'
else:
    print(f"Unsupported platform: {system}")
    sys.exit(1)

subprocess.run([interpreter, 'src/main.s'])
```

### المترجم / Compiler

```
┌────────────────────────────────────────────┐
│  Platform-Specific Builds                  │
└────────────────────────────────────────────┘

# يجب ترجمة لكل منصة بشكل منفصل:

# Windows build
$ sad compile --target=x86_64-windows program.s -o MyApp-Windows.exe

# Linux build
$ sad compile --target=x86_64-linux program.s -o MyApp-Linux

# macOS build
$ sad compile --target=aarch64-macos program.s -o MyApp-macOS

Distribution:
├─ MyApp-Windows.exe     (1.2 MB)
├─ MyApp-Linux           (900 KB)
└─ MyApp-macOS           (1.1 MB)
```

**Cross-Compilation:**
```bash
# من Windows، ترجمة لـ Linux:
$ sad compile --target=x86_64-linux --sysroot=/path/to/linux/sysroot program.s

# يحتاج:
# - Cross-compiler toolchain
# - Target system headers
# - Target system libraries
```

---

## 📱 النشر للتطبيقات / Application Deployment

### تطبيقات سطح المكتب / Desktop Applications

#### المفسر / Interpreter

```
Installer.exe
├─ Install sad runtime        (5 MB)
├─ Copy application files     (50 KB)
├─ Create shortcuts
└─ Register file associations

User sees:
Desktop\MyApp.lnk → sad.exe src\main.s
```

**مثال Installer (NSIS):**
```nsis
; installer.nsi
!define APP_NAME "MyApp"
!define APP_VERSION "1.0.0"

OutFile "MyApp-Setup.exe"
InstallDir "$PROGRAMFILES\${APP_NAME}"

Section "Install"
    SetOutPath "$INSTDIR"
    File "sad.exe"
    File "vcruntime140.dll"
    File /r "src"
    
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" \
        "$INSTDIR\sad.exe" \
        "src\main.s" \
        "$INSTDIR\icon.ico"
SectionEnd
```

#### المترجم / Compiler

```
Installer.exe
├─ Copy MyApp.exe             (1 MB)
├─ Copy data files            (10 KB)
├─ Create shortcuts
└─ Optional: Install runtime  (200 KB)

User sees:
Desktop\MyApp.lnk → MyApp.exe
```

**مثال Installer:**
```nsis
Section "Install"
    SetOutPath "$INSTDIR"
    File "MyApp.exe"
    File /r "data"
    
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" \
        "$INSTDIR\MyApp.exe"
SectionEnd
```

---

### تطبيقات الويب / Web Applications

#### المفسر / Interpreter

**Backend Server:**
```sad
# server.s
استورد HTTP؛

خادم = HTTP.أنشئ_خادم(8080)؛

خادم.عند_طلب(دالة(طلب، رد) {
    إذا(طلب.المسار == "/") {
        رد.أرسل_نص("مرحباً بالعالم!")؛
    }
})؛

خادم.استمع()؛
```

**Deployment:**
```bash
# Docker container
FROM sadlang:latest
COPY src/ /app/
CMD ["sad", "/app/server.s"]

# أو systemd service
[Service]
ExecStart=/usr/bin/sad /var/www/myapp/server.s
Restart=always
```

#### المترجم / Compiler

**Compiled Server:**
```bash
# ترجمة للخادم
$ sad compile --optimize-size server.s -o server

# Docker (smaller image)
FROM alpine:latest
COPY server /app/
CMD ["/app/server"]

# حجم الـ image:
# - Interpreter: ~50 MB
# - Compiler: ~10 MB
```

---

## 🔒 الأمان / Security

### المفسر / Interpreter

**التحديات / Challenges:**

❌ **الكود المصدري مكشوف:**
```
المستخدم يمكنه:
- قراءة الكود الكامل
- معرفة الخوارزميات
- استخراج API keys (إذا كانت hardcoded)
- تعديل الكود
```

**الحلول المحدودة / Limited Solutions:**

```bash
# 1. Obfuscation (تعقيد الكود)
$ sad-obfuscate program.s -o program-obf.s

# قبل:
متغير مفتاح_API = "secret123"؛

# بعد:
متغير _0x4a2b = "secret123"؛

# ⚠️ لا يزال قابل للقراءة!
```

```bash
# 2. Encryption (تشفير)
$ sad-encrypt program.s -o program.enc --key mykey

# المفسر يفك التشفير في runtime
$ sad-secure program.enc --key mykey

# ⚠️ المفتاح يجب أن يكون موجود
```

### المترجم / Compiler

**الحماية القوية / Strong Protection:**

✅ **الكود المصدري غير موجود:**
```
MyApp.exe → Binary machine code
لا يمكن قراءة الكود الأصلي بسهولة
```

✅ **الـ Reverse Engineering صعب:**
```bash
# Disassembly ممكن لكن معقد:
$ objdump -d MyApp.exe

# يعطي assembly code فقط:
000000014000100  push   %rbp
000000014000101  mov    %rsp,%rbp
000000014000104  sub    $0x20,%rsp
# ...

# لا يمكن استرجاع أسماء المتغيرات أو التعليقات
```

✅ **تقنيات إضافية:**
```bash
# 1. Strip symbols (إزالة رموز التصحيح)
$ sad compile --strip program.s

# 2. Code obfuscation
$ sad compile --obfuscate program.s

# 3. Anti-debugging
$ sad compile --anti-debug program.s

# 4. Encryption of constants
$ sad compile --encrypt-strings program.s
```

---

## 📊 مقارنة النشر / Deployment Comparison

| الجانب / Aspect | المفسر / Interpreter | المترجم / Compiler |
|----------------|---------------------|-------------------|
| **حجم الحزمة** | ~7-8 MB 📦📦📦 | ~1-2 MB 📦 |
| **Dependencies** | عالية ⚠️⚠️⚠️ | منخفضة ✅ |
| **سهولة النشر** | سهل ✅ | متوسط ⚠️ |
| **الأمان** | ضعيف ❌ | قوي ✅✅✅ |
| **التحديثات** | سهلة جداً ✅✅✅ | تحتاج إعادة ترجمة ⚠️ |
| **التوافق** | ممتاز ✅✅✅ | يحتاج builds متعددة ⚠️ |
| **السرعة** | بطيء ⚠️ | سريع ✅✅✅ |

---

## 🎯 استراتيجيات النشر / Deployment Strategies

### 1. النشر الهجين / Hybrid Deployment

```
┌────────────────────────────────────────────┐
│  Best of Both Worlds                       │
└────────────────────────────────────────────┘

Development Phase:
├─ استخدم المفسر
├─ تطوير سريع
└─ تصحيح سهل

Production Phase:
├─ استخدم المترجم
├─ أداء عالي
└─ حجم صغير

Strategy:
$ sad run app.s              # Development
$ sad compile -O2 app.s      # Production release
```

### 2. النشر بالتدريج / Staged Deployment

```
Alpha/Beta Testing:
├─ نسخة Interpreter
└─ سهولة جمع feedback وتصحيح bugs

Stable Release:
├─ نسخة Compiler
└─ أداء محسّن للمستخدمين النهائيين
```

### 3. النشر حسب السوق / Market-Specific Deployment

```
Free Version:
├─ Interpreter-based
├─ أبطأ قليلاً
└─ يشجع على الترقية

Premium Version:
├─ Compiler-based  
├─ أسرع
└─ ميزات إضافية
```

---

## 🚀 أتمتة النشر / Deployment Automation

### CI/CD Pipeline

```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  build-interpreter-package:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
    steps:
      - uses: actions/checkout@v2
      
      - name: Package Interpreter Version
        run: |
          mkdir MyApp-${{ matrix.os }}
          cp sad${{ runner.os == 'Windows' && '.exe' || '' }} MyApp-${{ matrix.os }}/
          cp -r src/ MyApp-${{ matrix.os }}/
          zip -r MyApp-${{ matrix.os }}.zip MyApp-${{ matrix.os }}/
      
      - name: Upload Release
        uses: actions/upload-artifact@v2
        with:
          name: MyApp-${{ matrix.os }}
          path: MyApp-${{ matrix.os }}.zip

  build-compiled-package:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
    steps:
      - uses: actions/checkout@v2
      
      - name: Compile Application
        run: |
          sad compile -O2 --static src/main.s -o MyApp${{ runner.os == 'Windows' && '.exe' || '' }}
          
      - name: Package Compiled Version
        run: |
          mkdir MyApp-Compiled-${{ matrix.os }}
          cp MyApp${{ runner.os == 'Windows' && '.exe' || '' }} MyApp-Compiled-${{ matrix.os }}/
          cp -r data/ MyApp-Compiled-${{ matrix.os }}/
          zip -r MyApp-Compiled-${{ matrix.os }}.zip MyApp-Compiled-${{ matrix.os }}/
      
      - name: Upload Release
        uses: actions/upload-artifact@v2
        with:
          name: MyApp-Compiled-${{ matrix.os }}
          path: MyApp-Compiled-${{ matrix.os }}.zip
```

---

## 💡 توصيات النشر / Deployment Recommendations

### للمشاريع الصغيرة / For Small Projects

```
✅ استخدم المفسر:
- نشر بسيط
- تحديثات سهلة
- لا يهم الأداء كثيراً
```

### للمشاريع التجارية / For Commercial Projects

```
✅ استخدم المترجم:
- أداء أفضل
- حماية الكود
- حجم أصغر
- مظهر احترافي
```

### للتطبيقات التعليمية / For Educational Applications

```
✅ استخدم المفسر:
- الكود مفتوح للدراسة
- سهل التعديل
- مناسب للتعلم
```

### للخوادم / For Servers

```
✅ استخدم المترجم:
- أداء عالي مهم
- استهلاك موارد أقل
- يقلل تكاليف الـ hosting
```

---

**التالي**: [التصحيح / Debugging →](06_debugging.md)
