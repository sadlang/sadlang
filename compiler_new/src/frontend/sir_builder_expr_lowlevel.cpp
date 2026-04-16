// ============================================================================
// sir_builder_expr_lowlevel.cpp
// ============================================================================
// Low-level expression builders (inline asm, sizeof, atomic)
// ============================================================================
#include "sir_builder.h"
#include "directive_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"

#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprInlineAsm
            // ============================================================================
            BuildResult SIRBuilder::buildExprInlineAsm(AST::InlineAsmExpr *inlineAsm)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found InlineAsmExpr" << std::endl;
#endif

                // (AR) إصدار تعليمة INLINE_ASM مباشرة — يتحول إلى llvm::InlineAsm في codegen
                // (EN) Emit INLINE_ASM instruction directly — becomes llvm::InlineAsm in codegen
                std::string resultReg = newTempRegister();

                SIRInstruction asmInst(SIROpcode::INLINE_ASM);
                // operands[0] = نص التجميع / assembly text
                asmInst.operands.push_back(SIROperand::ConstantString(inlineAsm->asmCode));
                // operands[1] = القيود / constraints (output + input combined)
                std::string constraints;
                if (!inlineAsm->outputConstraints.empty())
                {
                    constraints = inlineAsm->outputConstraints;
                    if (!inlineAsm->inputConstraints.empty())
                        constraints += "," + inlineAsm->inputConstraints;
                }
                else if (!inlineAsm->inputConstraints.empty())
                {
                    constraints = inlineAsm->inputConstraints;
                }
                if (!inlineAsm->clobbers.empty())
                {
                    if (!constraints.empty())
                        constraints += ",";
                    constraints += inlineAsm->clobbers;
                }
                asmInst.operands.push_back(SIROperand::ConstantString(constraints));

                // ====================================================================
                // (AR) ربط معاملات الإدخال تلقائياً — تعليمات @تجميع مع قيود "r" تحتاج قيم فعلية
                // (EN) Auto-bind input operands — @تجميع with "r" constraints need actual values
                //
                // الاصطلاح: المتغيرات قبل @تجميع مباشرة (بحسب ترتيب ALLOC في SIR)
                //   - آخر ALLOC = المخرج (إن وُجد output constraint)
                //   - الـ N ALLOCs قبله = المدخلات (بترتيب التصريح)
                // Convention: variables declared just before @تجميع (by ALLOC order in SIR)
                //   - Last ALLOC = output (if output constraint exists)
                //   - N ALLOCs before that = inputs (in declaration order)
                // ====================================================================
                if (!inlineAsm->inputConstraints.empty())
                {
                    // (AR) حساب عدد قيود الإدخال (مثال: "r" = 1، "r,r" = 2)
                    // (EN) Count input constraints (e.g., "r" = 1, "r,r" = 2)
                    int inputCount = 1;
                    for (char c : inlineAsm->inputConstraints)
                    {
                        if (c == ',')
                            inputCount++;
                    }

                    // (AR) بناء قائمة المتغيرات المدخلة من ALLOC بترتيب التصريح العكسي
                    // (EN) Build input variables from ALLOC instructions in reverse declaration order
                    //
                    // مبدأ العمل:
                    //   1. نعدّ ALLOC-ات من آخر الكتلة نحو أولها
                    //   2. نتخطى الأول (المخرج) إن كان هناك output constraint
                    //   3. نأخذ الـ N التالية كمدخلات
                    //   4. نعكس الترتيب ليكون بترتيب التصريح
                    // Algorithm:
                    //   1. Walk ALLOCs backwards from end of block
                    //   2. Skip first one (output) if output constraint exists
                    //   3. Take next N as inputs
                    //   4. Reverse to get declaration order
                    std::vector<std::string> inputRegs;
                    std::vector<SadTypeKind> inputRegTypes;

                    if (currentBlock_)
                    {
                        auto &instructions = currentBlock_->instructions;
                        int skip = inlineAsm->outputConstraints.empty() ? 0 : 1;
                        int found = 0;

                        for (auto it = instructions.rbegin(); it != instructions.rend() && found < inputCount; ++it)
                        {
                            if (it->opcode == SIROpcode::INLINE_ASM)
                                continue;
                            if (it->opcode == SIROpcode::ALLOC && it->result.has_value())
                            {
                                if (skip > 0)
                                {
                                    skip--;
                                    continue;
                                }
                                inputRegs.push_back(it->result->name);
                                inputRegTypes.push_back(it->result->dataType);
                                found++;
                            }
                        }

                        // (AR) عكس الترتيب ليكون بحسب تسلسل التصريح
                        // (EN) Reverse to match declaration order
                        std::reverse(inputRegs.begin(), inputRegs.end());
                        std::reverse(inputRegTypes.begin(), inputRegTypes.end());
                    }

                    // ====================================================================
                    // (AR) احتياطي: إذا لم يكف متغيرات ALLOC المحلية، نُكمّل من معاملات الدالة
                    //      معاملات الدالة لا تُنشئ تعليمات ALLOC في SIR — يُنشئها LLVM codegen مباشرة
                    //      لذلك نحتاج البحث في قائمة معاملات الدالة الحالية
                    // (EN) Fallback: if local ALLOCs are insufficient, supplement with function parameters
                    //      Function parameters do NOT create ALLOC instructions in SIR — they are created
                    //      directly by the LLVM codegen (emitFunctionParameters). So we need to check
                    //      the current function's parameter list.
                    //
                    //  ترتيب الربط: المعاملات أولاً (بترتيب التصريح)، ثم المحليات
                    //  Binding order: parameters first (declaration order), then locals
                    // ====================================================================
                    if ((int)inputRegs.size() < inputCount && currentFunction_)
                    {
                        const auto &params = currentFunction_->getParameters();
                        // (AR) نبني قائمة من معاملات الدالة ونضعها قبل المحليات
                        // (EN) Build list from function parameters and prepend before locals
                        std::vector<std::string> paramRegs;
                        std::vector<SadTypeKind> paramTypes;
                        for (size_t pi = 0; pi < params.size() &&
                                            (int)(paramRegs.size() + inputRegs.size()) < inputCount;
                             pi++)
                        {
                            paramRegs.push_back("%" + params[pi].name);
                            paramTypes.push_back(params[pi].type);
                        }
                        // (AR) المعاملات أولاً ثم المحليات
                        // (EN) Parameters first, then locals
                        paramRegs.insert(paramRegs.end(), inputRegs.begin(), inputRegs.end());
                        paramTypes.insert(paramTypes.end(), inputRegTypes.begin(), inputRegTypes.end());
                        inputRegs = std::move(paramRegs);
                        inputRegTypes = std::move(paramTypes);
                    }

                    // (AR) إضافة المعاملات كمدخلات operands[2..N]
                    // (EN) Add variables as input operands[2..N]
                    for (int i = 0; i < (int)inputRegs.size(); i++)
                    {
                        asmInst.operands.push_back(
                            SIROperand::Register(inputRegs[i], inputRegTypes[i]));
                    }
                }

                // (AR) إذا كانت هناك قيود مخرجات، نعتبر أن هناك نتيجة
                // (EN) If there are output constraints, set result register
                if (!inlineAsm->outputConstraints.empty())
                {
                    asmInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                }

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(asmInst);
                }

                // ====================================================================
                // (AR) تخزين نتيجة المخرجات تلقائياً في آخر متغير محلي مُخصص
                // (EN) Auto-store output result into the last allocated local variable
                //
                // النمط: متغير قيمة = 0 ← @تجميع("mov %%crX, %0", "=r", "", "") ← ارجع قيمة
                // Pattern: var val = 0 ← @asm("mov %%crX, %0", "=r", "", "") ← return val
                // ====================================================================
                if (!inlineAsm->outputConstraints.empty() && currentBlock_)
                {
                    std::string lastAllocReg;
                    auto &instructions = currentBlock_->instructions;
                    for (auto it = instructions.rbegin(); it != instructions.rend(); ++it)
                    {
                        // (AR) تخطي تعليمة INLINE_ASM التي أضفناها للتو
                        // (EN) Skip the INLINE_ASM we just added
                        if (it->opcode == SIROpcode::INLINE_ASM)
                            continue;
                        if (it->opcode == SIROpcode::ALLOC && it->result.has_value())
                        {
                            lastAllocReg = it->result->name;
                            break;
                        }
                    }

                    if (!lastAllocReg.empty())
                    {
                        SIRInstruction storeInst;
                        storeInst.opcode = SIROpcode::STORE;
                        storeInst.operands.push_back(
                            SIROperand::Register(resultReg, SadTypeKind::Integer));
                        storeInst.operands.push_back(
                            SIROperand::Register(lastAllocReg, SadTypeKind::Integer));
                        currentBlock_->addInstruction(storeInst);
                    }
                }

                return BuildResult(resultReg, SadTypeKind::Integer);
            }

            // ============================================================================
            // buildExprSizeof
            // ============================================================================
            BuildResult SIRBuilder::buildExprSizeof(AST::SizeofExpr *sizeofExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found SizeofExpr for type: "
                          << sizeofExpr->typeName << std::endl;
#endif

                // (AR) تحديد الحجم بناءً على اسم النوع
                // (EN) Determine size based on type name
                std::string typeName = sizeofExpr->typeName;
                int64_t typeSize = 8; // (AR) القيمة الافتراضية / (EN) Default value

                if (typeName == "رقم" || typeName == "عدد" || typeName == "صحيح" ||
                    typeName == "i64" || typeName == "int" || typeName == "integer")
                {
                    typeSize = 8;
                }
                else if (typeName == "عشري" || typeName == "مضاعف" || typeName == "حقيقي" ||
                         typeName == "f64" || typeName == "float" || typeName == "double")
                {
                    typeSize = 8;
                }
                else if (typeName == "منطقي" || typeName == "bool" || typeName == "boolean")
                {
                    typeSize = 1;
                }
                else if (typeName == "نص" || typeName == "string" || typeName == "str")
                {
                    typeSize = 32; // (AR) حجم بنية النص / (EN) String struct size
                }
                else if (typeName == "مصفوفة" || typeName == "array")
                {
                    typeSize = 24; // (AR) حجم بنية المصفوفة / (EN) Array struct size
                }
                else if (typeName == "خريطة" || typeName == "map")
                {
                    typeSize = 24; // (AR) حجم بنية الخريطة / (EN) Map struct size
                }
                else if (typeName == "i8" || typeName == "char" || typeName == "حرف")
                {
                    typeSize = 1;
                }
                else if (typeName == "i16" || typeName == "short")
                {
                    typeSize = 2;
                }
                else if (typeName == "i32" || typeName == "f32")
                {
                    typeSize = 4;
                }

                // (AR) إنشاء تعليمة Sizeof
                // (EN) Create Sizeof instruction
                std::string resultReg = newTempRegister();
                SIRInstruction sizeofInst;
                sizeofInst.opcode = SIROpcode::Sizeof;
                sizeofInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                sizeofInst.operands.push_back(SIROperand::ConstantI64(typeSize));
                sizeofInst.operands.push_back(SIROperand::ConstantString(typeName));
                sizeofInst.comment = "@حجم(" + typeName + ") = " + std::to_string(typeSize);

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(sizeofInst);
                }

                // (AR) إرجاع القيمة كثابت
                // (EN) Return value as constant
                BuildResult result(resultReg, SadTypeKind::Integer);
                result.isConstant = true;
                result.constantValue = std::to_string(typeSize);
                return result;
            }

            // ============================================================================
            // buildExprAtomic
            // ============================================================================
            BuildResult SIRBuilder::buildExprAtomic(AST::AtomicExpr *atomicExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found AtomicExpr, operation: "
                          << atomicExpr->operation << std::endl;
#endif

                std::string op = atomicExpr->operation;
                std::string resultReg = newTempRegister();

                // (AR) بناء المعاملات
                // (EN) Build operands
                std::vector<BuildResult> operandResults;
                for (const auto &operand : atomicExpr->operands)
                {
                    operandResults.push_back(buildExpression(operand.get()));
                }

                SIRInstruction atomicInst;

                // (AR) تحديد نوع العملية الذرية
                // (EN) Determine atomic operation type
                if (op == "تحميل" || op == "load")
                {
                    atomicInst.opcode = SIROpcode::AtomicLoad;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    if (!operandResults.empty())
                    {
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[0].registerName, operandResults[0].type));
                    }
                    atomicInst.comment = "@ذري(تحميل)";
                }
                else if (op == "تخزين" || op == "store")
                {
                    atomicInst.opcode = SIROpcode::AtomicStore;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Void);
                    if (operandResults.size() >= 2)
                    {
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[0].registerName, operandResults[0].type));
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[1].registerName, operandResults[1].type));
                    }
                    atomicInst.comment = "@ذري(تخزين)";
                }
                else if (op == "جمع" || op == "add")
                {
                    atomicInst.opcode = SIROpcode::AtomicAdd;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    if (operandResults.size() >= 2)
                    {
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[0].registerName, operandResults[0].type));
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[1].registerName, operandResults[1].type));
                    }
                    atomicInst.comment = "@ذري(جمع)";
                }
                else if (op == "طرح" || op == "sub")
                {
                    atomicInst.opcode = SIROpcode::AtomicSub;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    if (operandResults.size() >= 2)
                    {
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[0].registerName, operandResults[0].type));
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[1].registerName, operandResults[1].type));
                    }
                    atomicInst.comment = "@ذري(طرح)";
                }
                else if (op == "تبديل" || op == "exchange" || op == "xchg")
                {
                    atomicInst.opcode = SIROpcode::AtomicExchange;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    if (operandResults.size() >= 2)
                    {
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[0].registerName, operandResults[0].type));
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[1].registerName, operandResults[1].type));
                    }
                    atomicInst.comment = "@ذري(تبديل)";
                }
                else if (op == "قارن_وبدل" || op == "cmpxchg" || op == "cas")
                {
                    atomicInst.opcode = SIROpcode::AtomicCmpXchg;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    if (operandResults.size() >= 3)
                    {
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[0].registerName, operandResults[0].type));
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[1].registerName, operandResults[1].type));
                        atomicInst.operands.push_back(
                            SIROperand::Register(operandResults[2].registerName, operandResults[2].type));
                    }
                    atomicInst.comment = "@ذري(قارن_وبدل)";
                }
                else
                {
                    // (AR) عملية غير معروفة - نستخدم AtomicLoad كافتراضي
                    // (EN) Unknown operation - use AtomicLoad as default
                    atomicInst.opcode = SIROpcode::AtomicLoad;
                    atomicInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    atomicInst.comment = "@ذري(" + op + ")";
                }

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(atomicInst);
                }

                return BuildResult(resultReg, SadTypeKind::Integer);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
