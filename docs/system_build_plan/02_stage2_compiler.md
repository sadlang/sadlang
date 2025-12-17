# المرحلة 2: مترجم مستقل ينتج Native Code
# Stage 2: Native Standalone Compiler

**المدة المتوقعة / Duration:** 4-5 أشهر / months  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### العربية

هذه المرحلة تهدف لتطوير مترجم كامل ينتج ملفات تنفيذية native بدون الحاجة لمفسر. المترجم سيستخدم LLVM كـ backend لإنتاج machine code محسّن لمعماريات مختلفة (x86-64, ARM, RISC-V).

**الهدف الرئيسي:** تحويل لغة ص من لغة مفسرة إلى لغة مجمّعة بأداء عالي.

### English

This stage aims to develop a complete compiler that produces native executable files without needing an interpreter. The compiler will use LLVM as a backend to generate optimized machine code for different architectures (x86-64, ARM, RISC-V).

**Main Goal:** Transform Sad Language from interpreted to compiled with high performance.

---

## 🎯 الأهداف التفصيلية / Detailed Goals

### العربية

1. ✅ تصميم معمارية المترجم (Frontend + Middle-end + Backend)
2. ✅ تطوير LLVM IR Generator من AST
3. ✅ دعم التحسينات (Optimizations)
4. ✅ إنتاج ملفات ELF/PE/Mach-O
5. ✅ دعم Static و Dynamic Linking
6. ✅ دعم Cross-Compilation
7. ✅ دعم Bootable Binaries
8. ✅ تكامل مع Bootloaders (GRUB, UEFI)
9. ✅ أدوات تصحيح (Debug Info Generation)

### English

1. ✅ Design compiler architecture (Frontend + Middle-end + Backend)
2. ✅ Develop LLVM IR Generator from AST
3. ✅ Support optimizations
4. ✅ Produce ELF/PE/Mach-O files
5. ✅ Support static & dynamic linking
6. ✅ Support cross-compilation
7. ✅ Support bootable binaries
8. ✅ Integration with bootloaders (GRUB, UEFI)
9. ✅ Debugging tools (Debug info generation)

---

## 🏗️ البنية المعمارية / Architecture

```
ملف ص (.s)
Sad Source File
    ↓
[Lexer] → Tokens
    ↓
[Parser] → AST
    ↓
[Semantic Analyzer] → Validated AST
    ↓
[LLVM IR Generator] → LLVM IR
    ↓
[LLVM Optimizer] → Optimized IR
    ↓
[LLVM Backend] → Machine Code
    ↓
[Linker] → Executable (ELF/PE/Mach-O)
    ↓
Native Binary / ملف تنفيذي
```

---

## 📝 المكونات الرئيسية / Main Components

### 1. Frontend (تحليل الكود / Code Analysis)

```
المكونات الموجودة (يتم توسيعها):
- Lexer ✅
- Parser ✅
- AST ✅
- Type Checker (يتم تحسينه)
```

### 2. Middle-end (LLVM IR Generation)

```cpp
// مثال: توليد LLVM IR من AST
// Example: Generate LLVM IR from AST

class LLVMCodeGenerator {
public:
    // العربية
    llvm::Module* توليد_كود(AST::Program* برنامج);
    llvm::Function* توليد_دالة(AST::FunctionDecl* دالة);
    llvm::Value* توليد_تعبير(AST::Expression* تعبير);
    
    // English
    llvm::Module* generateCode(AST::Program* program);
    llvm::Function* generateFunction(AST::FunctionDecl* func);
    llvm::Value* generateExpression(AST::Expression* expr);
};
```

### 3. Backend (إنتاج Machine Code)

```cpp
// استخدام LLVM لإنتاج ملفات تنفيذية
// Use LLVM to produce executables

// تحسينات مختلفة
llvm::PassManagerBuilder pmBuilder;
pmBuilder.OptLevel = 3;  // O3 optimization
pmBuilder.SizeLevel = 0;

// إنتاج object file
llvm::TargetMachine::CodeGenFileType fileType = 
    llvm::TargetMachine::CGFT_ObjectFile;
```

---

## 📝 المهام التفصيلية / Detailed Tasks

### 2.1 إعداد بيئة LLVM (أسبوعان)

#### العربية
- [ ] تنصيب LLVM 14+ development libraries
- [ ] إعداد CMake للربط مع LLVM
- [ ] إنشاء مشروع compiler منفصل
- [ ] اختبار بناء LLVM IR بسيط (Hello World)
- [ ] إعداد CI/CD لبناء المترجم تلقائياً

