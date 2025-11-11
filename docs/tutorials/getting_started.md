# دليل البدء السريع - لغة ص / Quick Start Guide - Sad Language

<div dir="rtl">

## 🚀 البدء السريع

مرحباً بك في لغة ص! هذا الدليل سيساعدك على البدء خلال دقائق.

## 📦 المتطلبات الأساسية

قبل البدء، تأكد من تثبيت:

### Windows
1. **Visual Studio 2019 أو أحدث** (يتضمن مترجم C++)
   - قم بتحميله من: https://visualstudio.microsoft.com/
   - اختر "Desktop development with C++" أثناء التثبيت

2. **CMake 3.15 أو أحدث**
   - قم بتحميله من: https://cmake.org/download/
   - أو استخدم: `winget install Kitware.CMake`

### Linux/macOS
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# Fedora
sudo dnf install gcc-c++ cmake

# macOS
brew install cmake
```

## 🔨 بناء المشروع

### الخطوة 1: استنساخ المشروع
```bash
cd c:\s\s_language
```

### الخطوة 2: إنشاء مجلد البناء
```bash
mkdir build
cd build
```

### الخطوة 3: إنشاء ملفات البناء
```bash
# Windows (Visual Studio)
cmake .. -G "Visual Studio 16 2019"

# Linux/macOS
cmake ..
```

### الخطوة 4: بناء المشروع
```bash
# Windows
cmake --build . --config Release

# Linux/macOS
make -j4
```

### الخطوة 5: تشغيل البرنامج
```bash
# Windows
.\bin\Release\sad.exe --version

# Linux/macOS
./bin/sad --version
```

## 📝 كتابة أول برنامج

1. أنشئ ملف باسم `test.s`:
```sad
اطبع("مرحباً بالعالم!")
```

2. شغّل البرنامج:
```bash
# Windows
.\bin\Release\sad.exe test.s

# Linux/macOS
./bin/sad test.s
```

## 🎯 الأمثلة المتوفرة

يوجد في مجلد `examples/` عدة أمثلة:

```bash
# مثال مرحبا بالعالم
sad examples/basics/hello.s

# مثال الدوال (قريباً)
sad examples/functions/calculator.s

# مثال البرمجة الكائنية (قريباً)
sad examples/oop/person.s

# مثال الرسومات (قريباً)
sad examples/graphics/window.s
```

## 🐛 حل المشاكل الشائعة

### CMake غير موجود
```bash
# تثبيت CMake على Windows
winget install Kitware.CMake

# على Linux
sudo apt-get install cmake
```

### خطأ في المترجم
- تأكد من تثبيت Visual Studio 2019 أو أحدث (Windows)
- تأكد من تثبيت GCC 7+ أو Clang 5+ (Linux/macOS)

### خطأ في الترميز (UTF-8)
- تأكد من حفظ ملفات .s بترميز UTF-8 مع BOM
- في VS Code: File > Save with Encoding > UTF-8 with BOM

## 📚 المزيد من الموارد

- [الوثائق الكاملة](../docs/)
- [أمثلة متقدمة](../examples/)
- [دليل المساهمة](../CONTRIBUTING.md)

## 💡 نصائح

1. **استخدم UTF-8 مع BOM** لجميع ملفات .s
2. **فعّل وضع التنقيح** أثناء التطوير بإضافة `-DSAD_DEBUG_ENABLED=ON` لـ CMake
3. **راجع الأخطاء** في ملف `build/CMakeFiles/CMakeError.log` إذا واجهت مشاكل

## 🤝 الدعم

إذا واجهت أي مشكلة:
- افتح issue على GitHub
- راسلنا على: support@sadlang.org
- اطلع على [الأسئلة الشائعة](../docs/FAQ.md)

---

بالتوفيق في رحلتك مع لغة ص! 🚀

Happy coding with Sad! 🚀

</div>
