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
        
        // (AR) دالة قالب؟ (declarations.h:470 - class TemplateFunctionDecl)
        // (EN) Template function declaration?
        if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl*>(stmt.get())) {
            buildTemplateFunction(templateDecl);
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
    
    // (AR) إنشاء نطاق جديد للدالة
    // (EN) Create new scope for function
    enterScope();
    
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
    
    // (AR) الخروج من نطاق الدالة
    // (EN) Exit function scope
    exitScope();
    
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
    
    std::cout << "[DEBUG] buildClass: processing class '" << classDecl->name << "'" << std::endl;
    
    // (AR) تحديد الصنف الأب (إن وجد)
    // (EN) Determine parent class (if any)
    std::string parentClass = "";
    if (!classDecl->superclasses.empty()) {
        parentClass = classDecl->superclasses[0];  // دعم وراثة واحدة حالياً
        std::cout << "[DEBUG] buildClass: parent class = '" << parentClass << "'" << std::endl;
    }
    
    // (AR) إنشاء صنف SIR (SIRClass constructor: sir_module.h:409)
    // (EN) Create SIR class
    auto sirClass = std::make_shared<SIRClass>(classDecl->name, parentClass);
    
    // (AR) معالجة أعضاء الصنف (members)
    // (EN) Process class members
    for (const auto& member : classDecl->members) {
        // (AR) التحقق من نوع العضو: حقل أو دالة
        // (EN) Check member type: field or method
        
        // (AR) الحقول (FieldDecl - declarations.h:180)
        // (EN) Fields
        if (auto fieldDecl = dynamic_cast<AST::FieldDecl*>(member.get())) {
            std::cout << "[DEBUG] buildClass: found field '" << fieldDecl->name << "'" << std::endl;
            
            // (AR) تحويل النوع وإضافة الحقل
            // (EN) Convert type and add field
            SIRType fieldType = astTypeToSIRType(fieldDecl->type);
            sirClass->addField(fieldDecl->name, fieldType);
        }
        
        // (AR) الدوال (MethodDecl - declarations.h:222)
        // (EN) Methods
        else if (auto methodDecl = dynamic_cast<AST::MethodDecl*>(member.get())) {
            std::cout << "[DEBUG] buildClass: found method '" << methodDecl->name << "'" << std::endl;
            
            // (AR) إنشاء دالة SIR للطريقة
            // (EN) Create SIR function for method
            SIRType returnType = astTypeToSIRType(methodDecl->returnType);
            
            // (AR) اسم الدالة الكامل: ClassName.methodName
            // (EN) Full method name: ClassName.methodName
            std::string fullMethodName = classDecl->name + "." + methodDecl->name;
            auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);
            
            // (AR) إضافة معاملات الدالة
            // (EN) Add function parameters
            // أولاً: إضافة معامل ضمني 'self' للوصول للكائن
            // First: add implicit 'self' parameter for object access
            sirMethod->addParameter(SIRParameter("self", SIRType::PTR));
            
            for (const auto& param : methodDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirMethod->addParameter(SIRParameter(param.name, paramType));
            }
            
            // (AR) إضافة الدالة للصنف
            // (EN) Add method to class
            sirClass->addMethod(sirMethod);
            
            // (AR) TODO: بناء جسم الدالة
            // (EN) TODO: Build method body
            // سيتم في مرحلة لاحقة
        }
        
        // (AR) الدالة العادية داخل الصنف (FunctionDecl)
        // (EN) Regular function inside class (FunctionDecl)
        else if (auto funcDecl = dynamic_cast<AST::FunctionDecl*>(member.get())) {
            std::cout << "[DEBUG] buildClass: found function as method '" << funcDecl->name << "'" << std::endl;
            
            SIRType returnType = astTypeToSIRType(funcDecl->returnType);
            std::string fullMethodName = classDecl->name + "." + funcDecl->name;
            auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);
            
            // معامل self ضمني
            sirMethod->addParameter(SIRParameter("self", SIRType::PTR));
            
            for (const auto& param : funcDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirMethod->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirMethod);
        }
    }
    
    // (AR) إضافة الصنف للوحدة (sir_module.h:608 - addClass)
    // (EN) Add class to module
    module_->addClass(sirClass);
    
    std::cout << "[DEBUG] buildClass: class '" << classDecl->name << "' added with " 
              << sirClass->fields_.size() << " fields and "
              << sirClass->methods_.size() << " methods" << std::endl;
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
    
    std::cout << "[DEBUG] buildStatement: processing statement" << std::endl;
    
    // (AR) BlockStmt - كتلة من الجمل (statements.h:423)
    // (EN) Block statement
    if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt*>(stmt)) {
        std::cout << "[DEBUG] Found BlockStmt with " << blockStmt->statements.size() << " statements" << std::endl;
        // (AR) معالجة كل جملة في الكتلة
        // (EN) Process each statement in the block
        for (const auto& innerStmt : blockStmt->statements) {
            buildStatement(innerStmt.get());
        }
        return;
    }
    
    // (AR) IfStmt - جملة if (statements.h:104)
    // (EN) If statement
    if (auto ifStmt = dynamic_cast<Sad::AST::IfStmt*>(stmt)) {
        std::cout << "[DEBUG] Found IfStmt" << std::endl;
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
    
    // (AR) ForRangeStmt - حلقة لكل في (statements.h:228)
    // (EN) For-range loop (foreach)
    if (auto forRangeStmt = dynamic_cast<Sad::AST::ForRangeStmt*>(stmt)) {
        std::cout << "[DEBUG] Found ForRangeStmt" << std::endl;
        buildForRangeLoop(forRangeStmt);
        return;
    }
    
    // (AR) ReturnStmt - جملة return (statements.h:266)
    // (EN) Return statement
    if (auto returnStmt = dynamic_cast<Sad::AST::ReturnStmt*>(stmt)) {
        std::cout << "[DEBUG] Found ReturnStmt" << std::endl;
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
        std::cout << "[DEBUG] Found VarDeclStmt: " << varDecl->name << std::endl;
        buildLocalVariable(varDecl);
        return;
    }
    
    // (AR) TemplateFunctionDecl - تصريح دالة قالب (declarations.h:470)
    // (EN) Template function declaration
    if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl*>(stmt)) {
        std::cout << "[DEBUG] Found TemplateFunctionDecl: " << templateDecl->name << std::endl;
        buildTemplateFunction(templateDecl);
        return;
    }
    
    std::cout << "[DEBUG] Unknown statement type!" << std::endl;
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
        
        std::cout << "[DEBUG] buildReturnStatement: registerName='" << valueResult.registerName 
                  << "', isConstant=" << valueResult.isConstant 
                  << ", constantValue='" << valueResult.constantValue << "'" << std::endl;
        
        // (AR) توليد تعليمة RET مع القيمة
        // (EN) Generate RET instruction with value
        SIRInstruction retInst;
        retInst.opcode = SIROpcode::RET;
        
        // (AR) إذا كانت القيمة في متغير، نحتاج لتحميلها أولاً
        // (EN) If value is in a variable, we need to load it first
        if (!valueResult.registerName.empty() && valueResult.registerName[0] == '%') {
            // (AR) القيمة في عنوان alloca، نحتاج لتحميلها
            // (EN) Value is in alloca address, need to load it
            std::string loadedReg = newTempRegister();
            
            SIRInstruction loadInst;
            loadInst.opcode = SIROpcode::LOAD;
            loadInst.result = SIROperand::Register(loadedReg, valueResult.type);
            loadInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
            
            if (currentBlock_) {
                currentBlock_->addInstruction(loadInst);
            }
            
            // (AR) استخدام السجل المحمّل للإرجاع
            // (EN) Use loaded register for return
            SIROperand retOperand = SIROperand::Register(loadedReg, valueResult.type);
            retInst.operands.push_back(retOperand);
        } else if (valueResult.isConstant && !valueResult.constantValue.empty()) {
            // (AR) القيمة ثابتة
            // (EN) Value is constant
            switch (valueResult.type) {
                case SIRType::I64:
                    retInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                    break;
                case SIRType::F64:
                    retInst.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                    break;
                case SIRType::BOOL:
                    retInst.operands.push_back(SIROperand::ConstantBool(valueResult.constantValue == "true"));
                    break;
                default:
                    retInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
            }
        } else {
            // (AR) قيمة في سجل مؤقت
            // (EN) Value in temporary register
            SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
            retInst.operands.push_back(retOperand);
        }
        
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
    bool needsTypeInference = (varDecl->type == Data::DataType::UNKNOWN);
    
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
    // (EN) Handle initializer - process first for type inference
    BuildResult initResult;
    bool hasInitializer = varDecl->initializer && currentBlock_;
    
    if (hasInitializer) {
        initResult = buildExpression(varDecl->initializer.get());
        
        // (AR) استنتاج النوع من التعبير إذا كان النوع غير معروف
        // (EN) Infer type from expression if type is unknown
        if (needsTypeInference) {
            varType = initResult.type;
            varInfo.type = varType;
            std::cerr << "[DEBUG] Type inference: inferred type " 
                      << static_cast<int>(varType) << " for variable " 
                      << varDecl->name << std::endl;
        }
        
        std::cerr << "[DEBUG] buildLocalVariable: isConstant=" << initResult.isConstant 
                  << ", constantValue='" << initResult.constantValue << "'"
                  << ", type=" << static_cast<int>(initResult.type) << std::endl;
    }
    
    // (AR) توليد تعليمة ALLOC لتخصيص الذاكرة
    // (EN) Generate ALLOC instruction for memory allocation
    if (currentBlock_) {
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(varInfo.registerName, varType);
        currentBlock_->addInstruction(allocInst);
    }
    
    // (AR) توليد تعليمة STORE لإسناد القيمة الأولية
    // (EN) Generate STORE instruction to assign initial value
    if (hasInitializer) {
        SIRInstruction storeInst;
        storeInst.opcode = SIROpcode::STORE;
        
        // (AR) المعامل الأول: القيمة المراد تخزينها
        // (EN) First operand: value to store
        if (initResult.isConstant && !initResult.constantValue.empty()) {
            std::cerr << "[DEBUG] Using constant value" << std::endl;
            // (AR) القيمة ثابتة - تحويلها لثابت SIR
            // (EN) Value is constant - convert to SIR constant
            switch (initResult.type) {
                case SIRType::I64:
                    storeInst.operands.push_back(SIROperand::ConstantI64(std::stoll(initResult.constantValue)));
                    std::cerr << "[DEBUG] Added I64 constant: " << initResult.constantValue << std::endl;
                    break;
                case SIRType::F64:
                    storeInst.operands.push_back(SIROperand::ConstantF64(std::stod(initResult.constantValue)));
                    break;
                case SIRType::BOOL:
                    storeInst.operands.push_back(SIROperand::ConstantBool(initResult.constantValue == "true" || initResult.constantValue == "1"));
                    break;
                case SIRType::STRING:
                    storeInst.operands.push_back(SIROperand::ConstantString(initResult.constantValue));
                    break;
                default:
                    storeInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
                    break;
            }
        } else {
            std::cerr << "[DEBUG] Using register value: " << initResult.registerName << std::endl;
            // (AR) القيمة في سجل
            // (EN) Value is in register
            storeInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
        }
        
        // (AR) المعامل الثاني: عنوان المتغير
        // (EN) Second operand: variable address
        storeInst.operands.push_back(SIROperand::Register(varInfo.registerName, varType));
        
        currentBlock_->addInstruction(storeInst);
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
    
    std::cout << "[DEBUG] buildIfStatement: starting" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 1: إنشاء الكتل الأساسية
    // (EN) Step 1: Create basic blocks
    // المصدر: sir_builder.h:501 - createBasicBlock()
    // المصدر: sir_builder.h:520 - newLabel()
    // ========================================================================
    std::string thenLabel = newLabel("then");
    std::string mergeLabel = newLabel("merge");
    
    // (AR) كتلة else فقط إذا كان هناك فرع else
    // (EN) Else block only if there's an else branch
    std::string elseLabel = ifStmt->elseBranch ? newLabel("else") : mergeLabel;
    
    auto thenBlock = createBasicBlock(thenLabel);
    auto elseBlock = ifStmt->elseBranch ? createBasicBlock(elseLabel) : nullptr;
    auto mergeBlock = createBasicBlock(mergeLabel);
    
    std::cout << "[DEBUG] buildIfStatement: created blocks then=" << thenLabel 
              << ", else=" << elseLabel << ", merge=" << mergeLabel << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 2: بناء الشرط
    // (EN) Step 2: Build condition expression
    // المصدر: IfStmt::condition (statements.h:107)
    // ========================================================================
    auto condResult = buildExpression(ifStmt->condition.get());
    
    if (condResult.registerName.empty()) {
        std::cout << "[DEBUG] buildIfStatement: condition build failed!" << std::endl;
        errors_.push_back("Error: Failed to build if condition");
        return;
    }
    
    std::cout << "[DEBUG] buildIfStatement: condition reg=" << condResult.registerName << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 3: توليد تعليمة القفز الشرطي
    // (EN) Step 3: Generate conditional branch instruction
    // المصدر: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
    // المصدر: sir_types.h:366-372 - SIROperand::Label()
    // ========================================================================
    SIROperand condOp = SIROperand::Register(condResult.registerName, condResult.type);
    SIROperand thenLabelOp = SIROperand::Label(thenLabel);
    SIROperand elseLabelOp = SIROperand::Label(elseLabel);
    
    SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, thenLabelOp, elseLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondInst);
        std::cout << "[DEBUG] buildIfStatement: added BR_COND to current block" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 4: بناء فرع then
    // (EN) Step 4: Build then branch
    // المصدر: IfStmt::thenBranch (statements.h:108)
    // ========================================================================
    currentBlock_ = thenBlock;
    if (ifStmt->thenBranch) {
        buildStatement(ifStmt->thenBranch.get());
    }
    
    // (AR) قفز غير شرطي إلى merge (sir_instruction.h:178-183)
    // (EN) Unconditional jump to merge
    SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
    SIRInstruction brMergeInst = SIRInstruction::Branch(mergeLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brMergeInst);
        std::cout << "[DEBUG] buildIfStatement: added BR to merge from then" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 5: بناء فرع else إذا وُجد
    // (EN) Step 5: Build else branch if exists
    // المصدر: IfStmt::elseBranch (statements.h:109)
    // ========================================================================
    if (ifStmt->elseBranch && elseBlock) {
        currentBlock_ = elseBlock;
        buildStatement(ifStmt->elseBranch.get());
        
        // (AR) قفز غير شرطي إلى merge
        // (EN) Unconditional jump to merge
        if (currentBlock_) {
            currentBlock_->instructions.push_back(brMergeInst);
            std::cout << "[DEBUG] buildIfStatement: added BR to merge from else" << std::endl;
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 6: الاستمرار بعد if
    // (EN) Step 6: Continue after if statement
    // ========================================================================
    currentBlock_ = mergeBlock;
    std::cout << "[DEBUG] buildIfStatement: completed, now at merge block" << std::endl;
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
    
    std::cout << "[DEBUG] buildWhileLoop: starting" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 1: إنشاء الكتل الأساسية
    // (EN) Step 1: Create basic blocks
    // المصدر: sir_builder.h:501 - createBasicBlock()
    // المصدر: sir_builder.h:520 - newLabel()
    // ========================================================================
    std::string condLabel = newLabel("while_cond");
    std::string bodyLabel = newLabel("while_body");
    std::string exitLabel = newLabel("while_exit");
    
    auto condBlock = createBasicBlock(condLabel);
    auto bodyBlock = createBasicBlock(bodyLabel);
    auto exitBlock = createBasicBlock(exitLabel);
    
    std::cout << "[DEBUG] buildWhileLoop: created blocks cond=" << condLabel 
              << ", body=" << bodyLabel << ", exit=" << exitLabel << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 2: قفز غير شرطي إلى كتلة الشرط
    // (EN) Step 2: Unconditional jump to condition block
    // المصدر: sir_instruction.h:178-183 - SIRInstruction::Branch()
    // ========================================================================
    SIROperand condLabelOp = SIROperand::Label(condLabel);
    SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondBlockInst);
        std::cout << "[DEBUG] buildWhileLoop: added BR to condition block" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 3: بناء الشرط
    // (EN) Step 3: Build condition expression
    // المصدر: WhileStmt::condition (statements.h:149)
    // ========================================================================
    currentBlock_ = condBlock;
    auto condResult = buildExpression(whileLoop->condition.get());
    
    if (condResult.registerName.empty()) {
        std::cout << "[DEBUG] buildWhileLoop: condition build failed!" << std::endl;
        errors_.push_back("Error: Failed to build while condition");
        return;
    }
    
    std::cout << "[DEBUG] buildWhileLoop: condition reg=" << condResult.registerName << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 4: توليد تعليمة القفز الشرطي
    // (EN) Step 4: Generate conditional branch instruction
    // المصدر: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
    // ========================================================================
    SIROperand condOp = SIROperand::Register(condResult.registerName, condResult.type);
    SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
    SIROperand exitLabelOp = SIROperand::Label(exitLabel);
    
    SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondInst);
        std::cout << "[DEBUG] buildWhileLoop: added BR_COND (body/exit)" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 5: بناء جسم الحلقة
    // (EN) Step 5: Build loop body
    // المصدر: WhileStmt::body (statements.h:150)
    // ========================================================================
    currentBlock_ = bodyBlock;
    if (whileLoop->body) {
        buildStatement(whileLoop->body.get());
    }
    
    // ========================================================================
    // (AR) الخطوة 6: قفز للعودة إلى كتلة الشرط
    // (EN) Step 6: Jump back to condition block
    // ========================================================================
    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brBackInst);
        std::cout << "[DEBUG] buildWhileLoop: added BR back to condition" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 7: الاستمرار بعد الحلقة
    // (EN) Step 7: Continue after loop
    // ========================================================================
    currentBlock_ = exitBlock;
    std::cout << "[DEBUG] buildWhileLoop: completed, now at exit block" << std::endl;
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
    
    std::cout << "[DEBUG] buildForLoop: starting" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 1: دخول نطاق جديد للحلقة
    // (EN) Step 1: Enter new scope for loop
    // المصدر: sir_builder.h:587 - enterScope()
    // ========================================================================
    enterScope();
    
    // ========================================================================
    // (AR) الخطوة 2: تنفيذ initializer إذا وُجد
    // (EN) Step 2: Execute initializer if exists
    // المصدر: ForStmt::initializer (statements.h:193)
    // ========================================================================
    if (forLoop->initializer) {
        std::cout << "[DEBUG] buildForLoop: building initializer" << std::endl;
        buildStatement(forLoop->initializer.get());
    }
    
    // ========================================================================
    // (AR) الخطوة 3: إنشاء الكتل الأساسية
    // (EN) Step 3: Create basic blocks
    // ========================================================================
    std::string condLabel = newLabel("for_cond");
    std::string bodyLabel = newLabel("for_body");
    std::string incLabel = newLabel("for_inc");
    std::string exitLabel = newLabel("for_exit");
    
    auto condBlock = createBasicBlock(condLabel);
    auto bodyBlock = createBasicBlock(bodyLabel);
    auto incBlock = createBasicBlock(incLabel);
    auto exitBlock = createBasicBlock(exitLabel);
    
    std::cout << "[DEBUG] buildForLoop: created blocks cond=" << condLabel 
              << ", body=" << bodyLabel << ", inc=" << incLabel 
              << ", exit=" << exitLabel << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 4: قفز غير شرطي إلى كتلة الشرط
    // (EN) Step 4: Unconditional jump to condition block
    // ========================================================================
    SIROperand condLabelOp = SIROperand::Label(condLabel);
    SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondBlockInst);
        std::cout << "[DEBUG] buildForLoop: added BR to condition block" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 5: بناء الشرط
    // (EN) Step 5: Build condition
    // المصدر: ForStmt::condition (statements.h:194)
    // ========================================================================
    currentBlock_ = condBlock;
    
    SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
    SIROperand exitLabelOp = SIROperand::Label(exitLabel);
    
    if (forLoop->condition) {
        auto condResult = buildExpression(forLoop->condition.get());
        
        if (!condResult.registerName.empty()) {
            // (AR) توليد BR شرطي لـ body أو exit
            // (EN) Generate conditional BR to body or exit
            SIROperand condOp = SIROperand::Register(condResult.registerName, condResult.type);
            SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);
            
            if (currentBlock_) {
                currentBlock_->instructions.push_back(brCondInst);
                std::cout << "[DEBUG] buildForLoop: added BR_COND (body/exit)" << std::endl;
            }
        }
    } else {
        // (AR) لا يوجد شرط - قفز غير شرطي لـ body (حلقة لا نهائية)
        // (EN) No condition - unconditional jump to body (infinite loop)
        SIRInstruction brBodyInst = SIRInstruction::Branch(bodyLabelOp);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(brBodyInst);
            std::cout << "[DEBUG] buildForLoop: no condition, added BR to body" << std::endl;
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 6: بناء جسم الحلقة
    // (EN) Step 6: Build loop body
    // المصدر: ForStmt::body (statements.h:196)
    // ========================================================================
    currentBlock_ = bodyBlock;
    if (forLoop->body) {
        buildStatement(forLoop->body.get());
    }
    
    // (AR) قفز إلى كتلة الزيادة
    // (EN) Jump to increment block
    SIROperand incLabelOp = SIROperand::Label(incLabel);
    SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brIncInst);
        std::cout << "[DEBUG] buildForLoop: added BR to increment block" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 7: بناء الزيادة
    // (EN) Step 7: Build increment
    // المصدر: ForStmt::increment (statements.h:195)
    // ========================================================================
    currentBlock_ = incBlock;
    if (forLoop->increment) {
        buildExpression(forLoop->increment.get());
        std::cout << "[DEBUG] buildForLoop: built increment expression" << std::endl;
    }
    
    // (AR) قفز للعودة إلى كتلة الشرط
    // (EN) Jump back to condition block
    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brBackInst);
        std::cout << "[DEBUG] buildForLoop: added BR back to condition" << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 8: الاستمرار بعد الحلقة
    // (EN) Step 8: Continue after loop
    // ========================================================================
    currentBlock_ = exitBlock;
    
    // (AR) الخروج من نطاق الحلقة
    // (EN) Exit loop scope
    exitScope();
    
    std::cout << "[DEBUG] buildForLoop: completed, now at exit block" << std::endl;
}

// ============================================================================
// buildForRangeLoop - بناء حلقة لكل في (foreach)
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:405
// التوقيع / Signature: void buildForRangeLoop(AST::ForRangeStmt* forRange);
//
// المعاملات / Parameters:
// - forRange: AST::ForRangeStmt* = Sad::AST::ForRangeStmt* (statements.h:228)
//
// ForRangeStmt Members (statements.h:228-235):
// - variable: std::string (line 230) - اسم متغير الحلقة
// - valueVar: std::string (line 231) - متغير القيمة للقواميس (اختياري)
// - iterable: ExprPtr (line 232) - التعبير القابل للتكرار
// - body: StmtPtr (line 233) - جسم الحلقة
// ============================================================================
void SIRBuilder::buildForRangeLoop(AST::ForRangeStmt* forRange) {
    if (!forRange) {
        return;
    }
    
    std::cout << "[DEBUG] buildForRangeLoop: starting, variable='" 
              << forRange->variable << "'" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 1: دخول نطاق جديد للحلقة
    // (EN) Step 1: Enter new scope for loop
    // ========================================================================
    enterScope();
    
    // ========================================================================
    // (AR) الخطوة 2: بناء التعبير القابل للتكرار
    // (EN) Step 2: Build iterable expression
    // المصدر: ForRangeStmt::iterable (statements.h:232)
    // ========================================================================
    auto iterableResult = buildExpression(forRange->iterable.get());
    
    if (iterableResult.registerName.empty()) {
        std::cout << "[DEBUG] buildForRangeLoop: failed to build iterable!" << std::endl;
        errors_.push_back("Error: Failed to build iterable expression in for-range");
        exitScope();
        return;
    }
    
    std::cout << "[DEBUG] buildForRangeLoop: iterable reg='" 
              << iterableResult.registerName << "'" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 3: إنشاء الكتل الأساسية
    // (EN) Step 3: Create basic blocks
    // ========================================================================
    std::string condLabel = newLabel("foreach_cond");
    std::string bodyLabel = newLabel("foreach_body");
    std::string incLabel = newLabel("foreach_inc");
    std::string exitLabel = newLabel("foreach_exit");
    
    auto condBlock = createBasicBlock(condLabel);
    auto bodyBlock = createBasicBlock(bodyLabel);
    auto incBlock = createBasicBlock(incLabel);
    auto exitBlock = createBasicBlock(exitLabel);
    
    // ========================================================================
    // (AR) الخطوة 4: إنشاء متغير العداد (index)
    // (EN) Step 4: Create index variable
    // ========================================================================
    std::string indexReg = newTempRegister();
    
    // (AR) تهيئة العداد بـ 0
    // (EN) Initialize index to 0
    SIROperand indexOp = SIROperand::Register(indexReg, SIRType::I64);
    SIROperand zeroOp = SIROperand::ConstantI64(0);
    SIRInstruction initIndex = SIRInstruction::Binary(
        SIROpcode::ADD_I64, indexOp, zeroOp, zeroOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(initIndex);
    }
    
    // ========================================================================
    // (AR) الخطوة 5: إنشاء متغير الحلقة وتسجيله في النطاق
    // (EN) Step 5: Create loop variable and register in scope
    // المصدر: ForRangeStmt::variable (statements.h:230)
    // ========================================================================
    std::string loopVarReg = newTempRegister();
    
    // (AR) تسجيل متغير الحلقة (sir_builder.h:144 - VariableInfo)
    // (EN) Register loop variable using addVariable (sir_builder.h:591)
    VariableInfo varInfo;
    varInfo.name = forRange->variable;
    varInfo.registerName = loopVarReg;
    varInfo.type = SIRType::I64;  // (AR) نوع عنصر المصفوفة - افتراضياً I64
    varInfo.isMutable = true;
    
    addVariable(varInfo);
    
    std::cout << "[DEBUG] buildForRangeLoop: registered loop var '" 
              << forRange->variable << "' = " << loopVarReg << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 6: قفز غير شرطي إلى كتلة الشرط
    // (EN) Step 6: Unconditional jump to condition block
    // ========================================================================
    SIROperand condLabelOp = SIROperand::Label(condLabel);
    SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondBlockInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 7: بناء الشرط (index < length)
    // (EN) Step 7: Build condition (index < length)
    // ========================================================================
    currentBlock_ = condBlock;
    
    // (AR) الحصول على طول المصفوفة - حالياً نفترض أنه تعبير CALL لـ طول()
    // (EN) Get array length - currently we assume it's available
    // TODO: استدعاء دالة طول() على المصفوفة
    std::string lengthReg = newTempRegister();
    SIROperand lengthOp = SIROperand::Register(lengthReg, SIRType::I64);
    
    // (AR) إنشاء تعليمة استدعاء طول (ARRAY_LEN)
    // (EN) Create length call instruction
    SIROperand iterOp = SIROperand::Register(iterableResult.registerName, iterableResult.type);
    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
    lenInst.result = lengthOp;
    lenInst.operands.push_back(iterOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(lenInst);
    }
    
    // (AR) مقارنة index < length
    // (EN) Compare index < length
    std::string condReg = newTempRegister();
    SIROperand condResultOp = SIROperand::Register(condReg, SIRType::BOOL);
    SIRInstruction cmpInst = SIRInstruction::Binary(
        SIROpcode::LT, condResultOp, indexOp, lengthOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(cmpInst);
    }
    
    // (AR) قفز شرطي
    // (EN) Conditional branch
    SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
    SIROperand exitLabelOp = SIROperand::Label(exitLabel);
    SIRInstruction brCondInst = SIRInstruction::BranchCond(
        condResultOp, bodyLabelOp, exitLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 8: بناء جسم الحلقة
    // (EN) Step 8: Build loop body
    // ========================================================================
    currentBlock_ = bodyBlock;
    
    // (AR) تحميل العنصر الحالي من المصفوفة: loopVar = iterable[index]
    // (EN) Load current element from array
    SIROperand loopVarOp = SIROperand::Register(loopVarReg, SIRType::I64);
    SIRInstruction loadElem(SIROpcode::ARRAY_GET);
    loadElem.result = loopVarOp;
    loadElem.operands.push_back(iterOp);
    loadElem.operands.push_back(indexOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(loadElem);
    }
    
    // (AR) بناء جسم الحلقة
    // (EN) Build loop body
    if (forRange->body) {
        buildStatement(forRange->body.get());
    }
    
    // (AR) قفز إلى كتلة الزيادة
    // (EN) Jump to increment block
    SIROperand incLabelOp = SIROperand::Label(incLabel);
    SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brIncInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 9: بناء الزيادة (index = index + 1)
    // (EN) Step 9: Build increment (index = index + 1)
    // ========================================================================
    currentBlock_ = incBlock;
    
    SIROperand oneOp = SIROperand::ConstantI64(1);
    SIRInstruction incInst = SIRInstruction::Binary(
        SIROpcode::ADD_I64, indexOp, indexOp, oneOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(incInst);
    }
    
    // (AR) قفز للعودة إلى الشرط
    // (EN) Jump back to condition
    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brBackInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 10: الاستمرار بعد الحلقة
    // (EN) Step 10: Continue after loop
    // ========================================================================
    currentBlock_ = exitBlock;
    
    // (AR) الخروج من نطاق الحلقة
    // (EN) Exit loop scope
    exitScope();
    
    std::cout << "[DEBUG] buildForRangeLoop: completed" << std::endl;
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
//
// DataType Values (data_types.h:20):
// - UNKNOWN, INTEGER, FLOAT, STRING, BOOLEAN, NONE, ARRAY, MAP, TUPLE,
//   FUNCTION, OBJECT, ENUM, BYTE, ERROR
// ============================================================================
SIRType SIRBuilder::astTypeToSIRType(const Sad::Data::DataType& type) {
    // (AR) تحويل DataType إلى SIRType
    // (EN) Convert DataType to SIRType
    switch (type) {
        case Data::DataType::INTEGER:
            return SIRType::I64;
        case Data::DataType::FLOAT:
            return SIRType::F64;
        case Data::DataType::BOOLEAN:
            return SIRType::BOOL;
        case Data::DataType::STRING:
            return SIRType::STRING;
        case Data::DataType::ARRAY:
            return SIRType::ARRAY;
        case Data::DataType::FUNCTION:
            return SIRType::FUNCTION;
        case Data::DataType::OBJECT:
            return SIRType::STRUCT;
        case Data::DataType::NONE:
            return SIRType::VOID;
        case Data::DataType::UNKNOWN:
            // (AR) نوع غير معروف - سيتم استنتاجه من التعبير
            // (EN) Unknown type - will be inferred from expression
            return SIRType::I64;  // Default, will be overwritten by type inference
        default:
            // (AR) أنواع أخرى (MAP, TUPLE, ENUM, BYTE, ERROR)
            // (EN) Other types (MAP, TUPLE, ENUM, BYTE, ERROR)
            return SIRType::I64;  // Fallback
    }
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
        std::cout << "[DEBUG] buildExpression: expr is null!" << std::endl;
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildExpression: processing expression of type: " << typeid(*expr).name() << std::endl;
    
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
        std::cout << "[DEBUG] buildExpression: found VariableExpr" << std::endl;
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
    
    // (AR) NewExpr - إنشاء كائن جديد (class_nodes.h:164)
    // (EN) New object creation
    if (auto newExpr = dynamic_cast<Sad::AST::NewExpr*>(expr)) {
        return buildNewObject(newExpr);
    }
    
    // (AR) MemberAccessExpr - الوصول لعضو في كائن (class_nodes.h:206)
    // (EN) Member access
    if (auto memberExpr = dynamic_cast<Sad::AST::MemberAccessExpr*>(expr)) {
        return buildMemberAccess(memberExpr);
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
    BuildResult result;
    result.registerName = resultReg;
    result.isConstant = true;
    result.constantValue = value;
    
    if (tokenType == Lexer::TokenType::NUMBER_INTEGER) {
        result.type = SIRType::I64;
    }
    else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE) {
        result.type = SIRType::F64;
    }
    else if (tokenType == Lexer::TokenType::STRING_LITERAL) {
        result.type = SIRType::STRING;
    }
    else if (tokenType == Lexer::TokenType::LITERAL_TRUE || tokenType == Lexer::TokenType::LITERAL_FALSE) {
        result.type = SIRType::BOOL;
        result.constantValue = (tokenType == Lexer::TokenType::LITERAL_TRUE) ? "true" : "false";
    }
    else {
        result.type = SIRType::I64;
    }
    
    return result;
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
        std::cout << "[DEBUG] buildVariableAccess: var is null!" << std::endl;
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildVariableAccess: looking up variable '" << var->name << "'" << std::endl;
    
    // (AR) البحث عن المتغير في النطاقات (expressions.h:208 - name member)
    // (EN) Lookup variable in scopes
    VariableInfo* varInfo = lookupVariable(var->name);
    
    if (!varInfo) {
        std::cout << "[DEBUG] buildVariableAccess: variable NOT FOUND!" << std::endl;
        errors_.push_back("Error: Undefined variable '" + var->name + "'");
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildVariableAccess: found variable, registerName='" 
              << varInfo->registerName << "', type=" << static_cast<int>(varInfo->type) << std::endl;
    
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
// BinaryExpr Members (expressions.h:40-44):
// - left: ExprPtr (line 42)
// - op: Lexer::TokenType (line 43)
// - right: ExprPtr (line 44)
//
// TokenType للعمليات (token.h:205-229):
// - OP_PLUS (205), OP_MINUS (206), OP_MULTIPLY (207), OP_DIVIDE (208)
// - OP_MODULO (209), OP_POWER (210)
// - OP_EQUAL (219), OP_NOT_EQUAL (220), OP_LESS (221), OP_LESS_EQUAL (222)
// - OP_GREATER (223), OP_GREATER_EQUAL (224)
// - OP_AND (227), OP_OR (228)
//
// SIROpcode (sir_types.h:106-130):
// - ADD_I64/ADD_F64, SUB_I64/SUB_F64, MUL_I64/MUL_F64, DIV_I64/DIV_F64
// - MOD_I64, EQ, NE, LT, LE, GT, GE, AND, OR
//
// SIRInstruction::Binary (sir_instruction.h:100-107):
// - الاستخدام: SIRInstruction::Binary(opcode, result, left, right)
//
// SIROperand::Register (sir_types.h:306-312):
// - الاستخدام: SIROperand::Register(name, type)
//
// الإرجاع / Returns:
// - BuildResult (sir_builder.h:103-132): registerName, type
// ============================================================================
BuildResult SIRBuilder::buildBinaryOp(AST::BinaryOpNode* binOp) {
    if (!binOp) {
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildBinaryOp: بدء بناء عملية ثنائية" << std::endl;
    
    // (AR) بناء المعامل الأيسر (expressions.h:42 - left: ExprPtr)
    // (EN) Build left operand
    auto leftResult = buildExpression(binOp->left.get());
    std::cout << "[DEBUG] buildBinaryOp: leftResult.registerName='" << leftResult.registerName 
              << "', type=" << static_cast<int>(leftResult.type) 
              << ", isConstant=" << leftResult.isConstant << std::endl;
    
    // (AR) بناء المعامل الأيمن (expressions.h:44 - right: ExprPtr)
    // (EN) Build right operand
    auto rightResult = buildExpression(binOp->right.get());
    std::cout << "[DEBUG] buildBinaryOp: rightResult.registerName='" << rightResult.registerName 
              << "', type=" << static_cast<int>(rightResult.type) 
              << ", isConstant=" << rightResult.isConstant << std::endl;
    
    // (AR) إنشاء سجل للنتيجة (sir_builder.h:511 - newTempRegister)
    // (EN) Create result register
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد نوع النتيجة - إذا كان أحد المعاملين عشري، النتيجة عشرية
    // (EN) Determine result type - if either operand is float, result is float
    // (AR) إذا كان أحد المعاملين نصي، النتيجة نصية (دمج نصوص)
    // (EN) If either operand is string, result is string (concatenation)
    SIRType resultType = leftResult.type;
    bool isStringOp = (leftResult.type == SIRType::STRING || rightResult.type == SIRType::STRING);
    
    if (isStringOp) {
        resultType = SIRType::STRING;
    } else if (leftResult.type == SIRType::F64 || rightResult.type == SIRType::F64) {
        resultType = SIRType::F64;
    }
    
    // (AR) تحديد رمز العملية (SIROpcode) بناءً على TokenType (token.h:205-229)
    // (EN) Determine SIROpcode based on TokenType
    SIROpcode opcode;
    bool isComparison = false;  // (AR) عمليات المقارنة تُرجع BOOL
    
    // (AR) العملية من expressions.h:43 - op: Lexer::TokenType
    // (EN) Operation from expressions.h:43
    switch (binOp->op) {
        // ========== العمليات الحسابية (token.h:205-210) ==========
        case Lexer::TokenType::OP_PLUS:
            // (AR) إذا كانت العملية على نصوص: STRING_CONCAT (sir_types.h:182)
            // (EN) If operation on strings: STRING_CONCAT
            if (isStringOp) {
                opcode = SIROpcode::STRING_CONCAT;
                std::cout << "[DEBUG] buildBinaryOp: عملية دمج نصوص (+)" << std::endl;
            } else {
                // (AR) جمع: ADD_I64 للأعداد الصحيحة، ADD_F64 للعشرية
                opcode = (resultType == SIRType::F64) ? SIROpcode::ADD_F64 : SIROpcode::ADD_I64;
                std::cout << "[DEBUG] buildBinaryOp: عملية جمع (+)" << std::endl;
            }
            break;
            
        case Lexer::TokenType::OP_MINUS:
            // (AR) طرح: SUB_I64 للأعداد الصحيحة، SUB_F64 للعشرية
            opcode = (resultType == SIRType::F64) ? SIROpcode::SUB_F64 : SIROpcode::SUB_I64;
            std::cout << "[DEBUG] buildBinaryOp: عملية طرح (-)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_MULTIPLY:
            // (AR) ضرب: MUL_I64 للأعداد الصحيحة، MUL_F64 للعشرية
            opcode = (resultType == SIRType::F64) ? SIROpcode::MUL_F64 : SIROpcode::MUL_I64;
            std::cout << "[DEBUG] buildBinaryOp: عملية ضرب (*)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_DIVIDE:
            // (AR) قسمة: DIV_I64 للأعداد الصحيحة، DIV_F64 للعشرية
            opcode = (resultType == SIRType::F64) ? SIROpcode::DIV_F64 : SIROpcode::DIV_I64;
            std::cout << "[DEBUG] buildBinaryOp: عملية قسمة (/)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_MODULO:
            // (AR) باقي القسمة: MOD_I64 (لا يوجد للعشري)
            opcode = SIROpcode::MOD_I64;
            resultType = SIRType::I64;  // (AR) باقي القسمة دائماً عدد صحيح
            std::cout << "[DEBUG] buildBinaryOp: عملية باقي القسمة (%)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_POWER:
            // (AR) الأس: BUILTIN_POW (sir_types.h:223)
            opcode = SIROpcode::BUILTIN_POW;
            resultType = SIRType::F64;  // (AR) نتيجة الأس عادةً عشرية
            std::cout << "[DEBUG] buildBinaryOp: عملية الأس (**)" << std::endl;
            break;
        
        // ========== عمليات المقارنة (token.h:219-224) ==========
        case Lexer::TokenType::OP_EQUAL:
            // (AR) يساوي: EQ للأرقام، STRING_CMP للنصوص (sir_types.h:125, 183)
            // (EN) Equal: EQ for numbers, STRING_CMP for strings
            if (isStringOp) {
                opcode = SIROpcode::STRING_CMP;
                std::cout << "[DEBUG] buildBinaryOp: عملية مقارنة نصوص (==)" << std::endl;
            } else {
                opcode = SIROpcode::EQ;
                std::cout << "[DEBUG] buildBinaryOp: عملية يساوي (==)" << std::endl;
            }
            isComparison = true;
            break;
            
        case Lexer::TokenType::OP_NOT_EQUAL:
            // (AR) لا يساوي: NE للأرقام، STRING_CMP مع NOT للنصوص
            // (EN) Not equal: NE for numbers, STRING_CMP with NOT for strings
            if (isStringOp) {
                // (AR) سيتم معالجة النفي لاحقاً
                opcode = SIROpcode::STRING_CMP;
                std::cout << "[DEBUG] buildBinaryOp: عملية عدم تساوي نصوص (!=)" << std::endl;
            } else {
                opcode = SIROpcode::NE;
                std::cout << "[DEBUG] buildBinaryOp: عملية لا يساوي (!=)" << std::endl;
            }
            isComparison = true;
            break;
            
        case Lexer::TokenType::OP_LESS:
            // (AR) أصغر من: LT (sir_types.h:127)
            opcode = SIROpcode::LT;
            isComparison = true;
            std::cout << "[DEBUG] buildBinaryOp: عملية أصغر من (<)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_LESS_EQUAL:
            // (AR) أصغر أو يساوي: LE (sir_types.h:128)
            opcode = SIROpcode::LE;
            isComparison = true;
            std::cout << "[DEBUG] buildBinaryOp: عملية أصغر أو يساوي (<=)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_GREATER:
            // (AR) أكبر من: GT (sir_types.h:129)
            opcode = SIROpcode::GT;
            isComparison = true;
            std::cout << "[DEBUG] buildBinaryOp: عملية أكبر من (>)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_GREATER_EQUAL:
            // (AR) أكبر أو يساوي: GE (sir_types.h:130)
            opcode = SIROpcode::GE;
            isComparison = true;
            std::cout << "[DEBUG] buildBinaryOp: عملية أكبر أو يساوي (>=)" << std::endl;
            break;
        
        // ========== العمليات المنطقية (token.h:227-228) ==========
        case Lexer::TokenType::OP_AND:
            // (AR) AND المنطقي: AND (sir_types.h:118)
            opcode = SIROpcode::AND;
            isComparison = true;  // (AR) النتيجة منطقية
            std::cout << "[DEBUG] buildBinaryOp: عملية AND (&&)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_OR:
            // (AR) OR المنطقي: OR (sir_types.h:119)
            opcode = SIROpcode::OR;
            isComparison = true;  // (AR) النتيجة منطقية
            std::cout << "[DEBUG] buildBinaryOp: عملية OR (||)" << std::endl;
            break;
            
        default:
            // (AR) عملية غير مدعومة
            std::cout << "[DEBUG] buildBinaryOp: عملية غير مدعومة: " 
                      << static_cast<int>(binOp->op) << std::endl;
            errors_.push_back("عملية ثنائية غير مدعومة / Unsupported binary operation");
            return BuildResult(resultReg, resultType);
    }
    
    // (AR) نوع النتيجة للمقارنات هو BOOL
    // (EN) Result type for comparisons is BOOL
    if (isComparison) {
        resultType = SIRType::BOOL;
    }
    
    // (AR) إنشاء معاملات SIR (sir_types.h:306-312 - SIROperand::Register)
    // (EN) Create SIR operands
    SIROperand leftOp, rightOp, resultOp;
    
    // (AR) المعامل الأيسر
    if (leftResult.isConstant) {
        // (AR) قيمة ثابتة - استخدم ConstantI64/ConstantF64/ConstantString
        if (leftResult.type == SIRType::STRING) {
            leftOp = SIROperand::ConstantString(leftResult.constantValue);
        } else if (leftResult.type == SIRType::F64) {
            leftOp = SIROperand::ConstantF64(std::stod(leftResult.constantValue));
        } else {
            leftOp = SIROperand::ConstantI64(std::stoll(leftResult.constantValue));
        }
    } else {
        // (AR) سجل - استخدم Register
        leftOp = SIROperand::Register(leftResult.registerName, leftResult.type);
    }
    
    // (AR) المعامل الأيمن
    if (rightResult.isConstant) {
        if (rightResult.type == SIRType::STRING) {
            rightOp = SIROperand::ConstantString(rightResult.constantValue);
        } else if (rightResult.type == SIRType::F64) {
            rightOp = SIROperand::ConstantF64(std::stod(rightResult.constantValue));
        } else {
            rightOp = SIROperand::ConstantI64(std::stoll(rightResult.constantValue));
        }
    } else {
        rightOp = SIROperand::Register(rightResult.registerName, rightResult.type);
    }
    
    // (AR) سجل النتيجة
    resultOp = SIROperand::Register(resultReg, resultType);
    
    // (AR) إنشاء تعليمة SIR (sir_instruction.h:100-107 - SIRInstruction::Binary)
    // (EN) Create SIR instruction
    SIRInstruction inst = SIRInstruction::Binary(opcode, resultOp, leftOp, rightOp);
    
    // (AR) إضافة التعليمة للكتلة الحالية
    // (EN) Add instruction to current block
    if (currentBlock_) {
        currentBlock_->addInstruction(inst);
        std::cout << "[DEBUG] buildBinaryOp: تمت إضافة التعليمة للكتلة الحالية" << std::endl;
    } else {
        std::cout << "[DEBUG] buildBinaryOp: تحذير - لا توجد كتلة حالية!" << std::endl;
    }
    
    std::cout << "[DEBUG] buildBinaryOp: النتيجة في سجل " << resultReg 
              << " بنوع " << static_cast<int>(resultType) << std::endl;
    
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
// UnaryExpr Members (expressions.h:78-81):
// - op: Lexer::TokenType (line 80)
// - operand: ExprPtr (line 81)
//
// TokenType للعمليات الأحادية (token.h):
// - OP_MINUS (206): السالب
// - OP_NOT (229): النفي المنطقي
//
// SIROpcode (sir_types.h):
// - NEG (115): السالب
// - NOT (121): النفي
//
// SIRInstruction::Unary (sir_instruction.h:114-120):
// - الاستخدام: SIRInstruction::Unary(opcode, result, operand)
// ============================================================================
BuildResult SIRBuilder::buildUnaryOp(AST::UnaryOpNode* unaryOp) {
    if (!unaryOp) {
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildUnaryOp: بدء بناء عملية أحادية" << std::endl;
    
    // (AR) بناء المعامل (expressions.h:81 - operand: ExprPtr)
    // (EN) Build operand
    auto operandResult = buildExpression(unaryOp->operand.get());
    std::cout << "[DEBUG] buildUnaryOp: operandResult.registerName='" << operandResult.registerName 
              << "', type=" << static_cast<int>(operandResult.type) << std::endl;
    
    // (AR) إنشاء سجل للنتيجة (sir_builder.h:511 - newTempRegister)
    // (EN) Create result register
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد نوع النتيجة
    // (EN) Determine result type
    SIRType resultType = operandResult.type;
    
    // (AR) تحديد رمز العملية (SIROpcode) بناءً على TokenType
    // (EN) Determine SIROpcode based on TokenType
    SIROpcode opcode;
    
    // (AR) العملية من expressions.h:80 - op: Lexer::TokenType
    switch (unaryOp->op) {
        case Lexer::TokenType::OP_MINUS:
            // (AR) السالب: NEG (sir_types.h:115)
            opcode = SIROpcode::NEG;
            std::cout << "[DEBUG] buildUnaryOp: عملية السالب (-)" << std::endl;
            break;
            
        case Lexer::TokenType::OP_NOT:
            // (AR) النفي المنطقي: NOT (sir_types.h:121)
            opcode = SIROpcode::NOT;
            resultType = SIRType::BOOL;  // (AR) النفي يُرجع قيمة منطقية
            std::cout << "[DEBUG] buildUnaryOp: عملية النفي (!)" << std::endl;
            break;
            
        default:
            // (AR) عملية غير مدعومة
            std::cout << "[DEBUG] buildUnaryOp: عملية غير مدعومة: " 
                      << static_cast<int>(unaryOp->op) << std::endl;
            errors_.push_back("عملية أحادية غير مدعومة / Unsupported unary operation");
            return BuildResult(resultReg, resultType);
    }
    
    // (AR) إنشاء معاملات SIR
    // (EN) Create SIR operands
    SIROperand operandOp, resultOp;
    
    // (AR) المعامل
    if (operandResult.isConstant) {
        if (operandResult.type == SIRType::F64) {
            operandOp = SIROperand::ConstantF64(std::stod(operandResult.constantValue));
        } else if (operandResult.type == SIRType::BOOL) {
            operandOp = SIROperand::ConstantBool(operandResult.constantValue == "true" || operandResult.constantValue == "صحيح");
        } else {
            operandOp = SIROperand::ConstantI64(std::stoll(operandResult.constantValue));
        }
    } else {
        operandOp = SIROperand::Register(operandResult.registerName, operandResult.type);
    }
    
    // (AR) سجل النتيجة
    resultOp = SIROperand::Register(resultReg, resultType);
    
    // (AR) إنشاء تعليمة SIR (sir_instruction.h:114-120 - SIRInstruction::Unary)
    // (EN) Create SIR instruction
    SIRInstruction inst = SIRInstruction::Unary(opcode, resultOp, operandOp);
    
    // (AR) إضافة التعليمة للكتلة الحالية
    // (EN) Add instruction to current block
    if (currentBlock_) {
        currentBlock_->addInstruction(inst);
        std::cout << "[DEBUG] buildUnaryOp: تمت إضافة التعليمة للكتلة الحالية" << std::endl;
    } else {
        std::cout << "[DEBUG] buildUnaryOp: تحذير - لا توجد كتلة حالية!" << std::endl;
    }
    
    std::cout << "[DEBUG] buildUnaryOp: النتيجة في سجل " << resultReg << std::endl;
    
    return BuildResult(resultReg, resultType);
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
//
// الدوال المضمنة / Builtin Functions (sir_types.h:178-187):
// - طول() / length: STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
// - اطبع() / print: BUILTIN_PRINT
// - إلى_رقم() / to_int: STRING_TO_I64
// - إلى_نص() / to_string: I64_TO_STRING / F64_TO_STRING
// ============================================================================
BuildResult SIRBuilder::buildFunctionCall(AST::FunctionCallNode* call) {
    if (!call) {
        return BuildResult();
    }
    
    // ========================================================================
    // (AR) الخطوة 1: استخراج اسم الدالة من callee
    // (EN) Step 1: Extract function name from callee
    // المصدر: expressions.h:290 - callee: ExprPtr
    // ========================================================================
    std::string funcName;
    
    // (AR) callee يمكن أن يكون VariableExpr (اسم دالة مباشر)
    // (EN) callee can be VariableExpr (direct function name)
    // المصدر: expressions.h:210-212 - VariableExpr::name
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(call->callee.get())) {
        funcName = varExpr->name;
        std::cout << "[DEBUG] buildFunctionCall: function name = '" << funcName << "'" << std::endl;
    } else {
        // (AR) لا ندعم استدعاءات غير مباشرة حالياً
        // (EN) We don't support indirect calls currently
        errors_.push_back("Error: Only direct function calls are supported");
        return BuildResult();
    }
    
    // ========================================================================
    // (AR) الخطوة 2: بناء المعاملات (arguments)
    // (EN) Step 2: Build arguments
    // المصدر: expressions.h:291 - arguments: ExprList = vector<ExprPtr>
    // ========================================================================
    std::vector<SIROperand> argOperands;
    std::vector<BuildResult> argResults;
    
    for (const auto& arg : call->arguments) {
        // (AR) بناء كل معامل باستخدام buildExpression (sir_builder.h:440)
        // (EN) Build each argument using buildExpression
        BuildResult argResult = buildExpression(arg.get());
        
        if (argResult.registerName.empty()) {
            std::cout << "[DEBUG] buildFunctionCall: failed to build argument" << std::endl;
            errors_.push_back("Error: Failed to build function argument");
            return BuildResult();
        }
        
        argResults.push_back(argResult);
        
        // (AR) إنشاء SIROperand للمعامل (sir_types.h:355-362 - Register())
        // (EN) Create SIROperand for argument
        SIROperand argOp = SIROperand::Register(argResult.registerName, argResult.type);
        argOperands.push_back(argOp);
        
        std::cout << "[DEBUG] buildFunctionCall: added argument reg='" 
                  << argResult.registerName << "', type=" << static_cast<int>(argResult.type) << std::endl;
    }
    
    // ========================================================================
    // (AR) الخطوة 2.5: التحقق من الدوال المضمنة
    // (EN) Step 2.5: Check for builtin functions
    // ========================================================================
    
    // (AR) دالة طول() - STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
    // (EN) length() function - STRING_LEN for strings, ARRAY_LEN for arrays
    if (funcName == "طول" || funcName == "length") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: طول() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        // (AR) تحديد نوع العملية بناءً على نوع المعامل
        // (EN) Determine operation based on argument type
        SIROpcode opcode = (argResults[0].type == SIRType::STRING) 
            ? SIROpcode::STRING_LEN    // (sir_types.h:181)
            : SIROpcode::ARRAY_LEN;    // (sir_types.h:176)
        
        SIRInstruction lenInst(opcode);
        lenInst.result = resultOp;
        lenInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(lenInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin طول() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة لرقم() - STRING_TO_I64
    // (EN) to_int() function - STRING_TO_I64
    // الأسماء المدعومة: لرقم, to_int, int, إلى_رقم
    if (funcName == "لرقم" || funcName == "to_int" || funcName == "int" || funcName == "إلى_رقم") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: لرقم() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction convInst(SIROpcode::STRING_TO_I64);  // (sir_types.h:187)
        convInst.result = resultOp;
        convInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(convInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin لرقم() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة لعشري() - STRING_TO_F64
    // (EN) to_float() function
    // الأسماء المدعومة: لعشري, to_float, float
    if (funcName == "لعشري" || funcName == "to_float" || funcName == "float") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: لعشري() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction convInst(SIROpcode::F64_TO_I64);  // TODO: Add STRING_TO_F64 opcode
        convInst.result = resultOp;
        convInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(convInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin لعشري() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة لنص() - I64_TO_STRING أو F64_TO_STRING
    // (EN) to_string() function
    // الأسماء المدعومة: لنص, نص, to_string, str, string, إلى_نص
    if (funcName == "لنص" || funcName == "نص" || funcName == "to_string" || 
        funcName == "str" || funcName == "string" || funcName == "إلى_نص") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: لنص() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        
        SIROpcode opcode = (argResults[0].type == SIRType::F64) 
            ? SIROpcode::F64_TO_STRING  // (sir_types.h:210)
            : SIROpcode::I64_TO_STRING; // (sir_types.h:209)
        
        SIRInstruction convInst(opcode);
        convInst.result = resultOp;
        convInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(convInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin لنص() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // (AR) دالة اطبع() - BUILTIN_PRINT
    // (EN) print() function
    // الأسماء المدعومة: اطبع, print, إطبع
    if (funcName == "اطبع" || funcName == "print" || funcName == "إطبع") {
        SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);  // (sir_types.h:221)
        printInst.operands = argOperands;
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(printInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin اطبع()" << std::endl;
        return BuildResult("", SIRType::VOID);  // (AR) لا قيمة إرجاع
    }
    
    // (AR) دالة اقرأ() - BUILTIN_READ
    // (EN) input() function
    // الأسماء المدعومة: اقرأ, input
    if (funcName == "اقرأ" || funcName == "input") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        
        SIRInstruction readInst(SIROpcode::BUILTIN_READ);  // (sir_types.h:222)
        readInst.result = resultOp;
        readInst.operands = argOperands;
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(readInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin اقرأ() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // ========================================================================
    // (AR) الخطوة 3: البحث عن الدالة والحصول على نوع الإرجاع
    // (EN) Step 3: Look up function and get return type
    // المصدر: sir_builder.h:719 - functionTable_
    // المصدر: sir_builder.h:162-175 - FunctionInfo struct
    // ========================================================================
    SIRType returnType = SIRType::VOID;  // (AR) افتراضياً void
    
    auto it = functionTable_.find(funcName);
    if (it != functionTable_.end()) {
        // (AR) الدالة موجودة - استخدم نوع الإرجاع (sir_builder.h:165)
        // (EN) Function found - use return type
        returnType = it->second.returnType;
        std::cout << "[DEBUG] buildFunctionCall: function found, returnType=" 
                  << static_cast<int>(returnType) << std::endl;
    } else {
        // (AR) الدالة غير موجودة - تحقق إذا كانت قالباً
        // (EN) Function not found - check if it's a template
        if (templateFunctions_.find(funcName) != templateFunctions_.end()) {
            std::cout << "[Template] Found template function: " << funcName << std::endl;
            
            // (AR) استنتاج الأنواع من المعاملات
            // (EN) Infer types from arguments
            std::vector<SIRType> inferredTypes;
            for (const auto& argResult : argResults) {
                inferredTypes.push_back(argResult.type);
            }
            
            // (AR) إنشاء نسخة من القالب مع الأنواع المستنتجة
            // (EN) Instantiate template with inferred types
            std::string instanceName = instantiateTemplate(funcName, inferredTypes);
            
            if (instanceName.empty()) {
                std::cerr << "[Template Error] Failed to instantiate template: " << funcName << std::endl;
                errors_.push_back("Error: Failed to instantiate template '" + funcName + "'");
                return BuildResult();
            }
            
            // (AR) استخدم اسم النسخة المُنشأة
            // (EN) Use the instantiated function name
            funcName = instanceName;
            
            // (AR) ابحث عن نوع الإرجاع للنسخة المُنشأة
            // (EN) Look up return type for instantiated function
            auto instIt = functionTable_.find(funcName);
            if (instIt != functionTable_.end()) {
                returnType = instIt->second.returnType;
            } else {
                std::cerr << "[Template Error] Instantiated function not in function table: " << funcName << std::endl;
            }
            
            std::cout << "[Template] Using instantiated function: " << funcName << std::endl;
        } else {
            // (AR) قد تكون دالة مدمجة غير مسجلة بعد
            // (EN) May be a builtin function not registered yet
            std::cout << "[DEBUG] buildFunctionCall: function '" << funcName 
                      << "' not found (no template), assuming VOID return" << std::endl;
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 4: إنشاء سجل النتيجة وتعليمة الاستدعاء
    // (EN) Step 4: Create result register and call instruction
    // المصدر: sir_instruction.h:231-238 - SIRInstruction::Call()
    // المصدر: sir_types.h:390-395 - SIROperand::Function()
    // ========================================================================
    std::string resultReg = newTempRegister();
    
    // (AR) إنشاء معامل الدالة (sir_types.h:390)
    // (EN) Create function operand
    SIROperand funcOp = SIROperand::Function(funcName);
    
    // (AR) إنشاء معامل النتيجة (sir_types.h:355)
    // (EN) Create result operand
    SIROperand resultOp = SIROperand::Register(resultReg, returnType);
    
    // (AR) إنشاء تعليمة الاستدعاء (sir_instruction.h:231)
    // (EN) Create call instruction
    // SIRInstruction::Call(result, function, args)
    SIRInstruction callInst = SIRInstruction::Call(resultOp, funcOp, argOperands);
    
    // ========================================================================
    // (AR) الخطوة 5: إضافة التعليمة إلى الكتلة الحالية
    // (EN) Step 5: Add instruction to current block
    // المصدر: sir_builder.h:712 - currentBlock_
    // ========================================================================
    if (currentBlock_) {
        currentBlock_->instructions.push_back(callInst);
        std::cout << "[DEBUG] buildFunctionCall: added CALL instruction to block" << std::endl;
    } else {
        std::cout << "[DEBUG] buildFunctionCall: WARNING - no current block!" << std::endl;
    }
    
    std::cout << "[DEBUG] buildFunctionCall: returning result reg='" << resultReg 
              << "', type=" << static_cast<int>(returnType) << std::endl;
    
    return BuildResult(resultReg, returnType);
}

// ============================================================================
// buildNewObject - بناء تعبير إنشاء كائن جديد
// ============================================================================
// مصدر التعريف / Source: class_nodes.h:164
// التوقيع / Signature: BuildResult buildNewObject(AST::NewExpr* newExpr);
//
// المعاملات / Parameters:
// - newExpr: AST::NewExpr* (class_nodes.h:164)
//
// NewExpr Members:
// - className: std::string (line 167)
// - arguments: std::vector<std::unique_ptr<Expr>> (line 168)
// ============================================================================
BuildResult SIRBuilder::buildNewObject(AST::NewExpr* newExpr) {
    if (!newExpr) {
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildNewObject: creating object of class '" 
              << newExpr->className << "'" << std::endl;
    
    // (AR) الخطوة 1: البحث عن الصنف في الوحدة
    // (EN) Step 1: Find class in module
    auto sirClass = module_->getClass(newExpr->className);
    if (!sirClass) {
        errors_.push_back("Class not found: " + newExpr->className);
        return BuildResult();
    }
    
    // (AR) الخطوة 2: تخصيص ذاكرة للكائن
    // (EN) Step 2: Allocate memory for object
    std::string objReg = newTempRegister();
    
    if (currentBlock_) {
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(objReg, SIRType::PTR);
        // (AR) إضافة اسم الصنف كـ metadata
        // (EN) Add class name as metadata
        allocInst.operands.push_back(SIROperand::ConstantString(newExpr->className));
        currentBlock_->addInstruction(allocInst);
    }
    
    // (AR) الخطوة 3: استدعاء دالة البناء (constructor) إن وجدت
    // (EN) Step 3: Call constructor if exists
    std::string constructorName = newExpr->className + ".بناء";
    auto constructor = sirClass->getMethod("بناء");
    
    if (constructor || !newExpr->arguments.empty()) {
        // (AR) بناء معاملات البناء
        // (EN) Build constructor arguments
        std::vector<SIROperand> args;
        
        // (AR) المعامل الأول هو الكائن نفسه (self)
        // (EN) First argument is the object itself (self)
        args.push_back(SIROperand::Register(objReg, SIRType::PTR));
        
        // (AR) باقي المعاملات
        // (EN) Rest of arguments
        for (const auto& arg : newExpr->arguments) {
            auto argResult = buildExpression(arg.get());
            if (argResult.isConstant && !argResult.constantValue.empty()) {
                // تحويل الثابت حسب النوع
                switch (argResult.type) {
                    case SIRType::I64:
                        args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                        break;
                    case SIRType::F64:
                        args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                        break;
                    case SIRType::STRING:
                        args.push_back(SIROperand::ConstantString(argResult.constantValue));
                        break;
                    default:
                        args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        break;
                }
            } else {
                args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
            }
        }
        
        // (AR) إنشاء تعليمة استدعاء البناء
        // (EN) Create constructor call instruction
        if (currentBlock_) {
            SIRInstruction callInst;
            callInst.opcode = SIROpcode::CALL;
            callInst.result = SIROperand::Register(newTempRegister(), SIRType::VOID);
            callInst.operands.push_back(SIROperand::Function(constructorName));
            for (const auto& arg : args) {
                callInst.operands.push_back(arg);
            }
            currentBlock_->addInstruction(callInst);
        }
    }
    
    std::cout << "[DEBUG] buildNewObject: object created in register '" << objReg << "'" << std::endl;
    
    // (AR) إرجاع مؤشر للكائن
    // (EN) Return pointer to object
    return BuildResult(objReg, SIRType::PTR);
}

// ============================================================================
// buildMemberAccess - بناء تعبير الوصول لعضو في كائن
// ============================================================================
// مصدر التعريف / Source: class_nodes.h:206
// التوقيع / Signature: BuildResult buildMemberAccess(AST::MemberAccessExpr* memberExpr);
//
// المعاملات / Parameters:
// - memberExpr: AST::MemberAccessExpr* (class_nodes.h:206)
//
// MemberAccessExpr Members:
// - object: std::unique_ptr<Expr> (line 209)
// - memberName: std::string (line 210)
// ============================================================================
BuildResult SIRBuilder::buildMemberAccess(AST::MemberAccessExpr* memberExpr) {
    if (!memberExpr) {
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildMemberAccess: accessing member '" 
              << memberExpr->memberName << "'" << std::endl;
    
    // (AR) الخطوة 1: بناء تعبير الكائن
    // (EN) Step 1: Build object expression
    auto objResult = buildExpression(memberExpr->object.get());
    
    // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو
    // (EN) Step 2: Create member access instruction
    std::string resultReg = newTempRegister();
    
    if (currentBlock_) {
        SIRInstruction loadInst;
        loadInst.opcode = SIROpcode::LOAD;
        loadInst.result = SIROperand::Register(resultReg, SIRType::I64); // نوع افتراضي، سيتم تحديده لاحقاً
        
        // (AR) المعامل الأول: الكائن
        // (EN) First operand: object
        loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        
        // (AR) المعامل الثاني: اسم العضو (كـ offset أو اسم)
        // (EN) Second operand: member name (as offset or name)
        loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->memberName));
        
        currentBlock_->addInstruction(loadInst);
    }
    
    std::cout << "[DEBUG] buildMemberAccess: result in register '" << resultReg << "'" << std::endl;
    
    return BuildResult(resultReg, SIRType::I64);  // نوع افتراضي
}

// ============================================================================
// بناء دالة قالب / Build Template Function
// ============================================================================
// Purpose (AR): حفظ دالة قالب للإنشاء لاحقاً عند الاستدعاء مع أنواع محددة
// Purpose (EN): Store template function for later instantiation with specific types
//
// التعريف / Source: declarations.h:470 (TemplateFunctionDecl)
// التوقيع / Signature: void buildTemplateFunction(AST::TemplateFunctionDecl*)
//
// TemplateFunctionDecl Members:
// - typeParameters: std::vector<TypeParameter> (line 472)
// - name: std::string (line 473)
// - parameters: std::vector<Parameter> (line 474)
// - returnType: Data::DataType (line 475)
// - body: StmtPtr (line 476)
// ============================================================================
void SIRBuilder::buildTemplateFunction(AST::TemplateFunctionDecl* templateDecl) {
    if (!templateDecl) {
        return;
    }
    
    std::cout << "[Template] Registering template function: " << templateDecl->name << std::endl;
    std::cout << "[Template] Type parameters: ";
    for (const auto& typeParam : templateDecl->typeParameters) {
        std::cout << typeParam.name << " ";
    }
    std::cout << std::endl;
    
    // (AR) حفظ دالة القالب في الجدول
    // (EN) Store template function in table
    templateFunctions_[templateDecl->name] = templateDecl;
    
    std::cout << "[Template] Template function '" << templateDecl->name 
              << "' stored for later instantiation" << std::endl;
}

// ============================================================================
// إنشاء نسخة محددة من دالة قالب / Instantiate Template Function
// ============================================================================
// Purpose (AR): إنشاء نسخة محددة من دالة القالب باستبدال معاملات الأنواع
// Purpose (EN): Create concrete instance of template function with type substitution
//
// Example:
// - Template: قالب<نوع ت> دالة أكبر(ت أ، ت ب) ت
// - Call: أكبر<رقم>(5, 10)
// - Instantiation: أكبر_i64(رقم أ، رقم ب) رقم
// ============================================================================
std::string SIRBuilder::instantiateTemplate(const std::string& templateName,
                                           const std::vector<SIRType>& typeArguments) {
    std::cout << "[Template] Instantiating template: " << templateName << std::endl;
    
    // (AR) البحث عن دالة القالب
    // (EN) Find template function
    auto it = templateFunctions_.find(templateName);
    if (it == templateFunctions_.end()) {
        std::cerr << "[Template Error] Template function '" << templateName << "' not found" << std::endl;
        return "";
    }
    
    Sad::AST::TemplateFunctionDecl* templateDecl = it->second;
    
    // (AR) التحقق من عدد معاملات الأنواع
    // (EN) Check type argument count
    if (typeArguments.size() != templateDecl->typeParameters.size()) {
        std::cerr << "[Template Error] Template '" << templateName << "' expects " 
                  << templateDecl->typeParameters.size() << " type arguments, got " 
                  << typeArguments.size() << std::endl;
        return "";
    }
    
    // (AR) إنشاء اسم الدالة المُنشأة: اسم_القالب_نوع1_نوع2
    // (EN) Create instantiated function name: template_name_type1_type2
    std::string instantiatedName = templateName;
    for (const auto& type : typeArguments) {
        instantiatedName += "_";
        switch (type) {
            case SIRType::I64: instantiatedName += "i64"; break;
            case SIRType::F64: instantiatedName += "f64"; break;
            case SIRType::STRING: instantiatedName += "str"; break;
            case SIRType::BOOL: instantiatedName += "bool"; break;
            default: instantiatedName += "obj"; break;
        }
    }
    
    std::cout << "[Template] Instantiated name: " << instantiatedName << std::endl;
    
    // (AR) التحقق من وجود النسخة بالفعل
    // (EN) Check if already instantiated
    if (instantiatedTemplates_.find(instantiatedName) != instantiatedTemplates_.end()) {
        std::cout << "[Template] Already instantiated, reusing" << std::endl;
        return instantiatedName;
    }
    
    // (AR) إنشاء جدول استبدال الأنواع
    // (EN) Create type substitution map
    std::unordered_map<std::string, SIRType> typeSubstitutions;
    for (size_t i = 0; i < templateDecl->typeParameters.size(); ++i) {
        typeSubstitutions[templateDecl->typeParameters[i].name] = typeArguments[i];
        std::cout << "[Template] Substitution: " << templateDecl->typeParameters[i].name 
                  << " -> " << static_cast<int>(typeArguments[i]) << std::endl;
    }
    
    // (AR) استبدال نوع الإرجاع
    // (EN) Substitute return type
    SIRType returnType = astTypeToSIRType(templateDecl->returnType);
    
    // (AR) إذا كان نوع الإرجاع هو معامل قالب، نستبدله
    // (EN) If return type is template parameter, substitute it
    if (templateDecl->returnType == Data::DataType::OBJECT) {
        // (AR) قد يكون معامل قالب - نحتاج للتحقق من الاسم
        // (EN) Might be template parameter - need to check name
        // TODO: تحسين هذا الجزء لمعرفة ما إذا كان نوع الإرجاع هو معامل قالب
    }
    
    // (AR) إنشاء دالة SIR جديدة
    // (EN) Create new SIR function
    auto sirFunction = std::make_shared<SIRFunction>(instantiatedName, returnType);
    
    // (AR) استبدال أنواع المعاملات
    // (EN) Substitute parameter types
    for (const auto& param : templateDecl->parameters) {
        SIRType paramType = astTypeToSIRType(param.type);
        
        // (AR) إذا كان نوع المعامل هو OBJECT، قد يكون معامل قالب
        // (EN) If parameter type is OBJECT, might be template parameter
        if (param.type == Data::DataType::OBJECT) {
            // (AR) نحاول إيجاد استبدال لهذا النوع
            // (EN) Try to find substitution for this type
            // TODO: نحتاج معلومات إضافية من Parser لربط الأنواع بأسماء المعاملات
            // هنا نفترض أن المعامل الأول له نفس ترتيب معامل النوع الأول
            if (!typeArguments.empty()) {
                paramType = typeArguments[0]; // استبدال مؤقت بسيط
            }
        }
        
        sirFunction->addParameter(Sad::Compiler::SIR::SIRParameter(param.name, paramType));
    }
    
    // (AR) بناء جسم الدالة مع استبدال الأنواع
    // (EN) Build function body with type substitution
    std::shared_ptr<SIRFunction> previousFunction = currentFunction_;
    std::shared_ptr<SIRBasicBlock> previousBlock = currentBlock_;
    
    currentFunction_ = sirFunction;
    
    // (AR) إنشاء نطاق جديد
    // (EN) Create new scope
    enterScope();
    
    // (AR) إضافة استبدالات الأنواع للنطاق الحالي (للاستخدام عند بناء الجسم)
    // (EN) Add type substitutions to current scope
    GenericScope genericScope;
    
    // (AR) تحويل typeParameters من AST إلى SIR
    // (EN) Convert typeParameters from AST to SIR
    for (const auto& astParam : templateDecl->typeParameters) {
        TypeParameter sirParam(astParam.name);
        genericScope.typeParameters.push_back(sirParam);
    }
    
    genericScope.typeSubstitutions = typeSubstitutions;
    genericScopeStack_.push_back(genericScope);
    
    // (AR) إنشاء basic block للدخول
    // (EN) Create entry basic block
    auto entryBlock = createBasicBlock("entry");
    sirFunction->addBasicBlock(entryBlock);
    currentBlock_ = entryBlock;
    
    // (AR) بناء جسم الدالة
    // (EN) Build function body
    if (templateDecl->body) {
        buildStatement(templateDecl->body.get());
    }
    
    // (AR) إزالة نطاق الأنواع العامة
    // (EN) Remove generic scope
    genericScopeStack_.pop_back();
    
    // (AR) الخروج من النطاق
    // (EN) Exit scope
    exitScope();
    
    // (AR) التأكد من وجود terminator
    // (EN) Ensure terminator exists
    if (currentBlock_ && !currentBlock_->instructions.empty()) {
        const auto& lastInst = currentBlock_->instructions.back();
        if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID) {
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
    }
    
    // (AR) استعادة السياق
    // (EN) Restore context
    currentFunction_ = previousFunction;
    currentBlock_ = previousBlock;
    
    // (AR) حفظ الدالة المُنشأة
    // (EN) Store instantiated function
    instantiatedTemplates_[instantiatedName] = sirFunction;
    module_->addFunction(sirFunction);
    
    // (AR) إضافة الدالة المُنشأة إلى جدول الدوال
    // (EN) Add instantiated function to function table
    FunctionInfo funcInfo;
    funcInfo.name = instantiatedName;
    funcInfo.returnType = returnType;
    funcInfo.parameters = sirFunction->getParameters();
    funcInfo.sirFunction = sirFunction;
    functionTable_[instantiatedName] = funcInfo;
    
    std::cout << "[Template] Instantiation complete: " << instantiatedName << std::endl;
    
    return instantiatedName;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
