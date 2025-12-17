# التطوير: المفسر مقابل المترجم
# Development: Interpreter vs Compiler

## 🛠️ تجربة التطوير / Development Experience

### ملخص سريع / Quick Summary

| الجانب / Aspect | المفسر / Interpreter | المترجم / Compiler |
|----------------|---------------------|-------------------|
| **Edit-Run Cycle** | <1s ⚡⚡⚡ | 2-5s ⏱️ |
| **Debugging** | ممتاز 🔍🔍🔍 | جيد 🔍🔍 |
| **Error Messages** | Runtime ⚠️ | Compile-time ✅ |
| **Hot Reload** | ممكن ✅ | صعب ❌ |
| **Learning Curve** | سهل 📚 | متوسط 📚📚 |
| **Tooling** | بسيط 🔧 | معقد 🔧🔧🔧 |

---

## 🔄 دورة التحرير والتشغيل / Edit-Run Cycle

### المفسر / Interpreter

```
┌────────────────────────────────────────────────────┐
│  Development Workflow - المفسر                    │
└────────────────────────────────────────────────────┘

[1] Edit Code in VSCode
    ├─ تعديل program.s
    └─ حفظ الملف (Ctrl+S)
           ↓ <100ms

[2] Run Immediately
    └─ sad run program.s
           ↓ 80-120ms

[3] See Results
    ├─ إما النتيجة الصحيحة ✅
    └─ أو رسالة خطأ واضحة ❌
           ↓ <50ms

[4] Fix & Repeat
    └─ عودة للخطوة 1
    
────────────────────────────────────────────────────
Total Time: <500ms per iteration
Very Fast Feedback Loop! ⚡
```

**مثال عملي / Practical Example:**

```bash
# Terminal 1: Watch mode
$ sad watch program.s
> Watching for changes...
> Change detected! Running...
> ✓ Success in 95ms

# Terminal 2: VSCode
# تعديل الكود → Ctrl+S → رؤية النتيجة فوراً!
```

**المميزات / Advantages:**
- ✅ **فوري**: لا وقت انتظار
- ✅ **مرن**: تجربة سريعة للأفكار
- ✅ **تفاعلي**: REPL ممكن
- ✅ **مناسب للتعلم**: نتائج فورية

### المترجم / Compiler

```
┌────────────────────────────────────────────────────┐
│  Development Workflow - المترجم                   │
└────────────────────────────────────────────────────┘

[1] Edit Code in VSCode
    ├─ تعديل program.s
    └─ حفظ الملف (Ctrl+S)
           ↓ <100ms

[2] Compile
    └─ sad compile program.s
           ↓ 500-3000ms ⏳
           
[3] Check Errors
    ├─ إذا كان هناك خطأ ❌
    │   ├─ قراءة رسالة الخطأ
    │   └─ العودة للخطوة 1
    │
    └─ إذا نجحت الترجمة ✅
           ↓ 0ms
           
[4] Run Executable
    └─ ./program.exe
           ↓ 10-50ms
           
[5] See Results
    ├─ النتيجة الصحيحة ✅
    └─ أو خطأ runtime ⚠️
           ↓
           
[6] Fix & Repeat
    └─ عودة للخطوة 1
    
────────────────────────────────────────────────────
Total Time: 1-5 seconds per iteration
Slower Feedback Loop ⏱️
```

**تحسينات ممكنة / Possible Improvements:**

```bash
# Incremental compilation
$ sad compile --incremental program.s
> Recompiling only changed modules...
> Compiled in 450ms (saved 2100ms)

# Watch mode with auto-compile
$ sad watch --compile program.s
> Watching for changes...
> Change detected! Compiling...
> ✓ Compiled successfully in 520ms
> Running...
> Output: ...
```

---

## 🐛 التصحيح / Debugging

### المفسر / Interpreter

**المميزات / Advantages:**

