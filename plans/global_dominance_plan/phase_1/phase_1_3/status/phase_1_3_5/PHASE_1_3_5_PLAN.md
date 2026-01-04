# 🎯 Phase 1.3.5: Union Types & Type Guards
# المرحلة 1.3.5: أنواع الاتحاد وحراس الأنواع

**تاريخ البدء / Start Date:** 3 يناير 2026  
**الحالة / Status:** 🟢 جاهز للبدء / Ready to Start  
**المدة المتوقعة / Expected Duration:** 5-7 أيام  
**الأولوية / Priority:** 🟡 متوسطة / Medium  
**المتطلبات / Prerequisites:** ✅ Phase 1.3.0-1.3.4 مكتملة

---

## 📊 نظرة عامة / Overview

### الهدف الاستراتيجي / Strategic Goal

تطوير دعم **Union Types** (أنواع الاتحاد) و **Type Guards** (حراس الأنواع) لتمكين:
- ✅ التعامل مع قيم متعددة الأنواع بأمان
- ✅ Pattern matching مع الأنواع
- ✅ Type narrowing تلقائي في control flow
- ✅ Optional types (`T?`) كـ syntactic sugar

Develop **Union Types** and **Type Guards** support to enable:
- ✅ Safe handling of multi-typed values
- ✅ Pattern matching with types
- ✅ Automatic type narrowing in control flow
- ✅ Optional types (`T?`) as syntactic sugar

---

## 🏗️ البنية المعمارية / Architecture

### المكونات الرئيسية / Main Components

```
┌─────────────────────────────────────────────────────────────┐
│           Phase 1.3.5: Union Types & Guards                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  1. Union Types                                       │ │
│  │                                                       │ │
│  │  UnionType:                                          │ │
│  │    ├─ alternatives: [Type]                          │ │
│  │    ├─ contains(Type): bool                          │ │
│  │    ├─ intersection(UnionType): UnionType           │ │
│  │    └─ simplify(): UnionType                        │ │
│  │                                                       │ │
│  │  OptionalType (syntactic sugar):                    │ │
│  │    T? = T | null                                    │ │
│  └───────────────────────────────────────────────────────┘ │
│                          ↓                                   │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  2. Type Guards                                       │ │
│  │                                                       │ │
│  │  TypeGuard:                                          │ │
│  │    ├─ checkType(Expr, Type): bool                   │ │
│  │    ├─ narrowType(Expr, Type): Type                  │ │
│  │    └─ generateGuardIR(): LLVM IR                    │ │
│  │                                                       │ │
│  │  Syntax:                                             │ │
│  │    - value نوع Type                                 │ │
│  │    - value is Type                                   │ │
│  └───────────────────────────────────────────────────────┘ │
│                          ↓                                   │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  3. Type Narrowing                                    │ │
│  │                                                       │ │
│  │  TypeNarrowing:                                      │ │
│  │    ├─ control flow analysis                         │ │
│  │    ├─ type refinement per branch                    │ │
│  │    ├─ exhaustiveness checking                       │ │
│  │    └─ dead code detection                           │ │
│  └───────────────────────────────────────────────────────┘ │
│                          ↓                                   │
│  ┌───────────────────────────────────────────────────────┐ │
│  │  4. Pattern Matching Integration                      │ │
│  │                                                       │ │
│  │  PatternTypeChecker:                                 │ │
│  │    ├─ match expression type checking                │ │
│  │    ├─ pattern type extraction                       │ │
│  │    └─ exhaustiveness verification                   │ │
│  └───────────────────────────────────────────────────────┘ │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 🎯 Sub-Phases التفصيلية / Detailed Sub-Phases

### 📦 Phase 1.3.5.0: Union Type Core (يومان / 2 days)

**الأهداف / Objectives:**

#### 1. Union Type Definition
```cpp
// union_type.h
class UnionType : public Type {
private:
    TypeList alternatives_;  // الأنواع البديلة / Alternative types
    
public:
    // المُنشئات / Constructors
    UnionType(const TypeList& alternatives);
    
