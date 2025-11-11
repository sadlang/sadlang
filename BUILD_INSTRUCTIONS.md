# 🚀 تعليمات البناء السريعة / Quick Build Instructions

<div dir="rtl">

## ⚡ بناء سريع - Windows

### المتطلبات
```powershell
# تثبيت CMake
winget install Kitware.CMake

# تثبيت Visual Studio 2019 أو أحدث
# قم بتحميله من: https://visualstudio.microsoft.com/
```

### البناء
```powershell
# 1. افتح PowerShell في مجلد المشروع
cd c:\s\s_language

# 2. أنشئ مجلد البناء
mkdir build
cd build

# 3. أنشئ ملفات البناء
cmake ..

# 4. ابنِ المشروع
cmake --build . --config Release

# 5. شغّل البرنامج
.\bin\Release\sad.exe --version
```

---

## ⚡ بناء سريع - Linux/macOS

### المتطلبات
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# Fedora
sudo dnf install gcc-c++ cmake

# macOS
brew install cmake
```

### البناء
```bash
# 1. انتقل لمجلد المشروع
cd ~/s_language

# 2. أنشئ وابنِ
mkdir build && cd build
cmake ..
make -j4

# 3. شغّل البرنامج
./bin/sad --version
```

---

## 🧪 اختبار البناء

### اختبار بسيط
```bash
# أنشئ ملف test.s
echo 'اطبع("مرحباً!")' > test.s

# شغّل البرنامج
sad test.s
```

### تشغيل الأمثلة
```bash
# مثال مرحبا بالعالم
sad examples/basics/hello.s
```

---

## 🐛 حل المشاكل

### خطأ: cmake: command not found
```bash
# تأكد من تثبيت CMake
cmake --version

# إذا لم يكن مثبتاً، ثبته
# Windows: winget install Kitware.CMake
# Linux: sudo apt-get install cmake
# macOS: brew install cmake
```

### خطأ: No C++ compiler found
```bash
# تأكد من تثبيت المترجم
# Windows: Visual Studio 2019+
# Linux: sudo apt-get install build-essential
# macOS: xcode-select --install
```

### خطأ في الترميز UTF-8
```bash
# تأكد من حفظ الملفات بـ UTF-8 with BOM
# في VS Code: File > Save with Encoding > UTF-8 with BOM
```

---

## ⚙️ خيارات البناء المتقدمة

### تفعيل وضع التنقيح
```bash
cmake -DSAD_DEBUG_ENABLED=ON ..
```

### بناء بدون اختبارات
```bash
cmake -DBUILD_TESTS=OFF ..
```

### بناء محسّن
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

---

## 📦 بناء كامل من الصفر

```bash
# حذف البناء القديم
rm -rf build

# بناء جديد
mkdir build && cd build
cmake ..
cmake --build . --config Release

# اختبار
ctest --output-on-failure
```

---

## 🎯 أوامر سريعة

```bash
# عرض الإصدار
sad --version

# عرض المساعدة
sad --help

# تنفيذ ملف
sad program.s

# بناء مع خيارات كاملة
cmake -DCMAKE_BUILD_TYPE=Release -DSAD_DEBUG_ENABLED=ON -DBUILD_TESTS=ON ..
```

---

## ✅ التحقق من البناء الناجح

بعد البناء، يجب أن ترى:
```
build/
├── bin/
│   └── sad.exe (أو sad على Linux/macOS)
└── lib/
    └── sad_core.lib (أو sad_core.a)
```

جرب الأمر:
```bash
sad --version
```

إذا ظهرت معلومات الإصدار، فالبناء ناجح! 🎉

---

**للمساعدة الإضافية:**
- راجع [دليل البدء](docs/tutorials/getting_started.md)
- افتح issue على GitHub
- راسلنا: support@sadlang.org

</div>
