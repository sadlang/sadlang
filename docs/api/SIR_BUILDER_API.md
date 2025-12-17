# SIR Builder API Reference
# مرجع واجهة برمجة بناء SIR

**Version**: 1.0  
**Date**: December 3, 2025  
**Status**: Complete

---

## 📋 Table of Contents | جدول المحتويات

1. [Overview](#overview)
2. [Core Classes](#core-classes)
3. [Public API](#public-api)
4. [Helper Structures](#helper-structures)
5. [Usage Examples](#usage-examples)
6. [Error Handling](#error-handling)
7. [Generic Types](#generic-types)

---

## Overview | نظرة عامة

**SIRBuilder** is the core component responsible for converting Abstract Syntax Tree (AST) to S Intermediate Representation (SIR).

**بناء SIR** هو المكون الأساسي المسؤول عن تحويل الشجرة النحوية المجردة (AST) إلى التمثيل الوسيط S (SIR).

### Key Features | الميزات الرئيسية

- ✅ **Expression Conversion** - تحويل التعابير
- ✅ **Statement Processing** - معالجة الجمل
- ✅ **Virtual Register Management** - إدارة السجلات الافتراضية
- ✅ **Control Flow Graph (CFG)** - رسم التدفق البياني
- ✅ **Type System** - نظام الأنواع
- ✅ **Generic Types Support** - دعم الأنواع العامة
- ✅ **Error Accumulation** - تجميع الأخطاء

---

## Core Classes | الفئات الأساسية

### SIRBuilder

```cpp
namespace Sad::Compiler::SIR {

class SIRBuilder {
public:
    // Constructor
    SIRBuilder();
    
    // Main API
    std::shared_ptr<SIRModule> buildModule(AST::Program* program);
    
    // Error handling
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;
};

} // namespace Sad::Compiler::SIR
```

---

## Public API | الواجهة العامة

### Main Functions | الدوال الرئيسية

#### buildModule()

```cpp
/**
 * @brief Build complete SIR module from AST
 * @brief بناء وحدة SIR كاملة من شجرة AST
 * 
 * @param program AST program node
 * @return Complete SIR module or nullptr on error
 * 
 * @example
 * SIRBuilder builder;
 * auto ast = parser.parse(source);
 * auto sir = builder.buildModule(ast);
 * 
 * if (builder.hasErrors()) {
 *     for (const auto& error : builder.getErrors()) {
 *         std::cerr << error << std::endl;
 *     }
 * }
 */
std::shared_ptr<SIRModule> buildModule(AST::Program* program);
```

**Parameters**:
- `program`: Pointer to parsed AST program

**Returns**:
- `std::shared_ptr<SIRModule>`: Complete SIR module
- `nullptr`: If compilation fails

**Error Handling**:
- Errors accumulated in internal vector
- Check with `hasErrors()` after building
- Retrieve errors with `getErrors()`

---

### Expression Building | بناء التعابير

#### buildExpression()

```cpp
/**
 * @brief Build SIR from expression AST node
 * @brief بناء SIR من عقدة تعبير AST
 * 
 * @param expr Expression AST node
 * @return BuildResult with register and type
 */
BuildResult buildExpression(AST::Expression* expr);
```

**Supported Expression Types**:
- Integer literals (`42`)
- Float literals (`3.14`)
- String literals (`"hello"`)
- Boolean literals (`true`, `false`)
- Binary operations (`+`, `-`, `*`, `/`, `%`, etc.)
- Unary operations (`-`, `!`, `~`)
- Variable references (`x`)
- Function calls (`foo(x, y)`)
- Array access (`arr[i]`)
- Member access (`obj.field`)
- Method calls (`obj.method()`)
- New expressions (`new ClassName()`)
- Array literals (`[1, 2, 3]`)

**Example**:
```cpp
// S Language: var x = 10 + 20;
auto result = builder.buildExpression(addExpr);
// result.registerName = "%0"
// result.type = SIRType::I64
```

---

#### buildBinaryOp()

```cpp
/**
 * @brief Build binary operation
 * @brief بناء عملية ثنائية
 * 
 * @param left Left operand
 * @param op Binary operator
 * @param right Right operand
 * @return Result register and type
 */
BuildResult buildBinaryOp(
    const BuildResult& left,
    AST::BinaryOperator op,
    const BuildResult& right
);
```

**Supported Operators**:

| Category | Operators | Example |
|----------|-----------|---------|
| Arithmetic | `+`, `-`, `*`, `/`, `%` | `x + y` |
| Comparison | `==`, `!=`, `<`, `>`, `<=`, `>=` | `x < y` |
| Logical | `&&`, `\|\|` | `a && b` |
| Bitwise | `&`, `\|`, `^`, `<<`, `>>` | `x & y` |
| String | `+` (concat) | `"hello" + " world"` |

**Type Conversions**:
- Automatic I64 ↔ F64 conversion
- String concatenation with `+`
- Boolean logic operations

---

### Statement Processing | معالجة الجمل

#### buildStatement()

```cpp
/**
 * @brief Build SIR from statement AST node
 * @brief بناء SIR من عقدة جملة AST
 * 
 * @param stmt Statement AST node
 */
void buildStatement(AST::Statement* stmt);
```

**Supported Statement Types**:

| Statement | Description | Example |
|-----------|-------------|---------|
| Variable Declaration | `buildVarDecl()` | `var x: int = 10;` |
| Assignment | `buildAssignment()` | `x = 20;` |
| If-Else | `buildIfStatement()` | `if (x > 0) { ... }` |
| While Loop | `buildWhileStatement()` | `while (x < 10) { ... }` |
| For Loop | `buildForStatement()` | `for (var i = 0; i < 10; i++) { ... }` |
| Return | `buildReturnStatement()` | `return x + y;` |
| Break/Continue | `buildBreak()`, `buildContinue()` | `break;` |

---

#### buildIfStatement()

```cpp
/**
 * @brief Build if-else statement
 * @brief بناء جملة if-else
 * 
 * @param ifStmt If statement AST node
 * 
 * @details Creates CFG:
 *   entry → condition → then_block → merge
 *                    → else_block → merge
 */
void buildIfStatement(AST::IfStatement* ifStmt);
```

**Generated SIR**:
```
entry:
    %cond = ... (condition expression)
    BR %cond, then_block, else_block

then_block:
    ... (then statements)
    JUMP merge

else_block:
    ... (else statements)
    JUMP merge

merge:
    ... (continue)
```

---

#### buildWhileStatement()

```cpp
/**
 * @brief Build while loop
 * @brief بناء حلقة while
 * 
 * @param whileStmt While statement AST node
 * 
 * @details Creates CFG:
 *   entry → loop_cond → loop_body → loop_cond
 *                    → loop_exit
 */
void buildWhileStatement(AST::WhileStatement* whileStmt);
```

**Generated SIR**:
```
entry:
    JUMP loop_cond

loop_cond:
    %cond = ... (condition)
    BR %cond, loop_body, loop_exit

loop_body:
    ... (loop statements)
    JUMP loop_cond

loop_exit:
    ... (continue)
```

---

### Declaration Building | بناء التصريحات

#### buildFunctionDecl()

```cpp
/**
 * @brief Build function declaration
 * @brief بناء تصريح دالة
 * 
 * @param funcDecl Function declaration AST node
 * @return Shared pointer to SIR function
 */
std::shared_ptr<SIRFunction> buildFunctionDecl(
    AST::FunctionDeclaration* funcDecl
);
```

**Process**:
1. Create function signature
2. Enter new scope
3. Add parameters to scope
4. Build function body
5. Exit scope
6. Add to function table

**Example**:
```cpp
// S Language:
// function add(a: int, b: int) -> int {
//     return a + b;
// }

// Generated SIR:
function @add(i64 %a, i64 %b) -> i64 {
entry:
    %0 = ADD %a, %b
    RET %0
}
```

---

#### buildClassDecl()

```cpp
/**
 * @brief Build class declaration
 * @brief بناء تصريح صنف
 * 
 * @param classDecl Class declaration AST node
 * @return Shared pointer to SIR class
 */
std::shared_ptr<SIRClass> buildClassDecl(
    AST::ClassDeclaration* classDecl
);
```

**Process**:
1. Create class structure
2. Add fields
3. Build methods
4. Handle inheritance
5. Setup vtable (if needed)

**Example**:
```cpp
// S Language:
// class Point {
//     x: int;
//     y: int;
//     
//     function distance() -> float { ... }
// }

// Generated SIR:
struct @Point {
    i64 x
    i64 y
}

function @Point_distance(ptr %this) -> f64 {
    ...
}
```

---

## Helper Structures | الهياكل المساعدة

### BuildResult

```cpp
/**
 * @brief Result of building an expression
 * @brief نتيجة بناء تعبير
 */
struct BuildResult {
    std::string registerName;      // Virtual register name (%0, %1, etc.)
    SIRType type;                  // Data type
    bool isConstant;               // Is constant value?
    std::string constantValue;     // Constant value (if any)
    
    BuildResult();
    BuildResult(const std::string& reg, SIRType t);
    BuildResult(const std::string& value, SIRType t, bool isConst);
};
```

**Usage**:
```cpp
// Register result
BuildResult result("%0", SIRType::I64);

// Constant result
BuildResult constResult("42", SIRType::I64, true);
```

---

### VariableInfo

```cpp
/**
 * @brief Variable information in scope
 * @brief معلومات المتغير في النطاق
 */
struct VariableInfo {
    std::string name;              // Variable name
    SIRType type;                  // Data type
    std::string registerName;      // Associated register
    bool isGlobal;                 // Is global variable?
    bool isMutable;                // Is mutable?
    int scopeLevel;                // Scope depth level
    
    VariableInfo();
};
```

---

### TypeParameter

```cpp
/**
 * @brief Generic type parameter
 * @brief معامل النوع العام
 */
struct TypeParameter {
    std::string name;              // Parameter name (T, U, K, V)
    SIRType constraintType;        // Type constraint (optional)
    bool hasConstraint;            // Has constraint?
    
    TypeParameter();
    TypeParameter(const std::string& n);
};
```

**Example**:
```cpp
// function map<T, U>(arr: T[], fn: (T)->U) -> U[]
TypeParameter T("T");
TypeParameter U("U");
std::vector<TypeParameter> params = {T, U};
```

---

### GenericScope

```cpp
/**
 * @brief Generic type parameters scope
 * @brief نطاق معاملات الأنواع العامة
 */
struct GenericScope {
    std::vector<TypeParameter> typeParameters;
    std::unordered_map<std::string, SIRType> typeSubstitutions;
    
    bool hasTypeParameter(const std::string& name) const;
    SIRType getSubstitutedType(const std::string& name) const;
};
```

---

## Usage Examples | أمثلة الاستخدام

### Example 1: Simple Expression

```cpp
#include "sir_builder.h"

// S Language code:
// var x = 10 + 20;

// Parse AST
auto ast = parser.parse("var x = 10 + 20;");

// Build SIR
SIRBuilder builder;
auto module = builder.buildModule(ast);

// Check errors
if (builder.hasErrors()) {
    for (const auto& error : builder.getErrors()) {
        std::cerr << "Error: " << error << std::endl;
    }
    return;
}

// Print SIR
std::cout << module->toString() << std::endl;

// Expected output:
// %0 = LOAD_CONST 10
// %1 = LOAD_CONST 20
// %2 = ADD %0, %1
// %x = ALLOCA i64
// STORE %x, %2
```

---

### Example 2: Function with Control Flow

```cpp
// S Language code:
// function abs(x: int) -> int {
//     if (x < 0) {
//         return -x;
//     }
//     return x;
// }

auto ast = parser.parse(source);
SIRBuilder builder;
auto module = builder.buildModule(ast);

// Expected SIR:
// function @abs(i64 %x) -> i64 {
// entry:
//     %0 = ICMP_LT %x, 0
//     BR %0, then, else
// then:
//     %1 = NEG %x
//     RET %1
// else:
//     RET %x
// }
```

---

### Example 3: Generic Function

```cpp
// S Language code:
// function identity<T>(x: T) -> T {
//     return x;
// }
// 
// var result = identity<int>(42);

auto ast = parser.parse(source);
SIRBuilder builder;
auto module = builder.buildModule(ast);

// At definition: T is type parameter
// At call site: T → int
// Expected SIR (specialized):
// function @identity_int(i64 %x) -> i64 {
// entry:
//     RET %x
// }
```

---

### Example 4: Class with Methods

```cpp
// S Language code:
// class Counter {
//     value: int;
//     
//     function increment() {
//         value = value + 1;
//     }
//     
//     function getValue() -> int {
//         return value;
//     }
// }

auto ast = parser.parse(source);
SIRBuilder builder;
auto module = builder.buildModule(ast);

// Expected SIR:
// struct @Counter {
//     i64 value
// }
// 
// function @Counter_increment(ptr %this) -> void {
// entry:
//     %0 = MEMBER_ACCESS %this, 0  // value field
//     %1 = LOAD %0
//     %2 = ADD %1, 1
//     STORE %0, %2
//     RET
// }
// 
// function @Counter_getValue(ptr %this) -> i64 {
// entry:
//     %0 = MEMBER_ACCESS %this, 0
//     %1 = LOAD %0
//     RET %1
// }
```

---

## Error Handling | معالجة الأخطاء

### Error Accumulation

SIRBuilder accumulates all errors during compilation instead of stopping at the first error.

```cpp
SIRBuilder builder;
auto module = builder.buildModule(ast);

// Check for errors
if (builder.hasErrors()) {
    const auto& errors = builder.getErrors();
    std::cerr << "Compilation failed with " 
              << errors.size() << " errors:\n";
    
    for (size_t i = 0; i < errors.size(); i++) {
        std::cerr << (i + 1) << ". " << errors[i] << "\n";
    }
}
```

---

### Common Error Types

| Error Type | Description | Example |
|------------|-------------|---------|
| Type Mismatch | Incompatible types | `var x: int = "hello";` |
| Undefined Variable | Variable not declared | `y = x + 1;` (x not declared) |
| Undefined Function | Function not found | `result = foo();` (foo not defined) |
| Wrong Argument Count | Incorrect number of args | `add(1)` (add expects 2 args) |
| Break Outside Loop | Break not in loop | `if (x) { break; }` |
| Generic Type Error | Invalid type parameter | `function bad<t>() { }` (lowercase) |

---

## Generic Types | الأنواع العامة

### API Functions

#### enterGenericScope()

```cpp
/**
 * @brief Enter generic types scope
 * @brief دخول نطاق الأنواع العامة
 * 
 * @param typeParams List of type parameters
 */
void enterGenericScope(const std::vector<TypeParameter>& typeParams);
```

**Usage**:
```cpp
// function map<T, U>(...)
TypeParameter T("T"), U("U");
builder.enterGenericScope({T, U});
// ... build function body ...
builder.exitGenericScope();
```

---

#### setTypeSubstitution()

```cpp
/**
 * @brief Set type substitution at call site
 * @brief تعيين استبدال النوع عند الاستدعاء
 * 
 * @param paramName Type parameter name
 * @param concreteType Concrete type to substitute
 */
void setTypeSubstitution(const std::string& paramName, SIRType concreteType);
```

**Usage**:
```cpp
// map<int, string>(...)
builder.setTypeSubstitution("T", SIRType::I64);
builder.setTypeSubstitution("U", SIRType::STRING);
```

---

#### resolveType()

```cpp
/**
 * @brief Resolve type name to SIRType
 * @brief حل اسم النوع إلى SIRType
 * 
 * @param typeName Type name (may be generic parameter)
 * @return Resolved SIRType or VOID if not found
 */
SIRType resolveType(const std::string& typeName);
```

**Usage**:
```cpp
// Inside generic function body
SIRType type = builder.resolveType("T");
// If T was substituted with int → SIRType::I64
// If T not yet substituted → SIRType::VOID
```

---

#### validateTypeParameters()

```cpp
/**
 * @brief Validate type parameter list
 * @brief التحقق من صحة قائمة معاملات الأنواع
 * 
 * @param typeParams Type parameters to validate
 * @return true if valid, false otherwise
 */
bool validateTypeParameters(const std::vector<TypeParameter>& typeParams);
```

**Validation Rules**:
1. ✅ Names must start with uppercase letter (T, U, K, V)
2. ✅ No duplicate names
3. ✅ If constrained, constraint type must be valid

**Example**:
```cpp
TypeParameter T("T");     // ✓ Valid
TypeParameter t("t");     // ✗ Invalid (lowercase)
TypeParameter T2("T");    // ✗ Invalid (duplicate if T exists)
```

---

## Performance Considerations | اعتبارات الأداء

### Virtual Register Allocation

- Uses sequential numbering: `%0`, `%1`, `%2`, ...
- O(1) allocation time
- Unlimited virtual registers (optimization phase will reduce)

### Scope Management

- Stack-based scope management
- O(1) scope enter/exit
- O(log n) variable lookup (hash map)

### Generic Types

- Type resolution: O(k) where k = nesting depth
- Type substitution: O(1) hash map lookup
- Validation: O(n) where n = number of type parameters

---

## Thread Safety | السلامة في البيئات المتعددة

⚠️ **Warning**: SIRBuilder is **not thread-safe**.

- Create separate SIRBuilder instance per thread
- Do not share SIRBuilder across threads
- Output SIRModule can be shared (read-only)

```cpp
// ✓ Correct: Separate builders
void compileInThread(const std::string& source) {
    SIRBuilder builder;  // Thread-local
    auto module = builder.buildModule(parse(source));
}

// ✗ Incorrect: Shared builder
SIRBuilder sharedBuilder;  // Don't do this!
void compileInThread(const std::string& source) {
    auto module = sharedBuilder.buildModule(parse(source));  // Race condition!
}
```

---

## Limitations | القيود

### Current Limitations

1. **No Optimization**: SIRBuilder generates unoptimized SIR (optimizer is separate phase)
2. **No Type Inference**: Types must be explicitly specified
3. **Limited Generic Constraints**: Only basic type constraints supported
4. **No Macros**: Macro expansion happens in preprocessor (future)

### Future Enhancements

- 🔮 Template specialization caching
- 🔮 Incremental compilation support
- 🔮 Better error recovery
- 🔮 Source location tracking in SIR
- 🔮 Debug information generation

---

## See Also | انظر أيضاً

- [SIR Specification](../language_spec/SIR_SPECIFICATION.md)
- [Type System Guide](TYPE_SYSTEM_GUIDE.md)
- [Developer Guide](DEVELOPER_GUIDE.md)
- [Architecture Overview](../architecture/COMPILER_ARCHITECTURE.md)

---

**Document Version**: 1.0  
**Last Updated**: December 3, 2025  
**Maintained By**: SadLanguage Compiler Team
