// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include "sir_builder.h"
#include <stdexcept>
#include <iostream>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// المنشئ / Constructor
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:287
// التوقيع / Signature: SIRBuilder();
//
// المتغيرات المُهيّئة / Initialized variables:
// - module_: sir_builder.h:582 (std::shared_ptr<SIRModule>)
// - currentFunction_: sir_builder.h:583 (std::shared_ptr<SIRFunction>)
// - currentBlock_: sir_builder.h:584 (SIRBasicBlock*)
// - currentScopeLevel_: sir_builder.h:599 (int)
// - registerCounter_: sir_builder.h:600 (int)
// - labelCounter_: sir_builder.h:601 (int)
// ============================================================================
SIRBuilder::SIRBuilder() 
    : module_(nullptr)
    , currentFunction_(nullptr)
    , currentBlock_(nullptr)
    , nextTempRegister_(0)
    , nextLabel_(0)
    , currentScopeLevel_(0)
{
    // (AR) لا توجد تهيئة إضافية مطلوبة
    // (EN) No additional initialization required
}

// ============================================================================
// buildModule - بناء وحدة SIR كاملة من AST
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:329
// التوقيع / Signature: 
//   std::shared_ptr<SIRModule> buildModule(AST::ProgramNode* program);
//
// المعاملات / Parameters:
// - program: AST::ProgramNode* = Sad::AST::StmtList* (sir_builder.h:51)
//            وهو alias لـ std::vector<StmtPtr> (ast_node.h:170)
//
// الإرجاع / Returns:
// - std::shared_ptr<SIRModule>: وحدة SIR كاملة أو nullptr عند الخطأ
//
// الدوال المستدعاة / Called functions:
// - std::make_shared<SIRModule>: standard library
// - buildFunction: sir_builder.h:348
// - buildGlobalVariable: sir_builder.h:356
// - buildClass: sir_builder.h:364
//
// AST Node Types Used:
// - Sad::AST::FunctionDecl: declarations.h:19
// - Sad::AST::VarDeclStmt: statements.h:74
// - Sad::AST::ClassDecl: declarations.h:122
// ============================================================================
std::shared_ptr<SIRModule> SIRBuilder::buildModule(AST::ProgramNode* program) {
    if (!program) {
        errors_.push_back("Error: Null program AST");
        return nullptr;
    }
    
    // (AR) إنشاء وحدة SIR جديدة (sir_module.h:501 - SIRModule constructor)
    // (EN) Create new SIR module
    module_ = std::make_shared<SIRModule>("main");
    
    // (AR) معالجة جميع التصريحات في البرنامج
    // (EN) Process all declarations in program
    // program هو StmtList = std::vector<StmtPtr> (ast_node.h:170)
    for (const auto& stmt : *program) {
        if (!stmt) continue;
        
        // (AR) التحقق من نوع التصريح باستخدام dynamic_cast
        // (EN) Check declaration type using dynamic_cast
        
        // (AR) دالة؟ (declarations.h:19 - class FunctionDecl)
        // (EN) Function declaration?
        if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl*>(stmt.get())) {
            buildFunction(funcDecl);
            continue;
        }
        
        // (AR) متغير عام؟ (statements.h:74 - class VarDeclStmt)
        // (EN) Global variable?
        if (auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt*>(stmt.get())) {
            buildGlobalVariable(varDecl);
            continue;
        }
        
        // (AR) صنف؟ (declarations.h:122 - class ClassDecl)
        // (EN) Class declaration?
        if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl*>(stmt.get())) {
            buildClass(classDecl);
            continue;
        }
        
        // (AR) تصريح غير معروف - نتجاهله حالياً
        // (EN) Unknown declaration - ignore for now
    }
    
    return module_;
}

// ============================================================================
// buildFunction - بناء دالة كاملة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:348
// التوقيع / Signature: void buildFunction(AST::FunctionDeclNode* funcDecl);
//
// المعاملات / Parameters:
// - funcDecl: AST::FunctionDeclNode* = Sad::AST::FunctionDecl* (sir_builder.h:58)
//
// FunctionDecl Members (declarations.h:19-64):
// - name: std::string (line 43)
// - parameters: std::vector<Parameter> (line 44)
// - returnType: Data::DataType (line 45)
// - body: StmtPtr (line 46)
// - isBuiltin: bool (line 47)
//
// Parameter Structure (declarations.h:24-41):
// - name: std::string
// - type: Data::DataType
// - defaultValue: ExprPtr (optional)
//
// الدوال المستدعاة / Called functions:
// - std::make_shared<SIRFunction>: standard library
// - SIRFunction::addParameter: sir_module.h:289
// - astTypeToSIRType: sir_builder.h:713
// - createBasicBlock: sir_builder.h:501
// - SIRFunction::addBasicBlock: sir_module.h:321
// - buildStatement: sir_builder.h:372
// - module_->addFunction: sir_module.h:569
// ============================================================================
void SIRBuilder::buildFunction(AST::FunctionDeclNode* funcDecl) {
    if (!funcDecl) {
        return;
    }
    
    // (AR) تحويل نوع الإرجاع من DataType إلى SIRType
    // (EN) Convert return type from DataType to SIRType
    // astTypeToSIRType: sir_builder.h:713
    SIRType returnType = astTypeToSIRType(funcDecl->returnType);
    
    // (AR) إنشاء دالة SIR جديدة (sir_module.h:235 - SIRFunction constructor)
    // (EN) Create new SIR function
    auto sirFunction = std::make_shared<SIRFunction>(funcDecl->name, returnType);
    
    // (AR) إضافة المعاملات (declarations.h:44 - parameters: vector<Parameter>)
    // (EN) Add parameters
    for (const auto& param : funcDecl->parameters) {
        SIRType paramType = astTypeToSIRType(param.type);
        // SIRParameter constructor: sir_module.h:50
        SIRParameter sirParam(param.name, paramType);
        // addParameter: sir_module.h:289
        sirFunction->addParameter(sirParam);
    }
    
    // (AR) إذا لم يكن هناك جسم للدالة (builtin or declaration only), لا نبني الجسم
    // (EN) If no function body (builtin or declaration only), don't build body
    if (!funcDecl->body) {
        module_->addFunction(sirFunction);
        return;
    }
    
    // (AR) تعيين الدالة الحالية
    // (EN) Set current function
    currentFunction_ = sirFunction;
    
    // (AR) إنشاء basic block للدخول (createBasicBlock: sir_builder.h:501)
    // (EN) Create entry basic block
    auto entryBlock = createBasicBlock("entry");
    
    // (AR) إضافة block للدالة (sir_module.h:321 - addBasicBlock)
    // (EN) Add block to function
    sirFunction->addBasicBlock(entryBlock);
    currentBlock_ = entryBlock;
    
    // (AR) بناء جسم الدالة (declarations.h:46 - body: StmtPtr)
    // (EN) Build function body
    if (funcDecl->body) {
        // buildStatement: sir_builder.h:372
        buildStatement(funcDecl->body.get());
    }
    
    // (AR) التأكد من وجود terminator في نهاية الدالة
    // (EN) Ensure function has a terminator at the end
    // إذا كانت الدالة void ولا يوجد return صريح، نضيف RET_VOID
    // If function is void and has no explicit return, add RET_VOID
    if (currentBlock_ && !currentBlock_->instructions.empty()) {
        const auto& lastInst = currentBlock_->instructions.back();
        if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID) {
            // (AR) لا يوجد return - نضيف واحداً
            // (EN) No return - add one
            if (returnType == SIRType::VOID) {
                SIRInstruction retInst;
                retInst.opcode = SIROpcode::RET_VOID;
                currentBlock_->addInstruction(retInst);
            } else {
                // (AR) للدوال غير void، نضيف return بقيمة افتراضية (0)
                // (EN) For non-void functions, add return with default value (0)
                SIRInstruction retInst;
                retInst.opcode = SIROpcode::RET;
                retInst.operands.push_back(SIROperand::ConstantI64(0));
                currentBlock_->addInstruction(retInst);
            }
        }
    } else if (currentBlock_ && currentBlock_->instructions.empty()) {
        // (AR) الدالة فارغة - نضيف return
        // (EN) Empty function - add return
        if (returnType == SIRType::VOID) {
            SIRInstruction retInst;
            retInst.opcode = SIROpcode::RET_VOID;
            currentBlock_->addInstruction(retInst);
        } else {
            SIRInstruction retInst;
            retInst.opcode = SIROpcode::RET;
            retInst.operands.push_back(SIROperand::ConstantI64(0));
            currentBlock_->addInstruction(retInst);
        }
    }
    
    // (AR) إضافة الدالة للوحدة (sir_module.h:569 - addFunction)
    // (EN) Add function to module
    module_->addFunction(sirFunction);
    
    // (AR) إعادة تعيين الدالة الحالية
    // (EN) Reset current function
    currentFunction_ = nullptr;
    currentBlock_ = nullptr;
}

