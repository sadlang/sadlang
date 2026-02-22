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
    // (AR) المرحلة الثانية: بناء التصريحات وجمع الجمل التنفيذية
    // (EN) Phase 2: Build declarations and collect executable statements
    // ═══════════════════════════════════════════════════════════════════
    // (AR) جمع الجمل التنفيذية في المستوى الأعلى لتضمينها في __sad_main
    // (EN) Collect top-level executable statements for __sad_main wrapper
    std::vector<Sad::AST::Statement*> topLevelStatements;
    bool hasMainFunction = false;  // (AR) هل توجد دالة "رئيسية" أو "main"؟
    
    // program هو StmtList = std::vector<StmtPtr> (ast_node.h:170)
    for (const auto& stmt : *program) {
        if (!stmt) continue;
        
        // (AR) التحقق من نوع التصريح باستخدام dynamic_cast
        // (EN) Check declaration type using dynamic_cast
        
        // (AR) دالة؟ (declarations.h:19 - class FunctionDecl)
        // (EN) Function declaration?
        if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl*>(stmt.get())) {
            buildFunction(funcDecl);
            // (AR) التحقق هل هي دالة رئيسية
            if (funcDecl->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" || funcDecl->name == "main") {
                hasMainFunction = true;
            }
            continue;
        }
        
        // (AR) صنف؟ (declarations.h:122 - class ClassDecl)
        // (EN) Class declaration?
        if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl*>(stmt.get())) {
            buildClass(classDecl);
            continue;
        }
        
        // (AR) سمة/واجهة؟ (declarations.h - class TraitDecl)
        // (EN) Trait/Interface declaration?
        if (auto traitDecl = dynamic_cast<Sad::AST::TraitDecl*>(stmt.get())) {
            buildTrait(traitDecl);
            continue;
        }
        
        // (AR) تنفيذ سمة؟ (declarations.h - class ImplDecl)
        // (EN) Impl block?
        if (auto implDecl = dynamic_cast<Sad::AST::ImplDecl*>(stmt.get())) {
            buildImpl(implDecl);
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
            if (exportDecl->declaration) {
                auto* innerStmt = exportDecl->declaration.get();
                
                if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl*>(innerStmt)) {
                    buildFunction(innerFunc);
                    if (innerFunc->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" || innerFunc->name == "main") {
                        hasMainFunction = true;
                    }
                } else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl*>(innerStmt)) {
                    buildClass(innerClass);
                } else {
                    // (AR) أي تصدير آخر (متغير، إلخ) → جملة تنفيذية
                    topLevelStatements.push_back(stmt.get());
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
                } else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl*>(innerStmt)) {
                    buildClass(innerClass);
                } else {
                    topLevelStatements.push_back(stmt.get());
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
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) جملة تنفيذية في المستوى الأعلى (طباعة، متغير، شرط، حلقة...)
        // (EN) Top-level executable statement (print, var, if, while...)
        // ═══════════════════════════════════════════════════════════════
        topLevelStatements.push_back(stmt.get());
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) المرحلة الثالثة: إنشاء دالة __sad_main لتغليف الكود التنفيذي
    // (EN) Phase 3: Create __sad_main to wrap top-level executable code
    // ═══════════════════════════════════════════════════════════════════
    // (AR) إذا توجد جمل تنفيذية ولا توجد دالة رئيسية صريحة
    // (EN) If there are executable statements and no explicit main function
    if (!topLevelStatements.empty() && !hasMainFunction) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildModule: creating __sad_main with " 
                  << topLevelStatements.size() << " top-level statements" << std::endl;
        #endif
        
        // (AR) إنشاء دالة __sad_main من نوع void بدون معاملات
        // (EN) Create __sad_main function: void __sad_main()
        auto sadMainFunc = std::make_shared<SIRFunction>("__sad_main", SIRType::VOID);
        
        // (AR) حفظ الحالة الحالية
        // (EN) Save current state
        auto prevFunction = currentFunction_;
        auto prevBlock = currentBlock_;
        
        currentFunction_ = sadMainFunc;
        
        // (AR) إنشاء نطاق جديد للدالة الرئيسية
        // (EN) Create new scope for main function
        enterScope();
        
        // (AR) إنشاء basic block للدخول
        // (EN) Create entry basic block
        auto entryBlock = createBasicBlock("entry");
        sadMainFunc->addBasicBlock(entryBlock);
        currentBlock_ = entryBlock;
        
        // (AR) بناء جميع الجمل التنفيذية
        // (EN) Build all executable statements
        for (auto* stmtNode : topLevelStatements) {
            if (stmtNode) {
                buildStatement(stmtNode);
            }
        }
        
        // (AR) الخروج من النطاق
        // (EN) Exit scope
        exitScope();
        
        // (AR) التأكد من وجود terminator
        // (EN) Ensure terminator exists
        if (currentBlock_) {
            bool hasTerminator = false;
            if (!currentBlock_->instructions.empty()) {
                const auto& lastInst = currentBlock_->instructions.back();
                hasTerminator = (lastInst.opcode == SIROpcode::RET || 
                               lastInst.opcode == SIROpcode::RET_VOID);
            }
            if (!hasTerminator) {
                SIRInstruction retInst;
                retInst.opcode = SIROpcode::RET_VOID;
                currentBlock_->addInstruction(retInst);
            }
        }
        
        // (AR) إضافة الدالة للوحدة
        // (EN) Add function to module
        module_->addFunction(sadMainFunc);
        
        // (AR) تسجيل في جدول الدوال
        // (EN) Register in function table
        {
            FunctionInfo funcInfo;
            funcInfo.name = "__sad_main";
            funcInfo.returnType = SIRType::VOID;
            funcInfo.sirFunction = sadMainFunc;
            functionTable_["__sad_main"] = funcInfo;
        }
        
        // (AR) استعادة الحالة السابقة
        // (EN) Restore previous state
        currentFunction_ = prevFunction;
        currentBlock_ = prevBlock;
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
    SIRType returnType;
    
    // (AR) إذا كان نوع الإرجاع غير محدد (UNKNOWN/NONE)، نستنتجه من جسم الدالة
    // (EN) If return type is unspecified (UNKNOWN/NONE), infer it from function body
    if (funcDecl->returnType == Data::DataType::UNKNOWN ||
        funcDecl->returnType == Data::DataType::NONE) {
        // (AR) استنتاج النوع من جسم الدالة
        // (EN) Infer type from function body
        returnType = inferReturnTypeFromBody(funcDecl->body.get());
    } else {
        returnType = astTypeToSIRType(funcDecl->returnType);
    }
    
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

} // namespace SIR
} // namespace Compiler
} // namespace Sad
