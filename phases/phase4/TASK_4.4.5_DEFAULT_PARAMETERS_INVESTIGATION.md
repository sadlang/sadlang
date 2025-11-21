# Task 4.4.5: Default Parameters Investigation Report
# تقرير التحقيق في المعاملات الافتراضية (Default Parameters)

**Date:** November 21, 2025  
**Status:** Investigation Complete ✅  
**Next:** Implementation

---

## 1. Executive Summary | ملخص تنفيذي

### English
Default Parameters are **partially implemented** in S Language:
- ✅ AST Structure exists (`Parameter.defaultValue: ExprPtr`)
- ✅ Parser supports syntax (`func f(x, y = 10)`)
- ✅ FunctionManager stores default info
- ❌ ExpressionEvaluator does NOT evaluate defaults during function calls

**Action Required:** Update `visitCallExpr` to evaluate and use default parameter values when fewer arguments are provided.

### العربية
المعاملات الافتراضية **منفذة جزئياً** في لغة ص:
- ✅ هيكل AST موجود (`Parameter.defaultValue: ExprPtr`)
- ✅ المُحلل اللغوي يدعم الصيغة (`func f(x, y = 10)`)
- ✅ FunctionManager يخزن معلومات Default
- ❌ ExpressionEvaluator لا يُقيّم القيم الافتراضية عند استدعاء الدوال

**الإجراء المطلوب:** تحديث `visitCallExpr` لتقييم واستخدام قيم المعاملات الافتراضية عندما يتم تقديم معاملات أقل.

---

## 2. Current Architecture | الهيكل الحالي

### AST Level (Parser)

**File:** `include/parser/ast/expressions.h`  
**Lines:** 451-462

```cpp
struct Parameter {
    std::string name;           // Parameter name
    Data::DataType type;        // Parameter type
    ExprPtr defaultValue;       // ✅ Default value (optional) - EXISTS!
    
    Parameter(const std::string& n, Data::DataType t = Data::DataType::UNKNOWN,
              ExprPtr def = nullptr)
        : name(n), type(t), defaultValue(std::move(def)) {}
    
    // Move semantics
    Parameter(Parameter&&) = default;
    Parameter& operator=(Parameter&&) = default;
};
```

**Status:** ✅ **Complete**  
- `defaultValue` field exists as `ExprPtr`
- Constructor accepts default expressions
- Move semantics supported

---

### Parser Level

**File:** `src/parser/parser_core_helpers.cpp`  
**Lines:** 358-368

```cpp
ExprPtr defaultValue = nullptr;
if (match(TokenType::EQUAL)) {
    defaultValue = parseExpression();
}

// ... (inside parameter parsing)
parameters.push_back(Parameter(
    paramName,
    paramType,
    std::move(defaultValue)
));
```

**Status:** ✅ **Complete**  
- Parser recognizes `=` syntax
- Parses default value as expression
- Stores in Parameter

**Example Syntax:**
```s
func greet(name: string, greeting: string = "Hello") {
    print(greeting + ", " + name);
}
```

---

### Function Manager Level

**File:** `include/data/managers/function_manager.h`  
**Lines:** 108-115

```cpp
struct FunctionParameter {
    std::string name;           // Parameter name
    std::string type;           // Parameter type (optional)
    bool hasDefaultValue;       // ✅ Has default value flag
    std::string defaultValue;   // ✅ Default value (as string)
    
    FunctionParameter(const std::string& n = "", const std::string& t = "", 
                     bool hasDefault = false, const std::string& defVal = "")
        : name(n), type(t), hasDefaultValue(hasDefault), defaultValue(defVal) {}
};
```

**Status:** ✅ **Partial**  
- Has `hasDefaultValue` flag
- Has `defaultValue` field (but as string, not ExprPtr)
- Used for display/debugging

**Usage in Code:**
```cpp
// File: src/data/managers/function_manager.cpp, Line 74-75
if (parameters_[i].hasDefaultValue) {
    oss << " = " << parameters_[i].defaultValue;
}
```

**Problem:** `FunctionParameter.defaultValue` is **string**, not executable expression!

---

### Expression Evaluator Level

