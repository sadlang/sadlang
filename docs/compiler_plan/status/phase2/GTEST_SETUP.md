# Google Test Setup Instructions
# تعليمات إعداد Google Test

## للمستخدمين على Windows باستخدام vcpkg

### الطريقة 1: استخدام vcpkg (الموصى بها / Recommended)

```powershell
# 1. تثبيت vcpkg إذا لم يكن مثبتاً / Install vcpkg if not installed
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# 2. تثبيت Google Test / Install Google Test
.\vcpkg install gtest:x64-windows

# 3. دمج vcpkg مع CMake / Integrate vcpkg with CMake
.\vcpkg integrate install

# 4. إعادة تكوين المشروع / Reconfigure project
cd C:\s\s_language\build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake -DBUILD_TESTS=ON
cmake --build . --config Debug
```

### الطريقة 2: تحميل يدوي (Manual Download)

```powershell
# 1. تحميل Google Test من GitHub
# Download from: https://github.com/google/googletest/releases

# 2. استخراج في مجلد / Extract to folder
# C:\libs\googletest

# 3. بناء Google Test / Build Google Test
cd C:\libs\googletest
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cmake --build . --config Release

# 4. تكوين المشروع مع مسار Google Test / Configure project with GTest path
cd C:\s\s_language\build
cmake .. -DGTEST_ROOT=C:\libs\googletest -DBUILD_TESTS=ON
cmake --build . --config Debug
```

### الطريقة 3: استخدام FetchContent (مباشرة في CMake)

أضف هذا إلى CMakeLists.txt:

```cmake
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
```

---

## التحقق من التثبيت / Verify Installation

```powershell
# بناء الاختبارات / Build tests
cd C:\s\s_language\build
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Debug

# تشغيل الاختبارات / Run tests
ctest -C Debug --verbose

# أو تشغيل مباشرة / Or run directly
.\bin\Debug\constant_folding_tests.exe
.\bin\Debug\dead_code_elimination_tests.exe
.\bin\Debug\optimizer_integration_tests.exe
```

---

## ملاحظات / Notes

### بدون Google Test
إذا لم يتم العثور على Google Test، سيتخطى CMake اختبارات المحسّن تلقائياً.

If Google Test is not found, CMake will automatically skip optimizer tests.

### رسائل CMake المتوقعة / Expected CMake Messages

**مع Google Test / With Google Test**:
```
✓ اختبارات محسّن SIR مفعلة / SIR Optimizer tests enabled
✓ SIR Optimizer Tests configured
```

**بدون Google Test / Without Google Test**:
```
⚠ Google Test not found - اختبارات المحسّن ستكون بسيطة
  للحصول على Google Test: vcpkg install gtest
```

---

## الاختبارات المتاحة / Available Tests

عند التثبيت الصحيح، ستحصل على:

When properly installed, you'll get:

1. **constant_folding_tests** (30+ tests)
   - Integer arithmetic
   - Float arithmetic
   - Bitwise operations
   - Comparisons
   - Overflow/underflow
   - Division by zero

2. **dead_code_elimination_tests** (25+ tests)
   - Side effects detection
   - Dead instruction removal
   - Liveness analysis
   - Unreachable blocks
   - BFS traversal

3. **optimizer_integration_tests** (30+ tests)
   - Optimization levels (O0-O3)
   - Pass management
   - Statistics
   - Real-world examples

**المجموع / Total**: 85+ tests

---

## الدعم / Support

إذا واجهت مشاكل:

If you encounter issues:

1. تأكد من تثبيت CMake 3.15+ / Ensure CMake 3.15+ is installed
2. تأكد من Visual Studio 2017+ / Ensure Visual Studio 2017+
3. تحقق من متغيرات البيئة / Check environment variables
4. استخدم Developer Command Prompt لـ VS

---

## الخطوات السريعة (vcpkg) / Quick Steps (vcpkg)

```powershell
# تثبيت / Install
vcpkg install gtest:x64-windows
vcpkg integrate install

# بناء / Build
cd C:\s\s_language\build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DBUILD_TESTS=ON
cmake --build . --config Debug

# تشغيل / Run
ctest -C Debug -V
```

---

**ملاحظة**: هذا ليس ضرورياً لتشغيل المترجم، فقط للاختبارات!

**Note**: This is NOT required to run the compiler, only for tests!

الحمد لله
