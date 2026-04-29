// ============================================================================
// sir_builder_builtins_simd.cpp
// ============================================================================
// المؤلف / Author: Sad Compiler Team — Phase 3 SIMD Vectors
// التاريخ / Date: 2026
//
// (AR) دعم SIMD المتجهات في مُولّد SIR.
//      هذا الملف يكتشف استدعاءات الدوال المضمنة الخاصة بمتجهات SIMD ويُحوّلها
//      إلى تعليمات VECTOR_* في SIR. تعمل في طبقة LLVM على إصدار تعليمات
//      <N x T> أصلية (FMA, sqrt.v8f64, vector.reduce.fadd...) للحصول على
//      أداء يتفوق على C++ scalar.
//
//      الدوال المدعومة:
//        متجه(عناصر...)           — بناء متجه من قيم سُلَّمية
//        متجه_بث(قيمة، N)        — بث قيمة سُلَّمية إلى N lanes
//        متجه_جمع(أ، ب)          — جمع متجهين
//        متجه_طرح(أ، ب)          — طرح متجهين
//        متجه_ضرب(أ، ب)          — ضرب عناصر-عناصر
//        متجه_قسمة(أ، ب)         — قسمة عناصر-عناصر
//        متجه_ضرب_جمع(أ، ب، ج) — FMA: a*b+c (تعليمة واحدة على CPU حديث)
//        متجه_جذر(م)             — sqrt على كل lane
//        متجه_مطلق(م)            — abs على كل lane
//        متجه_أصغر(أ، ب)         — min عنصراً-عنصراً
//        متجه_أكبر(أ، ب)         — max عنصراً-عنصراً
//        متجه_جمع_عرضي(م)       — horizontal sum → scalar
//        متجه_ضرب_عرضي(م)       — horizontal product → scalar
//        متجه_أدنى_عرضي(م)      — horizontal min
//        متجه_أقصى_عرضي(م)      — horizontal max
//        متجه_جداء_قياسي(أ، ب) — dot product → scalar
//        متجه_طول(م)             — يُرجع عدد lanes
//        متجه_عنصر(م، فهرس)     — استخراج عنصر
//        متجه_ضع(م، فهرس، ق)   — إدراج عنصر
//
// (EN) SIMD vector support in SIR builder. Detects SIMD vector builtin calls
//      and converts them to VECTOR_* SIR instructions. The LLVM layer emits
//      native <N x T> instructions (FMA, sqrt.v8f64, vector.reduce.fadd...)
//      for performance exceeding C++ scalar code.
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include "utf8_utils.h"
#include <iostream>
#include <optional>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ────────────────────────────────────────────────────────────────────
            // (AR) دالة مساعدة داخلية: استنتاج نوع عنصر المتجه من المعاملات
            // (EN) Internal helper: infer element type from operands
            // ────────────────────────────────────────────────────────────────────
            static SadTypeKind inferVectorElementType(const std::vector<BuildResult> &args)
            {
                if (args.empty())
                    return SadTypeKind::Float; // افتراضي
                // (AR) إذا أي عنصر عشري، النوع عشري
                for (const auto &a : args)
                {
                    if (a.type == SadTypeKind::Float)
                        return SadTypeKind::Float;
                }
                // (AR) كل العناصر صحيحة
                return SadTypeKind::Integer;
            }

            // ────────────────────────────────────────────────────────────────────
            // (AR) دالة مساعدة: تحديد عدد lanes قانوني
            //      القيم المسموحة: 2, 4, 8, 16, 32, 64
            // (EN) Helper: validate lane count (must be power of 2 in [2,64])
            // ────────────────────────────────────────────────────────────────────
            static bool isValidLaneCount(int n)
            {
                return n == 2 || n == 4 || n == 8 || n == 16 || n == 32 || n == 64;
            }

            // ────────────────────────────────────────────────────────────────────
            // (AR) دالة مساعدة: بناء تعليمة VECTOR_* بسيطة بمعامل واحد أو اثنين
            // (EN) Helper: emit a simple VECTOR_* instruction (1 or 2 operands)
            // ────────────────────────────────────────────────────────────────────
            static void emitVectorOp(SIRBuilder *builder,
                                     std::shared_ptr<SIRBasicBlock> block,
                                     SIROpcode opcode,
                                     const SIROperand &result,
                                     const std::vector<SIROperand> &operands,
                                     SadTypeKind elemType,
                                     int laneCount,
                                     const std::string &comment)
            {
                (void)builder;
                SIRInstruction inst(opcode);
                inst.result = result;
                for (const auto &op : operands)
                    inst.operands.push_back(op);
                inst.vectorElementType = static_cast<int>(elemType);
                inst.vectorLaneCount = laneCount;
                inst.comment = comment;
                if (block)
                    block->instructions.push_back(inst);
            }

            // ============================================================================
            // (AR) المعالج الرئيسي للدوال المضمنة الخاصة بـ SIMD
            // (EN) Main dispatcher for SIMD vector builtin calls
            // ============================================================================
            std::optional<BuildResult> BuiltinBuilder::buildBuiltinCallSimd(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // (AR) إذا كانت الدالة معرّفة من المستخدم، تجاوز
                // (EN) Skip if user-defined function
                if (isUserDefinedFunction)
                    return std::nullopt;

                // ─────────────────────────────────────────────────────────────
                // (AR) متجه(عناصر...) — بناء متجه من قيم سُلَّمية
                //      عدد الـ lanes = عدد المعاملات. النوع يُستنتج تلقائياً.
                //      أمثلة: متجه(1.0، 2.0، 3.0، 4.0) → <4 x double>
                //              متجه(1، 2، 3، 4، 5، 6، 7، 8) → <8 x i64>
                // (EN) vector(elements...) — build vector from scalars
                // ─────────────────────────────────────────────────────────────
                if (funcName == "متجه" || funcName == "vector")
                {
                    if (argResults.empty())
                    {
                        b_.errors_.push_back("Error: متجه() requires at least 1 element");
                        return BuildResult();
                    }
                    int laneCount = Sad::Security::SafeArithmetic::assertSafeCast<int>(argResults.size(), "builtins_simd_size");
                    if (!isValidLaneCount(laneCount))
                    {
                        b_.errors_.push_back("Error: متجه() lane count must be one of {2,4,8,16,32,64}, got " +
                                          std::to_string(laneCount));
                        return BuildResult();
                    }
                    SadTypeKind elemType = inferVectorElementType(argResults);

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Vector);

                    emitVectorOp(&b_, b_.currentBlock_, SIROpcode::VECTOR_BUILD,
                                 resultOp, argOperands, elemType, laneCount,
                                 "build SIMD vector with " + std::to_string(laneCount) + " lanes");
                    return BuildResult(resultReg, SadTypeKind::Vector);
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) متجه_بث(قيمة، N) — بث قيمة سُلَّمية إلى N lanes
                //      مثال: متجه_بث(1.5، 8) → <8 x double> {1.5,1.5,...}
                // (EN) splat(value, N) — broadcast scalar to N lanes
                // ─────────────────────────────────────────────────────────────
                if (funcName == "متجه_بث" || funcName == "splat")
                {
                    if (argResults.size() != 2)
                    {
                        b_.errors_.push_back("Error: متجه_بث(قيمة، N) requires exactly 2 arguments");
                        return BuildResult();
                    }
                    // (AR) المعامل الثاني يجب أن يكون ثابتاً صحيحاً (لتحديد عدد lanes وقت الترجمة)
                    // (EN) Second arg must be integer constant for compile-time lane count
                    if (argOperands[1].type != SIROperandType::CONSTANT ||
                        argOperands[1].dataType != SadTypeKind::Integer)
                    {
                        b_.errors_.push_back("Error: متجه_بث() lane count must be a compile-time integer constant");
                        return BuildResult();
                    }
                    int laneCount = static_cast<int>(argOperands[1].intValue);
                    if (!isValidLaneCount(laneCount))
                    {
                        b_.errors_.push_back("Error: متجه_بث() lane count must be one of {2,4,8,16,32,64}");
                        return BuildResult();
                    }
                    SadTypeKind elemType = argResults[0].type;

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Vector);

                    // (AR) نمرر فقط القيمة المُبثّة كمعامل (lane count يخزن في metadata)
                    emitVectorOp(&b_, b_.currentBlock_, SIROpcode::VECTOR_SPLAT,
                                 resultOp, {argOperands[0]}, elemType, laneCount,
                                 "splat to " + std::to_string(laneCount) + " lanes");
                    return BuildResult(resultReg, SadTypeKind::Vector);
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) العمليات الثنائية على متجهين: جمع/طرح/ضرب/قسمة/min/max
                // (EN) Binary vector operations: add/sub/mul/div/min/max
                // ─────────────────────────────────────────────────────────────
                struct BinaryOp
                {
                    const char *arabic;
                    const char *english;
                    SIROpcode opcode;
                };
                static const BinaryOp kBinaryOps[] = {
                    {"متجه_جمع", "vec_add", SIROpcode::VECTOR_ADD},
                    {"متجه_طرح", "vec_sub", SIROpcode::VECTOR_SUB},
                    {"متجه_ضرب", "vec_mul", SIROpcode::VECTOR_MUL},
                    {"متجه_قسمة", "vec_div", SIROpcode::VECTOR_DIV},
                    {"متجه_أصغر", "vec_min", SIROpcode::VECTOR_MIN},
                    {"متجه_أكبر", "vec_max", SIROpcode::VECTOR_MAX},
                    {"متجه_و", "vec_and", SIROpcode::VECTOR_AND},
                    {"متجه_أو", "vec_or", SIROpcode::VECTOR_OR},
                    {"متجه_حصري", "vec_xor", SIROpcode::VECTOR_XOR},
                };
                for (const auto &bop : kBinaryOps)
                {
                    if (funcName == bop.arabic || funcName == bop.english)
                    {
                        if (argResults.size() != 2)
                        {
                            b_.errors_.push_back(std::string("Error: ") + bop.arabic + "() requires 2 vector arguments");
                            return BuildResult();
                        }
                        if (argResults[0].type != SadTypeKind::Vector ||
                            argResults[1].type != SadTypeKind::Vector)
                        {
                            b_.errors_.push_back(std::string("Error: ") + bop.arabic + "() arguments must be vectors");
                            return BuildResult();
                        }

                        std::string resultReg = b_.newTempRegister();
                        SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Vector);
                        // (AR) الـ metadata (نوع/عدد lanes) ستُملأ من المعامل في LLVM codegen
                        emitVectorOp(&b_, b_.currentBlock_, bop.opcode,
                                     resultOp, {argOperands[0], argOperands[1]},
                                     SadTypeKind::Float, 0, std::string(bop.english));
                        return BuildResult(resultReg, SadTypeKind::Vector);
                    }
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) العمليات الأحادية: sqrt/abs/neg
                // (EN) Unary vector operations
                // ─────────────────────────────────────────────────────────────
                struct UnaryOp
                {
                    const char *arabic;
                    const char *english;
                    SIROpcode opcode;
                };
                static const UnaryOp kUnaryOps[] = {
                    {"متجه_جذر", "vec_sqrt", SIROpcode::VECTOR_SQRT},
                    {"متجه_مطلق", "vec_abs", SIROpcode::VECTOR_ABS},
                    {"متجه_سالب", "vec_neg", SIROpcode::VECTOR_NEG},
                };
                for (const auto &uop : kUnaryOps)
                {
                    if (funcName == uop.arabic || funcName == uop.english)
                    {
                        if (argResults.size() != 1)
                        {
                            b_.errors_.push_back(std::string("Error: ") + uop.arabic + "() requires 1 vector argument");
                            return BuildResult();
                        }
                        if (argResults[0].type != SadTypeKind::Vector)
                        {
                            b_.errors_.push_back(std::string("Error: ") + uop.arabic + "() argument must be a vector");
                            return BuildResult();
                        }
                        std::string resultReg = b_.newTempRegister();
                        SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Vector);
                        emitVectorOp(&b_, b_.currentBlock_, uop.opcode,
                                     resultOp, {argOperands[0]},
                                     SadTypeKind::Float, 0, std::string(uop.english));
                        return BuildResult(resultReg, SadTypeKind::Vector);
                    }
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) متجه_ضرب_جمع(أ، ب، ج) — FMA: a*b+c تعليمة واحدة fma.v8f64
                // (EN) FMA: fused multiply-add → llvm.fma intrinsic
                // ─────────────────────────────────────────────────────────────
                if (funcName == "متجه_ضرب_جمع" || funcName == "vec_fma")
                {
                    if (argResults.size() != 3)
                    {
                        b_.errors_.push_back("Error: متجه_ضرب_جمع(أ،ب،ج) requires 3 vector arguments");
                        return BuildResult();
                    }
                    for (int i = 0; i < 3; ++i)
                    {
                        if (argResults[i].type != SadTypeKind::Vector)
                        {
                            b_.errors_.push_back("Error: متجه_ضرب_جمع() all arguments must be vectors");
                            return BuildResult();
                        }
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Vector);
                    emitVectorOp(&b_, b_.currentBlock_, SIROpcode::VECTOR_FMA,
                                 resultOp, {argOperands[0], argOperands[1], argOperands[2]},
                                 SadTypeKind::Float, 0, "fused multiply-add");
                    return BuildResult(resultReg, SadTypeKind::Vector);
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) العمليات العرضية (Reductions): vector → scalar
                // (EN) Horizontal reductions
                // ─────────────────────────────────────────────────────────────
                struct ReduceOp
                {
                    const char *arabic;
                    const char *english;
                    SIROpcode opcode;
                };
                static const ReduceOp kReduceOps[] = {
                    {"متجه_جمع_عرضي", "vec_hsum", SIROpcode::VECTOR_HSUM},
                    {"متجه_ضرب_عرضي", "vec_hmul", SIROpcode::VECTOR_HMUL},
                    {"متجه_أدنى_عرضي", "vec_hmin", SIROpcode::VECTOR_HMIN},
                    {"متجه_أقصى_عرضي", "vec_hmax", SIROpcode::VECTOR_HMAX},
                };
                for (const auto &rop : kReduceOps)
                {
                    if (funcName == rop.arabic || funcName == rop.english)
                    {
                        if (argResults.size() != 1)
                        {
                            b_.errors_.push_back(std::string("Error: ") + rop.arabic + "() requires 1 vector argument");
                            return BuildResult();
                        }
                        if (argResults[0].type != SadTypeKind::Vector)
                        {
                            b_.errors_.push_back(std::string("Error: ") + rop.arabic + "() argument must be a vector");
                            return BuildResult();
                        }
                        // (AR) النتيجة عشري (نفترض متجهات عشرية في الغالب — LLVM codegen سيتعامل مع الأنواع)
                        std::string resultReg = b_.newTempRegister();
                        SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                        emitVectorOp(&b_, b_.currentBlock_, rop.opcode,
                                     resultOp, {argOperands[0]},
                                     SadTypeKind::Float, 0, std::string(rop.english));
                        return BuildResult(resultReg, SadTypeKind::Float);
                    }
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) متجه_جداء_قياسي(أ، ب) — dot product
                //      ينفذ كـ VECTOR_MUL ثم VECTOR_HSUM في LLVM codegen
                // (EN) Dot product: implemented as MUL + HSUM
                // ─────────────────────────────────────────────────────────────
                if (funcName == "متجه_جداء_قياسي" || funcName == "vec_dot")
                {
                    if (argResults.size() != 2)
                    {
                        b_.errors_.push_back("Error: متجه_جداء_قياسي(أ،ب) requires 2 vector arguments");
                        return BuildResult();
                    }
                    if (argResults[0].type != SadTypeKind::Vector ||
                        argResults[1].type != SadTypeKind::Vector)
                    {
                        b_.errors_.push_back("Error: متجه_جداء_قياسي() arguments must be vectors");
                        return BuildResult();
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    emitVectorOp(&b_, b_.currentBlock_, SIROpcode::VECTOR_DOT,
                                 resultOp, {argOperands[0], argOperands[1]},
                                 SadTypeKind::Float, 0, "dot product");
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) متجه_عنصر(م، فهرس) — استخراج عنصر
                // (EN) extract element
                // ─────────────────────────────────────────────────────────────
                if (funcName == "متجه_عنصر" || funcName == "vec_extract")
                {
                    if (argResults.size() != 2)
                    {
                        b_.errors_.push_back("Error: متجه_عنصر(م، فهرس) requires 2 arguments");
                        return BuildResult();
                    }
                    if (argResults[0].type != SadTypeKind::Vector)
                    {
                        b_.errors_.push_back("Error: متجه_عنصر() first argument must be a vector");
                        return BuildResult();
                    }
                    std::string resultReg = b_.newTempRegister();
                    // (AR) النوع المرتجع نستنتجه عشري كافتراضي — LLVM codegen سيفحص النوع الفعلي
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    emitVectorOp(&b_, b_.currentBlock_, SIROpcode::VECTOR_EXTRACT,
                                 resultOp, {argOperands[0], argOperands[1]},
                                 SadTypeKind::Float, 0, "extract element");
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) متجه_ضع(م، فهرس، ق) — إدراج عنصر، يُرجع متجه جديد
                // (EN) insert element → returns new vector
                // ─────────────────────────────────────────────────────────────
                if (funcName == "متجه_ضع" || funcName == "vec_insert")
                {
                    if (argResults.size() != 3)
                    {
                        b_.errors_.push_back("Error: متجه_ضع(م، فهرس، ق) requires 3 arguments");
                        return BuildResult();
                    }
                    if (argResults[0].type != SadTypeKind::Vector)
                    {
                        b_.errors_.push_back("Error: متجه_ضع() first argument must be a vector");
                        return BuildResult();
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Vector);
                    emitVectorOp(&b_, b_.currentBlock_, SIROpcode::VECTOR_INSERT,
                                 resultOp, {argOperands[0], argOperands[1], argOperands[2]},
                                 SadTypeKind::Float, 0, "insert element");
                    return BuildResult(resultReg, SadTypeKind::Vector);
                }

                // (AR) ليست دالة SIMD — تابع معالجات أخرى
                // (EN) Not a SIMD function — fall through to other handlers
                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