    // الدوال الأساسية / Core methods
    bool contains(const TypePtr& type) const;
    bool isSubtypeOf(const Type* other) const override;
    UnionTypePtr simplify() const;  // إزالة التكرار / Remove duplicates
    
    // Intersection & difference
    UnionTypePtr intersection(const UnionTypePtr& other) const;
    UnionTypePtr difference(const TypePtr& type) const;
    
    // Getters
    const TypeList& getAlternatives() const;
    size_t size() const;
};
```

#### 2. Basic Union Operations
```s
# ✅ Union type declaration
متغير نتيجة: رقم | نص | خطأ

# ✅ Optional type (syntactic sugar)
متغير البريد: نص? = لاشيء  # نص | لاشيء

# ✅ Assignment
نتيجة = 42          # OK: رقم ⊂ (رقم | نص | خطأ)
نتيجة = "success"   # OK: نص ⊂ (رقم | نص | خطأ)
نتيجة = خطأ("fail") # OK: خطأ ⊂ (رقم | نص | خطأ)
```

#### الملفات المطلوبة / Required Files:
```
compiler/type_system/include/
├── union_type.h           (350 lines)
└── optional_type.h        (150 lines)

compiler/type_system/src/
├── union_type.cpp         (500 lines)
└── optional_type.cpp      (200 lines)
```

**Deliverables:**
- ✅ UnionType class كامل
- ✅ OptionalType (T?) support
- ✅ Union type simplification
- ✅ Type containment checking

---

### 🛡️ Phase 1.3.5.1: Type Guards (يومان / 2 days)

**الأهداف / Objectives:**

#### 1. Type Guard Syntax
```s
# ✅ Syntax 1: value نوع Type
إذا (نتيجة نوع رقم) 
    # هنا نتيجة: رقم / Here result: Integer
    اطبع(نتيجة * 2)
نهاية

# ✅ Negative guards
إذا (!(نتيجة نوع رقم)) 
    # هنا نتيجة: نص | خطأ (excluding رقم)
نهاية
```

#### 2. Type Guard Implementation
```cpp
// type_guard.h
class TypeGuard {
public:
    // Type checking
    static bool checkType(const ExprPtr& expr, 
                          const TypePtr& expectedType);
    
    // Type narrowing
    static TypePtr narrowType(const TypePtr& originalType,
                              const TypePtr& guardedType,
                              bool isPositive);
    
    // Control flow integration
    static void analyzeGuard(const IfStmt* stmt,
                            TypeEnvironment& env);
};
```

#### الملفات المطلوبة / Required Files:
```
compiler/type_system/include/
├── type_guard.h           (280 lines)
└── type_guard_visitor.h   (200 lines)

compiler/type_system/src/
├── type_guard.cpp         (450 lines)
└── type_guard_visitor.cpp (350 lines)
```

**Deliverables:**
- ✅ Type guard syntax parsing
- ✅ Type guard evaluation
- ✅ Runtime type checking
- ✅ Type narrowing في branches

---

### 🔍 Phase 1.3.5.2: Type Narrowing (يومان / 2 days)

**الأهداف / Objectives:**

#### 1. Control Flow Analysis
```s
# ✅ Type narrowing في if branches
دالة معالجة(قيمة: رقم | نص) 
    إذا (قيمة نوع رقم) 
        # Branch 1: قيمة: رقم
        اطبع(قيمة + 10)
     وإلا 
        # Branch 2: قيمة: نص (narrowed from union)
        اطبع(قيمة.طول())
    نهاية
نهاية

# ✅ Sequential narrowing
دالة تحقق(قيمة: رقم | نص | منطقي) 
    إذا (قيمة نوع رقم) 
        إرجاع قيمة * 2
    نهاية
    
    # هنا قيمة: نص | منطقي (رقم excluded)
    
    إذا (قيمة نوع نص) 
        إرجاع قيمة.طول()
    نهاية
    
    # هنا قيمة: منطقي (only option left)
    إرجاع قيمة ? 1 : 0
