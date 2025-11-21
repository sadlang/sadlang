# Phase 3 & 4: Interpreter Implementation Plan
# خطة تنفيذ المفسّر - المرحلة 3 و 4

**Status:** 🚀 IN PROGRESS  
**Start Date:** November 21, 2025  
**Target Duration:** 2-3 weeks  
**Prerequisites:** ✅ Phase 1 (Lexer) + ✅ Phase 2 (Parser/Python Features)

---

## 🎯 Overview / نظرة عامة

### English

Implement a complete **interpreter** for the S language that can:
- Execute all parsed AST nodes
- Manage variables and scopes
- Handle function calls and returns
- Support all Phase 2 Python features (lambda, comprehensions, decorators, generators)
- Provide Arabic and English error messages

### العربية

تنفيذ **مفسّر كامل** للغة ص يمكنه:
- تنفيذ جميع عقد AST المحللة
- إدارة المتغيرات والنطاقات
- معالجة استدعاءات الدوال والإرجاع
- دعم جميع ميزات Phase 2 على طراز Python
- توفير رسائل خطأ بالعربية والإنجليزية

---

## 📋 Phase Breakdown

### Phase 3: Data Layer (Week 1 - 5 days)
**Goal:** Build the foundation for runtime data management

#### Day 1: Value System ✅ PRIORITY
- [x] Task 3.1: Value class with all data types
- [x] Task 3.2: Operator overloading (+, -, *, /, ==, etc.)
- [x] Task 3.3: Type conversion methods
- [x] Task 3.4: Comprehensive tests (30+)

#### Day 2: Scope Management ✅ PRIORITY
- [x] Task 3.5: ScopeManager class
- [x] Task 3.6: pushScope() / popScope()
- [x] Task 3.7: Scope depth tracking
- [x] Task 3.8: Tests (15+)

#### Day 3: Variable Management ✅ PRIORITY
- [x] Task 3.9: VariableManager class
- [x] Task 3.10: define() / assign() / get()
- [x] Task 3.11: Variable shadowing support
- [x] Task 3.12: Tests (20+)

#### Day 4: Function Management
- [ ] Task 3.13: FunctionManager class
- [ ] Task 3.14: Function definition storage
- [ ] Task 3.15: Parameter matching
- [ ] Task 3.16: Tests (15+)

#### Day 5: Array & Map Types
- [ ] Task 3.17: Array type implementation
- [ ] Task 3.18: Map/Dict type implementation
- [ ] Task 3.19: Iterator support
- [ ] Task 3.20: Tests (25+)

### Phase 4: Interpreter Core (Week 2-3 - 10 days)
**Goal:** Implement full AST execution

#### Week 2: Expression & Statement Execution
- [ ] Task 4.1-4.5: Expression evaluator (5 days)
- [ ] Task 4.6-4.10: Statement executor (5 days)

#### Week 3: Advanced Features
- [ ] Task 4.11-4.15: Function calls, lambda, comprehensions
- [ ] Task 4.16-4.20: Decorators, generators, yield

---

## 🔧 Detailed Task Breakdown

### Task 3.1: Value Class Implementation

**Files to Create:**
```
include/data/types/value.h
src/data/types/value.cpp
tests/data_tests/test_value.cpp
```