// ============================================================================
// buildGlobalVariable - بناء متغير عام
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:356
// التوقيع / Signature: void buildGlobalVariable(AST::VariableDeclNode* varDecl);
//
// المعاملات / Parameters:
// - varDecl: AST::VariableDeclNode* = Sad::AST::VarDeclStmt* (sir_builder.h:59)
//
// VarDeclStmt Members (statements.h:74-100):
// - name: std::string (line 76)
// - type: Data::DataType (line 77)
// - initializer: ExprPtr (line 78)
// - isConst: bool (line 79)
//
// الدوال المستدعاة / Called functions:
// - astTypeToSIRType: sir_builder.h:713
// - module_->addGlobalVariable: sir_module.h:591
// ============================================================================
void SIRBuilder::buildGlobalVariable(AST::VariableDeclNode* varDecl) {
    if (!varDecl) {
        return;
    }
    
    // (AR) تحويل النوع (astTypeToSIRType: sir_builder.h:713)
    // (EN) Convert type
    SIRType varType = astTypeToSIRType(varDecl->type);
    
    // (AR) إنشاء متغير عام (SIRGlobalVariable constructor: sir_module.h:96)
    // (EN) Create global variable
    auto sirGlobal = std::make_shared<SIRGlobalVariable>(varDecl->name, varType);
    
    // (AR) TODO: معالجة القيمة الأولية (initializer)
    // (EN) TODO: Handle initializer
    // هذا يتطلب buildExpression والذي سنكتبه في المرحلة التالية
    
    // (AR) إضافة المتغير العام للوحدة (sir_module.h:591 - addGlobalVariable)
    // (EN) Add global variable to module
    module_->addGlobalVariable(sirGlobal);
}

// ============================================================================
// buildClass - بناء صنف
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:364
// التوقيع / Signature: void buildClass(AST::ClassDeclNode* classDecl);
//
// المعاملات / Parameters:
// - classDecl: AST::ClassDeclNode* = Sad::AST::ClassDecl* (sir_builder.h:60)
//
// ClassDecl Members (declarations.h:122-156):
// - name: std::string (line 125)
// - superclasses: std::vector<std::string> (line 126)
// - members: StmtList (line 127)
// - typeParameters: std::vector<TypeParameter> (line 128)
//
// الدوال المستدعاة / Called functions:
// - std::make_shared<SIRClass>: sir_module.h:409
// - module_->addClass: sir_module.h:608
// ============================================================================
void SIRBuilder::buildClass(AST::ClassDeclNode* classDecl) {
    if (!classDecl) {
        return;
    }
    
    // (AR) إنشاء صنف SIR (SIRClass constructor: sir_module.h:409)
    // (EN) Create SIR class
    auto sirClass = std::make_shared<SIRClass>(classDecl->name);
    
    // (AR) TODO: معالجة الأصناف الأساسية (superclasses)
    // (EN) TODO: Handle superclasses
    // declarations.h:126 - superclasses: vector<string>
    
    // (AR) TODO: معالجة الأعضاء (members)
    // (EN) TODO: Handle members
    // declarations.h:127 - members: StmtList
    // يحتاج dynamic_cast لـ FieldDecl و MethodDecl
    
    // (AR) إضافة الصنف للوحدة (sir_module.h:608 - addClass)
    // (EN) Add class to module
    module_->addClass(sirClass);
}

