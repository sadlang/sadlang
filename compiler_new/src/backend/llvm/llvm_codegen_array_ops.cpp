/*
 * ============================================================================
 * LLVM IR Code Generator - Array Operations
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

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // ============================================================================
        // Phase N: Array Core / عمليات المصفوفات الأساسية
        // ============================================================================

        // Array layout: [i64 length, i64 capacity, i64* data]
        // Stored as: { i64, i64, ptr } struct

        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            static llvm::StructType *arrTy = nullptr;
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(ctx, {
                                                          llvm::Type::getInt64Ty(ctx),      // length
                                                          llvm::Type::getInt64Ty(ctx),      // capacity
                                                          llvm::PointerType::getUnqual(ctx) // data pointer
                                                      },
                                                 "SadArray");
            }
            return arrTy;
        }

        // ============================================================================
        // (AR) دالة مساعدة موحّدة: تطبيع مؤشر المصفوفة من أيّ تمثيل داخلي
        //      المصفوفات تُخزّن كـ ptrtoint'd pointer في متغيرات i64 (محلية أو عامة).
        //      هذه الدالة تكتشف التمثيل الفعلي وتُرجع مؤشر SadArray* صالح.
        //
        //      الحالات المدعومة:
        //      1. AllocaInst بنوع i64 → تحميل القيمة + inttoptr
        //      2. GlobalVariable بنوع i64 → تحميل القيمة + inttoptr
        //      3. قيمة i64 خام (من تسجيل) → inttoptr مباشرة
        //      4. مؤشر جاهز (ptr) → يُعاد كما هو
        //
        // (EN) Unified helper: normalize array pointer from any internal representation.
        //      Arrays are stored as ptrtoint'd pointers in i64 variables (local or global).
        //      This function detects the actual representation and returns a valid SadArray*.
        //
        //      Supported cases:
        //      1. AllocaInst with i64 type → load value + inttoptr
        //      2. GlobalVariable with i64 type → load value + inttoptr
        //      3. Raw i64 value (from register) → inttoptr directly
        //      4. Ready pointer (ptr) → returned as-is
        // ============================================================================
        llvm::Value *LLVMCodeGen::normalizeArrayPtr(llvm::Value *arrPtr, const char *label)
        {
            if (!arrPtr)
                return nullptr;

            // (AR) الحالة 1: alloca i64 — متغير محلي يحمل مؤشر مصفوفة محوّل بـ ptrtoint
            // (EN) Case 1: alloca i64 — local variable holding ptrtoint'd array pointer
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(arrPtr))
            {
                if (allocaInst->getAllocatedType()->isIntegerTy(64))
                {
                    std::string loadName = std::string(label) + ".ptr.int";
                    std::string ptrName = std::string(label) + ".ptr";
                    llvm::Value *ptrAsInt = builder_->CreateLoad(getInt64Type(), allocaInst, loadName);
                    arrPtr = builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*context_), ptrName);
                }
            }
            // (AR) الحالة 2: GlobalVariable بنوع i64 — متغير عام يحمل مؤشر مصفوفة محوّل
            //      بـ ptrtoint. ضروري لأن المتغيرات العامة تُخزّن كـ `internal global i64 0`
            // (EN) Case 2: GlobalVariable with i64 type — global variable holding ptrtoint'd
            //      array pointer. Required because globals are stored as `internal global i64 0`
            else if (auto *gvInst = llvm::dyn_cast<llvm::GlobalVariable>(arrPtr))
            {
                if (gvInst->getValueType()->isIntegerTy(64))
                {
                    std::string loadName = std::string(label) + ".glob.int";
                    std::string ptrName = std::string(label) + ".glob.ptr";
                    llvm::Value *ptrAsInt = builder_->CreateLoad(getInt64Type(), gvInst, loadName);
                    arrPtr = builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*context_), ptrName);
                }
            }
            // (AR) الحالة 3: قيمة i64 خام — نتيجة من تسجيل SIR أو عملية سابقة
            // (EN) Case 3: raw i64 value — result from SIR register or previous operation
            else if (arrPtr->getType()->isIntegerTy(64))
            {
                std::string ptrName = std::string(label) + ".ptr.raw";
                arrPtr = builder_->CreateIntToPtr(arrPtr, llvm::PointerType::getUnqual(*context_), ptrName);
            }
            // (AR) الحالة 4: مؤشر جاهز — لا حاجة لتحويل
            // (EN) Case 4: already a pointer — no conversion needed

            return arrPtr;
        }

        // ============================================================================
        // (AR) دالة مساعدة: تحويل فهرس سالب إلى موجب بإضافة طول المصفوفة
        //      مثل Python: م[-1] = م[طول-1] = آخر عنصر
        //      م[-2] = م[طول-2] = ما قبل الأخير
        //      تتطلب مؤشر مصفوفة مطبّع (بعد normalizeArrayPtr)
        //
        //      التنفيذ: if (index < 0) index = len + index
        //      يُولِّد كتلة شرطية في LLVM IR مع PHI لاختيار القيمة
        //
        // (EN) Helper: convert negative index to positive by adding array length
        //      Python-like: м[-1] = м[len-1] = last element
        //      Requires normalized array pointer (after normalizeArrayPtr)
        //      Implementation: if (index < 0) index = len + index
        // ============================================================================
        llvm::Value *LLVMCodeGen::normalizeArrayIndex(llvm::Value *index, llvm::Value *arrPtr, const char *label)
        {
            if (!index || !arrPtr)
                return index;

            auto i64Ty = getInt64Type();

            // (AR) التحقق: هل الفهرس سالب؟
            // (EN) Check: is index negative?
            std::string negName = std::string(label) + ".is.neg";
            llvm::Value *isNeg = builder_->CreateICmpSLT(index, llvm::ConstantInt::get(i64Ty, 0), negName);

            // (AR) تحميل طول المصفوفة
            // (EN) Load array length
            llvm::StructType *arrTy = getArrayStructType(*context_);
            std::string lenGepName = std::string(label) + ".len.gep";
            std::string lenName = std::string(label) + ".len";
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, lenGepName);
            llvm::Value *len = builder_->CreateLoad(i64Ty, lenGep, lenName);

            // (AR) الفهرس الموجب: len + index (عندما index سالب، مثلاً: 5 + (-1) = 4)
            // (EN) Positive index: len + index (when index is negative, e.g.: 5 + (-1) = 4)
            std::string posName = std::string(label) + ".pos";
            llvm::Value *posIdx = builder_->CreateAdd(len, index, posName);

            // (AR) اختيار: إذا سالب → الفهرس المحوّل، وإلا → الفهرس الأصلي
            // (EN) Select: if negative → converted index, else → original index
            std::string resolvedName = std::string(label) + ".resolved";
            return builder_->CreateSelect(isNeg, posIdx, index, resolvedName);
        }

        // ====================================================================
        // (AR) فحص حدود المصفوفة: يتحقق أن 0 <= index < len
        //      إذا كان الفهرس خارج النطاق → يطبع رسالة خطأ واضحة ثم exit(1)
        //      يجب استدعاؤها بعد normalizeArrayIndex (الفهرس أصبح موجباً)
        // (EN) Array bounds check: verifies 0 <= index < len
        //      If out of bounds → prints clear error message then exit(1)
        //      Must be called after normalizeArrayIndex (index is already positive)
        // ====================================================================
        void LLVMCodeGen::emitBoundsCheck(llvm::Value *index, llvm::Value *arrPtr,
                                          const char *label)
        {
            auto i64Ty = getInt64Type();
            llvm::StructType *arrTy = getArrayStructType(*context_);

            // (AR) تحميل طول المصفوفة
            // (EN) Load array length
            std::string lenGepName = std::string(label) + ".bc.len.gep";
            std::string lenName = std::string(label) + ".bc.len";
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, lenGepName);
            llvm::Value *len = builder_->CreateLoad(i64Ty, lenGep, lenName);

            // (AR) التحقق: index < 0 أو index >= len
            // (EN) Check: index < 0 or index >= len
            std::string negName = std::string(label) + ".bc.neg";
            std::string ovfName = std::string(label) + ".bc.ovf";
            std::string oobName = std::string(label) + ".bc.oob";
            llvm::Value *isNeg = builder_->CreateICmpSLT(index, llvm::ConstantInt::get(i64Ty, 0), negName);
            llvm::Value *isOverflow = builder_->CreateICmpSGE(index, len, ovfName);
            llvm::Value *isOOB = builder_->CreateOr(isNeg, isOverflow, oobName);

            // (AR) إنشاء الكتل: فشل + استمرار
            // (EN) Create blocks: fail + continue
            llvm::Function *curFunc = builder_->GetInsertBlock()->getParent();
            std::string failBBName = std::string(label) + ".bc.fail";
            std::string contBBName = std::string(label) + ".bc.ok";
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*context_, failBBName, curFunc);
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*context_, contBBName, curFunc);

            builder_->CreateCondBr(isOOB, failBB, contBB);

            // (AR) كتلة الفشل: طباعة رسالة خطأ ثم exit(1)
            // (EN) Fail block: print error message then exit(1)
            builder_->SetInsertPoint(failBB);

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto *printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_), {ptrTy}, true);
            auto printfFunc = module_->getOrInsertFunction("printf", printfType);

            // (AR) رسالة: "خطأ: فهرس %lld خارج نطاق المصفوفة (الطول: %lld)\n"
            // (EN) Message: "Error: index %lld out of array bounds (length: %lld)\n"
            llvm::Value *fmtStr = builder_->CreateGlobalStringPtr(
                "Error: array index %lld out of bounds (length: %lld)\n", "bc.fmt");
            builder_->CreateCall(printfFunc, {fmtStr, index, len});

            auto *exitType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context_), {llvm::Type::getInt32Ty(*context_)}, false);
            auto exitFunc = module_->getOrInsertFunction("exit", exitType);
            builder_->CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1)});
            builder_->CreateUnreachable();

            // (AR) كتلة الاستمرار: الفهرس ضمن النطاق
            // (EN) Continue block: index is within bounds
            builder_->SetInsertPoint(contBB);
        }

        llvm::Value *LLVMCodeGen::emitArrayNew(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // Optional operand: initial capacity
            int64_t capacity = 8; // default
            if (!inst->operands.empty())
            {
                if (inst->operands[0].type == SIROperandType::CONSTANT)
                {
                    capacity = inst->operands[0].intValue;
                    if (capacity <= 0)
                        capacity = 8;
                }
            }

            // Allocate array struct on heap
            llvm::StructType *arrTy = getArrayStructType(*context_);
            auto *dlSize = llvm::ConstantExpr::getSizeOf(arrTy);
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *arrPtr = builder_->CreateCall(mallocFunc, {dlSize}, "arr_new");

            // Set length (0 by default, or initial length from operand[1])
            int64_t initialLength = 0;
            if (inst->operands.size() >= 2 && inst->operands[1].type == SIROperandType::CONSTANT)
            {
                initialLength = inst->operands[1].intValue;
            }
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
            builder_->CreateStore(llvm::ConstantInt::get(getInt64Type(), initialLength), lenGep);

            // Set capacity
            llvm::Value *capGep = builder_->CreateStructGEP(arrTy, arrPtr, 1, "arr.cap.gep");
            builder_->CreateStore(llvm::ConstantInt::get(getInt64Type(), capacity), capGep);

            // Allocate data buffer: capacity * sizeof(ptr) bytes (pointer-width elements for nested array support)
            // (AR) تخصيص مخزن البيانات: السعة × حجم المؤشر (لدعم المصفوفات المتداخلة)
            auto *ptrSize = llvm::ConstantExpr::getSizeOf(llvm::PointerType::getUnqual(*context_));
            llvm::Value *dataSize = builder_->CreateMul(
                llvm::ConstantInt::get(getInt64Type(), capacity),
                builder_->CreateIntCast(ptrSize, getInt64Type(), false), "arr.data.size");
            llvm::Value *dataPtr = builder_->CreateCall(mallocFunc, {dataSize}, "arr.data");
            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            builder_->CreateStore(dataPtr, dataGep);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }

        llvm::Value *LLVMCodeGen::emitArrayGet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("ARRAY_GET requires 2 operands (array, index)");
                return nullptr;
            }

            // (AR) إصلاح: استخدام find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Fix: use find() instead of [] to avoid inserting nullptr into the map
            llvm::Value *arrPtr = nullptr;
            auto arrIt = context_info_.namedValues.find(inst->operands[0].name);
            if (arrIt != context_info_.namedValues.end() && arrIt->second != nullptr)
            {
                arrPtr = arrIt->second;
            }
            if (!arrPtr)
                arrPtr = resolveOperand(inst->operands[0]);
            llvm::Value *index = resolveOperand(inst->operands[1]);
            if (!arrPtr || !index)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = normalizeArrayPtr(arrPtr, "arr");

            // (AR) تطبيع الفهرس: تحويل السالب إلى موجب (مثل م[-1] = آخر عنصر)
            // (EN) Normalize index: convert negative to positive (e.g. м[-1] = last element)
            index = normalizeArrayIndex(index, arrPtr, "get");

            // (AR) فحص حدود المصفوفة: exit(1) إذا كان الفهرس خارج النطاق
            // (EN) Bounds check: exit(1) if index is out of range
            emitBoundsCheck(index, arrPtr, "get");

            llvm::StructType *arrTy = getArrayStructType(*context_);

            // (AR) تحميل مؤشر البيانات من الحقل الثالث في بنية المصفوفة
            // (EN) Load data pointer from struct field 2
            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            llvm::Value *dataPtr = builder_->CreateLoad(
                llvm::PointerType::getUnqual(*context_), dataGep, "arr.data");

            // (AR) تحديد نوع العنصر: إذا كانت النتيجة مصفوفة/مؤشر → حمِّل كـ ptr (مصفوفات متداخلة)
            //      وإلا حمِّل كـ i64 (عدد صحيح / عشري مُحوَّل)
            // (EN) Determine element type: if result is ARRAY/PTR → load as ptr (for nested arrays)
            //      otherwise load as i64 (integer / bitcasted float)
            bool isNestedArray = false;
            if (inst->result.has_value())
            {
                auto resultType = inst->result->dataType;
                if (resultType == SadTypeKind::Array || resultType == SadTypeKind::Pointer ||
                    resultType == SadTypeKind::Struct || resultType == SadTypeKind::String)
                {
                    isNestedArray = true;
                }
            }

            llvm::Value *result;
            if (isNestedArray)
            {
                // (AR) العنصر مؤشر (مصفوفة متداخلة / نص / بنية)
                // (EN) Element is a pointer (nested array / string / struct)
                llvm::Value *elemPtr = builder_->CreateGEP(
                    llvm::PointerType::getUnqual(*context_), dataPtr, {index}, "arr.elem.ptr");
                result = builder_->CreateLoad(
                    llvm::PointerType::getUnqual(*context_), elemPtr, "arr.get.ptr");
            }
            else
            {
                // (AR) العنصر i64 (رقم / منطقي)
                // (EN) Element is i64 (number / boolean)
                llvm::Value *elemPtr = builder_->CreateGEP(getInt64Type(), dataPtr, {index}, "arr.elem");
                result = builder_->CreateLoad(getInt64Type(), elemPtr, "arr.get");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitArraySet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                reportError("ARRAY_SET requires 3 operands (array, index, value)");
                return nullptr;
            }

            // (AR) إصلاح: استخدام find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Fix: use find() instead of [] to avoid inserting nullptr into the map
            llvm::Value *arrPtr = nullptr;
            auto arrIt = context_info_.namedValues.find(inst->operands[0].name);
            if (arrIt != context_info_.namedValues.end() && arrIt->second != nullptr)
            {
                arrPtr = arrIt->second;
            }
            if (!arrPtr)
                arrPtr = resolveOperand(inst->operands[0]);
            llvm::Value *index = resolveOperand(inst->operands[1]);
            llvm::Value *value = resolveOperand(inst->operands[2]);
            if (!arrPtr || !index || !value)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = normalizeArrayPtr(arrPtr, "arr");

            // (AR) تطبيع الفهرس: تحويل السالب إلى موجب (مثل م[-1] = آخر عنصر)
            // (EN) Normalize index: convert negative to positive (e.g. arr[-1] = last element)
            index = normalizeArrayIndex(index, arrPtr, "set");

            // (AR) فحص حدود المصفوفة: exit(1) إذا كان الفهرس خارج النطاق
            // (EN) Bounds check: exit(1) if index is out of range
            emitBoundsCheck(index, arrPtr, "set");

            llvm::StructType *arrTy = getArrayStructType(*context_);

            // Load data pointer
            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            llvm::Value *dataPtr = builder_->CreateLoad(
                llvm::PointerType::getUnqual(*context_), dataGep, "arr.data");

            // (AR) تحديد نوع العنصر: إذا كانت القيمة مؤشراً (مصفوفة/نص/بنية) → خزِّن كمؤشر
            // (EN) Determine element type: if value is pointer type → store as pointer
            bool isPointerValue = value->getType()->isPointerTy();

            if (isPointerValue)
            {
                // (AR) تخزين مؤشر (مصفوفة متداخلة / نص / بنية)
                // (EN) Store pointer (nested array / string / struct)
                llvm::Value *elemPtr = builder_->CreateGEP(
                    llvm::PointerType::getUnqual(*context_), dataPtr, {index}, "arr.elem.ptr");
                builder_->CreateStore(value, elemPtr);
            }
            else
            {
                // (AR) تخزين قيمة i64
                // (EN) Store i64 value
                // (AR) تحويل القيمة إلى i64 إذا لزم الأمر
                //      إذا كانت double → bitcast إلى i64 (حفظ التمثيل الثنائي)
                //      إذا كانت عدد صحيح بحجم آخر → IntCast
                // (EN) Convert value to i64 if needed
                //      If double → bitcast to i64 (preserve binary representation)
                //      If integer of different size → IntCast
                if (!value->getType()->isIntegerTy(64))
                {
                    if (value->getType()->isDoubleTy() || value->getType()->isFloatTy())
                    {
                        // (AR) تحويل عشري إلى i64 عبر bitcast لحفظ البتات كما هي
                        // (EN) Convert float/double to i64 via bitcast to preserve bits
                        if (value->getType()->isFloatTy())
                        {
                            value = builder_->CreateFPExt(value, llvm::Type::getDoubleTy(*context_), "arr.elem.f2d");
                        }
                        value = builder_->CreateBitCast(value, getInt64Type(), "arr.elem.dcast");
                    }
                    else
                    {
                        value = builder_->CreateIntCast(value, getInt64Type(), true, "arr.elem.cast");
                    }
                }
                llvm::Value *elemPtr = builder_->CreateGEP(getInt64Type(), dataPtr, {index}, "arr.elem");
                builder_->CreateStore(value, elemPtr);
            }

            return value;
        }

        llvm::Value *LLVMCodeGen::emitArrayLen(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("ARRAY_LEN requires 1 operand (array)");
                return nullptr;
            }

            // (AR) إصلاح: استخدام find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Fix: use find() instead of [] to avoid inserting nullptr into the map
            llvm::Value *arrPtr = nullptr;
            auto arrIt = context_info_.namedValues.find(inst->operands[0].name);
            if (arrIt != context_info_.namedValues.end() && arrIt->second != nullptr)
            {
                arrPtr = arrIt->second;
            }
            if (!arrPtr)
                arrPtr = resolveOperand(inst->operands[0]);
            if (!arrPtr)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = normalizeArrayPtr(arrPtr, "arr");

            llvm::StructType *arrTy = getArrayStructType(*context_);
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
            llvm::Value *result = builder_->CreateLoad(getInt64Type(), lenGep, "arr.len");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // دمج مصفوفتين (ARRAY_CONCAT) — array1 + array2
        // Concatenate two arrays — array1 + array2
        // ============================================================================
        llvm::Value *LLVMCodeGen::emitArrayConcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("ARRAY_CONCAT requires 2 operands (array1, array2)");
                return nullptr;
            }

            // (AR) الحصول على المعاملين (المصفوفتين)
            // (EN) Get both operands (the two arrays)
            llvm::Value *arr1 = resolveOperand(inst->operands[0]);
            llvm::Value *arr2 = resolveOperand(inst->operands[1]);

            if (!arr1 || !arr2)
            {
                reportError("ARRAY_CONCAT: could not resolve operands");
                return nullptr;
            }

            // (AR) تطبيع المؤشرات — إذا كانت i64 حوّلها إلى ptr
            // (EN) Normalize pointers — if i64 convert to ptr
            auto *ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = getInt64Type();

            if (arr1->getType()->isIntegerTy(64))
                arr1 = builder_->CreateIntToPtr(arr1, ptrTy, "concat.arr1.ptr");
            if (arr2->getType()->isIntegerTy(64))
                arr2 = builder_->CreateIntToPtr(arr2, ptrTy, "concat.arr2.ptr");

            // (AR) تحميل الأطوال والبيانات من المصفوفتين
            // (EN) Load lengths and data pointers from both arrays
            llvm::StructType *arrTy = getArrayStructType(*context_);

            llvm::Value *len1Gep = builder_->CreateStructGEP(arrTy, arr1, 0, "concat.len1.gep");
            llvm::Value *len1 = builder_->CreateLoad(i64Ty, len1Gep, "concat.len1");
            llvm::Value *data1Gep = builder_->CreateStructGEP(arrTy, arr1, 2, "concat.data1.gep");
            llvm::Value *data1 = builder_->CreateLoad(ptrTy, data1Gep, "concat.data1");

            llvm::Value *len2Gep = builder_->CreateStructGEP(arrTy, arr2, 0, "concat.len2.gep");
            llvm::Value *len2 = builder_->CreateLoad(i64Ty, len2Gep, "concat.len2");
            llvm::Value *data2Gep = builder_->CreateStructGEP(arrTy, arr2, 2, "concat.data2.gep");
            llvm::Value *data2 = builder_->CreateLoad(ptrTy, data2Gep, "concat.data2");

            // (AR) حساب الطول الكلي
            // (EN) Calculate total length
            llvm::Value *totalLen = builder_->CreateAdd(len1, len2, "concat.total");

            // (AR) إنشاء مصفوفة جديدة بالطول الكلي
            // (EN) Allocate new array with total length
            auto *dlSize = llvm::ConstantExpr::getSizeOf(arrTy);
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *newArr = builder_->CreateCall(mallocFn, {dlSize}, "concat.arr");

            // (AR) تعيين الطول والسعة
            // (EN) Set length and capacity
            llvm::Value *newLenGep = builder_->CreateStructGEP(arrTy, newArr, 0, "concat.new.len.gep");
            builder_->CreateStore(totalLen, newLenGep);
            llvm::Value *newCapGep = builder_->CreateStructGEP(arrTy, newArr, 1, "concat.new.cap.gep");
            builder_->CreateStore(totalLen, newCapGep);

            // (AR) تخصيص مخزن البيانات: الطول × حجم المؤشر
            // (EN) Allocate data buffer: total * sizeof(ptr)
            auto *ptrSize = llvm::ConstantExpr::getSizeOf(ptrTy);
            llvm::Value *ptrSize64 = builder_->CreateIntCast(ptrSize, i64Ty, false);
            llvm::Value *dataSize = builder_->CreateMul(totalLen, ptrSize64, "concat.data.size");
            llvm::Value *newData = builder_->CreateCall(mallocFn, {dataSize}, "concat.data");
            llvm::Value *newDataGep = builder_->CreateStructGEP(arrTy, newArr, 2, "concat.new.data.gep");
            builder_->CreateStore(newData, newDataGep);

            // (AR) نسخ عناصر المصفوفة الأولى
            // (EN) Copy elements from first array
            llvm::Value *bytes1 = builder_->CreateMul(len1, ptrSize64, "concat.bytes1");
            builder_->CreateMemCpy(newData, llvm::MaybeAlign(8), data1, llvm::MaybeAlign(8), bytes1);

            // (AR) نسخ عناصر المصفوفة الثانية بعد الأولى
            // (EN) Copy elements from second array after the first
            llvm::Value *dst2 = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), newData, {bytes1}, "concat.dst2");
            llvm::Value *bytes2 = builder_->CreateMul(len2, ptrSize64, "concat.bytes2");
            builder_->CreateMemCpy(dst2, llvm::MaybeAlign(8), data2, llvm::MaybeAlign(8), bytes2);

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = newArr;
            }
            return newArr;
        }

        // ============================================================================
        // Phase N: String Core / عمليات النصوص الأساسية
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitStringNew(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            llvm::Value *result;
            if (!inst->operands.empty())
            {
                // Create from existing string/constant
                result = resolveOperand(inst->operands[0]);
            }
            else
            {
                // Create empty string
                result = builder_->CreateGlobalStringPtr("", "empty_str");
            }

            if (inst->result.has_value() && result)
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


    } // namespace LLVM
} // namespace Sad