**Value.h Structure:**
```cpp
#pragma once
#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Sad {
namespace Data {

/**
 * @brief (AR) أنواع القيم المدعومة
 * @brief (EN) Supported value types
 */
enum class ValueType {
    VOID,       ///< (AR) فراغ / (EN) Void/None
    INTEGER,    ///< (AR) رقم صحيح / (EN) Integer
    DOUBLE,     ///< (AR) رقم عشري / (EN) Double
    STRING,     ///< (AR) نص / (EN) String
    BOOLEAN,    ///< (AR) منطقي / (EN) Boolean
    ARRAY,      ///< (AR) مصفوفة / (EN) Array
    MAP,        ///< (AR) قاموس / (EN) Map/Dictionary
    FUNCTION    ///< (AR) دالة / (EN) Function
};

/**
 * @brief (AR) فئة تمثل قيمة في وقت التشغيل
 * @brief (EN) Runtime value class
 * 
 * Represents any value during program execution.
 * Supports all data types and operators.
 */
class Value {
public:
    // Constructors for each type
    Value();                                    // VOID
    explicit Value(int val);                    // INTEGER
    explicit Value(double val);                 // DOUBLE
    explicit Value(const std::string& val);     // STRING
    explicit Value(bool val);                   // BOOLEAN
    
    // Type checking
    ValueType getType() const { return type_; }
    bool isVoid() const { return type_ == ValueType::VOID; }
    bool isInteger() const { return type_ == ValueType::INTEGER; }
    bool isDouble() const { return type_ == ValueType::DOUBLE; }
    bool isString() const { return type_ == ValueType::STRING; }
    bool isBoolean() const { return type_ == ValueType::BOOLEAN; }
    bool isNumeric() const { return isInteger() || isDouble(); }
    
    // Type conversion
    int toInt() const;
    double toDouble() const;
    std::string toString() const;
    bool toBool() const;
    
    // Arithmetic operators
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;
    Value operator-() const;  // Unary minus
    
    // Comparison operators
    Value operator==(const Value& other) const;
    Value operator!=(const Value& other) const;
    Value operator<(const Value& other) const;
    Value operator>(const Value& other) const;
    Value operator<=(const Value& other) const;
    Value operator>=(const Value& other) const;
    
    // Logical operators
    Value operator&&(const Value& other) const;
    Value operator||(const Value& other) const;
    Value operator!() const;  // Logical NOT
    
    // Increment/Decrement
    Value& operator++();      // ++val
    Value operator++(int);    // val++
    Value& operator--();      // --val
    Value operator--(int);    // val--
    
    // Assignment operators
    Value& operator+=(const Value& other);
    Value& operator-=(const Value& other);
    Value& operator*=(const Value& other);
    Value& operator/=(const Value& other);
    
    // Debug output
    std::string debugString() const;
    
private:
    ValueType type_;
    std::variant<
        std::monostate,  // VOID
        int,             // INTEGER
        double,          // DOUBLE
        std::string,     // STRING
        bool             // BOOLEAN
    > data_;
};

} // namespace Data
} // namespace Sad
```

**Implementation Requirements:**
1. All arithmetic operators must handle type promotion (int → double)
2. String concatenation with `+` operator
3. Boolean conversion rules:
   - Numbers: 0 = false, non-zero = true
   - Strings: empty = false, non-empty = true
   - VOID: always false
4. Type mismatch errors in Arabic + English

**Tests Required (30+):**
- Construction tests (5)
- Type checking tests (5)
- Arithmetic operators (10)
- Comparison operators (6)
- Logical operators (4)
- Type conversion (5)
- Error handling (5)

**Time Estimate:** 6 hours

---

### Task 3.2: Scope Manager Implementation

**Files to Create:**
```
include/data/scope/scope_manager.h
src/data/scope/scope_manager.cpp
tests/data_tests/test_scope_manager.cpp
```

**ScopeManager.h Structure:**
```cpp
#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace Sad {
namespace Data {

/**
 * @brief (AR) مدير النطاقات المتداخلة
 * @brief (EN) Nested scope manager
 * 
 * Manages variable scopes in a stack-like fashion.
 * Supports nested blocks, functions, and loops.
 */
class ScopeManager {
public:
    ScopeManager();
    
    /**
     * @brief (AR) إنشاء نطاق جديد
     * @brief (EN) Create a new scope
     */
    void pushScope();
    
    /**
     * @brief (AR) إزالة النطاق الحالي
     * @brief (EN) Remove current scope
     * @throws (std::runtime_error) إذا كان النطاق الأساسي / If at global scope
     */
    void popScope();
    
    /**
     * @brief (AR) الحصول على عمق النطاق الحالي
     * @brief (EN) Get current scope depth
     * @return (size_t) العمق (0 = عالمي) / Depth (0 = global)
     */
    size_t getCurrentDepth() const { return scopes_.size() - 1; }
    
    /**
     * @brief (AR) هل نحن في النطاق العالمي؟
     * @brief (EN) Are we in global scope?
     */
    bool isGlobalScope() const { return getCurrentDepth() == 0; }
    
    /**
     * @brief (AR) الحصول على عدد النطاقات
     * @brief (EN) Get number of scopes
     */
    size_t getScopeCount() const { return scopes_.size(); }
    
    // Debug methods
    std::string debugString() const;
    
private:
    struct Scope {
        size_t depth;
        size_t id;
    };
    
    std::vector<Scope> scopes_;
    size_t next_scope_id_;
};

} // namespace Data
} // namespace Sad
```