1. **تتبع المكدس الكامل / Full Stack Trace**
```
خطأ في السطر 42 في الدالة 'احسب':
  41 | نتيجة = 10 / عدد؛
  42 | أرجع نتيجة * 2؛
     | ^^^^^^^^^^^^^^^ خطأ: القسمة على صفر

Call Stack:
  [1] احسب() في program.s:42
  [2] معالج() في program.s:28
  [3] main() في program.s:12
```

2. **فحص المتغيرات / Variable Inspection**
```cpp
// يمكن طباعة أي متغير في أي وقت
اطبع("قيمة س ="، س)؛  // Works immediately
اطبع("نوع ص ="، النوع(ص))؛
```

3. **Breakpoints (محتمل) / Potential Breakpoints**
```sad
# يمكن إضافة دعم breakpoints
وقف()؛  // توقف هنا وفحص الحالة
```

4. **تعديل أثناء التشغيل / Runtime Modification**
```python
# في REPL mode
> س = 10
> اطبع(س * 2)
20
> س = 5  # تعديل فوري
> اطبع(س * 2)
10
```

**التنفيذ الحالي / Current Implementation:**

```cpp
// src/interpreter/exception.cpp
try {
    interpreter.execute(ast);
} catch (const RuntimeError& e) {
    std::cerr << "❌ (AR) خطأ في التنفيذ / (EN) Runtime Error\n";
    std::cerr << "📍 (AR) الموقع / (EN) Location: " 
              << e.location() << "\n";
    std::cerr << "💬 (AR) الرسالة / (EN) Message: " 
              << e.what() << "\n";
    std::cerr << "📚 Call Stack:\n";
    for (const auto& frame : e.stackTrace()) {
        std::cerr << "  → " << frame << "\n";
    }
}
```

### المترجم / Compiler

**التحديات / Challenges:**

1. **فقدان المعلومات / Information Loss**
   - الكود الأصلي لم يعد موجوداً
   - يحتاج Debug Symbols (DWARF format)

2. **التحسينات تعقد التصحيح / Optimizations Complicate Debugging**
```cpp
// Original code:
دالة احسب(س، ص) {
    مؤقت1 = س * 2؛
    مؤقت2 = ص + 5؛
    أرجع مؤقت1 + مؤقت2؛
}

// Optimized code (مؤقت1 و مؤقت2 اختفوا):
دالة احسب(س، ص) {
    أرجع (س * 2) + (ص + 5)؛  // Inlined
}
```

3. **الحاجة لأدوات خارجية / Need External Tools**
   - GDB (Linux)
   - LLDB (macOS)
   - Visual Studio Debugger (Windows)

**الحلول / Solutions:**

1. **Debug Build**
```bash
# بناء مع معلومات التصحيح
$ sad compile -g program.s
# أو
$ sad compile --debug program.s

# الملف الناتج يحتوي على:
# - رموز التصحيح (Debug symbols)
# - خريطة السطور (Line mapping)
# - معلومات المتغيرات
```

2. **استخدام Debugger**
```bash
# Windows
$ devenv /debugexe program.exe

# Linux/macOS
$ gdb ./program
(gdb) break main
(gdb) run
(gdb) print variable_name
```

3. **Sanitizers**
```bash
# كشف الأخطاء في الذاكرة
$ sad compile --sanitize=address program.s
$ sad compile --sanitize=memory program.s
$ sad compile --sanitize=thread program.s
```

---

## 📝 رسائل الخطأ / Error Messages

### المفسر / Interpreter

**أخطاء وقت التشغيل / Runtime Errors:**