#### English
- [ ] Install LLVM 14+ development libraries
- [ ] Setup CMake to link with LLVM
- [ ] Create separate compiler project
- [ ] Test building simple LLVM IR (Hello World)
- [ ] Setup CI/CD for automated compiler builds

**الملفات الجديدة / New Files:**
- `compiler/CMakeLists.txt`
- `compiler/src/main.cpp`
- `compiler/include/llvm_generator.h`

---

### 2.2 تطوير LLVM IR Generator (شهران)

#### العربية

**أسبوع 1-2: البنية الأساسية**
- [ ] تصميم class `LLVMCodeGenerator`
- [ ] تنفيذ `generateModule()` - إنشاء LLVM Module
- [ ] تنفيذ `generateFunction()` - توليد functions
- [ ] دعم أنواع البيانات الأساسية (int, float, string, bool)

**أسبوع 3-4: التعابير**
- [ ] توليد IR للعمليات الحسابية (+, -, *, /, %)
- [ ] توليد IR للعمليات المنطقية (&&, ||, !)
- [ ] توليد IR للمقارنات (<, >, ==, !=, <=, >=)
- [ ] توليد IR لاستدعاءات الدوال

**أسبوع 5-6: العبارات**
- [ ] توليد IR لـ if/else
- [ ] توليد IR لـ while/for loops
- [ ] توليد IR لـ return
- [ ] توليد IR لـ variable declarations

**أسبوع 7-8: الميزات المتقدمة**
- [ ] توليد IR للمصفوفات (arrays)
- [ ] توليد IR للقواميس (maps)
- [ ] توليد IR للأصناف (classes) - أساسي
- [ ] توليد IR للمؤشرات (pointers)

#### English

**Week 1-2: Basic Structure**
- [ ] Design `LLVMCodeGenerator` class
- [ ] Implement `generateModule()` - Create LLVM Module
- [ ] Implement `generateFunction()` - Generate functions
- [ ] Support basic data types (int, float, string, bool)

**Week 3-4: Expressions**
- [ ] Generate IR for arithmetic operations (+, -, *, /, %)
- [ ] Generate IR for logical operations (&&, ||, !)
- [ ] Generate IR for comparisons (<, >, ==, !=, <=, >=)
- [ ] Generate IR for function calls

**Week 5-6: Statements**
- [ ] Generate IR for if/else
- [ ] Generate IR for while/for loops
- [ ] Generate IR for return
- [ ] Generate IR for variable declarations

**Week 7-8: Advanced Features**
- [ ] Generate IR for arrays
- [ ] Generate IR for maps
- [ ] Generate IR for classes (basic)
- [ ] Generate IR for pointers

**الملفات الجديدة / New Files:**
- `compiler/src/llvm_generator.cpp`
- `compiler/src/llvm_expression_gen.cpp`
- `compiler/src/llvm_statement_gen.cpp`
- `compiler/src/llvm_type_converter.cpp`

---

### 2.3 تنفيذ Runtime Library (شهر واحد)

#### العربية

المكتبة Runtime ضرورية لدعم ميزات اللغة التي لا يمكن توليدها مباشرة إلى LLVM IR.

**المكونات:**
- [ ] إدارة الذاكرة (malloc, free, GC)
- [ ] String operations
- [ ] Array operations (push, pop, slice)
- [ ] Map operations (insert, delete, lookup)
- [ ] I/O operations (print, read)
- [ ] Exception handling (throw, catch)

#### English

Runtime library is essential to support language features that cannot be directly generated to LLVM IR.

**Components:**
- [ ] Memory management (malloc, free, GC)
- [ ] String operations
- [ ] Array operations (push, pop, slice)
- [ ] Map operations (insert, delete, lookup)
- [ ] I/O operations (print, read)
- [ ] Exception handling (throw, catch)

**الملفات الجديدة / New Files:**
- `runtime/src/memory.c`
- `runtime/src/string_ops.c`
- `runtime/src/array_ops.c`
- `runtime/src/map_ops.c`
- `runtime/src/io_ops.c`
- `runtime/include/sad_runtime.h`

---

### 2.4 دعم التحسينات (Optimizations) (أسبوعان)

#### العربية
- [ ] تفعيل LLVM optimization passes
- [ ] دعم مستويات تحسين مختلفة (-O0, -O1, -O2, -O3)
- [ ] Dead code elimination
- [ ] Constant folding
- [ ] Inline expansion
- [ ] Loop optimizations
- [ ] قياس تأثير كل تحسين على الأداء