**Tests Required (15+):**
- Push/pop tests (5)
- Depth tracking (3)
- Error handling (3)
- Multiple scopes (4)

**Time Estimate:** 4 hours

---

### Task 3.3: Variable Manager Implementation

**Files to Create:**
```
include/data/managers/variable_manager.h
src/data/managers/variable_manager.cpp
tests/data_tests/test_variable_manager.cpp
```

**VariableManager.h Structure:**
```cpp
#pragma once
#include "../types/value.h"
#include "../scope/scope_manager.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace Sad {
namespace Data {

/**
 * @brief (AR) مدير المتغيرات
 * @brief (EN) Variable manager
 * 
 * Manages variable storage and retrieval across scopes.
 * Supports variable shadowing and scope-based lookup.
 */
class VariableManager {
public:
    explicit VariableManager(std::shared_ptr<ScopeManager> scope_mgr);
    
    /**
     * @brief (AR) تعريف متغير جديد في النطاق الحالي
     * @brief (EN) Define new variable in current scope
     * @param name اسم المتغير / Variable name
     * @param value القيمة الأولية / Initial value
     * @throws (std::runtime_error) إذا كان المتغير معرف مسبقاً / If already defined
     */
    void define(const std::string& name, const Value& value);
    
    /**
     * @brief (AR) تعيين قيمة لمتغير موجود
     * @brief (EN) Assign value to existing variable
     * @param name اسم المتغير / Variable name
     * @param value القيمة الجديدة / New value
     * @throws (std::runtime_error) إذا لم يكن المتغير معرفاً / If not defined
     */
    void assign(const std::string& name, const Value& value);
    
    /**
     * @brief (AR) الحصول على قيمة متغير
     * @brief (EN) Get variable value
     * @param name اسم المتغير / Variable name
     * @return (Value) القيمة / The value
     * @throws (std::runtime_error) إذا لم يكن المتغير معرفاً / If not defined
     */
    Value get(const std::string& name) const;
    
    /**
     * @brief (AR) التحقق من وجود متغير
     * @brief (EN) Check if variable exists
     * @param name اسم المتغير / Variable name
     * @return (bool) موجود أم لا / Exists or not
     */
    bool exists(const std::string& name) const;
    
    /**
     * @brief (AR) التحقق من وجود متغير في النطاق الحالي فقط
     * @brief (EN) Check if variable exists in current scope only
     */
    bool existsInCurrentScope(const std::string& name) const;
    
    /**
     * @brief (AR) حذف متغير من النطاق الحالي
     * @brief (EN) Remove variable from current scope
     */
    void remove(const std::string& name);
    
    /**
     * @brief (AR) حذف جميع متغيرات النطاق الحالي
     * @brief (EN) Clear all variables in current scope
     */
    void clearCurrentScope();
    
    // Debug methods
    std::string debugString() const;
    size_t getVariableCount() const;
    
private:
    std::shared_ptr<ScopeManager> scope_mgr_;
    std::vector<std::unordered_map<std::string, Value>> scopes_;
};

} // namespace Data
} // namespace Sad
```

**Implementation Notes:**
- Variable lookup searches from current scope upward to global
- Variable shadowing: inner scope can redefine outer scope variables
- `define()` only creates in current scope
- `assign()` modifies nearest matching variable