**File:** `src/interpreter/visitors/expression_evaluator.cpp`  
**Lines:** 420-525 (`visitCallExpr`)

**Current Logic:**
```cpp
void ExpressionEvaluator::visitCallExpr(CallExpr& node) {
    // 1. Get function name
    std::string funcName = ...;
    
    // 2. Evaluate arguments
    std::vector<Data::Value> arguments;
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
        arguments.push_back(lastResult_);
    }
    
    // 3. Find function
    auto func = functionManager_.getFunction(funcName, arguments.size());
    //                                                   ^^^^^^^^^^^^^^^^
    //                                            PROBLEM: Exact match required!
    
    if (!func) {
        throw RuntimeError("Function not defined with N parameters");
    }
    
    // 4. Create scope and define parameters
    scopeManager_.pushScope(Data::ScopeType::FUNCTION, funcName);
    const auto& params = func->getParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        variableManager_.define(params[i].name, arguments[i]);
        //                                       ^^^^^^^^^^^^
        //                              PROBLEM: arguments[i] may not exist!
    }
    
    // 5. Execute function body
    ...
}
```

**Problems Identified:**

1. **Exact Parameter Match:** 
   ```cpp
   auto func = functionManager_.getFunction(funcName, arguments.size());
   ```
   - This requires `arguments.size()` to EXACTLY match parameter count
   - Does not allow fewer arguments with defaults

2. **No Default Evaluation:**
   ```cpp
   for (size_t i = 0; i < params.size(); ++i) {
       variableManager_.define(params[i].name, arguments[i]);
   }
   ```
   - Assumes `arguments[i]` exists for all `i < params.size()`
   - Does not check for defaults when `i >= arguments.size()`

---

## 3. Implementation Plan | خطة التنفيذ

### Phase 1: Update FunctionManager (Optional)

**Goal:** Store executable defaults, not just strings

**Current:**
```cpp
struct FunctionParameter {
    std::string defaultValue;  // String representation
};
```

**Proposed:**
```cpp
struct FunctionParameter {
    std::string defaultValueStr;        // For display
    std::shared_ptr<Parser::ASTNode> defaultValueExpr;  // For evaluation
};
```

**Decision:** ⚠️ **Skip for now** - Can work with Parameter.defaultValue directly

---

### Phase 2: Update FunctionDefinition

**File:** `include/data/managers/function_manager.h`

**Add Method:**
```cpp
class FunctionDefinition {
public:
    // Existing
    size_t getRequiredParameterCount() const;  // Already exists!
    
    // New
    bool acceptsArgumentCount(size_t argCount) const {
        size_t required = getRequiredParameterCount();
        size_t total = parameters_.size();
        return argCount >= required && argCount <= total;
    }
};
```

**Implementation:**
```cpp
size_t FunctionDefinition::getRequiredParameterCount() const {
    size_t required = 0;
    for (const auto& param : parameters_) {
        if (!param.hasDefaultValue) {
            required++;
        }
    }
    return required;
}

bool FunctionDefinition::matchesParameterCount(size_t argCount) const {
    size_t required = getRequiredParameterCount();
    size_t total = parameters_.size();
    return argCount >= required && argCount <= total;
}
```

---

### Phase 3: Update visitCallExpr ⭐ MAIN TASK

**File:** `src/interpreter/visitors/expression_evaluator.cpp`  
**Function:** `visitCallExpr`

**Current Logic:**
```cpp
// 2. Evaluate arguments
std::vector<Data::Value> arguments;
for (const auto& arg : node.arguments) {
    arg->accept(*this);
    arguments.push_back(lastResult_);
}

// 3. Find function - EXACT match
auto func = functionManager_.getFunction(funcName, arguments.size());
```

**New Logic:**
```cpp
// 2. Evaluate arguments
std::vector<Data::Value> arguments;
for (const auto& arg : node.arguments) {
    arg->accept(*this);
    arguments.push_back(lastResult_);
}

// 3. Find function - FLEXIBLE match
// Try to find function that accepts this argument count
auto allOverloads = functionManager_.getFunctionOverloads(funcName);
std::shared_ptr<FunctionDefinition> func = nullptr;

for (const auto& candidate : allOverloads) {
    if (candidate->acceptsArgumentCount(arguments.size())) {
        func = candidate;
        break;
    }
}

if (!func) {
    throw RuntimeError(
        "(AR) الدالة '" + funcName + "' غير معرفة بعدد معاملات " + 
        std::to_string(arguments.size()) + 
        " / (EN) Function '" + funcName + "' not defined with " + 
        std::to_string(arguments.size()) + " parameters"
    );
}

// 4. Get parameters from ORIGINAL FunctionDecl (not FunctionParameter!)
// Need to access Parameter objects with defaultValue
// THIS IS THE CHALLENGE: FunctionDefinition stores FunctionParameter, not Parameter!

const auto& funcParams = func->getParameters();  // Returns FunctionParameter[]
// But we need access to Parameter[] with defaultValue ExprPtr!

// SOLUTION: Store FunctionDecl pointer in FunctionDefinition
// OR: Store Parameter[] alongside FunctionParameter[]

// 5. Fill missing arguments with defaults
while (arguments.size() < funcParams.size()) {
    size_t i = arguments.size();
    
    if (!funcParams[i].hasDefaultValue) {
        throw RuntimeError(
            "(AR) معامل مطلوب مفقود: " + funcParams[i].name + 
            " / (EN) Required parameter missing: " + funcParams[i].name
        );
    }
    
    // Evaluate default expression
    // PROBLEM: Need access to Parameter.defaultValue (ExprPtr)
    // Current FunctionParameter only has string!
    
    // TEMPORARY: Parse string value
    // BETTER: Store ExprPtr in FunctionDefinition
    
    Value defaultVal = parseDefaultValue(funcParams[i].defaultValue);
    arguments.push_back(defaultVal);
}

// 6. Define parameters as local variables
scopeManager_.pushScope(Data::ScopeType::FUNCTION, funcName);
for (size_t i = 0; i < funcParams.size(); ++i) {
    variableManager_.define(funcParams[i].name, arguments[i]);
}
```

---

### Phase 4: Challenge - Accessing defaultValue ExprPtr

**Problem:**  
`FunctionDefinition` stores `FunctionParameter[]` (with string defaults), but we need `Parameter[]` (with ExprPtr defaults) to evaluate them.

**Solution Options:**

#### Option A: Store FunctionDecl in FunctionDefinition ⭐ RECOMMENDED
```cpp
class FunctionDefinition {
private:
    std::shared_ptr<Parser::ASTNode> body_;  // Already exists
    std::vector<FunctionParameter> parameters_;  // For display
    std::shared_ptr<FunctionDecl> originalDecl_;  // NEW - for defaults
};
```

**Pros:**
- Clean access to Parameter[] with defaultValue
- No duplication
- Easy to implement

**Cons:**
- Circular dependency? (FunctionDecl already stored in body_)

#### Option B: Store Parameter[] in FunctionDefinition
```cpp
class FunctionDefinition {
private:
    std::vector<FunctionParameter> parameters_;  // For display
    std::vector<Parameter> parametersWithDefaults_;  // NEW - for evaluation
};
```

**Pros:**
- Direct access to defaults
- No circular dependencies

**Cons:**
- Data duplication
- Need to maintain two lists

#### Option C: Convert FunctionParameter to store ExprPtr
```cpp
struct FunctionParameter {
    std::string name;
    std::string type;
    bool hasDefaultValue;
    std::string defaultValueStr;  // For display
    ExprPtr defaultValueExpr;     // NEW - for evaluation
};
```

**Pros:**
- Single source of truth
- Clean architecture

**Cons:**
- Need to update FunctionParameter definition
- May require changes to FunctionManager

---

## 4. Recommended Approach | النهج الموصى به

### English

**Step 1:** Update `FunctionDefinition` to store original `FunctionDecl`
```cpp
class FunctionDefinition {
private:
    std::shared_ptr<FunctionDecl> decl_;  // Store original declaration
};
```

**Step 2:** Add helper method
```cpp
const std::vector<Parameter>& getParametersWithDefaults() const {
    return decl_->parameters;
}
```

**Step 3:** Update `visitCallExpr`
```cpp
// Find function with flexible matching
auto func = findFunctionForCall(funcName, arguments.size());

// Get parameters with default expressions
const auto& params = func->getParametersWithDefaults();

// Evaluate defaults for missing arguments
while (arguments.size() < params.size()) {
    size_t i = arguments.size();
    const auto& param = params[i];
    
    if (!param.defaultValue) {
        throw RuntimeError("Required parameter missing: " + param.name);
    }
    
    // Evaluate default expression
    param.defaultValue->accept(*this);
    arguments.push_back(lastResult_);
}
```

### العربية

**الخطوة 1:** تحديث `FunctionDefinition` لتخزين `FunctionDecl` الأصلي

**الخطوة 2:** إضافة دالة مساعدة للحصول على Parameters مع defaultValue

**الخطوة 3:** تحديث `visitCallExpr` لتقييم القيم الافتراضية

---

## 5. Test Plan | خطة الاختبار

### Test 1: Single Default Parameter
```s
func greet(name, greeting = "Hello") {
    return greeting + ", " + name;
}

var result1 = greet("Alice");              // "Hello, Alice"
var result2 = greet("Bob", "Hi");          // "Hi, Bob"
```

### Test 2: Multiple Defaults
```s
func calculate(a, b = 1, c = 2) {
    return a + b + c;
}

var r1 = calculate(10);           // 10 + 1 + 2 = 13
var r2 = calculate(10, 5);        // 10 + 5 + 2 = 17
var r3 = calculate(10, 5, 3);     // 10 + 5 + 3 = 18
```

### Test 3: Mixed Required and Optional
```s
func format(text, prefix = "[", suffix = "]") {
    return prefix + text + suffix;
}

var f1 = format("INFO");                   // "[INFO]"
var f2 = format("WARN", "<");              // "<WARN]"
var f3 = format("ERROR", "<<", ">>");      // "<<ERROR>>"
```

### Test 4: Error Cases
```s
func needsTwo(a, b) {
    return a + b;
}

needsTwo(5);  // Error: Required parameter 'b' missing
```

---

## 6. Implementation Estimate | تقدير التنفيذ

| Task | Complexity | Time | Priority |
|------|------------|------|----------|
| Update FunctionDefinition | Low | 30 min | High |
| Update visitCallExpr | Medium | 1-2 hours | High |
| Add flexible function lookup | Medium | 30 min | High |
| Create comprehensive tests | Low | 1 hour | High |
| Documentation | Low | 30 min | Medium |
| **TOTAL** | | **3-4 hours** | |

---

## 7. Next Steps | الخطوات التالية

1. ✅ **Investigation Complete** (This Document)
2. ⏳ **Update FunctionDefinition** - Store original FunctionDecl
3. ⏳ **Update FunctionManager** - Add flexible lookup
4. ⏳ **Update visitCallExpr** - Evaluate defaults
5. ⏳ **Create Tests** - Comprehensive test suite
6. ⏳ **Documentation** - Update language spec

---

## 8. Risks & Mitigation | المخاطر والحلول

### Risk 1: Circular Dependencies
**Problem:** FunctionDecl → FunctionDefinition → FunctionDecl?  
**Mitigation:** Use forward declarations, shared_ptr

### Risk 2: Default Evaluation Order
**Problem:** Defaults may depend on previous parameters  
**Example:** `func f(a, b = a * 2)`  
**Mitigation:** Evaluate in order, within function scope

### Risk 3: Complex Default Expressions
**Problem:** Default may call other functions  
**Example:** `func f(x = calculate())`  
**Mitigation:** Full expression evaluation (already supported)

---

## 9. Conclusion | الخلاصة

Default Parameters are **90% implemented** in S Language:
- ✅ Syntax supported
- ✅ AST structure ready
- ✅ Parser working
- ❌ Runtime evaluation missing

**Estimated Completion:** 3-4 hours of focused work

**Status:** Ready to implement ✅

---

**Document Author:** GitHub Copilot  
**Date:** November 21, 2025  
**Status:** Investigation Complete
