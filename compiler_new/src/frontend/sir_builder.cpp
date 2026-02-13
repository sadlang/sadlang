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
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

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
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) المرحلة الأولى: تسجيل توقيعات جميع الدوال مسبقاً
    // (EN) Phase 1: Pre-register all function signatures for forward references
    // ═══════════════════════════════════════════════════════════════════
    for (const auto& stmt : *program) {
        if (!stmt) continue;
        
        AST::FunctionDecl* funcDecl = nullptr;
        
        // (AR) دالة عادية
        if (auto fd = dynamic_cast<Sad::AST::FunctionDecl*>(stmt.get())) {
            funcDecl = fd;
        }
        // (AR) دالة مُصدَّرة (الإصدار الجديد)
        else if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl*>(stmt.get())) {
            if (exportDecl->declaration) {
                funcDecl = dynamic_cast<Sad::AST::FunctionDecl*>(exportDecl->declaration.get());
            }
        }
        // (AR) دالة مُصدَّرة (الإصدار القديم)
        else if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt*>(stmt.get())) {
            if (exportStmt->declaration) {
                funcDecl = dynamic_cast<Sad::AST::FunctionDecl*>(exportStmt->declaration.get());
            }
        }
        
        if (funcDecl) {
            // (AR) تسجيل توقيع الدالة في الجدول
            // (EN) Register function signature in function table
            FunctionInfo funcInfo;
            funcInfo.name = funcDecl->name;
            funcInfo.returnType = astTypeToSIRType(funcDecl->returnType);
            for (const auto& param : funcDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                funcInfo.parameters.push_back(SIRParameter(param.name, paramType));
            }
            // (AR) مؤشر الدالة سيُحدَّث لاحقاً في buildFunction
            // (EN) sirFunction pointer will be updated later in buildFunction
            funcInfo.sirFunction = nullptr;
            functionTable_[funcDecl->name] = funcInfo;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) المرحلة الثانية: بناء جميع التصريحات
    // (EN) Phase 2: Build all declarations
    // ═══════════════════════════════════════════════════════════════════
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
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) نظام الاستيراد والتصدير في المترجم
        // (EN) Import/Export system in compiler
        // ═══════════════════════════════════════════════════════════════
        
        // (AR) تصدير (الإصدار الجديد): صدّر دالة|صنف|متغير ...
        // (EN) Export declaration (new version): export function|class|var ...
        if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl*>(stmt.get())) {
            // (AR) استخراج التصريح الداخلي وترجمته مباشرة
            // (EN) Extract inner declaration and compile it directly
            if (exportDecl->declaration) {
                auto* innerStmt = exportDecl->declaration.get();
                
                if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl*>(innerStmt)) {
                    buildFunction(innerFunc);
                } else if (auto innerVar = dynamic_cast<Sad::AST::VarDeclStmt*>(innerStmt)) {
                    buildGlobalVariable(innerVar);
                } else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl*>(innerStmt)) {
                    buildClass(innerClass);
                }
            }
            continue;
        }
        
        // (AR) تصدير (الإصدار القديم): صدّر ...
        // (EN) Export statement (legacy): export ...
        if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt*>(stmt.get())) {
            if (exportStmt->declaration) {
                auto* innerStmt = exportStmt->declaration.get();
                
                if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl*>(innerStmt)) {
                    buildFunction(innerFunc);
                } else if (auto innerVar = dynamic_cast<Sad::AST::VarDeclStmt*>(innerStmt)) {
                    buildGlobalVariable(innerVar);
                } else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl*>(innerStmt)) {
                    buildClass(innerClass);
                }
            }
            continue;
        }
        
        // (AR) استيراد انتقائي: من وحدة استورد ...
        // (EN) Selective import: from module import ...
        if (auto fromImport = dynamic_cast<Sad::AST::FromImportStmt*>(stmt.get())) {
            buildFromImportStmt(fromImport);
            continue;
        }
        
        // (AR) استيراد كامل: استورد وحدة
        // (EN) Full import: import module
        if (auto importStmt = dynamic_cast<Sad::AST::ImportStmt*>(stmt.get())) {
            buildImportStmt(importStmt);
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
    
    // (AR) تسجيل معاملات الدالة في النطاق
    // (EN) Register function parameters in scope
    for (const auto& param : funcDecl->parameters) {
        VariableInfo paramInfo;
        paramInfo.name = param.name;
        paramInfo.type = astTypeToSIRType(param.type);
        // (AR) المعاملات تحصل على سجل باسم %اسم_المعامل
        // (EN) Parameters get register named %parameter_name
        paramInfo.registerName = "%" + param.name;
        paramInfo.isGlobal = false;
        paramInfo.isMutable = false;
        paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
        addVariable(paramInfo);
    }
    
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
                // (AR) للدوال غير void، نضيف return بقيمة افتراضية
                // (EN) For non-void functions, add return with default value
                SIRInstruction retInst;
                retInst.opcode = SIROpcode::RET;
                if (returnType == SIRType::STRING) {
                    retInst.operands.push_back(SIROperand::ConstantString(""));
                } else {
                    retInst.operands.push_back(SIROperand::ConstantI64(0));
                }
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
            if (returnType == SIRType::STRING) {
                retInst.operands.push_back(SIROperand::ConstantString(""));
            } else {
                retInst.operands.push_back(SIROperand::ConstantI64(0));
            }
            currentBlock_->addInstruction(retInst);
        }
    }
    
    // (AR) إضافة الدالة للوحدة (sir_module.h:569 - addFunction)
    // (EN) Add function to module
    module_->addFunction(sirFunction);
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) تسجيل/تحديث الدالة في جدول الدوال للبحث عنها عند الاستدعاء
    // (EN) Register/update function in functionTable_ for call resolution
    // ═══════════════════════════════════════════════════════════════════
    {
        FunctionInfo funcInfo;
        funcInfo.name = funcDecl->name;
        funcInfo.returnType = returnType;
        funcInfo.parameters = sirFunction->getParameters();
        funcInfo.sirFunction = sirFunction;
        functionTable_[funcDecl->name] = funcInfo;
    }
    
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
    
    // (AR) معالجة القيمة الأولية إذا كانت ثابتاً حرفياً
    // (EN) Handle initializer if it's a literal constant
    if (varDecl->initializer) {
        if (auto* litExpr = dynamic_cast<Sad::AST::LiteralExpr*>(varDecl->initializer.get())) {
            const auto& token = litExpr->token;
            std::string value = token.getValue();
            Lexer::TokenType tokenType = token.getType();
            
            if (tokenType == Lexer::TokenType::NUMBER_INTEGER) {
                // (AR) تحويل الأعداد الست عشرية/الثمانية/الثنائية إلى عشرية
                // (EN) Normalize hex/octal/binary literals to decimal
                if (value.size() > 2 && value[0] == '0') {
                    char prefix = value[1];
                    if (prefix == 'x' || prefix == 'X') {
                        value = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
                    } else if (prefix == 'o' || prefix == 'O') {
                        value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
                    } else if (prefix == 'b' || prefix == 'B') {
                        value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
                    }
                }
                sirGlobal->initialValue = value;
            } else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE) {
                sirGlobal->initialValue = value;
            } else if (tokenType == Lexer::TokenType::LITERAL_TRUE) {
                sirGlobal->initialValue = "1";
            } else if (tokenType == Lexer::TokenType::LITERAL_FALSE) {
                sirGlobal->initialValue = "0";
            } else if (tokenType == Lexer::TokenType::STRING_LITERAL) {
                sirGlobal->initialValue = value;
            }
        }
    }
    
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
        
        // (AR) الباني (ConstructorDecl - declarations.h:268)
        // (EN) Constructor
        else if (auto ctorDecl = dynamic_cast<Sad::AST::ConstructorDecl*>(member.get())) {
            std::cout << "[DEBUG] buildClass: found constructor" << std::endl;
            
            std::string fullCtorName = classDecl->name + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء
            auto sirCtor = std::make_shared<SIRFunction>(fullCtorName, SIRType::VOID);
            sirCtor->addParameter(SIRParameter("self", SIRType::I64));
            
            for (const auto& param : ctorDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirCtor->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirCtor);
            
            // (AR) بناء جسم الباني
            // (EN) Build constructor body
            if (ctorDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirCtor;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                // (AR) تسجيل معامل self
                // (EN) Register self parameter
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                // (AR) تسجيل المعاملات + حقول الصنف كمتغيرات محلية
                // (EN) Register parameters + class fields as local variables
                for (const auto& param : ctorDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                // (AR) تسجيل حقول الصنف كمتغيرات محلية للوصول المباشر
                // (EN) Register class fields as local variables for direct access
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirCtor->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                // (AR) Alloca لكل حقل
                // (EN) Alloca for each field
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                buildStatement(ctorDecl->body.get());
                
                // (AR) إضافة RET_VOID
                // (EN) Add RET_VOID
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        retInst.opcode = SIROpcode::RET_VOID;
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirCtor);
                
                // (AR) تسجيل في جدول الدوال
                // (EN) Register in function table
                FunctionInfo ctorInfo;
                ctorInfo.name = fullCtorName;
                ctorInfo.returnType = SIRType::VOID;
                ctorInfo.parameters = sirCtor->getParameters();
                ctorInfo.sirFunction = sirCtor;
                functionTable_[fullCtorName] = ctorInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
        }
        
        // (AR) الدوال (MethodDecl - declarations.h:222)
        // (EN) Methods
        else if (auto methodDecl = dynamic_cast<AST::MethodDecl*>(member.get())) {
            std::cout << "[DEBUG] buildClass: found method '" << methodDecl->name << "'" << std::endl;
            
            SIRType returnType = astTypeToSIRType(methodDecl->returnType);
            std::string fullMethodName = classDecl->name + "." + methodDecl->name;
            auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);
            
            sirMethod->addParameter(SIRParameter("self", SIRType::I64));
            
            for (const auto& param : methodDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirMethod->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirMethod);
            
            // (AR) بناء جسم الطريقة
            // (EN) Build method body
            if (methodDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirMethod;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                // (AR) تسجيل self + معاملات + حقول الصنف
                // (EN) Register self + params + class fields
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                for (const auto& param : methodDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirMethod->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                // (AR) Alloca لكل حقل
                // (EN) Alloca for each field
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                buildStatement(methodDecl->body.get());
                
                // (AR) إضافة terminator
                // (EN) Add terminator
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        if (returnType == SIRType::VOID) {
                            retInst.opcode = SIROpcode::RET_VOID;
                        } else {
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SIRType::STRING) {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            } else {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirMethod);
                
                FunctionInfo methodInfo;
                methodInfo.name = fullMethodName;
                methodInfo.returnType = returnType;
                methodInfo.parameters = sirMethod->getParameters();
                methodInfo.sirFunction = sirMethod;
                functionTable_[fullMethodName] = methodInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
        }
        
        // (AR) الدالة العادية داخل الصنف (FunctionDecl)
        // (EN) Regular function inside class (FunctionDecl)
        else if (auto funcDecl = dynamic_cast<AST::FunctionDecl*>(member.get())) {
            std::cout << "[DEBUG] buildClass: found function as method '" << funcDecl->name << "'" << std::endl;
            
            SIRType returnType = astTypeToSIRType(funcDecl->returnType);
            std::string fullMethodName = classDecl->name + "." + funcDecl->name;
            auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);
            
            sirMethod->addParameter(SIRParameter("self", SIRType::I64));
            
            for (const auto& param : funcDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirMethod->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirMethod);
            
            // (AR) بناء جسم الدالة (مثل الطريقة تماماً)
            // (EN) Build function body (same as method)
            if (funcDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirMethod;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                for (const auto& param : funcDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirMethod->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                buildStatement(funcDecl->body.get());
                
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        if (returnType == SIRType::VOID) {
                            retInst.opcode = SIROpcode::RET_VOID;
                        } else {
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SIRType::STRING) {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            } else {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirMethod);
                
                FunctionInfo methodInfo;
                methodInfo.name = fullMethodName;
                methodInfo.returnType = returnType;
                methodInfo.parameters = sirMethod->getParameters();
                methodInfo.sirFunction = sirMethod;
                functionTable_[fullMethodName] = methodInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
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
    
    // (AR) MatchStmt - جملة match (pattern_nodes.h:MatchStmt)
    // (EN) Match statement (pattern matching)
    if (auto matchStmt = dynamic_cast<Sad::AST::MatchStmt*>(stmt)) {
        std::cout << "[DEBUG] Found MatchStmt" << std::endl;
        buildMatchStatement(matchStmt);
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
        
        // (AR) إذا كانت القيمة ثابتة، أرجعها مباشرة (يجب أن يكون هذا قبل فحص %)
        // (EN) If value is constant, return it directly (must check before % check)
        if (valueResult.isConstant && !valueResult.constantValue.empty()) {
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
                case SIRType::STRING:
                    retInst.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                    break;
                default:
                    retInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
            }
        } else if (!valueResult.registerName.empty() && valueResult.registerName[0] == '%') {
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
    
    // (AR) توليد تعليمة STORE لإسناد القيمة
    // (EN) Generate STORE instruction to assign value
    if (currentBlock_ && !valueResult.registerName.empty()) {
        SIRInstruction storeInst;
        storeInst.opcode = SIROpcode::STORE;
        
        // (AR) المعامل الأول: القيمة المراد تخزينها
        // (EN) First operand: value to store
        SIROperand valueOp;
        if (valueResult.isConstant) {
            valueOp.type = SIROperandType::CONSTANT;
            valueOp.dataType = valueResult.type;
            valueOp.name = valueResult.constantValue;
            if (valueResult.type == SIRType::I64) {
                try { valueOp.intValue = std::stoll(valueResult.constantValue); } catch(...) { valueOp.intValue = 0; }
            } else if (valueResult.type == SIRType::F64) {
                try { valueOp.floatValue = std::stod(valueResult.constantValue); } catch(...) { valueOp.floatValue = 0.0; }
            }
        } else {
            valueOp.type = SIROperandType::REGISTER;
            valueOp.name = valueResult.registerName;
            valueOp.dataType = valueResult.type;
        }
        storeInst.operands.push_back(valueOp);
        
        // (AR) المعامل الثاني: المؤشر (alloca) للمتغير
        // (EN) Second operand: variable pointer (alloca)
        SIROperand ptrOp;
        ptrOp.type = SIROperandType::REGISTER;
        ptrOp.name = varInfo->registerName;
        ptrOp.dataType = varInfo->type;
        storeInst.operands.push_back(ptrOp);
        
        currentBlock_->instructions.push_back(storeInst);
    }
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
        }
        
        // (AR) تتبع نوع الصنف إذا كان التعبير جديد ClassName()
        // (EN) Track class type if expression is new ClassName()
        if (auto* newExpr = dynamic_cast<Sad::AST::NewExpr*>(varDecl->initializer.get())) {
            classInstanceTypes_[varDecl->name] = newExpr->className;
        }
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
            // (AR) القيمة ثابتة - تحويلها لثابت SIR
            // (EN) Value is constant - convert to SIR constant
            switch (initResult.type) {
                case SIRType::I64:
                    storeInst.operands.push_back(SIROperand::ConstantI64(std::stoll(initResult.constantValue)));
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
    
    // (AR) إضافة الكتل إلى الدالة الحالية
    // (EN) Add blocks to current function
    if (currentFunction_) {
        currentFunction_->addBasicBlock(thenBlock);
        if (elseBlock) currentFunction_->addBasicBlock(elseBlock);
        currentFunction_->addBasicBlock(mergeBlock);
    }
    
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
    // (AR) لا نضيف القفز إذا كان الفرع قد انتهى بـ RET أو BR أو BR_COND
    // (EN) Don't add branch if the block already ends with RET or BR or BR_COND
    SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
    SIRInstruction brMergeInst = SIRInstruction::Branch(mergeLabelOp);
    
    if (currentBlock_ && !currentBlock_->instructions.empty()) {
        const auto& lastInst = currentBlock_->instructions.back();
        bool hasTerminator = (lastInst.opcode == SIROpcode::RET || 
                             lastInst.opcode == SIROpcode::RET_VOID ||
                             lastInst.opcode == SIROpcode::BR ||
                             lastInst.opcode == SIROpcode::BR_COND);
        if (!hasTerminator) {
            currentBlock_->instructions.push_back(brMergeInst);
            std::cout << "[DEBUG] buildIfStatement: added BR to merge from then" << std::endl;
        } else {
            std::cout << "[DEBUG] buildIfStatement: then block already has terminator, skipping BR" << std::endl;
        }
    } else if (currentBlock_) {
        currentBlock_->instructions.push_back(brMergeInst);
        std::cout << "[DEBUG] buildIfStatement: added BR to merge from then (empty block)" << std::endl;
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
        // (AR) لا نضيف القفز إذا كان الفرع قد انتهى بـ RET أو BR أو BR_COND
        // (EN) Don't add branch if the block already ends with RET or BR or BR_COND
        if (currentBlock_ && !currentBlock_->instructions.empty()) {
            const auto& lastInst = currentBlock_->instructions.back();
            bool hasTerminator = (lastInst.opcode == SIROpcode::RET || 
                                 lastInst.opcode == SIROpcode::RET_VOID ||
                                 lastInst.opcode == SIROpcode::BR ||
                                 lastInst.opcode == SIROpcode::BR_COND);
            if (!hasTerminator) {
                currentBlock_->instructions.push_back(brMergeInst);
                std::cout << "[DEBUG] buildIfStatement: added BR to merge from else" << std::endl;
            } else {
                std::cout << "[DEBUG] buildIfStatement: else block already has terminator, skipping BR" << std::endl;
            }
        } else if (currentBlock_) {
            currentBlock_->instructions.push_back(brMergeInst);
            std::cout << "[DEBUG] buildIfStatement: added BR to merge from else (empty block)" << std::endl;
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
// buildMatchStatement - بناء جملة match (Pattern Matching)
// ============================================================================
// (AR) تحويل جملة match إلى سلسلة من BR_COND/BR باستخدام SIR الموجود
// (EN) Lower match statement to chain of BR_COND/BR using existing SIR
//
// (AR) الاستراتيجية:
// لكل case:
//   1. كتلة اختبار: مقارنة النمط مع القيمة
//   2. كتلة guard (إن وجد): تقييم الشرط الإضافي
//   3. كتلة الجسم: تنفيذ الكود
//   4. قفز إلى كتلة النهاية
//
// (EN) Strategy:
// For each case:
//   1. Test block: compare pattern with value
//   2. Guard block (if exists): evaluate guard condition
//   3. Body block: execute code
//   4. Jump to merge block
// ============================================================================
void SIRBuilder::buildMatchStatement(Sad::AST::MatchStmt* matchStmt) {
    if (!matchStmt) {
        return;
    }
    
    std::cout << "[DEBUG] buildMatchStatement: starting with " 
              << matchStmt->cases.size() << " cases" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 1: تقييم القيمة المُطابقة
    // (EN) Step 1: Evaluate the match value
    // ========================================================================
    auto matchResult = buildExpression(matchStmt->value.get());
    
    if (matchResult.registerName.empty() && !matchResult.isConstant) {
        errors_.push_back("Error: Failed to build match expression");
        return;
    }
    
    std::cout << "[DEBUG] buildMatchStatement: match value reg=" 
              << matchResult.registerName << ", isConst=" << matchResult.isConstant << std::endl;
    
    // (AR) إذا كانت القيمة ثابتة، نحتاج تحميلها في سجل
    // (EN) If value is constant, need to load it into a register
    std::string matchValueReg = matchResult.registerName;
    SIRType matchValueType = matchResult.type;
    
    if (matchResult.isConstant) {
        matchValueReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(matchValueReg, matchResult.type);
        SIROperand constOp;
        if (matchResult.type == SIRType::STRING) {
            constOp = SIROperand::ConstantString(matchResult.constantValue);
        } else if (matchResult.type == SIRType::F64) {
            constOp = SIROperand::ConstantF64(std::stod(matchResult.constantValue));
        } else if (matchResult.type == SIRType::BOOL) {
            constOp = SIROperand::ConstantBool(matchResult.constantValue == "true");
        } else {
            constOp = SIROperand::ConstantI64(std::stoll(matchResult.constantValue));
        }
        SIRInstruction moveInst(SIROpcode::MOVE);
        moveInst.result = resultOp;
        moveInst.operands = {constOp};
        if (currentBlock_) {
            currentBlock_->instructions.push_back(moveInst);
        }
    } else if (!matchResult.registerName.empty() && matchResult.registerName[0] == '%') {
        // (AR) القيمة في عنوان alloca (متغير)، نحتاج لتحميلها
        // (EN) Value is in alloca address (variable), need to LOAD it
        std::string loadedReg = newTempRegister();
        SIRInstruction loadInst;
        loadInst.opcode = SIROpcode::LOAD;
        loadInst.result = SIROperand::Register(loadedReg, matchValueType);
        loadInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
        if (currentBlock_) {
            currentBlock_->addInstruction(loadInst);
        }
        matchValueReg = loadedReg;
    }
    
    // ========================================================================
    // (AR) الخطوة 2: إنشاء كتلة النهاية
    // (EN) Step 2: Create merge block
    // ========================================================================
    std::string mergeLabel = newLabel("match.end");
    auto mergeBlock = createBasicBlock(mergeLabel);
    if (currentFunction_) {
        currentFunction_->addBasicBlock(mergeBlock);
    }
    
    // ========================================================================
    // (AR) الخطوة 3: إنشاء كتل لكل case
    // (EN) Step 3: Create blocks for each case
    // ========================================================================
    struct CaseBlockInfo {
        std::string testLabel;
        std::string bodyLabel;
        std::string guardLabel;
        std::shared_ptr<SIRBasicBlock> testBlock;
        std::shared_ptr<SIRBasicBlock> bodyBlock;
        std::shared_ptr<SIRBasicBlock> guardBlock;
    };
    
    std::vector<CaseBlockInfo> caseBlocks;
    
    for (size_t i = 0; i < matchStmt->cases.size(); ++i) {
        CaseBlockInfo info;
        info.testLabel = newLabel("match.case" + std::to_string(i) + ".test");
        info.bodyLabel = newLabel("match.case" + std::to_string(i) + ".body");
        info.testBlock = createBasicBlock(info.testLabel);
        info.bodyBlock = createBasicBlock(info.bodyLabel);
        
        if (currentFunction_) {
            currentFunction_->addBasicBlock(info.testBlock);
            currentFunction_->addBasicBlock(info.bodyBlock);
        }
        
        // (AR) كتلة guard إن وُجد
        // (EN) Guard block if exists
        if (matchStmt->cases[i].guard) {
            info.guardLabel = newLabel("match.case" + std::to_string(i) + ".guard");
            info.guardBlock = createBasicBlock(info.guardLabel);
            if (currentFunction_) {
                currentFunction_->addBasicBlock(info.guardBlock);
            }
        }
        
        caseBlocks.push_back(std::move(info));
    }
    
    // ========================================================================
    // (AR) الخطوة 4: القفز للـ case الأول
    // (EN) Step 4: Jump to first case
    // ========================================================================
    if (!caseBlocks.empty()) {
        SIROperand firstLabel = SIROperand::Label(caseBlocks[0].testLabel);
        SIRInstruction brFirst = SIRInstruction::Branch(firstLabel);
        if (currentBlock_) {
            currentBlock_->instructions.push_back(brFirst);
        }
    } else {
        // (AR) لا يوجد cases، القفز مباشرة للنهاية
        // (EN) No cases, jump directly to merge
        SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
        SIRInstruction brMerge = SIRInstruction::Branch(mergeLabelOp);
        if (currentBlock_) {
            currentBlock_->instructions.push_back(brMerge);
        }
        currentBlock_ = mergeBlock;
        return;
    }
    
    // ========================================================================
    // (AR) الخطوة 5: توليد كود كل case
    // (EN) Step 5: Generate code for each case
    // ========================================================================
    for (size_t i = 0; i < matchStmt->cases.size(); ++i) {
        const auto& caseClause = matchStmt->cases[i];
        auto& info = caseBlocks[i];
        
        // (AR) تحديد الـ case التالي للقفز عند الفشل
        // (EN) Determine next case to jump to on failure
        std::string nextLabel = (i + 1 < caseBlocks.size()) 
            ? caseBlocks[i + 1].testLabel 
            : mergeLabel;
        
        // === كتلة الاختبار / Test block ===
        currentBlock_ = info.testBlock;
        
        std::string condReg;
        
        if (caseClause.pattern) {
            // (AR) تحديد نوع النمط وتوليد الاختبار المناسب
            // (EN) Determine pattern type and generate appropriate test
            
            if (dynamic_cast<const Sad::AST::WildcardPattern*>(caseClause.pattern.get())) {
                // (AR) النمط الشامل _ - دائماً true
                // (EN) Wildcard _ - always true
                condReg = newTempRegister();
                SIROperand resultOp = SIROperand::Register(condReg, SIRType::BOOL);
                SIROperand trueOp = SIROperand::ConstantBool(true);
                SIRInstruction moveInst(SIROpcode::MOVE);
                moveInst.result = resultOp;
                moveInst.operands = {trueOp};
                currentBlock_->instructions.push_back(moveInst);
                
                std::cout << "[DEBUG] buildMatchStatement: case " << i << " is WildcardPattern" << std::endl;
            }
            else if (auto* varPat = dynamic_cast<const Sad::AST::VariablePattern*>(caseClause.pattern.get())) {
                // (AR) نمط متغير - دائماً true ويربط القيمة
                // (EN) Variable pattern - always true and binds value
                condReg = newTempRegister();
                SIROperand resultOp = SIROperand::Register(condReg, SIRType::BOOL);
                SIROperand trueOp = SIROperand::ConstantBool(true);
                SIRInstruction moveInst(SIROpcode::MOVE);
                moveInst.result = resultOp;
                moveInst.operands = {trueOp};
                currentBlock_->instructions.push_back(moveInst);
                
                // (AR) ربط المتغير: حجز + تخزين
                // (EN) Bind variable: alloc + store
                std::string varReg = newTempRegister();
                
                // (AR) إضافة المتغير إلى النطاق باسم النمط
                // (EN) Add variable to scope with pattern name
                VariableInfo varInfo;
                varInfo.name = varPat->name;
                varInfo.type = matchValueType;
                varInfo.registerName = matchValueReg;
                varInfo.isGlobal = false;
                varInfo.isMutable = false;
                varInfo.scopeLevel = currentScopeLevel_;
                addVariable(varInfo);
                
                std::cout << "[DEBUG] buildMatchStatement: case " << i 
                          << " is VariablePattern(" << varPat->name << ")" << std::endl;
            }
            else if (auto* litPat = dynamic_cast<const Sad::AST::LiteralPattern*>(caseClause.pattern.get())) {
                // (AR) نمط قيمة حرفية - مقارنة EQ
                // (EN) Literal pattern - EQ comparison
                condReg = newTempRegister();
                
                // (AR) تحديد قيمة الثابت
                // (EN) Determine constant value
                std::string litValue;
                SIRType litType = matchValueType;
                
                const auto& lit = litPat->literal;
                if (lit.isInteger()) {
                    litValue = std::to_string(lit.toInt());
                    litType = SIRType::I64;
                } else if (lit.getType() == Data::ValueType::DOUBLE) {
                    litValue = std::to_string(lit.toDouble());
                    litType = SIRType::F64;
                } else if (lit.getType() == Data::ValueType::BOOLEAN) {
                    litValue = lit.toBool() ? "true" : "false";
                    litType = SIRType::BOOL;
                } else if (lit.getType() == Data::ValueType::STRING) {
                    litValue = lit.toString();
                    litType = SIRType::STRING;
                } else {
                    litValue = "0";
                    litType = SIRType::I64;
                }
                
                // (AR) تحويل النوع ليطابق نوع القيمة المُطابقة
                // (EN) Coerce literal type to match the match value type
                if (matchValueType == SIRType::I64 && litType == SIRType::F64) {
                    // (AR) تحويل من عشري إلى صحيح
                    // (EN) Convert from float to integer
                    litValue = std::to_string(static_cast<int64_t>(lit.toDouble()));
                    litType = SIRType::I64;
                } else if (matchValueType == SIRType::F64 && litType == SIRType::I64) {
                    // (AR) تحويل من صحيح إلى عشري
                    // (EN) Convert from integer to float
                    litValue = std::to_string(static_cast<double>(lit.toInt()));
                    litType = SIRType::F64;
                }
                
                // (AR) تحميل الثابت في سجل
                // (EN) Load constant into register
                std::string litReg = newTempRegister();
                SIROperand litResultOp = SIROperand::Register(litReg, litType);
                SIROperand litConstOp;
                if (litType == SIRType::STRING) {
                    litConstOp = SIROperand::ConstantString(litValue);
                } else if (litType == SIRType::F64) {
                    litConstOp = SIROperand::ConstantF64(std::stod(litValue));
                } else if (litType == SIRType::BOOL) {
                    litConstOp = SIROperand::ConstantBool(litValue == "true");
                } else {
                    litConstOp = SIROperand::ConstantI64(std::stoll(litValue));
                }
                SIRInstruction moveLit(SIROpcode::MOVE);
                moveLit.result = litResultOp;
                moveLit.operands = {litConstOp};
                currentBlock_->instructions.push_back(moveLit);
                
                // (AR) مقارنة القيمتين
                // (EN) Compare values
                SIROperand matchOp = SIROperand::Register(matchValueReg, matchValueType);
                SIROperand cmpLitOp = SIROperand::Register(litReg, litType);
                SIROperand cmpResultOp = SIROperand::Register(condReg, SIRType::BOOL);
                SIRInstruction cmpInst = SIRInstruction::Binary(SIROpcode::EQ, cmpResultOp, matchOp, cmpLitOp);
                currentBlock_->instructions.push_back(cmpInst);
                
                std::cout << "[DEBUG] buildMatchStatement: case " << i 
                          << " is LiteralPattern(" << litValue << ")" << std::endl;
            }
            else if (auto* orPat = dynamic_cast<const Sad::AST::OrPattern*>(caseClause.pattern.get())) {
                // (AR) نمط OR - سلسلة مقارنات مع OR
                // (EN) OR pattern - chain comparisons with OR
                condReg = newTempRegister();
                
                // (AR) بدء بـ false
                // (EN) Start with false
                std::string accumReg = newTempRegister();
                SIROperand accResultOp = SIROperand::Register(accumReg, SIRType::BOOL);
                SIROperand falseOp = SIROperand::ConstantBool(false);
                SIRInstruction moveInit(SIROpcode::MOVE);
                moveInit.result = accResultOp;
                moveInit.operands = {falseOp};
                currentBlock_->instructions.push_back(moveInit);
                
                for (const auto& alt : orPat->alternatives) {
                    if (auto* altLit = dynamic_cast<const Sad::AST::LiteralPattern*>(alt.get())) {
                        // (AR) مقارنة مع كل بديل
                        // (EN) Compare with each alternative
                        std::string altLitValue;
                        SIRType altLitType = matchValueType;
                        
                        const auto& altVal = altLit->literal;
                        if (altVal.isInteger()) {
                            altLitValue = std::to_string(altVal.toInt());
                            altLitType = SIRType::I64;
                        } else if (altVal.getType() == Data::ValueType::DOUBLE) {
                            altLitType = SIRType::F64;
                            altLitValue = std::to_string(altVal.toDouble());
                        } else if (altVal.getType() == Data::ValueType::BOOLEAN) {
                            altLitValue = altVal.toBool() ? "true" : "false";
                            altLitType = SIRType::BOOL;
                        } else if (altVal.getType() == Data::ValueType::STRING) {
                            altLitValue = altVal.toString();
                            altLitType = SIRType::STRING;
                        } else {
                            altLitValue = altVal.toString();
                        }
                        
                        // (AR) تحويل النوع ليطابق نوع القيمة المُطابقة
                        // (EN) Coerce type to match the match value type
                        if (matchValueType == SIRType::I64 && altLitType == SIRType::F64) {
                            altLitValue = std::to_string(static_cast<int64_t>(altVal.toDouble()));
                            altLitType = SIRType::I64;
                        } else if (matchValueType == SIRType::F64 && altLitType == SIRType::I64) {
                            altLitValue = std::to_string(static_cast<double>(altVal.toInt()));
                            altLitType = SIRType::F64;
                        }
                        
                        std::string altReg = newTempRegister();
                        SIROperand altResultOp = SIROperand::Register(altReg, altLitType);
                        SIROperand altConstOp;
                        if (altLitType == SIRType::STRING) {
                            altConstOp = SIROperand::ConstantString(altLitValue);
                        } else if (altLitType == SIRType::F64) {
                            altConstOp = SIROperand::ConstantF64(std::stod(altLitValue));
                        } else if (altLitType == SIRType::BOOL) {
                            altConstOp = SIROperand::ConstantBool(altLitValue == "true");
                        } else {
                            altConstOp = SIROperand::ConstantI64(std::stoll(altLitValue));
                        }
                        SIRInstruction moveAlt(SIROpcode::MOVE);
                        moveAlt.result = altResultOp;
                        moveAlt.operands = {altConstOp};
                        currentBlock_->instructions.push_back(moveAlt);
                        
                        std::string cmpAltReg = newTempRegister();
                        SIROperand matchOp = SIROperand::Register(matchValueReg, matchValueType);
                        SIROperand cmpAltOp = SIROperand::Register(altReg, altLitType);
                        SIROperand cmpAltResultOp = SIROperand::Register(cmpAltReg, SIRType::BOOL);
                        SIRInstruction cmpAlt = SIRInstruction::Binary(SIROpcode::EQ, cmpAltResultOp, matchOp, cmpAltOp);
                        currentBlock_->instructions.push_back(cmpAlt);
                        
                        // (AR) دمج مع النتيجة المتراكمة
                        // (EN) OR with accumulated result
                        std::string newAccumReg = newTempRegister();
                        SIROperand prevAccOp = SIROperand::Register(accumReg, SIRType::BOOL);
                        SIROperand cmpResOp = SIROperand::Register(cmpAltReg, SIRType::BOOL);
                        SIROperand newAccResultOp = SIROperand::Register(newAccumReg, SIRType::BOOL);
                        SIRInstruction orInst = SIRInstruction::Binary(SIROpcode::OR, newAccResultOp, prevAccOp, cmpResOp);
                        currentBlock_->instructions.push_back(orInst);
                        
                        accumReg = newAccumReg;
                    }
                }
                
                // (AR) نقل النتيجة النهائية إلى condReg
                // (EN) Move final result to condReg
                SIROperand finalResultOp = SIROperand::Register(condReg, SIRType::BOOL);
                SIROperand finalAccOp = SIROperand::Register(accumReg, SIRType::BOOL);
                SIRInstruction moveFinal(SIROpcode::MOVE);
                moveFinal.result = finalResultOp;
                moveFinal.operands = {finalAccOp};
                currentBlock_->instructions.push_back(moveFinal);
                
                std::cout << "[DEBUG] buildMatchStatement: case " << i << " is OrPattern" << std::endl;
            }
            else {
                // (AR) نمط غير معروف - نعتبره true (يطابق أي شيء)
                // (EN) Unknown pattern - treat as true (matches anything)
                condReg = newTempRegister();
                SIROperand resultOp = SIROperand::Register(condReg, SIRType::BOOL);
                SIROperand trueOp = SIROperand::ConstantBool(true);
                SIRInstruction moveInst(SIROpcode::MOVE);
                moveInst.result = resultOp;
                moveInst.operands = {trueOp};
                currentBlock_->instructions.push_back(moveInst);
                
                std::cout << "[DEBUG] buildMatchStatement: case " << i << " is unknown pattern" << std::endl;
            }
        }   // end if (caseClause.pattern)
        
        // === التفريع / Branching ===
        if (caseClause.guard && !info.guardLabel.empty()) {
            // (AR) القفز إلى كتلة guard عند نجاح الاختبار
            // (EN) Jump to guard block on test success
            SIROperand condOp = SIROperand::Register(condReg, SIRType::BOOL);
            SIROperand guardLabelOp = SIROperand::Label(info.guardLabel);
            SIROperand nextLabelOp = SIROperand::Label(nextLabel);
            SIRInstruction brCond = SIRInstruction::BranchCond(condOp, guardLabelOp, nextLabelOp);
            currentBlock_->instructions.push_back(brCond);
            
            // === كتلة guard / Guard block ===
            currentBlock_ = info.guardBlock;
            enterScope();
            
            // (AR) ربط المتغيرات من النمط قبل تقييم الشرط
            // (EN) Bind pattern variables before evaluating guard
            if (auto* varPat = dynamic_cast<const Sad::AST::VariablePattern*>(caseClause.pattern.get())) {
                VariableInfo guardVar;
                guardVar.name = varPat->name;
                guardVar.type = matchValueType;
                guardVar.registerName = matchValueReg;
                guardVar.isGlobal = false;
                guardVar.isMutable = false;
                guardVar.scopeLevel = currentScopeLevel_;
                addVariable(guardVar);
            }
            
            auto guardResult = buildExpression(caseClause.guard.get());
            
            std::string guardCondReg = guardResult.registerName;
            if (guardResult.isConstant) {
                guardCondReg = newTempRegister();
                SIROperand gResultOp = SIROperand::Register(guardCondReg, guardResult.type);
                SIROperand gConstOp;
                if (guardResult.type == SIRType::BOOL) {
                    gConstOp = SIROperand::ConstantBool(guardResult.constantValue == "true");
                } else if (guardResult.type == SIRType::I64) {
                    gConstOp = SIROperand::ConstantI64(std::stoll(guardResult.constantValue));
                } else {
                    gConstOp = SIROperand::ConstantI64(guardResult.constantValue == "true" ? 1 : 0);
                }
                SIRInstruction moveG(SIROpcode::MOVE);
                moveG.result = gResultOp;
                moveG.operands = {gConstOp};
                currentBlock_->instructions.push_back(moveG);
            }
            
            SIROperand guardCondOp = SIROperand::Register(guardCondReg, SIRType::BOOL);
            SIROperand bodyLabelOp = SIROperand::Label(info.bodyLabel);
            SIROperand nextLabelOp2 = SIROperand::Label(nextLabel);
            SIRInstruction brGuard = SIRInstruction::BranchCond(guardCondOp, bodyLabelOp, nextLabelOp2);
            currentBlock_->instructions.push_back(brGuard);
            
            exitScope();
        } else {
            // (AR) بدون guard: القفز مباشرة إلى الجسم أو التالي
            // (EN) No guard: jump directly to body or next
            SIROperand condOp = SIROperand::Register(condReg, SIRType::BOOL);
            SIROperand bodyLabelOp = SIROperand::Label(info.bodyLabel);
            SIROperand nextLabelOp = SIROperand::Label(nextLabel);
            SIRInstruction brCond = SIRInstruction::BranchCond(condOp, bodyLabelOp, nextLabelOp);
            currentBlock_->instructions.push_back(brCond);
        }
        
        // === كتلة الجسم / Body block ===
        currentBlock_ = info.bodyBlock;
        enterScope();
        
        // (AR) ربط المتغيرات من النمط في الجسم
        // (EN) Bind pattern variables in body scope
        if (auto* varPat = dynamic_cast<const Sad::AST::VariablePattern*>(caseClause.pattern.get())) {
            VariableInfo bodyVar;
            bodyVar.name = varPat->name;
            bodyVar.type = matchValueType;
            bodyVar.registerName = matchValueReg;
            bodyVar.isGlobal = false;
            bodyVar.isMutable = false;
            bodyVar.scopeLevel = currentScopeLevel_;
            addVariable(bodyVar);
        }
        
        // (AR) تنفيذ جسم الحالة
        // (EN) Execute case body
        for (auto& bodyStmt : caseClause.body) {
            buildStatement(bodyStmt.get());
        }
        
        exitScope();
        
        // (AR) قفز غير مشروط إلى كتلة النهاية
        // (EN) Unconditional jump to merge block
        SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
        SIRInstruction brMerge = SIRInstruction::Branch(mergeLabelOp);
        if (currentBlock_) {
            currentBlock_->instructions.push_back(brMerge);
        }
        
        std::cout << "[DEBUG] buildMatchStatement: generated case " << i << std::endl;
    }   // end for each case
    
    // ========================================================================
    // (AR) الخطوة 6: الاستمرار بعد match
    // (EN) Step 6: Continue after match statement
    // ========================================================================
    currentBlock_ = mergeBlock;
    std::cout << "[DEBUG] buildMatchStatement: completed" << std::endl;
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
    
    // (AR) إضافة الكتل إلى الدالة الحالية
    // (EN) Add blocks to current function
    if (currentFunction_) {
        currentFunction_->addBasicBlock(condBlock);
        currentFunction_->addBasicBlock(bodyBlock);
        currentFunction_->addBasicBlock(exitBlock);
    }
    
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
    // (AR) لا نضيف القفز إذا كان الجسم قد انتهى بـ RET أو BR أو BR_COND
    // (EN) Don't add branch if the body already ends with RET or BR or BR_COND
    // ========================================================================
    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_ && !currentBlock_->instructions.empty()) {
        const auto& lastInst = currentBlock_->instructions.back();
        bool hasTerminator = (lastInst.opcode == SIROpcode::RET || 
                             lastInst.opcode == SIROpcode::RET_VOID ||
                             lastInst.opcode == SIROpcode::BR ||
                             lastInst.opcode == SIROpcode::BR_COND);
        if (!hasTerminator) {
            currentBlock_->instructions.push_back(brBackInst);
            std::cout << "[DEBUG] buildWhileLoop: added BR back to condition" << std::endl;
        } else {
            std::cout << "[DEBUG] buildWhileLoop: body block already has terminator, skipping BR" << std::endl;
        }
    } else if (currentBlock_) {
        currentBlock_->instructions.push_back(brBackInst);
        std::cout << "[DEBUG] buildWhileLoop: added BR back to condition (empty block)" << std::endl;
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
    
    // (AR) إضافة الكتل إلى الدالة الحالية
    // (EN) Add blocks to current function
    if (currentFunction_) {
        currentFunction_->addBasicBlock(condBlock);
        currentFunction_->addBasicBlock(bodyBlock);
        currentFunction_->addBasicBlock(incBlock);
        currentFunction_->addBasicBlock(exitBlock);
    }
    
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
    
    // (AR) إضافة الكتل إلى الدالة الحالية
    // (EN) Add blocks to current function
    if (currentFunction_) {
        currentFunction_->addBasicBlock(condBlock);
        currentFunction_->addBasicBlock(bodyBlock);
        currentFunction_->addBasicBlock(incBlock);
        currentFunction_->addBasicBlock(exitBlock);
    }
    
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
    
    // =====================================================================
    // (AR) البحث في المتغيرات العامة على مستوى الوحدة
    // (EN) Fallback: search in module-level global variables
    // عندما لا يُعثر على المتغير في النطاقات المحلية، نبحث في الوحدة
    // When not found in local scopes, search in module globals
    // =====================================================================
    if (module_) {
        auto globalVar = module_->getGlobalVariable(name);
        if (globalVar) {
            // (AR) إنشاء VariableInfo للمتغير العام وتخزينه في أول نطاق
            // (EN) Create VariableInfo for global var and cache in first scope
            VariableInfo globalInfo;
            globalInfo.name = name;
            globalInfo.type = globalVar->getType();
            globalInfo.registerName = name;  // use same name as LLVM global
            globalInfo.isGlobal = true;
            globalInfo.isMutable = !globalVar->getIsConstant();
            globalInfo.scopeLevel = 0;
            
            // (AR) تخزين في أقدم نطاق للبحث السريع لاحقاً
            // (EN) Cache in oldest scope for fast future lookups
            if (!scopeStack_.empty()) {
                scopeStack_.front()[name] = globalInfo;
                return &scopeStack_.front()[name];
            }
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
        // (AR) بعد الإسناد، نرجع معلومات المتغير المُسند إليه
        // (EN) After assignment, return the assigned variable info
        VariableInfo* varInfo = lookupVariable(assignExpr->name);
        if (varInfo) {
            return BuildResult(varInfo->registerName, varInfo->type);
        }
        return BuildResult();
    }
    
    // (AR) NewExpr - إنشاء كائن جديد (class_nodes.h:164)
    // (EN) New object creation
    if (auto newExpr = dynamic_cast<Sad::AST::NewExpr*>(expr)) {
        return buildNewObject(newExpr);
    }
    
    // (AR) MemberExpr - الوصول لعضو في كائن (expressions.h:406)
    // (EN) Member access via MemberExpr (from parser)
    if (auto memberExpr = dynamic_cast<Sad::AST::MemberExpr*>(expr)) {
        std::cout << "[DEBUG] buildExpression: found MemberExpr for member '"
                  << memberExpr->member << "'" << std::endl;
        
        // (AR) الخطوة 1: بناء تعبير الكائن
        // (EN) Step 1: Build object expression
        auto objResult = buildExpression(memberExpr->object.get());
        
        // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو
        // (EN) Step 2: Create member access instruction
        std::string resultReg = newTempRegister();
        
        if (currentBlock_) {
            SIRInstruction loadInst;
            loadInst.opcode = SIROpcode::LOAD;
            loadInst.result = SIROperand::Register(resultReg, SIRType::I64);
            
            // (AR) المعامل الأول: الكائن
            // (EN) First operand: object
            loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
            
            // (AR) المعامل الثاني: اسم العضو
            // (EN) Second operand: member name
            loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->member));
            
            currentBlock_->addInstruction(loadInst);
        }
        
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) MemberAccessExpr - الوصول لعضو في كائن (class_nodes.h:206)
    // (EN) Member access
    if (auto memberExpr = dynamic_cast<Sad::AST::MemberAccessExpr*>(expr)) {
        return buildMemberAccess(memberExpr);
    }
    
    // (AR) MethodCallExpr - استدعاء طريقة على كائن (class_nodes.h:245)
    // (EN) Method call on object
    if (auto methodCallExpr = dynamic_cast<Sad::AST::MethodCallExpr*>(expr)) {
        return buildMethodCall(methodCallExpr);
    }
    
    // (AR) ThisExpr - مرجع ذاتي 'هذا' (class_nodes.h:288)
    // (EN) This/self reference
    if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr*>(expr)) {
        // (AR) 'هذا' يشير إلى self في سياق الصنف
        // (EN) 'this' refers to self in class context
        VariableInfo* selfInfo = lookupVariable("self");
        if (selfInfo) {
            return BuildResult(selfInfo->registerName, selfInfo->type);
        }
        return BuildResult("%self", SIRType::I64);
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
        // (AR) تحويل الأعداد الست عشرية/الثمانية/الثنائية إلى عشرية
        //      لأن std::stoll() الافتراضي يستخدم أساس 10 فقط
        // (EN) Normalize hex/octal/binary literals to decimal strings
        //      because downstream std::stoll() uses base 10 by default
        if (value.size() > 2 && value[0] == '0') {
            char prefix = value[1];
            if (prefix == 'x' || prefix == 'X') {
                // Hex: 0x3F8 → "1016", 0xFFFF800000000000 → "-140737488355328"
                result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
            } else if (prefix == 'o' || prefix == 'O') {
                // Octal: 0o755 → "493"
                result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
            } else if (prefix == 'b' || prefix == 'B') {
                // Binary: 0b1010 → "10"
                result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
            }
        }
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
        
        // (AR) إنشاء SIROperand للمعامل (sir_types.h:355-362 - Register() أو Constant*)
        // (EN) Create SIROperand for argument
        SIROperand argOp;
        
        // (AR) إذا كان المعامل ثابتاً، استخدم Constant* factory methods
        // (EN) If argument is constant, use Constant* factory methods
        if (argResult.isConstant) {
            switch (argResult.type) {
                case SIRType::STRING:
                    // استخدام ConstantString (sir_types.h:353-361)
                    argOp = SIROperand::ConstantString(argResult.constantValue);
                    std::cout << "[DEBUG] buildFunctionCall: added STRING constant='" 
                              << argResult.constantValue << "'" << std::endl;
                    break;
                case SIRType::I64: {
                    int64_t intVal = std::stoll(argResult.constantValue);
                    argOp = SIROperand::ConstantI64(intVal);
                    std::cout << "[DEBUG] buildFunctionCall: added I64 constant=" 
                              << intVal << std::endl;
                    break;
                }
                case SIRType::F64: {
                    double floatVal = std::stod(argResult.constantValue);
                    argOp = SIROperand::ConstantF64(floatVal);
                    std::cout << "[DEBUG] buildFunctionCall: added F64 constant=" 
                              << floatVal << std::endl;
                    break;
                }
                case SIRType::BOOL: {
                    bool boolVal = (argResult.constantValue == "true");
                    argOp = SIROperand::ConstantBool(boolVal);
                    std::cout << "[DEBUG] buildFunctionCall: added BOOL constant=" 
                              << boolVal << std::endl;
                    break;
                }
                default:
                    // للأنواع الأخرى، استخدم Register كافتراضي
                    argOp = SIROperand::Register(argResult.registerName, argResult.type);
                    std::cout << "[DEBUG] buildFunctionCall: added register='" 
                              << argResult.registerName << "', type=" 
                              << static_cast<int>(argResult.type) << std::endl;
                    break;
            }
        } else {
            // (AR) للمتغيرات، استخدم Register
            // (EN) For variables, use Register
            argOp = SIROperand::Register(argResult.registerName, argResult.type);
            std::cout << "[DEBUG] buildFunctionCall: added register='" 
                      << argResult.registerName << "', type=" 
                      << static_cast<int>(argResult.type) << std::endl;
        }
        
        argOperands.push_back(argOp);
    }
    
    // ========================================================================
    // (AR) الخطوة 2.5: التحقق من الدوال المضمنة
    // (EN) Step 2.5: Check for builtin functions
    // ========================================================================
    
    // ========================================================================
    // (AR) إصلاح: تحقق أولاً إذا كانت الدالة معرّفة من المستخدم
    // (EN) Fix: First check if function is user-defined before checking builtins
    // هذا يمنع الدوال المضمنة من التداخل مع دوال المستخدم التي تحمل نفس الاسم
    // This prevents builtins from shadowing user-defined functions with same name
    // ========================================================================
    bool isUserDefinedFunction = (functionTable_.find(funcName) != functionTable_.end());
    
    // (AR) دالة طول() - STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
    // (EN) length() function - STRING_LEN for strings, ARRAY_LEN for arrays
    if (!isUserDefinedFunction && (funcName == "طول" || funcName == "length")) {
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
    // الأسماء المدعومة: لرقم, حول_رقم, to_int, int, إلى_رقم
    if (funcName == "لرقم" || funcName == "حول_رقم" || funcName == "to_int" || funcName == "int" || funcName == "إلى_رقم") {
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
    
    // (AR) دالة اطبع_سطر() - BUILTIN_PRINTLN
    // (EN) println() function - print with newline
    // الأسماء المدعومة: اطبع_سطر, println, طبع_سطر
    if (funcName == "اطبع_سطر" || funcName == "println" || funcName == "طبع_سطر") {
        // طباعة المعاملات أولاً
        SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);
        printInst.operands = argOperands;
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(printInst);
        }
        
        // ثم إضافة سطر جديد
        SIRInstruction newlineInst(SIROpcode::BUILTIN_PRINT);
        newlineInst.operands.push_back(SIROperand::ConstantString("\n"));
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(newlineInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin اطبع_سطر()" << std::endl;
        return BuildResult("", SIRType::VOID);
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
    // (AR) دوال رياضية - Math Functions
    // (EN) Math functions
    // ========================================================================
    
    // (AR) دالة جذر() - sqrt
    // (EN) sqrt() function - Square root
    // الأسماء المدعومة: جذر, sqrt, الجذر_التربيعي
    if (funcName == "جذر" || funcName == "sqrt" || funcName == "الجذر_التربيعي") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جذر تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction sqrtInst(SIROpcode::BUILTIN_SQRT);
        sqrtInst.result = resultOp;
        sqrtInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(sqrtInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin جذر() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة أس() - power
    // (EN) power() function - Base^Exponent
    // الأسماء المدعومة: أس, power, pow
    if (funcName == "أس" || funcName == "power" || funcName == "pow") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة أس تتطلب معاملين (الأساس والأس)" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction powInst(SIROpcode::BUILTIN_POW);
        powInst.result = resultOp;
        powInst.operands.push_back(argOperands[0]); // base
        powInst.operands.push_back(argOperands[1]); // exponent
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(powInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin أس() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة مطلق() - abs
    // (EN) abs() function - Absolute value
    // الأسماء المدعومة: مطلق, abs, القيمة_المطلقة, absolute
    if (funcName == "مطلق" || funcName == "abs" || funcName == "القيمة_المطلقة" || funcName == "absolute") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة مطلق تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIRType resultType = argResults[0].type; // preserve type (I64 or F64)
        SIROperand resultOp = SIROperand::Register(resultReg, resultType);
        
        SIRInstruction absInst(SIROpcode::BUILTIN_ABS);
        absInst.result = resultOp;
        absInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(absInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin مطلق() -> " << resultReg << std::endl;
        return BuildResult(resultReg, resultType);
    }
    
    // (AR) دالة تقريب() - round
    // (EN) round() function
    // الأسماء المدعومة: تقريب, round
    if (funcName == "تقريب" || funcName == "round") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة تقريب تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction roundInst(SIROpcode::BUILTIN_ROUND);
        roundInst.result = resultOp;
        roundInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(roundInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin تقريب() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة أرضية() - floor
    // (EN) floor() function
    // الأسماء المدعومة: أرضية, floor
    if (funcName == "أرضية" || funcName == "floor") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة أرضية تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction floorInst(SIROpcode::BUILTIN_FLOOR);
        floorInst.result = resultOp;
        floorInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(floorInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin أرضية() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة سقف() - ceil
    // (EN) ceil() function
    // الأسماء المدعومة: سقف, ceil, ceiling
    if (funcName == "سقف" || funcName == "ceil" || funcName == "ceiling") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة سقف تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction ceilInst(SIROpcode::BUILTIN_CEIL);
        ceilInst.result = resultOp;
        ceilInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(ceilInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin سقف() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة مربع() - square
    // (EN) square() function - x^2
    // الأسماء المدعومة: مربع, square
    if (funcName == "مربع" || funcName == "square") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة مربع تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction sqInst(SIROpcode::BUILTIN_POW);
        sqInst.result = resultOp;
        sqInst.operands.push_back(argOperands[0]);
        sqInst.operands.push_back(SIROperand::ConstantF64(2.0));
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(sqInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin مربع() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة جيب() - sin
    // (EN) sin() function - sine
    // الأسماء المدعومة: جيب, sin, sine
    if (funcName == "جيب" || funcName == "sin" || funcName == "sine") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جيب تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction sinInst(SIROpcode::BUILTIN_SIN);
        sinInst.result = resultOp;
        sinInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(sinInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin جيب() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة جيب_تمام() - cos
    // (EN) cos() function - cosine
    // الأسماء المدعومة: جيب_تمام, cos, cosine
    if (funcName == "جيب_تمام" || funcName == "cos" || funcName == "cosine") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جيب_تمام تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction cosInst(SIROpcode::BUILTIN_COS);
        cosInst.result = resultOp;
        cosInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(cosInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin جيب_تمام() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة ظل() - tan
    // (EN) tan() function - tangent
    // الأسماء المدعومة: ظل, tan, tangent
    if (funcName == "ظل" || funcName == "tan" || funcName == "tangent") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة ظل تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction tanInst(SIROpcode::BUILTIN_TAN);
        tanInst.result = resultOp;
        tanInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(tanInst);
        }
        
        std::cout << "[DEBUG] buildFunctionCall: builtin ظل() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // ========================================================================
    // String Functions (12 functions)
    // ========================================================================
    
    // 1. طول_نص / string_length
    if (funcName == "طول_نص" || funcName == "string_length" || funcName == "str_length" || 
        funcName == "نص_طول" || funcName == "نص_الطول") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة طول_نص تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 1b. رمز_حرف / char_at - get character at index
    if (funcName == "رمز_حرف" || funcName == "char_at" || funcName == "charAt") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة رمز_حرف تتطلب معاملين (نص, فهرس)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CHAR_AT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);  // string
        inst.operands.push_back(argOperands[1]);  // index
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 2. تحويل_كبير / toUpper
    if (funcName == "تحويل_كبير" || funcName == "toUpper" || funcName == "uppercase" || funcName == "لأكبر") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة تحويل_كبير تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_UPPER);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 3. تحويل_صغير / toLower
    if (funcName == "تحويل_صغير" || funcName == "toLower" || funcName == "lowercase" || funcName == "لأصغر") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة تحويل_صغير تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_LOWER);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 4. بحث / find
    if (funcName == "بحث" || funcName == "find" || funcName == "indexOf" || funcName == "ابحث") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة بحث تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_FIND);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 5. استبدل / replace
    if (funcName == "استبدل" || funcName == "replace" || funcName == "بدل") {
        if (argResults.size() < 3) {
            std::cerr << "[Error] دالة استبدل تتطلب 3 معاملات (نص, قديم, جديد)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 6. استخراج / substring
    if (funcName == "استخراج" || funcName == "substring" || funcName == "substr" || funcName == "slice") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة استخراج تتطلب على الأقل معاملين (نص, بداية)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]); // string
        inst.operands.push_back(argOperands[1]); // start
        if (argResults.size() >= 3) {
            inst.operands.push_back(argOperands[2]); // end (optional)
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 7. قص_أطراف / trim
    if (funcName == "قص_أطراف" || funcName == "trim" || funcName == "strip") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة قص_أطراف تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 8. تقسيم / split (returns array)
    if (funcName == "تقسيم" || funcName == "split") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة تقسيم تتطلب معاملين (نص, فاصل)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // 9. دمج / join (array to string)
    if (funcName == "دمج" || funcName == "join") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة دمج تتطلب معاملين (مصفوفة, فاصل)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_JOIN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 10. يبدأ_ب / startsWith
    if (funcName == "يبدأ_ب" || funcName == "startsWith" || funcName == "starts_with") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة يبدأ_ب تتطلب معاملين (نص, بادئة)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 11. ينتهي_ب / endsWith
    if (funcName == "ينتهي_ب" || funcName == "endsWith" || funcName == "ends_with") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة ينتهي_ب تتطلب معاملين (نص, لاحقة)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 12. يحتوي_على / contains
    if (funcName == "يحتوي_على" || funcName == "contains" || funcName == "includes") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة يحتوي_على تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }

    // ========================================================================
    // Array Functions (10 functions)
    // ========================================================================
    
    // 1. إضافة_عنصر / append
    if (funcName == "إضافة_عنصر" || funcName == "append" || funcName == "push" || funcName == "add") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة إضافة_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 2. إزالة_عنصر / remove
    if (funcName == "إزالة_عنصر" || funcName == "remove" || funcName == "delete" || funcName == "pop") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة إزالة_عنصر تتطلب معاملين (مصفوفة, فهرس)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 3. حجم_مصفوفة / array_size / length
    if (funcName == "حجم_مصفوفة" || funcName == "array_size" || funcName == "حجم") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة حجم_مصفوفة تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SIZE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 4. فهرس / indexOf (array)
    if (funcName == "فهرس_مصفوفة" || funcName == "array_indexOf" || funcName == "فهرس_عنصر") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة فهرس تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_INDEX_OF);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 5. يحتوي_عنصر / contains (array)
    if (funcName == "يحتوي_عنصر" || funcName == "array_contains" || funcName == "has") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة يحتوي_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_CONTAINS);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 6. قلب / reverse
    if (funcName == "قلب" || funcName == "reverse" || funcName == "اعكس") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة قلب تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REVERSE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // 7. فرز / sort
    if (funcName == "فرز" || funcName == "sort" || funcName == "رتب") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة فرز تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SORT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // 8. أول / first
    if (funcName == "أول" || funcName == "first" || funcName == "العنصر_الأول") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة أول تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::VOID);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_FIRST);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::VOID);
    }
    
    // 9. آخر / last
    if (funcName == "آخر" || funcName == "last" || funcName == "العنصر_الأخير") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة آخر تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::VOID);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_LAST);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::VOID);
    }
    
    // 10. شريحة / slice
    if (funcName == "شريحة" || funcName == "array_slice" || funcName == "قطع") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة شريحة تتطلب على الأقل معاملين (مصفوفة, بداية)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SLICE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (argResults.size() >= 3) {
            inst.operands.push_back(argOperands[2]); // end (optional)
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::ARRAY);
    }

    // ========================================================================
    // File I/O Functions (8 functions)
    // ========================================================================
    
    // 1. اقرأ_ملف / read_file
    if (funcName == "اقرأ_ملف" || funcName == "read_file" || funcName == "readFile") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة اقرأ_ملف تتطلب معامل واحد (مسار الملف)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_READ);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 2. اكتب_ملف / write_file
    if (funcName == "اكتب_ملف" || funcName == "write_file" || funcName == "writeFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة اكتب_ملف تتطلب معاملين (مسار, محتوى)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_WRITE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 3. أضف_إلى_ملف / append_to_file
    if (funcName == "أضف_إلى_ملف" || funcName == "append_to_file" || funcName == "appendFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة أضف_إلى_ملف تتطلب معاملين (مسار, محتوى)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_APPEND);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 4. احذف_ملف / delete_file
    if (funcName == "احذف_ملف" || funcName == "delete_file" || funcName == "deleteFile") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة احذف_ملف تتطلب معامل واحد (مسار)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_DELETE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 5. انسخ_ملف / copy_file
    if (funcName == "انسخ_ملف" || funcName == "copy_file" || funcName == "copyFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة انسخ_ملف تتطلب معاملين (مصدر, وجهة)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_COPY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 6. انقل_ملف / move_file
    if (funcName == "انقل_ملف" || funcName == "move_file" || funcName == "moveFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة انقل_ملف تتطلب معاملين (مصدر, وجهة)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_MOVE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 7. أنشئ_مجلد / create_dir
    if (funcName == "أنشئ_مجلد" || funcName == "create_dir" || funcName == "createDir" || funcName == "mkdir") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة أنشئ_مجلد تتطلب معامل واحد (مسار)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_CREATE_DIR);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 8. اسرد_مجلد / list_dir
    if (funcName == "اسرد_مجلد" || funcName == "list_dir" || funcName == "listDir" || funcName == "ls") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة اسرد_مجلد تتطلب معامل واحد (مسار)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_LIST_DIR);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // ========================================================================
    // Utility Functions (4 functions)
    // ========================================================================
    
    // 1. عشوائي / random
    if (funcName == "عشوائي" || funcName == "random" || funcName == "rand") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        SIRInstruction inst(SIROpcode::BUILTIN_RANDOM);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // 2. نم / sleep
    if (funcName == "نم" || funcName == "sleep" || funcName == "wait") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة نم تتطلب معامل واحد (مدة بالثواني)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SLEEP);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 3. اخرج / exit
    if (funcName == "اخرج" || funcName == "exit" || funcName == "quit") {
        SIRInstruction inst(SIROpcode::BUILTIN_EXIT);
        if (!argResults.empty()) {
            inst.operands.push_back(argOperands[0]); // exit code (optional)
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 4. النوع / type_of
    if (funcName == "النوع" || funcName == "type_of" || funcName == "typeof" || funcName == "نوع") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة النوع تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_TYPE_OF);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }

    // ========================================================================
    // (AR) دوال برمجة أنظمة التشغيل — OS Development Builtin Functions
    // (EN) OS Development Builtin Functions
    // ========================================================================
    // (AR) هذا القسم يضيف الدوال المضمنة اللازمة لبرمجة أنظمة التشغيل:
    //      - منفذ_اكتب/منفذ_اقرأ: التعامل مع منافذ الإدخال/الإخراج (I/O ports)
    //      - ذاكرة_اكتب/ذاكرة_اقرأ: الوصول المباشر للذاكرة (raw memory access)
    //      - مقاطعة: إطلاق مقاطعة برمجية (software interrupt)
    //      - توقف: إيقاف المعالج (halt CPU)
    //      - تعطيل_مقاطعات/تفعيل_مقاطعات: التحكم بالمقاطعات (CLI/STI)
    //      - شاشة_اكتب/شاشة_امسح: التعامل مع ذاكرة VGA
    //      - انسخ_ذاكرة/املأ_ذاكرة: عمليات الذاكرة الجماعية
    // ========================================================================
    
    // ──────────────────────────────────────────────
    // (AR) منفذ_اكتب(منفذ، قيمة) — كتابة بايت على منفذ I/O
    // (EN) port_write(port, value) — write byte to I/O port (outb)
    // (AR) يستخدم في: التحكم بالعتاد مثل VGA، لوحة المفاتيح، القرص
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_اكتب" || funcName == "port_write" || funcName == "outb") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة منفذ_اكتب تتطلب معاملين: رقم المنفذ والقيمة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE);
        inst.operands.push_back(argOperands[0]);  // (AR) رقم المنفذ / (EN) port number
        inst.operands.push_back(argOperands[1]);  // (AR) القيمة المكتوبة / (EN) value to write
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin منفذ_اكتب()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) منفذ_اقرأ(منفذ) — قراءة بايت من منفذ I/O
    // (EN) port_read(port) — read byte from I/O port (inb)
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_اقرأ" || funcName == "port_read" || funcName == "inb") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة منفذ_اقرأ تتطلب معامل واحد: رقم المنفذ" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_PORT_READ);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin منفذ_اقرأ() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // ──────────────────────────────────────────────
    // (AR) ذاكرة_اكتب(عنوان، قيمة) — كتابة بايت في عنوان ذاكرة محدد
    // (EN) mem_write(address, value) — write byte to memory address (poke)
    // (AR) يستخدم في: الكتابة على ذاكرة VGA (0xB8000)، جداول المقاطعات
    // ──────────────────────────────────────────────
    if (funcName == "ذاكرة_اكتب" || funcName == "mem_write" || funcName == "poke") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة ذاكرة_اكتب تتطلب معاملين: العنوان والقيمة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_8);
        inst.operands.push_back(argOperands[0]);  // (AR) عنوان الذاكرة / (EN) memory address
        inst.operands.push_back(argOperands[1]);  // (AR) القيمة / (EN) value
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin ذاكرة_اكتب()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) ذاكرة_اقرأ(عنوان) — قراءة بايت من عنوان ذاكرة محدد
    // (EN) mem_read(address) — read byte from memory address (peek)
    // ──────────────────────────────────────────────
    if (funcName == "ذاكرة_اقرأ" || funcName == "mem_read" || funcName == "peek") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة ذاكرة_اقرأ تتطلب معامل واحد: العنوان" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_8);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin ذاكرة_اقرأ() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // ──────────────────────────────────────────────
    // (AR) مقاطعة(رقم) — إطلاق مقاطعة برمجية
    // (EN) interrupt(number) — trigger software interrupt (int N)
    // (AR) مثال: مقاطعة(0x80) لاستدعاء نظام Linux
    // ──────────────────────────────────────────────
    if (funcName == "مقاطعة" || funcName == "interrupt" || funcName == "int_call") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة مقاطعة تتطلب معامل واحد: رقم المقاطعة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_INTERRUPT);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin مقاطعة()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) توقف() — إيقاف المعالج حتى المقاطعة التالية
    // (EN) halt() — halt CPU until next interrupt (hlt instruction)
    // (AR) يستخدم في: حلقة الخمول الرئيسية للنواة
    // ──────────────────────────────────────────────
    if (funcName == "توقف" || funcName == "halt" || funcName == "hlt") {
        SIRInstruction inst(SIROpcode::BUILTIN_HALT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin توقف()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) تعطيل_مقاطعات() — تعطيل جميع المقاطعات (cli)
    // (EN) disable_interrupts() — disable all interrupts (cli instruction)
    // (AR) ضروري عند تعديل جداول المقاطعات أو البيانات الحرجة
    // ──────────────────────────────────────────────
    if (funcName == "تعطيل_مقاطعات" || funcName == "disable_interrupts" || funcName == "cli") {
        SIRInstruction inst(SIROpcode::BUILTIN_CLI);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin تعطيل_مقاطعات()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) تفعيل_مقاطعات() — تفعيل جميع المقاطعات (sti)
    // (EN) enable_interrupts() — enable all interrupts (sti instruction)
    // ──────────────────────────────────────────────
    if (funcName == "تفعيل_مقاطعات" || funcName == "enable_interrupts" || funcName == "sti") {
        SIRInstruction inst(SIROpcode::BUILTIN_STI);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin تفعيل_مقاطعات()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) شاشة_اكتب(صف، عمود، حرف، لون) — كتابة حرف في ذاكرة VGA
    // (EN) vga_write(row, col, char, color) — write char to VGA text memory
    // (AR) عنوان VGA النصي: 0xB8000 + (صف * 80 + عمود) * 2
    // ──────────────────────────────────────────────
    if (funcName == "شاشة_اكتب" || funcName == "vga_write") {
        if (argResults.size() < 4) {
            std::cerr << "[خطأ] دالة شاشة_اكتب تتطلب 4 معاملات: صف، عمود، حرف، لون" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_VGA_WRITE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin شاشة_اكتب()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) شاشة_امسح(لون) — مسح شاشة VGA بلون محدد
    // (EN) vga_clear(color) — clear VGA screen with specified color
    // ──────────────────────────────────────────────
    if (funcName == "شاشة_امسح" || funcName == "vga_clear") {
        SIRInstruction inst(SIROpcode::BUILTIN_VGA_CLEAR);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin شاشة_امسح()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) انسخ_ذاكرة(وجهة، مصدر، حجم) — نسخ كتلة ذاكرة
    // (EN) mem_copy(dest, src, size) — copy memory block
    // ──────────────────────────────────────────────
    if (funcName == "انسخ_ذاكرة" || funcName == "mem_copy" || funcName == "memcpy") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] دالة انسخ_ذاكرة تتطلب 3 معاملات: وجهة، مصدر، حجم" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_COPY);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin انسخ_ذاكرة()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) املأ_ذاكرة(وجهة، قيمة، حجم) — ملء كتلة ذاكرة بقيمة محددة
    // (EN) mem_set(dest, value, size) — fill memory block with value
    // ──────────────────────────────────────────────
    if (funcName == "املأ_ذاكرة" || funcName == "mem_set" || funcName == "memset") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] دالة املأ_ذاكرة تتطلب 3 معاملات: وجهة، قيمة، حجم" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_SET);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin املأ_ذاكرة()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ========================================================================
    // (AR) دوال Embedded المتقدمة — Advanced Embedded Builtin Functions (18)
    // (EN) Advanced Embedded Builtin Functions (18 functions)
    // ========================================================================
    // تسلسلي / Serial I/O — 4 دوال
    // GPIO — 3 دوال
    // مؤقت / Timer — 3 دوال
    // تحكم بالنظام / System Control — 3 دوال
    // حواجز ذاكرة / Memory Barriers — 3 دوال
    // DMA — 2 دوال
    // ========================================================================

    // ──────────────────────────────────────────────
    // 1. تسلسلي_هيئ(منفذ، سرعة) — تهيئة منفذ تسلسلي
    //    serial_init(port, baud_rate) — initialize serial port
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_هيئ" || funcName == "serial_init")) {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تسلسلي_هيئ تتطلب 2 معاملات: منفذ، سرعة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_INIT);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin تسلسلي_هيئ()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 2. تسلسلي_ارسل(منفذ، بايت) — إرسال بايت عبر التسلسلي
    //    serial_send(port, byte) — send byte via serial
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_ارسل" || funcName == "serial_send" || funcName == "serial_write")) {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تسلسلي_ارسل تتطلب 2 معاملات: منفذ، بايت" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_WRITE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin تسلسلي_ارسل()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 3. تسلسلي_استقبل(منفذ) — استقبال بايت من التسلسلي
    //    serial_receive(port) — receive byte from serial
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_استقبل" || funcName == "serial_receive" || funcName == "serial_read")) {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تسلسلي_استقبل تتطلب معامل: منفذ" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_READ);
        inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin تسلسلي_استقبل()" << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 4. تسلسلي_جاهز(منفذ) — فحص جاهزية البيانات
    //    serial_ready(port) — check if data available
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_جاهز" || funcName == "serial_ready")) {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تسلسلي_جاهز تتطلب معامل: منفذ" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_READY);
        inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin تسلسلي_جاهز()" << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }

    // ──────────────────────────────────────────────
    // 5. منفذ_رقمي_اكتب(رقم، قيمة) — كتابة GPIO
    //    gpio_write(pin, value) — write to GPIO pin
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_رقمي_اكتب" || funcName == "gpio_write" || funcName == "digital_write") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة منفذ_رقمي_اكتب تتطلب 2 معاملات: رقم_المنفذ، قيمة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_GPIO_WRITE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin منفذ_رقمي_اكتب()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 6. منفذ_رقمي_اقرأ(رقم) — قراءة GPIO
    //    gpio_read(pin) — read from GPIO pin
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_رقمي_اقرأ" || funcName == "gpio_read" || funcName == "digital_read") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة منفذ_رقمي_اقرأ تتطلب معامل: رقم_المنفذ" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_GPIO_READ);
        inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin منفذ_رقمي_اقرأ()" << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 7. حدد_وضع_منفذ(رقم، وضع) — تحديد وضع GPIO
    //    gpio_mode(pin, mode) — set GPIO pin mode (0=input, 1=output)
    // ──────────────────────────────────────────────
    if (funcName == "حدد_وضع_منفذ" || funcName == "gpio_mode" || funcName == "pin_mode") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة حدد_وضع_منفذ تتطلب 2 معاملات: رقم_المنفذ، الوضع" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_GPIO_MODE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin حدد_وضع_منفذ()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 8. مؤقت_هيئ(تردد) — تهيئة مؤقت العتاد
    //    timer_init(freq) — initialize hardware timer
    // ──────────────────────────────────────────────
    if (funcName == "مؤقت_هيئ" || funcName == "timer_init") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة مؤقت_هيئ تتطلب معامل: التردد" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_TIMER_INIT);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin مؤقت_هيئ()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 9. مؤقت_قراءة() — قراءة قيمة المؤقت الحالية
    //    timer_read() — read current timer value
    // ──────────────────────────────────────────────
    if (funcName == "مؤقت_قراءة" || funcName == "timer_read") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_TIMER_READ);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin مؤقت_قراءة()" << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 10. مؤقت_انتظر(ميكروثانية) — انتظار عدد ميكروثوان
    //     timer_wait(us) — wait for microseconds
    // ──────────────────────────────────────────────
    if (funcName == "مؤقت_انتظر" || funcName == "timer_wait" || funcName == "delay_us") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة مؤقت_انتظر تتطلب معامل: ميكروثوان" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_TIMER_WAIT);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin مؤقت_انتظر()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 11. اعد_تشغيل() — إعادة تشغيل النظام
    //     reset() / reboot() — system reset
    // ──────────────────────────────────────────────
    if (funcName == "اعد_تشغيل" || funcName == "reset" || funcName == "reboot") {
        SIRInstruction inst(SIROpcode::BUILTIN_RESET);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin اعد_تشغيل()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 12. معرف_المعالج() — الحصول على معرّف المعالج
    //     cpu_id() / cpuid() — get CPU identification
    // ──────────────────────────────────────────────
    if (funcName == "معرف_المعالج" || funcName == "cpu_id" || funcName == "cpuid") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_CPUID);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin معرف_المعالج()" << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 13. عداد_الدورات() — قراءة عداد الساعة (TSC)
    //     rdtsc() / cycle_count() — read timestamp counter
    // ──────────────────────────────────────────────
    if (funcName == "عداد_الدورات" || funcName == "rdtsc" || funcName == "cycle_count") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_RDTSC);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin عداد_الدورات()" << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 14. حاجز_ذاكرة() — حاجز ذاكرة كامل (mfence)
    //     memory_barrier() / mfence() — full memory fence
    // ──────────────────────────────────────────────
    if (funcName == "حاجز_ذاكرة" || funcName == "memory_barrier" || funcName == "mfence") {
        SIRInstruction inst(SIROpcode::BUILTIN_MFENCE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin حاجز_ذاكرة()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 15. حاجز_قراءة() — حاجز قراءة ذاكرة (lfence)
    //     read_barrier() / lfence() — load fence
    // ──────────────────────────────────────────────
    if (funcName == "حاجز_قراءة" || funcName == "read_barrier" || funcName == "lfence") {
        SIRInstruction inst(SIROpcode::BUILTIN_LFENCE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin حاجز_قراءة()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 16. حاجز_كتابة() — حاجز كتابة ذاكرة (sfence)
    //     write_barrier() / sfence() — store fence
    // ──────────────────────────────────────────────
    if (funcName == "حاجز_كتابة" || funcName == "write_barrier" || funcName == "sfence") {
        SIRInstruction inst(SIROpcode::BUILTIN_SFENCE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin حاجز_كتابة()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 17. نقل_مباشر_هيئ(قناة، مصدر، وجهة، حجم) — تهيئة DMA
    //     dma_init(channel, src, dest, size) — initialize DMA channel
    // ──────────────────────────────────────────────
    if (funcName == "نقل_مباشر_هيئ" || funcName == "dma_init") {
        if (argResults.size() < 4) {
            std::cerr << "[خطأ] دالة نقل_مباشر_هيئ تتطلب 4 معاملات: قناة، مصدر، وجهة، حجم" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_DMA_INIT);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin نقل_مباشر_هيئ()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 18. نقل_مباشر_ابدأ(قناة) — بدء نقل DMA
    //     dma_start(channel) — start DMA transfer
    // ──────────────────────────────────────────────
    if (funcName == "نقل_مباشر_ابدأ" || funcName == "dma_start") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة نقل_مباشر_ابدأ تتطلب معامل: رقم_القناة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_DMA_START);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin نقل_مباشر_ابدأ()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // ========================================================================
    // (AR) دوال الأمان — Security Builtin Functions (14 functions)
    // (EN) Security Builtin Functions (14 functions)
    // ========================================================================
    // (AR) هذا القسم يضيف الدوال المضمنة لنظام الأمان:
    //      - تأكد/تحقق/آمن: فحص الشروط والتحقق من الأمان
    //      - ذعر: إيقاف طارئ مع رسالة
    //      - هاش/شفّر/فك_تشفير: عمليات التشفير
    //      - تأكد_نوع/تأكد_مساواة/تأكد_أكبر: تأكيدات متقدمة
    //      - نظّف: تنظيف المدخلات من HTML
    //      - وقت_الآن/عشوائي_آمن/ترميز_64: أدوات مساعدة
    // ========================================================================
    
    // 1. تأكد / assert - يتحقق من شرط ويوقف البرنامج إذا كان خاطئاً
    if (funcName == "تأكد" || funcName == "assert" || funcName == "تاكد") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تأكد تتطلب معامل واحد على الأقل (الشرط)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT);
        inst.operands.push_back(argOperands[0]); // condition
        if (argOperands.size() > 1) {
            inst.operands.push_back(argOperands[1]); // optional message
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 2. تحقق / verify - يعيد صحيح أو خطأ دون إيقاف البرنامج
    if (funcName == "تحقق" || funcName == "verify") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تحقق تتطلب معامل واحد (الشرط)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_VERIFY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 3. آمن / is_safe - يتحقق من أمان القيمة
    if (funcName == "آمن" || funcName == "is_safe" || funcName == "امن") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة آمن تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_IS_SAFE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 4. ذعر / panic - إيقاف طارئ مع رسالة خطأ
    if (funcName == "ذعر" || funcName == "panic") {
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_PANIC);
        if (!argOperands.empty()) {
            inst.operands.push_back(argOperands[0]); // message
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 5. هاش / hash - حساب هاش FNV-1a للنص
    if (funcName == "هاش" || funcName == "hash") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة هاش تتطلب معامل واحد (النص)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_HASH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 6. شفّر / encrypt - تشفير XOR
    if (funcName == "شفّر" || funcName == "شفر" || funcName == "encrypt") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة شفّر تتطلب معاملين (النص، المفتاح)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ENCRYPT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 7. فك_تشفير / decrypt - فك تشفير XOR
    if (funcName == "فك_تشفير" || funcName == "decrypt") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة فك_تشفير تتطلب معاملين (النص، المفتاح)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_DECRYPT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 8. تأكد_نوع / assert_type - التحقق من نوع القيمة
    if (funcName == "تأكد_نوع" || funcName == "assert_type" || funcName == "تاكد_نوع") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تأكد_نوع تتطلب معاملين (القيمة، النوع_المتوقع)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 9. تأكد_مساواة / assert_equal - التحقق من تساوي قيمتين
    if (funcName == "تأكد_مساواة" || funcName == "assert_equal" || funcName == "تاكد_مساواة") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تأكد_مساواة تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_EQUAL);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 10. تأكد_أكبر / assert_greater - التحقق من أن القيمة الأولى أكبر
    if (funcName == "تأكد_أكبر" || funcName == "assert_greater" || funcName == "تاكد_اكبر") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تأكد_أكبر تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_GREATER);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 11. نظّف / sanitize - تنظيف نص من HTML
    if (funcName == "نظّف" || funcName == "نظف" || funcName == "sanitize") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة نظّف تتطلب معامل واحد (النص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_SANITIZE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 12. وقت_الآن / timestamp - الحصول على الوقت الحالي
    if (funcName == "وقت_الآن" || funcName == "وقت_الان" || funcName == "timestamp" || funcName == "now") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_TIMESTAMP);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 13. عشوائي_آمن / secure_random - رقم عشوائي آمن
    if (funcName == "عشوائي_آمن" || funcName == "عشوائي_امن" || funcName == "secure_random") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة عشوائي_آمن تتطلب معاملين (الحد_الأدنى، الحد_الأقصى)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 14. ترميز_64 / base64_encode - ترميز Base64
    if (funcName == "ترميز_64" || funcName == "base64_encode" || funcName == "base64") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة ترميز_64 تتطلب معامل واحد (النص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::STRING);
    }

    // ========================================================================
    // (AR) التكامل مع C/C++ — FFI Functions (20 دالة)
    // (EN) C/C++ Foreign Function Interface — 20 functions
    // ========================================================================
    
    // 1. طباعة_تنسيق / printf — formatted print (variadic)
    if (funcName == "\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82" || funcName == "printf" || funcName == "c_printf") {
        if (argOperands.empty()) {
            std::cerr << "[ERROR] printf requires at least 1 argument (format string)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_PRINTF);
        inst.result = resultOp;
        for (size_t i = 0; i < argOperands.size(); i++) {
            inst.operands.push_back(argOperands[i]);
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin printf() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 2. حجز / malloc — allocate memory
    if (funcName == "\xd8\xad\xd8\xac\xd8\xb2" || funcName == "malloc" || funcName == "c_malloc") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] malloc requires 1 argument (size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_MALLOC);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin malloc() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 3. حرر / free — free memory
    if (funcName == "\xd8\xad\xd8\xb1\xd8\xb1" || funcName == "free" || funcName == "c_free") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] free requires 1 argument (pointer)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::FFI_FREE);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] builtin free()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }
    
    // 4. اعد_حجز / realloc
    if (funcName == "\xd8\xa7\xd8\xb9\xd8\xaf_\xd8\xad\xd8\xac\xd8\xb2" || funcName == "realloc" || funcName == "c_realloc") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] realloc requires 2 arguments (ptr, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_REALLOC);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 5. حجز_صفري / calloc
    if (funcName == "\xd8\xad\xd8\xac\xd8\xb2_\xd8\xb5\xd9\x81\xd8\xb1\xd9\x8a" || funcName == "calloc" || funcName == "c_calloc") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] calloc requires 2 arguments (count, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_CALLOC);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 6. طول_نص_س / strlen
    if (funcName == "\xd8\xb7\xd9\x88\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strlen" || funcName == "c_strlen") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] strlen requires 1 argument (string)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_STRLEN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 7. انسخ_نص_س / strcpy
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcpy" || funcName == "c_strcpy") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] strcpy requires 2 arguments (dest, src)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_STRCPY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 8. قارن_نص_س / strcmp
    if (funcName == "\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcmp" || funcName == "c_strcmp") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] strcmp requires 2 arguments (s1, s2)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_STRCMP);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 9. الحق_نص_س / strcat
    if (funcName == "\xd8\xa7\xd9\x84\xd8\xad\xd9\x82_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcat" || funcName == "c_strcat") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] strcat requires 2 arguments (dest, src)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_STRCAT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 10. انسخ_ذاكرة_س / memcpy
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memcpy" || funcName == "c_memcpy") {
        if (argOperands.size() != 3) {
            std::cerr << "[ERROR] memcpy requires 3 arguments (dest, src, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_MEMCPY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 11. عبئ_ذاكرة_س / memset
    if (funcName == "\xd8\xb9\xd8\xa8\xd8\xa6_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memset" || funcName == "c_memset") {
        if (argOperands.size() != 3) {
            std::cerr << "[ERROR] memset requires 3 arguments (ptr, value, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_MEMSET);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 12. افتح_ملف_س / fopen
    if (funcName == "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fopen" || funcName == "c_fopen") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] fopen requires 2 arguments (filename, mode)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_FOPEN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 13. اغلق_ملف_س / fclose
    if (funcName == "\xd8\xa7\xd8\xba\xd9\x84\xd9\x82_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fclose" || funcName == "c_fclose") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] fclose requires 1 argument (file pointer)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_FCLOSE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 14. اكتب_ملف_س / fputs — write string to file
    if (funcName == "\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fputs" || funcName == "c_fputs") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] fputs requires 2 arguments (string, file)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_FWRITE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 15. اقرأ_ملف_س / fgets — read line from file
    if (funcName == "\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fgets" || funcName == "c_fgets") {
        if (argOperands.size() != 3) {
            std::cerr << "[ERROR] fgets requires 3 arguments (buffer, size, file)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_FREAD);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 16. نفذ_امر / system — execute system command
    if (funcName == "\xd9\x86\xd9\x81\xd8\xb0_\xd8\xa7\xd9\x85\xd8\xb1" || funcName == "system" || funcName == "c_system") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] system requires 1 argument (command)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_SYSTEM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 17. قيمة_بيئة / getenv
    if (funcName == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9_\xd8\xa8\xd9\x8a\xd8\xa6\xd8\xa9" || funcName == "getenv" || funcName == "c_getenv") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] getenv requires 1 argument (name)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::FFI_GETENV);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 18. نص_لعدد / atoi
    if (funcName == "\xd9\x86\xd8\xb5_\xd9\x84\xd8\xb9\xd8\xaf\xd8\xaf" || funcName == "atoi" || funcName == "c_atoi") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] atoi requires 1 argument (string)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_ATOI);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 19. نص_لعشري / atof
    if (funcName == "\xd9\x86\xd8\xb5_\xd9\x84\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a" || funcName == "atof" || funcName == "c_atof") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] atof requires 1 argument (string)" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        SIRInstruction inst(SIROpcode::FFI_ATOF);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // 20. تنسيق_نص / snprintf — format to buffer (variadic)
    if (funcName == "\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82_\xd9\x86\xd8\xb5" || funcName == "snprintf" || funcName == "c_snprintf") {
        if (argOperands.size() < 3) {
            std::cerr << "[ERROR] snprintf requires at least 3 arguments (buf, size, fmt)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_SNPRINTF);
        inst.result = resultOp;
        for (size_t i = 0; i < argOperands.size(); i++) {
            inst.operands.push_back(argOperands[i]);
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }


    // ========================================================================
    // (AR) Async/Await - 23 functions
    // (EN) Async/Await & Concurrency - 23 functions
    // ========================================================================

    // 1. spawn - async task
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9" || funcName == "spawn" || funcName == "async_spawn") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] spawn requires at least 1 argument (function name)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_SPAWN);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] async spawn() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 2. await - wait for future/task
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9" || funcName == "await" || funcName == "async_await" || funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] await requires 1 argument (task/future id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_AWAIT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] async await() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 3. yield
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xaa\xd8\xac" || funcName == "yield" || funcName == "async_yield") {
        SIRInstruction inst(SIROpcode::ASYNC_YIELD);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] async yield()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 4. async_sleep - non-blocking sleep
    if (funcName == "\xd9\x86\xd9\x88\xd9\x85_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86" || funcName == "async_sleep" || funcName == "sleep_async") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] async_sleep requires 1 argument (milliseconds)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_SLEEP);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] async_sleep()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 5. create_future
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "create_future" || funcName == "\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_CREATE_FUTURE);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] create_future() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 6. resolve_future
    if (funcName == "\xd8\xa3\xd9\x88\xd9\x81_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "resolve_future" || funcName == "\xd8\xad\xd9\x82\xd9\x82_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] resolve_future requires 2 args (future_id, value)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_RESOLVE_FUTURE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] resolve_future()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 7. get_future
    if (funcName == "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "get_future") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] get_future requires 1 argument (future_id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_GET_FUTURE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] get_future() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 8. create_channel
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "create_channel" || funcName == "\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_CREATE_CHANNEL);
        inst.result = resultOp;
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] create_channel() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 9. channel_send
    if (funcName == "\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_send" || funcName == "\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] channel_send requires 2 args (channel_id, value)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_SEND);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] channel_send()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 10. channel_recv
    if (funcName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_recv" || funcName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] channel_recv requires 1 argument (channel_id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_RECV);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] channel_recv() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 11. channel_close
    if (funcName == "\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_close") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] channel_close requires 1 argument (channel_id)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_CLOSE);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] channel_close()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 12. create_mutex
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x81\xd9\x84" || funcName == "create_mutex" || funcName == "\xd9\x82\xd9\x81\xd9\x84") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_MUTEX_CREATE);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] create_mutex() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 13. mutex_lock
    if (funcName == "\xd8\xa7\xd9\x82\xd9\x81\xd9\x84" || funcName == "mutex_lock" || funcName == "lock") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] mutex_lock requires 1 argument (mutex_id)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_MUTEX_LOCK);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] mutex_lock()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 14. mutex_unlock
    if (funcName == "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x82\xd9\x81\xd9\x84" || funcName == "mutex_unlock" || funcName == "unlock") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] mutex_unlock requires 1 argument (mutex_id)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_MUTEX_UNLOCK);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] mutex_unlock()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 15. thread_spawn
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xae\xd9\x8a\xd8\xb7" || funcName == "thread_spawn" || funcName == "\xd8\xae\xd9\x8a\xd8\xb7") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] thread_spawn requires at least 1 argument" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_THREAD_SPAWN);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] thread_spawn() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 16. thread_join
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xb6\xd9\x85_\xd8\xae\xd9\x8a\xd8\xb7" || funcName == "thread_join") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] thread_join requires 1 argument (thread_id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_THREAD_JOIN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] thread_join() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 17. atomic_load
    if (funcName == "\xd8\xad\xd9\x85\xd9\x84_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_load") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] atomic_load requires 1 argument (address)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_LOAD);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] atomic_load() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 18. atomic_store
    if (funcName == "\xd8\xae\xd8\xb2\xd9\x86_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_store") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] atomic_store requires 2 args (address, value)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_STORE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] atomic_store()" << std::endl;
        return BuildResult("", SIRType::VOID);
    }

    // 19. atomic_add
    if (funcName == "\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_add") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] atomic_add requires 2 args (address, value)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_ADD);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] atomic_add() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 20. compare_and_swap / CAS
    if (funcName == "\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x88\xd8\xa8\xd8\xaf\xd9\x84" || funcName == "compare_and_swap" || funcName == "cas") {
        if (argResults.size() < 3) {
            std::cerr << "[ERROR] CAS requires 3 args (addr, expected, desired)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_CAS);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] compare_and_swap() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 21. wait_all
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84" || funcName == "wait_all") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_WAIT_ALL);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] wait_all() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 22. wait_any
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa3\xd9\x8a" || funcName == "wait_any") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_WAIT_ANY);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] wait_any() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
    }

    // 23. select / channel_select
    if (funcName == "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "select" || funcName == "channel_select") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_SELECT);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        std::cout << "[DEBUG] select() -> " << resultReg << std::endl;
        return BuildResult(resultReg, SIRType::I64);
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
        allocInst.result = SIROperand::Register(objReg, SIRType::I64);
        // (AR) إضافة اسم الصنف كـ metadata
        // (EN) Add class name as metadata
        allocInst.operands.push_back(SIROperand::ConstantString(newExpr->className));
        currentBlock_->addInstruction(allocInst);
    }
    
    // (AR) الخطوة 3: استدعاء دالة البناء (constructor) إن وجدت
    // (EN) Step 3: Call constructor if exists
    std::string constructorName = newExpr->className + ".بناء";
    auto constructor = sirClass->getMethod(constructorName);
    
    if (constructor || !newExpr->arguments.empty()) {
        // (AR) بناء معاملات البناء
        // (EN) Build constructor arguments
        std::vector<SIROperand> args;
        
        // (AR) المعامل الأول هو الكائن نفسه (self)
        // (EN) First argument is the object itself (self)
        args.push_back(SIROperand::Register(objReg, SIRType::I64));
        
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
    return BuildResult(objReg, SIRType::I64);
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
// buildMethodCall - بناء استدعاء طريقة على كائن
// ============================================================================
// مصدر التعريف / Source: class_nodes.h:245
// التوقيع / Signature: BuildResult buildMethodCall(AST::MethodCallExpr* methodCallExpr);
//
// MethodCallExpr Members:
// - object: std::unique_ptr<Expr> (line 247)
// - methodName: std::string (line 248)
// - arguments: std::vector<std::unique_ptr<Expr>> (line 249)
// ============================================================================
BuildResult SIRBuilder::buildMethodCall(AST::MethodCallExpr* methodCallExpr) {
    if (!methodCallExpr) {
        return BuildResult();
    }
    
    std::cout << "[DEBUG] buildMethodCall: calling method '" 
              << methodCallExpr->methodName << "'" << std::endl;
    
    // (AR) الخطوة 1: بناء تعبير الكائن
    // (EN) Step 1: Build object expression
    auto objResult = buildExpression(methodCallExpr->object.get());
    
    // (AR) الخطوة 2: تحديد اسم الصنف من المتغير
    // (EN) Step 2: Determine class name from variable
    std::string className;
    
    // (AR) نحاول إيجاد اسم الصنف من VariableExpr
    // (EN) Try to find class name from VariableExpr
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(methodCallExpr->object.get())) {
        // (AR) البحث عن معلومات المتغير في classInstanceTypes_
        // (EN) Look up variable info in classInstanceTypes_
        if (classInstanceTypes_.find(varExpr->name) != classInstanceTypes_.end()) {
            className = classInstanceTypes_[varExpr->name];
        }
    }
    
    // (AR) إذا 'هذا' (this), استخدم currentClassName_
    // (EN) If 'this', use currentClassName_
    if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr*>(methodCallExpr->object.get())) {
        className = currentClassName_;
    }
    
    // (AR) إذا لم نجد اسم الصنف، نحاول من الصنف الحالي
    // (EN) If class name not found, try current class
    if (className.empty() && !currentClassName_.empty()) {
        className = currentClassName_;
    }
    
    // (AR) البحث عن الطريقة في جدول الدوال
    // (EN) Look up method in function table
    std::string fullMethodName = className + "." + methodCallExpr->methodName;
    
    std::cout << "[DEBUG] buildMethodCall: full method name = '" << fullMethodName << "'" << std::endl;
    
    // (AR) تحديد نوع الإرجاع
    // (EN) Determine return type
    SIRType returnType = SIRType::VOID;
    if (functionTable_.find(fullMethodName) != functionTable_.end()) {
        returnType = functionTable_[fullMethodName].returnType;
    }
    
    // (AR) الخطوة 3: بناء المعاملات
    // (EN) Step 3: Build arguments
    std::vector<SIROperand> args;
    
    // (AR) المعامل الأول: self (مؤشر الكائن)
    // (EN) First argument: self (object pointer)
    args.push_back(SIROperand::Register(objResult.registerName, objResult.type));
    
    // (AR) باقي المعاملات
    // (EN) Rest of arguments
    for (const auto& arg : methodCallExpr->arguments) {
        auto argResult = buildExpression(arg.get());
        if (argResult.isConstant && !argResult.constantValue.empty()) {
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
    
    // (AR) الخطوة 4: إنشاء تعليمة CALL
    // (EN) Step 4: Create CALL instruction
    std::string resultReg = newTempRegister();
    
    if (currentBlock_) {
        SIRInstruction callInst;
        callInst.opcode = SIROpcode::CALL;
        callInst.result = SIROperand::Register(resultReg, returnType);
        callInst.operands.push_back(SIROperand::Function(fullMethodName));
        for (const auto& arg : args) {
            callInst.operands.push_back(arg);
        }
        currentBlock_->addInstruction(callInst);
    }
    
    std::cout << "[DEBUG] buildMethodCall: result in register '" << resultReg << "'" << std::endl;
    
    return BuildResult(resultReg, returnType);
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

// ============================================================================
// (AR) نظام الاستيراد والتصدير في المترجم
// (EN) Import/Export System in Compiler
// ============================================================================

/**
 * @brief (AR) تعيين مسار الملف الحالي - يُستخدم لحل مسارات الاستيراد النسبية
 * @brief (EN) Set current file path - used for resolving relative import paths
 */
void SIRBuilder::setCurrentFilePath(const std::string& filePath) {
    currentFilePath_ = filePath;
    
    // (AR) إنشاء محلل الوحدات إذا لم يكن موجوداً
    // (EN) Create module resolver if it doesn't exist
    if (!moduleResolver_) {
        moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
        
        // (AR) إضافة مجلد الملف الحالي كمسار بحث
        // (EN) Add current file's directory as search path
        if (!filePath.empty()) {
            try {
                auto fsPath = sad::utf8::make_path(filePath);
                auto parentDir = fsPath.parent_path();
                if (!parentDir.empty() && std::filesystem::exists(parentDir)) {
                    // (AR) نستخدم filesystem::path مباشرة لتجنب مشاكل ترميز UTF-8 مع ANSI
                    // (EN) Push filesystem::path directly to avoid UTF-8/ANSI encoding issues
                    moduleResolver_->addSearchPathDirect(parentDir);
                }
            } catch (const std::exception& e) {
                // (AR) تجاهل الأخطاء - مسارات البحث الافتراضية كافية
                // (EN) Ignore errors - default search paths are sufficient
                (void)e;
            }
        }
    }
}

/**
 * @brief (AR) معالجة وحدة مستوردة - تحليل ملف الوحدة وبناء دوالها
 * @brief (EN) Process imported module - parse module file and build its functions
 * 
 * @details
 * (AR) آلية العمل:
 *      1. استخدام ModuleResolver للعثور على ملف الوحدة
 *      2. تحليل AST الوحدة
 *      3. استخراج الدوال المُصدَّرة
 *      4. بناء SIR لكل دالة مُصدَّرة وإضافتها للوحدة الحالية
 * 
 * (EN) How it works:
 *      1. Use ModuleResolver to find module file
 *      2. Parse module AST
 *      3. Extract exported functions
 *      4. Build SIR for each exported function and add to current module
 */
void SIRBuilder::buildImportStmt(AST::ImportStmt* importStmt) {
    if (!importStmt) return;
    
    // (AR) التحقق من وجود محلل الوحدات
    // (EN) Check module resolver exists
    if (!moduleResolver_) {
        moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
    }
    
    std::string fullModuleName = importStmt->getFullModuleName();
    
    // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
    // (EN) Check if module was already processed
    if (processedModules_.count(fullModuleName)) {
        return;
    }
    
    // (AR) تحميل الوحدة
    // (EN) Load module
    Modules::Module* module = moduleResolver_->resolveModule(
        importStmt->modulePath,
        currentFilePath_
    );
    
    if (!module) {
        errors_.push_back(
            "خطأ: لم يُعثر على الوحدة '" + fullModuleName + "' / "
            "Error: Module '" + fullModuleName + "' not found"
        );
        return;
    }
    
    // (AR) تمييز الوحدة كمعالجة
    // (EN) Mark module as processed
    processedModules_.insert(fullModuleName);
    
    // (AR) معالجة كل تصريح في الوحدة
    // (EN) Process each declaration in module
    for (const auto& stmt : module->ast) {
        if (!stmt) continue;
        
        // (AR) استخراج الدوال (المُصدَّرة أو غير المُصدَّرة)
        // (EN) Extract functions (exported or not)
        AST::FunctionDecl* funcDecl = nullptr;
        AST::VarDeclStmt* varDecl = nullptr;
        AST::ClassDecl* classDecl = nullptr;
        
        // (AR) تحقق من تصدير صريح (ExportDecl)
        // (EN) Check for explicit export (ExportDecl)
        if (auto exportDecl = dynamic_cast<AST::ExportDecl*>(stmt.get())) {
            if (exportDecl->declaration) {
                funcDecl = dynamic_cast<AST::FunctionDecl*>(exportDecl->declaration.get());
                varDecl = dynamic_cast<AST::VarDeclStmt*>(exportDecl->declaration.get());
                classDecl = dynamic_cast<AST::ClassDecl*>(exportDecl->declaration.get());
            }
        }
        // (AR) تحقق من تصدير قديم (ExportStmt)
        // (EN) Check for legacy export (ExportStmt)
        else if (auto exportStmt = dynamic_cast<AST::ExportStmt*>(stmt.get())) {
            if (exportStmt->declaration) {
                funcDecl = dynamic_cast<AST::FunctionDecl*>(exportStmt->declaration.get());
                varDecl = dynamic_cast<AST::VarDeclStmt*>(exportStmt->declaration.get());
                classDecl = dynamic_cast<AST::ClassDecl*>(exportStmt->declaration.get());
            }
        }
        // (AR) دالة عادية (بدون تصدير صريح)
        // (EN) Regular function (no explicit export)
        else {
            funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt.get());
            varDecl = dynamic_cast<AST::VarDeclStmt*>(stmt.get());
            classDecl = dynamic_cast<AST::ClassDecl*>(stmt.get());
        }
        
        // (AR) بناء SIR للتصريحات المُكتشفة
        // (EN) Build SIR for discovered declarations
        if (funcDecl) {
            buildFunction(funcDecl);
        }
        if (varDecl) {
            buildGlobalVariable(varDecl);
        }
        if (classDecl) {
            buildClass(classDecl);
        }
    }
}

/**
 * @brief (AR) معالجة استيراد انتقائي: من وحدة استورد ...
 * @brief (EN) Process selective import: from module import ...
 */
void SIRBuilder::buildFromImportStmt(AST::FromImportStmt* fromImportStmt) {
    if (!fromImportStmt) return;
    
    if (!moduleResolver_) {
        moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
    }
    
    std::string fullModuleName = fromImportStmt->getFullModuleName();
    
    // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
    // (EN) Check if module was already processed
    if (processedModules_.count(fullModuleName)) {
        return;
    }
    
    // (AR) تحميل الوحدة
    // (EN) Load module
    Modules::Module* module = moduleResolver_->resolveModule(
        fromImportStmt->modulePath,
        currentFilePath_
    );
    
    if (!module) {
        errors_.push_back(
            "خطأ: لم يُعثر على الوحدة '" + fullModuleName + "' / "
            "Error: Module '" + fullModuleName + "' not found"
        );
        return;
    }
    
    processedModules_.insert(fullModuleName);
    
    // (AR) جمع أسماء الرموز المطلوبة (للاستيراد الانتقائي)
    // (EN) Collect requested symbol names (for selective import)
    std::unordered_set<std::string> requestedSymbols;
    bool isWildcard = fromImportStmt->isWildcard;
    
    if (!isWildcard) {
        for (const auto& item : fromImportStmt->items) {
            requestedSymbols.insert(item.name);
        }
    }
    
    // (AR) معالجة تصريحات الوحدة
    // (EN) Process module declarations
    for (const auto& stmt : module->ast) {
        if (!stmt) continue;
        
        AST::FunctionDecl* funcDecl = nullptr;
        AST::VarDeclStmt* varDecl = nullptr;
        AST::ClassDecl* classDecl = nullptr;
        bool isExported = false;
        
        if (auto exportDecl = dynamic_cast<AST::ExportDecl*>(stmt.get())) {
            isExported = true;
            if (exportDecl->declaration) {
                funcDecl = dynamic_cast<AST::FunctionDecl*>(exportDecl->declaration.get());
                varDecl = dynamic_cast<AST::VarDeclStmt*>(exportDecl->declaration.get());
                classDecl = dynamic_cast<AST::ClassDecl*>(exportDecl->declaration.get());
            }
        } else if (auto exportStmt = dynamic_cast<AST::ExportStmt*>(stmt.get())) {
            isExported = true;
            if (exportStmt->declaration) {
                funcDecl = dynamic_cast<AST::FunctionDecl*>(exportStmt->declaration.get());
                varDecl = dynamic_cast<AST::VarDeclStmt*>(exportStmt->declaration.get());
                classDecl = dynamic_cast<AST::ClassDecl*>(exportStmt->declaration.get());
            }
        } else {
            funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt.get());
            varDecl = dynamic_cast<AST::VarDeclStmt*>(stmt.get());
            classDecl = dynamic_cast<AST::ClassDecl*>(stmt.get());
        }
        
        // (AR) تحديد ما إذا كان الرمز مطلوباً
        // (EN) Determine if symbol is requested
        std::string symbolName;
        if (funcDecl) symbolName = funcDecl->name;
        else if (varDecl) symbolName = varDecl->name;
        else if (classDecl) symbolName = classDecl->name;
        else continue;
        
        // (AR) في حالة الاستيراد الانتقائي، نتحقق من أن الرمز مطلوب
        // (EN) For selective import, check if symbol is requested
        if (!isWildcard && requestedSymbols.find(symbolName) == requestedSymbols.end()) {
            continue;
        }
        
        // (AR) بناء SIR
        // (EN) Build SIR
        if (funcDecl) {
            buildFunction(funcDecl);
        }
        if (varDecl) {
            buildGlobalVariable(varDecl);
        }
        if (classDecl) {
            buildClass(classDecl);
        }
    }
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