**Tests Required (20+):**
- Define/assign/get (5)
- Scope isolation (4)
- Variable shadowing (4)
- Error handling (4)
- Multiple scopes (3)

**Time Estimate:** 6 hours

---

## 📊 Progress Tracking

### Phase 3: Data Layer

| Task | Component | Status | Tests | Time |
|------|-----------|--------|-------|------|
| 3.1 | Value System | ⏳ | 0/30 | 6h |
| 3.2 | Scope Manager | ⏸️ | 0/15 | 4h |
| 3.3 | Variable Manager | ⏸️ | 0/20 | 6h |
| 3.4 | Function Manager | ⏸️ | 0/15 | 6h |
| 3.5 | Array/Map Types | ⏸️ | 0/25 | 8h |
| **Total** | **5 components** | **0%** | **0/105** | **30h** |

### Phase 4: Interpreter Core

| Task | Component | Status | Tests | Time |
|------|-----------|--------|-------|------|
| 4.1 | Expression Evaluator | ⏸️ | TBD | 20h |
| 4.2 | Statement Executor | ⏸️ | TBD | 20h |
| 4.3 | Function Calls | ⏸️ | TBD | 15h |
| 4.4 | Advanced Features | ⏸️ | TBD | 25h |
| **Total** | **4 components** | **0%** | **TBD** | **80h** |

---

## 🎯 Success Criteria / معايير النجاح

### Phase 3 Completion Requirements
- ✅ All Value operations working correctly
- ✅ Scope management handles nesting properly
- ✅ Variable shadowing works as expected
- ✅ 105+ tests passing (100% pass rate)
- ✅ Zero compiler warnings
- ✅ Complete bilingual documentation

### Phase 4 Completion Requirements
- ✅ Can execute simple programs end-to-end
- ✅ All expression types evaluate correctly
- ✅ All statement types execute correctly
- ✅ Function calls work with parameters/returns
- ✅ All Phase 2 features (lambda, comprehensions, etc.) work
- ✅ 200+ integration tests passing

---

## 🚀 Getting Started

### Step 1: Create Directory Structure
```powershell
cd c:\s\s_language

# Create data layer directories
mkdir include\data\types -Force
mkdir include\data\scope -Force
mkdir include\data\managers -Force
mkdir src\data\types -Force
mkdir src\data\scope -Force
mkdir src\data\managers -Force
mkdir tests\data_tests -Force

# Create interpreter directories
mkdir include\interpreter\core -Force
mkdir include\interpreter\visitors -Force
mkdir src\interpreter\core -Force
mkdir src\interpreter\visitors -Force
mkdir tests\interpreter_tests -Force
```

### Step 2: Start with Value System
```powershell
# Create Value files
New-Item include\data\types\value.h
New-Item src\data\types\value.cpp
New-Item tests\data_tests\test_value.cpp

# Open in VS Code
code include\data\types\value.h
```

### Step 3: Run Tests Continuously
```powershell
# Compile test
g++ tests/data_tests/test_value.cpp src/data/types/value.cpp -o build/test_value.exe -Iinclude -std=c++17

# Run test
./build/test_value.exe
```

---

## 📝 Notes / ملاحظات

### Design Decisions
1. **Value as Variant:** Using std::variant for type safety and performance
2. **Scope Stack:** Vector-based scope stack for fast push/pop
3. **Variable Storage:** HashMap per scope for O(1) lookup
4. **Error Handling:** Exceptions with bilingual messages

### Future Enhancements (Post Phase 4)
- Garbage collection for complex types
- Lazy evaluation for comprehensions
- Generator state management
- JIT compilation for hot loops
- Memory pooling for Value objects

---

**Plan Version:** 1.0  
**Created:** November 21, 2025  
**Author:** S Language Development Team  

---

🚀 **Ready to begin interpreter implementation!**  
🚀 **جاهز لبدء تنفيذ المفسّر!**