```
════════════════════════════════════════════════
❌ (AR) خطأ في التنفيذ / (EN) Runtime Error
════════════════════════════════════════════════

(AR) نوع الخطأ / (EN) Error Type:
  القسمة على صفر / Division by Zero

📍 (AR) الموقع / (EN) Location:
  الملف / File: program.s
  السطر / Line: 42
  العمود / Column: 15

💬 (AR) الوصف / (EN) Description:
  حاولت القسمة على صفر في التعبير: (10 / 0)
  Division by zero in expression: (10 / 0)

🔎 (AR) السياق / (EN) Context:
  40 | دالة احسب(عدد) {
  41 |     نتيجة = 10 / عدد؛
  42 |     أرجع نتيجة * 2؛
     |     ^^^^^^^^^^^^^^^^
  43 | }

📚 (AR) مكدس الاستدعاءات / (EN) Call Stack:
  [1] احسب(عدد=0) في program.s:42
  [2] معالج_البيانات() في program.s:28
  [3] main() في program.s:12

💡 (AR) اقتراح / (EN) Suggestion:
  تحقق من قيمة المتغير 'عدد' قبل القسمة
  Check the value of variable 'عدد' before division
  
  مثال:
  إذا(عدد != 0) {
      نتيجة = 10 / عدد؛
  }
════════════════════════════════════════════════
```

**المميزات / Advantages:**
- ✅ تفاصيل كاملة عن الحالة
- ✅ Stack trace كامل
- ✅ قيم المتغيرات
- ✅ اقتراحات للحل
- ✅ ثنائي اللغة (عربي/إنجليزي)

### المترجم / Compiler

**أخطاء وقت الترجمة / Compile-Time Errors:**

```
════════════════════════════════════════════════
⛔ (AR) خطأ في الترجمة / (EN) Compilation Error
════════════════════════════════════════════════

(AR) نوع الخطأ / (EN) Error Type:
  عدم تطابق الأنواع / Type Mismatch

📍 (AR) الموقع / (EN) Location:
  الملف / File: program.s
  السطر / Line: 28
  العمود / Column: 12

💬 (AR) الوصف / (EN) Description:
  لا يمكن تعيين قيمة نصية لمتغير رقمي
  Cannot assign string value to numeric variable

🔎 (AR) السياق / (EN) Context:
  26 | متغير عدد: رقم = 10؛
  27 | 
  28 | عدد = "مرحبا"؛
     |       ^^^^^^^
     |       Expected: رقم (number)
     |       Got: نص (string)
  29 |

💡 (AR) اقتراح / (EN) Suggestion:
  - استخدم دالة لرقم() للتحويل
    Use لرقم() function for conversion
  - أو غيّر نوع المتغير إلى نص
    Or change variable type to نص

════════════════════════════════════════════════
```

**المميزات / Advantages:**
- ✅ اكتشاف الأخطاء قبل التشغيل
- ✅ التحقق من الأنواع compile-time
- ✅ اقتراحات تلقائية
- ✅ منع فئة كاملة من الأخطاء

**العيوب / Disadvantages:**
- ⚠️ لا يكتشف أخطاء logic
- ⚠️ أخطاء runtime ما زالت ممكنة
- ⚠️ رسائل أقل تفصيلاً من المفسر

---

## 🔥 Hot Reload

### المفسر / Interpreter

**الإمكانية / Capability:**
```sad
# script.s
دالة معالج() {
    اطبع("الإصدار 1")؛
}

معالج()؛
```

```bash
# Terminal 1: Run with watch
$ sad watch script.s
> الإصدار 1
> Watching for changes...

# Terminal 2: Edit file
$ echo 'اطبع("الإصدار 2")؛' > script.s

# Terminal 1: Auto-reload
> Change detected! Reloading...
> الإصدار 2
> Watching for changes...
```

**مميزات / Features:**
- ✅ تحديث فوري
- ✅ لا restart مطلوب
- ✅ مثالي للخوادم (servers)
- ✅ تطوير تفاعلي

### المترجم / Compiler

**التحديات / Challenges:**
- ❌ يحتاج إعادة ترجمة كاملة
- ❌ يحتاج restart للبرنامج
- ❌ hot reload معقد جداً

**البدائل / Alternatives:**
```bash
# Fast rebuild + restart
$ sad watch --compile --run program.s
> Watching for changes...
> Change detected!
> Compiling... (520ms)
> Restarting...
> Output: ...
```

---

## 📚 منحنى التعلم / Learning Curve

### للمطورين / For Developers