// ============================================================================
// buildStatement - بناء جملة (موزع للأنواع المختلفة)
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:372
// التوقيع / Signature: void buildStatement(AST::Statement* stmt);
//
// المعاملات / Parameters:
// - stmt: AST::Statement* = Sad::AST::Statement* (sir_builder.h:75)
//
// Statement Base Class (ast_node.h:88):
// - جميع الجمل ترث من هذا الصنف
// - All statements inherit from this class
//
// Statement Types (statements.h):
// - IfStmt (line 104): condition, thenBranch, elseBranch
// - WhileStmt (line 143): condition, body
// - ForStmt (line 187): initializer, condition, increment, body
// - ReturnStmt (line 266): value
// - BreakStmt (line 360): no members
// - ContinueStmt (line 392): no members
// - ExprStmt (line 38): expression
// - VarDeclStmt (line 74): name, type, initializer, isConst
//
// الدوال المستدعاة / Called functions:
// - buildIfStatement: sir_builder.h:385
// - buildWhileLoop: sir_builder.h:393
// - buildForLoop: sir_builder.h:401
// - buildReturnStatement: sir_builder.h:409
// - buildBreakStatement: sir_builder.h:417
// - buildContinueStatement: sir_builder.h:425
// ============================================================================
void SIRBuilder::buildStatement(AST::Statement* stmt) {
    if (!stmt) {
        return;
    }
    
    // (AR) IfStmt - جملة if (statements.h:104)
    // (EN) If statement
    if (auto ifStmt = dynamic_cast<Sad::AST::IfStmt*>(stmt)) {
        buildIfStatement(ifStmt);
        return;
    }
    
    // (AR) WhileStmt - حلقة while (statements.h:143)
    // (EN) While loop
    if (auto whileStmt = dynamic_cast<Sad::AST::WhileStmt*>(stmt)) {
        buildWhileLoop(whileStmt);
        return;
    }
    
    // (AR) ForStmt - حلقة for (statements.h:187)
    // (EN) For loop
    if (auto forStmt = dynamic_cast<Sad::AST::ForStmt*>(stmt)) {
        buildForLoop(forStmt);
        return;
    }
    
    // (AR) ReturnStmt - جملة return (statements.h:266)
    // (EN) Return statement
    if (auto returnStmt = dynamic_cast<Sad::AST::ReturnStmt*>(stmt)) {
        buildReturnStatement(returnStmt);
        return;
    }
    
    // (AR) BreakStmt - جملة break (statements.h:360)
    // (EN) Break statement
    if (auto breakStmt = dynamic_cast<Sad::AST::BreakStmt*>(stmt)) {
        buildBreakStatement(breakStmt);
        return;
    }
    
    // (AR) ContinueStmt - جملة continue (statements.h:392)
    // (EN) Continue statement
    if (auto continueStmt = dynamic_cast<Sad::AST::ContinueStmt*>(stmt)) {
        buildContinueStatement(continueStmt);
        return;
    }
    
    // (AR) ExprStmt - جملة تعبير (statements.h:38)
    // (EN) Expression statement
    if (auto exprStmt = dynamic_cast<Sad::AST::ExprStmt*>(stmt)) {
        // (AR) تنفيذ التعبير فقط (نتجاهل النتيجة)
        // (EN) Execute expression only (ignore result)
        if (exprStmt->expression) {
            buildExpression(exprStmt->expression.get());
        }
        return;
    }
    
    // (AR) VarDeclStmt - تصريح متغير (statements.h:74)
    // (EN) Variable declaration
    if (auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt*>(stmt)) {
        buildLocalVariable(varDecl);
        return;
    }
    
    // (AR) جملة غير معروفة - نتجاهلها
    // (EN) Unknown statement - ignore
}

// ============================================================================
// buildReturnStatement - بناء جملة return
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:409
// التوقيع / Signature: void buildReturnStatement(AST::ReturnStmt* retStmt);
//
// المعاملات / Parameters:
// - retStmt: AST::ReturnStmt* = Sad::AST::ReturnStmt* (sir_builder.h:409)
//
// ReturnStmt Members (statements.h:266):
// - value: ExprPtr (line 268) - optional, can be nullptr
//
// الدوال المستدعاة / Called functions:
// - buildExpression: sir_builder.h:432
// ============================================================================
void SIRBuilder::buildReturnStatement(AST::ReturnStmt* retStmt) {
    if (!retStmt) {
        return;
    }
    
    // (AR) ReturnStmt::value: ExprPtr (statements.h:268)
    // (EN) Build return instruction
    if (retStmt->value) {
        // (AR) بناء تعبير القيمة المُرجعة
        // (EN) Build return value expression
        BuildResult valueResult = buildExpression(retStmt->value.get());
        
        // (AR) توليد تعليمة RET مع القيمة
        // (EN) Generate RET instruction with value
        SIRInstruction retInst;
        retInst.opcode = SIROpcode::RET;
        // تحويل BuildResult إلى SIROperand
        SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
        retInst.operands.push_back(retOperand);
        if (currentBlock_) {
            currentBlock_->addInstruction(retInst);
        }
    } else {
        // (AR) توليد تعليمة RET_VOID
        // (EN) Generate RET_VOID instruction
        SIRInstruction retInst;
        retInst.opcode = SIROpcode::RET_VOID;
        if (currentBlock_) {
            currentBlock_->addInstruction(retInst);
        }
    }
}

// ============================================================================
// buildBreakStatement - بناء جملة break
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:417
// التوقيع / Signature: void buildBreakStatement(AST::BreakStmt* breakStmt);
//
// BreakStmt (statements.h:360):
// - لا توجد أعضاء إضافية
// - No additional members
// ============================================================================
void SIRBuilder::buildBreakStatement(AST::BreakStmt* breakStmt) {
    if (!breakStmt) {
        return;
    }
    
    // (AR) TODO: التحقق من وجود حلقة والقفز لنهايتها
    // (EN) TODO: Check for loop and jump to its end
    // استخدام getCurrentLoop() للحصول على سياق الحلقة
}

// ============================================================================
// buildContinueStatement - بناء جملة continue
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:425
// التوقيع / Signature: void buildContinueStatement(AST::ContinueStmt* continueStmt);
//
// ContinueStmt (statements.h:392):
// - لا توجد أعضاء إضافية
// - No additional members
// ============================================================================
void SIRBuilder::buildContinueStatement(AST::ContinueStmt* continueStmt) {
    if (!continueStmt) {
        return;
    }
    
    // (AR) TODO: التحقق من وجود حلقة والقفز لبدايتها
    // (EN) TODO: Check for loop and jump to its beginning
}

