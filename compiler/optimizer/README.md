# SIR Optimizer / محسّن SIR

مُحسِّن متعدد التمريرات لـ Sad Intermediate Representation (SIR)

Multi-pass optimizer for Sad Intermediate Representation (SIR)

---

## 📋 جدول المحتويات / Table of Contents

- [نظرة عامة / Overview](#نظرة-عامة--overview)
- [الميزات / Features](#الميزات--features)
- [المعمارية / Architecture](#المعمارية--architecture)
- [الاستخدام / Usage](#الاستخدام--usage)
- [مستويات التحسين / Optimization Levels](#مستويات-التحسين--optimization-levels)
- [التمريرات المتاحة / Available Passes](#التمريرات-المتاحة--available-passes)
- [API](#api)
- [الأمثلة / Examples](#الأمثلة--examples)
- [البناء / Building](#البناء--building)
- [الاختبار / Testing](#الاختبار--testing)

---

## 🎯 نظرة عامة / Overview

محسّن SIR هو نظام تحسين متقدم يعمل على التمثيل الوسيط SIR. يطبق مجموعة من تمريرات التحسين لتحسين جودة الكود وأدائه.

The SIR Optimizer is an advanced optimization system that works on the SIR intermediate representation. It applies a series of optimization passes to improve code quality and performance.

**الحالة / Status:** ✅ Core Implementation Complete (25%)

---

## ✨ الميزات / Features

### المطبق / Implemented ✅

- ✅ **4 مستويات تحسين** (O0, O1, O2, O3)
- ✅ **تنفيذ متكرر** للتمريرات حتى التقارب
- ✅ **نظام إحصائيات شامل** مع قياس الأداء
- ✅ **طي الثوابت** (Constant Folding)
  - عمليات حسابية (int, float)
  - عمليات منطقية (AND, OR, XOR, NOT)
  - عمليات مقارنة
  - فحص الفيض والقسمة على صفر
- ✅ **حذف الكود الميت** (Dead Code Elimination)
  - تحليل الحيوية (Liveness Analysis)
  - حذف التعليمات الميتة
  - حذف الكتل غير القابلة للوصول
- ✅ **تعليقات ثنائية اللغة** (عربي/إنجليزي)
- ✅ **وضع التصحيح** (Debug Mode)

### قادم / Coming Soon ⏳

- ⏳ Common Subexpression Elimination (CSE)
- ⏳ Copy Propagation
- ⏳ Register Coalescing
- ⏳ Loop Optimizations

---

## 🏗️ المعمارية / Architecture

### التسلسل الهرمي / Hierarchy

```
Optimizer (المحسن الرئيسي / Main Optimizer)
    ↓
PassManager (مدير التمريرات / Pass Manager)
    ↓
OptimizationPass (التمرير الأساسي / Base Pass)
    ├── ConstantFoldingPass (طي الثوابت)
    ├── DeadCodeEliminationPass (حذف الكود الميت)
    ├── CSE Pass (قريباً / Coming Soon)
    └── Copy Propagation Pass (قريباً / Coming Soon)
```

### هيكل الملفات / File Structure

```
compiler/optimizer/
├── include/
│   ├── optimizer.h                    (170 lines) ✅
│   ├── pass.h                         (120 lines) ✅
│   ├── pass_manager.h                 (110 lines) ✅
│   ├── constant_folding_pass.h        (150 lines) ✅
│   └── dead_code_elimination_pass.h   (150 lines) ✅
│
├── src/
│   ├── optimizer.cpp                  (440 lines) ✅
│   ├── pass.cpp                       (70 lines) ✅
│   ├── pass_manager.cpp               (190 lines) ✅
│   ├── constant_folding_pass.cpp      (350 lines) ✅
│   └── dead_code_elimination_pass.cpp (430 lines) ✅
│
└── tests/
    ├── test_constant_folding.cpp      (قريباً)
    ├── test_dce.cpp                   (قريباً)
    └── test_optimizer.cpp             (قريباً)
```

**المجموع / Total:** 2,180 سطر (Headers: 700, Source: 1,480)

---

## 🚀 الاستخدام / Usage

### استخدام أساسي / Basic Usage

```cpp
#include "optimizer/include/optimizer.h"

// إنشاء محسن بمستوى O2 / Create optimizer with O2 level
auto optimizer = std::make_unique<Optimizer>(OptimizationLevel::O2);

// تحسين وحدة SIR / Optimize SIR module
bool modified = optimizer->optimize(sirModule);

// طباعة الإحصائيات / Print statistics
optimizer->printStats();

// الحصول على نسبة التحسين / Get improvement percentage
double improvement = optimizer->getStats().getImprovementPercentage();
```

### استخدام متقدم / Advanced Usage

```cpp
// إنشاء محسن مخصص / Create custom optimizer
auto optimizer = std::make_unique<Optimizer>(OptimizationLevel::O0);

// إضافة تمريرات مخصصة / Add custom passes
optimizer->addPass(std::make_unique<ConstantFoldingPass>());
optimizer->addPass(std::make_unique<DeadCodeEliminationPass>());
optimizer->addPass(std::make_unique<MyCustomPass>());

// تحسين / Optimize
optimizer->optimize(sirModule);
```

---

## 🎚️ مستويات التحسين / Optimization Levels

### O0 - بدون تحسين / No Optimization
```cpp
OptimizationLevel::O0
```
- لا يتم تطبيق أي تحسينات
- سريع للترجمة، مفيد للتصحيح
- **الاستخدام:** التطوير والتصحيح

### O1 - تحسين أساسي / Basic Optimization
```cpp
OptimizationLevel::O1
```
- Constant Folding
- Dead Code Elimination
- **الاستخدام:** توازن بين السرعة وجودة الكود

### O2 - تحسين قياسي / Standard Optimization (Default)
```cpp
OptimizationLevel::O2  // الافتراضي / Default
```
- جميع تحسينات O1
- Common Subexpression Elimination (قريباً)
- Copy Propagation (قريباً)
- **الاستخدام:** الإنتاج (Production)

### O3 - تحسين عدواني / Aggressive Optimization
```cpp
OptimizationLevel::O3
```
- جميع تحسينات O2
- Register Coalescing (قريباً)
- Loop Optimizations (قريباً)
- **الاستخدام:** الأداء الأقصى

---

## 🔧 التمريرات المتاحة / Available Passes

### 1. Constant Folding Pass ✅

**الوصف / Description:**  
يقيّم العمليات على الثوابت في وقت الترجمة.  
Evaluates operations on constants at compile-time.

**الأمثلة / Examples:**
```
Before:  %1 = add 2, 3
         %2 = mul %1, 4
After:   %1 = 5        (2 + 3 → 5)
         %2 = 20       (5 * 4 → 20)
```

**القدرات / Capabilities:**
- ✅ العمليات الحسابية: `+, -, *, /, %`
- ✅ العمليات المنطقية: `&, |, ^, ~, <<, >>`
- ✅ المقارنات: `==, !=, <, >, <=, >=`
- ✅ العمليات الأحادية: `-, !`
- ✅ فحص الفيض (Overflow detection)
- ✅ فحص القسمة على صفر

**الاستخدام / Usage:**
```cpp
auto pass = std::make_unique<ConstantFoldingPass>();
bool modified = pass->runOnFunction(function);
```

---

### 2. Dead Code Elimination Pass ✅

**الوصف / Description:**  
يزيل التعليمات والكتل التي لا تؤثر على نتيجة البرنامج.  
Removes instructions and blocks that don't affect program output.

**الأمثلة / Examples:**
```
Before:  %1 = add 2, 3     (unused)
         %2 = mul 4, 5
         ret %2
After:   %2 = mul 4, 5
         ret %2
```

**القدرات / Capabilities:**
- ✅ تحليل الحيوية (Liveness Analysis)
- ✅ حذف التعليمات غير المستخدمة
- ✅ حذف الكتل غير القابلة للوصول
- ✅ الحفاظ على التعليمات ذات التأثيرات الجانبية
- ✅ خوارزمية BFS لتحديد الكتل القابلة للوصول

**الخوارزمية / Algorithm:**
1. وضع علامة على جميع التعليمات ذات التأثيرات الجانبية
2. انتشار عكسي من هذه التعليمات
3. وضع علامة على جميع التعليمات المُعتمد عليها
4. حذف التعليمات غير المعلمة

**الاستخدام / Usage:**
```cpp
auto pass = std::make_unique<DeadCodeEliminationPass>();
bool modified = pass->runOnFunction(function);
```

---

### 3. Common Subexpression Elimination (CSE) ⏳

**الحالة / Status:** قيد التطوير / In Development

**الوصف / Description:**  
يزيل الحسابات المكررة عن طريق إعادة استخدام النتائج.  
Eliminates redundant computations by reusing results.

---

### 4. Copy Propagation ⏳

**الحالة / Status:** قيد التطوير / In Development

**الوصف / Description:**  
يستبدل النسخ بالقيم الأصلية.  
Replaces copies with original values.

---

## 📚 API

### Optimizer Class

```cpp
class Optimizer {
public:
    // Constructor / البناء
    explicit Optimizer(OptimizationLevel level = OptimizationLevel::O2);
    
    // تحسين وحدة / Optimize module
    bool optimize(std::shared_ptr<SIR::SIRModule> module);
    
    // تحسين دالة / Optimize function
    bool optimizeFunction(SIR::SIRFunction* function);
    
    // إضافة تمرير / Add pass
    void addPass(std::unique_ptr<OptimizationPass> pass);
    
    // الحصول على الإحصائيات / Get statistics
    const OptimizationStats& getStats() const;
    
    // طباعة الإحصائيات / Print statistics
    void printStats() const;
    
    // التحقق / Verify
    bool verify(std::shared_ptr<SIR::SIRModule> module) const;
};
```

### OptimizationStats Structure

```cpp
struct OptimizationStats {
    int originalInstructionCount;      // عدد التعليمات الأصلية
    int optimizedInstructionCount;     // عدد التعليمات المحسّنة
    int instructionsRemoved;           // عدد التعليمات المحذوفة
    double optimizationTimeMs;         // وقت التحسين (ميلي ثانية)
    std::vector<PassStats> passStats;  // إحصائيات كل تمرير
    
    // حساب نسبة التحسين / Calculate improvement percentage
    double getImprovementPercentage() const;
};
```

### PassManager Class

```cpp
class PassManager {
public:
    // إضافة تمرير / Add pass
    void addPass(std::unique_ptr<OptimizationPass> pass);
    
    // تشغيل على دالة / Run on function
    bool runOnFunction(SIR::SIRFunction* function);
    
    // تعيين الحد الأقصى للتكرارات / Set max iterations
    void setMaxIterations(int maxIter);
    
    // تعيين وضع التكرار / Set iterative mode
    void setIterative(bool iterative);
    
    // طباعة معلومات التمريرات / Print pass info
    void printPassInfo() const;
};
```

---

## 💡 الأمثلة / Examples

### مثال 1: تحسين بسيط / Simple Optimization

```cpp
#include "optimizer/include/optimizer.h"
#include "sir/include/sir_module.h"

int main() {
    // تحميل وحدة SIR / Load SIR module
    auto module = loadSIRModule("program.sir");
    
    // إنشاء محسن / Create optimizer
    auto optimizer = createOptimizer(OptimizationLevel::O2);
    
    // تحسين / Optimize
    bool modified = optimizer->optimize(module);
    
    if (modified) {
        // طباعة الإحصائيات / Print stats
        optimizer->printStats();
        
        // حفظ الوحدة المحسّنة / Save optimized module
        saveSIRModule(module, "program_optimized.sir");
    }
    
    return 0;
}
```

**الإخراج / Output:**
```
╔════════════════════════════════════════════════════╗
║  Optimization Statistics / إحصائيات التحسين       ║
╠════════════════════════════════════════════════════╣
║  Original Instructions:   150                      ║
║  Optimized Instructions:  105                      ║
║  Instructions Removed:    45                       ║
║  Improvement:             30.00%                   ║
║  Optimization Time:       12.5 ms                  ║
╚════════════════════════════════════════════════════╝
```

### مثال 2: محسن مخصص / Custom Optimizer

```cpp
// إنشاء محسن فارغ / Create empty optimizer
auto optimizer = std::make_unique<Optimizer>(OptimizationLevel::O0);

// إضافة تمريرات بالترتيب المطلوب / Add passes in desired order
optimizer->addPass(std::make_unique<ConstantFoldingPass>());
optimizer->addPass(std::make_unique<MyCustomPass>());
optimizer->addPass(std::make_unique<DeadCodeEliminationPass>());

// تحسين / Optimize
optimizer->optimize(module);
```

### مثال 3: تحسين دالة واحدة / Single Function Optimization

```cpp
// الحصول على دالة من الوحدة / Get function from module
auto* function = module->getFunction("main");

// إنشاء محسن / Create optimizer
auto optimizer = std::make_unique<Optimizer>(OptimizationLevel::O1);

// تحسين الدالة فقط / Optimize function only
bool modified = optimizer->optimizeFunction(function);

// الحصول على الإحصائيات / Get statistics
auto stats = optimizer->getStats();
std::cout << "Removed " << stats.instructionsRemoved 
          << " instructions" << std::endl;
```

---

## 🔨 البناء / Building

### المتطلبات / Requirements

- C++17 أو أحدث
- CMake 3.15+
- MSVC (Windows) أو GCC/Clang (Linux/Mac)

### خطوات البناء / Build Steps

```bash
# إنشاء مجلد البناء / Create build directory
mkdir build
cd build

# تكوين / Configure
cmake ..

# بناء / Build
cmake --build . --config Release

# أو استخدم البرنامج النصي / Or use the script
.\build_interpreter.bat  # Windows
```

### التكامل / Integration

أضف إلى `CMakeLists.txt`:

```cmake
# إضافة مكتبة المحسن / Add optimizer library
add_subdirectory(compiler/optimizer)

# ربط مع المشروع / Link with your project
target_link_libraries(your_target PRIVATE sad_optimizer)
```

---

## 🧪 الاختبار / Testing

### تشغيل الاختبارات / Running Tests

```bash
# بناء الاختبارات / Build tests
cmake --build . --target tests

# تشغيل / Run
ctest --output-on-failure

# أو مباشرة / Or directly
.\build\tests\test_optimizer.exe
```

### كتابة اختبارات / Writing Tests

```cpp
#include "gtest/gtest.h"
#include "optimizer/include/optimizer.h"

TEST(OptimizerTest, ConstantFolding) {
    // إنشاء دالة اختبار / Create test function
    auto func = createTestFunction();
    
    // تطبيق التحسين / Apply optimization
    auto pass = std::make_unique<ConstantFoldingPass>();
    bool modified = pass->runOnFunction(func.get());
    
    // التحقق / Verify
    EXPECT_TRUE(modified);
    EXPECT_EQ(func->getInstructionCount(), expectedCount);
}
```

---

## 📊 الإحصائيات / Statistics

### كود المشروع / Project Code

```
الترويسة / Headers:          700 lines (5 files)
التطبيق / Implementation:   1,480 lines (5 files)
الاختبارات / Tests:            0 lines (قريباً)
التوثيق / Documentation:    2,000+ lines
──────────────────────────────────────────────
المجموع / Total:           4,180+ lines
```

### التقدم / Progress

```
Overall: ████████░░░░░░░░░░░░ 25%

✅ Planning         100%
✅ Architecture     100%
✅ Headers          100%
✅ Core Impl        100%
✅ Pass Impl        100%
⏳ Testing            0%
⏳ Documentation     50%
```

---

## 🗺️ خارطة الطريق / Roadmap

### الأسبوع 4 (Dec 3-10, 2025)
- ✅ التخطيط والتصميم
- ✅ التطبيق الأساسي
- ✅ Constant Folding
- ✅ Dead Code Elimination

### الأسبوع 5 (Dec 11-17, 2025)
- ⏳ الاختبارات الشاملة
- ⏳ Common Subexpression Elimination
- ⏳ Copy Propagation
- ⏳ Register Coalescing
- ⏳ التوثيق الكامل

---

## 👥 المساهمة / Contributing

نرحب بالمساهمات! يرجى:
1. Fork المشروع
2. إنشاء فرع للميزة (`git checkout -b feature/AmazingFeature`)
3. Commit التغييرات (`git commit -m 'Add AmazingFeature'`)
4. Push إلى الفرع (`git push origin feature/AmazingFeature`)
5. فتح Pull Request

---

## 📝 الترخيص / License

هذا المشروع مرخص تحت رخصة MIT. انظر ملف [LICENSE](../../../LICENSE) للتفاصيل.

---

## 📞 الاتصال / Contact

**المشروع / Project:** SadLanguage Compiler  
**الفريق / Team:** SadLanguage Compiler Team  
**المرحلة / Phase:** Phase 2 - SIR Optimizer  
**الحالة / Status:** 🟢 In Active Development

---

## 🔗 روابط مفيدة / Useful Links

- [خطة المرحلة 2](../../docs/compiler_plan/status/phase2/week4_5_optimizer_plan.md)
- [تقرير التقدم](../../docs/compiler_plan/status/phase2/week4_5_optimizer_progress.md)
- [تقرير التطبيق الأساسي](../../docs/compiler_plan/status/phase2/CORE_IMPLEMENTATION_REPORT.md)
- [المرحلة 1: SIR Builder](../../docs/compiler_plan/status/phase1/)

---

**آخر تحديث / Last Updated:** December 3, 2025  
**الإصدار / Version:** 1.0.0-alpha  
**التقدم / Progress:** 25% → 100% (Target: Dec 17, 2025)

🚀 **Happy Optimizing! / تحسين سعيد!** 🚀
