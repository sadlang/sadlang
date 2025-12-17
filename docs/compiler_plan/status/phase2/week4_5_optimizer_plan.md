# المرحلة 2: SIR Optimizer - خطة التنفيذ
# Phase 2: SIR Optimizer - Implementation Plan

**التاريخ / Date:** 3-17 ديسمبر 2025  
**الحالة / Status:** 🔵 Planning - التخطيط  
**المدة / Duration:** أسبوعان / 2 Weeks

**تاريخ البدء المخطط / Planned Start:** December 4, 2025  
**تاريخ الإكمال المستهدف / Target Completion:** December 17, 2025

---

## 📋 نظرة عامة / Overview

### الهدف / Objective

تطوير محسّن SIR الذي يحول التمثيل الوسيط غير المحسّن إلى كود محسّن وفعّال.

**Develop SIR Optimizer that transforms unoptimized intermediate representation into efficient optimized code.**

### المخرجات المتوقعة / Expected Outputs

1. **Optimizer Engine** - محرك التحسين الرئيسي
2. **5+ Optimization Passes** - 5+ تمريرات تحسين
3. **Performance Improvement** - تحسين أداء 20-30%
4. **Comprehensive Tests** - اختبارات شاملة (50+ tests)
5. **Documentation** - توثيق كامل

---

## 🏗️ المعمارية / Architecture

### الهيكل العام / Overall Structure

```
SIR Optimizer
├── Optimizer Engine (محرك التحسين)
│   ├── Pass Manager (مدير التمريرات)
│   ├── Analysis Manager (مدير التحليلات)
│   └── Statistics Collector (جامع الإحصائيات)
│
├── Analysis Passes (تمريرات التحليل)
│   ├── Control Flow Analysis (تحليل التدفق)
│   ├── Data Flow Analysis (تحليل البيانات)
│   ├── Use-Def Chains (سلاسل الاستخدام-التعريف)
│   └── Liveness Analysis (تحليل الحيوية)
│
├── Transformation Passes (تمريرات التحويل)
│   ├── Constant Folding (طي الثوابت)
│   ├── Dead Code Elimination (إزالة الكود الميت)
│   ├── Common Subexpression Elimination (إزالة التعابير المكررة)
│   ├── Copy Propagation (نشر النسخ)
│   └── Register Coalescing (دمج السجلات)
│
└── Validation (التحقق)
    ├── Pre-optimization Validation
    ├── Post-optimization Validation
    └── Semantic Preservation Check
```

---

## 📝 المكونات التفصيلية / Detailed Components

### 1. Optimizer Engine | محرك التحسين

**المسؤوليات / Responsibilities:**
- إدارة تمريرات التحسين / Manage optimization passes
- جدولة التنفيذ / Schedule execution
- جمع الإحصائيات / Collect statistics
- التحقق من الصحة / Validate correctness

**الملفات / Files:**
```
compiler/optimizer/
├── include/
│   ├── optimizer.h          // الواجهة الرئيسية / Main interface
│   ├── pass.h               // صنف التمرير الأساسي / Base pass class
│   └── pass_manager.h       // مدير التمريرات / Pass manager
└── src/
    ├── optimizer.cpp
    ├── pass.cpp
    └── pass_manager.cpp
```

**الواجهة العامة / Public API:**
```cpp
/**
 * @brief محسّن التمثيل الوسيط SIR
 * @brief SIR Intermediate Representation Optimizer
 * 
 * يطبق تمريرات متعددة من التحسينات على وحدة SIR
 * Applies multiple optimization passes to SIR module
 */
class Optimizer {
public:
    /**
     * @brief Constructor
     * @param level مستوى التحسين (0-3) / Optimization level (0-3)
     */
    explicit Optimizer(int level = 2);
    
    /**
     * @brief تحسين وحدة SIR كاملة
     * @brief Optimize complete SIR module
     * 
     * @param module الوحدة المراد تحسينها / Module to optimize
     * @return true إذا نجح التحسين / if optimization succeeded
     */
    bool optimize(std::shared_ptr<SIRModule> module);
    
    /**
     * @brief إضافة تمرير تحسين مخصص
     * @brief Add custom optimization pass
     */
    void addPass(std::unique_ptr<OptimizationPass> pass);
    
    /**
     * @brief الحصول على إحصائيات التحسين
     * @brief Get optimization statistics
     */
    OptimizationStats getStats() const;
    
private:
    int optimizationLevel_;
    PassManager passManager_;
    AnalysisManager analysisManager_;
    StatisticsCollector stats_;
};
```