// ============================================================================
// buildAssignment - بناء إسناد متغير
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:429
// التوقيع / Signature: void buildAssignment(AST::AssignExpr* assignment);
//
// المعاملات / Parameters:
// - assignment: AST::AssignExpr* = Sad::AST::AssignExpr* (sir_builder.h:429)
//
// AssignExpr Members (expressions.h:247):
// - name: std::string (line 249)
// - value: ExprPtr (line 250)
//
// الدوال المستدعاة / Called functions:
// - buildExpression: sir_builder.h:432
// - lookupVariable: sir_builder.h:597
// ============================================================================
void SIRBuilder::buildAssignment(AST::AssignExpr* assignment) {
    if (!assignment) {
        return;
    }
    
    // (AR) البحث عن المتغير (AssignExpr::name: std::string, line 249)
    // (EN) Lookup variable
    VariableInfo* varInfo = lookupVariable(assignment->name);
    if (!varInfo) {
        // (AR) متغير غير معرف
        // (EN) Undefined variable
        errors_.push_back("Undefined variable: " + assignment->name);
        return;
    }
    
    // (AR) التحقق من إمكانية التعديل (VariableInfo::isMutable, sir_builder.h:145)
    // (EN) Check if mutable
    if (!varInfo->isMutable) {
        // (AR) متغير ثابت لا يمكن تعديله
        // (EN) Constant variable cannot be modified
        errors_.push_back("Cannot assign to const variable: " + assignment->name);
        return;
    }
    
    // (AR) بناء قيمة التعبير (AssignExpr::value: ExprPtr, line 250)
    // (EN) Build value expression
    auto valueResult = buildExpression(assignment->value.get());
    
    // (AR) TODO: توليد تعليمة STORE لإسناد القيمة
    // (EN) TODO: Generate STORE instruction to assign value
    // STORE valueResult.registerName -> varInfo->registerName
}

// ============================================================================
// buildLocalVariable - بناء تصريح متغير محلي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:433
// التوقيع / Signature: void buildLocalVariable(AST::VarDeclStmt* varDecl);
//
// المعاملات / Parameters:
// - varDecl: AST::VarDeclStmt* (statements.h:74)
//
// VarDeclStmt Members (statements.h:74-100):
// - name: std::string (line 76)
// - type: Data::DataType (line 77)
// - initializer: ExprPtr (line 78)
// - isConst: bool (line 79)
// ============================================================================
void SIRBuilder::buildLocalVariable(AST::VarDeclStmt* varDecl) {
    if (!varDecl) {
        return;
    }
    
    // (AR) تحويل النوع (VarDeclStmt::type: Data::DataType, line 77)
    // (EN) Convert type
    SIRType varType = astTypeToSIRType(varDecl->type);
    
    // (AR) إنشاء معلومات المتغير (sir_builder.h:139 - VariableInfo)
    // (EN) Create variable info
    VariableInfo varInfo;
    varInfo.name = varDecl->name;           // line 76
    varInfo.type = varType;
    varInfo.registerName = "%" + varDecl->name;
    varInfo.isGlobal = false;
    varInfo.isMutable = !varDecl->isConst;  // line 79
    varInfo.scopeLevel = currentScopeLevel_;
    
    // (AR) معالجة القيمة الأولية (VarDeclStmt::initializer: ExprPtr, line 78)
    // (EN) Handle initializer
    if (varDecl->initializer) {
        auto initResult = buildExpression(varDecl->initializer.get());
        // (AR) TODO: توليد تعليمة MOVE لإسناد القيمة الأولية
        // (EN) TODO: Generate MOVE instruction to assign initial value
    }
    
    // (AR) إضافة المتغير للنطاق (sir_builder.h:591 - addVariable)
    // (EN) Add variable to scope
    addVariable(varInfo);
}

// ============================================================================
// buildIfStatement - بناء جملة if
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:385
// التوقيع / Signature: void buildIfStatement(AST::IfStmt* ifStmt);
//
// المعاملات / Parameters:
// - ifStmt: AST::IfStmt* = Sad::AST::IfStmt* (sir_builder.h:385)
//
// IfStmt Members (statements.h:104-135):
// - condition: ExprPtr (line 107)
// - thenBranch: StmtPtr (line 108)
// - elseBranch: StmtPtr (line 109) - optional, can be nullptr
//
// المتغيرات المستخدمة / Used variables:
// - currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
//
// الدوال المستدعاة / Called functions:
// - buildExpression: sir_builder.h:432
// - buildStatement: sir_builder.h:372
// - createBasicBlock: sir_builder.h:501
// - newLabel: sir_builder.h:520
// ============================================================================
void SIRBuilder::buildIfStatement(AST::IfStmt* ifStmt) {
    if (!ifStmt) {
        return;
    }
    
    // (AR) إنشاء الكتل / Create blocks
    // (EN) Create basic blocks for if/then/else/merge
    std::string thenLabel = newLabel("then");
    std::string elseLabel = newLabel("else");
    std::string mergeLabel = newLabel("merge");
    
    auto thenBlock = createBasicBlock(thenLabel);
    auto elseBlock = createBasicBlock(elseLabel);
    auto mergeBlock = createBasicBlock(mergeLabel);
    
    // (AR) بناء الشرط (IfStmt::condition: ExprPtr, line 107)
    // (EN) Build condition expression
    auto condResult = buildExpression(ifStmt->condition.get());
    
    // (AR) TODO: توليد تعليمة BR الشرطي
    // (EN) TODO: Generate conditional BR instruction
    // BR condResult.registerName, thenBlock, elseBlock
    
    // (AR) بناء فرع then (IfStmt::thenBranch: StmtPtr, line 108)
    // (EN) Build then branch
    currentBlock_ = thenBlock;
    if (ifStmt->thenBranch) {
        buildStatement(ifStmt->thenBranch.get());
    }
    // (AR) TODO: توليد قفز غير شرطي لـ merge
    // (EN) TODO: Generate unconditional jump to merge
    
    // (AR) بناء فرع else إذا وُجد (IfStmt::elseBranch: StmtPtr, line 109)
    // (EN) Build else branch if exists
    currentBlock_ = elseBlock;
    if (ifStmt->elseBranch) {
        buildStatement(ifStmt->elseBranch.get());
    }
    // (AR) TODO: توليد قفز غير شرطي لـ merge
    // (EN) TODO: Generate unconditional jump to merge
    
    // (AR) الاستمرار بعد الـ if
    // (EN) Continue after if statement
    currentBlock_ = mergeBlock;
}

