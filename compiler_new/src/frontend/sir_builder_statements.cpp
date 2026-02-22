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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildStatement: processing statement" << std::endl;
    #endif
    
    // (AR) BlockStmt - كتلة من الجمل (statements.h:423)
    // (EN) Block statement
    if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found BlockStmt with " << blockStmt->statements.size() << " statements" << std::endl;
        #endif
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
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found IfStmt" << std::endl;
        #endif
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
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found ForRangeStmt" << std::endl;
        #endif
        buildForRangeLoop(forRangeStmt);
        return;
    }
    
    // (AR) ReturnStmt - جملة return (statements.h:266)
    // (EN) Return statement
    if (auto returnStmt = dynamic_cast<Sad::AST::ReturnStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found ReturnStmt" << std::endl;
        #endif
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
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found VarDeclStmt: " << varDecl->name << std::endl;
        #endif
        buildLocalVariable(varDecl);
        return;
    }
    
    // (AR) TemplateFunctionDecl - تصريح دالة قالب (declarations.h:470)
    // (EN) Template function declaration
    if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found TemplateFunctionDecl: " << templateDecl->name << std::endl;
        #endif
        buildTemplateFunction(templateDecl);
        return;
    }
    
    // (AR) TemplateClassDecl - تصريح صنف قالب (declarations.h:520)
    // (EN) Template class declaration — store for later instantiation
    if (auto templateClassDecl = dynamic_cast<Sad::AST::TemplateClassDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found TemplateClassDecl: " << templateClassDecl->name 
                  << " with " << templateClassDecl->typeParameters.size() << " type params" << std::endl;
        #endif
        
        // (AR) تسجيل صنف القالب — يُنشأ عند TemplateInstantiation
        // (EN) Register template class — instantiated at TemplateInstantiation
        templateClasses_[templateClassDecl->name] = templateClassDecl;
        
        // (AR) إنشاء SIRClass فارغ بالاسم الأساسي ليُعرف في الجدول
        // (EN) Create empty SIRClass with base name so it's known
        std::string parentClass;
        if (!templateClassDecl->superclasses.empty()) {
            parentClass = templateClassDecl->superclasses[0];
        }
        auto sirClass = std::make_shared<SIRClass>(templateClassDecl->name, parentClass);
        
        for (const auto& tp : templateClassDecl->typeParameters) {
            sirClass->addField(tp.name, SIRType::PTR);
        }
        
        module_->addClass(sirClass);
        return;
    }
    
    // (AR) MatchStmt - جملة match (pattern_nodes.h:MatchStmt)
    // (EN) Match statement (pattern matching)
    if (auto matchStmt = dynamic_cast<Sad::AST::MatchStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found MatchStmt" << std::endl;
        #endif
        buildMatchStatement(matchStmt);
        return;
    }
    
    // ========================================================================
    // (AR) SwitchStmt - جملة حالة/switch (statements.h:SwitchStmt)
    // (EN) Switch statement
    // ========================================================================
    if (auto switchStmt = dynamic_cast<Sad::AST::SwitchStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found SwitchStmt with " << switchStmt->cases.size() << " cases" << std::endl;
        #endif
        
        // (AR) بناء تعبير الحالة
        // (EN) Build switch expression
        auto exprResult = buildExpression(switchStmt->expression.get());
        
        // (AR) إنشاء كتل لكل حالة وكتلة الخروج
        // (EN) Create blocks for each case and exit block
        std::string exitLabel = newLabel("switch_exit");
        auto exitBlock = createBasicBlock(exitLabel);
        
        std::string defaultLabel = exitLabel;
        if (switchStmt->defaultCase) {
            defaultLabel = newLabel("switch_default");
        }
        
        // (AR) بناء سلسلة من فروع شرطية لكل حالة
        // (EN) Build chain of conditional branches for each case
        std::vector<std::pair<std::string, std::shared_ptr<SIRBasicBlock>>> caseBlocks;
        
        for (size_t i = 0; i < switchStmt->cases.size(); ++i) {
            std::string caseLabel = newLabel("switch_case_" + std::to_string(i));
            auto caseBlock = createBasicBlock(caseLabel);
            caseBlocks.push_back({caseLabel, caseBlock});
        }
        
        // (AR) بناء شروط المقارنة
        // (EN) Build comparison conditions
        for (size_t i = 0; i < switchStmt->cases.size(); ++i) {
            auto& caseBranch = switchStmt->cases[i];
            
            // (AR) بناء تعبير قيمة الحالة
            // (EN) Build case value expression
            auto caseVal = buildExpression(caseBranch.value.get());
            
            // (AR) مقارنة المساواة
            // (EN) Equality comparison
            std::string cmpReg = newTempRegister();
            SIRInstruction cmpInst = SIRInstruction::Binary(
                SIROpcode::EQ,
                SIROperand::Register(cmpReg, SIRType::BOOL),
                SIROperand::Register(exprResult.registerName, exprResult.type),
                SIROperand::Register(caseVal.registerName, caseVal.type)
            );
            if (currentBlock_) {
                currentBlock_->addInstruction(cmpInst);
            }
            
            // (AR) التحديد: إذا تطابقت القيمة، اذهب لكتلة الحالة
            // (EN) Branch: if matched, go to case block
            std::string nextCheckLabel;
            if (i + 1 < switchStmt->cases.size()) {
                nextCheckLabel = newLabel("switch_check_" + std::to_string(i + 1));
            } else {
                nextCheckLabel = defaultLabel;
            }
            
            auto nextCheckBlock = createBasicBlock(nextCheckLabel);
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::BranchCond(
                    SIROperand::Register(cmpReg, SIRType::BOOL),
                    SIROperand::Label(caseBlocks[i].first),
                    SIROperand::Label(nextCheckLabel)
                ));
            }
            
            if (currentFunction_) {
                currentFunction_->addBasicBlock(nextCheckBlock);
            }
            currentBlock_ = nextCheckBlock;
        }
        
        // (AR) بناء أجسام الحالات
        // (EN) Build case bodies
        for (size_t i = 0; i < switchStmt->cases.size(); ++i) {
            if (currentFunction_) {
                currentFunction_->addBasicBlock(caseBlocks[i].second);
            }
            currentBlock_ = caseBlocks[i].second;
            
            enterScope();
            if (switchStmt->cases[i].body) {
                buildStatement(switchStmt->cases[i].body.get());
            }
            exitScope();
            
            // (AR) لا fall-through — القفز مباشرة للخروج
            // (EN) No fall-through — jump directly to exit
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
            }
        }
        
        // (AR) بناء الحالة الافتراضية
        // (EN) Build default case
        if (switchStmt->defaultCase) {
            auto defaultBlock = createBasicBlock(defaultLabel);
            if (currentFunction_) {
                currentFunction_->addBasicBlock(defaultBlock);
            }
            currentBlock_ = defaultBlock;
            
            enterScope();
            buildStatement(switchStmt->defaultCase.get());
            exitScope();
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
            }
        }
        
        // (AR) كتلة الخروج
        // (EN) Exit block
        if (currentFunction_) {
            currentFunction_->addBasicBlock(exitBlock);
        }
        currentBlock_ = exitBlock;
        return;
    }
    
    // ========================================================================
    // (AR) TryStmt - جملة حاول/التقط (statements.h:TryStmt)
    // (EN) Try-catch statement
    // ========================================================================
    if (auto tryStmt = dynamic_cast<Sad::AST::TryStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found TryStmt" << std::endl;
        #endif
        
        // (AR) إنشاء كتل: try، catch، finally، exit
        // (EN) Create blocks: try, catch, finally, exit
        std::string tryLabel = newLabel("try_body");
        std::string catchLabel = newLabel("catch_body");
        std::string finallyLabel = newLabel("finally_body");
        std::string exitLabel = newLabel("try_exit");
        
        auto tryBlock = createBasicBlock(tryLabel);
        auto catchBlock = createBasicBlock(catchLabel);
        auto finallyBlock = createBasicBlock(finallyLabel);
        auto exitBlock = createBasicBlock(exitLabel);
        
        // (AR) القفز إلى كتلة try
        // (EN) Jump to try block
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(tryLabel)));
        }
        
        // (AR) بناء كتلة try
        // (EN) Build try block
        if (currentFunction_) currentFunction_->addBasicBlock(tryBlock);
        currentBlock_ = tryBlock;
        
        enterScope();
        if (tryStmt->tryBlock) {
            buildStatement(tryStmt->tryBlock.get());
        }
        exitScope();
        
        // (AR) في النهاية العادية: اذهب لـ finally ثم exit
        // (EN) Normal end: go to finally then exit
        std::string normalDest = tryStmt->finallyBlock ? finallyLabel : exitLabel;
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(normalDest)));
        }
        
        // (AR) بناء كتل catch
        // (EN) Build catch blocks
        if (currentFunction_) currentFunction_->addBasicBlock(catchBlock);
        currentBlock_ = catchBlock;
        
        for (auto& catchClause : tryStmt->catchClauses) {
            enterScope();
            
            // (AR) تسجيل متغير الاستثناء
            // (EN) Register exception variable
            if (!catchClause.exceptionVar.empty()) {
                std::string exReg = newTempRegister();
                VariableInfo exVar;
                exVar.name = catchClause.exceptionVar;
                exVar.type = SIRType::STRING;
                exVar.registerName = exReg;
                exVar.isMutable = false;
                exVar.scopeLevel = currentScopeLevel_;
                addVariable(exVar);
                
                // (AR) تعليمة خاصة لتحميل الاستثناء
                // (EN) Special instruction to load exception
                SIRInstruction loadExInst;
                loadExInst.opcode = SIROpcode::CALL;
                loadExInst.result = SIROperand::Register(exReg, SIRType::STRING);
                loadExInst.operands.push_back(SIROperand::ConstantString("__sad_get_exception"));
                loadExInst.comment = "load caught exception into " + catchClause.exceptionVar;
                if (currentBlock_) currentBlock_->addInstruction(loadExInst);
            }
            
            if (catchClause.body) {
                buildStatement(catchClause.body.get());
            }
            
            exitScope();
        }
        
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::Branch(
                SIROperand::Label(tryStmt->finallyBlock ? finallyLabel : exitLabel)));
        }
        
        // (AR) بناء كتلة finally
        // (EN) Build finally block
        if (tryStmt->finallyBlock) {
            if (currentFunction_) currentFunction_->addBasicBlock(finallyBlock);
            currentBlock_ = finallyBlock;
            
            enterScope();
            buildStatement(tryStmt->finallyBlock.get());
            exitScope();
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
            }
        }
        
        // (AR) كتلة الخروج
        // (EN) Exit block
        if (currentFunction_) currentFunction_->addBasicBlock(exitBlock);
        currentBlock_ = exitBlock;
        return;
    }
    
    // ========================================================================
    // (AR) RaiseStmt - جملة رفع استثناء: ارفع خطأ("...")
    // (EN) Raise/throw statement
    // ========================================================================
    if (auto raiseStmt = dynamic_cast<Sad::AST::RaiseStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found RaiseStmt" << std::endl;
        #endif
        
        // (AR) بناء تعبير الاستثناء
        // (EN) Build the exception expression
        BuildResult exResult;
        if (raiseStmt->exception) {
            exResult = buildExpression(raiseStmt->exception.get());
        }
        
        // (AR) استدعاء دالة runtime لرفع الاستثناء
        // (EN) Call runtime function to raise exception
        SIRInstruction raiseInst;
        raiseInst.opcode = SIROpcode::CALL;
        raiseInst.operands.push_back(SIROperand::ConstantString("__sad_raise"));
        if (!exResult.registerName.empty()) {
            raiseInst.operands.push_back(SIROperand::Register(exResult.registerName, exResult.type));
        }
        raiseInst.comment = "raise exception";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(raiseInst);
        }
        
        // (AR) بعد الرفع، لا يمكن الوصول لهذه النقطة
        // (EN) After raise, this point is unreachable — emit ret void as terminator
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
        }
        
        // (AR) كتلة جديدة للكود بعد raise (كود ميت)
        // (EN) New block for code after raise (dead code)
        std::string afterRaiseLabel = newLabel("after_raise");
        auto afterRaiseBlock = createBasicBlock(afterRaiseLabel);
        if (currentFunction_) currentFunction_->addBasicBlock(afterRaiseBlock);
        currentBlock_ = afterRaiseBlock;
        return;
    }
    
    // ========================================================================
    // (AR) YieldStmt - جملة yield (مولّد): اعطِ قيمة
    // (EN) Yield statement (generator)
    // ========================================================================
    if (auto yieldStmt = dynamic_cast<Sad::AST::YieldStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found YieldStmt" << std::endl;
        #endif
        
        BuildResult valResult;
        if (yieldStmt->value) {
            valResult = buildExpression(yieldStmt->value.get());
        }
        
        // (AR) استدعاء دالة runtime للـ yield
        // (EN) Call runtime yield function
        SIRInstruction yieldInst;
        yieldInst.opcode = SIROpcode::CALL;
        yieldInst.operands.push_back(SIROperand::ConstantString(
            yieldStmt->isYieldFrom ? "__sad_yield_from" : "__sad_yield"));
        if (!valResult.registerName.empty()) {
            yieldInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
        }
        yieldInst.comment = yieldStmt->isYieldFrom ? "yield from" : "yield";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(yieldInst);
        }
        return;
    }
    
    // ========================================================================
    // (AR) WithStmt - جملة باستخدام (مدير سياق)
    // (EN) With statement (context manager)
    // ========================================================================
    if (auto withStmt = dynamic_cast<Sad::AST::WithStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found WithStmt" << std::endl;
        #endif
        
        enterScope();
        
        // (AR) بناء تعبير المورد
        // (EN) Build resource expression
        auto resResult = buildExpression(withStmt->resource.get());
        
        // (AR) تسجيل المتغير المستعار
        // (EN) Register alias variable
        if (!withStmt->alias.empty()) {
            VariableInfo aliasVar;
            aliasVar.name = withStmt->alias;
            aliasVar.type = resResult.type;
            aliasVar.registerName = resResult.registerName;
            aliasVar.isMutable = false;
            aliasVar.scopeLevel = currentScopeLevel_;
            addVariable(aliasVar);
        }
        
        // (AR) استدعاء __دخول__() على المورد
        // (EN) Call __enter__() on resource
        SIRInstruction enterInst;
        enterInst.opcode = SIROpcode::CALL;
        enterInst.operands.push_back(SIROperand::ConstantString("__sad_context_enter"));
        enterInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
        enterInst.comment = "context manager __enter__";
        if (currentBlock_) currentBlock_->addInstruction(enterInst);
        
        // (AR) بناء جسم الكتلة
        // (EN) Build block body
        if (withStmt->body) {
            buildStatement(withStmt->body.get());
        }
        
        // (AR) استدعاء __خروج__() على المورد
        // (EN) Call __exit__() on resource
        SIRInstruction exitInst;
        exitInst.opcode = SIROpcode::CALL;
        exitInst.operands.push_back(SIROperand::ConstantString("__sad_context_exit"));
        exitInst.operands.push_back(SIROperand::Register(resResult.registerName, resResult.type));
        exitInst.comment = "context manager __exit__";
        if (currentBlock_) currentBlock_->addInstruction(exitInst);
        
        exitScope();
        return;
    }
    
    // ========================================================================
    // (AR) EnumDecl - تصريح تعداد: تعداد اللون { أحمر، أخضر، أزرق }
    // (EN) Enum declaration
    // ========================================================================
    if (auto enumDecl = dynamic_cast<Sad::AST::EnumDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found EnumDecl: " << enumDecl->name << std::endl;
        #endif
        
        // (AR) تحويل التعداد إلى ثوابت عددية صحيحة
        // (EN) Lower enum to integer constants
        for (size_t i = 0; i < enumDecl->members.size(); ++i) {
            const auto& member = enumDecl->members[i];
            std::string fullName = enumDecl->name + "." + member.name;
            
            // (AR) تحديد القيمة: إما صريحة أو تلقائية
            // (EN) Determine value: explicit or automatic
            int64_t enumValue = static_cast<int64_t>(i);
            if (member.value) {
                auto valResult = buildExpression(member.value.get());
                if (valResult.isConstant) {
                    try {
                        enumValue = std::stoll(valResult.constantValue);
                    } catch (const std::exception&) {
                        // إصلاح: استخدام القيمة الافتراضية عند فشل التحويل
                        // enumValue = i (already set)
                    }
                }
            }
            
            // (AR) إنشاء متغير عام للثابت
            // (EN) Create global variable for constant
            std::string regName = "%" + fullName;
            
            if (module_) {
                auto global = std::make_shared<SIRGlobalVariable>(
                    fullName, SIRType::I64, std::to_string(enumValue), true);
                module_->addGlobalVariable(global);
            }
            
            // (AR) تسجيل كثابت في النطاق
            // (EN) Register as constant in scope
            VariableInfo enumVar;
            enumVar.name = fullName;
            enumVar.type = SIRType::I64;
            enumVar.registerName = regName;
            enumVar.isGlobal = true;
            enumVar.isMutable = false;
            enumVar.scopeLevel = 0;
            addVariable(enumVar);
        }
        return;
    }
    
    // ========================================================================
    // (AR) StructDecl - تصريح بنية: بنية نقطة { عشري س، عشري ي }
    // (EN) Struct declaration
    // ========================================================================
    if (auto structDecl = dynamic_cast<Sad::AST::StructDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found StructDecl: " << structDecl->name << std::endl;
        #endif
        
        // (AR) إنشاء "صنف" SIR بدون وراثة لتمثيل البنية
        // (EN) Create SIR "class" without inheritance to represent struct
        auto sirClass = std::make_shared<SIRClass>(structDecl->name, "");
        
        // (AR) إضافة الحقول
        // (EN) Add fields
        for (const auto& field : structDecl->fields) {
            SIRType fieldType = SIRType::I64;
            switch (field.type) {
                case Sad::Data::DataType::INTEGER:  fieldType = SIRType::I64;    break;
                case Sad::Data::DataType::FLOAT:    fieldType = SIRType::F64;    break;
                case Sad::Data::DataType::BOOLEAN:  fieldType = SIRType::BOOL;   break;
                case Sad::Data::DataType::STRING:   fieldType = SIRType::STRING; break;
                case Sad::Data::DataType::ARRAY:    fieldType = SIRType::ARRAY;  break;
                default:                            fieldType = SIRType::I64;    break;
            }
            sirClass->addField(field.name, fieldType);
        }
        
        // (AR) إضافة الدوال المنفذة (إن وجدت)
        // (EN) Add implemented methods (if any)
        for (auto& methodStmt : structDecl->methods) {
            buildStatement(methodStmt.get());
        }
        
        if (module_) {
            module_->addClass(sirClass);
        }
        
        // (AR) تسجيل الصنف في الجدول
        // (EN) Register class in table
        classTable_[structDecl->name] = sirClass;
        return;
    }
    
    // ========================================================================
    // (AR) NamespaceDecl - فضاء أسماء: فضاء رياضيات ... نهاية_فضاء
    // (EN) Namespace declaration
    // ========================================================================
    if (auto nsDecl = dynamic_cast<Sad::AST::NamespaceDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found NamespaceDecl: " << nsDecl->name << std::endl;
        #endif
        
        // (AR) بناء أعضاء الفضاء (الدوال، الأصناف، المتغيرات)
        // (EN) Build namespace members (functions, classes, variables)
        enterScope();
        
        for (const auto& member : nsDecl->members) {
            if (!member) continue;
            
            // (AR) محاولة بناء كتعريف عام أو جملة
            // (EN) Try to build as top-level declaration or statement
            auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl*>(member.get());
            auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt*>(member.get());
            auto classDecl = dynamic_cast<Sad::AST::ClassDecl*>(member.get());
            
            if (funcDecl) {
                // (AR) بناء الدالة مع بادئة الفضاء
                // (EN) Build function with namespace prefix
                std::string originalName = funcDecl->name;
                funcDecl->name = nsDecl->name + "::" + originalName;
                buildFunction(funcDecl);
                funcDecl->name = originalName;
            } else if (varDecl) {
                buildGlobalVariable(varDecl);
            } else if (classDecl) {
                std::string originalName = classDecl->name;
                classDecl->name = nsDecl->name + "::" + originalName;
                buildClass(classDecl);
                classDecl->name = originalName;
            } else {
                buildStatement(member.get());
            }
        }
        
        exitScope();
        return;
    }
    
    // ========================================================================
    // (AR) ClassDeclStmt - تصريح صنف (الصيغة البديلة)
    // (EN) ClassDeclStmt - alternate class declaration syntax
    // ========================================================================
    if (auto classDeclStmt = dynamic_cast<Sad::AST::ClassDeclStmt*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found ClassDeclStmt: " << classDeclStmt->name << std::endl;
        #endif
        
        // (AR) تحويل ClassDeclStmt إلى SIRClass
        // (EN) Convert ClassDeclStmt to SIRClass
        std::string parentClass;
        if (!classDeclStmt->baseClasses.empty()) {
            parentClass = classDeclStmt->baseClasses[0];
        }
        
        auto sirClass = std::make_shared<SIRClass>(classDeclStmt->name, parentClass);
        
        // (AR) معالجة الحقول
        // (EN) Process fields
        for (const auto& field : classDeclStmt->fields) {
            SIRType fieldType = SIRType::I64;
            sirClass->addField(field->name, fieldType);
        }
        
        // (AR) حفظ السياق
        // (EN) Save context
        std::string savedClassName = currentClassName_;
        currentClassName_ = classDeclStmt->name;
        
        // (AR) معالجة الباني
        // (EN) Process constructor
        if (classDeclStmt->constructor) {
            auto ctorDecl = classDeclStmt->constructor.get();
            std::string ctorName = classDeclStmt->name + "::باني";
            
            std::vector<SIRParameter> ctorParams;
            ctorParams.push_back(SIRParameter("self", SIRType::PTR));
            if (auto ctor = dynamic_cast<Sad::AST::ConstructorDecl*>(ctorDecl)) {
                for (const auto& p : ctor->parameters) {
                    ctorParams.push_back(SIRParameter(p.name, SIRType::I64));
                }
            }
            
            auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SIRType::VOID);
            for (const auto& cp : ctorParams) ctorFunc->addParameter(cp);
            auto savedFunc = currentFunction_;
            auto savedBlock = currentBlock_;
            
            currentFunction_ = ctorFunc;
            auto entryBlock = createBasicBlock("ctor_entry");
            ctorFunc->addBasicBlock(entryBlock);
            currentBlock_ = entryBlock;
            
            enterScope();
            VariableInfo selfVar;
            selfVar.name = "self";
            selfVar.type = SIRType::PTR;
            selfVar.registerName = "%self";
            addVariable(selfVar);
            
            if (auto ctor = dynamic_cast<Sad::AST::ConstructorDecl*>(ctorDecl)) {
                if (ctor->body) buildStatement(ctor->body.get());
            }
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
            }
            exitScope();
            
            if (module_) module_->addFunction(ctorFunc);
            sirClass->addMethod(ctorFunc);
            
            currentFunction_ = savedFunc;
            currentBlock_ = savedBlock;
        }
        
        // (AR) معالجة الدوال
        // (EN) Process methods
        for (const auto& method : classDeclStmt->methods) {
            if (auto methodDecl = dynamic_cast<Sad::AST::MethodDecl*>(method.get())) {
                std::string methodName = classDeclStmt->name + "::" + methodDecl->name;
                
                std::vector<SIRParameter> methodParams;
                methodParams.push_back(SIRParameter("self", SIRType::PTR));
                for (const auto& p : methodDecl->parameters) {
                    methodParams.push_back(SIRParameter(p.name, SIRType::I64));
                }
                
                auto methodFunc = std::make_shared<SIRFunction>(methodName, SIRType::I64);
                for (const auto& mp : methodParams) methodFunc->addParameter(mp);
                auto savedFunc = currentFunction_;
                auto savedBlock = currentBlock_;
                
                currentFunction_ = methodFunc;
                auto entryBlock = createBasicBlock("method_entry");
                methodFunc->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                enterScope();
                VariableInfo selfVar;
                selfVar.name = "self";
                selfVar.type = SIRType::PTR;
                selfVar.registerName = "%self";
                addVariable(selfVar);
                
                for (const auto& p : methodDecl->parameters) {
                    VariableInfo pVar;
                    pVar.name = p.name;
                    pVar.type = SIRType::I64;
                    pVar.registerName = "%" + p.name;
                    addVariable(pVar);
                }
                
                if (methodDecl->body) buildStatement(methodDecl->body.get());
                exitScope();
                
                if (module_) module_->addFunction(methodFunc);
                sirClass->addMethod(methodFunc);
                
                currentFunction_ = savedFunc;
                currentBlock_ = savedBlock;
            }
        }
        
        currentClassName_ = savedClassName;
        
        if (module_) module_->addClass(sirClass);
        classTable_[classDeclStmt->name] = sirClass;
        return;
    }
    
    // ========================================================================
    // (AR) FunctionDecl داخل جسم دالة (دوال متداخلة)
    // (EN) FunctionDecl inside function body (nested function)
    // ========================================================================
    if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found nested FunctionDecl: " << funcDecl->name << std::endl;
        #endif
        buildFunction(funcDecl);
        return;
    }
    
    // ========================================================================
    // (AR) ClassDecl داخل جسم (صنف متداخل)
    // (EN) ClassDecl inside body (nested class)
    // ========================================================================
    if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found nested ClassDecl: " << classDecl->name << std::endl;
        #endif
        buildClass(classDecl);
        return;
    }
    
    // ========================================================================
    // (AR) DestructorDecl - هادم الصنف (declarations.h:DestructorDecl)
    // (EN) Destructor declaration — lowered to a special __destructor function
    // ========================================================================
    if (auto destructorDecl = dynamic_cast<Sad::AST::DestructorDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found DestructorDecl" << std::endl;
        #endif
        
        // (AR) بناء دالة هادم خاصة: __destructor_<className>
        // (EN) Build special destructor function: __destructor_<className>
        std::string dtorName = "__destructor";
        auto dtorFunc = std::make_shared<SIRFunction>(dtorName, SIRType::VOID);
        
        // (AR) إضافة معامل this
        // (EN) Add 'this' parameter
        dtorFunc->addParameter(SIRParameter("this", SIRType::PTR));
        
        auto entryBlock = createBasicBlock("destructor_entry");
        dtorFunc->addBasicBlock(entryBlock);
        
        auto savedFunction = currentFunction_;
        auto savedBlock = currentBlock_;
        currentFunction_ = dtorFunc;
        currentBlock_ = entryBlock;
        
        enterScope();
        if (destructorDecl->body) {
            buildStatement(destructorDecl->body.get());
        }
        exitScope();
        
        if (currentBlock_) {
            currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
        }
        
        currentFunction_ = savedFunction;
        currentBlock_ = savedBlock;
        module_->addFunction(dtorFunc);
        return;
    }
    
    // ========================================================================
    // (AR) TestDecl - إعلان اختبار (declarations.h:TestDecl)
    // (EN) Test declaration — lowered to a __test_<name> function
    // ========================================================================
    if (auto testDecl = dynamic_cast<Sad::AST::TestDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found TestDecl: " << testDecl->testName << std::endl;
        #endif
        
        // (AR) بناء دالة اختبار: __test_<name>
        // (EN) Build test function: __test_<name>
        std::string testFuncName = "__test_" + testDecl->testName;
        auto testFunc = std::make_shared<SIRFunction>(testFuncName, SIRType::BOOL);
        
        auto entryBlock = createBasicBlock("test_entry");
        testFunc->addBasicBlock(entryBlock);
        
        auto savedFunction = currentFunction_;
        auto savedBlock = currentBlock_;
        currentFunction_ = testFunc;
        currentBlock_ = entryBlock;
        
        enterScope();
        if (testDecl->body) {
            buildStatement(testDecl->body.get());
        }
        exitScope();
        
        // (AR) ارجاع true (نجاح الاختبار) افتراضياً
        // (EN) Return true (test passed) by default
        if (currentBlock_) {
            if (testDecl->shouldFail) {
                // (AR) إذا كان الاختبار يجب أن يفشل، ارجع false
                // (EN) If test should fail, return false
                currentBlock_->addInstruction(SIRInstruction::Return(
                    SIROperand::ConstantBool(false)));
            } else {
                currentBlock_->addInstruction(SIRInstruction::Return(
                    SIROperand::ConstantBool(true)));
            }
        }
        
        currentFunction_ = savedFunction;
        currentBlock_ = savedBlock;
        module_->addFunction(testFunc);
        return;
    }
    
    // ========================================================================
    // (AR) AtomicDecl - متغير ذري (declarations.h:AtomicDecl)
    // (EN) Atomic variable declaration — lowered to regular variable with atomic hint
    // ========================================================================
    if (auto atomicDecl = dynamic_cast<Sad::AST::AtomicDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found AtomicDecl: " << atomicDecl->name << std::endl;
        #endif
        
        // (AR) تحويل النوع الداخلي إلى SIRType
        // (EN) Convert inner type to SIRType
        SIRType varType = astTypeToSIRType(atomicDecl->innerType);
        
        // (AR) تخصيص مكان في الذاكرة
        // (EN) Allocate memory
        std::string reg = newTempRegister();
        if (currentBlock_) {
            SIRInstruction allocInst;
            allocInst.opcode = SIROpcode::ALLOC;
            allocInst.result = SIROperand::Register(reg, varType);
            currentBlock_->addInstruction(allocInst);
        }
        
        // (AR) تهيئة القيمة إن وُجدت
        // (EN) Initialize value if present
        if (atomicDecl->initialValue) {
            auto initResult = buildExpression(atomicDecl->initialValue.get());
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::Store(
                    SIROperand::Register(reg, varType),
                    SIROperand::Register(initResult.registerName, initResult.type)));
            }
        }
        
        // (AR) تسجيل المتغير في النطاق الحالي
        // (EN) Register variable in current scope
        VariableInfo varInfo;
        varInfo.name = atomicDecl->name;
        varInfo.type = varType;
        varInfo.registerName = reg;
        varInfo.isGlobal = false;
        varInfo.isMutable = true;
        addVariable(varInfo);
        return;
    }
    
    // ========================================================================
    // (AR) PropertyDecl - خاصية مع getter/setter (property_nodes.h:PropertyDecl)
    // (EN) Property declaration — lowered to getter/setter functions
    // ========================================================================
    if (auto propDecl = dynamic_cast<Sad::AST::PropertyDecl*>(stmt)) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] Found PropertyDecl: " << propDecl->name << std::endl;
        #endif
        
        SIRType propType = astTypeToSIRType(propDecl->type);
        
        // (AR) بناء دالة getter إن وُجدت
        // (EN) Build getter function if present
        if (propDecl->getter) {
            std::string getterName = "__get_" + propDecl->name;
            auto getterFunc = std::make_shared<SIRFunction>(getterName, propType);
            
            // (AR) معامل this
            // (EN) this parameter
            getterFunc->addParameter(SIRParameter("this", SIRType::PTR));
            
            auto entryBlock = createBasicBlock("getter_entry");
            getterFunc->addBasicBlock(entryBlock);
            
            auto savedFunction = currentFunction_;
            auto savedBlock = currentBlock_;
            currentFunction_ = getterFunc;
            currentBlock_ = entryBlock;
            
            enterScope();
            if (propDecl->getter->body) {
                buildStatement(propDecl->getter->body.get());
            }
            exitScope();
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
            }
            
            currentFunction_ = savedFunction;
            currentBlock_ = savedBlock;
            module_->addFunction(getterFunc);
        }
        
        // (AR) بناء دالة setter إن وُجدت
        // (EN) Build setter function if present
        if (propDecl->setter) {
            std::string setterName = "__set_" + propDecl->name;
            auto setterFunc = std::make_shared<SIRFunction>(setterName, SIRType::VOID);
            
            // (AR) معامل this + معامل القيمة
            // (EN) this parameter + value parameter
            setterFunc->addParameter(SIRParameter("this", SIRType::PTR));
            setterFunc->addParameter(SIRParameter(propDecl->setter->parameterName, propType));
            
            auto entryBlock = createBasicBlock("setter_entry");
            setterFunc->addBasicBlock(entryBlock);
            
            auto savedFunction = currentFunction_;
            auto savedBlock = currentBlock_;
            currentFunction_ = setterFunc;
            currentBlock_ = entryBlock;
            
            enterScope();
            // (AR) تسجيل معامل القيمة في النطاق
            // (EN) Register value parameter in scope
            VariableInfo paramInfo;
            paramInfo.name = propDecl->setter->parameterName;
            paramInfo.type = propType;
            paramInfo.registerName = propDecl->setter->parameterName;
            paramInfo.isGlobal = false;
            paramInfo.isMutable = true;
            addVariable(paramInfo);
            
            if (propDecl->setter->body) {
                buildStatement(propDecl->setter->body.get());
            }
            exitScope();
            
            if (currentBlock_) {
                currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
            }
            
            currentFunction_ = savedFunction;
            currentBlock_ = savedBlock;
            module_->addFunction(setterFunc);
        }
        return;
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] Unknown statement type: " << typeid(*stmt).name() << std::endl;
    #endif
    // (AR) جملة غير معروفة - نتجاهلها
    // (EN) Unknown statement - ignore
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