نهاية
```

#### 2. Type Narrowing Engine
```cpp
// type_narrowing.h
class TypeNarrowing {
private:
    // Control flow graph
    std::unordered_map<BasicBlock*, TypeEnvironment> blockTypes_;
    
public:
    // Analyze control flow
    void analyzeFunction(const FunctionDecl* func);
    
    // Narrow type in branch
    TypePtr narrowInBranch(const TypePtr& original,
                           const TypeGuard& guard,
                           bool isTrueBranch);
    
    // Check exhaustiveness
    bool isExhaustive(const UnionTypePtr& type,
                     const std::vector<TypeGuard>& guards);
    
    // Detect dead code
    std::vector<BasicBlock*> findDeadBlocks();
};
```

#### الملفات المطلوبة / Required Files:
```
compiler/type_system/include/
├── type_narrowing.h         (350 lines)
├── control_flow_analyzer.h  (280 lines)
└── exhaustiveness_checker.h (220 lines)

compiler/type_system/src/
├── type_narrowing.cpp         (550 lines)
├── control_flow_analyzer.cpp  (450 lines)
└── exhaustiveness_checker.cpp (380 lines)
```

**Deliverables:**
- ✅ Control flow analysis
- ✅ Type narrowing per branch
- ✅ Exhaustiveness checking
- ✅ Dead code detection

---

### 🎭 Phase 1.3.5.3: Pattern Matching Integration (1-2 أيام)

**الأهداف / Objectives:**

#### 1. Pattern Type Checking
```s
# ✅ Pattern matching مع union types
دالة معالجة_نتيجة(نتيجة: نجاح<T> | فشل<E>) 
    طابق نتيجة 
        نجاح(قيمة) => 
            # قيمة: T
            اطبع("نجح: " + قيمة)
        نهاية
        فشل(خطأ) => 
            # خطأ: E
            اطبع("فشل: " + خطأ)
        نهاية
    نهاية
نهاية

# ✅ Type extraction في patterns
طابق قيمة: رقم | نص | مصفوفة<رقم> 
    رقم(ع) => ع * 2
    نص(ن) => ن.طول()
    مصفوفة(م) => م.length
    _ => 0
نهاية
```

#### 2. Pattern Type Checker
```cpp
// pattern_type_checker.h
class PatternTypeChecker {
public:
    // Check match expression
    bool checkMatch(const MatchExpr* match);
    
    // Extract type from pattern
    TypePtr extractPatternType(const Pattern* pattern);
    
    // Verify exhaustiveness
    bool isExhaustive(const TypePtr& matchedType,
                     const std::vector<Pattern*>& patterns);
    
    // Check pattern compatibility
    bool isPatternCompatible(const Pattern* pattern,
                            const TypePtr& type);
};
```

#### الملفات المطلوبة / Required Files:
```
compiler/type_system/include/
└── pattern_type_checker.h  (320 lines)

compiler/type_system/src/
└── pattern_type_checker.cpp (500 lines)
```

**Deliverables:**
- ✅ Pattern type extraction
- ✅ Match exhaustiveness checking
- ✅ Pattern compatibility verification
- ✅ Type narrowing في match branches

---

### 🔧 Phase 1.3.5.4: LLVM Integration (1 يوم / 1 day)

**الأهداف / Objectives:**

#### 1. Tagged Union Representation
```cpp
// LLVM representation of union type:
struct UnionValue {
    i32 tag;           // نوع الـ tag / Tag type
    [max_size x i8] data;  // البيانات / Data
};

// Example: Integer | String | Error
// tag: 0 = Integer, 1 = String, 2 = Error
```

#### 2. Runtime Type Checking
```llvm
; Type guard IR generation
define i1 @check_type(ptr %union_value, i32 %expected_tag) {
    %tag_ptr = getelementptr %UnionValue, ptr %union_value, i32 0, i32 0
    %current_tag = load i32, ptr %tag_ptr
    %is_match = icmp eq i32 %current_tag, %expected_tag
    ret i1 %is_match
}
```

#### الملفات المطلوبة / Required Files:
```
compiler/backend/llvm/include/
├── union_type_codegen.h  (280 lines)
└── type_guard_codegen.h  (220 lines)