// ============================================================================
// buildWhileLoop - بناء حلقة while
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:393
// التوقيع / Signature: void buildWhileLoop(AST::WhileStmt* whileLoop);
//
// المعاملات / Parameters:
// - whileLoop: AST::WhileStmt* = Sad::AST::WhileStmt* (sir_builder.h:393)
//
// WhileStmt Members (statements.h:143-175):
// - condition: ExprPtr (line 149)
// - body: StmtPtr (line 150)
//
// المتغيرات المستخدمة / Used variables:
// - currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
//
// الدوال المستدعاة / Called functions:
// - buildExpression: sir_builder.h:432
// - buildStatement: sir_builder.h:372
// - createBasicBlock: sir_builder.h:501
// - newLabel: sir_builder.h:520
// ============================================================================
void SIRBuilder::buildWhileLoop(AST::WhileStmt* whileLoop) {
    if (!whileLoop) {
        return;
    }
    
    // (AR) إنشاء الكتل / Create blocks
    // (EN) Create basic blocks for condition/body/exit
    std::string condLabel = newLabel("while_cond");
    std::string bodyLabel = newLabel("while_body");
    std::string exitLabel = newLabel("while_exit");
    
    auto condBlock = createBasicBlock(condLabel);
    auto bodyBlock = createBasicBlock(bodyLabel);
    auto exitBlock = createBasicBlock(exitLabel);
    
    // (AR) TODO: توليد قفز لكتلة الشرط
    // (EN) TODO: Generate jump to condition block
    
    // (AR) بناء الشرط (WhileStmt::condition: ExprPtr, line 149)
    // (EN) Build condition expression
    currentBlock_ = condBlock;
    auto condResult = buildExpression(whileLoop->condition.get());
    
    // (AR) TODO: توليد BR شرطي لـ body أو exit
    // (EN) TODO: Generate conditional BR to body or exit
    
    // (AR) بناء جسم الحلقة (WhileStmt::body: StmtPtr, line 150)
    // (EN) Build loop body
    currentBlock_ = bodyBlock;
    if (whileLoop->body) {
        buildStatement(whileLoop->body.get());
    }
    
    // (AR) TODO: توليد قفز للشرط (العودة للبداية)
    // (EN) TODO: Generate jump back to condition
    
    // (AR) الاستمرار بعد الحلقة
    // (EN) Continue after loop
    currentBlock_ = exitBlock;
}

// ============================================================================
// buildForLoop - بناء حلقة for
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:401
// التوقيع / Signature: void buildForLoop(AST::ForStmt* forLoop);
//
// المعاملات / Parameters:
// - forLoop: AST::ForStmt* = Sad::AST::ForStmt* (sir_builder.h:401)
//
// ForStmt Members (statements.h:187-253):
// - initializer: StmtPtr (line 193) - optional
// - condition: ExprPtr (line 194) - optional
// - increment: ExprPtr (line 195) - optional
// - body: StmtPtr (line 196)
//
// المتغيرات المستخدمة / Used variables:
// - currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
// - currentScopeLevel_: sir_builder.h:599 (int)
//
// الدوال المستدعاة / Called functions:
// - buildStatement: sir_builder.h:372
// - buildExpression: sir_builder.h:432
// - createBasicBlock: sir_builder.h:501
// - newLabel: sir_builder.h:520
// - enterScope: sir_builder.h:587
// - exitScope: sir_builder.h:589
// ============================================================================
void SIRBuilder::buildForLoop(AST::ForStmt* forLoop) {
    if (!forLoop) {
        return;
    }
    
    // (AR) دخول نطاق جديد للحلقة
    // (EN) Enter new scope for loop
    enterScope();
    
    // (AR) تنفيذ initializer إذا وُجد (ForStmt::initializer: StmtPtr, line 193)
    // (EN) Execute initializer if exists
    if (forLoop->initializer) {
        buildStatement(forLoop->initializer.get());
    }
    
    // (AR) إنشاء الكتل / Create blocks
    // (EN) Create basic blocks
    std::string condLabel = newLabel("for_cond");
    std::string bodyLabel = newLabel("for_body");
    std::string incLabel = newLabel("for_inc");
    std::string exitLabel = newLabel("for_exit");
    
    auto condBlock = createBasicBlock(condLabel);
    auto bodyBlock = createBasicBlock(bodyLabel);
    auto incBlock = createBasicBlock(incLabel);
    auto exitBlock = createBasicBlock(exitLabel);
    
    // (AR) TODO: توليد قفز لكتلة الشرط
    // (EN) TODO: Generate jump to condition block
    
    // (AR) بناء الشرط (ForStmt::condition: ExprPtr, line 194)
    // (EN) Build condition - if no condition, always true
    currentBlock_ = condBlock;
    if (forLoop->condition) {
        auto condResult = buildExpression(forLoop->condition.get());
        // (AR) TODO: توليد BR شرطي لـ body أو exit
        // (EN) TODO: Generate conditional BR to body or exit
    } else {
        // (AR) TODO: قفز غير شرطي لـ body (لا يوجد شرط)
        // (EN) TODO: Unconditional jump to body (no condition)
    }
    
    // (AR) بناء جسم الحلقة (ForStmt::body: StmtPtr, line 196)
    // (EN) Build loop body
    currentBlock_ = bodyBlock;
    if (forLoop->body) {
        buildStatement(forLoop->body.get());
    }
    
    // (AR) TODO: توليد قفز لكتلة الزيادة
    // (EN) TODO: Generate jump to increment block
    
    // (AR) بناء الزيادة (ForStmt::increment: ExprPtr, line 195)
    // (EN) Build increment
    currentBlock_ = incBlock;
    if (forLoop->increment) {
        buildExpression(forLoop->increment.get());
    }
    
    // (AR) TODO: توليد قفز للشرط (العودة للبداية)
    // (EN) TODO: Generate jump back to condition
    
    // (AR) الاستمرار بعد الحلقة
    // (EN) Continue after loop
    currentBlock_ = exitBlock;
    
    // (AR) الخروج من نطاق الحلقة
    // (EN) Exit loop scope
    exitScope();
}

// ============================================================================
// HELPER FUNCTIONS - الدوال المساعدة
// ============================================================================

// ============================================================================
// createBasicBlock - إنشاء كتلة أساسية جديدة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:501
// التوقيع / Signature: std::shared_ptr<SIRBasicBlock> createBasicBlock(const std::string& name);
//
// الإرجاع / Returns:
// - std::shared_ptr<SIRBasicBlock>: sir_module.h:145 (SIRBasicBlock class)
//
// SIRBasicBlock Constructor (sir_module.h:145):
// - SIRBasicBlock(const std::string& name): line 168
// ============================================================================
std::shared_ptr<SIRBasicBlock> SIRBuilder::createBasicBlock(const std::string& name) {
    // (AR) إنشاء كتلة جديدة (sir_module.h:168 - SIRBasicBlock constructor)
    // (EN) Create new block
    return std::make_shared<SIRBasicBlock>(name);
}