---

### 2. Constant Folding | طي الثوابت

**الهدف / Goal:**  
تقييم التعابير الثابتة في وقت الترجمة بدلاً من وقت التشغيل.

**Evaluate constant expressions at compile-time instead of runtime.**

**مثال / Example:**
```
قبل التحسين / Before:
    %0 = LOAD_CONST 10
    %1 = LOAD_CONST 20
    %2 = ADD %0, %1        // 10 + 20
    %3 = MUL %2, 2         // 30 * 2

بعد التحسين / After:
    %3 = LOAD_CONST 60     // Computed at compile-time
```

**التطبيق / Implementation:**
```cpp
/**
 * @brief تمرير طي الثوابت
 * @brief Constant Folding Pass
 * 
 * يقيّم العمليات على الثوابت في وقت الترجمة
 * Evaluates operations on constants at compile-time
 */
class ConstantFoldingPass : public OptimizationPass {
public:
    bool runOnFunction(SIRFunction* function) override;
    
private:
    /**
     * @brief طي عملية ثنائية
     * @brief Fold binary operation
     */
    std::optional<int64_t> foldBinary(
        SIROpcode op,
        int64_t left,
        int64_t right
    );
    
    /**
     * @brief طي عملية أحادية
     * @brief Fold unary operation
     */
    std::optional<int64_t> foldUnary(
        SIROpcode op,
        int64_t operand
    );
    
    /**
     * @brief التحقق من كون القيمة ثابتة
     * @brief Check if value is constant
     */
    bool isConstant(const std::string& reg);
    
    /**
     * @brief الحصول على قيمة الثابت
     * @brief Get constant value
     */
    std::optional<int64_t> getConstantValue(const std::string& reg);
};
```

**العمليات المدعومة / Supported Operations:**
- ✅ Arithmetic: `+`, `-`, `*`, `/`, `%`
- ✅ Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>`
- ✅ Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- ✅ Logical: `&&`, `||`, `!`
- ✅ Type conversions: `I64_TO_F64`, `F64_TO_I64`

---

### 3. Dead Code Elimination | إزالة الكود الميت

**الهدف / Goal:**  
حذف التعليمات غير المستخدمة والكود غير القابل للوصول.

**Remove unused instructions and unreachable code.**

**أنواع الكود الميت / Types of Dead Code:**

1. **Unused Variables** - متغيرات غير مستخدمة
```
%x = ADD %a, %b    // %x never used → DELETE
%y = MUL %c, %d
RET %y
```

2. **Unreachable Blocks** - كتل غير قابلة للوصول
```
entry:
    RET %0

dead_block:        // Unreachable → DELETE
    %1 = ADD %a, %b
    RET %1
```

3. **Dead Stores** - تخزينات ميتة
```
STORE %x, 10      // Overwritten → DELETE
STORE %x, 20      // This one stays
```

**التطبيق / Implementation:**
```cpp
/**
 * @brief تمرير إزالة الكود الميت
 * @brief Dead Code Elimination Pass
 * 
 * يحذف التعليمات والكتل غير المستخدمة
 * Removes unused instructions and blocks
 */
class DeadCodeEliminationPass : public OptimizationPass {
public:
    bool runOnFunction(SIRFunction* function) override;
    
private:
    /**
     * @brief حساب التعليمات الحية
     * @brief Compute live instructions
     */
    std::unordered_set<SIRInstruction*> computeLiveInstructions(
        SIRFunction* function
    );
    
    /**
     * @brief إزالة الكتل غير القابلة للوصول
     * @brief Remove unreachable blocks
     */
    void removeUnreachableBlocks(SIRFunction* function);
    
    /**
     * @brief إزالة التعليمات الميتة
     * @brief Remove dead instructions
     */
    void removeDeadInstructions(
        SIRFunction* function,
        const std::unordered_set<SIRInstruction*>& live
    );
};
```

---

### 4. Common Subexpression Elimination (CSE) | إزالة التعابير المكررة

**الهدف / Goal:**  
إعادة استخدام نتائج الحسابات المكررة بدلاً من إعادة الحساب.

**Reuse results of duplicate computations instead of recomputing.**

**مثال / Example:**
```
قبل التحسين / Before:
    %0 = ADD %a, %b
    %1 = MUL %0, 2
    %2 = ADD %a, %b      // Duplicate!
    %3 = MUL %2, 3

بعد التحسين / After:
    %0 = ADD %a, %b
    %1 = MUL %0, 2
    %3 = MUL %0, 3       // Reuse %0
```

**التطبيق / Implementation:**
```cpp
/**
 * @brief تمرير إزالة التعابير المشتركة
 * @brief Common Subexpression Elimination Pass
 * 
 * يحدد ويزيل الحسابات المكررة
 * Identifies and eliminates duplicate computations
 */
class CSEPass : public OptimizationPass {
public:
    bool runOnFunction(SIRFunction* function) override;
    
private:
    /**
     * @brief بصمة التعبير للمقارنة
     * @brief Expression fingerprint for comparison
     */
    struct ExpressionKey {
        SIROpcode opcode;
        std::vector<std::string> operands;
        
        bool operator==(const ExpressionKey& other) const;
        size_t hash() const;
    };
    
    /**
     * @brief جدول التعابير المتاحة
     * @brief Available expressions table
     */
    std::unordered_map<ExpressionKey, std::string> availableExprs_;
    
    /**
     * @brief إنشاء مفتاح من تعليمة
     * @brief Create key from instruction
     */
    std::optional<ExpressionKey> getExpressionKey(SIRInstruction* inst);
};
```

---

### 5. Copy Propagation | نشر النسخ

**الهدف / Goal:**  
استبدال استخدامات المتغيرات بقيمها الأصلية عندما تكون مجرد نسخ.

**Replace variable uses with their original values when they're just copies.**

**مثال / Example:**
```
قبل التحسين / Before:
    %0 = LOAD %x
    %1 = %0          // Copy
    %2 = ADD %1, 10  // Use copy

بعد التحسين / After:
    %0 = LOAD %x
    %2 = ADD %0, 10  // Use original
```

**التطبيق / Implementation:**
```cpp
/**
 * @brief تمرير نشر النسخ
 * @brief Copy Propagation Pass
 * 
 * يستبدل استخدامات النسخ بالقيم الأصلية
 * Replaces copy uses with original values
 */
class CopyPropagationPass : public OptimizationPass {
public:
    bool runOnFunction(SIRFunction* function) override;
    
private:
    /**
     * @brief خريطة النسخ المتاحة
     * @brief Available copies map
     */
    std::unordered_map<std::string, std::string> copyMap_;
    
    /**
     * @brief التحقق من كون التعليمة نسخة بسيطة
     * @brief Check if instruction is simple copy
     */
    bool isSimpleCopy(SIRInstruction* inst);
    
    /**
     * @brief استبدال الاستخدامات في التعليمة
     * @brief Replace uses in instruction
     */
    void replaceUses(SIRInstruction* inst);
};
```

---

### 6. Register Coalescing | دمج السجلات

**الهدف / Goal:**  
تقليل عدد السجلات الافتراضية من خلال دمج السجلات المتوافقة.

**Reduce number of virtual registers by merging compatible registers.**

**مثال / Example:**
```
قبل التحسين / Before:
    %0 = LOAD_CONST 10
    %1 = %0
    %2 = %1
    %3 = ADD %2, 5
    // Uses registers: %0, %1, %2, %3

بعد التحسين / After:
    %0 = LOAD_CONST 10
    %3 = ADD %0, 5
    // Uses registers: %0, %3
```

**التطبيق / Implementation:**
```cpp
/**
 * @brief تمرير دمج السجلات
 * @brief Register Coalescing Pass
 * 
 * يدمج السجلات المتوافقة لتقليل استخدام الذاكرة
 * Merges compatible registers to reduce memory usage
 */
class RegisterCoalescingPass : public OptimizationPass {
public:
    bool runOnFunction(SIRFunction* function) override;
    
private:
    /**
     * @brief بناء رسم التداخل
     * @brief Build interference graph
     */
    InterferenceGraph buildInterferenceGraph(SIRFunction* function);
    
    /**
     * @brief إيجاد السجلات القابلة للدمج
     * @brief Find coalescable registers
     */
    std::vector<std::pair<std::string, std::string>> findCoalescablePairs(
        const InterferenceGraph& graph
    );
    
    /**
     * @brief دمج زوج من السجلات
     * @brief Coalesce register pair
     */
    void coalesceRegisters(
        SIRFunction* function,
        const std::string& src,
        const std::string& dst
    );
};
```

---

## 📊 مستويات التحسين / Optimization Levels

### Level 0: No Optimization
- لا توجد تحسينات / No optimizations
- للتصحيح / For debugging

### Level 1: Basic (-O1)
- ✅ Constant Folding
- ✅ Dead Code Elimination (basic)
- الوقت: سريع جداً / Time: Very fast
- التحسين: 10-15%

### Level 2: Standard (-O2) - Default
- ✅ All Level 1
- ✅ Common Subexpression Elimination
- ✅ Copy Propagation
- ✅ Dead Code Elimination (aggressive)
- الوقت: متوسط / Time: Medium
- التحسين: 20-30%

### Level 3: Aggressive (-O3)
- ✅ All Level 2
- ✅ Register Coalescing
- ✅ Loop optimizations (future)
- ✅ Function inlining (future)
- الوقت: بطيء / Time: Slow
- التحسين: 30-40%

---

## 🧪 استراتيجية الاختبار / Testing Strategy

### 1. Unit Tests - اختبارات الوحدة (30+ tests)

**لكل تمرير / Per Pass:**
```cpp
TEST_CASE("Constant Folding - Arithmetic") {
    // Test: 10 + 20 → 30
    // Test: 30 * 2 → 60
    // Test: 100 / 4 → 25
}

TEST_CASE("Dead Code Elimination - Unused Variables") {
    // Test: Remove unused %x
    // Test: Keep used %y
}