#### English
- [ ] Enable LLVM optimization passes
- [ ] Support different optimization levels (-O0, -O1, -O2, -O3)
- [ ] Dead code elimination
- [ ] Constant folding
- [ ] Inline expansion
- [ ] Loop optimizations
- [ ] Measure impact of each optimization on performance

---

### 2.5 إنتاج ملفات تنفيذية (ثلاثة أسابيع)

#### العربية

**أسبوع 1: Object File Generation**
- [ ] توليد ملفات `.o` (ELF format لـ Linux)
- [ ] توليد ملفات `.obj` (PE format لـ Windows)
- [ ] دعم debug symbols (DWARF)
- [ ] اختبار الملفات المولدة بـ `objdump`, `nm`

**أسبوع 2: Linking**
- [ ] تكامل مع system linker (`ld`, `lld`)
- [ ] Static linking مع runtime library
- [ ] Dynamic linking مع shared libraries
- [ ] دعم custom linker scripts

**أسبوع 3: Executable Testing**
- [ ] اختبار تشغيل البرامج المجمّعة
- [ ] قياس الأداء مقابل المفسر
- [ ] اختبار على أنظمة مختلفة (Linux, Windows, macOS)

#### English

**Week 1: Object File Generation**
- [ ] Generate `.o` files (ELF format for Linux)
- [ ] Generate `.obj` files (PE format for Windows)
- [ ] Support debug symbols (DWARF)
- [ ] Test generated files with `objdump`, `nm`

**Week 2: Linking**
- [ ] Integrate with system linker (`ld`, `lld`)
- [ ] Static linking with runtime library
- [ ] Dynamic linking with shared libraries
- [ ] Support custom linker scripts

**Week 3: Executable Testing**
- [ ] Test running compiled programs
- [ ] Measure performance vs interpreter
- [ ] Test on different systems (Linux, Windows, macOS)

---

### 2.6 دعم Cross-Compilation (أسبوعان)

#### العربية
- [ ] دعم target triples (x86_64-linux, x86_64-windows, arm64-linux)
- [ ] تكامل مع cross-compilation toolchains
- [ ] اختبار بناء binaries لأنظمة مختلفة
- [ ] توثيق كيفية استخدام cross-compilation

#### English
- [ ] Support target triples (x86_64-linux, x86_64-windows, arm64-linux)
- [ ] Integrate with cross-compilation toolchains
- [ ] Test building binaries for different systems
- [ ] Document how to use cross-compilation

---

### 2.7 دعم Bootable Binaries (ثلاثة أسابيع)

#### العربية

**أسبوع 1: Bare-Metal Setup**
- [ ] إنشاء custom linker script لـ bare-metal
- [ ] تعطيل standard library (freestanding mode)
- [ ] دعم entry point مخصص (`_start`)
- [ ] اختبار بناء binary بدون OS dependencies

**أسبوع 2: GRUB Integration**
- [ ] إضافة multiboot header
- [ ] اختبار إقلاع kernel عبر GRUB على QEMU
- [ ] دعم passing boot parameters
- [ ] توثيق خطوات إعداد GRUB

**أسبوع 3: UEFI Support**
- [ ] دعم UEFI application format (PE32+)
- [ ] اختبار إقلاع UEFI binary على QEMU
- [ ] توثيق الفروقات بين BIOS و UEFI

#### English

**Week 1: Bare-Metal Setup**
- [ ] Create custom linker script for bare-metal
- [ ] Disable standard library (freestanding mode)
- [ ] Support custom entry point (`_start`)
- [ ] Test building binary without OS dependencies

**Week 2: GRUB Integration**
- [ ] Add multiboot header
- [ ] Test kernel boot via GRUB on QEMU
- [ ] Support passing boot parameters
- [ ] Document GRUB setup steps

**Week 3: UEFI Support**
- [ ] Support UEFI application format (PE32+)
- [ ] Test UEFI binary boot on QEMU
- [ ] Document differences between BIOS and UEFI

---

### 2.8 Debug Info Generation (أسبوعان)

#### العربية
- [ ] توليد DWARF debug info
- [ ] دعم line number information
- [ ] دعم variable names في debugger
- [ ] اختبار debugging مع GDB/LLDB
- [ ] توثيق كيفية استخدام debugger مع كود ص

#### English
- [ ] Generate DWARF debug info
- [ ] Support line number information
- [ ] Support variable names in debugger
- [ ] Test debugging with GDB/LLDB
- [ ] Document how to use debugger with Sad code

---

## 🔧 أمثلة الاستخدام / Usage Examples

### العربية

```bash
# تجميع برنامج بسيط
sadc program.s -o program

# تجميع مع تحسينات
sadc program.s -O3 -o program_optimized

# تجميع لنظام آخر (cross-compile)
sadc program.s --target=x86_64-windows -o program.exe

# تجميع kernel bootable
sadc kernel.s --freestanding --linker-script=kernel.ld -o kernel.bin

# تجميع مع debug info
sadc program.s -g -o program_debug

# عرض LLVM IR
sadc program.s --emit-llvm -o program.ll

# عرض assembly
sadc program.s -S -o program.asm
```

### English

```bash
# Compile simple program
sadc program.s -o program

# Compile with optimizations
sadc program.s -O3 -o program_optimized

# Cross-compile to another system
sadc program.s --target=x86_64-windows -o program.exe

# Compile bootable kernel
sadc kernel.s --freestanding --linker-script=kernel.ld -o kernel.bin

# Compile with debug info
sadc program.s -g -o program_debug

# Show LLVM IR
sadc program.s --emit-llvm -o program.ll

# Show assembly
sadc program.s -S -o program.asm
```

---

## 🧪 الاختبارات / Testing

### العربية

#### اختبارات الوحدة / Unit Tests
- اختبار كل component من LLVM generator بشكل منفصل
- اختبار توليد IR لكل نوع من AST nodes
- اختبار التحسينات المختلفة

#### اختبارات التكامل / Integration Tests
- تجميع برامج كاملة وتشغيلها
- مقارنة نتائج البرامج المجمّعة مع المفسرة
- اختبار cross-compilation على أنظمة مختلفة

#### اختبارات الأداء / Performance Tests
- Benchmark برامج مجمّعة vs مفسرة
- قياس سرعة compilation
- قياس حجم الملفات التنفيذية

### English

#### Unit Tests
- Test each LLVM generator component separately
- Test IR generation for each AST node type
- Test different optimizations

#### Integration Tests
- Compile complete programs and run them
- Compare results of compiled vs interpreted programs
- Test cross-compilation on different systems

#### Performance Tests
- Benchmark compiled vs interpreted programs
- Measure compilation speed
- Measure executable file sizes

---

## 📊 معايير النجاح / Success Criteria

### العربية

✅ **المرحلة مكتملة عندما:**

1. المترجم ينتج ملفات تنفيذية تعمل على Linux, Windows, macOS
2. البرامج المجمّعة أسرع من المفسرة بـ 10x على الأقل
3. التحسينات تعمل بشكل صحيح (O0, O1, O2, O3)
4. Cross-compilation يعمل لأنظمة مختلفة
5. Bootable binaries تعمل على QEMU (GRUB + UEFI)
6. Debug info يعمل مع GDB/LLDB
7. جميع الاختبارات تمر بنجاح
8. التوثيق كامل مع أمثلة
9. Runtime library مستقر وسريع

### English

✅ **Stage Complete When:**

1. Compiler produces working executables on Linux, Windows, macOS
2. Compiled programs are at least 10x faster than interpreted
3. Optimizations work correctly (O0, O1, O2, O3)
4. Cross-compilation works for different systems
5. Bootable binaries work on QEMU (GRUB + UEFI)
6. Debug info works with GDB/LLDB
7. All tests pass successfully
8. Complete documentation with examples
9. Runtime library is stable and fast

---

## 🔗 الربط مع المراحل الأخرى / Links to Other Stages

- **المرحلة 1 (Low-Level):** يستخدم أنواع البيانات الجديدة
- **المرحلة 3 (System Library):** يربط مع المكتبة القياسية
- **المرحلة 4 (Assembly):** يدمج inline assembly في IR
- **المرحلة 6 (Kernel):** ينتج bootable kernel binary

---

## 📚 موارد إضافية / Additional Resources

### العربية

- [LLVM Tutorial](https://llvm.org/docs/tutorial/)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)
- [Kaleidoscope Tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/)
- [ELF File Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)
- [PE Format](https://docs.microsoft.com/en-us/windows/win32/debug/pe-format)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [UEFI Specification](https://uefi.org/specifications)

### English

- [LLVM Tutorial]
- [LLVM Language Reference]
- [Kaleidoscope Tutorial]
- [ELF File Format]
- [PE Format]
- [Multiboot Specification]
- [UEFI Specification]

---

**السابق / Previous:** [المرحلة 1: دعم الميزات منخفضة المستوى](01_stage1_low_level.md)  
**التالي / Next:** [المرحلة 3: المكتبة القياسية للنظام](03_stage3_stdlib.md)