// ============================================================================
// newTempRegister - إنشاء سجل مؤقت جديد
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:511
// التوقيع / Signature: std::string newTempRegister();
//
// المتغيرات المستخدمة / Used variables:
// - registerCounter_: sir_builder.h:600 (int)
//
// الإرجاع / Returns:
// - std::string: اسم السجل بصيغة %0, %1, %2, ...
// ============================================================================
std::string SIRBuilder::newTempRegister() {
    // (AR) إنشاء سجل بصيغة %N حيث N هو nextTempRegister_
    // (EN) Create register in format %N where N is nextTempRegister_
    return "%" + std::to_string(nextTempRegister_++);
}

// ============================================================================
// newLabel - إنشاء تسمية جديدة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:520
// التوقيع / Signature: std::string newLabel(const std::string& prefix);
//
// المعاملات / Parameters:
// - prefix: std::string = البادئة (L, if, loop, etc)
//
// المتغيرات المستخدمة / Used variables:
// - labelCounter_: sir_builder.h:601 (int)
//
// الإرجاع / Returns:
// - std::string: اسم التسمية بصيغة prefix_N
// ============================================================================
std::string SIRBuilder::newLabel(const std::string& prefix) {
    // (AR) إنشاء تسمية بصيغة prefix_N
    // (EN) Create label in format prefix_N
    return prefix + "_" + std::to_string(nextLabel_++);
}

// ============================================================================
// astTypeToSIRType - تحويل DataType إلى SIRType
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:741
// التوقيع / Signature: SIRType astTypeToSIRType(const Sad::Data::DataType& type);
//
// المعاملات / Parameters:
// - type: const Sad::Data::DataType& (parser/data.h)
//
// الإرجاع / Returns:
// - SIRType: sir_types.h:57 (enum class SIRType)
//
// SIRType Values (sir_types.h:57):
// - VOID, I64, F64, BOOL, PTR, ARRAY, STRING, STRUCT, FUNCTION
// ============================================================================
SIRType SIRBuilder::astTypeToSIRType(const Sad::Data::DataType& type) {
    // (AR) TODO: تنفيذ التحويل الكامل
    // (EN) TODO: Implement full conversion
    // حالياً نرجع I64 كقيمة افتراضية
    // Currently return I64 as default
    return SIRType::I64;
}

// ============================================================================
// enterScope - دخول نطاق جديد
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:587
// التوقيع / Signature: void enterScope();
//
// المتغيرات المستخدمة / Used variables:
// - currentScopeLevel_: sir_builder.h:599 (int)
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
// ============================================================================
void SIRBuilder::enterScope() {
    // (AR) زيادة مستوى النطاق (sir_builder.h:599 - currentScopeLevel_)
    // (EN) Increase scope level
    currentScopeLevel_++;
    
    // (AR) إضافة نطاق جديد (sir_builder.h:714 - scopeStack_)
    // (EN) Add new scope
    scopeStack_.push_back(std::unordered_map<std::string, VariableInfo>());
}

// ============================================================================
// exitScope - الخروج من النطاق الحالي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:589
// التوقيع / Signature: void exitScope();
//
// المتغيرات المستخدمة / Used variables:
// - currentScopeLevel_: sir_builder.h:599 (int)
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
// ============================================================================
void SIRBuilder::exitScope() {
    // (AR) إزالة النطاق الأخير (sir_builder.h:714 - scopeStack_)
    // (EN) Remove last scope
    if (!scopeStack_.empty()) {
        scopeStack_.pop_back();
    }
    
    // (AR) تقليل مستوى النطاق (sir_builder.h:599 - currentScopeLevel_)
    // (EN) Decrease scope level
    if (currentScopeLevel_ > 0) {
        currentScopeLevel_--;
    }
}

// ============================================================================
// addVariable - إضافة متغير للنطاق الحالي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:591
// التوقيع / Signature: void addVariable(const VariableInfo& varInfo);
//
// المعاملات / Parameters:
// - varInfo: const VariableInfo& = sir_builder.h:139 (VariableInfo struct)
//
// المتغيرات المستخدمة / Used variables:
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
// ============================================================================
void SIRBuilder::addVariable(const VariableInfo& varInfo) {
    // (AR) إضافة المتغير للنطاق الحالي (sir_builder.h:714 - scopeStack_)
    // (EN) Add variable to current scope
    if (!scopeStack_.empty()) {
        scopeStack_.back()[varInfo.name] = varInfo;
    }
}

// ============================================================================
// lookupVariable - البحث عن متغير في النطاقات
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:597
// التوقيع / Signature: VariableInfo* lookupVariable(const std::string& name);
//
// المعاملات / Parameters:
// - name: const std::string& (اسم المتغير)
//
// المتغيرات المستخدمة / Used variables:
// - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
//
// الإرجاع / Returns:
// - VariableInfo*: مؤشر للمتغير أو nullptr
// ============================================================================
VariableInfo* SIRBuilder::lookupVariable(const std::string& name) {
    // (AR) البحث في النطاقات من الأحدث للأقدم (sir_builder.h:714 - scopeStack_)
    // (EN) Search in scopes from newest to oldest
    for (auto scopeIt = scopeStack_.rbegin(); scopeIt != scopeStack_.rend(); ++scopeIt) {
        auto it = scopeIt->find(name);
        if (it != scopeIt->end()) {
            return &(it->second);
        }
    }
    
    // (AR) لم يُعثر على المتغير
    // (EN) Variable not found
    return nullptr;
}

// ============================================================================
// enterLoop - دخول سياق حلقة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:607
// التوقيع / Signature: void enterLoop(const LoopContext& ctx);
//
// المعاملات / Parameters:
// - ctx: const LoopContext& = sir_builder.h:180 (LoopContext struct)
//
// المتغيرات المستخدمة / Used variables:
// - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
// ============================================================================
void SIRBuilder::enterLoop(const LoopContext& ctx) {
    // (AR) إضافة سياق حلقة جديد (sir_builder.h:639 - loopStack_)
    // (EN) Add new loop context
    loopStack_.push_back(ctx);
}

// ============================================================================
// exitLoop - الخروج من سياق الحلقة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:615
// التوقيع / Signature: void exitLoop();
//
// المتغيرات المستخدمة / Used variables:
// - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
// ============================================================================
void SIRBuilder::exitLoop() {
    // (AR) إزالة سياق الحلقة الأخير (sir_builder.h:639 - loopStack_)
    // (EN) Remove last loop context
    if (!loopStack_.empty()) {
        loopStack_.pop_back();
    }
}