TEST_CASE("CSE - Basic") {
    // Test: Eliminate duplicate a+b
}
```

### 2. Integration Tests - اختبارات التكامل (20+ tests)

**برامج كاملة / Complete Programs:**
```cpp
TEST_CASE("Optimize Complete Function") {
    const char* sir = R"(
        function @test() {
        entry:
            %0 = LOAD_CONST 10
            %1 = LOAD_CONST 20
            %2 = ADD %0, %1
            %3 = MUL %2, 2
            RET %3
        }
    )";
    
    Optimizer opt(2);
    auto optimized = opt.optimize(parseSIR(sir));
    
    // Should reduce to:
    // RET 60
}
```

### 3. Performance Tests - اختبارات الأداء

**قياس التحسين / Measure Improvement:**
```cpp
TEST_CASE("Performance - Fibonacci") {
    auto original = generateFibonacciSIR(20);
    auto optimized = optimize(original);
    
    // Count instructions
    REQUIRE(optimized->instructionCount() < 
            original->instructionCount() * 0.7);
}
```

### 4. Regression Tests - اختبارات الانحدار

**التحقق من عدم كسر الوظائف / Verify No Functionality Broken:**
```cpp
TEST_CASE("Regression - All Phase 1 Tests") {
    // Run all 32 Phase 1 tests
    // Verify same results after optimization
}
```

---

## 📈 معايير النجاح / Success Criteria

### المتطلبات الإلزامية / Mandatory Requirements

- ✅ **5+ Optimization Passes** تعمل بشكل صحيح
- ✅ **50+ Tests** جميعها تمر بنجاح (100% pass rate)
- ✅ **Code Coverage** أكثر من 80%
- ✅ **Performance** تحسين 20-30% على الأقل
- ✅ **Semantic Preservation** الحفاظ على المعنى البرمجي
- ✅ **Documentation** توثيق كامل لكل تمرير

### المتطلبات الاختيارية / Optional Requirements

- 🔲 Register count reduction by 40%+
- 🔲 Support for -O0, -O1, -O2, -O3 levels
- 🔲 Statistics reporting (what was optimized)
- 🔲 Optimization visualization tool

---

## 📅 الجدول الزمني / Timeline

### Week 4 (Dec 4-10, 2025)

**Day 1-2: Setup & Architecture**
- ✅ Create optimizer infrastructure
- ✅ Implement Optimizer class
- ✅ Implement PassManager
- ✅ Setup test framework

**Day 3-4: Constant Folding**
- ✅ Implement constant folding
- ✅ Support all arithmetic operations
- ✅ Add 10+ tests

**Day 5-6: Dead Code Elimination**
- ✅ Implement DCE
- ✅ Remove unused variables
- ✅ Remove unreachable blocks
- ✅ Add 10+ tests

**Day 7: Week 4 Review**
- ✅ Review progress
- ✅ Fix bugs
- ✅ Update documentation

### Week 5 (Dec 11-17, 2025)

**Day 1-2: Common Subexpression Elimination**
- ✅ Implement CSE
- ✅ Expression hashing
- ✅ Add 10+ tests

**Day 3-4: Copy Propagation**
- ✅ Implement copy propagation
- ✅ Handle complex cases
- ✅ Add 10+ tests

**Day 5: Register Coalescing**
- ✅ Implement basic coalescing
- ✅ Add 5+ tests

**Day 6: Integration & Testing**
- ✅ Integration tests
- ✅ Performance benchmarks
- ✅ Regression tests

**Day 7: Final Review & Documentation**
- ✅ Complete API documentation
- ✅ Write user guide
- ✅ Create examples
- ✅ Final report

---

## 📝 Deliverables | المخرجات

### Code Files
```
compiler/optimizer/
├── include/
│   ├── optimizer.h
│   ├── pass.h
│   ├── pass_manager.h
│   ├── constant_folding_pass.h
│   ├── dead_code_elimination_pass.h
│   ├── cse_pass.h
│   ├── copy_propagation_pass.h
│   └── register_coalescing_pass.h
│
├── src/
│   ├── optimizer.cpp
│   ├── pass.cpp
│   ├── pass_manager.cpp
│   ├── constant_folding_pass.cpp
│   ├── dead_code_elimination_pass.cpp
│   ├── cse_pass.cpp
│   ├── copy_propagation_pass.cpp
│   └── register_coalescing_pass.cpp
│
└── tests/
    ├── test_constant_folding.cpp
    ├── test_dce.cpp
    ├── test_cse.cpp
    ├── test_copy_propagation.cpp
    ├── test_register_coalescing.cpp
    └── test_optimizer_integration.cpp
```

### Documentation Files
```
docs/compiler_plan/status/phase2/
├── week4_5_optimizer_plan.md (this file)
├── week4_5_optimizer_progress.md
├── optimizer_api_reference.md
├── optimization_passes_guide.md
└── PHASE2_COMPLETION_REPORT.md
```

---

## 🔗 المراجع / References

- [Phase 1: SIR Builder](../phase1/week2_3_sirbuilder.md)
- [SIR Specification](../../05_phase2_middleend.md)
- [Compiler Design Literature](../../13_technical_refs.md)

---

**الحالة / Status:** 🔵 Planning Complete  
**التالي / Next:** Implementation Week 4  
**المسؤول / Owner:** Compiler Team  
**آخر تحديث / Last Updated:** December 3, 2025
