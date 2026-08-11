/*
 * ============================================================================
 * LLVM IR Code Generator - Array Operations
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sad_dyn_repr.h" // (AR) ISSUE-063: حمولة %SadDyn عند خانات المصفوفة / (EN) %SadDyn payload at array slots
#include "sir_constants.h" // (AR) kSadPanicCheckViolation (رمز سبب الهلع)
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

        // (AR) تخطيط المصفوفة: [i64 طول، i64 سعة، i64* بيانات، i8* وسوم].
        //      الحقل الرابع «الوسوم» (option A، وسمٌ زمن-تشغيل): null للمصفوفة المتجانسة
        //      (المسار الساكن القديم بلا تغيير)؛ وإلّا مخزنُ بايتٍ لكلّ خانةٍ يحمل DynKind،
        //      فتقرِّر القراءةُ نوعَ العنصر **زمنَ التشغيل** من الوسوم لا من النوع الساكن —
        //      فيصمد التمييز عبر إعادة الإسناد/الالتقاط/المعامل/الإرجاع (لا انهيار وسمٍ بائت).
        //      إضافةُ الحقل ٣ لا تُزيح الفهارس ٠–٢، والتخصيصُ ينمو عبر getSizeOf.
        // (EN) Array layout: [i64 length, i64 capacity, i64* data, i8* tags].
        //      The 4th "tags" field (option A, runtime tag): null for a homogeneous array
        //      (the old static path, unchanged); else a per-slot byte buffer of DynKind, so
        //      the READ decides the element kind at RUNTIME from the tags — not the static
        //      type — surviving reassignment/capture/param/return (no stale-tag crash).
        //      Adding field 3 doesn't shift indices 0-2, and allocations grow via getSizeOf.
        //      يُفوَّض إلى التعريفِ الوحيدِ في sad_dyn_repr.
        // (EN) Delegated to the single definition in sad_dyn_repr.
        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            return sadArrayStructType(ctx);
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
        // (AR) انظر التوثيق في array_ops_codegen.h.
        // (EN) See array_ops_codegen.h.
        void ArrayOpsCodeGen::emitDynNotArrayFailure(const char *label)
        {
            if (cg_.freestanding_)
            {
                cg_.emitFreestandingPanicCall(Sad::Compiler::kSadPanicDynTypeMismatch);
            }
            else
            {
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
                auto *printfType = llvm::FunctionType::get(i32Ty, {ptrTy}, true);
                auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
                llvm::Value *msg = cg_.builder_->CreateGlobalStringPtr(
                    Sad::Compiler::kDynTypeMismatchArrayMsg, std::string(label) + ".dyn.fmt");
                cg_.builder_->CreateCall(printfFunc, {msg});
                auto *exitType =
                    llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i32Ty}, false);
                auto exitFunc = cg_.module_->getOrInsertFunction("exit", exitType);
                cg_.builder_->CreateCall(exitFunc, {llvm::ConstantInt::get(i32Ty, 1)});
            }
            cg_.builder_->CreateUnreachable();
        }

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
                // (AR) الحالة 1ب: alloca ptr — خانة محلّيّة تحمل مؤشّر SadArray مباشرةً
                //      (تنشأ من المصفوفات الملتقَطة في الإغلاقات: تُخزَّن الخانةُ عنوانًا لـ SadArray*).
                //      يجب تحميلُ المؤشّر من الخانة؛ وإلّا عُومل عنوانُ الخانة مؤشّرَ مصفوفةٍ فقُرِئت قمامة.
                // (EN) Case 1b: alloca ptr — local slot holding a SadArray* directly (arises from
                //      captured arrays in closures). Must load the pointer out of the slot; otherwise
                //      the slot's address is treated as the array pointer and garbage is read.
                else if (allocaInst->getAllocatedType()->isPointerTy())
                {
                    std::string ptrName = std::string(label) + ".ptr.load";
                    arrPtr = cg_.builder_->CreateLoad(llvm::PointerType::getUnqual(*cg_.context_), allocaInst, ptrName);
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
            // ════════════════════════════════════════════════════════════════
            // (AR) [م-٠٠١] الحالة ٢ب: قيمةٌ موسومةٌ زمنَ التشغيل (%SadDyn) — تصلُ حين
            //      يكون نوعُ الكائنِ الساكنُ «أي»: عنصرُ مصفوفةٍ مختلطةٍ، أو قيمةُ خريطةٍ
            //      مقروءةٌ موسومةً (`م["ج"][0]`). كان هذا الفرعُ غائبًا — بخلافِ نظيرِه في
            //      `normalizeMapPtr` — فكانت البنيةُ تُمرَّرُ إلى GEP بلا فكٍّ فيقرأُ
            //      الباعثُ قمامةً أو يُجهِضُ التوليد. وهو **شرطُ القبولِ الأوّلُ** المسجَّلُ
            //      لعيبِ ز.٤٢ في `template_infer_return.cpp`.
            //
            //      الوسمُ يُحرَسُ زمنَ التشغيل: إن لم يكن مصفوفةً فَشَلٌ **صاخبٌ** برمزٍ
            //      مميَّزٍ بدل فكِّ مرجعِ ما ليس مصفوفة (SIGSEGV صامت).
            // (EN) [card م-٠٠١] Case 2b: a runtime-tagged value (%SadDyn) — it arrives when the
            //      object's static type is «أي»: an element of a heterogeneous array, or a map
            //      value read tagged (`م["ج"][0]`). This branch was missing — unlike its twin in
            //      `normalizeMapPtr` — so the struct reached a GEP unpacked and the emitter read
            //      garbage or aborted codegen. This is the **first acceptance condition** recorded
            //      for defect ز.٤٢ in `template_infer_return.cpp`.
            //
            //      The tag is guarded at runtime: anything that is not an array fails **loudly**
            //      with a distinct code instead of dereferencing a non-array (a silent SIGSEGV).
            // ════════════════════════════════════════════════════════════════
            else if (Sad::LLVM::isSadDyn(arrPtr))
            {
                auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                llvm::Value *kind = Sad::LLVM::dynKindByte(cg_, arrPtr);
                llvm::Value *payload = Sad::LLVM::dynPayloadI64(cg_, arrPtr);
                llvm::Value *isArray = cg_.builder_->CreateICmpEQ(
                    kind, llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Array),
                    std::string(label) + ".dyn.is.array");

                llvm::Function *parentFn = cg_.builder_->GetInsertBlock()->getParent();
                auto *okBB = llvm::BasicBlock::Create(*cg_.context_, "arr.dyn.ok", parentFn);
                auto *failBB = llvm::BasicBlock::Create(*cg_.context_, "arr.dyn.fail", parentFn);
                cg_.builder_->CreateCondBr(isArray, okBB, failBB);

                cg_.builder_->SetInsertPoint(failBB);
                emitDynNotArrayFailure(label);

                cg_.builder_->SetInsertPoint(okBB);
                arrPtr = cg_.builder_->CreateIntToPtr(payload, ptrTy,
                                                      std::string(label) + ".dyn.ptr");
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

            // (AR) ISSUE-063: فهرسٌ %SadDyn (خانة رقّاها المسحُ المسبق) ⇒ فكّ i64 دقيق
            //      قبل مقارنات/حسابات الفهرسة — كان ICmpSLT على بنية ⇒ IR فاسد.
            // (EN) ISSUE-063: a %SadDyn index (pre-scan-promoted slot) ⇒ precise i64
            //      unpack before the indexing compares/arithmetic — ICmpSLT on a struct
            //      was invalid IR.
            if (isSadDyn(index))
                index = unpackI64(cg_, index);

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

            // (AR) كتلة الفشل: مستضاف ⇒ تشخيص إنجليزيّ للمطوّر ثم exit(1)؛ وضع حرّ
            //      ⇒ __sad_panic وحده (لا تشخيص إنجليزيّ يسبق لافتة الهلع).
            // (EN) Fail block: hosted ⇒ English developer diagnostic then exit(1);
            //      freestanding ⇒ __sad_panic only (no English before the panic banner).
            cg_.builder_->SetInsertPoint(failBB);

            if (cg_.freestanding_)
            {
                // (AR) وضع حرّ: لا exit في نواة/معدن عارٍ — نستدعي __sad_panic
                //      (تُبثّ نسخة weak_odr منه في emitFreestandingRuntime؛ وللنواة
                //      تجاوزها بتعريف قويّ خاصّ بها). لا نُصدر printf: النواة تحمل
                //      التشخيص عبر لافتة الهلع العربيّة السياديّة في تعريفها القويّ
                //      لـ__sad_panic، فرسالة إنجليزيّة سابقة تلوّث المخرج السياديّ.
                //      (سبب حذف printf الحرّيّ — RFC مسار الهلع العربيّ الموحَّد.)
                // (EN) Freestanding: no exit on bare metal — call __sad_panic
                //      (weak_odr default emitted in emitFreestandingRuntime; a kernel
                //      overrides it with a strong Arabic-banner definition). No printf:
                //      an English line would pollute the sovereign serial output.
                cg_.emitFreestandingPanicCall(Sad::Compiler::kSadPanicCheckViolation);
            }
            else
            {
                // (AR) مستضاف: تشخيص إنجليزيّ للمطوّر (منفذ libc) ثم exit(1)
                // (EN) Hosted: English developer diagnostic (libc stdout) then exit(1)
                auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg_.context_), {ptrTy}, true);
                auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
                llvm::Value *fmtStr = cg_.builder_->CreateGlobalStringPtr(
                    "Error: array index %lld out of bounds (length: %lld)\n", "bc.fmt");
                cg_.builder_->CreateCall(printfFunc, {fmtStr, index, len});

                auto *exitType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt32Ty(*cg_.context_)}, false);
                auto exitFunc = cg_.module_->getOrInsertFunction("exit", exitType);
                cg_.builder_->CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 1)});
            }
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
            llvm::Value *arrPtr = cg_.emitMalloc(dlSize, "arr_new");

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

            // (AR) تخصيص مخزن البيانات: السعة × حجم الخانة الموحَّد (8 بايت) —
            //      لا getSizeOf(ptr): على i686 المؤشّر 4 بينما الوصول للأرقام
            //      يخطو i64 (8) ⇒ فيضان. انظر SAD_ARRAY_SLOT_BYTES.
            // (EN) Data buffer: capacity × unified slot size (8) — not getSizeOf(ptr).
            llvm::Value *dataSize = cg_.builder_->CreateMul(
                llvm::ConstantInt::get(cg_.getInt64Type(), capacity),
                llvm::ConstantInt::get(cg_.getInt64Type(), SAD_ARRAY_SLOT_BYTES), "arr.data.size");
            llvm::Value *dataPtr = cg_.emitMalloc(dataSize, "arr.data");
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            cg_.builder_->CreateStore(dataPtr, dataGep);

            // (AR) الحقل ٣ (الوسوم) = null افتراضًا: مصفوفةٌ متجانسة (المسار الساكن). تُملأ
            //      لاحقًا عند أوّل كتابةٍ مختلطة (emitArraySet). التخصيصُ لا يُصفّر فيلزم التصريح.
            // (EN) Field 3 (tags) = null by default: a homogeneous array (static path). Populated
            //      later on the first heterogeneous write (emitArraySet). malloc doesn't zero.
            cg_.builder_->CreateStore(
                llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)),
                cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "arr.tags.gep"));
            // (AR) الحقل ٤ (homogKind) = DynKind::Int افتراضًا (يُكتب ولا يُقرأ بعد — خطوة خاملة)
            // (EN) Field 4 (homogKind) = DynKind::Int default (written, not yet read — inert step)
            cg_.builder_->CreateStore(
                llvm::ConstantInt::get(cg_.getInt8Type(), Sad::LLVM::DynKind::Int),
                cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "arr.homogkind.gep"));

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }

        // ====================================================================
        // (AR) **حارسُ الفهرسِ النصّيّ على عمليّةِ مصفوفة.**
        //      معامِلٌ بلا نوعٍ يُفهرَس بمفتاحٍ نصّيّ (`دالة اضف(خ) خ["ا"] = 1`)
        //      تُخفّضه الأماميّةُ `array.set` لا مُساعِدَ خريطة، فيصل إلى هنا فهرسٌ
        //      **مؤشّرُ نصّ**. وكان يمضي إلى `normalizeArrayIndex` فيقارَن بـ0 عبر
        //      ICmpSLT: طيّةٌ ثابتةٌ بين مؤشّرٍ وعددٍ ⇒ تأكيدُ LLVM الحاجب
        //      «Op types should be identical!». وذلك التأكيدُ في بناءِ Debug يفتح
        //      نافذةَ إجهاضٍ مشروطةً فتبقى العمليّةُ حيّةً بلا عملٍ فتُقفِل الملفَّ
        //      التنفيذيَّ ويُخفِق الربطُ لاحقًا بـLNK1168 بلا علاقةٍ ظاهرة.
        //      ⇒ نرفض هنا **صراحةً** بتشخيصِ الكتالوج. الرفضُ مقصودٌ ولا يُستبدَل
        //      بإسنادِ خريطةٍ صامت: الخريطةُ تُمرَّر بالقيمةِ في المفسّر، فإرسالُها
        //      خريطةً هنا يُنتِج جوابًا **مقبولَ المظهرِ خاطئًا** بدل انهيارٍ ظاهر —
        //      وهو تدهور. دلالةُ الإسنادِ المفهرَسِ على معامِلٍ مجهولِ النوع قرارٌ
        //      لغويٌّ (RFC) لا ترقيعُ خلفيّة.
        // (EN) String-index guard on an array op. An untyped parameter indexed by a
        //      string key lowers to `array.set`, so a string POINTER arrives here as
        //      the index and used to be constant-folded against 0 → LLVM's
        //      "Op types should be identical!" assert (a modal abort in Debug that
        //      orphans the process and locks the exe). We now reject explicitly
        //      rather than silently re-routing to a map set, which would turn a
        //      visible crash into a plausible-looking wrong answer.
        // ====================================================================
        bool ArrayOpsCodeGen::rejectStringIndex(const std::shared_ptr<SIRInstruction> &inst)
        {
            if (!inst || inst->operands.size() < 2)
                return false;
            // (AR) والعشريُّ مثلُ النصّ: `خ["أ"][2.5]` يبلغ `normalizeArrayIndex`
            //      فيُقارَنُ `double` بـ`i64` ⇒ التأكيدُ الحاجبُ نفسُه. و«أي» **ليس**
            //      منها: فهرسٌ موسومٌ زمنَ التشغيل مشروعٌ ويفكُّه المسارُ الموسوم.
            // (EN) Float behaves like String here: it reaches normalizeArrayIndex and
            //      compares a double against an i64 ⇒ the same blocking assert. «أي» is
            //      NOT in this set: a runtime-tagged index is legitimate and unpacked.
            if (inst->operands[1].dataType != SadTypeKind::String &&
                inst->operands[1].dataType != SadTypeKind::Float)
                return false;
            cg_.reportError(::Sad::Errors::ErrorCode::SEM_INDEXING_NOT_SUPPORTED,
                            {{"type", ::Sad::Types::sadTypeKindArabicName(inst->operands[0].dataType)}});
            return true;
        }

        // ====================================================================
        // (AR) **إرسالُ الفهرسةِ بوسمِ الكائنِ زمنَ التشغيل** (انظر الترويسةَ للحجّة).
        //      يُفتحُ الفرعُ فقط حين يصل الكائنُ `%SadDyn` والفهرسُ i64: عندئذٍ
        //      وحدَه يستحيل الحسمُ ساكنًا. وخارجَ ذلك يعود بـ`active=false` فلا
        //      يُغيّرُ حرفًا من المسارِ القائم.
        //      الدمجُ بخانةِ مكدّسٍ لا بـPHI: نوعُ نتيجةِ مسارِ المصفوفةِ غيرُ معروفٍ
        //      قبلَ توليدِه، وقيمةٌ مُعرَّفةٌ في فرعٍ لا تسودُ كتلةَ الالتقاء.
        // (EN) Runtime-tag dispatch for indexing. Opened only when the object arrives
        //      as %SadDyn and the index is an i64 — precisely the case no static
        //      analysis can settle. Merging via a stack slot rather than a PHI: the
        //      array path's result type is unknown before it is generated, and a value
        //      defined in one branch does not dominate the join block.
        // ====================================================================
        ArrayOpsCodeGen::DynIndexDispatch
        ArrayOpsCodeGen::beginDynMapDispatch(const std::shared_ptr<SIRInstruction> &inst,
                                             llvm::Value *objValue,
                                             llvm::Value *index,
                                             llvm::Value *value,
                                             bool isSet)
        {
            DynIndexDispatch dispatch;
            if (!inst || !objValue || !index)
                return dispatch;
            if (!Sad::LLVM::isSadDyn(objValue) || !index->getType()->isIntegerTy(64))
                return dispatch;
            if (isSet && !value)
                return dispatch;

            // (AR) المنطقيُّ يُرقّى i64 فيجتاز الحارسَ أعلاه، لكنّ الأمامَ ينصِّصه
            //      «صحيح/خطأ» (BOOL_TO_STRING) بينما ننصِّصُه هنا `%lld` — طرفانِ
            //      لا يتّفقان على شكلِ المفتاح، فيُكتَبُ تحت «0» ويُقرَأُ تحت «صحيح»
            //      فيعود **عدمٌ بخروجٍ ٠**: جوابٌ خاطئٌ صامتٌ محلَّ إجهاضٍ صاخبٍ كان
            //      قائمًا — تدهور. فنُبقي المنطقيَّ خارجَ الإرسالِ حتّى يتّحدَ
            //      التنصيصُ في الطرفَين (دَينٌ موثَّق).
            // (EN) A boolean is promoted to i64 and would pass the guard above, but the
            //      frontend stringifies it as «صحيح/خطأ» while we would use %lld — two
            //      ends disagreeing on the key's shape, yielding a silent wrong answer
            //      where a loud abort stood before. Keep booleans out until both ends
            //      agree (documented debt).
            if (inst->operands.size() > 1 &&
                inst->operands[1].dataType == SadTypeKind::Boolean)
                return dispatch;

            auto *i8Ty = cg_.getInt8Type();
            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Function *parentFn = cg_.builder_->GetInsertBlock()->getParent();
            if (!parentFn)
                return dispatch;

            auto *mapBB = llvm::BasicBlock::Create(*cg_.context_, "idx.dyn.map", parentFn);
            auto *arrBB = llvm::BasicBlock::Create(*cg_.context_, "idx.dyn.arr", parentFn);
            dispatch.contBB = llvm::BasicBlock::Create(*cg_.context_, "idx.dyn.cont", parentFn);

            // (AR) الخاناتُ في **كتلةِ الدخول** لا عند نقطةِ الإدراجِ الجارية: تخصيصُها
            //      في موضعِ الفهرسةِ يعني `alloca` داخلَ جسمِ الحلقة، والمكدّسُ لا
            //      يُستردُّ إلّا بالعودةِ من الدالّة ⇒ فيضانٌ **صامتٌ بلا رسالة** عند
            //      ‏١م.ب÷١٦ب = ٦٥٥٣٦ دورةً بالضبط (قِيس: ٦٠٠٠٠ تنجح و٦٥٠٠٠ تخرج ١٢٧).
            //      والمخزنُ مكدّسٌ لا كومة: `__sad_map_set_typed` يحتفظ بنسخةِ `strdup`
            //      لا بالمخزنِ نفسِه، والقراءةُ لا يحتفظ بها أحد — فكان `emitMalloc`
            //      تسريبَ ٣٢ بايتًا لكلّ عمليّة (قِيس: ١٢٦م.ب مقابل ٥م.ب لشاهدٍ).
            // (EN) Allocate in the ENTRY block, not at the current insertion point: an
            //      alloca inside a loop body is never reclaimed until the function returns
            //      ⇒ a silent stack overflow at exactly 1MB/16B = 65536 iterations. And the
            //      key buffer is stack, not heap: __sad_map_set_typed keeps a strdup copy
            //      and the read keeps nothing, so emitMalloc leaked 32 bytes per operation.
            llvm::IRBuilder<> entryBuilder(&parentFn->getEntryBlock(),
                                           parentFn->getEntryBlock().getFirstInsertionPt());
            if (!isSet)
                dispatch.slot = entryBuilder.CreateAlloca(
                    Sad::LLVM::getSadDynType(*cg_.context_), nullptr, "idx.dyn.slot");
            llvm::Value *keyBuf = entryBuilder.CreateAlloca(
                llvm::ArrayType::get(cg_.getInt8Type(), 32), nullptr, "idx.dyn.key.buf");

            llvm::Value *kind = Sad::LLVM::dynKindByte(cg_, objValue);
            llvm::Value *isMap = cg_.builder_->CreateICmpEQ(
                kind, llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Map), "idx.dyn.is.map");
            cg_.builder_->CreateCondBr(isMap, mapBB, arrBB);

            // (AR) فرعُ الخريطة: المفتاحُ يُنصَّصُ بمساعِدِ زمنِ التشغيلِ نفسِه الذي
            //      يستعمله `نص()` — الكومةُ لا المكدّس، لأنّ `__sad_map_set_typed`
            //      يحتفظ بالمؤشّرِ في مصفوفةِ المفاتيح.
            // (EN) Map branch: the key is stringified by the same runtime helper `نص()`
            //      uses — heap, not stack, because __sad_map_set_typed retains the pointer.
            cg_.builder_->SetInsertPoint(mapBB);
            llvm::Value *key = keyBuf;
            auto *sprintfTy = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_), {ptrTy, ptrTy}, true);
            auto sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);
            llvm::Value *keyFmt = cg_.builder_->CreateGlobalStringPtr("%lld", "idx.dyn.key.fmt");
            cg_.builder_->CreateCall(sprintfFn, {key, keyFmt, index});

            std::vector<llvm::Value *> mapArgs{objValue, key};
            if (isSet)
            {
                mapArgs.push_back(value);
                mapArgs.push_back(llvm::ConstantInt::get(
                    i64Ty, ::Sad::Compiler::mapValueTagFor(inst->operands[2].dataType)));
                cg_.emitCallMap(::Sad::Compiler::kRuntimeMapSetTyped, mapArgs, inst);
            }
            else
            {
                auto mapValue = cg_.emitCallMap(::Sad::Compiler::kRuntimeMapGetDyn, mapArgs, inst);
                if (mapValue.has_value() && mapValue.value())
                    cg_.builder_->CreateStore(mapValue.value(), dispatch.slot);
            }
            if (!cg_.builder_->GetInsertBlock()->getTerminator())
                cg_.builder_->CreateBr(dispatch.contBB);

            cg_.builder_->SetInsertPoint(arrBB);
            dispatch.active = true;
            return dispatch;
        }

        llvm::Value *ArrayOpsCodeGen::endDynMapDispatch(DynIndexDispatch &dispatch,
                                                        llvm::Value *arrayResult)
        {
            if (!dispatch.active || !dispatch.contBB)
                return arrayResult;

            // (AR) مسارُ المصفوفةِ قد يكون بدّل الكتلةَ الحاليّةَ (حرّاسٌ وفحوصُ حدود).
            // (EN) The array path may have moved the current block (guards, bounds checks).
            if (dispatch.slot && arrayResult)
            {
                llvm::Value *asDyn = Sad::LLVM::isSadDyn(arrayResult)
                                         ? arrayResult
                                         : Sad::LLVM::toDyn(cg_, arrayResult, SadTypeKind::Any);
                if (asDyn)
                    cg_.builder_->CreateStore(asDyn, dispatch.slot);
            }
            if (!cg_.builder_->GetInsertBlock()->getTerminator())
                cg_.builder_->CreateBr(dispatch.contBB);

            cg_.builder_->SetInsertPoint(dispatch.contBB);
            if (!dispatch.slot)
                return arrayResult;
            return cg_.builder_->CreateLoad(
                Sad::LLVM::getSadDynType(*cg_.context_), dispatch.slot, "idx.dyn.merged");
        }

        llvm::Value *ArrayOpsCodeGen::emitArrayGet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_GET"}});
                return nullptr;
            }

            if (rejectStringIndex(inst))
                return nullptr;

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

            DynIndexDispatch dispatch =
                beginDynMapDispatch(inst, arrPtr, index, nullptr, /*isSet=*/false);

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
            // (AR) [عناصر موسومة زمنَ التشغيل — الخيار أ الجذريّ] نتيجةٌ ديناميّة (Any):
            //      نتفرّع زمنَ التشغيل على مخزنِ الوسوم (الحقل ٣). إن كان ≠null فالمصفوفةُ
            //      مختلطة ⇒ نعيد بناء %SadDyn من (الوسم، الحمولة الخام)؛ وإلّا (احتياطيّ)
            //      نبنيه بوسم Int من الحمولة. كلا المسارين يُنتجان Any — فلا فكُّ تعليبٍ
            //      على خانةٍ خام أبدًا (يُغلق انهيارَ الوسم البائت). نظيرُ كتابة emitArraySet.
            // (EN) [runtime-tagged elements — radical option A] a dynamic (Any) result:
            //      branch at RUNTIME on the tags buffer (field 3). If != null the array is
            //      heterogeneous ⇒ reconstruct %SadDyn from (tag, raw payload); else (defensive
            //      fallback) build it with an Int tag from the payload. Both yield Any — never
            //      an unbox on a raw slot (closes the stale-tag crash). Inverse of emitArraySet.
            bool isBoxedDyn = (inst->result.has_value() &&
                               inst->result->dataType == SadTypeKind::Any);
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

            if (isBoxedDyn)
            {
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i8Ty = cg_.getInt8Type();
                auto *i64Ty = cg_.getInt64Type();

                // (AR) حمّل الحمولةَ الخام i64 من خانة البيانات (نظير كتابة emitArraySet)
                // (EN) load the raw i64 payload from the data slot (inverse of emitArraySet)
                llvm::Value *slot = cg_.builder_->CreateGEP(i64Ty, dataPtr, {index}, "arr.dyn.slot");
                llvm::Value *payload = cg_.builder_->CreateLoad(i64Ty, slot, "arr.dyn.payload");

                // (AR) تفرّع زمنَ التشغيل على مخزنِ الوسوم
                // (EN) branch at RUNTIME on the tags buffer
                llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "arr.tags.gep");
                llvm::Value *tagsPtr = cg_.builder_->CreateLoad(ptrTy, tagsGep, "arr.tags");
                llvm::Value *hasTags = cg_.builder_->CreateICmpNE(
                    tagsPtr, llvm::ConstantPointerNull::get(ptrTy), "arr.tags.present");

                llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
                auto *tagBB = llvm::BasicBlock::Create(*cg_.context_, "arr.get.tagged", fn);
                auto *rawBB = llvm::BasicBlock::Create(*cg_.context_, "arr.get.raw", fn);
                auto *joinBB = llvm::BasicBlock::Create(*cg_.context_, "arr.get.join", fn);
                cg_.builder_->CreateCondBr(hasTags, tagBB, rawBB);

                // (AR) مختلطة: الوسمُ من tags[index]، القيمةُ makeDyn(الوسم، الحمولة)
                // (EN) heterogeneous: tag from tags[index], value = makeDyn(tag, payload)
                cg_.builder_->SetInsertPoint(tagBB);
                llvm::Value *tagSlot = cg_.builder_->CreateGEP(i8Ty, tagsPtr, {index}, "arr.tag.slot");
                llvm::Value *tagByte = cg_.builder_->CreateLoad(i8Ty, tagSlot, "arr.tag");
                llvm::Value *dynTagged = makeDyn(cg_, tagByte, payload);
                llvm::BasicBlock *tagEndBB = cg_.builder_->GetInsertBlock();
                cg_.builder_->CreateBr(joinBB);

                // (AR) لا وسوم ⇒ المصفوفةُ متجانسة: اقرأ نوعَها الحقيقيّ من الحقل ٤ (homogKind)
                //      وابنِ %SadDyn به (بدل وسمِ Int المُصلَّب سابقًا). نظيرُ كتابة homogKind في emitArraySet.
                // (EN) no tags ⇒ homogeneous array: load its true kind from field 4 (homogKind)
                //      and build %SadDyn with it (was hardcoded Int). Inverse of the homogKind store in emitArraySet.
                cg_.builder_->SetInsertPoint(rawBB);
                llvm::Value *homogKindGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "arr.homogkind.gep");
                llvm::Value *homogKindByte = cg_.builder_->CreateLoad(i8Ty, homogKindGep, "arr.homogkind");
                llvm::Value *dynRaw = makeDyn(cg_, homogKindByte, payload);
                llvm::BasicBlock *rawEndBB = cg_.builder_->GetInsertBlock();
                cg_.builder_->CreateBr(joinBB);

                cg_.builder_->SetInsertPoint(joinBB);
                llvm::PHINode *dynPhi = cg_.builder_->CreatePHI(dynTagged->getType(), 2, "arr.get.dyn");
                dynPhi->addIncoming(dynTagged, tagEndBB);
                dynPhi->addIncoming(dynRaw, rawEndBB);
                result = dynPhi;
            }
            else if (isNestedArray)
            {
                // (AR) العنصر مؤشر (مصفوفة متداخلة / نص / بنية). الخطوة i64 (8)
                //      لتوحيد حجم الخانة عبر الأهداف (على i686 خطوة ptr=4 كانت
                //      تخالف تخصيص/وصول الأرقام). التحميل يبقى بنوع مؤشّر.
                // (EN) Pointer element; stride by i64 (8) for unified slots, load as ptr.
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(
                    cg_.getInt64Type(), dataPtr, {index}, "arr.elem.ptr");
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
                // (AR) [ISSUE-082] عنصر قائمةٍ (غير-صفّ) عشريّ: الخانة i64 تحمل بتات
                //      double (bitcast عند التخزين في emitArraySet)؛ نعيد bitcast لـdouble
                //      حين النوع المُعلَن Float ليتطابق تمثيل السجلّ مع نوعه — وإلّا خطأ
                //      مدقّق LLVM أو قيمة عدديّة خاطئة في المستهلك النازل. متماثلٌ مع
                //      تخزين emitArraySet. الصفوف (tuple) خارج هذا (ISSUE-052 أعلاه).
                // (EN) [ISSUE-082] Float (non-tuple) list element: the i64 slot holds
                //      double bits (bitcast at store in emitArraySet); bitcast back to
                //      double when the declared type is Float so the register matches its
                //      type — else LLVM verifier error / wrong numeric value downstream.
                //      Symmetric with emitArraySet. Tuples are excluded (ISSUE-052 above).
                else if (inst->result.has_value() &&
                         inst->result->dataType == SadTypeKind::Float)
                {
                    result = cg_.builder_->CreateBitCast(result, cg_.getDoubleType(), "arr.get.f2d");
                }
                // (AR) [ISSUE-082] عنصر منطقيّ (غير-صفّ): خُزِّن i1 مُمتدًّا بإشارة إلى i64
                //      (صحيح→-1، خطأ→0، emitArraySet)؛ نعيده i1 قانونيًّا (Boolean=i1) عبر
                //      مقارنةٍ بصفر، ليتطابق مع تمثيل المنطقيّ في المقارنة النازلة `ن == صحيح`.
                // (EN) [ISSUE-082] Boolean (non-tuple) list element: stored as sign-extended
                //      i1→i64 (true→-1, false→0, emitArraySet); read back as canonical i1
                //      (Boolean=i1) via compare-nonzero, matching the bool representation in
                //      the downstream comparison `ن == صحيح`.
                else if (inst->result.has_value() &&
                         inst->result->dataType == SadTypeKind::Boolean)
                {
                    result = cg_.builder_->CreateICmpNE(
                        result, llvm::ConstantInt::get(cg_.getInt64Type(), 0), "arr.get.b2i1");
                }
            }

            result = endDynMapDispatch(dispatch, result);

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

            if (rejectStringIndex(inst))
                return nullptr;

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

            DynIndexDispatch dispatch =
                beginDynMapDispatch(inst, arrPtr, index, value, /*isSet=*/true);

            // ================================================================
            // (AR) [عناصر موسومة زمنَ التشغيل — الخيار أ الجذريّ] مصفوفةٌ مختلطةٌ قياسيّة
            //      (elementType=Any على معامل المصفوفة): بدل تعليب الكومة، نخزّن الحمولةَ
            //      الخام i64 في خانة البيانات، ووسمَ DynKind (i8) في مخزنِ الوسوم المتوازي
            //      (الحقل ٣). القراءةُ تتفرّع زمنَ التشغيل على (الوسوم≠null) فيصمد التمييزُ
            //      عبر إعادة الإسناد/الالتقاط/المعامل/الإرجاع — لا اعتماد على النوع الساكن
            //      البائت (يُغلق صنفَ الانهيار الذي أثبتته أميليا في مسار التعليب).
            // (EN) [runtime-tagged elements — radical option A] a scalar-heterogeneous array
            //      (elementType=Any on the array operand): instead of heap boxing, store the
            //      RAW i64 payload in the data slot and the DynKind tag (i8) in the parallel
            //      tags buffer (field 3). The READ branches at RUNTIME on (tags != null), so
            //      the discrimination survives reassignment/capture/param/return — no reliance
            //      on the stale static type (closes the crash class Amelia proved in boxing).
            // ================================================================
            if (inst->operands[0].elementType == SadTypeKind::Any)
            {
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *i8Ty = cg_.getInt8Type();
                auto *i64Ty = cg_.getInt64Type();

                // (AR) وحِّد العنصرَ إلى %SadDyn (يشتقّ الوسمَ + الحمولة لأيّ نوع؛ الديناميّ
                //      يُمرَّر كما هو)، ثمّ فكّكه إلى وسمٍ (i8) وحمولةٍ (i64).
                // (EN) Normalize the element to %SadDyn (derives kind + payload for any type;
                //      an already-dynamic value passes through), then split into i8 kind + i64.
                llvm::Value *dyn = toDyn(cg_, value, inst->operands[2].dataType);
                llvm::Value *kindByte = dynKindByte(cg_, dyn);
                llvm::Value *payload = dynPayloadI64(cg_, dyn);

                arrPtr = normalizeArrayPtr(arrPtr, "arr");
                index = normalizeArrayIndex(index, arrPtr, "set");
                emitBoundsCheck(index, arrPtr, "set");
                llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

                // (AR) خزّن الحمولةَ الخام i64 في خانة البيانات (خطوة i64 موحّدة)
                // (EN) store the raw i64 payload in the data slot (unified i64 stride)
                llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
                llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "arr.data");
                llvm::Value *slot = cg_.builder_->CreateGEP(i64Ty, dataPtr, {index}, "arr.dyn.slot");
                cg_.builder_->CreateStore(payload, slot);

                // (AR) اضمن مخزنَ الوسوم (يُخصَّص كسولًا عند أوّل كتابةٍ مختلطة، مُصفَّرًا
                //      إلى Null) ثمّ اكتب وسمَ هذه الخانة.
                // (EN) ensure the tags buffer (lazily allocated on first heterogeneous write,
                //      zeroed to Null) then write this slot's tag.
                llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "arr.tags.gep");
                llvm::Value *curTags = cg_.builder_->CreateLoad(ptrTy, tagsGep, "arr.tags.cur");
                llvm::Value *tagsNull = cg_.builder_->CreateICmpEQ(
                    curTags, llvm::ConstantPointerNull::get(ptrTy), "arr.tags.isnull");

                llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
                auto *allocBB = llvm::BasicBlock::Create(*cg_.context_, "arr.tags.alloc", fn);
                auto *contBB = llvm::BasicBlock::Create(*cg_.context_, "arr.tags.cont", fn);
                llvm::BasicBlock *preBB = cg_.builder_->GetInsertBlock();
                cg_.builder_->CreateCondBr(tagsNull, allocBB, contBB);

                cg_.builder_->SetInsertPoint(allocBB);
                llvm::Value *capGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "arr.tags.cap.gep");
                llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "arr.tags.cap");
                llvm::Value *newTags = cg_.emitMalloc(cap, "arr.tags.buf");
                cg_.builder_->CreateMemSet(
                    newTags, llvm::ConstantInt::get(i8Ty, DynKind::Null), cap, llvm::MaybeAlign(1));
                cg_.builder_->CreateStore(newTags, tagsGep);
                llvm::BasicBlock *allocEndBB = cg_.builder_->GetInsertBlock();
                cg_.builder_->CreateBr(contBB);

                cg_.builder_->SetInsertPoint(contBB);
                llvm::PHINode *tags = cg_.builder_->CreatePHI(ptrTy, 2, "arr.tags");
                tags->addIncoming(curTags, preBB);
                tags->addIncoming(newTags, allocEndBB);

                llvm::Value *tagSlot = cg_.builder_->CreateGEP(i8Ty, tags, {index}, "arr.tag.slot");
                cg_.builder_->CreateStore(kindByte, tagSlot);
                endDynMapDispatch(dispatch, nullptr);
                return dyn;
            }
            // (AR) ISSUE-063 (غير-Any): قيمة %SadDyn بخانةٍ عاديّة ⇒ خزّن الحمولة i64 (بتّاتها
            //      تطابق أعراف الخانة لكلّ وسم) بدل كتابة بنية 16 بايت خامًا (IR فاسد).
            // (EN) ISSUE-063 (non-Any): a %SadDyn value into a plain slot ⇒ store the i64
            //      payload (bits match the slot conventions) instead of the raw 16-byte struct.
            else if (isSadDyn(value))
            {
                value = dynPayloadI64(cg_, value);
            }

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
                // (AR) تخزين مؤشر (مصفوفة متداخلة / نص / بنية). الخطوة i64 (8)
                //      لتوحيد حجم الخانة عبر الأهداف. التخزين يبقى بقيمة مؤشّر.
                // (EN) Store pointer element; stride by i64 (8) for unified slots.
                llvm::Value *elemPtr = cg_.builder_->CreateGEP(
                    cg_.getInt64Type(), dataPtr, {index}, "arr.elem.ptr");
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

            // (AR) [homogKind — الحقل ٤] مسارُ الكتابة الساكن (غير-Any): كلُّ عناصر المصفوفة
            //      المتجانسة تُكتب بنفس النوع ⇒ خزِّن DynKind المشتقَّ من نوع القيمة في الحقل ٤
            //      (متعادِلٌ: تكرارُ نفس القيمة). القارئ يقرؤه فقط حين tags==null، فلا يُقرأ
            //      لمصفوفةٍ مختلطة (لها tags≠null). لا تكتبه في مسار Any (يترك الافتراضيّ Int).
            // (EN) [homogKind — field 4] static (non-Any) store path: every element of a
            //      homogeneous array is stored with the same kind ⇒ store the DynKind derived
            //      from the value type into field 4 (idempotent). The reader only reads it when
            //      tags==null, so it is never read for a mixed array (tags!=null). Not written on
            //      the Any path (which leaves the default Int).
            if (inst->operands.size() > 2)
            {
                SadTypeKind concreteKind = inst->operands[2].dataType;
                uint8_t k = Sad::LLVM::DynKind::Int;
                switch (concreteKind)
                {
                case SadTypeKind::Float: k = Sad::LLVM::DynKind::Float; break;
                case SadTypeKind::String: case SadTypeKind::Pointer: k = Sad::LLVM::DynKind::Str; break;
                case SadTypeKind::Boolean: k = Sad::LLVM::DynKind::Bool; break;
                case SadTypeKind::Null: k = Sad::LLVM::DynKind::Null; break;
                case SadTypeKind::Array: k = Sad::LLVM::DynKind::Array; break;
                default: k = Sad::LLVM::DynKind::Int; break;
                }
                llvm::Value *hkGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "arr.set.homogkind.gep");
                cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt8Type(), k), hkGep);
            }

            endDynMapDispatch(dispatch, nullptr);
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
            llvm::Value *newArr = cg_.emitMalloc(dlSize, "concat.arr");

            // (AR) تعيين الطول والسعة
            // (EN) Set length and capacity
            llvm::Value *newLenGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 0, "concat.new.len.gep");
            cg_.builder_->CreateStore(totalLen, newLenGep);
            llvm::Value *newCapGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 1, "concat.new.cap.gep");
            cg_.builder_->CreateStore(totalLen, newCapGep);

            // (AR) تخصيص مخزن البيانات ونسخه: الطول × حجم الخانة الموحَّد (8) —
            //      متّسق مع تخطيط خانات المصدر (SAD_ARRAY_SLOT_BYTES).
            // (EN) Allocate & copy by unified slot size (8), matching source layout.
            llvm::Value *ptrSize64 = llvm::ConstantInt::get(i64Ty, SAD_ARRAY_SLOT_BYTES);
            llvm::Value *dataSize = cg_.builder_->CreateMul(totalLen, ptrSize64, "concat.data.size");
            llvm::Value *newData = cg_.emitMalloc(dataSize, "concat.data");
            llvm::Value *newDataGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 2, "concat.new.data.gep");
            cg_.builder_->CreateStore(newData, newDataGep);
            // (AR) الحقل ٤ (homogKind) = DynKind::Int افتراضًا (خامل — يُكتب ولا يُقرأ بعد)
            // (EN) Field 4 (homogKind) = DynKind::Int default (inert — written, not yet read)
            cg_.builder_->CreateStore(
                llvm::ConstantInt::get(cg_.getInt8Type(), Sad::LLVM::DynKind::Int),
                cg_.builder_->CreateStructGEP(arrTy, newArr, 4, "concat.homogkind.gep"));
            llvm::Value *newTagsGep = cg_.builder_->CreateStructGEP(arrTy, newArr, 3, "concat.tags.gep");

            // (AR) نسخ عناصر المصفوفة الأولى
            // (EN) Copy elements from first array
            llvm::Value *bytes1 = cg_.builder_->CreateMul(len1, ptrSize64, "concat.bytes1");
            cg_.builder_->CreateMemCpy(newData, llvm::MaybeAlign(8), data1, llvm::MaybeAlign(8), bytes1);

            // (AR) نسخ عناصر المصفوفة الثانية بعد الأولى
            // (EN) Copy elements from second array after the first
            llvm::Value *dst2 = cg_.builder_->CreateGEP(llvm::Type::getInt8Ty(*cg_.context_), newData, {bytes1}, "concat.dst2");
            llvm::Value *bytes2 = cg_.builder_->CreateMul(len2, ptrSize64, "concat.bytes2");
            cg_.builder_->CreateMemCpy(dst2, llvm::MaybeAlign(8), data2, llvm::MaybeAlign(8), bytes2);

            // (AR) [وسم زمن-تشغيل] دمجُ الوسوم: إن كان أيُّ معاملٍ ديناميّ (Any) فالنتيجةُ
            //      واصفةٌ لنفسها. لكلّ منطقة: معاملٌ Any ⇒ انسخ وسومَه الزمنيّة (بحارس null)؛
            //      معاملٌ محدَّدُ النوع ⇒ املأها بوسمِه الساكن (memset ثابت). فتُقرأ النتيجةُ
            //      صحيحةً حتّى لو خُلط محدَّدٌ بمختلط.
            // (EN) [runtime tag] merge tags: if either operand is dynamic (Any) the result is
            //      self-describing. Per region: an Any operand ⇒ copy its runtime tags (null-
            //      guarded); a concrete operand ⇒ fill its static kind (constant memset). So the
            //      result reads correctly even when a concrete array is concatenated with a mixed one.
            bool op0Any = (inst->operands[0].elementType == SadTypeKind::Any);
            bool op1Any = (inst->operands[1].elementType == SadTypeKind::Any);
            // (AR) نبني مخزنَ الوسوم متى نتجت مصفوفةٌ واصفةٌ لنفسها. يوافق هذا الشرطُ
            //      تمامًا معيارَ الواجهة (expression_binary_op.cpp): أيُّ طرفٍ Any، أو
            //      طرفان محدَّدا النوع لكن مختلفان (تنافرٌ حقيقيّ ⇒ لا يكفيه وسمٌ ساكنٌ
            //      واحد). المتجانسان أو فارغُ-النوع لا يحتاجانه (المسار الساكن null).
            // (EN) build the tags buffer whenever the result is self-describing. This
            //      predicate mirrors the frontend (expression_binary_op.cpp) exactly:
            //      either side Any, OR both sides concrete-but-different (a genuine
            //      mismatch a single static kind can't describe). Homogeneous or
            //      unknown-element sides don't need it (null static path).
            SadTypeKind e0 = inst->operands[0].elementType;
            SadTypeKind e1 = inst->operands[1].elementType;
            bool bothKnown = (e0 != SadTypeKind::Void && e1 != SadTypeKind::Void);
            if (op0Any || op1Any || (bothKnown && e0 != e1))
            {
                auto *i8Ty = cg_.getInt8Type();
                llvm::Value *newTags = cg_.emitMalloc(totalLen, "concat.tags.buf");
                // (AR) مُعينٌ يملأ منطقةً: Any⇒نسخ وسوم زمنيّة (حارس null)، محدَّد⇒memset ثابت.
                // (EN) helper to fill a region: Any⇒copy runtime tags (null-guarded), concrete⇒const memset.
                auto fillRegion = [&](llvm::Value *destBase, llvm::Value *srcTags, llvm::Value *rlen,
                                      bool isAny, SadTypeKind concreteKind, const char *tag) {
                    if (isAny)
                    {
                        // (AR) صفّر Null ثمّ انسخ الوسومَ الزمنيّة إن وُجدت
                        cg_.builder_->CreateMemSet(destBase, llvm::ConstantInt::get(i8Ty, DynKind::Null), rlen, llvm::MaybeAlign(1));
                        llvm::Value *hasT = cg_.builder_->CreateICmpNE(
                            srcTags, llvm::ConstantPointerNull::get(ptrTy), std::string(tag) + ".has");
                        llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
                        auto *cpBB = llvm::BasicBlock::Create(*cg_.context_, std::string(tag) + ".cp", fn);
                        auto *ctBB = llvm::BasicBlock::Create(*cg_.context_, std::string(tag) + ".ct", fn);
                        cg_.builder_->CreateCondBr(hasT, cpBB, ctBB);
                        cg_.builder_->SetInsertPoint(cpBB);
                        cg_.builder_->CreateMemCpy(destBase, llvm::MaybeAlign(1), srcTags, llvm::MaybeAlign(1), rlen);
                        cg_.builder_->CreateBr(ctBB);
                        cg_.builder_->SetInsertPoint(ctBB);
                    }
                    else
                    {
                        // (AR) نوعٌ محدَّد ⇒ اشتقّ الوسمَ الساكن واملأ به
                        uint8_t k = DynKind::Int;
                        switch (concreteKind)
                        {
                        case SadTypeKind::Float: k = DynKind::Float; break;
                        case SadTypeKind::String: case SadTypeKind::Pointer: k = DynKind::Str; break;
                        case SadTypeKind::Boolean: k = DynKind::Bool; break;
                        case SadTypeKind::Null: k = DynKind::Null; break;
                        case SadTypeKind::Array: k = DynKind::Array; break;
                        default: k = DynKind::Int; break;
                        }
                        cg_.builder_->CreateMemSet(destBase, llvm::ConstantInt::get(i8Ty, k), rlen, llvm::MaybeAlign(1));
                    }
                };
                llvm::Value *tags1 = cg_.builder_->CreateLoad(ptrTy, cg_.builder_->CreateStructGEP(arrTy, arr1, 3, "concat.t1.gep"), "concat.t1");
                llvm::Value *tags2 = cg_.builder_->CreateLoad(ptrTy, cg_.builder_->CreateStructGEP(arrTy, arr2, 3, "concat.t2.gep"), "concat.t2");
                fillRegion(newTags, tags1, len1, op0Any, inst->operands[0].elementType, "concat.r1");
                llvm::Value *tdst2 = cg_.builder_->CreateGEP(i8Ty, newTags, {len1}, "concat.tags.dst2");
                fillRegion(tdst2, tags2, len2, op1Any, inst->operands[1].elementType, "concat.r2");
                cg_.builder_->CreateStore(newTags, newTagsGep);
            }
            else
            {
                // (AR) كلا المعاملين محدَّدُ النوع ⇒ الوسوم=null (المسار الساكن، بلا تكلفة)
                // (EN) both operands concrete ⇒ tags=null (static path, no cost)
                cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(ptrTy), newTagsGep);

                // (AR) [homogKind] النتيجةُ متجانسةٌ (tags==null) ⇒ إن تطابق نوعُ الطرفين وكانا
                //      معلومَين (e0==e1، غير-Void، غير-Any) فخزِّن DynKindهما الحقيقيّ في الحقل ٤
                //      ليقرأه القارئُ صحيحًا (بدل الافتراضيّ Int). إن اختلف مجهولٌ (Void) اترك Int.
                // (EN) [homogKind] the result is homogeneous (tags==null) ⇒ if both element types
                //      match and are known (e0==e1, non-Void, non-Any) store their true DynKind into
                //      field 4 so the reader reports it correctly (instead of the default Int).
                if (bothKnown && e0 == e1)
                {
                    uint8_t k = Sad::LLVM::DynKind::Int;
                    switch (e0)
                    {
                    case SadTypeKind::Float: k = Sad::LLVM::DynKind::Float; break;
                    case SadTypeKind::String: case SadTypeKind::Pointer: k = Sad::LLVM::DynKind::Str; break;
                    case SadTypeKind::Boolean: k = Sad::LLVM::DynKind::Bool; break;
                    case SadTypeKind::Null: k = Sad::LLVM::DynKind::Null; break;
                    case SadTypeKind::Array: k = Sad::LLVM::DynKind::Array; break;
                    default: k = Sad::LLVM::DynKind::Int; break;
                    }
                    cg_.builder_->CreateStore(
                        llvm::ConstantInt::get(cg_.getInt8Type(), k),
                        cg_.builder_->CreateStructGEP(arrTy, newArr, 4, "concat.homogkind2.gep"));
                }
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = newArr;
            }
            return newArr;
        }

        // ============================================================================
        // (AR) زاوج (ARRAY_ZIP) — اقتران مصفوفتين أزواجًا: النتيجة مصفوفة بطول
        //      min(|أ|، |ب|)، كلّ خانة فيها مؤشّر (ptrtoint) لمصفوفة زوج {أ[i]، ب[i]}.
        //      الخانات تُنسخ خامًا (كما في ARRAY_CONCAT) فيُحفَظ تمثيل العنصر أيًّا كان.
        // (EN) zip (ARRAY_ZIP) — pair two arrays: result has length min(|a|, |b|);
        //      each slot holds a ptrtoint'd two-element pair array {a[i], b[i]}.
        //      Slots are copied raw (like ARRAY_CONCAT), preserving element repr.
        // ============================================================================
        llvm::Value *ArrayOpsCodeGen::emitArrayZip(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_ZIP"}});
                return nullptr;
            }

            llvm::Value *arr1 = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *arr2 = cg_.resolveOperand(inst->operands[1]);
            if (!arr1 || !arr2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "ARRAY_ZIP"}});
                return nullptr;
            }

            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            arr1 = cg_.normalizeArrayPtr(arr1, "zip.a");
            arr2 = cg_.normalizeArrayPtr(arr2, "zip.b");
            if (!arr1 || !arr2)
                return nullptr;

            // (AR) الأطوال ومؤشّرا البيانات / (EN) lengths and data pointers
            llvm::Value *len1 = cg_.builder_->CreateLoad(
                i64Ty, cg_.builder_->CreateStructGEP(arrTy, arr1, 0, "zip.len1.gep"), "zip.len1");
            llvm::Value *data1 = cg_.builder_->CreateLoad(
                ptrTy, cg_.builder_->CreateStructGEP(arrTy, arr1, 2, "zip.data1.gep"), "zip.data1");
            llvm::Value *len2 = cg_.builder_->CreateLoad(
                i64Ty, cg_.builder_->CreateStructGEP(arrTy, arr2, 0, "zip.len2.gep"), "zip.len2");
            llvm::Value *data2 = cg_.builder_->CreateLoad(
                ptrTy, cg_.builder_->CreateStructGEP(arrTy, arr2, 2, "zip.data2.gep"), "zip.data2");

            // (AR) الطول = min(len1, len2) — دلالة المفسّر (std::min)
            // (EN) length = min(len1, len2) — interpreter semantics (std::min)
            llvm::Value *useLen1 = cg_.builder_->CreateICmpSLT(len1, len2, "zip.cmp");
            llvm::Value *outLen = cg_.builder_->CreateSelect(useLen1, len1, len2, "zip.len");

            // (AR) تخصيص مصفوفة الناتج / (EN) allocate the result array
            auto *dlSize = llvm::ConstantExpr::getSizeOf(arrTy);
            llvm::Value *outArr = cg_.emitMalloc(dlSize, "zip.arr");
            cg_.builder_->CreateStore(outLen, cg_.builder_->CreateStructGEP(arrTy, outArr, 0, "zip.out.len.gep"));
            cg_.builder_->CreateStore(outLen, cg_.builder_->CreateStructGEP(arrTy, outArr, 1, "zip.out.cap.gep"));
            llvm::Value *slotSize = llvm::ConstantInt::get(i64Ty, SAD_ARRAY_SLOT_BYTES);
            llvm::Value *outBytes = cg_.builder_->CreateMul(outLen, slotSize, "zip.out.bytes");
            llvm::Value *outData = cg_.emitMalloc(outBytes, "zip.out.data");
            cg_.builder_->CreateStore(outData, cg_.builder_->CreateStructGEP(arrTy, outArr, 2, "zip.out.data.gep"));
            cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(ptrTy),
                cg_.builder_->CreateStructGEP(arrTy, outArr, 3, "zip.out.tags.gep")); // (AR) وسوم=null
            // (AR) الحقل ٤ (homogKind): كلُّ خانةٍ مؤشّرٌ لمصفوفة زوج ⇒ homogKind=Array
            //      (المصفوفةُ متجانسةٌ من مصفوفات، tags==null فيُقرأ الحقل ٤).
            // (EN) Field 4 (homogKind): every slot is a pointer to a pair-array ⇒ homogKind=Array
            //      (homogeneous array-of-arrays, tags==null so field 4 is read).
            cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt8Type(), Sad::LLVM::DynKind::Array),
                cg_.builder_->CreateStructGEP(arrTy, outArr, 4, "zip.out.homogkind.gep"));

            // (AR) الحلقة: لكلّ i أنشئ مصفوفة زوج {أ[i]، ب[i]} وخزّن مؤشّرها
            // (EN) loop: for each i build pair array {a[i], b[i]} and store its pointer
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "zip.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "zip.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "zip.done", curFunc);

            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *iVal = cg_.builder_->CreatePHI(i64Ty, 2, "zip.i");
            iVal->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            llvm::Value *cond = cg_.builder_->CreateICmpSLT(iVal, outLen, "zip.cond");
            cg_.builder_->CreateCondBr(cond, bodyBB, doneBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *two = llvm::ConstantInt::get(i64Ty, 2);
            llvm::Value *pairArr = cg_.emitMalloc(dlSize, "zip.pair");
            cg_.builder_->CreateStore(two, cg_.builder_->CreateStructGEP(arrTy, pairArr, 0, "zip.pair.len.gep"));
            cg_.builder_->CreateStore(two, cg_.builder_->CreateStructGEP(arrTy, pairArr, 1, "zip.pair.cap.gep"));
            llvm::Value *pairBytes = cg_.builder_->CreateMul(two, slotSize, "zip.pair.bytes");
            llvm::Value *pairData = cg_.emitMalloc(pairBytes, "zip.pair.data");
            cg_.builder_->CreateStore(pairData, cg_.builder_->CreateStructGEP(arrTy, pairArr, 2, "zip.pair.data.gep"));
            cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(ptrTy),
                cg_.builder_->CreateStructGEP(arrTy, pairArr, 3, "zip.pair.tags.gep")); // (AR) وسوم=null
            // (AR) الحقل ٤ (homogKind) = DynKind::Int افتراضًا (خامل — يُكتب ولا يُقرأ بعد)
            cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt8Type(), Sad::LLVM::DynKind::Int),
                cg_.builder_->CreateStructGEP(arrTy, pairArr, 4, "zip.pair.homogkind.gep"));

            // (AR) نسخ الخانتين خامًا / (EN) raw copy of both slots
            llvm::Value *e1 = cg_.builder_->CreateLoad(
                i64Ty, cg_.builder_->CreateGEP(i64Ty, data1, {iVal}, "zip.e1.gep"), "zip.e1");
            llvm::Value *e2 = cg_.builder_->CreateLoad(
                i64Ty, cg_.builder_->CreateGEP(i64Ty, data2, {iVal}, "zip.e2.gep"), "zip.e2");
            cg_.builder_->CreateStore(e1, cg_.builder_->CreateGEP(
                i64Ty, pairData, {llvm::ConstantInt::get(i64Ty, 0)}, "zip.p0.gep"));
            cg_.builder_->CreateStore(e2, cg_.builder_->CreateGEP(
                i64Ty, pairData, {llvm::ConstantInt::get(i64Ty, 1)}, "zip.p1.gep"));

            // (AR) خزّن مؤشّر الزوج (ptrtoint) في خانة الناتج
            // (EN) store the pair pointer (ptrtoint) into the result slot
            llvm::Value *pairAsI64 = cg_.builder_->CreatePtrToInt(pairArr, i64Ty, "zip.pair.i64");
            cg_.builder_->CreateStore(pairAsI64, cg_.builder_->CreateGEP(i64Ty, outData, {iVal}, "zip.out.slot"));

            llvm::Value *nextI = cg_.builder_->CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1), "zip.next");
            iVal->addIncoming(nextI, cg_.builder_->GetInsertBlock());
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = outArr;
            }
            return outArr;
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