// ============================================================================
// getCurrentLoop - الحصول على سياق الحلقة الحالية
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:622
// التوقيع / Signature: LoopContext* getCurrentLoop();
//
// المتغيرات المستخدمة / Used variables:
// - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
//
// الإرجاع / Returns:
// - LoopContext*: مؤشر لسياق الحلقة الحالية أو nullptr
// ============================================================================
LoopContext* SIRBuilder::getCurrentLoop() {
    // (AR) إرجاع سياق الحلقة الأخير (sir_builder.h:639 - loopStack_)
    // (EN) Return last loop context
    if (!loopStack_.empty()) {
        return &loopStack_.back();
    }
    
    // (AR) لا توجد حلقة حالية
    // (EN) No current loop
    return nullptr;
}

// ============================================================================
// enterGenericScope - دخول نطاق أنواع عامة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:641
// التوقيع / Signature: void enterGenericScope(const std::vector<TypeParameter>& typeParams);
//
// المعاملات / Parameters:
// - typeParams: const std::vector<TypeParameter>& = sir_builder.h:187 (TypeParameter)
//
// المتغيرات المستخدمة / Used variables:
// - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
// ============================================================================
void SIRBuilder::enterGenericScope(const std::vector<TypeParameter>& typeParams) {
    // (AR) إنشاء نطاق جديد وإضافته (sir_builder.h:643 - genericScopeStack_)
    // (EN) Create new scope and add it
    GenericScope scope;
    scope.typeParameters = typeParams;
    genericScopeStack_.push_back(scope);
}

// ============================================================================
// exitGenericScope - الخروج من نطاق الأنواع العامة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:649
// التوقيع / Signature: void exitGenericScope();
//
// المتغيرات المستخدمة / Used variables:
// - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
// ============================================================================
void SIRBuilder::exitGenericScope() {
    // (AR) إزالة النطاق الأخير (sir_builder.h:643 - genericScopeStack_)
    // (EN) Remove last scope
    if (!genericScopeStack_.empty()) {
        genericScopeStack_.pop_back();
    }
}

// ============================================================================
// EXPRESSION BUILDING - بناء التعابير
// ============================================================================

// ============================================================================
// buildExpression - بناء تعبير (موزع للأنواع المختلفة)
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:432
// التوقيع / Signature: BuildResult buildExpression(AST::ExpressionNode* expr);
//
// المعاملات / Parameters:
// - expr: AST::ExpressionNode* = Sad::AST::Expression* (sir_builder.h:53)
//
// الإرجاع / Returns:
// - BuildResult: sir_builder.h:103 (contains registerName, type)
//
// Expression Types (expressions.h):
// - BinaryExpr (line 40): left, op, right
// - UnaryExpr (line 78): op, operand
// - LiteralExpr (line 174): token
// - VariableExpr (line 206): name
// - CallExpr (line 276): callee, arguments
// - MemberExpr (line 353): object, member
// ============================================================================
BuildResult SIRBuilder::buildExpression(AST::ExpressionNode* expr) {
    if (!expr) {
        return BuildResult();
    }
    
    // (AR) BinaryExpr - تعبير ثنائي (expressions.h:40)
    // (EN) Binary expression
    if (auto binaryExpr = dynamic_cast<Sad::AST::BinaryExpr*>(expr)) {
        return buildBinaryOp(binaryExpr);
    }
    
    // (AR) UnaryExpr - تعبير أحادي (expressions.h:78)
    // (EN) Unary expression
    if (auto unaryExpr = dynamic_cast<Sad::AST::UnaryExpr*>(expr)) {
        return buildUnaryOp(unaryExpr);
    }
    
    // (AR) LiteralExpr - ثابت حرفي (expressions.h:174)
    // (EN) Literal
    if (auto literalExpr = dynamic_cast<Sad::AST::LiteralExpr*>(expr)) {
        return buildLiteral(literalExpr);
    }
    
    // (AR) VariableExpr - متغير (expressions.h:206)
    // (EN) Variable
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(expr)) {
        return buildVariableAccess(varExpr);
    }
    
    // (AR) CallExpr - استدعاء دالة (expressions.h:276)
    // (EN) Function call
    if (auto callExpr = dynamic_cast<Sad::AST::CallExpr*>(expr)) {
        return buildFunctionCall(callExpr);
    }
    
    // (AR) AssignExpr - إسناد متغير (expressions.h:247)
    // (EN) Variable assignment
    if (auto assignExpr = dynamic_cast<Sad::AST::AssignExpr*>(expr)) {
        buildAssignment(assignExpr);
        // (AR) بعد الإسناد، نرجع المتغير المُسند إليه
        // (EN) After assignment, return the assigned variable
        return buildVariableAccess(dynamic_cast<Sad::AST::VariableExpr*>(expr));
    }
    
    // (AR) تعبير غير معروف - نرجع قيمة افتراضية
    // (EN) Unknown expression - return default
    return BuildResult();
}

// ============================================================================
// buildLiteral - بناء ثابت حرفي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:492
// التوقيع / Signature: BuildResult buildLiteral(AST::LiteralNode* literal);
//
// المعاملات / Parameters:
// - literal: AST::LiteralNode* = Sad::AST::LiteralExpr* (sir_builder.h:71)
//
// LiteralExpr Members (expressions.h:174):
// - token: Lexer::Token (line 175)
//
// Token Members (lexer/token.h):
// - type: TokenType
// - value: std::string
// - getType(): TokenType
// - getValue(): std::string
//
// TokenType Values (lexer/token.h):
// - INTEGER, FLOAT, STRING, TRUE, FALSE, NONE
//
// الإرجاع / Returns:
// - BuildResult with constant value
// ============================================================================
BuildResult SIRBuilder::buildLiteral(AST::LiteralNode* literal) {
    if (!literal) {
        return BuildResult();
    }
    
    // (AR) الحصول على Token (expressions.h:175 - token member)
    // (EN) Get token
    const auto& token = literal->token;
    Lexer::TokenType tokenType = token.getType();
    std::string value = token.getValue();
    
    // (AR) إنشاء سجل مؤقت للنتيجة
    // (EN) Create temporary register for result
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد النوع بناءً على نوع Token
    // (EN) Determine type based on token type
    SIRType sirType = SIRType::I64; // default
    
    // (AR) معالجة الأنواع المختلفة
    // (EN) Handle different types
    // أسماء TokenType من token.h:198-202
    // TokenType names from token.h:198-202
    if (tokenType == Lexer::TokenType::NUMBER_INTEGER) {
        sirType = SIRType::I64;
        // (AR) TODO: توليد تعليمة LOAD_CONST للعدد الصحيح
        // (EN) TODO: Generate LOAD_CONST instruction for integer
    }
    else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE) {
        sirType = SIRType::F64;
        // (AR) TODO: توليد تعليمة LOAD_CONST للعدد العشري
        // (EN) TODO: Generate LOAD_CONST instruction for float
    }
    else if (tokenType == Lexer::TokenType::STRING_LITERAL) {
        sirType = SIRType::STRING;
        // (AR) TODO: توليد تعليمة LOAD_STR للنص
        // (EN) TODO: Generate LOAD_STR instruction for string
    }
    else if (tokenType == Lexer::TokenType::LITERAL_TRUE || tokenType == Lexer::TokenType::LITERAL_FALSE) {
        sirType = SIRType::BOOL;
        // (AR) TODO: توليد تعليمة LOAD_CONST للمنطقي
        // (EN) TODO: Generate LOAD_CONST instruction for boolean
    }
    
    return BuildResult(resultReg, sirType);
}

// ============================================================================
// buildVariableAccess - بناء وصول لمتغير
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:480
// التوقيع / Signature: BuildResult buildVariableAccess(AST::VariableNode* var);
//
// المعاملات / Parameters:
// - var: AST::VariableNode* = Sad::AST::VariableExpr* (sir_builder.h:70)
//
// VariableExpr Members (expressions.h:206):
// - name: std::string (line 208)
//
// الدوال المستدعاة / Called functions:
// - lookupVariable: sir_builder.h:597 (returns VariableInfo*)
//
// الإرجاع / Returns:
// - BuildResult with variable register and type
// ============================================================================
BuildResult SIRBuilder::buildVariableAccess(AST::VariableNode* var) {
    if (!var) {
        return BuildResult();
    }
    
    // (AR) البحث عن المتغير في النطاقات (expressions.h:208 - name member)
    // (EN) Lookup variable in scopes
    VariableInfo* varInfo = lookupVariable(var->name);
    
    if (!varInfo) {
        errors_.push_back("Error: Undefined variable '" + var->name + "'");
        return BuildResult();
    }
    
    // (AR) إرجاع معلومات المتغير (sir_builder.h:139 - VariableInfo struct)
    // (EN) Return variable information
    // VariableInfo members: registerName (line 143), type (line 142)
    return BuildResult(varInfo->registerName, varInfo->type);
}

// ============================================================================
// buildBinaryOp - بناء عملية ثنائية
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:448
// التوقيع / Signature: BuildResult buildBinaryOp(AST::BinaryOpNode* binOp);
//
// المعاملات / Parameters:
// - binOp: AST::BinaryOpNode* = Sad::AST::BinaryExpr* (sir_builder.h:66)
//
// BinaryExpr Members (expressions.h:40):
// - left: ExprPtr (line 42)
// - op: Lexer::TokenType (line 43)
// - right: ExprPtr (line 44)
//
// الدوال المستدعاة / Called functions:
// - buildExpression: sir_builder.h:432 (recursive)
// - newTempRegister: sir_builder.h:511
//
// الإرجاع / Returns:
// - BuildResult with result register
// ============================================================================
BuildResult SIRBuilder::buildBinaryOp(AST::BinaryOpNode* binOp) {
    if (!binOp) {
        return BuildResult();
    }
    
    // (AR) بناء المعامل الأيسر (expressions.h:42 - left: ExprPtr)
    // (EN) Build left operand
    auto leftResult = buildExpression(binOp->left.get());
    
    // (AR) بناء المعامل الأيمن (expressions.h:44 - right: ExprPtr)
    // (EN) Build right operand
    auto rightResult = buildExpression(binOp->right.get());
    
    // (AR) إنشاء سجل للنتيجة
    // (EN) Create result register
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد نوع النتيجة
    // (EN) Determine result type
    SIRType resultType = leftResult.type;
    
    // (AR) TODO: توليد التعليمة المناسبة بناءً على نوع العملية
    // (EN) TODO: Generate appropriate instruction based on operation type
    // op: Lexer::TokenType (expressions.h:43)
    // يمكن أن يكون: PLUS, MINUS, STAR, SLASH, etc.
    
    return BuildResult(resultReg, resultType);
}

// ============================================================================
// buildUnaryOp - بناء عملية أحادية
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:456
// التوقيع / Signature: BuildResult buildUnaryOp(AST::UnaryOpNode* unaryOp);
//
// المعاملات / Parameters:
// - unaryOp: AST::UnaryOpNode* = Sad::AST::UnaryExpr* (sir_builder.h:67)
//
// UnaryExpr Members (expressions.h:78):
// - op: Lexer::TokenType (line 80)
// - operand: ExprPtr (line 81)
// ============================================================================
BuildResult SIRBuilder::buildUnaryOp(AST::UnaryOpNode* unaryOp) {
    if (!unaryOp) {
        return BuildResult();
    }
    
    // (AR) بناء المعامل (expressions.h:81 - operand: ExprPtr)
    // (EN) Build operand
    auto operandResult = buildExpression(unaryOp->operand.get());
    
    // (AR) إنشاء سجل للنتيجة
    // (EN) Create result register
    std::string resultReg = newTempRegister();
    
    // (AR) TODO: توليد التعليمة المناسبة (NOT, NEG, etc)
    // (EN) TODO: Generate appropriate instruction
    // op: Lexer::TokenType (expressions.h:80)
    
    return BuildResult(resultReg, operandResult.type);
}

// ============================================================================
// buildFunctionCall - بناء استدعاء دالة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:464
// التوقيع / Signature: BuildResult buildFunctionCall(AST::FunctionCallNode* call);
//
// المعاملات / Parameters:
// - call: AST::FunctionCallNode* = Sad::AST::CallExpr* (sir_builder.h:68)
//
// CallExpr Members (expressions.h:276):
// - callee: ExprPtr (line 278)
// - arguments: ExprList (line 279)
// ============================================================================
BuildResult SIRBuilder::buildFunctionCall(AST::FunctionCallNode* call) {
    if (!call) {
        return BuildResult();
    }
    
    // (AR) TODO: بناء استدعاء الدالة
    // (EN) TODO: Build function call
    // callee: ExprPtr (expressions.h:278)
    // arguments: ExprList = vector<ExprPtr> (expressions.h:279)
    
    std::string resultReg = newTempRegister();
    return BuildResult(resultReg, SIRType::VOID);
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad


