// ============================================================================
// sir_builder_expressions.cpp - بناء التعبيرات: الثوابت، المتغيرات، المنطقية، الأحادية
// ============================================================================
// (AR) هذا الملف يحتوي على بناء التعبيرات الأساسية:
//      buildLiteral, buildVariableAccess, buildShortCircuitLogical, buildUnaryOp
//      تم فصله من sir_builder_operators.cpp وفق CW-05 (حد 800 سطر)
// (EN) This file contains basic expression building:
//      buildLiteral, buildVariableAccess, buildShortCircuitLogical, buildUnaryOp
//      Split from sir_builder_operators.cpp per CW-05 (800 line limit)
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            BuildResult SIRBuilder::buildLiteral(AST::LiteralNode *literal)
            {
                if (!literal)
                {
                    return BuildResult();
                }

                // (AR) الحصول على Token (expressions.h:175 - token member)
                // (EN) Get token
                const auto &token = literal->token;
                Lexer::TokenType tokenType = token.getType();
                std::string value = token.getValue();

                // (AR) إنشاء سجل مؤقت للنتيجة
                // (EN) Create temporary register for result
                std::string resultReg = newTempRegister();

                // (AR) تحديد النوع بناءً على نوع Token
                // (EN) Determine type based on token type
                SadTypeKind sirType = SadTypeKind::Integer; // default

                // (AR) معالجة الأنواع المختلفة
                // (EN) Handle different types
                // أسماء TokenType من token.h:198-202
                // TokenType names from token.h:198-202
                BuildResult result;
                result.registerName = resultReg;
                result.isConstant = true;
                result.constantValue = value;

                if (tokenType == Lexer::TokenType::NUMBER_INTEGER)
                {
                    result.type = SadTypeKind::Integer;
                    // (AR) تحويل الأعداد الست عشرية/الثمانية/الثنائية إلى عشرية
                    //      لأن std::stoll() الافتراضي يستخدم أساس 10 فقط
                    // (EN) Normalize hex/octal/binary literals to decimal strings
                    //      because downstream std::stoll() uses base 10 by default
                    if (value.size() > 2 && value[0] == '0')
                    {
                        char prefix = value[1];
                        if (prefix == 'x' || prefix == 'X')
                        {
                            // Hex: 0x3F8 → "1016", 0xFFFF800000000000 → "-140737488355328"
                            result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
                        }
                        else if (prefix == 'o' || prefix == 'O')
                        {
                            // Octal: 0o755 → "493"
                            result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
                        }
                        else if (prefix == 'b' || prefix == 'B')
                        {
                            // Binary: 0b1010 → "10"
                            result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
                        }
                    }
                }
                else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE)
                {
                    result.type = SadTypeKind::Float;
                }
                else if (tokenType == Lexer::TokenType::STRING_LITERAL)
                {
                    result.type = SadTypeKind::String;
                }
                else if (tokenType == Lexer::TokenType::LITERAL_TRUE || tokenType == Lexer::TokenType::LITERAL_FALSE)
                {
                    result.type = SadTypeKind::Boolean;
                    result.constantValue = (tokenType == Lexer::TokenType::LITERAL_TRUE) ? "true" : "false";
                }
                else if (tokenType == Lexer::TokenType::LITERAL_NULL)
                {
                    // (AR) لاشيء / null — قيمة حارسة (sentinel) مختلفة عن الصفر العددي.
                    //      هذا يحقق دلالة اللغة: (لاشيء == لاشيء) صحيح و (لاشيء != 0) صحيح.
                    //      تم اختيار قيمة سالبة كبيرة لتفادي التعارض مع الأعداد الشائعة.
                    // (EN) null literal — use a sentinel distinct from numeric zero.
                    //      This preserves language semantics: (null == null) true and (null != 0) true.
                    //      A large negative sentinel is used to avoid clashes with common integers.
                    result.type = SadTypeKind::Integer;
                    result.constantValue = Sad::Compiler::kSadNullSentinelStr;
                    result.isConstant = true;
                }
                else
                {
                    result.type = SadTypeKind::Integer;
                }

                // ========================================================================
                // (AR) إصدار تعليمة MOVE لتعريف السجل فعلياً في SIR
                //      هذا يمنع أخطاء "Undefined register" عندما يستخدم كود المستهلك
                //      registerName مباشرة بدون فحص isConstant
                // (EN) Emit MOVE instruction to actually define the register in SIR.
                //      This prevents "Undefined register" errors when consumer code uses
                //      registerName directly without checking isConstant.
                // ========================================================================
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(resultReg, result.type);
                    switch (result.type)
                    {
                    case SadTypeKind::Integer:
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(result.constantValue)));
                        }
                        catch (const std::exception &)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                        break;
                    case SadTypeKind::Float:
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(result.constantValue)));
                        }
                        catch (const std::exception &)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantF64(0.0));
                        }
                        break;
                    case SadTypeKind::Boolean:
                        moveInst.operands.push_back(SIROperand::ConstantBool(result.constantValue == "true" || result.constantValue == "1"));
                        break;
                    case SadTypeKind::String:
                        moveInst.operands.push_back(SIROperand::ConstantString(result.constantValue));
                        break;
                    case SadTypeKind::Pointer:
                        moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        break;
                    default:
                        moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        break;
                    }
                    if (currentBlock_)
                        currentBlock_->addInstruction(moveInst);
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
            BuildResult SIRBuilder::buildVariableAccess(AST::VariableNode *var)
            {
                if (!var)
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildVariableAccess: var is null!" << std::endl;
#endif
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildVariableAccess: looking up variable '" << var->name << "'" << std::endl;
#endif

                // ================================================================
                // (AR) [Phase 4 — Monomorphization] فحص const-generic substitutions:
                //      أثناء بناء جسم قالب مُخصَّص، أي مرجع لمعامل ثابت (مثل N)
                //      يُستبدل فوراً بقيمة Constant بدون تخصيص ذاكرة أو load.
                //      هذا هو جوهر monomorphization — الثابت يصبح literal مضمَّن
                //      يسمح للـoptimizer بطيّ ثوابت كاملة (constant folding).
                // (EN) [Phase 4 — Monomorphization] Const-generic substitution check:
                //      While building a specialized template body, any reference
                //      to a const-generic param (e.g. N) is short-circuited to
                //      an immediate Constant — no alloca, no load. This is the
                //      core of monomorphization: the const becomes an inlined
                //      literal enabling full constant folding by the optimizer.
                // ================================================================
                if (!genericScopeStack_.empty())
                {
                    const auto &topScope = genericScopeStack_.back();
                    auto cIt = topScope.constSubstitutions.find(var->name);
                    if (cIt != topScope.constSubstitutions.end())
                    {
                        const SIROperand &constOp = cIt->second;
                        // (AR) ننشئ سجل مؤقت يحمل القيمة الثابتة (لكي يبقى نموذج
                        //      SIR متسقاً — كل تعبير يُرجع registerName).
                        // (EN) Materialize the constant into a fresh register so
                        //      downstream SIR consumers (which expect a register)
                        //      keep working uniformly.
                        std::string reg = newTempRegister();
                        SIRInstruction movInst(SIROpcode::MOVE);
                        movInst.result = SIROperand::Register(reg, constOp.dataType);
                        movInst.operands.push_back(constOp);
                        movInst.comment = "const-generic-subst:" + var->name;
                        if (currentBlock_)
                            currentBlock_->addInstruction(movInst);
                        BuildResult result(reg, constOp.dataType);
                        // (AR) لا نضع isConstant=true لتجنب اعتماد المستهلكات
                        //      على constantValue؛ MOVE من ثابت يتم طيّه لاحقاً
                        //      بواسطة constant folding في LLVM.
                        // (EN) Don't set isConstant=true to avoid downstream
                        //      consumers expecting constantValue; LLVM's constant
                        //      folding will fold the MOVE-from-constant later.
                        return result;
                    }
                }

                // (AR) البحث عن المتغير في النطاقات (expressions.h:208 - name member)
                // (EN) Lookup variable in scopes
                VariableInfo *varInfo = lookupVariable(var->name);

                if (!varInfo)
                {
                    // ================================================================
                    // (AR) [Fix #55 v2] مرجع دالة — تمرير اسم دالة كوسيط:
                    //      في لغة ص الديناميكية: طبّق(مضاعف، 5)
                    //      نُنشئ بنية إغلاق (closure struct) تحمل مؤشر الدالة
                    //      ونُصدر CLOSURE_CREATE لتمكين CLOSURE_CALL لاحقاً.
                    //      ملاحظة: نُنشئ wrapper لأن CLOSURE_CALL يُضيف __env
                    //      كمعامل أخير، والدالة الأصلية لا تتوقعه.
                    // (EN) [Fix #55 v2] Function reference — passing function name as arg:
                    //      We emit CLOSURE_CREATE to build a closure struct that holds
                    //      the function pointer, enabling CLOSURE_CALL later.
                    //      We mark the function ref for wrapper generation in codegen.
                    // ================================================================
                    auto funcIt = functionTable_.find(var->name);
                    if (funcIt != functionTable_.end())
                    {
                        // (AR) إصدار CLOSURE_CREATE لبناء بنية إغلاق حقيقية
                        // (EN) Emit CLOSURE_CREATE to build real closure struct
                        std::string closureReg = newTempRegister();
                        SIRInstruction closureInst;
                        closureInst.opcode = SIROpcode::CLOSURE_CREATE;
                        closureInst.result = SIROperand::Register(closureReg, SadTypeKind::Function);
                        closureInst.operands.push_back(SIROperand::Register(var->name, SadTypeKind::Function));
                        closureInst.comment = "func-ref:" + var->name;
                        if (currentBlock_)
                            currentBlock_->addInstruction(closureInst);

                        // (AR) تسجيل أن هذه الدالة تُستخدم كمرجع (للحاجة لإنشاء wrapper)
                        funcRefNames_.insert(var->name);

                        BuildResult result;
                        result.type = SadTypeKind::Function;
                        result.registerName = closureReg;
                        result.isConstant = false;
                        return result;
                    }
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildVariableAccess: variable NOT FOUND!" << std::endl;
#endif
                    errors_.push_back("Error: Undefined variable '" + var->name + "'");
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildVariableAccess: found variable, registerName='"
                          << varInfo->registerName << "', type=" << static_cast<int>(varInfo->type) << std::endl;
#endif

                // (AR) إرجاع معلومات المتغير (sir_builder.h:139 - VariableInfo struct)
                // (EN) Return variable information
                // VariableInfo members: registerName (line 143), type (line 142)

                // ================================================================
                // (AR) معاملة خاصة للمتغيرات العامة الثابتة (مثل حالات Unit في ADT):
                //      شكل.نقطة مسجلة كمتغير عام ثابت بقيمة = المميّز (2 مثلاً).
                //      عند قراءتها في تعبير (مثل: متغير ش = شكل.نقطة)
                //      نحتاج LOAD من المتغير العام لتحميل القيمة الفعلية.
                //      بدون هذا: يُرجع "%شكل.نقطة" كسجل محلي → غير معرّف في LLVM
                // (EN) Special handling for global constant variables (like ADT Unit variants):
                //      Shape.Point is a global constant with value = tag (e.g. 2).
                //      When read in expression (e.g.: var s = Shape.Point)
                //      we need LOAD from global to get the actual value.
                //      Without this: returns "%Shape.Point" as local register → undefined in LLVM
                // ================================================================
                if (varInfo->isGlobal && !varInfo->isMutable && currentBlock_)
                {
                    std::string loadReg = newTempRegister();
                    SIRInstruction loadInst(SIROpcode::LOAD);
                    loadInst.result = SIROperand::Register(loadReg, varInfo->type);
                    loadInst.operands.push_back(SIROperand::Global(var->name, varInfo->type));
                    loadInst.comment = "Load global constant: " + var->name;
                    currentBlock_->addInstruction(loadInst);

                    BuildResult result(loadReg, varInfo->type);
                    result.isParameter = varInfo->isParameter;
                    if (varInfo->elementType != SadTypeKind::Void)
                    {
                        result.elementType = varInfo->elementType;
                    }
                    if (!varInfo->elementClassName.empty())
                    {
                        result.elementClassName = varInfo->elementClassName;
                    }
                    auto cit = classInstanceTypes_.find(var->name);
                    if (cit != classInstanceTypes_.end())
                    {
                        result.className = cit->second;
                    }
                    return result;
                }
                BuildResult result(varInfo->registerName, varInfo->type);
                result.isParameter = varInfo->isParameter;

                // (AR) تتبع اسم الصنف إذا كان المتغير يحمل كائناً
                // (EN) Track class name if variable holds an object
                auto cit = classInstanceTypes_.find(var->name);
                if (cit != classInstanceTypes_.end())
                {
                    result.className = cit->second;
                }
                // (AR) محاولة البحث باسم السجل أيضاً
                // (EN) Also try register name lookup
                if (result.className.empty())
                {
                    cit = classInstanceTypes_.find(varInfo->registerName);
                    if (cit != classInstanceTypes_.end())
                    {
                        result.className = cit->second;
                    }
                }

                // (AR) نقل نوع عنصر المصفوفة إلى BuildResult لدعم حلقات foreach
                // (EN) Propagate array element type to BuildResult for foreach support
                if (varInfo->elementType != SadTypeKind::Void)
                {
                    result.elementType = varInfo->elementType;
                }
                if (!varInfo->elementClassName.empty())
                {
                    result.elementClassName = varInfo->elementClassName;
                }

                return result;
            }

            // ============================================================================
            // buildShortCircuitLogical - بناء تقييم كسول للعوامل المنطقية (و/أو)
            // ============================================================================
            // (AR) إصلاح: العمليات المنطقية (&&/||/و/أو) كانت تُقيّم الطرفين دائماً (eager)
            //      مما يسبب تنفيذ آثار جانبية غير مرغوبة عند استخدام دوال في الشروط.
            //      الحل: بناء كتل SIR منفصلة (basic blocks) مع تفرع مشروط:
            //      - تقييم الأيسر في الكتلة الحالية
            //      - تفرع مشروط: إذا كان الأيسر كافياً → القفز للنتيجة
            //      - تقييم الأيمن في كتلة منفصلة (فقط إذا لزم)
            //      - دمج النتائج في كتلة merge عبر PHI
            // (EN) Fix: Logical operators (&&/||) were eagerly evaluating both operands,
            //      causing unwanted side effects. Now uses SIR basic blocks with
            //      conditional branching for short-circuit evaluation.
            //
            // البنية المولّدة / Generated structure:
            //
            // AND (و/&&):
            //   current_block:
            //     %left = buildExpression(left)
            //     br_cond %left, eval_right, merge
            //   eval_right:
            //     %right = buildExpression(right)
            //     br merge
            //   merge:
            //     %result = ALLOC bool
            //     STORE (came from eval_right ? %right : false), %result
            //
            // OR (أو/||):
            //   current_block:
            //     %left = buildExpression(left)
            //     br_cond %left, merge, eval_right
            //   eval_right:
            //     %right = buildExpression(right)
            //     br merge
            //   merge:
            //     %result = ALLOC bool
            //     STORE (came from current ? true : %right), %result
            // ============================================================================
            BuildResult SIRBuilder::buildShortCircuitLogical(AST::BinaryOpNode *binOp)
            {
                if (!binOp)
                {
                    return BuildResult();
                }

                bool isAnd = (binOp->op == Lexer::TokenType::OP_AND);

#ifndef NDEBUG
                std::cout << "[DEBUG] buildShortCircuitLogical: بدء بناء "
                          << (isAnd ? "AND (و/&&)" : "OR (أو/||)")
                          << " بتقييم كسول" << std::endl;
#endif

                // ================================================================
                // (AR) الخطوة 1: تقييم الطرف الأيسر في الكتلة الحالية
                // (EN) Step 1: Evaluate left operand in current block
                // ================================================================
                auto leftResult = buildExpression(binOp->left.get());

                if (leftResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildShortCircuitLogical: فشل بناء الطرف الأيسر!" << std::endl;
#endif
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildShortCircuitLogical: leftReg='"
                          << leftResult.registerName << "'" << std::endl;
#endif

                // ================================================================
                // (AR) الخطوة 2: إنشاء الكتل الأساسية
                //      - evalRightBlock: كتلة تقييم الطرف الأيمن
                //      - mergeBlock: كتلة دمج النتيجة
                // (EN) Step 2: Create basic blocks
                //      - evalRightBlock: block for evaluating right operand
                //      - mergeBlock: block for merging the result
                // ================================================================
                std::string evalRightLabel = newLabel("sc_eval_right");
                std::string mergeLabel = newLabel("sc_merge");

                auto evalRightBlock = createBasicBlock(evalRightLabel);
                auto mergeBlock = createBasicBlock(mergeLabel);

                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(evalRightBlock);
                    currentFunction_->addBasicBlock(mergeBlock);
                }

                // ================================================================
                // (AR) الخطوة 3: إنشاء متغير النتيجة (ALLOC + تخزين مبدئي)
                //      AND: النتيجة المبدئية = false (إذا كان الأيسر false لن نقيّم الأيمن)
                //      OR: النتيجة المبدئية = true (إذا كان الأيسر true لن نقيّم الأيمن)
                // (EN) Step 3: Create result variable (ALLOC + initial store)
                //      AND: default result = false (if left is false, skip right)
                //      OR: default result = true (if left is true, skip right)
                // ================================================================
                std::string resultReg = newTempRegister();
                {
                    SIRInstruction allocResult(SIROpcode::ALLOC);
                    allocResult.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(allocResult);
                }
                {
                    // (AR) تخزين القيمة المبدئية: false لـ AND، true لـ OR
                    // (EN) Store default: false for AND, true for OR
                    SIRInstruction storeDefault(SIROpcode::STORE);
                    storeDefault.operands.push_back(SIROperand::ConstantBool(!isAnd));
                    storeDefault.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Boolean));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(storeDefault);
                }

                // ================================================================
                // (AR) الخطوة 4: تفرع مشروط بناءً على الطرف الأيسر
                //      AND: إذا true → تقييم الأيمن، إذا false → merge (النتيجة false)
                //      OR: إذا true → merge (النتيجة true)، إذا false → تقييم الأيمن
                // (EN) Step 4: Conditional branch based on left operand
                //      AND: if true → eval right, if false → merge (result stays false)
                //      OR: if true → merge (result stays true), if false → eval right
                // ================================================================
                if (currentBlock_)
                {
                    SIROperand condOp = SIROperand::Register(leftResult.registerName, leftResult.type);
                    if (isAnd)
                    {
                        // (AR) AND: إذا الأيسر صحيح → نقيّم الأيمن، وإلا → merge
                        currentBlock_->instructions.push_back(
                            SIRInstruction::BranchCond(condOp,
                                                       SIROperand::Label(evalRightLabel),
                                                       SIROperand::Label(mergeLabel)));
                    }
                    else
                    {
                        // (AR) OR: إذا الأيسر صحيح → merge، وإلا → نقيّم الأيمن
                        currentBlock_->instructions.push_back(
                            SIRInstruction::BranchCond(condOp,
                                                       SIROperand::Label(mergeLabel),
                                                       SIROperand::Label(evalRightLabel)));
                    }
                }

                // ================================================================
                // (AR) الخطوة 5: كتلة تقييم الطرف الأيمن
                //      يتم تقييم الطرف الأيمن فقط إذا وصلنا لهذه الكتلة
                //      ثم تُخزّن النتيجة ونقفز إلى merge
                // (EN) Step 5: Evaluate right operand block
                //      Right is evaluated only if we reach this block
                //      Result is stored and we jump to merge
                // ================================================================
                currentBlock_ = evalRightBlock;
                auto rightResult = buildExpression(binOp->right.get());

                if (!rightResult.registerName.empty())
                {
                    // (AR) تخزين نتيجة الطرف الأيمن في متغير النتيجة
                    // (EN) Store right operand result into result variable
                    SIRInstruction storeRight(SIROpcode::STORE);
                    storeRight.operands.push_back(
                        SIROperand::Register(rightResult.registerName, rightResult.type));
                    storeRight.operands.push_back(
                        SIROperand::Register(resultReg, SadTypeKind::Boolean));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(storeRight);
                }

                // (AR) قفز غير مشروط إلى كتلة الدمج
                // (EN) Unconditional jump to merge block
                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(
                        SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // ================================================================
                // (AR) الخطوة 6: كتلة الدمج — تحميل النتيجة النهائية
                // (EN) Step 6: Merge block — load final result
                // ================================================================
                currentBlock_ = mergeBlock;

                std::string finalReg = newTempRegister();
                {
                    SIRInstruction loadResult(SIROpcode::LOAD);
                    loadResult.result = SIROperand::Register(finalReg, SadTypeKind::Boolean);
                    loadResult.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Boolean));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(loadResult);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildShortCircuitLogical: النتيجة في سجل '"
                          << finalReg << "'" << std::endl;
#endif

                BuildResult result(finalReg, SadTypeKind::Boolean);
                result.isDirectValue = true;
                return result;
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

            BuildResult SIRBuilder::buildUnaryOp(AST::UnaryOpNode *unaryOp)
            {
                if (!unaryOp)
                {
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildUnaryOp: بدء بناء عملية أحادية" << std::endl;
#endif

                // (AR) بناء المعامل (expressions.h:81 - operand: ExprPtr)
                // (EN) Build operand
                auto operandResult = buildExpression(unaryOp->operand.get());
#ifndef NDEBUG
                std::cout << "[DEBUG] buildUnaryOp: operandResult.registerName='" << operandResult.registerName
                          << "', type=" << static_cast<int>(operandResult.type) << std::endl;
#endif

                // ================================================================
                // (AR) تحميل العوامل الأحادية الزائد: إذا كان المعامل كائن، نبحث عن عامل u-/!/u+ في الصنف
                //      هذا يتوافق مع سلوك المفسر في expression_evaluator_calls.cpp:visitUnaryExpr
                //      الأسماء المدعومة: u- → __op_neg__, ! → __op_not__, u+ → __op_pos__
                // (EN) Unary operator overloading: if operand is an object, search for u-/!/u+ in class
                //      This matches interpreter behavior in expression_evaluator_calls.cpp:visitUnaryExpr
                //      Supported names: u- → __op_neg__, ! → __op_not__, u+ → __op_pos__
                // ================================================================
                std::string operandClassName = operandResult.className;
                if (operandClassName.empty() && !operandResult.registerName.empty())
                {
                    auto it = classInstanceTypes_.find(operandResult.registerName);
                    if (it != classInstanceTypes_.end())
                    {
                        operandClassName = it->second;
                    }
                }

                if (!operandClassName.empty())
                {
                    // (AR) تحديد اسم دالة العامل الأحادي
                    // (EN) Determine unary operator function name
                    std::string opSafeName;
                    switch (unaryOp->op)
                    {
                    case Lexer::TokenType::OP_MINUS:
                        opSafeName = "__op_neg__";
                        break;
                    case Lexer::TokenType::OP_NOT:
                        opSafeName = "__op_not__";
                        break;
                    case Lexer::TokenType::OP_PLUS:
                        opSafeName = "__op_pos__";
                        break;
                    default:
                        break;
                    }

                    if (!opSafeName.empty())
                    {
                        // (AR) البحث في سلسلة الوراثة (نفس نمط buildBinaryOp)
                        // (EN) Search inheritance chain (same pattern as buildBinaryOp)
                        std::string searchClass = operandClassName;
                        std::string fullOpName;
                        bool found = false;
                        while (!searchClass.empty())
                        {
                            fullOpName = searchClass + "." + opSafeName;
                            auto funcIt = functionTable_.find(fullOpName);
                            if (funcIt != functionTable_.end())
                            {
                                found = true;
                                break;
                            }
                            auto parentClass = module_->getClass(searchClass);
                            if (parentClass && !parentClass->parentClass.empty())
                            {
                                searchClass = parentClass->parentClass;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (found)
                        {
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildUnaryOp: dispatching to operator overload '"
                                      << fullOpName << "'" << std::endl;
#endif

                            std::string resultReg = newTempRegister();
                            auto &opInfo = functionTable_[fullOpName];
                            SadTypeKind returnType = opInfo.returnType;

                            if (currentBlock_)
                            {
                                // (AR) استدعاء عامل أحادي: OBJECT_CALL(self, اسم_العامل) — بدون معامل إضافي
                                // (EN) Unary operator call: OBJECT_CALL(self, op_name) — no extra operand
                                SIRInstruction callInst;
                                callInst.opcode = SIROpcode::OBJECT_CALL;
                                callInst.result = SIROperand::Register(resultReg, returnType);
                                callInst.operands.push_back(SIROperand::Register(operandResult.registerName, operandResult.type));
                                callInst.operands.push_back(SIROperand::ConstantString(opSafeName));
                                currentBlock_->addInstruction(callInst);
                            }

                            BuildResult result(resultReg, returnType);
                            result.className = operandClassName;
                            return result;
                        }
                    }
                }

                // (AR) إنشاء سجل للنتيجة (sir_builder.h:511 - newTempRegister)
                // (EN) Create result register
                std::string resultReg = newTempRegister();

                // (AR) تحديد نوع النتيجة
                // (EN) Determine result type
                SadTypeKind resultType = operandResult.type;

                // (AR) تحديد رمز العملية (SIROpcode) بناءً على TokenType
                // (EN) Determine SIROpcode based on TokenType
                SIROpcode opcode;

                // (AR) العملية من expressions.h:80 - op: Lexer::TokenType
                switch (unaryOp->op)
                {
                case Lexer::TokenType::OP_MINUS:
                    // (AR) السالب: NEG (sir_types.h:115)
                    opcode = SIROpcode::NEG;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildUnaryOp: عملية السالب (-)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_NOT:
                    // (AR) النفي المنطقي: NOT (sir_types.h:121)
                    opcode = SIROpcode::NOT;
                    resultType = SadTypeKind::Boolean; // (AR) النفي يُرجع قيمة منطقية
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildUnaryOp: عملية النفي (!)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_BITWISE_NOT:
                    // (AR) النفي البتّي: NOT (sir_types.h:134)
                    opcode = SIROpcode::NOT;
// (AR) النتيجة بنفس نوع المعامل (عدد صحيح)
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildUnaryOp: عملية النفي البتّي (~)" << std::endl;
#endif
                    break;

                default:
// (AR) عملية غير مدعومة
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildUnaryOp: عملية غير مدعومة: "
                              << static_cast<int>(unaryOp->op) << std::endl;
#endif
                    errors_.push_back("عملية أحادية غير مدعومة / Unsupported unary operation");
                    return BuildResult(resultReg, resultType);
                }

                // (AR) إنشاء معاملات SIR
                // (EN) Create SIR operands
                SIROperand operandOp, resultOp;

                // (AR) المعامل
                if (operandResult.isConstant)
                {
                    if (operandResult.type == SadTypeKind::Float)
                    {
                        operandOp = SIROperand::ConstantF64(std::stod(operandResult.constantValue));
                    }
                    else if (operandResult.type == SadTypeKind::Boolean)
                    {
                        operandOp = SIROperand::ConstantBool(operandResult.constantValue == "true" || operandResult.constantValue == "صحيح");
                    }
                    else
                    {
                        operandOp = SIROperand::ConstantI64(std::stoll(operandResult.constantValue));
                    }
                }
                else
                {
                    operandOp = SIROperand::Register(operandResult.registerName, operandResult.type);
                }

                // (AR) سجل النتيجة
                resultOp = SIROperand::Register(resultReg, resultType);

                // (AR) إنشاء تعليمة SIR (sir_instruction.h:114-120 - SIRInstruction::Unary)
                // (EN) Create SIR instruction
                SIRInstruction inst = SIRInstruction::Unary(opcode, resultOp, operandOp);

                // (AR) إضافة التعليمة للكتلة الحالية
                // (EN) Add instruction to current block
                if (currentBlock_)
                {
                    currentBlock_->addInstruction(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildUnaryOp: تمت إضافة التعليمة للكتلة الحالية" << std::endl;
#endif
                }
                else
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildUnaryOp: تحذير - لا توجد كتلة حالية!" << std::endl;
#endif
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildUnaryOp: النتيجة في سجل " << resultReg << std::endl;
#endif

                return BuildResult(resultReg, resultType);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad