/*
 * ملف تنفيذ SIR Builder - SIR Builder Implementation File
 * 
 * الوصف: تنفيذ محول AST إلى SIR
 * Description: Implementation of AST to SIR converter
 *
 * يحتوي على:
 * Contains:
 * - SIRBuilder: المحول الرئيسي / Main converter
 * - بناء التصريحات / Declaration building
 * - بناء الجمل / Statement building
 * - بناء التعابير / Expression building
 * - إدارة السجلات والنطاقات / Register and scope management
 */

#include "sir_builder.h"
#include <sstream>
#include <stdexcept>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// المنشئ / Constructor
// ============================================================================

SIRBuilder::SIRBuilder() 
    : module_(nullptr)
    , currentFunction_(nullptr)
    , currentBlock_(nullptr)
    , nextTempRegister_(0)
    , nextLabel_(0)
    , currentScopeLevel_(0)
{
    // (AR) تهيئة النطاق العام
    // (EN) Initialize global scope
    scopeStack_.push_back(std::unordered_map<std::string, VariableInfo>());
}

// ============================================================================
// الدوال الرئيسية / Main Functions
// ============================================================================

std::shared_ptr<SIRModule> SIRBuilder::buildModule(AST::ProgramNode* program) {
    if (!program) {
        reportError("Program node is null");
        return nullptr;
    }
    
    // (AR) إنشاء وحدة جديدة
    // (EN) Create new module
    module_ = std::make_shared<SIRModule>("main");
    
    // (AR) معالجة جميع التصريحات في البرنامج (دوال، متغيرات عامة، أصناف)
    // (EN) Process all declarations in program (functions, global variables, classes)
    // Note: program is StmtList* (vector<StmtPtr>*) - iterate directly
    // ملاحظة: program هو StmtList* (vector<StmtPtr>*) - نتكرر مباشرة
    for (auto& stmt : *program) {
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
    
    // (AR) التحقق من وجود أخطاء
    // (EN) Check for errors
    if (hasErrors()) {
        return nullptr;
    }
    
    // (AR) التحقق من صحة الوحدة
    // (EN) Validate module
    if (!module_->validate()) {
        reportError("Module validation failed");
        return nullptr;
    }
    
    return module_;
}

// ============================================================================
// بناء التصريحات / Building Declarations
// ============================================================================

void SIRBuilder::buildFunction(AST::FunctionDeclNode* funcDecl) {
    if (!funcDecl) {
        reportError("Function declaration is null");
        return;
    }
    
    // (AR) تحويل نوع الإرجاع
    // (EN) Convert return type
    SIRType returnType = astTypeToSIRType(funcDecl->returnType);
    
    // (AR) إنشاء دالة SIR
    // (EN) Create SIR function
    currentFunction_ = std::make_shared<SIRFunction>(funcDecl->name, returnType);
    
    // (AR) إضافة المعاملات
    // (EN) Add parameters
    for (auto& param : funcDecl->parameters) {
        SIRType paramType = astTypeToSIRType(param.type);
        SIRParameter sirParam(param.name, paramType);
        currentFunction_->addParameter(sirParam);
    }
    
    // (AR) إنشاء كتلة البداية
    // (EN) Create entry block
    auto entryBlock = createBasicBlock("entry");
    currentFunction_->addBasicBlock(entryBlock);
    setCurrentBlock(entryBlock);
    
    // (AR) فتح نطاق جديد للدالة
    // (EN) Enter new scope for function
    enterScope();
    
    // (AR) إضافة المعاملات كمتغيرات محلية
    // (EN) Add parameters as local variables
    for (const auto& param : currentFunction_->getParameters()) {
        VariableInfo varInfo;
        varInfo.name = param.name;
        varInfo.type = param.type;
        varInfo.registerName = "%" + param.name;
        varInfo.isGlobal = false;
        varInfo.isMutable = true;
        varInfo.scopeLevel = currentScopeLevel_;
        addVariable(varInfo);
    }
    
    // (AR) بناء جسم الدالة
    // (EN) Build function body
    if (funcDecl->body) {
        for (auto& stmt : funcDecl->body->statements) {
            buildStatement(stmt.get());
        }
    }
    
    // (AR) إضافة return افتراضي إذا لم يكن موجوداً
    // (EN) Add default return if not present
    if (getCurrentBlock() && getCurrentBlock()->getTerminator() == nullptr) {
        if (returnType == SIRType::VOID) {
            SIRInstruction retInst(SIROpcode::RET);
            addInstruction(retInst);
        } else {
            reportError("Function '" + funcDecl->name + "' must return a value");
        }
    }
    
    // (AR) إغلاق النطاق
    // (EN) Exit scope
    exitScope();
    
    // (AR) إضافة الدالة للوحدة
    // (EN) Add function to module
    module_->addFunction(currentFunction_);
    
    // (AR) تسجيل الدالة في الجدول
    // (EN) Register function in table
    FunctionInfo funcInfo;
    funcInfo.name = funcDecl->name;
    funcInfo.returnType = returnType;
    funcInfo.parameters = currentFunction_->getParameters();
    funcInfo.sirFunction = currentFunction_;
    functionTable_[funcDecl->name] = funcInfo;
    
    // (AR) إعادة تعيين الدالة الحالية
    // (EN) Reset current function
    currentFunction_ = nullptr;
    currentBlock_ = nullptr;
}

void SIRBuilder::buildGlobalVariable(AST::VariableDeclNode* varDecl) {
    if (!varDecl) {
        reportError("Variable declaration is null");
        return;
    }
    
    // (AR) تحويل النوع
    // (EN) Convert type
    SIRType varType = astTypeToSIRType(varDecl->type);
    
    // (AR) بناء القيمة الأولية إذا وجدت
    // (EN) Build initial value if present
    std::string initialValue;
    if (varDecl->initializer) {
        auto initResult = buildExpression(varDecl->initializer.get());
        if (initResult.isConstant) {
            initialValue = initResult.constantValue;
        } else {
            reportError("Global variable initializer must be a constant expression");
            return;
        }
    }
    
    // (AR) إنشاء متغير عام
    // (EN) Create global variable
    auto globalVar = std::make_shared<SIRGlobalVariable>(
        varDecl->name,
        varType,
        initialValue,
        varDecl->isConst
    );
    
    // (AR) إضافة للوحدة
    // (EN) Add to module
    module_->addGlobalVariable(globalVar);
    
    // (AR) إضافة للنطاق العام
    // (EN) Add to global scope
    VariableInfo varInfo;
    varInfo.name = varDecl->name;
    varInfo.type = varType;
    varInfo.registerName = "$" + varDecl->name;
    varInfo.isGlobal = true;
    varInfo.isMutable = !varDecl->isConst;
    varInfo.scopeLevel = 0;
    addVariable(varInfo);
}

void SIRBuilder::buildClass(AST::ClassDeclNode* classDecl) {
    if (!classDecl) {
        reportError("Class declaration is null");
        return;
    }
    
    // (AR) إنشاء صنف SIR
    // (EN) Create SIR class
    auto sirClass = std::make_shared<SIRClass>(classDecl->name);
    
    // (AR) معالجة الصنف الأب إن وجد
    // (EN) Process parent class if exists
    if (!classDecl->parentClass.empty()) {
        auto parentIt = classTable_.find(classDecl->parentClass);
        if (parentIt == classTable_.end()) {
            reportError("Parent class '" + classDecl->parentClass + "' not found");
            return;
        }
        sirClass->setParent(classDecl->parentClass);
    }
    
    // (AR) إضافة الحقول
    // (EN) Add fields
    for (auto& field : classDecl->fields) {
        SIRType fieldType = astTypeToSIRType(field->type);
        sirClass->addField(field->name, fieldType);
    }
    
    // (AR) إضافة التوابع
    // (EN) Add methods
    for (auto& method : classDecl->methods) {
        SIRType returnType = astTypeToSIRType(method->returnType);
        auto sirMethod = std::make_shared<SIRFunction>(
            classDecl->name + "::" + method->name, 
            returnType
        );
        
        // (AR) إضافة معامل this
        // (EN) Add this parameter
        SIRType thisType = SIRType::PTR; // Pointer to class instance
        SIRParameter thisParam("this", thisType);
        sirMethod->addParameter(thisParam);
        
        // (AR) إضافة المعاملات الأخرى
        // (EN) Add other parameters
        for (auto& param : method->parameters) {
            SIRType paramType = astTypeToSIRType(param.type);
            SIRParameter sirParam(param.name, paramType);
            sirMethod->addParameter(sirParam);
        }
        
        // (AR) بناء جسم التابع
        // (EN) Build method body
        auto prevFunction = currentFunction_;
        currentFunction_ = sirMethod;
        
        auto entryBlock = createBasicBlock("entry");
        currentFunction_->addBasicBlock(entryBlock);
        setCurrentBlock(entryBlock);
        
        enterScope();
        
        // (AR) إضافة this كمتغير محلي
        // (EN) Add this as local variable
        VariableInfo thisVar;
        thisVar.name = "this";
        thisVar.type = thisType;
        thisVar.registerName = "%this";
        thisVar.isGlobal = false;
        thisVar.isMutable = false;
        thisVar.scopeLevel = currentScopeLevel_;
        addVariable(thisVar);
        
        // (AR) إضافة المعاملات
        // (EN) Add parameters
        for (size_t i = 1; i < sirMethod->getParameters().size(); ++i) {
            const auto& param = sirMethod->getParameters()[i];
            VariableInfo varInfo;
            varInfo.name = param.name;
            varInfo.type = param.type;
            varInfo.registerName = "%" + param.name;
            varInfo.isGlobal = false;
            varInfo.isMutable = true;
            varInfo.scopeLevel = currentScopeLevel_;
            addVariable(varInfo);
        }
        
        // (AR) بناء الجسم
        // (EN) Build body
        if (method->body) {
            for (auto& stmt : method->body->statements) {
                buildStatement(stmt.get());
            }
        }
        
        // (AR) إضافة return افتراضي
        // (EN) Add default return
        if (getCurrentBlock() && getCurrentBlock()->getTerminator() == nullptr) {
            if (returnType == SIRType::VOID) {
                SIRInstruction retInst(SIROpcode::RET);
                addInstruction(retInst);
            }
        }
        
        exitScope();
        
        // (AR) إضافة التابع للوحدة والصنف
        // (EN) Add method to module and class
        module_->addFunction(sirMethod);
        sirClass->addMethod(method->name, sirMethod);
        
        currentFunction_ = prevFunction;
    }
    
    // (AR) إضافة الصنف للوحدة
    // (EN) Add class to module
    module_->addClass(sirClass);
    
    // (AR) تسجيل الصنف في الجدول
    // (EN) Register class in table
    classTable_[classDecl->name] = sirClass;
}

// ============================================================================
// بناء الجمل / Building Statements
// ============================================================================

void SIRBuilder::buildStatement(AST::StatementNode* stmt) {
    if (!stmt) {
        return;
    }
    
    // (AR) توزيع على النوع المناسب
    // (EN) Dispatch to appropriate type
    if (auto ifStmt = dynamic_cast<AST::IfStatementNode*>(stmt)) {
        buildIfStatement(ifStmt);
    }
    else if (auto whileLoop = dynamic_cast<AST::WhileLoopNode*>(stmt)) {
        buildWhileLoop(whileLoop);
    }
    else if (auto forLoop = dynamic_cast<AST::ForLoopNode*>(stmt)) {
        buildForLoop(forLoop);
    }
    else if (auto breakStmt = dynamic_cast<AST::BreakStatementNode*>(stmt)) {
        buildBreakStatement(breakStmt);
    }
    else if (auto continueStmt = dynamic_cast<AST::ContinueStatementNode*>(stmt)) {
        buildContinueStatement(continueStmt);
    }
    else if (auto retStmt = dynamic_cast<AST::ReturnStatementNode*>(stmt)) {
        buildReturnStatement(retStmt);
    }
    else if (auto assignment = dynamic_cast<AST::AssignmentNode*>(stmt)) {
        buildAssignment(assignment);
    }
    else if (auto varDecl = dynamic_cast<AST::VariableDeclNode*>(stmt)) {
        buildLocalVariable(varDecl);
    }
    else if (auto exprStmt = dynamic_cast<AST::ExpressionStatementNode*>(stmt)) {
        // (AR) تنفيذ التعبير وتجاهل النتيجة
        // (EN) Execute expression and ignore result
        buildExpression(exprStmt->expression.get());
    }
}

void SIRBuilder::buildReturnStatement(AST::ReturnStatementNode* retStmt) {
    if (!retStmt || !currentFunction_) {
        return;
    }
    
    SIRInstruction retInst(SIROpcode::RET);
    
    // (AR) معالجة القيمة المُرجعة
    // (EN) Handle return value
    if (retStmt->returnValue) {
        auto result = buildExpression(retStmt->returnValue.get());
        
        // (AR) التحقق من توافق النوع
        // (EN) Check type compatibility
        if (!areTypesCompatible(result.type, currentFunction_->returnType)) {
            result = convertType(result, currentFunction_->returnType);
        }
        
        retInst.addOperand(SIROperand::makeRegister(result.registerName, result.type));
    }
    else {
        // (AR) return بدون قيمة
        // (EN) return without value
        if (currentFunction_->returnType != SIRType::VOID) {
            reportError("Function must return a value");
            return;
        }
    }
    
    addInstruction(retInst);
}

void SIRBuilder::buildAssignment(AST::AssignmentNode* assignment) {
    if (!assignment) {
        return;
    }
    
    // (AR) بناء التعبير الأيمن (القيمة)
    // (EN) Build right expression (value)
    auto valueResult = buildExpression(assignment->value.get());
    
    // (AR) معالجة الهدف
    // (EN) Handle target
    if (auto varTarget = dynamic_cast<AST::VariableNode*>(assignment->target.get())) {
        // (AR) إسناد لمتغير
        // (EN) Assignment to variable
        VariableInfo* varInfo = lookupVariable(varTarget->name);
        if (!varInfo) {
            reportError("Undefined variable: " + varTarget->name);
            return;
        }
        
        // (AR) التحقق من إمكانية التعديل
        // (EN) Check mutability
        if (!varInfo->isMutable) {
            reportError("Cannot assign to immutable variable: " + varTarget->name);
            return;
        }
        
        // (AR) التحقق من توافق النوع
        // (EN) Check type compatibility
        if (!areTypesCompatible(valueResult.type, varInfo->type)) {
            valueResult = convertType(valueResult, varInfo->type);
        }
        
        // (AR) توليد تعليمة STORE
        // (EN) Generate STORE instruction
        SIRInstruction storeInst(SIROpcode::STORE);
        storeInst.addOperand(SIROperand::makeRegister(varInfo->registerName, varInfo->type));
        storeInst.addOperand(SIROperand::makeRegister(valueResult.registerName, valueResult.type));
        addInstruction(storeInst);
    }
    else if (auto arrayAccess = dynamic_cast<AST::ArrayAccessNode*>(assignment->target.get())) {
        // (AR) إسناد لعنصر مصفوفة
        // (EN) Assignment to array element
        auto arrayResult = buildExpression(arrayAccess->array.get());
        auto indexResult = buildExpression(arrayAccess->index.get());
        
        // (AR) التحقق من الأنواع
        // (EN) Check types
        if (arrayResult.type != SIRType::ARRAY) {
            reportError("Cannot index non-array type");
            return;
        }
        if (indexResult.type != SIRType::I64) {
            reportError("Array index must be an integer");
            return;
        }
        
        // (AR) توليد تعليمة ARRAY_SET
        // (EN) Generate ARRAY_SET instruction
        SIRInstruction arraySetInst(SIROpcode::ARRAY_SET);
        arraySetInst.addOperand(SIROperand::makeRegister(arrayResult.registerName, arrayResult.type));
        arraySetInst.addOperand(SIROperand::makeRegister(indexResult.registerName, indexResult.type));
        arraySetInst.addOperand(SIROperand::makeRegister(valueResult.registerName, valueResult.type));
        addInstruction(arraySetInst);
    }
    else if (auto memberAccess = dynamic_cast<AST::MemberAccessNode*>(assignment->target.get())) {
        // (AR) إسناد لعضو كائن
        // (EN) Assignment to object member
        auto objectResult = buildExpression(memberAccess->object.get());
        
        // (AR) التحقق من الأنواع
        // (EN) Check types
        if (objectResult.type != SIRType::STRUCT && objectResult.type != SIRType::PTR) {
            reportError("Cannot access member of non-object type");
            return;
        }
        
        // (AR) توليد تعليمة FIELD_SET
        // (EN) Generate FIELD_SET instruction
        SIRInstruction fieldSetInst(SIROpcode::FIELD_SET);
        fieldSetInst.addOperand(SIROperand::makeRegister(objectResult.registerName, objectResult.type));
        fieldSetInst.addOperand(SIROperand::makeLabel(memberAccess->memberName));
        fieldSetInst.addOperand(SIROperand::makeRegister(valueResult.registerName, valueResult.type));
        addInstruction(fieldSetInst);
    }
    // (AR) إسناد حقول الكائن مُنفّذ بالأعلى
    // (EN) Object field assignment is implemented above
}

void SIRBuilder::buildLocalVariable(AST::VariableDeclNode* varDecl) {
    if (!varDecl) {
        return;
    }
    
    // (AR) تحويل النوع
    // (EN) Convert type
    SIRType varType = astTypeToSIRType(varDecl->type);
    
    // (AR) تخصيص سجل للمتغير
    // (EN) Allocate register for variable
    std::string varReg = "%" + varDecl->name;
    
    // (AR) معالجة القيمة الأولية
    // (EN) Handle initial value
    if (varDecl->initializer) {
        auto initResult = buildExpression(varDecl->initializer.get());
        
        // (AR) التحقق من توافق النوع
        // (EN) Check type compatibility
        if (!areTypesCompatible(initResult.type, varType)) {
            initResult = convertType(initResult, varType);
        }
        
        // (AR) توليد تعليمة MOVE
        // (EN) Generate MOVE instruction
        SIRInstruction moveInst(SIROpcode::MOVE);
        moveInst.setResult(varReg, varType);
        moveInst.addOperand(SIROperand::makeRegister(initResult.registerName, initResult.type));
        addInstruction(moveInst);
    }
    
    // (AR) إضافة المتغير للنطاق الحالي
    // (EN) Add variable to current scope
    VariableInfo varInfo;
    varInfo.name = varDecl->name;
    varInfo.type = varType;
    varInfo.registerName = varReg;
    varInfo.isGlobal = false;
    varInfo.isMutable = !varDecl->isConst;
    varInfo.scopeLevel = currentScopeLevel_;
    addVariable(varInfo);
}

void SIRBuilder::buildIfStatement(AST::IfStatementNode* ifStmt) {
    if (!ifStmt) {
        return;
    }
    
    // (AR) بناء الشرط
    // (EN) Build condition
    auto condResult = buildExpression(ifStmt->condition.get());
    
    // (AR) إنشاء الكتل
    // (EN) Create blocks
    auto thenBlock = createBasicBlock(newLabel("then"));
    auto elseBlock = ifStmt->elseBody ? createBasicBlock(newLabel("else")) : nullptr;
    auto mergeBlock = createBasicBlock(newLabel("merge"));
    
    // (AR) توليد تعليمة BR
    // (EN) Generate BR instruction
    SIRInstruction brInst(SIROpcode::BR);
    brInst.addOperand(SIROperand::makeRegister(condResult.registerName, condResult.type));
    brInst.addOperand(SIROperand::makeLabel(thenBlock->name));
    brInst.addOperand(SIROperand::makeLabel(elseBlock ? elseBlock->name : mergeBlock->name));
    addInstruction(brInst);
    
    // (AR) بناء كتلة then
    // (EN) Build then block
    currentFunction_->addBasicBlock(thenBlock);
    setCurrentBlock(thenBlock);
    enterScope();
    
    if (ifStmt->thenBody) {
        for (auto& stmt : ifStmt->thenBody->statements) {
            buildStatement(stmt.get());
        }
    }
    
    // (AR) إضافة JMP إلى merge إذا لم تكن الكتلة منتهية
    // (EN) Add JMP to merge if block not terminated
    if (getCurrentBlock()->getTerminator() == nullptr) {
        SIRInstruction jmpInst(SIROpcode::JMP);
        jmpInst.addOperand(SIROperand::makeLabel(mergeBlock->name));
        addInstruction(jmpInst);
    }
    
    exitScope();
    
    // (AR) بناء كتلة else إذا وجدت
    // (EN) Build else block if present
    if (elseBlock) {
        currentFunction_->addBasicBlock(elseBlock);
        setCurrentBlock(elseBlock);
        enterScope();
        
        if (ifStmt->elseBody) {
            for (auto& stmt : ifStmt->elseBody->statements) {
                buildStatement(stmt.get());
            }
        }
        
        if (getCurrentBlock()->getTerminator() == nullptr) {
            SIRInstruction jmpInst(SIROpcode::JMP);
            jmpInst.addOperand(SIROperand::makeLabel(mergeBlock->name));
            addInstruction(jmpInst);
        }
        
        exitScope();
    }
    
    // (AR) الانتقال إلى كتلة merge
    // (EN) Move to merge block
    currentFunction_->addBasicBlock(mergeBlock);
    setCurrentBlock(mergeBlock);
}

void SIRBuilder::buildWhileLoop(AST::WhileLoopNode* whileLoop) {
    if (!whileLoop) {
        return;
    }
    
    // (AR) إنشاء الكتل
    // (EN) Create blocks
    auto condBlock = createBasicBlock(newLabel("while_cond"));
    auto bodyBlock = createBasicBlock(newLabel("while_body"));
    auto exitBlock = createBasicBlock(newLabel("while_exit"));
    
    // (AR) القفز إلى كتلة الشرط
    // (EN) Jump to condition block
    SIRInstruction jmpToCondInst(SIROpcode::JMP);
    jmpToCondInst.addOperand(SIROperand::makeLabel(condBlock->name));
    addInstruction(jmpToCondInst);
    
    // (AR) بناء كتلة الشرط
    // (EN) Build condition block
    currentFunction_->addBasicBlock(condBlock);
    setCurrentBlock(condBlock);
    
    auto condResult = buildExpression(whileLoop->condition.get());
    
    SIRInstruction brInst(SIROpcode::BR);
    brInst.addOperand(SIROperand::makeRegister(condResult.registerName, condResult.type));
    brInst.addOperand(SIROperand::makeLabel(bodyBlock->name));
    brInst.addOperand(SIROperand::makeLabel(exitBlock->name));
    addInstruction(brInst);
    
    // (AR) بناء كتلة الجسم
    // (EN) Build body block
    currentFunction_->addBasicBlock(bodyBlock);
    setCurrentBlock(bodyBlock);
    
    // (AR) تسجيل سياق الحلقة
    // (EN) Register loop context
    LoopContext loopCtx;
    loopCtx.continueLabel = condBlock->name;
    loopCtx.breakLabel = exitBlock->name;
    enterLoop(loopCtx);
    enterScope();
    
    if (whileLoop->body) {
        for (auto& stmt : whileLoop->body->statements) {
            buildStatement(stmt.get());
        }
    }
    
    // (AR) العودة إلى كتلة الشرط
    // (EN) Jump back to condition
    if (getCurrentBlock()->getTerminator() == nullptr) {
        SIRInstruction jmpToCondInst2(SIROpcode::JMP);
        jmpToCondInst2.addOperand(SIROperand::makeLabel(condBlock->name));
        addInstruction(jmpToCondInst2);
    }
    
    exitScope();
    exitLoop();
    
    // (AR) الانتقال إلى كتلة الخروج
    // (EN) Move to exit block
    currentFunction_->addBasicBlock(exitBlock);
    setCurrentBlock(exitBlock);
}

void SIRBuilder::buildForLoop(AST::ForLoopNode* forLoop) {
    if (!forLoop) {
        return;
    }
    
    // (AR) فتح نطاق جديد لحلقة for
    // (EN) Enter new scope for for loop
    enterScope();
    
    // (AR) بناء التهيئة (initialization)
    // (EN) Build initialization
    if (forLoop->initialization) {
        if (auto varDecl = dynamic_cast<AST::VariableDeclNode*>(forLoop->initialization.get())) {
            buildLocalVariable(varDecl);
        } else if (auto assignment = dynamic_cast<AST::AssignmentNode*>(forLoop->initialization.get())) {
            buildAssignment(assignment);
        }
    }
    
    // (AR) إنشاء الكتل
    // (EN) Create blocks
    auto condBlock = createBasicBlock(newLabel("for_cond"));
    auto bodyBlock = createBasicBlock(newLabel("for_body"));
    auto incrementBlock = createBasicBlock(newLabel("for_inc"));
    auto exitBlock = createBasicBlock(newLabel("for_exit"));
    
    // (AR) القفز إلى كتلة الشرط
    // (EN) Jump to condition block
    SIRInstruction jmpToCondInst(SIROpcode::JMP);
    jmpToCondInst.addOperand(SIROperand::makeLabel(condBlock->name));
    addInstruction(jmpToCondInst);
    
    // (AR) بناء كتلة الشرط
    // (EN) Build condition block
    currentFunction_->addBasicBlock(condBlock);
    setCurrentBlock(condBlock);
    
    if (forLoop->condition) {
        auto condResult = buildExpression(forLoop->condition.get());
        
        SIRInstruction brInst(SIROpcode::BR);
        brInst.addOperand(SIROperand::makeRegister(condResult.registerName, condResult.type));
        brInst.addOperand(SIROperand::makeLabel(bodyBlock->name));
        brInst.addOperand(SIROperand::makeLabel(exitBlock->name));
        addInstruction(brInst);
    } else {
        // (AR) بدون شرط - حلقة لا نهائية
        // (EN) No condition - infinite loop
        SIRInstruction jmpInst(SIROpcode::JMP);
        jmpInst.addOperand(SIROperand::makeLabel(bodyBlock->name));
        addInstruction(jmpInst);
    }
    
    // (AR) بناء كتلة الجسم
    // (EN) Build body block
    currentFunction_->addBasicBlock(bodyBlock);
    setCurrentBlock(bodyBlock);
    
    // (AR) تسجيل سياق الحلقة
    // (EN) Register loop context
    LoopContext loopCtx;
    loopCtx.continueLabel = incrementBlock->name;
    loopCtx.breakLabel = exitBlock->name;
    enterLoop(loopCtx);
    
    if (forLoop->body) {
        for (auto& stmt : forLoop->body->statements) {
            buildStatement(stmt.get());
        }
    }
    
    // (AR) القفز إلى كتلة الزيادة
    // (EN) Jump to increment block
    if (getCurrentBlock()->getTerminator() == nullptr) {
        SIRInstruction jmpToIncInst(SIROpcode::JMP);
        jmpToIncInst.addOperand(SIROperand::makeLabel(incrementBlock->name));
        addInstruction(jmpToIncInst);
    }
    
    exitLoop();
    
    // (AR) بناء كتلة الزيادة
    // (EN) Build increment block
    currentFunction_->addBasicBlock(incrementBlock);
    setCurrentBlock(incrementBlock);
    
    if (forLoop->increment) {
        if (auto assignment = dynamic_cast<AST::AssignmentNode*>(forLoop->increment.get())) {
            buildAssignment(assignment);
        } else {
            buildExpression(forLoop->increment.get());
        }
    }
    
    // (AR) العودة إلى كتلة الشرط
    // (EN) Jump back to condition
    SIRInstruction jmpToCondInst2(SIROpcode::JMP);
    jmpToCondInst2.addOperand(SIROperand::makeLabel(condBlock->name));
    addInstruction(jmpToCondInst2);
    
    // (AR) الانتقال إلى كتلة الخروج
    // (EN) Move to exit block
    currentFunction_->addBasicBlock(exitBlock);
    setCurrentBlock(exitBlock);
    
    // (AR) إغلاق النطاق
    // (EN) Exit scope
    exitScope();
}

void SIRBuilder::buildBreakStatement(AST::BreakStatementNode* breakStmt) {
    if (!breakStmt) {
        return;
    }
    
    // (AR) التحقق من وجود حلقة
    // (EN) Check if inside loop
    LoopContext* loopCtx = getCurrentLoop();
    if (!loopCtx) {
        reportError("Break statement outside loop");
        return;
    }
    
    // (AR) القفز إلى نهاية الحلقة
    // (EN) Jump to loop exit
    SIRInstruction jmpInst(SIROpcode::JMP);
    jmpInst.addOperand(SIROperand::makeLabel(loopCtx->breakLabel));
    addInstruction(jmpInst);
}

void SIRBuilder::buildContinueStatement(AST::ContinueStatementNode* continueStmt) {
    if (!continueStmt) {
        return;
    }
    
    // (AR) التحقق من وجود حلقة
    // (EN) Check if inside loop
    LoopContext* loopCtx = getCurrentLoop();
    if (!loopCtx) {
        reportError("Continue statement outside loop");
        return;
    }
    
    // (AR) القفز إلى بداية الحلقة
    // (EN) Jump to loop continue point
    SIRInstruction jmpInst(SIROpcode::JMP);
    jmpInst.addOperand(SIROperand::makeLabel(loopCtx->continueLabel));
    addInstruction(jmpInst);
}

// ============================================================================
// بناء التعابير / Building Expressions
// ============================================================================

BuildResult SIRBuilder::buildExpression(AST::ExpressionNode* expr) {
    if (!expr) {
        return BuildResult();
    }
    
    // (AR) توزيع على النوع المناسب
    // (EN) Dispatch to appropriate type
    if (auto binOp = dynamic_cast<AST::BinaryOpNode*>(expr)) {
        return buildBinaryOp(binOp);
    }
    else if (auto unOp = dynamic_cast<AST::UnaryOpNode*>(expr)) {
        return buildUnaryOp(unOp);
    }
    else if (auto call = dynamic_cast<AST::FunctionCallNode*>(expr)) {
        return buildFunctionCall(call);
    }
    else if (auto var = dynamic_cast<AST::VariableNode*>(expr)) {
        return buildVariableAccess(var);
    }
    else if (auto literal = dynamic_cast<AST::LiteralNode*>(expr)) {
        return buildLiteral(literal);
    }
    else if (auto arrayCreate = dynamic_cast<AST::ArrayCreationNode*>(expr)) {
        return buildArrayCreation(arrayCreate);
    }
    else if (auto arrayAccess = dynamic_cast<AST::ArrayAccessNode*>(expr)) {
        return buildArrayAccess(arrayAccess);
    }
    else if (auto arrayLiteral = dynamic_cast<AST::ArrayLiteralNode*>(expr)) {
        return buildArrayLiteral(arrayLiteral);
    }
    else if (auto memberAccess = dynamic_cast<AST::MemberAccessNode*>(expr)) {
        return buildMemberAccess(memberAccess);
    }
    else if (auto methodCall = dynamic_cast<AST::MethodCallNode*>(expr)) {
        return buildMethodCall(methodCall);
    }
    else if (auto newExpr = dynamic_cast<AST::NewExpressionNode*>(expr)) {
        return buildNewExpression(newExpr);
    }
    
    reportError("Unknown expression type");
    return BuildResult();
}

BuildResult SIRBuilder::buildLiteral(AST::LiteralNode* literal) {
    if (!literal) {
        return BuildResult();
    }
    
    // (AR) معالجة حسب نوع الثابت
    // (EN) Handle based on literal type
    if (literal->type == AST::LiteralType::INTEGER) {
        // (AR) رقم صحيح
        // (EN) Integer number
        std::string tempReg = newTempRegister();
        SIRInstruction loadInst(SIROpcode::LOAD_IMM);
        loadInst.setResult(tempReg, SIRType::I64);
        loadInst.addOperand(SIROperand::makeImmediate(literal->value, SIRType::I64));
        addInstruction(loadInst);
        
        return BuildResult(tempReg, SIRType::I64);
    }
    else if (literal->type == AST::LiteralType::FLOAT) {
        // (AR) رقم عشري
        // (EN) Floating point number
        std::string tempReg = newTempRegister();
        SIRInstruction loadInst(SIROpcode::LOAD_IMM);
        loadInst.setResult(tempReg, SIRType::F64);
        loadInst.addOperand(SIROperand::makeImmediate(literal->value, SIRType::F64));
        addInstruction(loadInst);
        
        return BuildResult(tempReg, SIRType::F64);
    }
    else if (literal->type == AST::LiteralType::STRING) {
        // (AR) نص
        // (EN) String
        std::string strConst = module_->addStringConstant(literal->value);
        std::string tempReg = newTempRegister();
        SIRInstruction loadInst(SIROpcode::LOAD_STR);
        loadInst.setResult(tempReg, SIRType::STRING);
        loadInst.addOperand(SIROperand::makeLabel(strConst));
        addInstruction(loadInst);
        
        return BuildResult(tempReg, SIRType::STRING);
    }
    else if (literal->type == AST::LiteralType::BOOLEAN) {
        // (AR) منطقي
        // (EN) Boolean
        std::string tempReg = newTempRegister();
        SIRInstruction loadInst(SIROpcode::LOAD_IMM);
        loadInst.setResult(tempReg, SIRType::BOOL);
        std::string boolValue = (literal->value == "true" || literal->value == "صحيح") ? "1" : "0";
        loadInst.addOperand(SIROperand::makeImmediate(boolValue, SIRType::BOOL));
        addInstruction(loadInst);
        
        return BuildResult(tempReg, SIRType::BOOL);
    }
    
    reportError("Unknown literal type");
    return BuildResult();
}

BuildResult SIRBuilder::buildBinaryOp(AST::BinaryOpNode* binOp) {
    if (!binOp) {
        return BuildResult();
    }
    
    // (AR) بناء المعاملين
    // (EN) Build operands
    auto leftResult = buildExpression(binOp->left.get());
    auto rightResult = buildExpression(binOp->right.get());
    
    // (AR) معالجة عمليات النصوص الخاصة
    // (EN) Handle special string operations
    if (leftResult.type == SIRType::STRING || rightResult.type == SIRType::STRING) {
        // (AR) كلا المعاملين يجب أن يكونا نصوص
        // (EN) Both operands must be strings
        if (leftResult.type != SIRType::STRING || rightResult.type != SIRType::STRING) {
            reportError("Cannot mix string with non-string in operation");
            return BuildResult();
        }
        
        // (AR) معالجة دمج النصوص (+)
        // (EN) Handle string concatenation (+)
        if (binOp->op == "+") {
            std::string tempReg = newTempRegister();
            SIRInstruction inst(SIROpcode::STR_CONCAT);
            inst.setResult(tempReg, SIRType::STRING);
            inst.addOperand(SIROperand::makeRegister(leftResult.registerName, SIRType::STRING));
            inst.addOperand(SIROperand::makeRegister(rightResult.registerName, SIRType::STRING));
            addInstruction(inst);
            return BuildResult(tempReg, SIRType::STRING);
        }
        
        // (AR) معالجة مقارنة النصوص (==, !=, <, >, <=, >=)
        // (EN) Handle string comparison (==, !=, <, >, <=, >=)
        if (binOp->op == "==" || binOp->op == "!=" || 
            binOp->op == "<" || binOp->op == ">" || 
            binOp->op == "<=" || binOp->op == ">=") {
            
            // (AR) توليد STR_CMP instruction
            // (EN) Generate STR_CMP instruction
            std::string cmpReg = newTempRegister();
            SIRInstruction cmpInst(SIROpcode::STR_CMP);
            cmpInst.setResult(cmpReg, SIRType::I64); // يرجع -1, 0, أو 1
            cmpInst.addOperand(SIROperand::makeRegister(leftResult.registerName, SIRType::STRING));
            cmpInst.addOperand(SIROperand::makeRegister(rightResult.registerName, SIRType::STRING));
            addInstruction(cmpInst);
            
            // (AR) تطبيق عملية المقارنة المطلوبة
            // (EN) Apply requested comparison operation
            std::string tempReg = newTempRegister();
            SIROpcode cmpOpcode;
            
            if (binOp->op == "==") {
                // cmpReg == 0
                cmpOpcode = SIROpcode::ICMP_EQ;
            } else if (binOp->op == "!=") {
                // cmpReg != 0
                cmpOpcode = SIROpcode::ICMP_NE;
            } else if (binOp->op == "<") {
                // cmpReg < 0
                cmpOpcode = SIROpcode::ICMP_SLT;
            } else if (binOp->op == ">") {
                // cmpReg > 0
                cmpOpcode = SIROpcode::ICMP_SGT;
            } else if (binOp->op == "<=") {
                // cmpReg <= 0
                cmpOpcode = SIROpcode::ICMP_SLE;
            } else { // ">="
                // cmpReg >= 0
                cmpOpcode = SIROpcode::ICMP_SGE;
            }
            
            SIRInstruction resultInst(cmpOpcode);
            resultInst.setResult(tempReg, SIRType::BOOL);
            resultInst.addOperand(SIROperand::makeRegister(cmpReg, SIRType::I64));
            resultInst.addOperand(SIROperand::makeImmediate("0", SIRType::I64));
            addInstruction(resultInst);
            
            return BuildResult(tempReg, SIRType::BOOL);
        }
        
        // (AR) عملية نصية غير مدعومة
        // (EN) Unsupported string operation
        reportError("Unsupported operation on strings: " + binOp->op);
        return BuildResult();
    }
    
    // (AR) التحقق من توافق الأنواع للأنواع غير النصية
    // (EN) Check type compatibility for non-string types
    SIRType resultType = leftResult.type;
    if (leftResult.type != rightResult.type) {
        // (AR) ترقية النوع إذا لزم الأمر
        // (EN) Promote type if needed
        if (leftResult.type == SIRType::I64 && rightResult.type == SIRType::F64) {
            leftResult = convertType(leftResult, SIRType::F64);
            resultType = SIRType::F64;
        }
        else if (leftResult.type == SIRType::F64 && rightResult.type == SIRType::I64) {
            rightResult = convertType(rightResult, SIRType::F64);
            resultType = SIRType::F64;
        }
        else {
            reportError("Type mismatch in binary operation");
            return BuildResult();
        }
    }
    
    // (AR) تحديد العملية
    // (EN) Determine operation
    SIROpcode opcode = binaryOpToOpcode(binOp->op, leftResult.type);
    
    // (AR) عمليات المقارنة ترجع bool
    // (EN) Comparison operations return bool
    if (opcode >= SIROpcode::ICMP_EQ && opcode <= SIROpcode::FCMP_UGE) {
        resultType = SIRType::BOOL;
    }
    
    // (AR) توليد التعليمة
    // (EN) Generate instruction
    std::string tempReg = newTempRegister();
    SIRInstruction inst(opcode);
    inst.setResult(tempReg, resultType);
    inst.addOperand(SIROperand::makeRegister(leftResult.registerName, leftResult.type));
    inst.addOperand(SIROperand::makeRegister(rightResult.registerName, rightResult.type));
    addInstruction(inst);
    
    return BuildResult(tempReg, resultType);
}

BuildResult SIRBuilder::buildUnaryOp(AST::UnaryOpNode* unOp) {
    if (!unOp) {
        return BuildResult();
    }
    
    // (AR) بناء المعامل
    // (EN) Build operand
    auto operandResult = buildExpression(unOp->operand.get());
    
    // (AR) تحديد العملية
    // (EN) Determine operation
    SIROpcode opcode = unaryOpToOpcode(unOp->op, operandResult.type);
    
    // (AR) توليد التعليمة
    // (EN) Generate instruction
    std::string tempReg = newTempRegister();
    SIRInstruction inst(opcode);
    inst.setResult(tempReg, operandResult.type);
    inst.addOperand(SIROperand::makeRegister(operandResult.registerName, operandResult.type));
    addInstruction(inst);
    
    return BuildResult(tempReg, operandResult.type);
}

BuildResult SIRBuilder::buildFunctionCall(AST::FunctionCallNode* call) {
    if (!call) {
        return BuildResult();
    }
    
    // (AR) البحث عن الدالة
    // (EN) Look up function
    auto funcIt = functionTable_.find(call->functionName);
    if (funcIt == functionTable_.end()) {
        reportError("Undefined function: " + call->functionName);
        return BuildResult();
    }
    
    const FunctionInfo& funcInfo = funcIt->second;
    
    // (AR) التحقق من عدد المعاملات
    // (EN) Check parameter count
    if (call->arguments.size() != funcInfo.parameters.size()) {
        reportError("Function '" + call->functionName + "' expects " + 
                   std::to_string(funcInfo.parameters.size()) + " arguments, got " +
                   std::to_string(call->arguments.size()));
        return BuildResult();
    }
    
    // (AR) بناء المعاملات
    // (EN) Build arguments
    std::vector<BuildResult> argResults;
    for (size_t i = 0; i < call->arguments.size(); ++i) {
        auto argResult = buildExpression(call->arguments[i].get());
        
        // (AR) التحقق من توافق النوع
        // (EN) Check type compatibility
        if (!areTypesCompatible(argResult.type, funcInfo.parameters[i].type)) {
            argResult = convertType(argResult, funcInfo.parameters[i].type);
        }
        
        argResults.push_back(argResult);
    }
    
    // (AR) توليد تعليمة CALL
    // (EN) Generate CALL instruction
    SIRInstruction callInst(SIROpcode::CALL);
    
    // (AR) إضافة اسم الدالة
    // (EN) Add function name
    callInst.addOperand(SIROperand::makeLabel(call->functionName));
    
    // (AR) إضافة المعاملات
    // (EN) Add arguments
    for (const auto& argResult : argResults) {
        callInst.addOperand(SIROperand::makeRegister(argResult.registerName, argResult.type));
    }
    
    // (AR) معالجة القيمة المرجعة
    // (EN) Handle return value
    if (funcInfo.returnType != SIRType::VOID) {
        std::string tempReg = newTempRegister();
        callInst.setResult(tempReg, funcInfo.returnType);
        addInstruction(callInst);
        return BuildResult(tempReg, funcInfo.returnType);
    }
    else {
        addInstruction(callInst);
        return BuildResult();
    }
}

BuildResult SIRBuilder::buildVariableAccess(AST::VariableNode* var) {
    if (!var) {
        return BuildResult();
    }
    
    // (AR) البحث عن المتغير
    // (EN) Look up variable
    VariableInfo* varInfo = lookupVariable(var->name);
    if (!varInfo) {
        reportError("Undefined variable: " + var->name);
        return BuildResult();
    }
    
    // (AR) توليد تعليمة LOAD
    // (EN) Generate LOAD instruction
    std::string tempReg = newTempRegister();
    SIRInstruction loadInst(SIROpcode::LOAD);
    loadInst.setResult(tempReg, varInfo->type);
    loadInst.addOperand(SIROperand::makeRegister(varInfo->registerName, varInfo->type));
    addInstruction(loadInst);
    
    return BuildResult(tempReg, varInfo->type);
}

BuildResult SIRBuilder::buildArrayCreation(AST::ArrayCreationNode* arrayCreate) {
    if (!arrayCreate) {
        return BuildResult();
    }
    
    // (AR) بناء حجم المصفوفة
    // (EN) Build array size
    auto sizeResult = buildExpression(arrayCreate->size.get());
    
    // (AR) التحقق من أن الحجم رقم صحيح
    // (EN) Verify size is integer
    if (sizeResult.type != SIRType::I64) {
        reportError("Array size must be an integer");
        return BuildResult();
    }
    
    // (AR) تحديد نوع العنصر
    // (EN) Determine element type
    SIRType elementType = astTypeToSIRType(arrayCreate->elementType);
    
    // (AR) توليد تعليمة ARRAY_NEW
    // (EN) Generate ARRAY_NEW instruction
    std::string tempReg = newTempRegister();
    SIRInstruction arrayNewInst(SIROpcode::ARRAY_NEW);
    arrayNewInst.setResult(tempReg, SIRType::ARRAY);
    arrayNewInst.addOperand(SIROperand::makeRegister(sizeResult.registerName, sizeResult.type));
    addInstruction(arrayNewInst);
    
    // (AR) إرجاع النتيجة مع نوع العنصر
    // (EN) Return result with element type
    BuildResult result(tempReg, SIRType::ARRAY);
    result.elementType = elementType;
    return result;
}

BuildResult SIRBuilder::buildArrayAccess(AST::ArrayAccessNode* arrayAccess) {
    if (!arrayAccess) {
        return BuildResult();
    }
    
    // (AR) بناء المصفوفة
    // (EN) Build array
    auto arrayResult = buildExpression(arrayAccess->array.get());
    
    // (AR) التحقق من أن المصفوفة من النوع ARRAY
    // (EN) Verify array type
    if (arrayResult.type != SIRType::ARRAY) {
        reportError("Cannot index non-array type");
        return BuildResult();
    }
    
    // (AR) بناء الفهرس
    // (EN) Build index
    auto indexResult = buildExpression(arrayAccess->index.get());
    
    // (AR) التحقق من أن الفهرس رقم صحيح
    // (EN) Verify index is integer
    if (indexResult.type != SIRType::I64) {
        reportError("Array index must be an integer");
        return BuildResult();
    }
    
    // (AR) توليد تعليمة ARRAY_GET
    // (EN) Generate ARRAY_GET instruction
    std::string tempReg = newTempRegister();
    
    // (AR) الحصول علم نوع عنصر المصفوفة من arrayResult إن كان متاحاً
    // (EN) Get array element type from arrayResult if available
    SIRType resultType = (arrayResult.elementType != SIRType::VOID) 
                        ? arrayResult.elementType 
                        : SIRType::I64; // (AR) نوع افتراضي / default type
    
    SIRInstruction arrayGetInst(SIROpcode::ARRAY_GET);
    arrayGetInst.setResult(tempReg, resultType);
    arrayGetInst.addOperand(SIROperand::makeRegister(arrayResult.registerName, arrayResult.type));
    arrayGetInst.addOperand(SIROperand::makeRegister(indexResult.registerName, indexResult.type));
    addInstruction(arrayGetInst);
    
    return BuildResult(tempReg, resultType);
}

BuildResult SIRBuilder::buildArrayLiteral(AST::ArrayLiteralNode* arrayLiteral) {
    if (!arrayLiteral) {
        return BuildResult();
    }
    
    // (AR) إنشاء مصفوفة بحجم العناصر
    // (EN) Create array with element count
    std::string sizeReg = newTempRegister();
    SIRInstruction loadSizeInst(SIROpcode::LOAD_IMM);
    loadSizeInst.setResult(sizeReg, SIRType::I64);
    loadSizeInst.addOperand(SIROperand::makeImmediate(
        std::to_string(arrayLiteral->elements.size()), SIRType::I64));
    addInstruction(loadSizeInst);
    
    std::string arrayReg = newTempRegister();
    SIRInstruction arrayNewInst(SIROpcode::ARRAY_NEW);
    arrayNewInst.setResult(arrayReg, SIRType::ARRAY);
    arrayNewInst.addOperand(SIROperand::makeRegister(sizeReg, SIRType::I64));
    addInstruction(arrayNewInst);
    
    // (AR) ملء المصفوفة بالعناصر وتحديد نوع العنصر
    // (EN) Fill array with elements and determine element type
    SIRType commonElementType = SIRType::VOID;
    
    for (size_t i = 0; i < arrayLiteral->elements.size(); ++i) {
        auto elementResult = buildExpression(arrayLiteral->elements[i].get());
        
        // (AR) تحديد النوع المشترك للعناصر
        // (EN) Determine common element type
        if (i == 0) {
            commonElementType = elementResult.type;
        } else if (commonElementType != elementResult.type && commonElementType != SIRType::VOID) {
            // (AR) أنواع مختلفة - استخدم VOID للدلالة على مصفوفة مختلطة
            // (EN) Mixed types - use VOID to indicate mixed array
            commonElementType = SIRType::VOID;
        }
        
        // (AR) إنشاء فهرس
        // (EN) Create index
        std::string indexReg = newTempRegister();
        SIRInstruction loadIndexInst(SIROpcode::LOAD_IMM);
        loadIndexInst.setResult(indexReg, SIRType::I64);
        loadIndexInst.addOperand(SIROperand::makeImmediate(std::to_string(i), SIRType::I64));
        addInstruction(loadIndexInst);
        
        // (AR) تعيين العنصر
        // (EN) Set element
        SIRInstruction arraySetInst(SIROpcode::ARRAY_SET);
        arraySetInst.addOperand(SIROperand::makeRegister(arrayReg, SIRType::ARRAY));
        arraySetInst.addOperand(SIROperand::makeRegister(indexReg, SIRType::I64));
        arraySetInst.addOperand(SIROperand::makeRegister(elementResult.registerName, elementResult.type));
        addInstruction(arraySetInst);
    }
    
    // (AR) إرجاع النتيجة مع نوع العنصر
    // (EN) Return result with element type
    BuildResult result(arrayReg, SIRType::ARRAY);
    result.elementType = commonElementType;
    return result;
}

BuildResult SIRBuilder::buildMemberAccess(AST::MemberAccessNode* memberAccess) {
    if (!memberAccess) {
        return BuildResult();
    }
    
    // (AR) بناء الكائن
    // (EN) Build object
    auto objectResult = buildExpression(memberAccess->object.get());
    
    // (AR) التحقق من أن الكائن من نوع STRUCT أو PTR
    // (EN) Verify object is STRUCT or PTR type
    if (objectResult.type != SIRType::STRUCT && objectResult.type != SIRType::PTR) {
        reportError("Cannot access member of non-object type");
        return BuildResult();
    }
    
    // (AR) الحصول على نوع الحقل من جدول الأصناف إن أمكن
    // (EN) Get field type from class table if possible
    SIRType fieldType = SIRType::I64; // (AR) نوع افتراضي / default type
    
    // (AR) محاولة البحث عن الصنف في الجدول
    // (EN) Try to find class in table
    // Note: هذا يحتاج لتتبع نوع الكائن أثناء البناء
    // Note: This needs object type tracking during build
    // TODO: تحسين مستقبلي - إضافة تتبع نوع الكائن في BuildResult
    // TODO: Future improvement - add object type tracking in BuildResult
    
    // (AR) توليد تعليمة FIELD_GET
    // (EN) Generate FIELD_GET instruction
    std::string tempReg = newTempRegister();
    SIRInstruction fieldGetInst(SIROpcode::FIELD_GET);
    fieldGetInst.setResult(tempReg, fieldType);
    fieldGetInst.addOperand(SIROperand::makeRegister(objectResult.registerName, objectResult.type));
    fieldGetInst.addOperand(SIROperand::makeLabel(memberAccess->memberName));
    addInstruction(fieldGetInst);
    
    return BuildResult(tempReg, fieldType);
}

BuildResult SIRBuilder::buildMethodCall(AST::MethodCallNode* methodCall) {
    if (!methodCall) {
        return BuildResult();
    }
    
    // (AR) بناء الكائن
    // (EN) Build object
    auto objectResult = buildExpression(methodCall->object.get());
    
    // (AR) إذا كان نص، استخدم buildStringMethod
    // (EN) If string, use buildStringMethod
    if (objectResult.type == SIRType::STRING) {
        return buildStringMethod(methodCall);
    }
    
    // (AR) التحقق من نوع الكائن
    // (EN) Verify object type
    if (objectResult.type != SIRType::STRUCT && objectResult.type != SIRType::PTR) {
        reportError("Cannot call method on non-object type");
        return BuildResult();
    }
    
    // (AR) محاولة البحث عن التابع في جدول الأصناف
    // (EN) Try to find method in class table
    SIRType returnType = SIRType::I64; // (AR) نوع افتراضي / default type
    std::string className = objectResult.className; // (AR) الحصول على اسم الصنف من النتيجة / Get class name from result
    
    // (AR) إذا كان اسم الصنف غير معروف، استخدم "unknown"
    // (EN) If class name is unknown, use "unknown"
    if (className.empty()) {
        className = "unknown";
    } else {
        // (AR) محاولة الحصول على نوع الإرجاع من جدول الأصناف
        // (EN) Try to get return type from class table
        auto classIt = classTable_.find(className);
        if (classIt != classTable_.end()) {
            // (AR) يمكن هنا البحث عن التابع في الصنف للحصول على نوع الإرجاع
            // (EN) Could search for method in class to get return type
            // Note: يحتاج لإضافة method lookup في SIRClass
            // Note: Needs method lookup in SIRClass
        }
    }
    
    // (AR) بناء المعاملات
    // (EN) Build arguments
    std::vector<BuildResult> argResults;
    argResults.push_back(objectResult); // this pointer
    
    for (auto& arg : methodCall->arguments) {
        auto argResult = buildExpression(arg.get());
        argResults.push_back(argResult);
    }
    
    // (AR) توليد تعليمة CALL
    // (EN) Generate CALL instruction
    SIRInstruction callInst(SIROpcode::CALL);
    
    // (AR) اسم التابع الكامل: ClassName::MethodName
    // (EN) Full method name: ClassName::MethodName
    std::string fullMethodName = className + "::" + methodCall->methodName;
    callInst.addOperand(SIROperand::makeLabel(fullMethodName));
    
    // (AR) إضافة المعاملات
    // (EN) Add arguments
    for (const auto& argResult : argResults) {
        callInst.addOperand(SIROperand::makeRegister(argResult.registerName, argResult.type));
    }
    
    // (AR) معالجة القيمة المرجعة
    // (EN) Handle return value
    std::string tempReg = newTempRegister();
    callInst.setResult(tempReg, returnType);
    addInstruction(callInst);
    
    return BuildResult(tempReg, returnType);
}

BuildResult SIRBuilder::buildStringMethod(AST::MethodCallNode* methodCall) {
    if (!methodCall) {
        return BuildResult();
    }
    
    // (AR) بناء الكائن (يجب أن يكون نص)
    // (EN) Build object (must be string)
    auto objectResult = buildExpression(methodCall->object.get());
    
    if (objectResult.type != SIRType::STRING) {
        reportError("String method called on non-string type");
        return BuildResult();
    }
    
    const std::string& methodName = methodCall->methodName;
    
    // (AR) معالجة length() - يرجع i64
    // (EN) Handle length() - returns i64
    if (methodName == "length") {
        std::string tempReg = newTempRegister();
        SIRInstruction inst(SIROpcode::STR_LEN);
        inst.setResult(tempReg, SIRType::I64);
        inst.addOperand(SIROperand::makeRegister(objectResult.registerName, SIRType::STRING));
        addInstruction(inst);
        return BuildResult(tempReg, SIRType::I64);
    }
    
    // (AR) معالجة substring(start, end) - يرجع string
    // (EN) Handle substring(start, end) - returns string
    if (methodName == "substring") {
        if (methodCall->arguments.size() != 2) {
            reportError("substring() requires exactly 2 arguments (start, end)");
            return BuildResult();
        }
        
        auto startResult = buildExpression(methodCall->arguments[0].get());
        auto endResult = buildExpression(methodCall->arguments[1].get());
        
        if (startResult.type != SIRType::I64 || endResult.type != SIRType::I64) {
            reportError("substring() arguments must be integers");
            return BuildResult();
        }
        
        std::string tempReg = newTempRegister();
        SIRInstruction inst(SIROpcode::STR_SUBSTR);
        inst.setResult(tempReg, SIRType::STRING);
        inst.addOperand(SIROperand::makeRegister(objectResult.registerName, SIRType::STRING));
        inst.addOperand(SIROperand::makeRegister(startResult.registerName, SIRType::I64));
        inst.addOperand(SIROperand::makeRegister(endResult.registerName, SIRType::I64));
        addInstruction(inst);
        return BuildResult(tempReg, SIRType::STRING);
    }
    
    // (AR) معالجة indexOf(needle) - يرجع i64
    // (EN) Handle indexOf(needle) - returns i64
    if (methodName == "indexOf") {
        if (methodCall->arguments.size() != 1) {
            reportError("indexOf() requires exactly 1 argument");
            return BuildResult();
        }
        
        auto needleResult = buildExpression(methodCall->arguments[0].get());
        
        if (needleResult.type != SIRType::STRING) {
            reportError("indexOf() argument must be a string");
            return BuildResult();
        }
        
        std::string tempReg = newTempRegister();
        SIRInstruction inst(SIROpcode::STR_INDEX);
        inst.setResult(tempReg, SIRType::I64);
        inst.addOperand(SIROperand::makeRegister(objectResult.registerName, SIRType::STRING));
        inst.addOperand(SIROperand::makeRegister(needleResult.registerName, SIRType::STRING));
        addInstruction(inst);
        return BuildResult(tempReg, SIRType::I64);
    }
    
    // (AR) معالجة charAt(index) - يرجع string (حرف واحد)
    // (EN) Handle charAt(index) - returns string (single char)
    if (methodName == "charAt") {
        if (methodCall->arguments.size() != 1) {
            reportError("charAt() requires exactly 1 argument");
            return BuildResult();
        }
        
        auto indexResult = buildExpression(methodCall->arguments[0].get());
        
        if (indexResult.type != SIRType::I64) {
            reportError("charAt() argument must be an integer");
            return BuildResult();
        }
        
        std::string tempReg = newTempRegister();
        SIRInstruction inst(SIROpcode::STR_CHARAT);
        inst.setResult(tempReg, SIRType::STRING);
        inst.addOperand(SIROperand::makeRegister(objectResult.registerName, SIRType::STRING));
        inst.addOperand(SIROperand::makeRegister(indexResult.registerName, SIRType::I64));
        addInstruction(inst);
        return BuildResult(tempReg, SIRType::STRING);
    }
    
    // (AR) method غير مدعوم
    // (EN) Unsupported method
    reportError("Unknown string method: " + methodName);
    return BuildResult();
}

BuildResult SIRBuilder::buildNewExpression(AST::NewExpressionNode* newExpr) {
    if (!newExpr) {
        return BuildResult();
    }
    
    // (AR) البحث عن الصنف
    // (EN) Look up class
    auto classIt = classTable_.find(newExpr->className);
    if (classIt == classTable_.end()) {
        reportError("Class '" + newExpr->className + "' not found");
        return BuildResult();
    }
    
    // (AR) توليد تعليمة OBJ_NEW
    // (EN) Generate OBJ_NEW instruction
    std::string tempReg = newTempRegister();
    SIRInstruction objNewInst(SIROpcode::OBJ_NEW);
    objNewInst.setResult(tempReg, SIRType::PTR);
    objNewInst.addOperand(SIROperand::makeLabel(newExpr->className));
    addInstruction(objNewInst);
    
    // (AR) استدعاء المُنشئ إن وجد
    // (EN) Call constructor if exists
    if (!newExpr->arguments.empty()) {
        // (AR) بناء المعاملات
        // (EN) Build arguments
        std::vector<BuildResult> argResults;
        argResults.push_back(BuildResult(tempReg, SIRType::PTR)); // this
        
        for (auto& arg : newExpr->arguments) {
            auto argResult = buildExpression(arg.get());
            argResults.push_back(argResult);
        }
        
        // (AR) استدعاء المُنشئ
        // (EN) Call constructor
        SIRInstruction callInst(SIROpcode::CALL);
        callInst.addOperand(SIROperand::makeLabel(newExpr->className + "::__init__"));
        
        for (const auto& argResult : argResults) {
            callInst.addOperand(SIROperand::makeRegister(argResult.registerName, argResult.type));
        }
        
        addInstruction(callInst);
    }
    
    // (AR) إرجاع النتيجة مع اسم الصنف
    // (EN) Return result with class name
    BuildResult result(tempReg, SIRType::PTR);
    result.className = newExpr->className;
    return result;
}

// TODO: باقي تنفيذات التعابير
// TODO: Remaining expression implementations

// ============================================================================
// إدارة السجلات / Register Management
// ============================================================================

std::string SIRBuilder::newTempRegister() {
    return "%" + std::to_string(nextTempRegister_++);
}

std::string SIRBuilder::newLabel(const std::string& prefix) {
    return prefix + std::to_string(nextLabel_++);
}

// ============================================================================
// إدارة Basic Blocks / Basic Block Management
// ============================================================================

std::shared_ptr<SIRBasicBlock> SIRBuilder::createBasicBlock(const std::string& name) {
    return std::make_shared<SIRBasicBlock>(name);
}

void SIRBuilder::setCurrentBlock(std::shared_ptr<SIRBasicBlock> block) {
    currentBlock_ = block;
}

std::shared_ptr<SIRBasicBlock> SIRBuilder::getCurrentBlock() const {
    return currentBlock_;
}

void SIRBuilder::addInstruction(const SIRInstruction& inst) {
    if (currentBlock_) {
        currentBlock_->addInstruction(inst);
    } else {
        reportError("No current basic block");
    }
}

// ============================================================================
// إدارة النطاقات / Scope Management
// ============================================================================

void SIRBuilder::enterScope() {
    currentScopeLevel_++;
    scopeStack_.push_back(std::unordered_map<std::string, VariableInfo>());
}

void SIRBuilder::exitScope() {
    if (!scopeStack_.empty()) {
        scopeStack_.pop_back();
        currentScopeLevel_--;
    }
}

void SIRBuilder::addVariable(const VariableInfo& varInfo) {
    if (!scopeStack_.empty()) {
        scopeStack_.back()[varInfo.name] = varInfo;
    }
}

VariableInfo* SIRBuilder::lookupVariable(const std::string& name) {
    // (AR) البحث من الداخل للخارج
    // (EN) Search from inner to outer
    for (auto it = scopeStack_.rbegin(); it != scopeStack_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

// ============================================================================
// إدارة الحلقات / Loop Management
// ============================================================================

void SIRBuilder::enterLoop(const LoopContext& ctx) {
    loopStack_.push_back(ctx);
}

void SIRBuilder::exitLoop() {
    if (!loopStack_.empty()) {
        loopStack_.pop_back();
    }
}

LoopContext* SIRBuilder::getCurrentLoop() {
    if (!loopStack_.empty()) {
        return &loopStack_.back();
    }
    return nullptr;
}

// ============================================================================
// معالجة الأخطاء / Error Handling
// ============================================================================

void SIRBuilder::reportError(const std::string& message) {
    errors_.push_back(message);
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

SIRType SIRBuilder::astTypeToSIRType(Data::DataType dataType) {
    // (AR) تحويل شامل من DataType إلى SIRType
    // (EN) Comprehensive conversion from DataType to SIRType
    
    switch (dataType) {
        // (AR) الأنواع الأساسية / Basic types
        case Data::DataType::INTEGER:
            return SIRType::I64;
        
        case Data::DataType::FLOAT:
            return SIRType::F64;
        
        case Data::DataType::BOOLEAN:
            return SIRType::BOOL;
        
        case Data::DataType::STRING:
            return SIRType::STRING;
        
        case Data::DataType::NONE:
            return SIRType::VOID;
        
        // (AR) الأنواع المركبة / Composite types
        case Data::DataType::ARRAY:
            return SIRType::ARRAY;
        
        case Data::DataType::MAP:
            // (AR) نستخدم STRUCT لتمثيل القواميس
            // (EN) Use STRUCT to represent maps/dictionaries
            return SIRType::STRUCT;
        
        case Data::DataType::TUPLE:
            // (AR) نستخدم STRUCT لتمثيل الثنائيات
            // (EN) Use STRUCT to represent tuples
            return SIRType::STRUCT;
        
        case Data::DataType::FUNCTION:
            return SIRType::FUNCTION;
        
        case Data::DataType::OBJECT:
            // (AR) كائنات الأصناف تُمثل بمؤشرات
            // (EN) Class objects are represented as pointers
            return SIRType::PTR;
        
        case Data::DataType::ENUM:
            // (AR) التعدادات تُخزن كأرقام صحيحة
            // (EN) Enums are stored as integers
            return SIRType::I64;
        
        case Data::DataType::BYTE:
            // (AR) البايتات تُمثل كأرقام صحيحة صغيرة
            // (EN) Bytes are represented as small integers
            return SIRType::I64;
        
        case Data::DataType::ERROR:
            // (AR) الأخطاء تُمثل بمؤشرات لكائنات الأخطاء
            // (EN) Errors are represented as pointers to error objects
            return SIRType::PTR;
        
        case Data::DataType::UNKNOWN:
        default:
            // (AR) نوع غير معروف - نستخدم VOID كافتراضي
            // (EN) Unknown type - use VOID as default
            reportError("Unknown or unresolved data type");
            return SIRType::VOID;
    }
}

SIROpcode SIRBuilder::binaryOpToOpcode(Lexer::TokenType op, SIRType leftType) {
    // (AR) تحويل شامل للعوامل الثنائية مع دعم Integer و Float
    // (EN) Comprehensive binary operator conversion with Integer and Float support
    
    // (AR) تحديد ما إذا كان النوع عشري
    // (EN) Determine if type is floating point
    bool isFloat = (leftType == SIRType::F64);
    
    // =========================================================================
    // (AR) العمليات الحسابية / Arithmetic Operations
    // =========================================================================
    
    if (op == Lexer::TokenType::OP_PLUS) {
        return isFloat ? SIROpcode::FADD : SIROpcode::IADD;
    }
    
    if (op == Lexer::TokenType::OP_MINUS) {
        return isFloat ? SIROpcode::FSUB : SIROpcode::ISUB;
    }
    
    if (op == Lexer::TokenType::OP_MULTIPLY) {
        return isFloat ? SIROpcode::FMUL : SIROpcode::IMUL;
    }
    
    if (op == Lexer::TokenType::OP_DIVIDE) {
        // (AR) القسمة الصحيحة تُستخدم IDIV، العشرية FDIV
        // (EN) Integer division uses IDIV, float uses FDIV
        return isFloat ? SIROpcode::FDIV : SIROpcode::IDIV;
    }
    
    if (op == Lexer::TokenType::OP_MODULO) {
        // (AR) باقي القسمة متاح للأعداد الصحيحة فقط
        // (EN) Modulo is available for integers only
        if (isFloat) {
            reportError("Modulo operator not supported for floating point types");
            return SIROpcode::IMOD;
        }
        return SIROpcode::IMOD;
    }
    
    if (op == Lexer::TokenType::OP_POWER) {
        // (AR) عملية الأس: استخدام BUILTIN_POW
        // (EN) Power operation: use BUILTIN_POW
        // Note: سيتم معالجتها كاستدعاء دالة مضمنة
        // Note: Will be handled as builtin function call
        return SIROpcode::BUILTIN_POW;
    }
    
    // =========================================================================
    // (AR) العمليات المنطقية / Logical Operations
    // =========================================================================
    
    if (op == Lexer::TokenType::OP_AND) {
        // (AR) العملية المنطقية AND
        // (EN) Logical AND operation
        return SIROpcode::AND;
    }
    
    if (op == Lexer::TokenType::OP_OR) {
        // (AR) العملية المنطقية OR
        // (EN) Logical OR operation
        return SIROpcode::OR;
    }
    
    // =========================================================================
    // (AR) عمليات المقارنة / Comparison Operations
    // =========================================================================
    
    if (op == Lexer::TokenType::OP_EQUAL) {
        // (AR) التساوي
        // (EN) Equality
        return isFloat ? SIROpcode::FCMP_OEQ : SIROpcode::ICMP_EQ;
    }
    
    if (op == Lexer::TokenType::OP_NOT_EQUAL) {
        // (AR) عدم التساوي
        // (EN) Inequality
        return isFloat ? SIROpcode::FCMP_ONE : SIROpcode::ICMP_NE;
    }
    
    if (op == Lexer::TokenType::OP_LESS) {
        // (AR) أصغر من
        // (EN) Less than
        return isFloat ? SIROpcode::FCMP_OLT : SIROpcode::ICMP_SLT;
    }
    
    if (op == Lexer::TokenType::OP_LESS_EQUAL) {
        // (AR) أصغر أو يساوي
        // (EN) Less than or equal
        return isFloat ? SIROpcode::FCMP_OLE : SIROpcode::ICMP_SLE;
    }
    
    if (op == Lexer::TokenType::OP_GREATER) {
        // (AR) أكبر من
        // (EN) Greater than
        return isFloat ? SIROpcode::FCMP_OGT : SIROpcode::ICMP_SGT;
    }
    
    if (op == Lexer::TokenType::OP_GREATER_EQUAL) {
        // (AR) أكبر أو يساوي
        // (EN) Greater than or equal
        return isFloat ? SIROpcode::FCMP_OGE : SIROpcode::ICMP_SGE;
    }
    
    // =========================================================================
    // (AR) عمليات البت (للأعداد الصحيحة فقط) / Bitwise Operations (integers only)
    // =========================================================================
    
    // (AR) عمليات البت ستُضاف في المستقبل إن احتُجت
    // (EN) Bitwise operations will be added in future if needed
    
    // (AR) عملية غير معروفة
    // (EN) Unknown operation
    reportError("Unknown or unsupported binary operator");
    return SIROpcode::IADD; // (AR) قيمة افتراضية / default value
}

SIROpcode SIRBuilder::unaryOpToOpcode(Lexer::TokenType op, SIRType operandType) {
    // (AR) تحويل شامل للعوامل الأحادية مع دعم Integer و Float
    // (EN) Comprehensive unary operator conversion with Integer and Float support
    
    // (AR) تحديد ما إذا كان النوع عشري
    // (EN) Determine if type is floating point
    bool isFloat = (operandType == SIRType::F64);
    
    // =========================================================================
    // (AR) العمليات الحسابية الأحادية / Unary Arithmetic Operations
    // =========================================================================
    
    if (op == Lexer::TokenType::OP_MINUS) {
        // (AR) النفي (عكس الإشارة): -x
        // (EN) Negation (sign inversion): -x
        return isFloat ? SIROpcode::FNEG : SIROpcode::INEG;
    }
    
    if (op == Lexer::TokenType::OP_PLUS) {
        // (AR) الإيجاب (لا تأثير): +x
        // (EN) Positive (no effect): +x
        // (AR) يمكن أن نتجاهلها أو نستخدم MOVE
        // (EN) We can ignore it or use MOVE
        return SIROpcode::MOVE;
    }
    
    // =========================================================================
    // (AR) العمليات المنطقية الأحادية / Unary Logical Operations
    // =========================================================================
    
    if (op == Lexer::TokenType::OP_NOT) {
        // (AR) النفي المنطقي: !x أو ليس x
        // (EN) Logical NOT: !x or not x
        return SIROpcode::NOT;
    }
    
    // =========================================================================
    // (AR) عمليات الزيادة والنقصان / Increment and Decrement Operations
    // =========================================================================
    
    if (op == Lexer::TokenType::OP_INCREMENT) {
        // (AR) الزيادة: ++x أو x++
        // (EN) Increment: ++x or x++
        // Note: يحتاج معالجة خاصة للتمييز بين prefix و postfix
        // Note: Needs special handling to differentiate prefix and postfix
        return isFloat ? SIROpcode::FADD : SIROpcode::IADD;
    }
    
    if (op == Lexer::TokenType::OP_DECREMENT) {
        // (AR) النقصان: --x أو x--
        // (EN) Decrement: --x or x--
        return isFloat ? SIROpcode::FSUB : SIROpcode::ISUB;
    }
    
    // =========================================================================
    // (AR) عمليات البت الأحادية (للأعداد الصحيحة فقط)
    // (EN) Unary Bitwise Operations (integers only)
    // =========================================================================
    
    // (AR) عملية NOT البتية: ~x
    // (EN) Bitwise NOT: ~x
    // TODO: إذا احتُجت في المستقبل
    // TODO: If needed in the future
    
    // (AR) عملية غير معروفة
    // (EN) Unknown operation
    reportError("Unknown or unsupported unary operator");
    return SIROpcode::INEG; // (AR) قيمة افتراضية / default value
}

bool SIRBuilder::areTypesCompatible(SIRType t1, SIRType t2) {
    // (AR) نفس النوع - متوافق
    // (EN) Same type - compatible
    if (t1 == t2) {
        return true;
    }
    
    // (AR) التحويلات الضمنية المدعومة
    // (EN) Supported implicit conversions
    
    // (AR) تحويل رقم صحيح إلى عشري (I64 → F64)
    // (EN) Integer to float conversion (I64 → F64)
    if (t1 == SIRType::I64 && t2 == SIRType::F64) {
        return true;
    }
    if (t2 == SIRType::I64 && t1 == SIRType::F64) {
        return true;
    }
    
    // (AR) تحويل BOOL إلى I64 (true=1, false=0)
    // (EN) Boolean to integer conversion (true=1, false=0)
    if (t1 == SIRType::BOOL && t2 == SIRType::I64) {
        return true;
    }
    
    // (AR) المؤشرات متوافقة مع STRUCT
    // (EN) Pointers compatible with STRUCT
    if ((t1 == SIRType::PTR && t2 == SIRType::STRUCT) ||
        (t1 == SIRType::STRUCT && t2 == SIRType::PTR)) {
        return true;
    }
    
    // (AR) المصفوفات متوافقة مع PTR
    // (EN) Arrays compatible with PTR
    if ((t1 == SIRType::ARRAY && t2 == SIRType::PTR) ||
        (t1 == SIRType::PTR && t2 == SIRType::ARRAY)) {
        return true;
    }
    
    return false;
}

BuildResult SIRBuilder::convertType(const BuildResult& value, SIRType targetType) {
    if (value.type == targetType) {
        return value;
    }
    
    std::string tempReg = newTempRegister();
    
    // (AR) I64 → F64
    // (EN) Integer to float conversion
    if (value.type == SIRType::I64 && targetType == SIRType::F64) {
        SIRInstruction convertInst(SIROpcode::I64_TO_F64);
        convertInst.setResult(tempReg, SIRType::F64);
        convertInst.addOperand(SIROperand::makeRegister(value.registerName, SIRType::I64));
        addInstruction(convertInst);
        return BuildResult(tempReg, SIRType::F64);
    }
    
    // (AR) F64 → I64 (truncate)
    // (EN) Float to integer conversion (truncate)
    if (value.type == SIRType::F64 && targetType == SIRType::I64) {
        SIRInstruction convertInst(SIROpcode::F64_TO_I64);
        convertInst.setResult(tempReg, SIRType::I64);
        convertInst.addOperand(SIROperand::makeRegister(value.registerName, SIRType::F64));
        addInstruction(convertInst);
        return BuildResult(tempReg, SIRType::I64);
    }
    
    // (AR) BOOL → I64 (1 or 0)
    // (EN) Boolean to integer conversion
    if (value.type == SIRType::BOOL && targetType == SIRType::I64) {
        SIRInstruction convertInst(SIROpcode::BOOL_TO_I64);
        convertInst.setResult(tempReg, SIRType::I64);
        convertInst.addOperand(SIROperand::makeRegister(value.registerName, SIRType::BOOL));
        addInstruction(convertInst);
        return BuildResult(tempReg, SIRType::I64);
    }
    
    // (AR) I64 → BOOL (0=false, other=true)
    // (EN) Integer to boolean conversion
    if (value.type == SIRType::I64 && targetType == SIRType::BOOL) {
        SIRInstruction convertInst(SIROpcode::ICMP_NE);
        convertInst.setResult(tempReg, SIRType::BOOL);
        convertInst.addOperand(SIROperand::makeRegister(value.registerName, SIRType::I64));
        convertInst.addOperand(SIROperand::makeImmediate("0", SIRType::I64));
        addInstruction(convertInst);
        return BuildResult(tempReg, SIRType::BOOL);
    }
    
    // (AR) تحويل غير مدعوم
    // (EN) Unsupported conversion
    reportError("Cannot convert from type to target type\n لا يمكن التحويل من نوع إلى نوع الهدف");
    return value;
        convertInst.addOperand(SIROperand::makeRegister(value.registerName, value.type));
        addInstruction(convertInst);
        
        return BuildResult(tempReg, SIRType::F64);
    }
    
    reportError("Type conversion not supported");
    return value;
}

bool SIRBuilder::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> SIRBuilder::getErrors() const {
    return errors_;
}

// ======================================================================
// إدارة الأنواع العامة / Generic Types Management
// ======================================================================

void SIRBuilder::enterGenericScope(const std::vector<TypeParameter>& typeParams) {
    GenericScope scope;
    scope.typeParameters = typeParams;
    genericScopeStack_.push_back(scope);
}

void SIRBuilder::exitGenericScope() {
    if (!genericScopeStack_.empty()) {
        genericScopeStack_.pop_back();
    }
}

void SIRBuilder::setTypeSubstitution(const std::string& paramName, SIRType concreteType) {
    if (!genericScopeStack_.empty()) {
        genericScopeStack_.back().typeSubstitutions[paramName] = concreteType;
    }
}

SIRType SIRBuilder::resolveType(const std::string& typeName) {
    // (AR) البحث في نطاقات الأنواع العامة من الداخل للخارج
    // (EN) Search in generic scopes from innermost to outermost
    for (auto it = genericScopeStack_.rbegin(); it != genericScopeStack_.rend(); ++it) {
        if (it->hasTypeParameter(typeName)) {
            SIRType substituted = it->getSubstitutedType(typeName);
            if (substituted != SIRType::VOID) {
                return substituted;
            }
            // (AR) معامل نوع غير مستبدل بعد - سيتم تحديده عند الاستدعاء
            // (EN) Type parameter not substituted yet - will be determined at call site
            return SIRType::VOID;
        }
    }
    
    // (AR) ليس معامل نوع عام - حاول تحويل الاسم إلى SIRType
    // (EN) Not a generic type parameter - try converting name to SIRType
    if (typeName == "int" || typeName == "i64") return SIRType::I64;
    if (typeName == "float" || typeName == "f64") return SIRType::F64;
    if (typeName == "string") return SIRType::STRING;
    if (typeName == "bool") return SIRType::BOOL;
    if (typeName == "void") return SIRType::VOID;
    
    // (AR) نوع غير معروف
    // (EN) Unknown type
    return SIRType::VOID;
}

bool SIRBuilder::validateTypeParameters(const std::vector<TypeParameter>& typeParams) {
    // (AR) التحقق من عدم وجود أسماء مكررة
    // (EN) Check for duplicate names
    std::unordered_set<std::string> names;
    for (const auto& param : typeParams) {
        if (names.count(param.name) > 0) {
            reportError("Duplicate type parameter name: " + param.name + "\n اسم معامل النوع مكرر: " + param.name);
            return false;
        }
        names.insert(param.name);
        
        // (AR) التحقق من صحة الاسم (يجب أن يبدأ بحرف كبير)
        // (EN) Validate name (should start with uppercase letter)
        if (param.name.empty() || !std::isupper(param.name[0])) {
            reportError("Type parameter must start with uppercase letter: " + param.name +"\n يجب أن يبدأ معامل النوع بحرف كبير: " + param.name);
            return false;
        }
    }
    
    return true;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
