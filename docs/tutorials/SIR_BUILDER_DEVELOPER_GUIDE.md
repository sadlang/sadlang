# SIR Builder Developer Guide
# دليل مطور بناء SIR

**Version**: 1.0  
**Date**: December 3, 2025  
**Target Audience**: Compiler developers extending SIRBuilder

---

## 📋 Table of Contents

1. [Introduction](#introduction)
2. [Architecture Overview](#architecture-overview)
3. [Extending SIRBuilder](#extending-sirbuilder)
4. [Adding New Instructions](#adding-new-instructions)
5. [Adding New Types](#adding-new-types)
6. [Adding New Operators](#adding-new-operators)
7. [Generic Types Implementation](#generic-types-implementation)
8. [Testing Guidelines](#testing-guidelines)
9. [Debugging Tips](#debugging-tips)
10. [Best Practices](#best-practices)

---

## Introduction | مقدمة

This guide helps compiler developers extend and maintain **SIRBuilder**, the AST-to-SIR conversion component of the S Language compiler.

**Prerequisites**:
- C++17 knowledge
- Understanding of compiler design
- Familiarity with AST and IR concepts
- Knowledge of S Language syntax

**What You'll Learn**:
- How to add new language features
- How to extend the type system
- How to add new SIR instructions
- Best practices for maintainability

---

## Architecture Overview | نظرة معمارية

### Component Layers

```
┌─────────────────────────────────────────┐
│         AST (Abstract Syntax Tree)      │
│  - Expression, Statement, Declaration   │
└────────────────┬────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────┐
│            SIRBuilder                   │
│  - buildExpression()                    │
│  - buildStatement()                     │
│  - buildDeclaration()                   │
│  - Virtual Register Management          │
│  - Scope Management                     │
│  - Type System                          │
│  - Generic Types                        │
└────────────────┬────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────┐
│         SIR (S Intermediate Rep)        │
│  - Three-Address Code (TAC)             │
│  - Unlimited Virtual Registers          │
│  - Control Flow Graph (CFG)             │
│  - Type Information                     │
└─────────────────────────────────────────┘
```

### Key Design Principles

1. **Single Responsibility**: Each method handles one AST node type
2. **Error Accumulation**: Collect all errors, don't stop at first
3. **Type Safety**: Strict type checking during conversion
4. **Virtual Registers**: Unlimited registers (optimizer will reduce)
5. **Immutable AST**: Never modify input AST
6. **Explicit Types**: No implicit conversions (unless safe)

---

## Extending SIRBuilder | توسيع بناء SIR

### Step-by-Step Extension Process

#### 1. Identify the Feature

**Example**: Adding ternary operator `? :`

```cpp
// S Language syntax:
var result = (x > 0) ? x : -x;  // Absolute value
```

#### 2. Define AST Node (if new)

```cpp
// In ast.h
class TernaryExpression : public Expression {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> trueExpr;
    std::unique_ptr<Expression> falseExpr;
    
    TernaryExpression(
        std::unique_ptr<Expression> cond,
        std::unique_ptr<Expression> t,
        std::unique_ptr<Expression> f
    ) : condition(std::move(cond)),
        trueExpr(std::move(t)),
        falseExpr(std::move(f)) {}
};
```

#### 3. Add Builder Method

```cpp
// In sir_builder.h
class SIRBuilder {
private:
    /**
     * @brief Build ternary expression (condition ? true : false)
     */
    BuildResult buildTernaryExpression(AST::TernaryExpression* ternary);
};
```

#### 4. Implement Conversion Logic

```cpp
// In sir_builder.cpp
BuildResult SIRBuilder::buildTernaryExpression(AST::TernaryExpression* ternary) {
    // Step 1: Build condition
    auto condResult = buildExpression(ternary->condition.get());
    
    // Step 2: Create basic blocks
    std::string trueBlock = getNextLabel();   // "true_block_0"
    std::string falseBlock = getNextLabel();  // "false_block_1"
    std::string mergeBlock = getNextLabel();  // "merge_block_2"
    
    // Step 3: Branch on condition
    auto brInstr = std::make_shared<SIRBranchInstruction>(
        condResult.registerName, trueBlock, falseBlock
    );
    currentBlock_->addInstruction(brInstr);
    
    // Step 4: Build true branch
    auto trueBlockObj = std::make_shared<SIRBasicBlock>(trueBlock);
    currentBlock_ = trueBlockObj;
    auto trueResult = buildExpression(ternary->trueExpr.get());
    std::string trueReg = trueResult.registerName;
    
    // Jump to merge
    auto trueJump = std::make_shared<SIRJumpInstruction>(mergeBlock);
    currentBlock_->addInstruction(trueJump);
    currentFunction_->addBlock(trueBlockObj);
    
    // Step 5: Build false branch
    auto falseBlockObj = std::make_shared<SIRBasicBlock>(falseBlock);
    currentBlock_ = falseBlockObj;
    auto falseResult = buildExpression(ternary->falseExpr.get());
    std::string falseReg = falseResult.registerName;
    
    // Jump to merge
    auto falseJump = std::make_shared<SIRJumpInstruction>(mergeBlock);
    currentBlock_->addInstruction(falseJump);
    currentFunction_->addBlock(falseBlockObj);
    
    // Step 6: Create merge block with PHI node
    auto mergeBlockObj = std::make_shared<SIRBasicBlock>(mergeBlock);
    currentBlock_ = mergeBlockObj;
    
    // Create PHI node to select value
    std::string resultReg = getNextRegister();
    auto phiInstr = std::make_shared<SIRPhiInstruction>(
        resultReg,
        trueResult.type,
        std::vector<std::pair<std::string, std::string>>{
            {trueReg, trueBlock},
            {falseReg, falseBlock}
        }
    );
    currentBlock_->addInstruction(phiInstr);
    currentFunction_->addBlock(mergeBlockObj);
    
    // Step 7: Verify type compatibility
    if (!areTypesCompatible(trueResult.type, falseResult.type)) {
        reportError("Ternary operator branches have incompatible types");
    }
    
    return BuildResult(resultReg, trueResult.type);
}
```

#### 5. Integrate into buildExpression()

```cpp
BuildResult SIRBuilder::buildExpression(AST::Expression* expr) {
    // ... existing cases ...
    
    if (auto* ternary = dynamic_cast<AST::TernaryExpression*>(expr)) {
        return buildTernaryExpression(ternary);
    }
    
    // ... rest of cases ...
}
```

#### 6. Add Unit Tests

```cpp
// In test_sir_builder.cpp
TEST_CASE("Ternary Expression") {
    // Test: (x > 0) ? x : -x
    auto ast = createTernaryAST();
    
    SIRBuilder builder;
    auto result = builder.buildTernaryExpression(ast);
    
    REQUIRE(!builder.hasErrors());
    REQUIRE(result.type == SIRType::I64);
    
    // Verify CFG structure
    auto blocks = builder.getCurrentFunction()->getBlocks();
    REQUIRE(blocks.size() == 4);  // entry, true, false, merge
    
    // Verify PHI node in merge block
    auto mergeBlock = blocks[3];
    auto instructions = mergeBlock->getInstructions();
    REQUIRE(instructions[0]->getOpcode() == SIROpcode::PHI);
}
```

---

## Adding New Instructions | إضافة تعليمات جديدة

### Process Overview

1. Define instruction in SIR specification
2. Add opcode enum value
3. Create instruction class
4. Implement toString() for debugging
5. Add builder helper method
6. Test thoroughly

### Example: Adding ATOMIC_ADD Instruction

#### Step 1: Add Opcode

```cpp
// In sir_instruction.h
enum class SIROpcode {
    // ... existing opcodes ...
    ATOMIC_ADD,      // Atomic addition
    ATOMIC_SUB,      // Atomic subtraction
    ATOMIC_CAS,      // Compare-and-swap
    // ...
};
```

#### Step 2: Create Instruction Class

```cpp
// In sir_instruction.h
/**
 * @brief Atomic addition instruction
 * @example %result = ATOMIC_ADD ptr %addr, %value
 */
class SIRAtomicAddInstruction : public SIRInstruction {
private:
    std::string result_;       // Result register
    std::string address_;      // Memory address
    std::string value_;        // Value to add
    SIRType type_;             // Data type
    
public:
    SIRAtomicAddInstruction(
        const std::string& result,
        const std::string& address,
        const std::string& value,
        SIRType type
    ) : result_(result), address_(address), 
        value_(value), type_(type) {}
    
    SIROpcode getOpcode() const override {
        return SIROpcode::ATOMIC_ADD;
    }
    
    std::string toString() const override {
        return result_ + " = ATOMIC_ADD ptr " + 
               address_ + ", " + value_;
    }
    
    // Getters
    const std::string& getResult() const { return result_; }
    const std::string& getAddress() const { return address_; }
    const std::string& getValue() const { return value_; }
    SIRType getType() const { return type_; }
};
```

#### Step 3: Add Builder Helper

```cpp
// In sir_builder.h
class SIRBuilder {
private:
    /**
     * @brief Emit atomic addition instruction
     */
    std::string emitAtomicAdd(
        const std::string& address,
        const std::string& value,
        SIRType type
    );
};

// In sir_builder.cpp
std::string SIRBuilder::emitAtomicAdd(
    const std::string& address,
    const std::string& value,
    SIRType type
) {
    std::string result = getNextRegister();
    
    auto instr = std::make_shared<SIRAtomicAddInstruction>(
        result, address, value, type
    );
    
    currentBlock_->addInstruction(instr);
    
    return result;
}
```

#### Step 4: Use in High-Level Code

```cpp
// S Language:
// atomic_add(&counter, 1);

BuildResult SIRBuilder::buildAtomicAddCall(AST::FunctionCall* call) {
    // Get address argument
    auto addrResult = buildExpression(call->arguments[0].get());
    
    // Get value argument
    auto valueResult = buildExpression(call->arguments[1].get());
    
    // Emit atomic add
    std::string result = emitAtomicAdd(
        addrResult.registerName,
        valueResult.registerName,
        valueResult.type
    );
    
    return BuildResult(result, valueResult.type);
}
```

---

## Adding New Types | إضافة أنواع جديدة

### Process Overview

1. Add to SIRType enum
2. Update type conversion functions
3. Add compatibility rules
4. Update string conversion
5. Test type checking

### Example: Adding Complex Number Type

#### Step 1: Define Type

```cpp
// In sir_type.h
enum class SIRType {
    // ... existing types ...
    COMPLEX,      // Complex number (real + imaginary)
    // ...
};
```

#### Step 2: Add Type Information

```cpp
// In sir_builder.cpp
std::string SIRBuilder::typeToString(SIRType type) {
    switch (type) {
        // ... existing cases ...
        case SIRType::COMPLEX: return "complex";
        // ...
    }
}

size_t SIRBuilder::getTypeSize(SIRType type) {
    switch (type) {
        // ... existing cases ...
        case SIRType::COMPLEX: return 16;  // 2 x f64
        // ...
    }
}
```

#### Step 3: Update Type Compatibility

```cpp
bool SIRBuilder::areTypesCompatible(SIRType from, SIRType to) {
    if (from == to) return true;
    
    // ... existing rules ...
    
    // Complex number conversions
    if (to == SIRType::COMPLEX) {
        // Can convert int/float to complex
        return from == SIRType::I64 || 
               from == SIRType::F64;
    }
    
    return false;
}
```

#### Step 4: Add Conversion Logic

```cpp
BuildResult SIRBuilder::convertType(
    const BuildResult& value,
    SIRType targetType
) {
    // ... existing conversions ...
    
    // Convert to complex
    if (targetType == SIRType::COMPLEX) {
        if (value.type == SIRType::I64 || value.type == SIRType::F64) {
            std::string result = getNextRegister();
            
            // Create complex from real part (imaginary = 0)
            auto instr = std::make_shared<SIRComplexCreateInstruction>(
                result, value.registerName, "0.0"
            );
            
            currentBlock_->addInstruction(instr);
            return BuildResult(result, SIRType::COMPLEX);
        }
    }
    
    reportError("Cannot convert type");
    return value;
}
```

#### Step 5: Add Operations

```cpp
BuildResult SIRBuilder::buildComplexAdd(
    const BuildResult& left,
    const BuildResult& right
) {
    std::string result = getNextRegister();
    
    // Complex addition: (a+bi) + (c+di) = (a+c) + (b+d)i
    auto instr = std::make_shared<SIRComplexAddInstruction>(
        result,
        left.registerName,
        right.registerName
    );
    
    currentBlock_->addInstruction(instr);
    
    return BuildResult(result, SIRType::COMPLEX);
}
```

---

## Adding New Operators | إضافة عمليات جديدة

### Example: Adding Power Operator `**`

#### Step 1: Update AST

```cpp
// In ast.h
enum class BinaryOperator {
    // ... existing operators ...
    POWER,  // **
    // ...
};
```

#### Step 2: Add SIR Instruction

```cpp
// In sir_instruction.h
class SIRPowerInstruction : public SIRInstruction {
private:
    std::string result_;
    std::string base_;
    std::string exponent_;
    SIRType type_;
    
public:
    SIRPowerInstruction(
        const std::string& result,
        const std::string& base,
        const std::string& exponent,
        SIRType type
    ) : result_(result), base_(base), 
        exponent_(exponent), type_(type) {}
    
    std::string toString() const override {
        return result_ + " = POW " + base_ + ", " + exponent_;
    }
};
```

#### Step 3: Handle in buildBinaryOp()

```cpp
BuildResult SIRBuilder::buildBinaryOp(
    const BuildResult& left,
    AST::BinaryOperator op,
    const BuildResult& right
) {
    // ... existing cases ...
    
    if (op == AST::BinaryOperator::POWER) {
        // x ** y
        std::string result = getNextRegister();
        
        // Type must be numeric
        if (!isNumericType(left.type) || !isNumericType(right.type)) {
            reportError("Power operator requires numeric types");
            return BuildResult();
        }
        
        // Convert to float if needed
        auto leftConverted = left;
        auto rightConverted = right;
        
        if (left.type == SIRType::I64) {
            leftConverted = convertType(left, SIRType::F64);
        }
        if (right.type == SIRType::I64) {
            rightConverted = convertType(right, SIRType::F64);
        }
        
        // Emit power instruction
        auto instr = std::make_shared<SIRPowerInstruction>(
            result,
            leftConverted.registerName,
            rightConverted.registerName,
            SIRType::F64
        );
        
        currentBlock_->addInstruction(instr);
        
        return BuildResult(result, SIRType::F64);
    }
    
    // ... rest of cases ...
}
```

---

## Generic Types Implementation | تطبيق الأنواع العامة

### Design Overview

Generic types use **stack-based scope management** with **type substitution**.

```
Generic Scope Stack:
┌──────────────────────────────────┐
│ Scope 2: <K, V>                  │  ← Current (innermost)
│   Substitutions: K→int, V→string │
├──────────────────────────────────┤
│ Scope 1: <T, U>                  │
│   Substitutions: T→int, U→float  │
└──────────────────────────────────┘
```

### Workflow

#### 1. Function Definition with Generics

```cpp
// S Language:
// function map<T, U>(arr: T[], fn: (T)->U) -> U[] { ... }

void SIRBuilder::buildGenericFunctionDecl(AST::FunctionDeclaration* funcDecl) {
    // Step 1: Extract type parameters
    std::vector<TypeParameter> typeParams;
    for (const auto& param : funcDecl->typeParameters) {
        typeParams.push_back(TypeParameter(param.name));
    }
    
    // Step 2: Validate
    if (!validateTypeParameters(typeParams)) {
        return;  // Error reported
    }
    
    // Step 3: Enter generic scope
    enterGenericScope(typeParams);
    
    // Step 4: Build function body
    // Inside body, T and U are available as type names
    for (auto* stmt : funcDecl->body->statements) {
        buildStatement(stmt);
    }
    
    // Step 5: Exit scope
    exitGenericScope();
}
```

#### 2. Function Call with Type Arguments

```cpp
// S Language:
// var result = map<int, string>([1, 2, 3], toString);

BuildResult SIRBuilder::buildGenericFunctionCall(AST::FunctionCall* call) {
    // Step 1: Find generic function
    auto* funcDecl = findFunction(call->functionName);
    
    // Step 2: Enter scope for specialization
    std::vector<TypeParameter> typeParams = funcDecl->getTypeParameters();
    enterGenericScope(typeParams);
    
    // Step 3: Set type substitutions
    for (size_t i = 0; i < call->typeArguments.size(); i++) {
        std::string paramName = typeParams[i].name;
        SIRType concreteType = parseType(call->typeArguments[i]);
        
        setTypeSubstitution(paramName, concreteType);
    }
    
    // Step 4: Build call with concrete types
    auto result = buildFunctionCallInternal(call);
    
    // Step 5: Exit scope
    exitGenericScope();
    
    return result;
}
```

#### 3. Type Resolution

```cpp
SIRType SIRBuilder::resolveType(const std::string& typeName) {
    // Step 1: Check generic scopes (innermost to outermost)
    for (auto it = genericScopeStack_.rbegin(); 
         it != genericScopeStack_.rend(); ++it) {
        
        if (it->hasTypeParameter(typeName)) {
            // Found type parameter
            SIRType substituted = it->getSubstitutedType(typeName);
            
            if (substituted != SIRType::VOID) {
                return substituted;  // Has substitution
            }
            
            // Parameter exists but not yet substituted
            return SIRType::VOID;
        }
    }
    
    // Step 2: Try built-in types
    if (typeName == "int") return SIRType::I64;
    if (typeName == "float") return SIRType::F64;
    if (typeName == "string") return SIRType::STRING;
    if (typeName == "bool") return SIRType::BOOL;
    
    // Step 3: Unknown type
    return SIRType::VOID;
}
```

---

## Testing Guidelines | إرشادات الاختبار

### Test Categories

1. **Unit Tests**: Individual methods
2. **Integration Tests**: Complete programs
3. **Edge Cases**: Error conditions, boundaries
4. **Performance Tests**: Large programs

### Unit Test Template

```cpp
TEST_CASE("Feature Name") {
    SECTION("Happy path") {
        // Setup
        auto ast = createTestAST();
        SIRBuilder builder;
        
        // Execute
        auto result = builder.buildExpression(ast);
        
        // Verify
        REQUIRE(!builder.hasErrors());
        REQUIRE(result.type == SIRType::I64);
        REQUIRE(result.registerName == "%0");
    }
    
    SECTION("Error case") {
        auto invalidAST = createInvalidAST();
        SIRBuilder builder;
        
        auto result = builder.buildExpression(invalidAST);
        
        REQUIRE(builder.hasErrors());
        REQUIRE(builder.getErrors().size() == 1);
        REQUIRE(builder.getErrors()[0].find("type mismatch") != std::string::npos);
    }
}
```

### Integration Test Template

```cpp
TEST_CASE("Complete Program - Feature X") {
    const char* source = R"(
        function main() {
            var x = 10;
            var y = x + 20;
            return y;
        }
    )";
    
    // Parse
    Parser parser;
    auto ast = parser.parse(source);
    REQUIRE(ast != nullptr);
    
    // Build SIR
    SIRBuilder builder;
    auto module = builder.buildModule(ast);
    
    // Verify
    REQUIRE(!builder.hasErrors());
    REQUIRE(module != nullptr);
    REQUIRE(module->getFunctions().size() == 1);
    
    // Verify SIR structure
    auto mainFunc = module->getFunction("main");
    REQUIRE(mainFunc != nullptr);
    REQUIRE(mainFunc->getBlocks().size() >= 1);
    
    // Verify instructions
    auto entryBlock = mainFunc->getBlock("entry");
    auto instructions = entryBlock->getInstructions();
    REQUIRE(instructions.size() >= 3);
    
    // Verify specific instruction types
    REQUIRE(instructions[0]->getOpcode() == SIROpcode::ALLOCA);
    REQUIRE(instructions[1]->getOpcode() == SIROpcode::STORE);
}
```

---

## Debugging Tips | نصائح التصحيح

### Enable Verbose Logging

```cpp
// In sir_builder.cpp
#define SIR_DEBUG 1

#ifdef SIR_DEBUG
#define DEBUG_LOG(msg) std::cout << "[SIR] " << msg << std::endl
#else
#define DEBUG_LOG(msg)
#endif

BuildResult SIRBuilder::buildExpression(AST::Expression* expr) {
    DEBUG_LOG("Building expression: " << expr->getTypeName());
    // ...
}
```

### Dump SIR at Each Stage

```cpp
void SIRBuilder::dumpCurrentFunction() {
    if (currentFunction_) {
        std::cout << "=== Current Function: " 
                  << currentFunction_->getName() << " ===" << std::endl;
        std::cout << currentFunction_->toString() << std::endl;
    }
}

// Call after each major operation
buildStatement(stmt);
dumpCurrentFunction();
```

### Validate Register Usage

```cpp
void SIRBuilder::validateRegister(const std::string& reg) {
    if (reg.empty()) {
        reportError("Empty register name");
        return;
    }
    
    if (reg[0] != '%') {
        reportError("Register must start with %: " + reg);
        return;
    }
    
    // Check if register was allocated
    if (allocatedRegisters_.count(reg) == 0) {
        reportError("Using unallocated register: " + reg);
    }
}
```

### Check Type Consistency

```cpp
void SIRBuilder::validateInstruction(SIRInstruction* instr) {
    switch (instr->getOpcode()) {
        case SIROpcode::ADD: {
            auto* addInstr = static_cast<SIRAddInstruction*>(instr);
            
            // Check operand types match
            SIRType leftType = getRegisterType(addInstr->getLeft());
            SIRType rightType = getRegisterType(addInstr->getRight());
            
            if (leftType != rightType) {
                reportError("ADD operands have different types");
            }
            break;
        }
        // ... other instructions ...
    }
}
```

---

## Best Practices | أفضل الممارسات

### 1. Error Messages

**❌ Bad**:
```cpp
reportError("Invalid type");
```

**✅ Good**:
```cpp
reportError("Type mismatch in binary operation: cannot add " +
            typeToString(left.type) + " and " + 
            typeToString(right.type));
```

### 2. Code Organization

**❌ Bad** - Giant method:
```cpp
BuildResult SIRBuilder::buildExpression(AST::Expression* expr) {
    // 500 lines of if-else...
}
```

**✅ Good** - Split by type:
```cpp
BuildResult SIRBuilder::buildExpression(AST::Expression* expr) {
    if (auto* binOp = dynamic_cast<AST::BinaryExpression*>(expr)) {
        return buildBinaryOp(binOp);
    }
    // ... dispatch to specialized methods ...
}

BuildResult SIRBuilder::buildBinaryOp(AST::BinaryExpression* expr) {
    // Focused logic for binary operations
}
```

### 3. Type Safety

**❌ Bad** - Unchecked cast:
```cpp
auto* funcDecl = (AST::FunctionDeclaration*)decl;
```

**✅ Good** - Safe cast with check:
```cpp
auto* funcDecl = dynamic_cast<AST::FunctionDeclaration*>(decl);
if (!funcDecl) {
    reportError("Expected function declaration");
    return;
}
```

### 4. Resource Management

**❌ Bad** - Raw pointer:
```cpp
SIRInstruction* instr = new SIRAddInstruction(...);
currentBlock_->addInstruction(instr);  // Memory leak risk
```

**✅ Good** - Smart pointer:
```cpp
auto instr = std::make_shared<SIRAddInstruction>(...);
currentBlock_->addInstruction(instr);  // Automatic cleanup
```

### 5. Validation

**❌ Bad** - Assume valid:
```cpp
auto result = buildExpression(expr);
return result.registerName;  // What if expr was invalid?
```

**✅ Good** - Check result:
```cpp
auto result = buildExpression(expr);
if (result.registerName.empty()) {
    reportError("Failed to build expression");
    return BuildResult();  // Return invalid result
}
return result;
```

### 6. Documentation

**❌ Bad** - No docs:
```cpp
void foo(int x) {
    // ...
}
```

**✅ Good** - Clear documentation:
```cpp
/**
 * @brief Build SIR for function call expression
 * 
 * @param call Function call AST node
 * @return BuildResult with return value register and type
 * 
 * @note Handles both regular and generic function calls
 * @throws Nothing (errors reported via reportError())
 */
BuildResult buildFunctionCall(AST::FunctionCall* call);
```

---

## Common Pitfalls | الأخطاء الشائعة

### Pitfall 1: Forgetting to Allocate Register

**❌ Wrong**:
```cpp
auto instr = std::make_shared<SIRAddInstruction>(
    "result",  // Oops! Not a register
    "%0",
    "%1"
);
```

**✅ Correct**:
```cpp
std::string result = getNextRegister();  // "%2"
auto instr = std::make_shared<SIRAddInstruction>(
    result,
    "%0",
    "%1"
);
```

### Pitfall 2: Not Handling Null Pointers

**❌ Wrong**:
```cpp
void buildFunction(AST::FunctionDeclaration* funcDecl) {
    for (auto* stmt : funcDecl->body->statements) {  // Crash if body is null!
        buildStatement(stmt);
    }
}
```

**✅ Correct**:
```cpp
void buildFunction(AST::FunctionDeclaration* funcDecl) {
    if (!funcDecl || !funcDecl->body) {
        reportError("Function has no body");
        return;
    }
    
    for (auto* stmt : funcDecl->body->statements) {
        if (stmt) {  // Also check each statement
            buildStatement(stmt);
        }
    }
}
```

### Pitfall 3: Modifying Current Block After Jump

**❌ Wrong**:
```cpp
// Jump to next block
auto jump = std::make_shared<SIRJumpInstruction>("next");
currentBlock_->addInstruction(jump);

// WRONG: Adding more instructions after jump!
auto deadCode = std::make_shared<SIRReturnInstruction>("%0");
currentBlock_->addInstruction(deadCode);
```

**✅ Correct**:
```cpp
// Jump to next block
auto jump = std::make_shared<SIRJumpInstruction>("next");
currentBlock_->addInstruction(jump);

// Switch to next block before adding more instructions
currentBlock_ = nextBlock;
auto ret = std::make_shared<SIRReturnInstruction>("%0");
currentBlock_->addInstruction(ret);
```

---

## Performance Optimization | تحسين الأداء

### Optimization 1: Pre-allocate Vectors

```cpp
// Reserve space if size is known
std::vector<std::shared_ptr<SIRInstruction>> instructions;
instructions.reserve(estimatedSize);
```

### Optimization 2: Use Hash Maps for Lookups

```cpp
// Fast O(1) lookup
std::unordered_map<std::string, VariableInfo> symbolTable_;

// Instead of O(n) vector search
// std::vector<VariableInfo> symbolTable_;  // Slow!
```

### Optimization 3: Cache Type Information

```cpp
// Cache register types
std::unordered_map<std::string, SIRType> registerTypes_;

SIRType getRegisterType(const std::string& reg) {
    auto it = registerTypes_.find(reg);
    if (it != registerTypes_.end()) {
        return it->second;  // Cached
    }
    // ... compute and cache ...
}
```

---

## Conclusion | الخلاصة

This guide covered:
- ✅ Architecture and design principles
- ✅ Extending SIRBuilder with new features
- ✅ Adding instructions, types, and operators
- ✅ Generic types implementation
- ✅ Testing and debugging strategies
- ✅ Best practices and common pitfalls

**Next Steps**:
1. Read [SIR Builder API Reference](../api/SIR_BUILDER_API.md)
2. Study existing code in `sir_builder.cpp`
3. Write tests before implementing features
4. Follow code review checklist

**Resources**:
- [SIR Specification](../language_spec/SIR_SPECIFICATION.md)
- [Architecture Diagrams](../architecture/)
- [Example Programs](../../examples/)

---

**Document Version**: 1.0  
**Last Updated**: December 3, 2025  
**Maintained By**: SadLanguage Compiler Team
