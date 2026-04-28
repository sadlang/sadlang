/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>
#ifdef _WIN32
#include <filesystem>
#include <windows.h>
#else
#include <cstdio>
#include "builders/arithmetic/arithmetic_codegen.h" // (Phase 7 Step 1)
#include "llvm_codegen.h"
#endif

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
// ============================================================================
// (AR) تحليل المعاملات: resolveOperand — تحويل SIROperand إلى llvm::Value
// (EN) Operand resolution: resolveOperand — convert SIROperand to llvm::Value
// (AR) تم فصله من llvm_codegen_output.cpp وفق CW-05
// ============================================================================
        llvm::Value *ArithmeticCodeGen::resolveOperand(const SIROperand &operand)
        {
            switch (operand.type)
            {
            case SIROperandType::CONSTANT:
            {
                switch (operand.dataType)
                {
                case SadTypeKind::Integer:
                    // case SadTypeKind::I32:
                    // case SadTypeKind::I16:
                    // case SadTypeKind::I8:
                    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), operand.intValue, true);
                case SadTypeKind::Boolean:
                    // (AR) القيم المنطقية يجب أن تكون i1 وليس i64
                    // (EN) Boolean values must be i1 not i64 — fixes type mismatch in branch conditions
                    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*cg_.context_), operand.intValue != 0 ? 1 : 0);
                case SadTypeKind::Float:
                    // case SadTypeKind::F32:
                    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*cg_.context_), operand.floatValue);
                case SadTypeKind::String:
                {
                    return cg_.builder_->CreateGlobalStringPtr(operand.name, "str.const");
                }
                default:
                    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), operand.intValue, true);
                }
            }
            case SIROperandType::REGISTER:
            {
                // (AR) تخطي السجلات بأسماء فارغة — ناتجة عن تعبيرات غير محلولة في SIR
                // (EN) Skip registers with empty names — caused by unresolved expressions in SIR
                if (operand.name.empty())
                {
                    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0, true);
                }
                auto it = cg_.context_info_.namedValues.find(operand.name);
                if (it != cg_.context_info_.namedValues.end() && it->second != nullptr)
                {
                    llvm::Value *val = it->second;
                    // (AR) إذا كان النوع نص، لا نحمّل - نُرجع المؤشر مباشرة
                    // (EN) If data type is STRING, don't load - return pointer directly
                    // (AR) النص عبارة عن مؤشر i8* — إذا كان alloca نحمّل المؤشر المخزّن
                    //      وإلا نرجع المؤشر مباشرة (مثل GlobalStringPtr)
                    // (EN) String is an i8* pointer — if it's an alloca, load the stored pointer
                    //      otherwise return the pointer as-is (e.g. GlobalStringPtr)
                    if (operand.dataType == SadTypeKind::String)
                    {
                        if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(val))
                        {
                            // (AR) alloca يحتوي مؤشر نص — نحمّل المؤشر المخزّن
                            // (EN) Alloca holds a string pointer — load the stored pointer
                            return cg_.builder_->CreateLoad(
                                allocaInst->getAllocatedType(), allocaInst,
                                operand.name + ".load");
                        }
                        // (AR) المتغير العالمي للنصوص — إذا كان مصفوفة [N x i8] (ثابت نصي) نستخدم GEP
                        //      وإذا كان i64 (مؤشر مخزّن كعدد) نحمّله ونحوّله لمؤشر
                        // (EN) Global string variable — if array [N x i8] (string constant) use GEP,
                        //      if i64 (pointer stored as integer) load and inttoptr
                        if (auto *gv = llvm::dyn_cast<llvm::GlobalVariable>(val))
                        {
                            if (gv->getValueType()->isArrayTy())
                            {
                                // (AR) ثابت نصي مثل @true_str من نوع [N x i8] — GEP للعنصر الأول
                                // (EN) String constant like @true_str of type [N x i8] — GEP to first element
                                return cg_.builder_->CreateInBoundsGEP(
                                    gv->getValueType(), gv,
                                    {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0),
                                     llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0)},
                                    operand.name + ".strptr");
                            }
                            // (AR) متغير عام يخزّن مؤشر النص كـ i64 — نحمّله ثم نحوّله لمؤشر
                            // (EN) Global variable stores string pointer as i64 — load and convert to ptr
                            llvm::Value *loaded = cg_.builder_->CreateLoad(
                                gv->getValueType(), gv, operand.name + ".gload");
                            return cg_.builder_->CreateIntToPtr(loaded,
                                                            llvm::PointerType::getUnqual(*cg_.context_), operand.name + ".strptr");
                        }
                        return val; // Return the pointer as-is
                    }

                    // (AR) إذا كانت القيمة مؤشر alloca، نحمّل القيمة الفعلية تلقائياً
                    // (EN) If value is an alloca pointer, auto-load the actual value
                    if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(val))
                    {
                        // (AR) لا نحمّل إذا كان alloca لمصفوفة (i8 مع حجم > 1)
                        // (EN) Don't load if it's an array alloca (i8 with size > 1)
                        // هذا يشمل النصوص المُجمّعة (concat.buf)
                        // This includes concatenated strings (concat.buf)
                        if (allocaInst->getAllocatedType()->isIntegerTy(8) && allocaInst->isArrayAllocation())
                        {
                            return val; // Return buffer pointer as-is
                        }
                        // (AR) لا نحمّل إذا كان alloca لهيكل صنف - نُرجع المؤشر مباشرة
                        // (EN) Don't load if it's a struct alloca - return pointer as-is
                        // هذا ضروري لتمرير الكائنات للدوال والبناة
                        // This is needed for passing objects to methods and constructors
                        if (allocaInst->getAllocatedType()->isStructTy())
                        {
                            return val; // Return struct pointer as-is
                        }
                        return cg_.builder_->CreateLoad(allocaInst->getAllocatedType(), allocaInst, operand.name + ".load");
                    }
                    // (AR) إذا كانت القيمة متغير عام، نحمّل القيمة من الذاكرة
                    // (EN) If value is a global variable, auto-load from memory
                    if (auto *gvInst = llvm::dyn_cast<llvm::GlobalVariable>(val))
                    {
                        // (AR) إذا كان المتغير العالمي مصفوفة (مثل ثابت نصي [N x i8]) — نُرجع GEP بدلاً من تحميل المصفوفة
                        // (EN) If global is an array (e.g. string constant [N x i8]) — return GEP instead of loading array
                        if (gvInst->getValueType()->isArrayTy())
                        {
                            return cg_.builder_->CreateInBoundsGEP(
                                gvInst->getValueType(), gvInst,
                                {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0),
                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0)},
                                operand.name + ".ptr");
                        }
                        return cg_.builder_->CreateLoad(gvInst->getValueType(), gvInst, operand.name + ".load");
                    }
                    // (AR) إذا كانت القيمة مؤشر GEP (حقل في هيكل)، نحمّل القيمة تلقائياً
                    // (EN) If value is a GEP instruction (struct field pointer), auto-load the value
                    if (auto *gepInst = llvm::dyn_cast<llvm::GetElementPtrInst>(val))
                    {
                        llvm::Type *pointedType = gepInst->getResultElementType();
                        return cg_.builder_->CreateLoad(pointedType, gepInst, operand.name + ".load");
                    }
                    return val;
                }
                // (AR) بديل: البحث في المتغيرات العامة الدائمة ثم في الوحدة مباشرة
                // (EN) Fallback: search persistent globalValues map, then module globals
                // namedValues يُمسح عند دخول كل دالة، لكن globalValues لا تُمسح أبداً
                // namedValues is cleared on each function entry, but globalValues is never cleared
                {
                    // (AR) إزالة % من بداية الاسم إذا وجدت (السجلات تبدأ بـ % لكن المتغيرات العامة لا)
                    // (EN) Strip % prefix if present (registers start with % but globals don't)
                    std::string globalName = operand.name;
                    if (!globalName.empty() && globalName[0] == '%')
                    {
                        globalName = globalName.substr(1);
                    }

                    // (AR) البحث أولاً في الخريطة الدائمة للمتغيرات العالمية (الأكثر موثوقية)
                    // (EN) First search persistent global values map (most reliable)
                    auto git = cg_.context_info_.globalValues.find(globalName);
                    if (git == cg_.context_info_.globalValues.end())
                    {
                        // (AR) محاولة بالاسم الأصلي (مع %)
                        // (EN) Try with original name (with %)
                        git = cg_.context_info_.globalValues.find(operand.name);
                    }
                    if (git != cg_.context_info_.globalValues.end() && git->second != nullptr)
                    {
                        llvm::Value *gval = git->second;
                        if (auto *gvInst = llvm::dyn_cast<llvm::GlobalVariable>(gval))
                        {
                            // (AR) إذا كان مصفوفة [N x i8] (ثابت نصي) — GEP بدلاً من load
                            // (EN) If array [N x i8] (string constant) — GEP instead of load
                            if (gvInst->getValueType()->isArrayTy())
                            {
                                return cg_.builder_->CreateInBoundsGEP(
                                    gvInst->getValueType(), gvInst,
                                    {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0),
                                     llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0)},
                                    globalName + ".ptr");
                            }
                            // ================================================================
                            // (AR) إصلاح: المتغيرات النصية العمومية تُخزّن كـ i64 (ptrtoint)
                            //      عند تحميلها من داخل دالة عبر مسار globalValues,
                            //      يجب تحويل القيمة المحمّلة من i64 إلى ptr عبر inttoptr
                            //      بدون هذا: sprintf(%lld) يطبع قيمة المؤشر كرقم بدلاً من النص
                            // (EN) Fix: Global string vars stored as i64 (ptrtoint).
                            //      When loaded from inside a function via globalValues path,
                            //      must convert loaded i64 to ptr via inttoptr.
                            //      Without this: sprintf(%lld) prints pointer value as number
                            // ================================================================
                            {
                                llvm::Value *loaded = cg_.builder_->CreateLoad(gvInst->getValueType(), gvInst, globalName + ".gload");
                                if (operand.dataType == SadTypeKind::String && loaded->getType()->isIntegerTy(64))
                                {
                                    return cg_.builder_->CreateIntToPtr(loaded,
                                                                    llvm::PointerType::getUnqual(*cg_.context_), globalName + ".strptr");
                                }
                                return loaded;
                            }
                        }
                        // (AR) إذا لم يكن GlobalVariable (يحدث أحياناً)، نحاول التحميل العام
                        // (EN) If not GlobalVariable (rare), try generic load
                        if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(gval))
                        {
                            return cg_.builder_->CreateLoad(allocaInst->getAllocatedType(), allocaInst, globalName + ".load");
                        }
                        return gval;
                    }

                    // (AR) بديل ثانٍ: البحث في وحدة LLVM مباشرة
                    // (EN) Second fallback: search LLVM module directly
                    llvm::GlobalVariable *gv = cg_.module_->getGlobalVariable(globalName);
                    if (!gv)
                    {
                        gv = cg_.module_->getGlobalVariable(operand.name);
                    }
                    // (AR) بديل ثالث: إزالة بادئة التضمين _inlN_ والبحث مجدداً
                    // (EN) Third fallback: strip inlining prefix _inlN_ and search again
                    if (!gv)
                    {
                        std::string stripped = globalName;
                        // Pattern: _inl<digits>_<original_name>
                        if (stripped.size() > 4 && stripped.substr(0, 4) == "_inl")
                        {
                            size_t underscorePos = stripped.find('_', 4);
                            if (underscorePos != std::string::npos && underscorePos + 1 < stripped.size())
                            {
                                std::string originalName = stripped.substr(underscorePos + 1);
                                gv = cg_.module_->getGlobalVariable(originalName);
                            }
                        }
                    }
                    if (gv)
                    {
                        // (AR) إذا كان مصفوفة [N x i8] (ثابت نصي) — GEP بدلاً من load
                        // (EN) If array [N x i8] (string constant) — GEP instead of load
                        if (gv->getValueType()->isArrayTy())
                        {
                            return cg_.builder_->CreateInBoundsGEP(
                                gv->getValueType(), gv,
                                {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0),
                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0)},
                                globalName + ".ptr");
                        }
                        // ================================================================
                        // (AR) إصلاح: نفس المعالجة للمتغيرات النصية العمومية عبر مسار module
                        // (EN) Fix: Same treatment for global string vars via module path
                        // ================================================================
                        {
                            llvm::Value *loaded = cg_.builder_->CreateLoad(gv->getValueType(), gv, globalName + ".gload");
                            if (operand.dataType == SadTypeKind::String && loaded->getType()->isIntegerTy(64))
                            {
                                return cg_.builder_->CreateIntToPtr(loaded,
                                                                llvm::PointerType::getUnqual(*cg_.context_), globalName + ".strptr");
                            }
                            return loaded;
                        }
                    }
                }
                cg_.reportError("Undefined register: " + operand.name);
                // (AR) إرجاع قيمة صفرية ثابتة كـ fallback لتجنب crash
                // (EN) Return a zero constant as fallback to avoid crash
                return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0, true);
            }
            case SIROperandType::GLOBAL:
            {
                // (AR) البحث أولاً في الخريطة الدائمة ثم في الوحدة
                //      نزيل بادئة % إذا وجدت لأن المتغيرات العامة مسجلة بدون %
                // (EN) First search persistent map, then module
                //      Strip % prefix if present since globals are registered without %
                std::string cleanName = operand.name;
                if (!cleanName.empty() && cleanName[0] == '%')
                {
                    cleanName = cleanName.substr(1);
                }

                auto git = cg_.context_info_.globalValues.find(cleanName);
                if (git == cg_.context_info_.globalValues.end())
                {
                    // (AR) محاولة بالاسم الأصلي (مع % أو بدون)
                    // (EN) Try with original name
                    git = cg_.context_info_.globalValues.find(operand.name);
                }
                if (git != cg_.context_info_.globalValues.end() && git->second != nullptr)
                {
                    if (auto *gvInst = llvm::dyn_cast<llvm::GlobalVariable>(git->second))
                    {
                        // (AR) إذا كان مصفوفة [N x i8] (ثابت نصي) — GEP بدلاً من load
                        // (EN) If array [N x i8] (string constant) — GEP instead of load
                        if (gvInst->getValueType()->isArrayTy())
                        {
                            return cg_.builder_->CreateInBoundsGEP(
                                gvInst->getValueType(), gvInst,
                                {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0),
                                 llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0)},
                                cleanName + ".ptr");
                        }
                        // (AR) إصلاح: المتغيرات النصية العمومية — inttoptr بعد التحميل
                        // (EN) Fix: Global string vars — inttoptr after load
                        {
                            llvm::Value *loaded = cg_.builder_->CreateLoad(gvInst->getValueType(), gvInst, cleanName + ".val");
                            if (operand.dataType == SadTypeKind::String && loaded->getType()->isIntegerTy(64))
                            {
                                return cg_.builder_->CreateIntToPtr(loaded,
                                                                llvm::PointerType::getUnqual(*cg_.context_), cleanName + ".strptr");
                            }
                            return loaded;
                        }
                    }
                    return git->second;
                }
                llvm::GlobalVariable *gv = cg_.module_->getGlobalVariable(cleanName);
                if (!gv)
                {
                    gv = cg_.module_->getGlobalVariable(operand.name);
                }
                if (gv)
                {
                    // (AR) إذا كان مصفوفة [N x i8] (ثابت نصي) — GEP بدلاً من load
                    // (EN) If array [N x i8] (string constant) — GEP instead of load
                    if (gv->getValueType()->isArrayTy())
                    {
                        return cg_.builder_->CreateInBoundsGEP(
                            gv->getValueType(), gv,
                            {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0),
                             llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0)},
                            cleanName + ".ptr");
                    }
                    // (AR) إصلاح: المتغيرات النصية العمومية — inttoptr بعد التحميل (مسار module)
                    // (EN) Fix: Global string vars — inttoptr after load (module path)
                    {
                        llvm::Value *loaded = cg_.builder_->CreateLoad(gv->getValueType(), gv, cleanName + ".val");
                        if (operand.dataType == SadTypeKind::String && loaded->getType()->isIntegerTy(64))
                        {
                            return cg_.builder_->CreateIntToPtr(loaded,
                                                            llvm::PointerType::getUnqual(*cg_.context_), cleanName + ".strptr");
                        }
                        return loaded;
                    }
                }
                cg_.reportError("Undefined global: " + operand.name);
                return nullptr;
            }
            case SIROperandType::FUNCTION:
            {
                llvm::Function *fn = cg_.module_->getFunction(operand.name);
                if (fn)
                    return fn;
                cg_.reportError("Undefined function: " + operand.name);
                return nullptr;
            }
            case SIROperandType::LABEL:
            {
                return nullptr;
            }
            default:
                cg_.reportError("Unknown operand type");
                return nullptr;
            }
        }
        /**
         * إصدار تعليمة دمج نصوص
         * Emit string concatenation instruction
         * Concatenates two strings using snprintf (freestanding-safe)
         */
    } // namespace LLVM
} // namespace Sad