compiler/backend/llvm/src/
├── union_type_codegen.cpp  (450 lines)
└── type_guard_codegen.cpp  (350 lines)
```

**Deliverables:**
- ✅ Tagged union IR generation
- ✅ Type guard IR generation
- ✅ Efficient memory layout
- ✅ Runtime type checking

---

## 🧪 استراتيجية الاختبار / Testing Strategy

### Unit Tests

```cpp
// test_union_type.cpp
TEST(UnionType, Creation) {
    auto intType = PrimitiveType::getInteger();
    auto strType = PrimitiveType::getString();
    auto unionType = makeUnionType({intType, strType});
    
    EXPECT_EQ(unionType->size(), 2);
    EXPECT_TRUE(unionType->contains(intType));
    EXPECT_TRUE(unionType->contains(strType));
}

TEST(UnionType, Simplification) {
    auto intType = PrimitiveType::getInteger();
    auto unionType = makeUnionType({intType, intType, intType});
    
    auto simplified = unionType->simplify();
    EXPECT_EQ(simplified->size(), 1);
}

TEST(TypeGuard, BasicCheck) {
    auto guard = makeTypeGuard(expr, intType);
    EXPECT_TRUE(guard->evaluate());
}

TEST(TypeNarrowing, IfBranch) {
    auto unionType = makeUnionType({intType, strType});
    auto narrowed = narrower.narrowInBranch(unionType, intGuard, true);
    EXPECT_EQ(narrowed, intType);
}
```

### Integration Tests

```s
# test_union_integration.s

# Test 1: Basic union usage
دالة اختبار_union_بسيط() 
    متغير قيمة: رقم | نص = 42
    تحقق(قيمة نوع رقم)
    
    قيمة = "test"
    تحقق (قيمة نوع نص)
نهاية

# Test 2: Type narrowing
دالة اختبار_narrowing() 
    متغير x: رقم | نص | منطقي = 10
    
    إذا (x نوع رقم) 
        # x is Integer here
        تحقق(x * 2 == 20)
    نهاية
نهاية

# Test 3: Pattern matching
دالة اختبار_pattern_matching() 
    متغير نتيجة: نجاح<رقم> | فشل<نص> = نجاح(42)
    
    طابق نتيجة 
        نجاح(ع) => assert(ع == 42)
        فشل(_) => assert(خطأ)
    نهاية
نهاية

# Test 4: Optional types
دالة اختبار_اختياري() 
    متغير قيمة: رقم? = لاشيء
    تحقق(قيمة نوع لاشيء)
    
    قيمة = 42
    تحقق(قيمة نوع رقم)
نهاية
```

---

## 📊 معايير النجاح / Success Criteria

### الوظائف / Functionality

- ✅ **Union Types:** إنشاء واستخدام / Creation and usage
- ✅ **Optional Types:** `T?` syntax working
- ✅ **Type Guards:** `نوع` و `is` working
- ✅ **Type Narrowing:** في if/else branches
- ✅ **Pattern Matching:** type extraction working
- ✅ **Exhaustiveness:** checking implemented

### الأداء / Performance

- ✅ **Union Overhead:** < 20% slowdown
- ✅ **Type Check Cost:** minimized
- ✅ **Memory Layout:** optimized
- ✅ **Compile Time:** reasonable

### الجودة / Quality

- ✅ **Test Coverage:** > 90%
- ✅ **Error Messages:** clear in Arabic
- ✅ **Documentation:** comprehensive
- ✅ **Code Quality:** professional

---

## 📚 الأمثلة العملية / Practical Examples

### Example 1: Result Type
```s
# نوع النتيجة العام / Generic result type
صنف نجاح<T> 
    قيمة: T
نهاية

صنف فشل<E> 
    خطأ: E
نهاية

نوع نتيجة<T, E> = نجاح<T> | فشل<E>

# Usage
دالة  نتيجة<رقم, نص> قسمة(أ: رقم, ب: رقم)  
    إذا (ب == 0) 
        إرجاع فشل("لا يمكن القسمة على صفر")
    نهاية
    إرجاع نجاح(أ / ب)