```
┌────────────────────────────────────────┐
│  Learning Difficulty                   │
├────────────────────────────────────────┤
│                                        │
│  Sad Interpreter:                      │
│  ████ Easy                             │
│  - Similar to Python                   │
│  - Instant feedback                    │
│  - Simple mental model                 │
│                                        │
│  Sad Compiler:                         │
│  ████████ Medium                       │
│  - Need to understand compilation      │
│  - Build systems                       │
│  - Linking concepts                    │
│  - Optimization flags                  │
└────────────────────────────────────────┘
```

### للطلاب / For Students

**المفسر / Interpreter:**
- ✅ مثالي للتعليم
- ✅ رؤية مباشرة للنتائج
- ✅ سهل التجربة
- ✅ لا complexity إضافي

**المترجم / Compiler:**
- ⚠️ مفاهيم إضافية مطلوبة
- ⚠️ وقت انتظار يقلل التجربة
- ✅ يعلم مفاهيم أعمق
- ✅ مفيد لفهم الأداء

---

## 🔧 الأدوات / Tooling

### المفسر / Interpreter

**الأدوات المطلوبة / Required Tools:**
```
Minimal Toolchain:
├─ sad.exe          (المفسر فقط)
├─ Text Editor      (أي محرر نصوص)
└─ Terminal         (سطر الأوامر)

Total Size: ~5 MB
Setup Time: <1 minute
```

**أدوات إضافية مفيدة / Useful Additional Tools:**
```
Enhanced Setup:
├─ VSCode           (محرر متقدم)
├─ Sad Extension    (syntax highlighting)
├─ Git              (version control)
└─ sad-format       (code formatter)

Total Size: ~300 MB
Setup Time: ~10 minutes
```

### المترجم / Compiler

**الأدوات المطلوبة / Required Tools:**
```
Full Toolchain:
├─ sad-compiler     (المترجم)
├─ LLVM/GCC         (backend compiler)
├─ Linker           (system linker)
├─ C++ Runtime      (standard library)
├─ Build System     (cmake/make)
├─ Debugger         (gdb/lldb)
└─ Text Editor      (VSCode/IDE)

Total Size: ~2-5 GB
Setup Time: 30-60 minutes
```

**صعوبة الإعداد / Setup Complexity:**
```
Windows:
$ choco install llvm
$ choco install cmake
$ sad setup compiler

Linux:
$ sudo apt install llvm-14 clang cmake
$ sad setup compiler

macOS:
$ brew install llvm cmake
$ sad setup compiler
```

---

## 💡 أفضل الممارسات / Best Practices

### للتطوير بالمفسر / Interpreter Development

```sad
# 1. استخدم print للتصحيح
اطبع("القيمة الحالية:"، متغير)؛

# 2. تحقق من الأنواع
اطبع("نوع المتغير:"، النوع(متغير))؛

# 3. استخدم try/catch للأخطاء المتوقعة
جرب {
    نتيجة = 10 / عدد؛
} اصطد(خطأ) {
    اطبع("خطأ:"، خطأ)؛
}

# 4. اختبر بشكل تفاعلي
# استخدم REPL mode للتجربة السريعة
```

### للتطوير بالمترجم / Compiler Development

```bash
# 1. استخدم Debug build أثناء التطوير
$ sad compile -g --debug program.s

# 2. اختبر بشكل شامل قبل Release build
$ sad test program.s
$ sad compile -O2 program.s  # Release build

# 3. استخدم CI/CD
# .github/workflows/build.yml
name: Build
on: [push]
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v2
      - run: sad compile -O2 app.s
      - run: ./app.exe

# 4. Profile قبل التحسين
$ sad compile --profile app.s
$ ./app
$ sad analyze-profile app.profile
```

---

## 📊 مقارنة سريعة / Quick Comparison

| السيناريو / Scenario | المفسر / Interpreter | المترجم / Compiler |
|---------------------|---------------------|-------------------|
| **Learning** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Prototyping** | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| **Production** | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Debugging** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| **Setup** | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| **Speed** | ⭐⭐ | ⭐⭐⭐⭐⭐ |

---

**التالي**: [النشر / Deployment →](05_deployment.md)
