// ============================================================================
// llvm_codegen_simd.cpp — Phase 3 SIMD Vector LLVM Codegen
// ============================================================================
// (AR) إصدار تعليمات LLVM متجهة <N x T> أصلية لعمليات SIMD.
//      يستخدم CreateFAdd/Add على أنواع متجهة → LLVM يُنتج SSE/AVX/NEON تلقائياً.
//      للعمليات الخاصة (FMA, sqrt, min, max, abs) نستخدم intrinsics.
//      للعمليات العرضية (hsum, hmin, hmax) نستخدم llvm.vector.reduce.*.
//
// (EN) Emits native LLVM <N x T> vector instructions for SIMD ops.
//      Uses CreateFAdd/Add on vector types → LLVM auto-generates SSE/AVX/NEON.
//      For specialized ops (FMA, sqrt, min, max, abs) uses intrinsics.
//      For reductions (hsum, hmin, hmax) uses llvm.vector.reduce.*.
// ============================================================================

#include "llvm_codegen.h"
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <iostream>
#include <vector>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        // ────────────────────────────────────────────────────────────────────
        // (AR) دوال مساعدة محلية
        // (EN) Local helpers
        // ────────────────────────────────────────────────────────────────────

        // (AR) تحويل SadTypeKind إلى نوع LLVM scalar
        // (EN) Convert SadTypeKind to LLVM scalar type
        static llvm::Type *scalarTypeFromKind(llvm::IRBuilder<> *builder, int kindAsInt)
        {
            auto kind = static_cast<SadTypeKind>(kindAsInt);
            switch (kind)
            {
            case SadTypeKind::Float:
                return builder->getDoubleTy();
            case SadTypeKind::Integer:
                return builder->getInt64Ty();
            case SadTypeKind::Boolean:
                return builder->getInt1Ty();
            default:
                // (AR) افتراضي: عشري (الأكثر شيوعاً في SIMD)
                return builder->getDoubleTy();
            }
        }

        // (AR) إنشاء نوع متجه LLVM <N x T>
        // (EN) Create LLVM vector type <N x T>
        static llvm::FixedVectorType *vectorType(llvm::IRBuilder<> *builder, int kindAsInt, int laneCount)
        {
            llvm::Type *elemTy = scalarTypeFromKind(builder, kindAsInt);
            return llvm::FixedVectorType::get(elemTy, laneCount);
        }

        // (AR) فحص هل نوع المتجه عشري (لتمييز FAdd عن Add، إلخ)
        // (EN) Check if vector type is float (for FAdd vs Add dispatch)
        static bool isFloatVector(llvm::Value *v)
        {
            if (!v)
                return false;
            auto *vt = llvm::dyn_cast<llvm::VectorType>(v->getType());
            if (!vt)
                return false;
            return vt->getElementType()->isFloatingPointTy();
        }

        // ============================================================================
        // (AR) Dispatcher رئيسي لتعليمات SIMD
        // (EN) Main dispatcher for SIMD instructions
        // ============================================================================
        llvm::Value *LLVMCodeGen::emitInstructionSimd(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            auto opcode = inst->opcode;
            llvm::Value *result = nullptr;

            switch (opcode)
            {
            // ─────────────────────────────────────────────────────────────
            // VECTOR_BUILD: %v = insertelement chain من القيم السُلَّمية
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_BUILD:
            {
                int laneCount = inst->vectorLaneCount;
                if (laneCount <= 0)
                    laneCount = static_cast<int>(inst->operands.size());

                llvm::FixedVectorType *vTy = vectorType(builder_.get(), inst->vectorElementType, laneCount);
                llvm::Type *elemTy = vTy->getElementType();

                // (AR) ابدأ بمتجه undef ثم ضع كل عنصر بـ insertelement
                // (EN) Start with undef, insert each element
                result = llvm::UndefValue::get(vTy);
                for (int i = 0; i < laneCount && i < (int)inst->operands.size(); ++i)
                {
                    llvm::Value *elem = resolveOperand(inst->operands[i]);
                    if (!elem)
                    {
                        reportError("VECTOR_BUILD: missing operand " + std::to_string(i));
                        return nullptr;
                    }
                    // (AR) تحويل النوع إذا لزم
                    if (elem->getType() != elemTy)
                    {
                        if (elemTy->isFloatingPointTy() && elem->getType()->isIntegerTy())
                            elem = builder_->CreateSIToFP(elem, elemTy, "i2f");
                        else if (elemTy->isIntegerTy() && elem->getType()->isFloatingPointTy())
                            elem = builder_->CreateFPToSI(elem, elemTy, "f2i");
                        else if (elemTy->isFloatingPointTy() && elem->getType()->isFloatingPointTy())
                            elem = builder_->CreateFPCast(elem, elemTy, "fpcast");
                    }
                    llvm::Value *idx = llvm::ConstantInt::get(builder_->getInt32Ty(), i);
                    result = builder_->CreateInsertElement(result, elem, idx, "vbuild");
                }
                break;
            }

            // ─────────────────────────────────────────────────────────────
            // VECTOR_SPLAT: بث قيمة سُلَّمية إلى N lanes
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_SPLAT:
            {
                int laneCount = inst->vectorLaneCount;
                if (laneCount <= 0)
                {
                    reportError("VECTOR_SPLAT: invalid lane count");
                    return nullptr;
                }
                llvm::FixedVectorType *vTy = vectorType(builder_.get(), inst->vectorElementType, laneCount);
                llvm::Type *elemTy = vTy->getElementType();

                llvm::Value *scalar = resolveOperand(inst->operands[0]);
                if (!scalar)
                    return nullptr;
                if (scalar->getType() != elemTy)
                {
                    if (elemTy->isFloatingPointTy() && scalar->getType()->isIntegerTy())
                        scalar = builder_->CreateSIToFP(scalar, elemTy, "i2f");
                    else if (elemTy->isIntegerTy() && scalar->getType()->isFloatingPointTy())
                        scalar = builder_->CreateFPToSI(scalar, elemTy, "f2i");
                }
                // (AR) أنشئ undef، ضع scalar في lane 0، ثم shuffle بقناع كله أصفار
                // (EN) undef → insert at lane 0 → shuffle with all-zero mask
                llvm::Value *undef = llvm::UndefValue::get(vTy);
                llvm::Value *zero = llvm::ConstantInt::get(builder_->getInt32Ty(), 0);
                llvm::Value *singleton = builder_->CreateInsertElement(undef, scalar, zero, "splat0");
                std::vector<int> mask(laneCount, 0);
                result = builder_->CreateShuffleVector(singleton, undef, mask, "splat");
                break;
            }

            // ─────────────────────────────────────────────────────────────
            // العمليات الثنائية على متجهين — LLVM يدعم vector ops أصلياً
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_ADD:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = isFloatVector(a) ? builder_->CreateFAdd(a, b, "vadd")
                                          : builder_->CreateAdd(a, b, "vadd");
                break;
            }
            case SIROpcode::VECTOR_SUB:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = isFloatVector(a) ? builder_->CreateFSub(a, b, "vsub")
                                          : builder_->CreateSub(a, b, "vsub");
                break;
            }
            case SIROpcode::VECTOR_MUL:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = isFloatVector(a) ? builder_->CreateFMul(a, b, "vmul")
                                          : builder_->CreateMul(a, b, "vmul");
                break;
            }
            case SIROpcode::VECTOR_DIV:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = isFloatVector(a) ? builder_->CreateFDiv(a, b, "vdiv")
                                          : builder_->CreateSDiv(a, b, "vdiv");
                break;
            }
            case SIROpcode::VECTOR_NEG:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                result = isFloatVector(a) ? builder_->CreateFNeg(a, "vneg")
                                          : builder_->CreateNeg(a, "vneg");
                break;
            }
            case SIROpcode::VECTOR_AND:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = builder_->CreateAnd(a, b, "vand");
                break;
            }
            case SIROpcode::VECTOR_OR:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = builder_->CreateOr(a, b, "vor");
                break;
            }
            case SIROpcode::VECTOR_XOR:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                result = builder_->CreateXor(a, b, "vxor");
                break;
            }

            // ─────────────────────────────────────────────────────────────
            // intrinsics: FMA, sqrt, abs, min, max
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_FMA:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                llvm::Value *c = resolveOperand(inst->operands[2]);
                if (!a || !b || !c)
                    return nullptr;
                // (AR) llvm.fma.<vectorType> — تعليمة واحدة fma على CPU حديث
                llvm::Function *fmaFn = llvm::Intrinsic::getDeclaration(
                    module_.get(), llvm::Intrinsic::fma, {a->getType()});
                result = builder_->CreateCall(fmaFn, {a, b, c}, "vfma");
                break;
            }
            case SIROpcode::VECTOR_SQRT:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                    module_.get(), llvm::Intrinsic::sqrt, {a->getType()});
                result = builder_->CreateCall(fn, {a}, "vsqrt");
                break;
            }
            case SIROpcode::VECTOR_ABS:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                if (isFloatVector(a))
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::fabs, {a->getType()});
                    result = builder_->CreateCall(fn, {a}, "vabs");
                }
                else
                {
                    // llvm.abs.vN.iM(vector, isIntMinPoison: i1)
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::abs, {a->getType()});
                    llvm::Value *poison = builder_->getInt1(false);
                    result = builder_->CreateCall(fn, {a, poison}, "vabs");
                }
                break;
            }
            case SIROpcode::VECTOR_MIN:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                if (isFloatVector(a))
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::minnum, {a->getType()});
                    result = builder_->CreateCall(fn, {a, b}, "vmin");
                }
                else
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::smin, {a->getType()});
                    result = builder_->CreateCall(fn, {a, b}, "vmin");
                }
                break;
            }
            case SIROpcode::VECTOR_MAX:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                if (isFloatVector(a))
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::maxnum, {a->getType()});
                    result = builder_->CreateCall(fn, {a, b}, "vmax");
                }
                else
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::smax, {a->getType()});
                    result = builder_->CreateCall(fn, {a, b}, "vmax");
                }
                break;
            }

            // ─────────────────────────────────────────────────────────────
            // Reductions: vector → scalar
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_HSUM:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                if (isFloatVector(a))
                {
                    // llvm.vector.reduce.fadd needs a starting accumulator
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::vector_reduce_fadd, {a->getType()});
                    auto *vt = llvm::cast<llvm::VectorType>(a->getType());
                    llvm::Value *zero = llvm::ConstantFP::get(vt->getElementType(), 0.0);
                    result = builder_->CreateCall(fn, {zero, a}, "vhsum");
                }
                else
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::vector_reduce_add, {a->getType()});
                    result = builder_->CreateCall(fn, {a}, "vhsum");
                }
                break;
            }
            case SIROpcode::VECTOR_HMUL:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                if (isFloatVector(a))
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::vector_reduce_fmul, {a->getType()});
                    auto *vt = llvm::cast<llvm::VectorType>(a->getType());
                    llvm::Value *one = llvm::ConstantFP::get(vt->getElementType(), 1.0);
                    result = builder_->CreateCall(fn, {one, a}, "vhmul");
                }
                else
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::vector_reduce_mul, {a->getType()});
                    result = builder_->CreateCall(fn, {a}, "vhmul");
                }
                break;
            }
            case SIROpcode::VECTOR_HMIN:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                llvm::Intrinsic::ID id = isFloatVector(a) ? llvm::Intrinsic::vector_reduce_fmin
                                                          : llvm::Intrinsic::vector_reduce_smin;
                llvm::Function *fn = llvm::Intrinsic::getDeclaration(module_.get(), id, {a->getType()});
                result = builder_->CreateCall(fn, {a}, "vhmin");
                break;
            }
            case SIROpcode::VECTOR_HMAX:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                if (!a)
                    return nullptr;
                llvm::Intrinsic::ID id = isFloatVector(a) ? llvm::Intrinsic::vector_reduce_fmax
                                                          : llvm::Intrinsic::vector_reduce_smax;
                llvm::Function *fn = llvm::Intrinsic::getDeclaration(module_.get(), id, {a->getType()});
                result = builder_->CreateCall(fn, {a}, "vhmax");
                break;
            }

            // ─────────────────────────────────────────────────────────────
            // VECTOR_DOT: a*b ثم vector.reduce.fadd
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_DOT:
            {
                llvm::Value *a = resolveOperand(inst->operands[0]);
                llvm::Value *b = resolveOperand(inst->operands[1]);
                if (!a || !b)
                    return nullptr;
                llvm::Value *prod = isFloatVector(a) ? builder_->CreateFMul(a, b, "vdot.mul")
                                                     : builder_->CreateMul(a, b, "vdot.mul");
                if (isFloatVector(prod))
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::vector_reduce_fadd, {prod->getType()});
                    auto *vt = llvm::cast<llvm::VectorType>(prod->getType());
                    llvm::Value *zero = llvm::ConstantFP::get(vt->getElementType(), 0.0);
                    result = builder_->CreateCall(fn, {zero, prod}, "vdot");
                }
                else
                {
                    llvm::Function *fn = llvm::Intrinsic::getDeclaration(
                        module_.get(), llvm::Intrinsic::vector_reduce_add, {prod->getType()});
                    result = builder_->CreateCall(fn, {prod}, "vdot");
                }
                break;
            }

            // ─────────────────────────────────────────────────────────────
            // VECTOR_EXTRACT / VECTOR_INSERT
            // ─────────────────────────────────────────────────────────────
            case SIROpcode::VECTOR_EXTRACT:
            {
                llvm::Value *v = resolveOperand(inst->operands[0]);
                llvm::Value *idx = resolveOperand(inst->operands[1]);
                if (!v || !idx)
                    return nullptr;
                // (AR) idx لازم i32 لـ extractelement
                if (idx->getType() != builder_->getInt32Ty())
                    idx = builder_->CreateTrunc(idx, builder_->getInt32Ty(), "idx32");
                result = builder_->CreateExtractElement(v, idx, "vext");
                break;
            }
            case SIROpcode::VECTOR_INSERT:
            {
                llvm::Value *v = resolveOperand(inst->operands[0]);
                llvm::Value *idx = resolveOperand(inst->operands[1]);
                llvm::Value *elem = resolveOperand(inst->operands[2]);
                if (!v || !idx || !elem)
                    return nullptr;
                if (idx->getType() != builder_->getInt32Ty())
                    idx = builder_->CreateTrunc(idx, builder_->getInt32Ty(), "idx32");
                // (AR) تطابق نوع العنصر مع نوع عنصر المتجه
                auto *vt = llvm::cast<llvm::VectorType>(v->getType());
                llvm::Type *elemTy = vt->getElementType();
                if (elem->getType() != elemTy)
                {
                    if (elemTy->isFloatingPointTy() && elem->getType()->isIntegerTy())
                        elem = builder_->CreateSIToFP(elem, elemTy, "i2f");
                    else if (elemTy->isIntegerTy() && elem->getType()->isFloatingPointTy())
                        elem = builder_->CreateFPToSI(elem, elemTy, "f2i");
                }
                result = builder_->CreateInsertElement(v, elem, idx, "vins");
                break;
            }

            // (AR) opcodes أخرى غير مدعومة هنا — ارجع nullptr لتمر للطبقات التالية
            // (EN) Other opcodes not handled here — return nullptr for next tier
            default:
                return nullptr;
            }

            // (AR) حفظ النتيجة في خريطة السجلات
            // (EN) Save result in register map
            if (result && inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

    } // namespace LLVM
} // namespace Sad