نهاية

# Pattern matching على النتيجة
متغير نتيجة = قسمة(10, 2)
طابق نتيجة {
    نجاح(قيمة) => اطبع("النتيجة: " + قيمة)
    فشل(رسالة) => اطبع("خطأ: " + رسالة)
}
```

### Example 2: JSON Value
```s
# JSON value type
نوع JsonValue = 
    | JsonNull
    | JsonBool(منطقي)
    | JsonNumber(رقم)
    | JsonString(نص)
    | JsonArray(مصفوفة<JsonValue>)
    | JsonObject(قاموس<نص, JsonValue>)

دالة stringify(قيمة: JsonValue) -> نص {
    طابق قيمة {
        JsonNull => "null"
        JsonBool(ب) => ب ? "true" : "false"
        JsonNumber(ع) => toString(ع)
        JsonString(ن) => "\"" + ن + "\""
        JsonArray(م) => "[" + م.map(stringify).join(", ") + "]"
        JsonObject(ق) => "{" + ... + "}"
    }
}
```

### Example 3: Safe Navigation
```s
# Optional chaining مع type narrowing
دالة احصل_على_طول_البريد(مستخدم: User?) -> رقم {
    إذا (مستخدم نوع User) {
        إذا (مستخدم.بريد نوع نص) {
            إرجاع مستخدم.بريد.طول()
        }
    }
    إرجاع 0
}
```

---

## ⚠️ المخاطر والتحديات / Risks & Challenges

### Technical Challenges

```
🔴 High Risk:
   ├─ Type narrowing complexity
   │  └─ Solution: Start simple, iterate
   │
   ├─ Exhaustiveness checking
   │  └─ Solution: Use proven algorithms
   │
   └─ Performance overhead
      └─ Solution: Optimize hot paths

🟡 Medium Risk:
   ├─ Pattern matching integration
   │  └─ Solution: Close collaboration with parser
   │
   └─ Error messages clarity
      └─ Solution: Extensive testing
```

### Timeline Risks

```
⚠️ Potential Delays:
   ├─ Type narrowing edge cases (+2 days)
   ├─ Pattern matching complexity (+1-2 days)
   └─ LLVM integration issues (+1 day)

Total Buffer: +4-5 days
```

---

## 📅 الجدول الزمني / Timeline

```
يوم 1-2:   Union Type Core
يوم 3-4:   Type Guards
يوم 5-6:   Type Narrowing
يوم 7:     Pattern Matching Integration
يوم 8:     LLVM Integration
يوم 9-10:  Testing & Documentation (buffer)

Total: 5-7 days (with 2-day buffer)
```

---

## 🎯 التسليمات / Deliverables

### Code Files (14 files - 5,430 lines):
- ✅ union_type.h/cpp (850 lines)
- ✅ optional_type.h/cpp (350 lines)
- ✅ type_guard.h/cpp (730 lines)
- ✅ type_guard_visitor.h/cpp (550 lines)
- ✅ type_narrowing.h/cpp (900 lines)
- ✅ control_flow_analyzer.h/cpp (730 lines)
- ✅ exhaustiveness_checker.h/cpp (600 lines)
- ✅ pattern_type_checker.h/cpp (820 lines)
- ✅ union_type_codegen.h/cpp (730 lines)
- ✅ type_guard_codegen.h/cpp (570 lines)

### Documentation (2 files - 1,200 lines):
- ✅ PHASE_1_3_5_COMPLETION.md
- ✅ UNION_TYPES_GUIDE.md

### Tests (3 files - 1,500 lines):
- ✅ test_union_type.cpp
- ✅ test_type_guards.cpp
- ✅ test_type_narrowing.cpp

---

**Status:** 🟢 Ready to start  
**Next Phase:** Phase 1.3.6 - LLVM Integration & Optimization

---

*"من نوع واحد إلى union من الأنواع - المرونة مع الأمان"*  
*"From single type to union of types - flexibility with safety"*

🚀✨🎯
