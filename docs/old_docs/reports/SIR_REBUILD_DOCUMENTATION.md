# توثيق إعادة كتابة SIR - SIR Rebuild Documentation
**التاريخ / Date**: 4 يناير 2026  
**المؤلف / Author**: GitHub Copilot  
**الالتزام / Compliance**: STRICT_CODING_RULES.md

---

## 📋 المحتويات / Table of Contents

1. [📖 فهم القاعدة الذهبية](#golden-rule)
2. [🔍 توثيق AST الحالي](#current-ast)
3. [🔍 توثيق SIR الحالي](#current-sir)
4. [🔄 مصفوفة التوافق](#compatibility-matrix)
5. [📝 خطة إعادة الكتابة](#rebuild-plan)
6. [✅ قائمة التحقق](#checklist)

---

## <a id="golden-rule"></a>📖 1. القاعدة الذهبية / Golden Rule

### من STRICT_CODING_RULES.md:
> **"DO NOT write any function or variable unless you have READ its full definition from the source file"**

### الإجراء الإلزامي / Mandatory Procedure:
1. ✅ قراءة كل header بالكامل
2. ✅ توثيق كل دالة بالملف والسطر
3. ✅ توثيق كل متغير بالملف والسطر  
4. ✅ التحقق من النوع بدقة (unique_ptr vs shared_ptr vs raw pointer)
5. ✅ عدم إعادة تنفيذ دوال inline
6. ✅ اختبار البناء بعد كل تعديل

---

## <a id="current-ast"></a>🔍 2. توثيق AST الحالي / Current AST Documentation

### 📁 الملفات / Files:
- **include/parser/ast/ast_node.h** (176 سطر)
- **include/parser/ast/expressions.h** (932 سطر)
- **include/parser/ast/statements.h** (651 سطر)
- **include/parser/ast/declarations.h** (500 سطر)

---

### 📦 2.1. الأنواع الأساسية / Base Types

#### من ast_node.h:

| النوع / Type | السطر / Line | الملف / File | الوصف / Description |
|-------------|-------------|-------------|---------------------|
| `ASTNode` | 33 | ast_node.h | الفئة الأساسية لجميع عقد AST |
| `Expression` | 95 | ast_node.h | الفئة الأساسية للتعابير |
| `Statement` | 126 | ast_node.h | الفئة الأساسية للجمل |
| `ExprPtr` | 146 | ast_node.h | `std::unique_ptr<Expression>` |
| `StmtPtr` | 159 | ast_node.h | `std::unique_ptr<Statement>` |
| `ExprList` | 165 | ast_node.h | `std::vector<ExprPtr>` |
| `StmtList` | 170 | ast_node.h | `std::vector<StmtPtr>` |

**ملاحظة مهمة جداً:**
- ✅ **StmtList** هو نوع البرنامج الأساسي (`std::vector<StmtPtr>`)
- ✅ **لا يوجد** `ProgramNode` في الكود الحالي
- ✅ البرنامج هو مباشرة `StmtList` (قائمة من العبارات)

---

### 📦 2.2. التعابير / Expressions

#### من expressions.h:

| الفئة / Class | السطر / Line | الملف / File | الأعضاء / Members |
|--------------|-------------|-------------|-------------------|
| `BinaryExpr` | 36 | expressions.h | `ExprPtr left`, `TokenType op`, `ExprPtr right` |
| `UnaryExpr` | 81 | expressions.h | `TokenType op`, `ExprPtr operand` |
| `TernaryExpr` | 130 | expressions.h | `ExprPtr condition`, `ExprPtr trueExpr`, `ExprPtr falseExpr` |
| `LiteralExpr` | 174 | expressions.h | `Token token` |
| `VariableExpr` | 210 | expressions.h | `std::string name` |
| `AssignExpr` | 247 | expressions.h | `std::string name`, `ExprPtr value` |
| `CallExpr` | 289 | expressions.h | `ExprPtr callee`, `ExprList arguments` |
| `IndexExpr` | 326 | expressions.h | `ExprPtr object`, `ExprPtr index` |
| `MemberExpr` | 367 | expressions.h | `ExprPtr object`, `std::string member` |
| `MemberAssignExpr` | 408 | expressions.h | `ExprPtr object`, `std::string member`, `ExprPtr value` |
| `ArrayExpr` | 447 | expressions.h | `ExprList elements` |
| `MapExpr` | 482 | expressions.h | `std::vector<MapPair> pairs` |

**الأعضاء الرئيسية:**
```cpp
// BinaryExpr - expressions.h:36
ExprPtr left;           // line 38
TokenType op;           // line 39  
ExprPtr right;          // line 40
void accept(ASTVisitor&) override;  // line 51 (inline)
std::string toString() const override;  // line 55 (inline)
DataType getType() const override;  // line 61 (NOT inline - needs .cpp)

// VariableExpr - expressions.h:210
std::string name;       // line 212
void accept(ASTVisitor&) override;  // line 221 (inline)
std::string toString() const override;  // line 225 (inline)
DataType getType() const override;  // line 229 (NOT inline - needs .cpp)
```

---

### 📦 2.3. الجمل / Statements

#### من statements.h:

| الفئة / Class | السطر / Line | الملف / File | الأعضاء / Members |
|--------------|-------------|-------------|-------------------|
| `ExprStmt` | 34 | statements.h | `ExprPtr expression` |
| `VarDeclStmt` | 73 | statements.h | `std::string name`, `DataType type`, `ExprPtr initializer`, `bool isConst` |
| `IfStmt` | 115 | statements.h | `ExprPtr condition`, `StmtPtr thenBranch`, `StmtPtr elseBranch` |
| `WhileStmt` | 154 | statements.h | `ExprPtr condition`, `StmtPtr body` |
| `ForStmt` | 190 | statements.h | `StmtPtr initializer`, `ExprPtr condition`, `ExprPtr increment`, `StmtPtr body` |
| `ForRangeStmt` | 234 | statements.h | `std::string variable`, `std::string valueVar`, `ExprPtr iterable`, `StmtPtr body` |
| `ReturnStmt` | 266 | statements.h | `ExprPtr value` |
| `YieldStmt` | 301 | statements.h | `ExprPtr value`, `bool isYieldFrom` |
| `BreakStmt` | 360 | statements.h | (لا أعضاء / no members) |
| `ContinueStmt` | 388 | statements.h | (لا أعضاء / no members) |
| `BlockStmt` | 416 | statements.h | `StmtList statements` |
| `TryStmt` | 485 | statements.h | `StmtPtr tryBlock`, `std::vector<CatchClause> catchClauses`, `StmtPtr finallyBlock` |

**الأعضاء الرئيسية:**
```cpp
// VarDeclStmt - statements.h:73
std::string name;           // line 75
DataType type;              // line 76
ExprPtr initializer;        // line 77
bool isConst;               // line 78
void accept(ASTVisitor&) override;  // line 93 (inline)
std::string toString() const override;  // line 97 (NOT inline)

// IfStmt - statements.h:115  
ExprPtr condition;          // line 117
StmtPtr thenBranch;         // line 118
StmtPtr elseBranch;         // line 119
void accept(ASTVisitor&) override;  // line 133 (inline)
std::string toString() const override;  // line 137 (NOT inline)
```

---

### 📦 2.4. التصريحات / Declarations

#### من declarations.h:

| الفئة / Class | السطر / Line | الملف / File | الأعضاء / Members |
|--------------|-------------|-------------|-------------------|
| `FunctionDecl` | 38 | declarations.h | `std::string name`, `std::vector<Parameter> parameters`, `DataType returnType`, `StmtPtr body`, `bool isExported`, `bool isMainFunction`, `bool is_async`, `ExprList decorators` |
| `ClassDecl` | 122 | declarations.h | `std::string name`, `std::vector<std::string> superclasses`, `StmtList members`, `bool isExported` |
| `FieldDecl` | 180 | declarations.h | `std::string name`, `DataType type`, `ExprPtr initializer`, `AccessModifier access`, `bool isStatic` |
| `MethodDecl` | 233 | declarations.h | `std::string name`, `std::vector<Parameter> parameters`, `DataType returnType`, `StmtPtr body`, `AccessModifier access`, `bool isStatic`, `bool isVirtual`, `bool isOverride` |
| `ConstructorDecl` | 287 | declarations.h | `std::vector<Parameter> parameters`, `StmtPtr body`, `ExprList superArgs` |
| `EnumDecl` | 361 | declarations.h | `std::string name`, `std::vector<EnumMember> members`, `bool isExported` |

**الأعضاء الرئيسية:**
```cpp
// FunctionDecl - declarations.h:38
std::string name;                       // line 40
std::vector<Parameter> parameters;      // line 41
DataType returnType;                    // line 42
StmtPtr body;                           // line 43
bool isExported;                        // line 44
bool isMainFunction;                    // line 45
bool is_async;                          // line 46
ExprList decorators;                    // line 47
void accept(ASTVisitor&) override;      // line 84 (inline)
std::string toString() const override;  // line 88 (NOT inline)
```

---

## <a id="current-sir"></a>🔍 3. توثيق SIR الحالي / Current SIR Documentation

### 📁 الملفات / Files:
- **compiler/frontend/include/sir_builder.h** (743 سطر)
- **compiler/frontend/include/sir_module.h** (472 سطر)
- **compiler/frontend/include/sir_instruction.h** (تحتاج قراءة)
- **compiler/frontend/include/sir_types.h** (تحتاج قراءة)

---

### 📦 3.1. SIRBuilder - البناء الرئيسي

#### من sir_builder.h:

**الأسماء المستعارة الخاطئة (السطور 53-70):**
```cpp
namespace AST {
    using ProgramNode = Sad::AST::StmtList;          // line 55 ✅ صحيح
    using StatementNode = Sad::AST::Statement;       // line 56 ✅ صحيح
    using ExpressionNode = Sad::AST::Expression;     // line 57 ✅ صحيح
    using FunctionDeclNode = Sad::AST::FunctionDecl; // line 58 ✅ صحيح
    using VariableDeclNode = Sad::AST::VarDeclStmt;  // line 59 ✅ صحيح
    using ClassDeclNode = Sad::AST::ClassDecl;       // line 60 ✅ صحيح
    using IfStatementNode = Sad::AST::IfStmt;        // line 61 ✅ صحيح
    using WhileLoopNode = Sad::AST::WhileStmt;       // line 62 ✅ صحيح
    using ForLoopNode = Sad::AST::ForStmt;           // line 63 ✅ صحيح
    using ReturnStatementNode = Sad::AST::ReturnStmt;// line 64 ✅ صحيح
    using AssignmentNode = Sad::AST::AssignExpr;     // line 65 ✅ صحيح
    using BinaryOpNode = Sad::AST::BinaryExpr;       // line 66 ✅ صحيح
    using UnaryOpNode = Sad::AST::UnaryExpr;         // line 67 ✅ صحيح
    using FunctionCallNode = Sad::AST::CallExpr;     // line 68 ✅ صحيح
    using MethodCallNode = Sad::AST::MemberExpr;     // line 69 ✅ صحيح
    using VariableNode = Sad::AST::VariableExpr;     // line 70 ✅ صحيح
    using LiteralNode = Sad::AST::LiteralExpr;       // line 71 ✅ صحيح
    
    // Operator types
    using BinaryOperator = Sad::Lexer::TokenType;    // line 74 ✅ صحيح
    using UnaryOperator = Sad::Lexer::TokenType;     // line 75 ✅ صحيح
    
    // Type alias
    using Type = Sad::Data::Type;                    // line 78 ✅ صحيح
}
```

**الخلاصة:** الأسماء المستعارة **صحيحة** ولكن تحتاج إلى التحقق من استخدامها في .cpp

---

**الدوال الرئيسية (السطور 280-524):**

| الدالة / Function | السطر / Line | نوع الإرجاع / Return Type | المعاملات / Parameters |
|------------------|-------------|--------------------------|------------------------|
| `buildModule` | 314 | `std::shared_ptr<SIRModule>` | `AST::ProgramNode* program` |
| `buildFunction` | 330 | `void` | `AST::FunctionDeclNode* funcDecl` |
| `buildGlobalVariable` | 339 | `void` | `AST::VariableDeclNode* varDecl` |
| `buildClass` | 348 | `void` | `AST::ClassDeclNode* classDecl` |
| `buildStatement` | 363 | `void` | `AST::StatementNode* stmt` |
| `buildIfStatement` | 378 | `void` | `AST::IfStatementNode* ifStmt` |
| `buildWhileLoop` | 388 | `void` | `AST::WhileLoopNode* whileLoop` |
| `buildForLoop` | 397 | `void` | `AST::ForLoopNode* forLoop` |
| `buildReturnStatement` | 406 | `void` | `AST::ReturnStatementNode* retStmt` |
| `buildAssignment` | 415 | `void` | `AST::AssignmentNode* assignment` |
| `buildLocalVariable` | 424 | `void` | `AST::VariableDeclNode* varDecl` |
| `buildExpression` | 442 | `BuildResult` | `AST::ExpressionNode* expr` |
| `buildBinaryOp` | 455 | `BuildResult` | `AST::BinaryOpNode* binOp` |
| `buildUnaryOp` | 466 | `BuildResult` | `AST::UnaryOpNode* unOp` |
| `buildFunctionCall` | 477 | `BuildResult` | `AST::FunctionCallNode* call` |
| `buildVariableAccess` | 486 | `BuildResult` | `AST::VariableNode* var` |
| `buildLiteral` | 495 | `BuildResult` | `AST::LiteralNode* literal` |

**المتغيرات الخاصة (السطور 670-695):**

| المتغير / Variable | السطر / Line | النوع / Type | الوصف / Description |
|-------------------|-------------|-------------|---------------------|
| `module_` | 675 | `std::shared_ptr<SIRModule>` | الوحدة الحالية |
| `currentFunction_` | 676 | `std::shared_ptr<SIRFunction>` | الدالة الحالية |
| `currentBlock_` | 677 | `std::shared_ptr<SIRBasicBlock>` | الكتلة الحالية |
| `nextTempRegister_` | 679 | `int` | رقم السجل المؤقت التالي |
| `nextLabel_` | 680 | `int` | رقم التسمية التالية |
| `currentScopeLevel_` | 681 | `int` | مستوى النطاق الحالي |
| `scopeStack_` | 685 | `std::vector<std::unordered_map<std::string, VariableInfo>>` | مكدس النطاقات |
| `functionTable_` | 688 | `std::unordered_map<std::string, FunctionInfo>` | جدول الدوال |
| `classTable_` | 691 | `std::unordered_map<std::string, std::shared_ptr<SIRClass>>` | جدول الأصناف |
| `loopStack_` | 694 | `std::vector<LoopContext>` | مكدس سياق الحلقات |
| `genericScopeStack_` | 697 | `std::vector<GenericScope>` | مكدس نطاقات الأنواع العامة |
| `errors_` | 700 | `std::vector<std::string>` | قائمة الأخطاء |

---

### 📦 3.2. SIRModule - الوحدة

#### من sir_module.h:

**الدوال الرئيسية (السطور 70-198):**

| الدالة / Function | السطر / Line | نوع الإرجاع / Return Type | المعاملات / Parameters |
|------------------|-------------|--------------------------|------------------------|
| `SIRModule` (constructor) | 75 | - | `const std::string& moduleName` |
| `addFunction` | 84 | `void` | `std::shared_ptr<SIRFunction> function` |
| `getFunction` | 94 | `std::shared_ptr<SIRFunction>` | `const std::string& name` const |
| `getFunctions` | 103 | `const std::vector<std::shared_ptr<SIRFunction>>&` | - const |
| `addGlobalVariable` | 112 | `void` | `std::shared_ptr<SIRGlobalVariable> globalVar` |
| `getGlobalVariable` | 121 | `std::shared_ptr<SIRGlobalVariable>` | `const std::string& name` const |
| `addClass` | 130 | `void` | `std::shared_ptr<SIRClass> sirClass` |
| `getClass` | 139 | `std::shared_ptr<SIRClass>` | `const std::string& name` const |
| `addStringConstant` | 149 | `std::string` | `const std::string& str` |
| `validate` | 158 | `bool` | - const |
| `toString` | 167 | `std::string` | - const |
| `print` | 173 | `void` | - const |
| `getFunctionCount` | 180 | `size_t` | - const (inline) |
| `getGlobalCount` | 186 | `size_t` | - const (inline) |
| `getTotalInstructions` | 192 | `size_t` | - const |
| `getGlobalVariables` | 198 | `const std::vector<std::shared_ptr<SIRGlobalVariable>>&` | - const (inline) |

**ملاحظات مهمة:**
- ✅ `getFunctionCount()` هو **inline** في السطر 180
- ✅ `getGlobalCount()` هو **inline** في السطر 186  
- ✅ `getGlobalVariables()` هو **inline** في السطر 198
- ⚠️ `print()` في السطر 173 **ليس inline** - يحتاج تنفيذ في .cpp
- ⚠️ `getTotalInstructions()` في السطر 192 **ليس inline** - يحتاج تنفيذ في .cpp

---

## <a id="compatibility-matrix"></a>🔄 4. مصفوفة التوافق / Compatibility Matrix

### 4.1. توافق الأسماء المستعارة / Type Alias Compatibility

| الاسم المستعار / Alias | النوع المستهدف / Target Type | الحالة / Status |
|------------------------|------------------------------|-----------------|
| `AST::ProgramNode` | `Sad::AST::StmtList` | ✅ صحيح |
| `AST::StatementNode` | `Sad::AST::Statement` | ✅ صحيح |
| `AST::ExpressionNode` | `Sad::AST::Expression` | ✅ صحيح |
| `AST::FunctionDeclNode` | `Sad::AST::FunctionDecl` | ✅ صحيح |
| `AST::VariableDeclNode` | `Sad::AST::VarDeclStmt` | ✅ صحيح |
| `AST::ClassDeclNode` | `Sad::AST::ClassDecl` | ✅ صحيح |
| `AST::IfStatementNode` | `Sad::AST::IfStmt` | ✅ صحيح |
| `AST::WhileLoopNode` | `Sad::AST::WhileStmt` | ✅ صحيح |
| `AST::ForLoopNode` | `Sad::AST::ForStmt` | ✅ صحيح |
| `AST::ReturnStatementNode` | `Sad::AST::ReturnStmt` | ✅ صحيح |
| `AST::AssignmentNode` | `Sad::AST::AssignExpr` | ✅ صحيح |
| `AST::BinaryOpNode` | `Sad::AST::BinaryExpr` | ✅ صحيح |
| `AST::UnaryOpNode` | `Sad::AST::UnaryExpr` | ✅ صحيح |
| `AST::FunctionCallNode` | `Sad::AST::CallExpr` | ✅ صحيح |
| `AST::MethodCallNode` | `Sad::AST::MemberExpr` | ✅ صحيح |
| `AST::VariableNode` | `Sad::AST::VariableExpr` | ✅ صحيح |
| `AST::LiteralNode` | `Sad::AST::LiteralExpr` | ✅ صحيح |

**الخلاصة:** جميع الأسماء المستعارة في sir_builder.h **صحيحة** ✅

---

### 4.2. توافق توقيعات الدوال / Function Signature Compatibility

#### ✅ buildModule - sir_builder.h:314
```cpp
std::shared_ptr<SIRModule> buildModule(AST::ProgramNode* program);
```
- **AST::ProgramNode** = `Sad::AST::StmtList` = `std::vector<std::unique_ptr<Statement>>`
- **المعامل المتوقع**: مؤشر إلى `StmtList`
- **الاستخدام الصحيح**: `buildModule(&ast)` حيث `ast` هو `StmtList`

#### ✅ buildFunction - sir_builder.h:330
```cpp
void buildFunction(AST::FunctionDeclNode* funcDecl);
```
- **AST::FunctionDeclNode** = `Sad::AST::FunctionDecl`
- **الاستخدام**: عند المرور على `StmtList` واكتشاف `FunctionDecl`

#### ✅ buildGlobalVariable - sir_builder.h:339
```cpp
void buildGlobalVariable(AST::VariableDeclNode* varDecl);
```
- **AST::VariableDeclNode** = `Sad::AST::VarDeclStmt`
- **الاستخدام**: عند المرور على `StmtList` واكتشاف `VarDeclStmt`

---

## <a id="rebuild-plan"></a>📝 5. خطة إعادة الكتابة / Rebuild Plan

### المرحلة 1: قراءة ملفات SIR المتبقية ✅ (بدون كتابة)

1. ✅ قراءة sir_instruction.h بالكامل
2. ✅ قراءة sir_types.h بالكامل
3. ✅ قراءة sir_function.h بالكامل (إن وجد)
4. ✅ قراءة sir_basicblock.h بالكامل (إن وجد)
5. ✅ توثيق جميع الأنواع والدوال

### المرحلة 2: قراءة ملف sir_builder.cpp الحالي ✅ (بدون كتابة)

1. ✅ قراءة sir_builder.cpp بالكامل
2. ✅ توثيق جميع التنفيذات الموجودة
3. ✅ تحديد الدوال المنفذة vs غير المنفذة
4. ✅ تحديد الأخطاء في التنفيذات الموجودة

### المرحلة 3: تصحيح sir_builder.cpp ⚠️ (كتابة بحذر شديد)

#### 3.1. تصحيح buildModule
- ✅ قراءة التنفيذ الحالي
- ✅ التحقق من المعامل (هل `program*` أم `&program`)
- ✅ التحقق من cast للأنواع المختلفة
- ✅ تصحيح الأخطاء إن وجدت

#### 3.2. تصحيح buildFunction
- ✅ قراءة التنفيذ الحالي
- ✅ التحقق من الوصول إلى أعضاء FunctionDecl
- ✅ التحقق من استخدام parameters
- ✅ تصحيح الأخطاء إن وجدت

#### 3.3. تصحيح باقي الدوال
- ✅ تطبيق نفس العملية لكل دالة

### المرحلة 4: تصحيح sir_module.cpp ⚠️ (كتابة بحذر شديد)

#### 4.1. التحقق من تنفيذ print()
```cpp
// ============================================================================
// print - sir_module.h:173
// void print() const;
// NOT inline - needs implementation in .cpp
// ============================================================================
void SIRModule::print() const {
    std::cout << toString() << std::endl;
}
```

#### 4.2. التحقق من تنفيذ getTotalInstructions()
```cpp
// ============================================================================
// getTotalInstructions - sir_module.h:192
// size_t getTotalInstructions() const;
// NOT inline - needs implementation in .cpp
// Needs: SIRFunction::getBasicBlocks(), SIRBasicBlock::getInstructions()
// ============================================================================
// ⚠️ MUST READ SIRFunction and SIRBasicBlock headers FIRST!
```

### المرحلة 5: اختبار البناء 🔨

```powershell
cmake --build build --config Debug --target llvm_pipeline
```

**المتوقع:** 0 أخطاء ✅

---

## <a id="checklist"></a>✅ 6. قائمة التحقق / Checklist

### قبل كتابة أي كود:

- [ ] هل قرأت الـ header بالكامل؟
- [ ] هل وثقت الدالة بالملف والسطر؟
- [ ] هل وثقت كل متغير مستخدم؟
- [ ] هل تحققت من النوع بدقة (unique_ptr vs shared_ptr)?
- [ ] هل الدالة inline؟ (إذا نعم، لا تنفذها في .cpp)
- [ ] هل قرأت التبعيات (dependencies)?

### أثناء الكتابة:

- [ ] هل التوقيع مطابق تماماً للـ header؟
- [ ] هل جميع المتغيرات موجودة؟
- [ ] هل const correctness صحيحة؟
- [ ] هل استخدمت .get() للـ unique_ptr عند اللزوم؟

### بعد الكتابة:

- [ ] هل اختبرت البناء؟
- [ ] هل الأخطاء = 0؟
- [ ] هل التحذيرات ≤ 5؟

---

## 🎯 الخلاصة / Summary

### الوضع الحالي:
1. ✅ AST محدد بالكامل وصحيح
2. ✅ الأسماء المستعارة في sir_builder.h صحيحة
3. ❌ **خطأ كبير في sir_builder.cpp::buildModule**
4. ⚠️ التنفيذات في sir_module.cpp تحتاج تحقق/إضافة

### الأخطاء المكتشفة:

#### ❌ خطأ 1: sir_builder.cpp:59
```cpp
// ❌ خطأ فادح - line 59
for (auto& decl : program->declarations) {
    //              ^^^^^^^^^^^^^^^^^ 
    // StmtList لا يحتوي على عضو اسمه declarations!
}
```

**التشخيص:**
- `AST::ProgramNode` = `Sad::AST::StmtList` = `std::vector<StmtPtr>`
- `std::vector` لا يحتوي على عضو `declarations`
- الكود يحاول الوصول لعضو غير موجود

**التصحيح الصحيح:**
```cpp
// ✅ الطريقة الصحيحة - التكرار على العناصر مباشرة
// program هو مؤشر لـ StmtList (std::vector<StmtPtr>)
for (auto& stmt : *program) {  // ✅ dereference ثم iterate
    if (auto funcDecl = dynamic_cast<AST::FunctionDeclNode*>(stmt.get())) {
        buildFunction(funcDecl);
    }
    else if (auto varDecl = dynamic_cast<AST::VariableDeclNode*>(stmt.get())) {
        buildGlobalVariable(varDecl);
    }
    else if (auto classDecl = dynamic_cast<AST::ClassDeclNode*>(stmt.get())) {
        buildClass(classDecl);
    }
}
```

### الخطوات التالية:
1. ✅ قراءة ملفات SIR المتبقية
2. ✅ قراءة ملفات .cpp
3. ⚠️ تصحيح buildModule (تم تحديد الخطأ)
4. ⚠️ اختبار بعد كل تعديل

### الالتزام:
**أتعهد بعدم كتابة أي سطر كود قبل قراءة وتوثيق كل شيء متعلق به.**

---

**نهاية التوثيق**  
**End of Documentation**
