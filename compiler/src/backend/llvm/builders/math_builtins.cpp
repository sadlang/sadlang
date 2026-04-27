/*
 * ============================================================================
 * MathBuiltinsCodeGen — توليد LLVM IR لدوال الرياضيات المضمنة
 * Phase 7 Step 8 — 21 methods استُخرِجت من ملفين
 * ============================================================================
 */
#include "builders/math_builtins_codegen.h"
#include "llvm_codegen.h"
#include "sir_constants.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Intrinsics.h>

using namespace Sad::Compiler::SIR;

namespace Sad {
namespace LLVM {

        // ============================================================================
        // Phase N: Builtin Extra / دوال مضمنة إضافية
        // ============================================================================

        llvm::Value *MathBuiltinsCodeGen::emitBuiltinMin(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError("MIN requires 2 operands");
                return nullptr;
            }
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;

            llvm::Value *result;
            if (a->getType()->isDoubleTy() || b->getType()->isDoubleTy())
            {
                if (!a->getType()->isDoubleTy())
                    a = cg_.builder_->CreateSIToFP(a, cg_.getDoubleType(), "a.f64");
                if (!b->getType()->isDoubleTy())
                    b = cg_.builder_->CreateSIToFP(b, cg_.getDoubleType(), "b.f64");
                llvm::Value *cmp = cg_.builder_->CreateFCmpOLT(a, b, "min.cmp");
                result = cg_.builder_->CreateSelect(cmp, a, b, "min");
            }
            else
            {
                llvm::Value *cmp = cg_.builder_->CreateICmpSLT(a, b, "min.cmp");
                result = cg_.builder_->CreateSelect(cmp, a, b, "min");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinMax(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError("MAX requires 2 operands");
                return nullptr;
            }
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;

            llvm::Value *result;
            if (a->getType()->isDoubleTy() || b->getType()->isDoubleTy())
            {
                if (!a->getType()->isDoubleTy())
                    a = cg_.builder_->CreateSIToFP(a, cg_.getDoubleType(), "a.f64");
                if (!b->getType()->isDoubleTy())
                    b = cg_.builder_->CreateSIToFP(b, cg_.getDoubleType(), "b.f64");
                llvm::Value *cmp = cg_.builder_->CreateFCmpOGT(a, b, "max.cmp");
                result = cg_.builder_->CreateSelect(cmp, a, b, "max");
            }
            else
            {
                llvm::Value *cmp = cg_.builder_->CreateICmpSGT(a, b, "max.cmp");
                result = cg_.builder_->CreateSelect(cmp, a, b, "max");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinSqrt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sqrt", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "sqrt.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinLog(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("log", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "log.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinSin(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sin", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "sin.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinCos(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("cos", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "cos.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinTan(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("tan", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "tan.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinAbs(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;

            // Check if input is integer type - if so, return integer abs
            bool inputIsInt = arg->getType()->isIntegerTy();

            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("fabs", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "fabs.ret");

            // If input was integer, convert result back to i64
            if (inputIsInt)
            {
                result = cg_.builder_->CreateFPToSI(result, llvm::Type::getInt64Ty(*cg_.context_));
            }

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinFloor(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("floor", ft);
            llvm::Value *result = cg_.builder_->CreateFPToSI(cg_.builder_->CreateCall(fn, {dArg}, "floor.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinCeil(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("ceil", ft);
            llvm::Value *result = cg_.builder_->CreateFPToSI(cg_.builder_->CreateCall(fn, {dArg}, "ceil.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinRound(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("round", ft);
            llvm::Value *result = cg_.builder_->CreateFPToSI(cg_.builder_->CreateCall(fn, {dArg}, "round.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinPow(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *base = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *exp = cg_.resolveOperand(inst->operands[1]);
            if (!base || !exp)
                return nullptr;
            llvm::Type *dblTy = llvm::Type::getDoubleTy(*cg_.context_);
            llvm::Value *dBase = base->getType()->isDoubleTy() ? base : cg_.builder_->CreateSIToFP(base, dblTy);
            llvm::Value *dExp = exp->getType()->isDoubleTy() ? exp : cg_.builder_->CreateSIToFP(exp, dblTy);
            llvm::FunctionType *ft = llvm::FunctionType::get(dblTy, {dblTy, dblTy}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("pow", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dBase, dExp}, "pow.ret");
            // (AR) إصلاح BF-04 (OE-006/OE-029): الحفاظ على نوع النتيجة المتوقع.
            //      إذا كان النوع المتوقع للنتيجة Float (لأن أحد المعاملين عشري) → نُبقي double.
            //      إذا كان Integer (كلاهما صحيح) → نحوّل إلى i64 لمطابقة المفسر.
            //      السلوك السابق: تحويل قسري إلى i64 دائماً، وكان sir_builder يضع
            //      resultType=Float فيُطبع بتنسيق %f مع قيمة i64 خاطئة.
            // (EN) BF-04 fix: respect inst->result type. Float-expected → keep double;
            //      Int-expected → cast to i64. Previous code force-casted to i64 always,
            //      while sir_builder always set Float type → print formatted as %f with
            //      a value that was actually an integer bit pattern.
            bool resultIsFloat = inst->result.has_value() &&
                                 inst->result->dataType == SadTypeKind::Float;
            if (!resultIsFloat)
            {
                // (AR) تحويل النتيجة من double إلى i64 لأن لغة ص تتعامل مع الأعداد كـ i64
                // (EN) Convert result from double to i64 since Sad treats numbers as i64
                result = cg_.builder_->CreateFPToSI(result, cg_.getInt64Type(), "pow.i64");
            }
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        // ============================================================================
        // (AR) دوال رياضية إضافية — وحدة رياضيات الموسّعة
        // (EN) Extended math builtins — stdlib رياضيات module
        // ============================================================================

        llvm::Value *MathBuiltinsCodeGen::emitBuiltinLog10(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("log10", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "log10.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinLog2(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("log2", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "log2.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinAsin(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("asin", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "asin.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinAcos(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("acos", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "acos.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinAtan(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("atan", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dArg}, "atan.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinTrunc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arg = cg_.resolveOperand(inst->operands[0]);
            if (!arg)
                return nullptr;
            llvm::Value *dArg = arg->getType()->isDoubleTy() ? arg : cg_.builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {llvm::Type::getDoubleTy(*cg_.context_)}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("trunc", ft);
            llvm::Value *result = cg_.builder_->CreateFPToSI(cg_.builder_->CreateCall(fn, {dArg}, "trunc.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinFmod(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;
            llvm::Type *dblTy = llvm::Type::getDoubleTy(*cg_.context_);
            llvm::Value *dA = a->getType()->isDoubleTy() ? a : cg_.builder_->CreateSIToFP(a, dblTy);
            llvm::Value *dB = b->getType()->isDoubleTy() ? b : cg_.builder_->CreateSIToFP(b, dblTy);
            llvm::FunctionType *ft = llvm::FunctionType::get(dblTy, {dblTy, dblTy}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("fmod", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dA, dB}, "fmod.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinClamp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *minVal = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *maxVal = cg_.resolveOperand(inst->operands[2]);
            if (!val || !minVal || !maxVal)
                return nullptr;

            llvm::Type *dblTy = llvm::Type::getDoubleTy(*cg_.context_);
            llvm::Value *dVal = val->getType()->isDoubleTy() ? val : cg_.builder_->CreateSIToFP(val, dblTy);
            llvm::Value *dMin = minVal->getType()->isDoubleTy() ? minVal : cg_.builder_->CreateSIToFP(minVal, dblTy);
            llvm::Value *dMax = maxVal->getType()->isDoubleTy() ? maxVal : cg_.builder_->CreateSIToFP(maxVal, dblTy);

            // clamp(x, lo, hi) = max(lo, min(x, hi))
            // min(x, hi): if x < hi then x else hi
            llvm::Value *cmpLt = cg_.builder_->CreateFCmpOLT(dVal, dMax, "clamp.lt.max");
            llvm::Value *minResult = cg_.builder_->CreateSelect(cmpLt, dVal, dMax, "clamp.min");
            // max(lo, minResult): if minResult > lo then minResult else lo
            llvm::Value *cmpGt = cg_.builder_->CreateFCmpOGT(minResult, dMin, "clamp.gt.min");
            llvm::Value *result = cg_.builder_->CreateSelect(cmpGt, minResult, dMin, "clamp.ret");

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *MathBuiltinsCodeGen::emitBuiltinRandom(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("rand", ft);
            llvm::Value *rval = cg_.builder_->CreateCall(fn, {}, "rand.ret");
            llvm::Value *dval = cg_.builder_->CreateSIToFP(rval, llvm::Type::getDoubleTy(*cg_.context_));
            llvm::Value *result = cg_.builder_->CreateFDiv(dval, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*cg_.context_), 2147483647.0));
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }


} // namespace LLVM
} // namespace Sad
