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
#include "builders/collections/array_ops_codegen.h" // (Phase 7 Step 5)
#include "llvm_codegen.h"

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
        llvm::Value *ArrayOpsCodeGen::normalizeArrayPtr(llvm::Value *arrPtr, const char *label)
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
                    llvm::Value *ptrAsInt = cg_.builder_->CreateLoad(cg_.getInt64Type(), allocaInst, loadName);
                    arrPtr = cg_.builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*cg_.context_), ptrName);
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
                    llvm::Value *ptrAsInt = cg_.builder_->CreateLoad(cg_.getInt64Type(), gvInst, loadName);
                    arrPtr = cg_.builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*cg_.context_), ptrName);
                }
            }
            // (AR) الحالة 3: قيمة i64 خام — نتيجة من تسجيل SIR أو عملية سابقة
            // (EN) Case 3: raw i64 value — result from SIR register or previous operation
            else if (arrPtr->getType()->isIntegerTy(64))
            {
                std::string ptrName = std::string(label) + ".ptr.raw";
                arrPtr = cg_.builder_->CreateIntToPtr(arrPtr, llvm::PointerType::getUnqual(*cg_.context_), ptrName);
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
        llvm::Value *ArrayOpsCodeGen::normalizeArrayIndex(llvm::Value *index, llvm::Value *arrPtr, const char *label)
        {
            if (!index || !arrPtr)
                return index;

            auto i64Ty = cg_.getInt64Type();

            // (AR) التحقق: هل الفهرس سالب؟
            // (EN) Check: is index negative?
            std::string negName = std::string(label) + ".is.neg";
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(index, llvm::ConstantInt::get(i64Ty, 0), negName);

            // (AR) تحميل طول المصفوفة
            // (EN) Load array length
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);
            std::string lenGepName = std::string(label) + ".len.gep";
            std::string lenName = std::string(label) + ".len";
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, lenGepName);
            llvm::Value *len = cg_.builder_->CreateLoad(i64Ty, lenGep, lenName);

            // (AR) الفهرس الموجب: len + index (عندما index سالب، مثلاً: 5 + (-1) = 4)
            // (EN) Positive index: len + index (when index is negative, e.g.: 5 + (-1) = 4)
            std::string posName = std::string(label) + ".pos";
            llvm::Value *posIdx = cg_.builder_->CreateAdd(len, index, posName);

            // (AR) اختيار: إذا سالب → الفهرس المحوّل، وإلا → الفهرس الأصلي
            // (EN) Select: if negative → converted index, else → original index
            std::string resolvedName = std::string(label) + ".resolved";
            return cg_.builder_->CreateSelect(isNeg, posIdx, index, resolvedName);
        }

        // ====================================================================
        // (AR) فحص حدود المصفوفة: يتحقق أن 0 <= index < len
        //      إذا كان الفهرس خارج النطاق → يطبع رسالة خطأ واضحة ثم exit(1)
        //      يجب استدعاؤها بعد normalizeArrayIndex (الفهرس أصبح موجباً)
        // (EN) Array bounds check: verifies 0 <= index < len
        //      If out of bounds → prints clear error message then exit(1)
        //      Must be called after normalizeArrayIndex (index is already positive)
        // ====================================================================
        void ArrayOpsCodeGen::emitBoundsCheck(llvm::Value *index, llvm::Value *arrPtr,
                                          const char *label)
        {
            auto i64Ty = cg_.getInt64Type();
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // (AR) تحميل طول المصفوفة
            // (EN) Load array length
            std::string lenGepName = std::string(label) + ".bc.len.gep";
            std::string lenName = std::string(label) + ".bc.len";
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, lenGepName);
            llvm::Value *len = cg_.builder_->CreateLoad(i64Ty, lenGep, lenName);

            // (AR) التحقق: index < 0 أو index >= len
            // (EN) Check: index < 0 or index >= len
            std::string negName = std::string(label) + ".bc.neg";
            std::string ovfName = std::string(label) + ".bc.ovf";
            std::string oobName = std::string(label) + ".bc.oob";
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(index, llvm::ConstantInt::get(i64Ty, 0), negName);
            llvm::Value *isOverflow = cg_.builder_->CreateICmpSGE(index, len, ovfName);
            llvm::Value *isOOB = cg_.builder_->CreateOr(isNeg, isOverflow, oobName);

            // (AR) إنشاء الكتل: فشل + استمرار
            // (EN) Create blocks: fail + continue
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            std::string failBBName = std::string(label) + ".bc.fail";
            std::string contBBName = std::string(label) + ".bc.ok";
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, failBBName, curFunc);
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, contBBName, curFunc);

            cg_.builder_->CreateCondBr(isOOB, failBB, contBB);

            // (AR) كتلة الفشل: طباعة رسالة خطأ ثم exit(1)
            // (EN) Fail block: print error message then exit(1)
            cg_.builder_->SetInsertPoint(failBB);

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_), {ptrTy}, true);
            auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);

            // (AR) رسالة: "خطأ: فهرس %lld خارج نطاق المصفوفة (الطول: %lld)\n"
            // (EN) Message: "Error: index %lld out of array bounds (length: %lld)\n"
            llvm::Value *fmtStr = cg_.builder_->CreateGlobalStringPtr(
                "Error: array index %lld out of bounds (length: %lld)\n", "bc.fmt");
            cg_.builder_->CreateCall(printfFunc, {fmtStr, index, len});

            auto *exitType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt32Ty(*cg_.context_)}, false);
            auto exitFunc = cg_.module_->getOrInsertFunction("exit", exitType);
            cg_.builder_->CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 1)});
            cg_.builder_->CreateUnreachable();

            // (AR) كتلة الاستمرار: الفهرس ضمن النطاق
            // (EN) Continue block: index is within bounds
            cg_.builder_->SetInsertPoint(contBB);
        }

        llvm::Value *ArrayOpsCodeGen::emitArrayNew(std::shared_ptr<SIRInstruction> inst)
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
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);
            auto *dlSize = llvm::ConstantExpr::getSizeOf(arrTy);
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_), {cg_.getInt64Type()}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *arrPtr = cg_.builder_->CreateCall(mallocFunc, {dlSize}, "arr_new");

            // Set length (0 by default, or initial length from operand[1])
            int64_t initialLength = 0;
            if (inst->operands.size() >= 2 && inst->operands[1].type == SIROperandType::CONSTANT)
            {
                initialLength = inst->operands[1].intValue;
            }
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt64Type(), initialLength), lenGep);

            // Set capacity
            llvm::Value *capGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "arr.cap.gep");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt64Type(), capacity), capGep);

            // Allocate data buffer: capacity * sizeof(ptr) bytes (pointer-width elements for nested array support)
            // (AR) تخصيص مخزن البيانات: السعة × حجم المؤشر (لدعم المصفوفات المتداخلة)
            auto *ptrSize = llvm::ConstantExpr::getSizeOf(llvm::PointerType::getUnqual(*cg_.context_));
            llvm::Value *dataSize = cg_.builder_->CreateMul(
                llvm::ConstantInt::get(cg_.getInt64Type(), capacity),
                cg_.builder_->CreateIntCast(ptrSize, cg_.getInt64Type(), false), "arr.data.size");
            llvm::Value *dataPtr = cg_.builder_->CreateCall(mallocFunc, {dataSize}, "arr.data");
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            cg_.builder_->CreateStore(dataPtr, dataGep);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }

        llvm::Value *ArrayOpsCodeGen::emitArrayGet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_GET"}});
                return nullptr;
            }

            // (AR) إصلاح: استخدام find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Fix: use find() instead of [] to avoid inserting nullptr into the map
            llvm::Value *arrPtr = nullptr;
            auto arrIt = cg_.context_info_.namedValues.find(inst->operands[0].name);
            if (arrIt != cg_.context_info_.namedValues.end() && arrIt->second != nullptr)
            {
                arrPtr = arrIt->second;
            }
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *index = cg_.resolveOperand(inst->operands[1]);
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

            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // (AR) تحميل مؤشر البيانات من الحقل الثالث في بنية المصفوفة
            // (EN) Load data pointer from struct field 2
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(
                llvm::PointerType::getUnqual(*cg_.context_), dataGep, "arr.data");

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
            // (AR) فحص هل المصدر صف (tuple) لفك وسم MSB بعد التحميل
            // (EN) Check if source is tuple for MSB untagging after load
            bool isTupleSource = (inst->operands.size() > 0 && inst->operands[0].dataType == SadTypeKind::Tuple);

            if (isNestedArray)
            {
                // (AR) العنصر مؤشر (مصفوفة متداخلة / نص / بنية)
                // (EN) Element is a pointer (nested array / string / struct)
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(
                    llvm::PointerType::getUnqual(*cg_.context_), dataPtr, {index}, "arr.elem.ptr");
                result = cg_.builder_->CreateLoad(
                    llvm::PointerType::getUnqual(*cg_.context_), elemPtr, "arr.get.ptr");
            }
            else
            {
                // (AR) العنصر i64 (رقم / منطقي)
                // (EN) Element is i64 (number / boolean)
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(cg_.getInt64Type(), dataPtr, {index}, "arr.elem");
                result = cg_.builder_->CreateLoad(cg_.getInt64Type(), elemPtr, "arr.get");

                // (AR) وسم MSB لعناصر الصفوف:
                //      نحفظ __is_ptr flag بناءً على bit 63:
                //      bit63=1 → رقم (isPtr=false)، bit63=0 → مؤشر (isPtr=true)
                //      لا نفك MSB هنا — ensureString يفكه عند الحاجة
                // (EN) MSB tagging for tuple elements:
                //      Save __is_ptr flag based on bit 63
                //      Don't untag here — ensureString handles it
                if (isTupleSource)
                {
                    // (AR) وسم 2-bit: bit63=1 → ليس مؤشر، bit63=0 → مؤشر
                    llvm::Value *msbMask = llvm::ConstantInt::get(cg_.getInt64Type(), 1ULL << 63);
                    llvm::Value *msbBit = cg_.builder_->CreateAnd(result, msbMask, "tup.get.msb");
                    llvm::Value *isInt = cg_.builder_->CreateICmpNE(msbBit, llvm::ConstantInt::get(cg_.getInt64Type(), 0), "tup.get.isint");
                    llvm::Value *isPtr = cg_.builder_->CreateNot(isInt, "tup.get.isptr");

                    if (inst->result.has_value())
                    {
                        std::string flagName = inst->result->name + ".__is_ptr";
                        cg_.context_info_.namedValues[flagName] = isPtr;
                    }
                    // (AR) [ISSUE-052] لا يُفكّ وسم العنصر العدديّ هنا: المخطّط يحجز bit63 للنوع،
                    //      وهو يصطدم ببت إشارة العدد السالب (لا يمكن التمييز قراءةً بين «موجب موسوم»
                    //      و«سالب خام» — كلاهما bit63=1). فكُّ bit63 يُصلح الموجب لكنه يفسد السالب.
                    //      الإصلاح الصحيح يتطلّب إعادة تصميم الوسم (نوع + قيمة 62-بتًّا بمدّ إشارة)
                    //      عبر الكتابة والقراءة معًا — معماريّ مؤجَّل (انظر DISCOVERED_ISSUES 052/045).
                    // (EN) [ISSUE-052] Do NOT untag integer elements here: the scheme reuses bit63
                    //      for the type tag, colliding with the sign bit of negative integers (a
                    //      "tagged positive" and a "raw negative" are read-indistinguishable — both
                    //      have bit63=1). Clearing bit63 fixes positives but corrupts negatives. A
                    //      correct fix needs a tagging redesign (type + 62-bit sign-extended value)
                    //      across write & read — deferred as architectural (see DISCOVERED_ISSUES).
                }
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArrayOpsCodeGen::emitArraySet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_SET"}});
                return nullptr;
            }

            // (AR) إصلاح: استخدام find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Fix: use find() instead of [] to avoid inserting nullptr into the map
            llvm::Value *arrPtr = nullptr;
            auto arrIt = cg_.context_info_.namedValues.find(inst->operands[0].name);
            if (arrIt != cg_.context_info_.namedValues.end() && arrIt->second != nullptr)
            {
                arrPtr = arrIt->second;
            }
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *index = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[2]);
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

            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // Load data pointer
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(
                llvm::PointerType::getUnqual(*cg_.context_), dataGep, "arr.data");

            // (AR) تحديد نوع العنصر: إذا كانت القيمة مؤشراً (مصفوفة/نص/بنية) → خزِّن كمؤشر
            // (EN) Determine element type: if value is pointer type → store as pointer
            bool isPointerValue = value->getType()->isPointerTy();

            // (AR) وسم MSB للصفوف: عناصر الصف مختلطة الأنواع (نص/رقم/منطقي)
            //      نخزن كل شيء كـ i64:
            //      - ptr (نص/مصفوفة) → ptrtoint → i64 (bit 63 = 0 دائمًا في userspace x64)
            //      - i64 (رقم/منطقي) → val | (1<<63) → i64 مع وسم MSB
            //      عند القراءة: bit63=1 → رقم (أزل MSB)، bit63=0 → مؤشر (inttoptr)
            // (EN) MSB tagging for tuples: mixed-type elements stored uniformly as i64
            bool isTupleContainer = (inst->operands.size() > 0 && inst->operands[0].dataType == SadTypeKind::Tuple);

            if (isTupleContainer)
            {
                llvm::Value *i64Val = nullptr;
                if (isPointerValue)
                {
                    // (AR) ptr → ptrtoint → i64 (bit 63 = 0 في userspace)
                    i64Val = cg_.builder_->CreatePtrToInt(value, cg_.getInt64Type(), "tup.p2i");
                }
                else
                {
                    // (AR) وسم 2-bit للصفوف:
                    //      bit 63 = 1: ليس مؤشر
                    //      bit 62 = 0: رقم عادي
                    //      bit 62 = 1: boolean
                    //      bit 63 = 0: مؤشر (نص/مصفوفة)
                    // (EN) 2-bit tagging for tuples:
                    //      bit 63 = 1: not a pointer
                    //      bit 62 = 0: integer
                    //      bit 62 = 1: boolean
                    //      bit 63 = 0: pointer (string/array)
                    bool isBoolVal = value->getType()->isIntegerTy(1) ||
                                     (inst->operands.size() > 2 && inst->operands[2].dataType == SadTypeKind::Boolean);
                    i64Val = value;
                    if (!i64Val->getType()->isIntegerTy(64))
                    {
                        if (i64Val->getType()->isDoubleTy() || i64Val->getType()->isFloatTy())
                        {
                            if (i64Val->getType()->isFloatTy())
                                i64Val = cg_.builder_->CreateFPExt(i64Val, llvm::Type::getDoubleTy(*cg_.context_), "tup.f2d");
                            i64Val = cg_.builder_->CreateBitCast(i64Val, cg_.getInt64Type(), "tup.dcast");
                        }
                        else
                        {
                            i64Val = cg_.builder_->CreateZExt(i64Val, cg_.getInt64Type(), "tup.zext");
                        }
                    }
                    // (AR) وسم: bit63 للأرقام/منطقي + bit62 إضافي للمنطقي
                    uint64_t tag = isBoolVal ? (3ULL << 62) : (1ULL << 63); // 0xC0... أو 0x80...
                    llvm::Value *tagVal = llvm::ConstantInt::get(cg_.getInt64Type(), tag);
                    i64Val = cg_.builder_->CreateOr(i64Val, tagVal, "tup.tag");
                }
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(cg_.getInt64Type(), dataPtr, {index}, "tup.elem");
                cg_.builder_->CreateStore(i64Val, elemPtr);
            }
            else if (isPointerValue)
            {
                // (AR) تخزين مؤشر (مصفوفة متداخلة / نص / بنية)
                // (EN) Store pointer (nested array / string / struct)
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(
                    llvm::PointerType::getUnqual(*cg_.context_), dataPtr, {index}, "arr.elem.ptr");
                cg_.builder_->CreateStore(value, elemPtr);
            }
            else
            {
                // (AR) تخزين قيمة i64
                // (EN) Store i64 value
                if (!value->getType()->isIntegerTy(64))
                {
                    if (value->getType()->isDoubleTy() || value->getType()->isFloatTy())
                    {
                        if (value->getType()->isFloatTy())
                        {
                            value = cg_.builder_->CreateFPExt(value, llvm::Type::getDoubleTy(*cg_.context_), "arr.elem.f2d");
                        }
                        value = cg_.builder_->CreateBitCast(value, cg_.getInt64Type(), "arr.elem.dcast");
                    }
                    else
                    {
                        value = cg_.builder_->CreateIntCast(value, cg_.getInt64Type(), true, "arr.elem.cast");
                    }
                }
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(cg_.getInt64Type(), dataPtr, {index}, "arr.elem");
                cg_.builder_->CreateStore(value, elemPtr);
            }

            return value;
        }

        llvm::Value *ArrayOpsCodeGen::emitArrayLen(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_LEN"}});
                return nullptr;
            }

            // (AR) إصلاح: استخدام find() بدلاً من [] لتجنب إدخال nullptr في الخريطة
            // (EN) Fix: use find() instead of [] to avoid inserting nullptr into the map
            llvm::Value *arrPtr = nullptr;
            auto arrIt = cg_.context_info_.namedValues.find(inst->operands[0].name);
            if (arrIt != cg_.context_info_.namedValues.end() && arrIt->second != nullptr)
            {
                arrPtr = arrIt->second;
            }
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            if (!arrPtr)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = normalizeArrayPtr(arrPtr, "arr");

            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
            llvm::Value *result = cg_.builder_->CreateLoad(cg_.getInt64Type(), lenGep, "arr.len");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // دمج مصفوفتين (ARRAY_CONCAT) — array1 + array2
        // Concatenate two arrays — array1 + array2
        // ============================================================================
        llvm::Value *ArrayOpsCodeGen::emitArrayConcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_CONCAT"}});
                return nullptr;
            }

            // (AR) الحصول على المعاملين (المصفوفتين)
            // (EN) Get both operands (the two arrays)
            llvm::Value *arr1 = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *arr2 = cg_.resolveOperand(inst->operands[1]);

            if (!arr1 || !arr2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "ARRAY_CONCAT"}});
                return nullptr;
            }

            // (AR) تطبيع المؤشرات — إذا كانت i64 حوّلها إلى ptr
            // (EN) Normalize pointers — if i64 convert to ptr
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            if (arr1->getType()->isIntegerTy(64))
                arr1 = cg_.builder_->CreateIntToPtr(arr1, ptrTy, "concat.arr1.ptr");
            if (arr2->getType()->isIntegerTy(64))
                arr2 = cg_.builder_->CreateIntToPtr(arr2, ptrTy, "concat.arr2.ptr");

            // (AR) تحميل الأطوال والبيانات من المصفوفتين
            // (EN) Load lengths and data pointers from both arrays
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            llvm::Value *len1Gep = cg_.builder_->CreateStructGEP(arrTy, arr1, 0, "concat.len1.gep");
            llvm::Value *len1 = cg_.builder_->CreateLoad(i64Ty, len1Gep, "concat.len1");
            llvm::Value *data1Gep = cg_.builder_->CreateStructGEP(arrTy, arr1, 2, "concat.data1.gep");
            llvm::Value *data1 = cg_.builder_->CreateLoad(ptrTy, data1Gep, "concat.data1");

            llvm::Value *len2Gep = cg_.builder_->CreateStructGEP(arrTy, arr2, 0, "concat.len2.gep");
            llvm::Value *len2 = cg_.builder_->CreateLoad(i64Ty, len2Gep, "concat.len2");
            llvm::Value *data2Gep = cg_.builder_->CreateStructGEP(arrTy, arr2, 2, "concat.data2.gep");
            llvm::Value *data2 = cg_.builder_->CreateLoad(ptrTy, data2Gep, "concat.data2");

            // (AR) حساب الطول الكلي
            // (EN) Calculate total length
            llvm::Value *totalLen = cg_.builder_->CreateAdd(len1, len2, "concat.total");

            // (AR) إنشاء مصفوفة جديدة بالطول الكلي
            // (EN) Allocate new array with total length
            auto *dlSize = llvm::ConstantExpr::getSizeOf(arrTy);
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *newArr = cg_.builder_->CreateCall(mallocFn, {dlSize}, "concat.arr");

            // (AR) تعيين الطول والسعة
            // (EN) Set length and capacity
            llvm::Value *newLenGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 0, "concat.new.len.gep");
            cg_.builder_->CreateStore(totalLen, newLenGep);
            llvm::Value *newCapGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 1, "concat.new.cap.gep");
            cg_.builder_->CreateStore(totalLen, newCapGep);

            // (AR) تخصيص مخزن البيانات: الطول × حجم المؤشر
            // (EN) Allocate data buffer: total * sizeof(ptr)
            auto *ptrSize = llvm::ConstantExpr::getSizeOf(ptrTy);
            llvm::Value *ptrSize64 = cg_.builder_->CreateIntCast(ptrSize, i64Ty, false);
            llvm::Value *dataSize = cg_.builder_->CreateMul(totalLen, ptrSize64, "concat.data.size");
            llvm::Value *newData = cg_.builder_->CreateCall(mallocFn, {dataSize}, "concat.data");
            llvm::Value *newDataGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 2, "concat.new.data.gep");
            cg_.builder_->CreateStore(newData, newDataGep);

            // (AR) نسخ عناصر المصفوفة الأولى
            // (EN) Copy elements from first array
            llvm::Value *bytes1 = cg_.builder_->CreateMul(len1, ptrSize64, "concat.bytes1");
            cg_.builder_->CreateMemCpy(newData, llvm::MaybeAlign(8), data1, llvm::MaybeAlign(8), bytes1);

            // (AR) نسخ عناصر المصفوفة الثانية بعد الأولى
            // (EN) Copy elements from second array after the first
            llvm::Value *dst2 = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), newData, {bytes1}, "concat.dst2");
            llvm::Value *bytes2 = cg_.builder_->CreateMul(len2, ptrSize64, "concat.bytes2");
            cg_.builder_->CreateMemCpy(dst2, llvm::MaybeAlign(8), data2, llvm::MaybeAlign(8), bytes2);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = newArr;
            }
            return newArr;
        }

        // ============================================================================
        // Phase N: String Core / عمليات النصوص الأساسية
        // ============================================================================

        llvm::Value *ArrayOpsCodeGen::emitStringNew(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            llvm::Value *result;
            if (!inst->operands.empty())
            {
                // Create from existing string/constant
                result = cg_.resolveOperand(inst->operands[0]);
            }
            else
            {
                // Create empty string
                result = cg_.builder_->CreateGlobalStringPtr("", "empty_str");
            }

            if (inst->result.has_value() && result)
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

    } // namespace LLVM
} // namespace Sad
