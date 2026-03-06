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
    
    // (AR) إذا كنا داخل كوروتين، نستخدم CORO_RETURN بدلاً من RET
    // (EN) Inside a coroutine, use CORO_RETURN instead of RET
    if (currentFunction_ && currentFunction_->isCoroutine && retStmt->value) {
        BuildResult valueResult = buildExpression(retStmt->value.get());
        SIRInstruction coroRet;
        coroRet.opcode = SIROpcode::CORO_RETURN;
        if (valueResult.isConstant && !valueResult.constantValue.empty()) {
            switch (valueResult.type) {
                case SIRType::I64:
                    coroRet.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                    break;
                case SIRType::F64:
                    coroRet.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                    break;
                case SIRType::STRING:
                    coroRet.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                    break;
                default:
                    coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
            }
        } else {
            coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
        }
        if (currentBlock_) currentBlock_->addInstruction(coroRet);
        return;
    }
    
    // (AR) ReturnStmt::value: ExprPtr (statements.h:268)
    // (EN) Build return instruction
    if (retStmt->value) {
        // (AR) بناء تعبير القيمة المُرجعة
        // (EN) Build return value expression
        BuildResult valueResult = buildExpression(retStmt->value.get());
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildReturnStatement: registerName='" << valueResult.registerName 
                  << "', isConstant=" << valueResult.isConstant 
                  << ", constantValue='" << valueResult.constantValue << "'" << std::endl;
        #endif
        
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
    
    // ========================================================================
    // (AR) جملة break: القفز إلى نهاية الحلقة الحالية
    //      نستخدم مكدس الحلقات (loopStack_) للحصول على تسمية كتلة الخروج
    //      ثم نولّد تعليمة قفز غير شرطي (BR) إلى تلك الكتلة
    //
    // (EN) break statement: Jump to the end of current loop
    //      We use the loop stack (loopStack_) to get the exit block label
    //      Then generate an unconditional branch (BR) to that block
    // ========================================================================
    LoopContext* loop = getCurrentLoop();
    if (!loop) {
        errors_.push_back("(AR) خطأ: جملة 'قف' خارج حلقة. (EN) Error: 'break' outside of loop.");
        return;
    }
    
    // (AR) توليد قفز غير شرطي إلى كتلة خروج الحلقة
    // (EN) Generate unconditional branch to loop exit block
    SIROperand exitLabel = SIROperand::Label(loop->breakLabel);
    SIRInstruction brInst = SIRInstruction::Branch(exitLabel);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brInst);
    }
    
    // (AR) إنشاء كتلة جديدة للكود بعد break (كود ميت)
    //      هذا ضروري لأن LLVM يتطلب أن كل كتلة تنتهي بمُنهِي واحد فقط
    // (EN) Create new block for code after break (dead code)
    //      Required because LLVM needs each block to end with exactly one terminator
    std::string afterBreakLabel = newLabel("after_break");
    auto afterBreakBlock = createBasicBlock(afterBreakLabel);
    if (currentFunction_) {
        currentFunction_->addBasicBlock(afterBreakBlock);
    }
    currentBlock_ = afterBreakBlock;
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
    
    // ========================================================================
    // (AR) جملة continue: القفز إلى بداية تكرار الحلقة التالي
    //      - في حلقة while: نقفز إلى كتلة الشرط (while_cond)
    //      - في حلقة for: نقفز إلى كتلة الزيادة (for_inc) 
    //        ثم الزيادة ستقفز بدورها للشرط
    //      نستخدم continueLabel من مكدس الحلقات
    //
    // (EN) continue statement: Jump to next loop iteration
    //      - In while loop: jump to condition block (while_cond)
    //      - In for loop: jump to increment block (for_inc)
    //        then increment jumps to condition
    //      We use continueLabel from the loop stack
    // ========================================================================
    LoopContext* loop = getCurrentLoop();
    if (!loop) {
        errors_.push_back("(AR) خطأ: جملة 'أكمل' خارج حلقة. (EN) Error: 'continue' outside of loop.");
        return;
    }
    
    // (AR) توليد قفز غير شرطي إلى كتلة استمرار الحلقة
    // (EN) Generate unconditional branch to loop continue block
    SIROperand continueLabel = SIROperand::Label(loop->continueLabel);
    SIRInstruction brInst = SIRInstruction::Branch(continueLabel);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brInst);
    }
    
    // (AR) إنشاء كتلة جديدة للكود بعد continue (كود ميت)
    //      هذا ضروري لأن LLVM يتطلب أن كل كتلة تنتهي بمُنهِي واحد فقط
    // (EN) Create new block for code after continue (dead code)
    //      Required because LLVM needs each block to end with exactly one terminator
    std::string afterContinueLabel = newLabel("after_continue");
    auto afterContinueBlock = createBasicBlock(afterContinueLabel);
    if (currentFunction_) {
        currentFunction_->addBasicBlock(afterContinueBlock);
    }
    currentBlock_ = afterContinueBlock;
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
                try { valueOp.intValue = std::stoll(valueResult.constantValue); } 
                catch(const std::exception&) { 
                    valueOp.intValue = 0; 
                    // إصلاح: تسجيل فشل التحويل للتصحيح
                }
            } else if (valueResult.type == SIRType::F64) {
                try { valueOp.floatValue = std::stod(valueResult.constantValue); } 
                catch(const std::exception&) { 
                    valueOp.floatValue = 0.0; 
                    // إصلاح: تسجيل فشل التحويل للتصحيح
                }
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
    
    // (AR) تحديث اسم اللامدا المستعار عند إعادة تعيين دالة لامدا لمتغير
    // (EN) Update lambda alias when reassigning a lambda to a variable
    if (valueResult.type == SIRType::FUNCTION && !valueResult.constantValue.empty()) {
        lambdaAliases_[assignment->name] = valueResult.constantValue;
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
    
    std::cerr << "[EXC-DBG] buildLocalVariable: name='" << varDecl->name 
              << "' type=" << static_cast<int>(varDecl->type) << std::endl;
    
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
        std::cerr << "[EXC-DBG]   building initializer, expr type=" 
                  << typeid(*varDecl->initializer).name() << std::endl;
        initResult = buildExpression(varDecl->initializer.get());
        std::cerr << "[EXC-DBG]   initResult: reg='" << initResult.registerName 
                  << "' type=" << static_cast<int>(initResult.type) 
                  << " isConst=" << initResult.isConstant 
                  << " constVal='" << initResult.constantValue << "'" << std::endl;
        
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
        // (AR) تحقق: هل القيمة ثابتة يمكن استخدامها مباشرة؟
        // (EN) Check: is the value a usable constant?
        bool useConstant = initResult.isConstant && (
            initResult.type == SIRType::STRING ||
            !initResult.constantValue.empty()
        );
        
        // (AR) تخطي STORE إذا لم تكن القيمة ثابتة ولا في سجل صالح
        //      هذا يحدث عند فشل buildExpression (مثلاً: متغير غير معرّف)
        // (EN) Skip STORE if value is neither a usable constant nor in a valid register.
        //      This happens when buildExpression fails (e.g., undefined variable).
        if (!useConstant && initResult.registerName.empty()) {
            // Just register the variable without initialization
            addVariable(varInfo);
            return;
        }
        
        SIRInstruction storeInst;
        storeInst.opcode = SIROpcode::STORE;
        
        // (AR) المعامل الأول: القيمة المراد تخزينها
        // (EN) First operand: value to store
        if (useConstant) {
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
    
    // (AR) تسجيل اسم اللامدا المستعار عند تعيين دالة لامدا لمتغير
    // (EN) Register lambda alias when assigning a lambda to a variable
    // مثال: متغير ف = لامدا(س): س + 1 → lambdaAliases_["ف"] = "__lambda_0"
    if (hasInitializer && initResult.type == SIRType::FUNCTION && !initResult.constantValue.empty()) {
        lambdaAliases_[varDecl->name] = initResult.constantValue;
        std::cerr << "[LAMBDA] Registered alias: '" << varDecl->name 
                  << "' -> '" << initResult.constantValue << "'" << std::endl;
    }
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildIfStatement: starting" << std::endl;
    #endif
    
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildIfStatement: created blocks then=" << thenLabel 
              << ", else=" << elseLabel << ", merge=" << mergeLabel << std::endl;
    #endif
    
    // ========================================================================
    // (AR) الخطوة 2: بناء الشرط
    // (EN) Step 2: Build condition expression
    // المصدر: IfStmt::condition (statements.h:107)
    // ========================================================================
    auto condResult = buildExpression(ifStmt->condition.get());
    
    if (condResult.registerName.empty()) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildIfStatement: condition build failed!" << std::endl;
        #endif
        errors_.push_back("Error: Failed to build if condition");
        return;
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildIfStatement: condition reg=" << condResult.registerName << std::endl;
    #endif
    
    // ========================================================================
    // (AR) الخطوة 3: توليد تعليمة القفز الشرطي
    // (EN) Step 3: Generate conditional branch instruction
    // المصدر: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
    // المصدر: sir_types.h:366-372 - SIROperand::Label()
    // ========================================================================
    // (AR) إذا كان الشرط ثابتاً منطقياً (صحيح/خطأ)، نستخدم ConstantBool بدلاً من Register
    //      لتجنب توليد سجل غير معرّف في LLVM IR
    // (EN) If condition is a boolean constant (true/false), use ConstantBool instead of Register
    //      to avoid generating an undefined register in LLVM IR
    SIROperand condOp;
    if (condResult.isConstant && condResult.type == SIRType::BOOL) {
        condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
    } else if (condResult.isConstant && condResult.type == SIRType::I64) {
        condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
    } else {
        condOp = SIROperand::Register(condResult.registerName, condResult.type);
    }
    SIROperand thenLabelOp = SIROperand::Label(thenLabel);
    SIROperand elseLabelOp = SIROperand::Label(elseLabel);
    
    SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, thenLabelOp, elseLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildIfStatement: added BR_COND to current block" << std::endl;
        #endif
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
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildIfStatement: added BR to merge from then" << std::endl;
            #endif
        } else {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildIfStatement: then block already has terminator, skipping BR" << std::endl;
            #endif
        }
    } else if (currentBlock_) {
        currentBlock_->instructions.push_back(brMergeInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildIfStatement: added BR to merge from then (empty block)" << std::endl;
        #endif
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
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: added BR to merge from else" << std::endl;
                #endif
            } else {
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: else block already has terminator, skipping BR" << std::endl;
                #endif
            }
        } else if (currentBlock_) {
            currentBlock_->instructions.push_back(brMergeInst);
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildIfStatement: added BR to merge from else (empty block)" << std::endl;
            #endif
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 6: الاستمرار بعد if
    // (EN) Step 6: Continue after if statement
    // ========================================================================
    currentBlock_ = mergeBlock;
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildIfStatement: completed, now at merge block" << std::endl;
    #endif
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMatchStatement: starting with " 
              << matchStmt->cases.size() << " cases" << std::endl;
    #endif
    
    // ========================================================================
    // (AR) الخطوة 1: تقييم القيمة المُطابقة
    // (EN) Step 1: Evaluate the match value
    // ========================================================================
    auto matchResult = buildExpression(matchStmt->value.get());
    
    if (matchResult.registerName.empty() && !matchResult.isConstant) {
        errors_.push_back("Error: Failed to build match expression");
        return;
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMatchStatement: match value reg=" 
              << matchResult.registerName << ", isConst=" << matchResult.isConstant << std::endl;
    #endif
    
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
                
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: case " << i << " is WildcardPattern" << std::endl;
                #endif
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
                
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: case " << i 
                          << " is VariablePattern(" << varPat->name << ")" << std::endl;
                #endif
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
                
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: case " << i 
                          << " is LiteralPattern(" << litValue << ")" << std::endl;
                #endif
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
                
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: case " << i << " is OrPattern" << std::endl;
                #endif
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
                
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: case " << i << " is unknown pattern" << std::endl;
                #endif
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
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildMatchStatement: generated case " << i << std::endl;
        #endif
    }   // end for each case
    
    // ========================================================================
    // (AR) الخطوة 6: الاستمرار بعد match
    // (EN) Step 6: Continue after match statement
    // ========================================================================
    currentBlock_ = mergeBlock;
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMatchStatement: completed" << std::endl;
    #endif
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildWhileLoop: starting" << std::endl;
    #endif
    
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildWhileLoop: created blocks cond=" << condLabel 
              << ", body=" << bodyLabel << ", exit=" << exitLabel << std::endl;
    #endif
    
    // ========================================================================
    // (AR) الخطوة 2: قفز غير شرطي إلى كتلة الشرط
    // (EN) Step 2: Unconditional jump to condition block
    // المصدر: sir_instruction.h:178-183 - SIRInstruction::Branch()
    // ========================================================================
    SIROperand condLabelOp = SIROperand::Label(condLabel);
    SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondBlockInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildWhileLoop: added BR to condition block" << std::endl;
        #endif
    }
    
    // ========================================================================
    // (AR) الخطوة 3: بناء الشرط
    // (EN) Step 3: Build condition expression
    // المصدر: WhileStmt::condition (statements.h:149)
    // ========================================================================
    currentBlock_ = condBlock;
    auto condResult = buildExpression(whileLoop->condition.get());
    
    if (condResult.registerName.empty()) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildWhileLoop: condition build failed!" << std::endl;
        #endif
        errors_.push_back("Error: Failed to build while condition");
        return;
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildWhileLoop: condition reg=" << condResult.registerName << std::endl;
    #endif
    
    // ========================================================================
    // (AR) الخطوة 4: توليد تعليمة القفز الشرطي
    // (EN) Step 4: Generate conditional branch instruction
    // المصدر: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
    // ========================================================================
    // (AR) إذا كان الشرط ثابتاً منطقياً (صحيح/خطأ)، نستخدم ConstantBool بدلاً من Register
    //      لتجنب توليد سجل غير معرّف في LLVM IR
    // (EN) If condition is a boolean constant (true/false), use ConstantBool instead of Register
    //      to avoid generating an undefined register in LLVM IR
    SIROperand condOp;
    if (condResult.isConstant && condResult.type == SIRType::BOOL) {
        condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
    } else if (condResult.isConstant && condResult.type == SIRType::I64) {
        condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
    } else {
        condOp = SIROperand::Register(condResult.registerName, condResult.type);
    }
    SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
    SIROperand exitLabelOp = SIROperand::Label(exitLabel);
    
    SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildWhileLoop: added BR_COND (body/exit)" << std::endl;
        #endif
    }
    
    // ========================================================================
    // (AR) الخطوة 4.5: تسجيل سياق الحلقة لدعم break/continue
    //      continueLabel = كتلة الشرط (while_cond) — continue يقفز للشرط
    //      breakLabel = كتلة الخروج (while_exit) — break يقفز للخروج
    // (EN) Step 4.5: Register loop context for break/continue support
    //      continueLabel = condition block (while_cond) — continue jumps to condition
    //      breakLabel = exit block (while_exit) — break jumps to exit
    // ========================================================================
    LoopContext whileLoopCtx;
    whileLoopCtx.continueLabel = condLabel;
    whileLoopCtx.breakLabel = exitLabel;
    enterLoop(whileLoopCtx);
    
    // ========================================================================
    // (AR) الخطوة 5: بناء جسم الحلقة
    // (EN) Step 5: Build loop body
    // المصدر: WhileStmt::body (statements.h:150)
    // ========================================================================
    currentBlock_ = bodyBlock;
    if (whileLoop->body) {
        buildStatement(whileLoop->body.get());
    }
    
    // (AR) الخروج من سياق الحلقة بعد بناء الجسم
    // (EN) Exit loop context after building body
    exitLoop();
    
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
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildWhileLoop: added BR back to condition" << std::endl;
            #endif
        } else {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildWhileLoop: body block already has terminator, skipping BR" << std::endl;
            #endif
        }
    } else if (currentBlock_) {
        currentBlock_->instructions.push_back(brBackInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildWhileLoop: added BR back to condition (empty block)" << std::endl;
        #endif
    }
    
    // ========================================================================
    // (AR) الخطوة 7: الاستمرار بعد الحلقة
    // (EN) Step 7: Continue after loop
    // ========================================================================
    currentBlock_ = exitBlock;
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildWhileLoop: completed, now at exit block" << std::endl;
    #endif
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForLoop: starting" << std::endl;
    #endif
    
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
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildForLoop: building initializer" << std::endl;
        #endif
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForLoop: created blocks cond=" << condLabel 
              << ", body=" << bodyLabel << ", inc=" << incLabel 
              << ", exit=" << exitLabel << std::endl;
    #endif
    
    // ========================================================================
    // (AR) الخطوة 4: قفز غير شرطي إلى كتلة الشرط
    // (EN) Step 4: Unconditional jump to condition block
    // ========================================================================
    SIROperand condLabelOp = SIROperand::Label(condLabel);
    SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brCondBlockInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildForLoop: added BR to condition block" << std::endl;
        #endif
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
            // (AR) إذا كان الشرط ثابتاً منطقياً، نستخدم ConstantBool بدلاً من Register
            // (EN) If condition is a boolean constant, use ConstantBool instead of Register
            SIROperand condOp;
            if (condResult.isConstant && condResult.type == SIRType::BOOL) {
                condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
            } else if (condResult.isConstant && condResult.type == SIRType::I64) {
                condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
            } else {
                condOp = SIROperand::Register(condResult.registerName, condResult.type);
            }
            SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, bodyLabelOp, exitLabelOp);
            
            if (currentBlock_) {
                currentBlock_->instructions.push_back(brCondInst);
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildForLoop: added BR_COND (body/exit)" << std::endl;
                #endif
            }
        }
    } else {
        // (AR) لا يوجد شرط - قفز غير شرطي لـ body (حلقة لا نهائية)
        // (EN) No condition - unconditional jump to body (infinite loop)
        SIRInstruction brBodyInst = SIRInstruction::Branch(bodyLabelOp);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(brBodyInst);
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildForLoop: no condition, added BR to body" << std::endl;
            #endif
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 5.5: تسجيل سياق الحلقة لدعم break/continue
    //      continueLabel = كتلة الزيادة (for_inc) — continue يقفز للزيادة أولاً
    //      breakLabel = كتلة الخروج (for_exit) — break يقفز للخروج مباشرة
    //      ملاحظة: في for، continue يجب أن ينفّذ الزيادة ثم يعود للشرط
    // (EN) Step 5.5: Register loop context for break/continue support
    //      continueLabel = increment block (for_inc) — continue goes to increment first
    //      breakLabel = exit block (for_exit) — break jumps to exit directly
    //      Note: In for, continue must execute increment then go to condition
    // ========================================================================
    LoopContext forLoopCtx;
    forLoopCtx.continueLabel = incLabel;
    forLoopCtx.breakLabel = exitLabel;
    enterLoop(forLoopCtx);
    
    // ========================================================================
    // (AR) الخطوة 6: بناء جسم الحلقة
    // (EN) Step 6: Build loop body
    // المصدر: ForStmt::body (statements.h:196)
    // ========================================================================
    currentBlock_ = bodyBlock;
    if (forLoop->body) {
        buildStatement(forLoop->body.get());
    }
    
    // (AR) الخروج من سياق الحلقة بعد بناء الجسم
    // (EN) Exit loop context after building body
    exitLoop();
    
    // (AR) قفز إلى كتلة الزيادة
    // (EN) Jump to increment block
    SIROperand incLabelOp = SIROperand::Label(incLabel);
    SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brIncInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildForLoop: added BR to increment block" << std::endl;
        #endif
    }
    
    // ========================================================================
    // (AR) الخطوة 7: بناء الزيادة
    // (EN) Step 7: Build increment
    // المصدر: ForStmt::increment (statements.h:195)
    // ========================================================================
    currentBlock_ = incBlock;
    if (forLoop->increment) {
        buildExpression(forLoop->increment.get());
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildForLoop: built increment expression" << std::endl;
        #endif
    }
    
    // (AR) قفز للعودة إلى كتلة الشرط
    // (EN) Jump back to condition block
    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
    
    if (currentBlock_) {
        currentBlock_->instructions.push_back(brBackInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildForLoop: added BR back to condition" << std::endl;
        #endif
    }
    
    // ========================================================================
    // (AR) الخطوة 8: الاستمرار بعد الحلقة
    // (EN) Step 8: Continue after loop
    // ========================================================================
    currentBlock_ = exitBlock;
    
    // (AR) الخروج من نطاق الحلقة
    // (EN) Exit loop scope
    exitScope();
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForLoop: completed, now at exit block" << std::endl;
    #endif
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
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForRangeLoop: starting, variable='" 
              << forRange->variable << "'" << std::endl;
    #endif
    
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
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildForRangeLoop: failed to build iterable!" << std::endl;
        #endif
        errors_.push_back("Error: Failed to build iterable expression in for-range");
        exitScope();
        return;
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForRangeLoop: iterable reg='" 
              << iterableResult.registerName << "'" << std::endl;
    #endif
    
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
    // (AR) الخطوة 4: إنشاء متغير العداد عبر ALLOC+STORE (نمط غير-SSA آمن)
    // (EN) Step 4: Create index counter via ALLOC+STORE (safe non-SSA pattern)
    // (AR) نستخدم نمط التخصيص على المكدس مثل المتغيرات المحلية لتجنب
    //      مشكلة تعريف السجل الواحد في كتل متعددة (non-SSA counter).
    // (EN) We use stack allocation like local variables to avoid single-def
    //      register being "defined" in multiple blocks (non-SSA counter).
    // ========================================================================
    std::string idxSuffix = condLabel.substr(condLabel.find_last_of('_') + 1);
    std::string indexAllocName = "%_foreach_idx_" + idxSuffix;

    // ALLOC the counter slot
    {
        SIRInstruction allocIdx(SIROpcode::ALLOC);
        allocIdx.result = SIROperand::Register(indexAllocName, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(allocIdx);
    }

    // STORE 0 into the counter slot (initialize to 0)
    {
        SIRInstruction storeZero(SIROpcode::STORE);
        storeZero.operands.push_back(SIROperand::ConstantI64(0));
        storeZero.operands.push_back(SIROperand::Register(indexAllocName, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(storeZero);
    }
    
    // ========================================================================
    // (AR) الخطوة 5: إنشاء متغير الحلقة وتسجيله في النطاق
    // (EN) Step 5: Create loop variable and register in scope
    // المصدر: ForRangeStmt::variable (statements.h:230)
    // ========================================================================
    std::string loopVarAllocName = "%" + forRange->variable;

    // ALLOC the loop variable slot
    // (AR) نستخدم I64 لأن عناصر المصفوفة مخزنة كـ i64
    // (EN) Use I64 because array elements are stored as i64
    {
        SIRInstruction allocLoop(SIROpcode::ALLOC);
        allocLoop.result = SIROperand::Register(loopVarAllocName, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(allocLoop);
    }

    // (AR) تسجيل متغير الحلقة (sir_builder.h:144 - VariableInfo)
    // (EN) Register loop variable using addVariable (sir_builder.h:591)
    VariableInfo varInfo;
    varInfo.name = forRange->variable;
    varInfo.registerName = loopVarAllocName;
    varInfo.type = SIRType::I64;
    varInfo.isMutable = true;
    
    addVariable(varInfo);
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForRangeLoop: registered loop var '" 
              << forRange->variable << "' alloc=" << loopVarAllocName << std::endl;
    #endif
    
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

    SIROperand iterOp = SIROperand::Register(iterableResult.registerName, iterableResult.type);

    // LOAD current counter value
    std::string loadedIdxCond = newTempRegister();
    {
        SIRInstruction loadIdx(SIROpcode::LOAD);
        loadIdx.result = SIROperand::Register(loadedIdxCond, SIRType::I64);
        loadIdx.operands.push_back(SIROperand::Register(indexAllocName, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(loadIdx);
    }

    // ARRAY_LEN
    std::string lengthReg = newTempRegister();
    SIROperand lengthOp = SIROperand::Register(lengthReg, SIRType::I64);
    {
        SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
        lenInst.result = lengthOp;
        lenInst.operands.push_back(iterOp);
        if (currentBlock_) currentBlock_->instructions.push_back(lenInst);
    }
    
    // LT: loadedIdxCond < length
    std::string condReg = newTempRegister();
    SIROperand condResultOp = SIROperand::Register(condReg, SIRType::BOOL);
    {
        SIRInstruction cmpInst = SIRInstruction::Binary(
            SIROpcode::LT, condResultOp,
            SIROperand::Register(loadedIdxCond, SIRType::I64),
            lengthOp);
        if (currentBlock_) currentBlock_->instructions.push_back(cmpInst);
    }
    
    // BR_COND → body / exit
    SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
    SIROperand exitLabelOp = SIROperand::Label(exitLabel);
    {
        SIRInstruction brCondInst = SIRInstruction::BranchCond(
            condResultOp, bodyLabelOp, exitLabelOp);
        if (currentBlock_) currentBlock_->instructions.push_back(brCondInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 8: بناء جسم الحلقة
    // (EN) Step 8: Build loop body
    // ========================================================================
    currentBlock_ = bodyBlock;

    // LOAD current counter value for ARRAY_GET
    std::string loadedIdxBody = newTempRegister();
    {
        SIRInstruction loadIdxB(SIROpcode::LOAD);
        loadIdxB.result = SIROperand::Register(loadedIdxBody, SIRType::I64);
        loadIdxB.operands.push_back(SIROperand::Register(indexAllocName, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(loadIdxB);
    }

    // ARRAY_GET: loopVar = iterable[loadedIdx]
    // (AR) نوع النتيجة I64 لأن عناصر المصفوفة مخزنة كـ i64
    // (EN) Result type I64 because array elements are stored as i64
    std::string elemReg = newTempRegister();
    {
        SIRInstruction loadElem(SIROpcode::ARRAY_GET);
        loadElem.result = SIROperand::Register(elemReg, SIRType::I64);
        loadElem.operands.push_back(iterOp);
        loadElem.operands.push_back(SIROperand::Register(loadedIdxBody, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(loadElem);
    }

    // STORE element into loop variable slot
    {
        SIRInstruction storeElem(SIROpcode::STORE);
        storeElem.operands.push_back(SIROperand::Register(elemReg, SIRType::I64));
        storeElem.operands.push_back(SIROperand::Register(loopVarAllocName, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(storeElem);
    }
    
    // (AR) بناء جسم الحلقة
    // (EN) Build loop body
    if (forRange->body) {
        buildStatement(forRange->body.get());
    }
    
    // (AR) قفز إلى كتلة الزيادة
    // (EN) Jump to increment block
    SIROperand incLabelOp = SIROperand::Label(incLabel);
    {
        SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);
        if (currentBlock_) currentBlock_->instructions.push_back(brIncInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 9: بناء الزيادة (index = index + 1)
    // (EN) Step 9: Build increment (index = index + 1)
    // ========================================================================
    currentBlock_ = incBlock;

    // LOAD counter current value
    std::string loadedIdxInc = newTempRegister();
    {
        SIRInstruction loadIdxI(SIROpcode::LOAD);
        loadIdxI.result = SIROperand::Register(loadedIdxInc, SIRType::I64);
        loadIdxI.operands.push_back(SIROperand::Register(indexAllocName, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(loadIdxI);
    }

    // ADD: newIdx = loadedIdx + 1
    std::string newIdxReg = newTempRegister();
    {
        SIRInstruction incInst = SIRInstruction::Binary(
            SIROpcode::ADD_I64,
            SIROperand::Register(newIdxReg, SIRType::I64),
            SIROperand::Register(loadedIdxInc, SIRType::I64),
            SIROperand::ConstantI64(1));
        if (currentBlock_) currentBlock_->instructions.push_back(incInst);
    }

    // STORE newIdx back into counter slot
    {
        SIRInstruction storeIdx(SIROpcode::STORE);
        storeIdx.operands.push_back(SIROperand::Register(newIdxReg, SIRType::I64));
        storeIdx.operands.push_back(SIROperand::Register(indexAllocName, SIRType::I64));
        if (currentBlock_) currentBlock_->instructions.push_back(storeIdx);
    }
    
    // (AR) قفز للعودة إلى الشرط
    // (EN) Jump back to condition
    {
        SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
        if (currentBlock_) currentBlock_->instructions.push_back(brBackInst);
    }
    
    // ========================================================================
    // (AR) الخطوة 10: الاستمرار بعد الحلقة
    // (EN) Step 10: Continue after loop
    // ========================================================================
    currentBlock_ = exitBlock;
    
    // (AR) الخروج من نطاق الحلقة
    // (EN) Exit loop scope
    exitScope();
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildForRangeLoop: completed" << std::endl;
    #endif
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
