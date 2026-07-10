/*
 * ============================================================================
 * LLVM IR Code Generator - Enum/ADT Operations
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/oop/enum_ops_codegen.h"
#include "sad_dyn_repr.h"   // (AR) ISSUE-076: التمثيل الديناميّ المميّز %SadDyn (تخزين/استخراج حمولة ADT)
#include "sir_constants.h"  // (AR) ISSUE-080: kAdtFieldDispatchSentinel + رسالة trap الحالة الخاطئة
#include "adt_payload_tags.h" // (AR) وسم قديم — مسارٌ ميّت لخانات %SadDyn، يُحذف في تنظيف ISSUE-076
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
        // (AR) أنواع البيانات الجبرية — تعدادات ADT
        //      هذه الدوال الأربع تعالج التعليمات الجديدة:
        //      ENUM_CONSTRUCT, ENUM_GET_TAG, ENUM_GET_PAYLOAD, ENUM_IS_VARIANT
        //
        //      تمثيل التعداد الجبري في الذاكرة:
        //      struct __adt_<اسم> {
        //          i64 __tag;      // المميّز (رقم الحالة)
        //          ptr __f0;       // الحقل الأول (أو null)
        //          ptr __f1;       // الحقل الثاني ...
        //          ...
        //      }
        //
        // (EN) Algebraic Data Types — ADT Enums
        //      These four functions handle the new instructions:
        //      ENUM_CONSTRUCT, ENUM_GET_TAG, ENUM_GET_PAYLOAD, ENUM_IS_VARIANT
        //
        //      ADT memory layout:
        //      struct __adt_<name> {
        //          i64 __tag;      // discriminant (variant index)
        //          ptr __f0;       // first field (or null)
        //          ptr __f1;       // second field ...
        //          ...
        //      }
        // ============================================================================

        llvm::Value *EnumOpsCodeGen::emitEnumConstruct(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) بناء حالة تعداد جبري
            //      المُعاملات: [0]=اسم البنية، [1]=المميّز (tag)، [2..N]=قيم الحقول
            //      النتيجة: مؤشر إلى البنية المُخصصة على الكومة
            // (EN) Construct ADT enum variant
            //      Operands: [0]=struct name, [1]=tag value, [2..N]=field values
            //      Result: pointer to heap-allocated struct

            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_CONSTRUCT"}});
                return nullptr;
            }

            std::string structName = inst->operands[0].name;
            int64_t tagValue = inst->operands[1].intValue;

            // (AR) البحث عن نوع البنية المسجل في classStructTypes
            //      أثناء SIR→LLVM المعالجة المسبقة، CLASS_DEF ينشئ StructType
            // (EN) Look up the struct type registered in classStructTypes
            //      During SIR→LLVM preprocessing, CLASS_DEF creates the StructType
            auto structIt = cg_.context_info_.classStructTypes.find(structName);
            llvm::StructType *structType = nullptr;

            if (structIt != cg_.context_info_.classStructTypes.end())
            {
                structType = structIt->second;
            }
            else
            {
                // (AR) إذا لم تُسجل البنية — ننشئ نوع مجهول بناءً على عدد الحقول
                //      حقل 0 = i64 (tag)، الباقي = %SadDyn (ISSUE-076: حمولة واصفة لذاتها)
                // (EN) If struct not registered — create anonymous type based on field count
                //      field 0 = i64 (tag), rest = %SadDyn (ISSUE-076: self-describing payload)
                std::vector<llvm::Type *> fieldTypes;
                fieldTypes.push_back(cg_.getInt64Type()); // __tag
                for (size_t i = 2; i < inst->operands.size(); ++i)
                {
                    fieldTypes.push_back(getSadDynType(*cg_.context_));
                }
                structType = llvm::StructType::create(*cg_.context_, fieldTypes, structName);
                cg_.context_info_.classStructTypes[structName] = structType;
            }

            // (AR) تخصيص ذاكرة على الكومة
            // (EN) Heap allocate
            auto *structSize = llvm::ConstantExpr::getSizeOf(structType);
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_), {cg_.getInt64Type()}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *objPtr = cg_.builder_->CreateCall(
                mallocFunc,
                {cg_.builder_->CreateIntCast(structSize, cg_.getInt64Type(), false)},
                structName + ".adt");

            // (AR) تخزين المميّز في الحقل 0 (__tag)
            // (EN) Store discriminant in field 0 (__tag)
            llvm::Value *tagGEP = cg_.builder_->CreateStructGEP(structType, objPtr, 0, "tag.gep");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(cg_.getInt64Type(), tagValue), tagGEP);

            // (AR) تخزين حقول الحمولة في الحقول التالية
            // (EN) Store payload fields in subsequent fields
            for (size_t i = 2; i < inst->operands.size(); ++i)
            {
                llvm::Value *fieldVal = cg_.resolveOperand(inst->operands[i]);
                if (fieldVal)
                {
                    // (AR) الحقول تبدأ من الفهرس 1 (الفهرس 0 للمميّز)
                    // (EN) Fields start at index 1 (index 0 is the tag)
                    unsigned fieldIdx = static_cast<unsigned>(i - 1);
                    if (fieldIdx < structType->getNumElements())
                    {
                        llvm::Value *fieldGEP = cg_.builder_->CreateStructGEP(
                            structType, objPtr, fieldIdx,
                            "field." + std::to_string(fieldIdx - 1) + ".gep");

                        // (AR) تحويل القيمة إلى نوع الحقل إذا لزم الأمر
                        // (EN) Cast value to field type if necessary
                        llvm::Type *expectedType = structType->getElementType(fieldIdx);

                        // (AR) === ISSUE-076 (حلّ %SadDyn الجذريّ): خانة حمولة ديناميّة ===
                        //      نغلّف القيمة المحدَّدة (double/int/ptr/bool) إلى %SadDyn عبر toDyn
                        //      (يشتقّ الوسم من نوع المعامل)؛ وإن كانت أصلًا %SadDyn (إعادة بناء من
                        //      حمولةٍ مُستخرَجة) تُمرَّر كما هي. **لا malloc ولا وسم بتّات** ⇒ يزول
                        //      تسريب الصناديق، ولا تصطدم بِتّة إشارة الـdouble بأيّ وسم. بعد التغليف
                        //      يتطابق النوع مع الخانة فيُتخطّى مسار الوسم القديم أدناه (ميّت لـ%SadDyn).
                        // (EN) === ISSUE-076 (%SadDyn root fix): a dynamic payload slot ===
                        //      Pack the concrete value (double/int/ptr/bool) into %SadDyn via toDyn
                        //      (derives the kind from the operand type); an already-%SadDyn value
                        //      (re-construction from an extracted payload) passes through. NO malloc,
                        //      NO bit-tagging ⇒ the box leak is gone and the double's sign bit never
                        //      collides with a tag. After packing the type matches the slot, so the
                        //      legacy tagging path below is skipped (dead for %SadDyn slots).
                        if (expectedType == getSadDynType(*cg_.context_))
                        {
                            fieldVal = toDyn(cg_, fieldVal, inst->operands[i].dataType);
                        }

                        if (fieldVal->getType() != expectedType)
                        {
                            if (inst->operands[i].dataType == SadTypeKind::Any &&
                                fieldVal->getType()->isIntegerTy() && expectedType->isPointerTy())
                            {
                                // (AR) ISSUE-076/084 (ب″): معامل Any = حمولةٌ موسومةٌ مسبقًا
                                //      (00 مؤشّر · 01 صندوق عشريّ · 10 صحيح · 11 منطقيّ) عائدةٌ من
                                //      استخراجٍ سابق. **لا تُعِد الوسم** (وإلّا صار 01→11 فيُقرأ منطقيًّا
                                //      أو يُشتقّ عنوانٌ فاسد ⇒ SIGSEGV عند إعادة البناء `ن.ق(س)`).
                                //      نُمرّر البتّات كما هي (inttoptr): الصندوق العشريّ يبقى صندوقًا
                                //      صالحًا بوسم 01 في البنية الجديدة، والصحيح يبقى 10 … إلخ.
                                // (EN) ISSUE-076/084 (ب″): an Any operand is an already-tagged payload
                                //      (00 ptr · 01 boxed float · 10 int · 11 bool) from a prior
                                //      extraction. **Do NOT re-tag** (else 01→11 reads as bool, or a
                                //      corrupt address is derived ⇒ SIGSEGV on re-construction `ن.ق(س)`).
                                //      Pass the bits through (inttoptr): a boxed float stays a valid box
                                //      tagged 01 in the new struct, an int stays 10, and so on.
                                fieldVal = cg_.builder_->CreateIntToPtr(fieldVal, expectedType, "any.payload.i2p");
                            }
                            else if (fieldVal->getType()->isIntegerTy() && expectedType->isPointerTy())
                            {
                                // (AR) MSB tagging: الأرقام تُعلّم بـ bit 63 = 1
                                //      val → val | (1 << 63) → inttoptr
                                //      هذا يميّز الأرقام عن المؤشرات (bit 63=0 في userspace دائمًا)
                                //      MSB أفضل من LSB لأن string constants قد تكون بمحاذاة 1 (LSB فردي)
                                // (EN) MSB tagging: integers tagged with bit 63 = 1
                                //      val → val | (1 << 63) → inttoptr
                                //      Distinguishes integers from pointers (bit 63=0 in userspace always)
                                //      MSB better than LSB because string constants may be 1-aligned (odd LSB)
                                llvm::Value *tagged = cg_.builder_->CreateOr(
                                    fieldVal,
                                    llvm::ConstantInt::get(cg_.getInt64Type(), static_cast<uint64_t>(1) << 63),
                                    "tag.msb");
                                fieldVal = cg_.builder_->CreateIntToPtr(tagged, expectedType, "tag.i2p");
                            }
                            else if (fieldVal->getType()->isDoubleTy() && expectedType->isPointerTy())
                            {
                                // (AR) ISSUE-076/082/084 (ب″): حمولة عشريّة — **تُعلَّب**.
                                //      نخصّص صندوق كومة (malloc(8))، نخزّن الـdouble فيه، ثمّ
                                //      نضع مؤشّر الصندوق في الخانة موسومًا بـ01 (kAdtPayloadTagFloat).
                                //      هكذا لا تصطدم بِتّة إشارة الـdouble ببتّات الوسم، والوسم 01
                                //      يميّزه عن النصّ (00) فيمنع طباعة قمامة/انهيار عند القيم
                                //      العشريّة الموجبة الصغيرة (bit63=0). ملاحظة تسريب: الصندوق
                                //      لا يُحرَّر (نفس ملمح تسريب حمولة النصوص القائمة) — مقبول.
                                // (EN) ISSUE-076/082/084 (ب″): decimal payload — **boxed**.
                                //      malloc(8) a heap box, store the double into it, then place
                                //      the box pointer in the slot tagged with 01
                                //      (kAdtPayloadTagFloat). The double's sign bit lives inside the
                                //      box and never collides with the tag; tag 01 distinguishes it
                                //      from a string (00), preventing garbage/segfault on small
                                //      positive floats (bit63=0). Leak note: the box is not freed
                                //      (same leak profile as existing string payloads) — acceptable.
                                auto *i64Ty = cg_.getInt64Type();
                                auto *boxPtrTy = llvm::PointerType::getUnqual(*cg_.context_);
                                auto *boxMallocTy = llvm::FunctionType::get(boxPtrTy, {i64Ty}, false);
                                auto boxMallocFn = cg_.module_->getOrInsertFunction("malloc", boxMallocTy);
                                llvm::Value *box = cg_.builder_->CreateCall(
                                    boxMallocFn, {llvm::ConstantInt::get(i64Ty, 8)}, "float.box");
                                cg_.builder_->CreateStore(fieldVal, box);
                                llvm::Value *boxI64 = cg_.builder_->CreatePtrToInt(box, i64Ty, "float.box.p2i");
                                llvm::Value *tagged = cg_.builder_->CreateOr(
                                    boxI64, llvm::ConstantInt::get(i64Ty, kAdtPayloadTagFloat), "float.box.tag");
                                fieldVal = cg_.builder_->CreateIntToPtr(tagged, expectedType, "float.box.i2p");
                            }
                            else if (fieldVal->getType()->isPointerTy() && expectedType->isIntegerTy())
                            {
                                fieldVal = cg_.builder_->CreatePtrToInt(fieldVal, expectedType);
                            }
                            else if (fieldVal->getType()->isPointerTy() && expectedType->isPointerTy())
                            {
                                // (AR) لا حاجة للتحويل — الأنواع المؤشرية متوافقة (opaque)
                                // (EN) No cast needed — pointer types compatible (opaque)
                            }
                        }
                        cg_.builder_->CreateStore(fieldVal, fieldGEP);
                    }
                }
            }

            // (AR) تخزين النتيجة
            // (EN) Store result
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = objPtr;
                cg_.context_info_.objectClassMap[inst->result->name] = structName;
            }
            return objPtr;
        }

        llvm::Value *EnumOpsCodeGen::emitEnumGetTag(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) استخراج المميّز (tag) من بنية تعداد جبري
            //      المُعاملات: [0]=مؤشر البنية
            //      النتيجة: قيمة i64 (رقم الحالة)
            // (EN) Extract discriminant (tag) from ADT struct
            //      Operands: [0]=struct pointer
            //      Result: i64 value (variant index)

            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_GET_TAG"}});
                return nullptr;
            }

            llvm::Value *enumPtr = cg_.resolveOperand(inst->operands[0]);
            if (!enumPtr)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_GET_TAG"}});
                return nullptr;
            }

            // (AR) التأكد أن القيمة مؤشر — قد تكون i64 إذا مرّت عبر متغير عام
            // (EN) Ensure the value is a pointer — may be i64 if passed through a global variable
            if (!enumPtr->getType()->isPointerTy())
            {
                enumPtr = cg_.builder_->CreateIntToPtr(enumPtr,
                                                   llvm::PointerType::getUnqual(*cg_.context_), "gettag.toptr");
            }

            // (AR) البحث عن نوع البنية من objectClassMap
            // (EN) Look up struct type from objectClassMap
            llvm::StructType *structType = nullptr;
            auto classIt = cg_.context_info_.objectClassMap.find(inst->operands[0].name);
            if (classIt != cg_.context_info_.objectClassMap.end())
            {
                auto typeIt = cg_.context_info_.classStructTypes.find(classIt->second);
                if (typeIt != cg_.context_info_.classStructTypes.end())
                {
                    structType = typeIt->second;
                }
            }

            llvm::Value *tagVal;
            if (structType)
            {
                // (AR) GEP + load للحقل 0
                // (EN) GEP + load for field 0
                llvm::Value *tagGEP = cg_.builder_->CreateStructGEP(structType, enumPtr, 0, "tag.gep");
                tagVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), tagGEP, "tag.val");
            }
            else
            {
                // (AR) إذا لم نعرف النوع — نفترض أن الحقل الأول هو i64
                //      ونستخدم GEP بدون StructType عبر bitcast
                // (EN) If type unknown — assume first field is i64
                //      Use raw pointer arithmetic
                tagVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), enumPtr, "tag.val.raw");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = tagVal;
            }
            return tagVal;
        }

        llvm::Value *EnumOpsCodeGen::emitEnumGetPayload(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) استخراج حقل من حمولة تعداد جبري
            //      المُعاملات: [0]=مؤشر البنية، [1]=فهرس الحقل (i64)
            //      النتيجة: قيمة الحقل (ptr)
            //
            //      الحقول تبدأ من الفهرس 0 في النمط، لكن في البنية
            //      الفهرس الفعلي = fieldIndex + 1 (لأن الحقل 0 هو __tag)
            // (EN) Extract field from ADT payload
            //      Operands: [0]=struct pointer, [1]=field index (i64)
            //      Result: field value (ptr)
            //
            //      Fields start at index 0 in the pattern, but in the struct
            //      actual index = fieldIndex + 1 (because field 0 is __tag)

            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT, {{"detail", "ENUM_GET_PAYLOAD"}});
                return nullptr;
            }

            llvm::Value *enumPtr = cg_.resolveOperand(inst->operands[0]);
            if (!enumPtr)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_GET_PAYLOAD"}});
                return nullptr;
            }

            // (AR) التأكد أن القيمة مؤشر — قد تكون i64 إذا مرّت عبر متغير عام
            // (EN) Ensure the value is a pointer — may be i64 if passed through a global variable
            if (!enumPtr->getType()->isPointerTy())
            {
                enumPtr = cg_.builder_->CreateIntToPtr(enumPtr,
                                                   llvm::PointerType::getUnqual(*cg_.context_), "getpay.toptr");
            }

            int64_t fieldIndex = inst->operands[1].intValue;
            unsigned structFieldIdx = static_cast<unsigned>(fieldIndex + 1); // +1 لتجاوز __tag

            // (AR) البحث عن نوع البنية — نستخدم عدة استراتيجيات:
            //      1. objectClassMap (إذا كان المتغير مسجلاً)
            //      2. operands[2] إضافي (اسم التعداد — إذا أضيف)
            //      3. نستكشف classStructTypes بحثاً عن بنية تحتوي __tag كأول حقل
            // (EN) Look up struct type — using multiple strategies:
            //      1. objectClassMap (if variable is registered)
            //      2. extra operands[2] (enum name — if added)
            //      3. scan classStructTypes for structs with __tag as first field
            llvm::StructType *structType = nullptr;
            auto classIt = cg_.context_info_.objectClassMap.find(inst->operands[0].name);
            if (classIt != cg_.context_info_.objectClassMap.end())
            {
                auto typeIt = cg_.context_info_.classStructTypes.find(classIt->second);
                if (typeIt != cg_.context_info_.classStructTypes.end())
                {
                    structType = typeIt->second;
                }
            }

            // (AR) البديل: إذا لم نجد عبر objectClassMap، نبحث عبر اسم التعداد من operand إضافي
            // (EN) Fallback: if not found via objectClassMap, try extra enum name operand
            if (!structType && inst->operands.size() >= 3 && !inst->operands[2].name.empty())
            {
                auto typeIt = cg_.context_info_.classStructTypes.find(inst->operands[2].name);
                if (typeIt != cg_.context_info_.classStructTypes.end())
                {
                    structType = typeIt->second;
                }
            }

            // (AR) البديل 2: البحث في كل classStructTypes عن بنية ADT مناسبة
            //      (آخر ملاذ عندما لا نعرف الاسم)
            // (EN) Fallback 2: search all classStructTypes for a suitable ADT struct
            //      (last resort when name is unknown)
            if (!structType)
            {
                for (const auto &[cname, ctype] : cg_.context_info_.classStructTypes)
                {
                    if (ctype && ctype->getNumElements() > 1 + fieldIndex)
                    {
                        // (AR) فحص أن الحقل الأول هو i64 (للـ __tag)
                        // (EN) Check that first field is i64 (for __tag)
                        if (ctype->getElementType(0) == cg_.getInt64Type())
                        {
                            structType = ctype;
                            break;
                        }
                    }
                }
            }

            llvm::Value *fieldVal = nullptr;

            // (AR) === [ISSUE-080] وضع التوزيع زمن-التشغيليّ ===
            //      operand[1] == الكاشف ⇒ الوصول النقطيّ المباشر لا يعرف حالة القيمة سكونيًّا.
            //      operand[3]=عدد الأزواج، ثمّ أزواج (وسم الحالة، فهرس الحقل داخلها). نقرأ الوسم
            //      (الحقل 0) ونبني سلسلة اختيار: لكلّ زوجٍ نحمّل خانته (idx+1) ونطبّعها %SadDyn
            //      عبر toDyn ونختار بالمطابقة؛ ونجمع matchedAny. حالةٌ لا تحوي الحقل (لا يطابق
            //      وسمُها أيّ وسمٍ حاوٍ) ⇒ trap حتميّ (خروج ≠0) بدل قمامة صامتة (العلّة ب).
            // (EN) === [ISSUE-080] runtime dispatch mode ===
            //      operand[1] == sentinel ⇒ direct field access whose variant is unknown at compile
            //      time. operand[3]=pair count, then (variant tag, in-variant index) pairs. Read the
            //      tag (field 0) and build a select chain: for each pair load its slot (idx+1),
            //      normalize to %SadDyn via toDyn, and select on the tag match; OR into matchedAny.
            //      A variant lacking the field (no containing tag matched) ⇒ deterministic trap
            //      (exit≠0) instead of silent garbage (cause b).
            if (inst->operands[1].intValue == Sad::Compiler::kAdtFieldDispatchSentinel && structType)
            {
                llvm::Value *tagGEP = cg_.builder_->CreateStructGEP(structType, enumPtr, 0, "disp.tag.gep");
                llvm::Value *tagVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), tagGEP, "disp.tag");
                int64_t nPairs = (inst->operands.size() >= 4) ? inst->operands[3].intValue : 0;
                llvm::Value *matchedAny = llvm::ConstantInt::getFalse(*cg_.context_);
                llvm::Value *selected = nullptr;
                for (int64_t k = 0; k < nPairs; ++k)
                {
                    size_t base = static_cast<size_t>(4 + 2 * k);
                    if (base + 1 >= inst->operands.size())
                        break;
                    int64_t tagK = inst->operands[base].intValue;
                    int64_t idxK = inst->operands[base + 1].intValue;
                    unsigned sfi = static_cast<unsigned>(idxK + 1);
                    if (sfi >= structType->getNumElements())
                        continue;
                    llvm::Value *eq = cg_.builder_->CreateICmpEQ(
                        tagVal, llvm::ConstantInt::get(cg_.getInt64Type(), tagK), "disp.eq");
                    matchedAny = cg_.builder_->CreateOr(matchedAny, eq, "disp.any");
                    llvm::Value *slotGEP = cg_.builder_->CreateStructGEP(structType, enumPtr, sfi, "disp.slot.gep");
                    llvm::Type *slotTy = structType->getElementType(sfi);
                    llvm::Value *loaded = cg_.builder_->CreateLoad(slotTy, slotGEP, "disp.slot.val");
                    // (AR) طبّع لـ%SadDyn لتوحيد نوع فرعَي الاختيار (الخانات كلّها %SadDyn بعد
                    //      ISSUE-076 فتمرّ كما هي؛ ويحمي هذا حال خانةٍ محسوسة نادرة).
                    // (EN) Normalize to %SadDyn so select branches share a type (all slots are
                    //      %SadDyn after ISSUE-076, passed through; guards a rare concrete slot too).
                    llvm::Value *loadedDyn = toDyn(cg_, loaded, SadTypeKind::Any);
                    selected = selected ? cg_.builder_->CreateSelect(eq, loadedDyn, selected, "disp.sel")
                                        : loadedDyn;
                }

                // (AR) كتلة الـ trap عند حالةٍ لا تحوي الحقل / (EN) trap block on wrong variant
                llvm::Function *parentFn = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *trapBB = llvm::BasicBlock::Create(*cg_.context_, "adt.field.wrongvar", parentFn);
                llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, "adt.field.ok", parentFn);
                cg_.builder_->CreateCondBr(matchedAny, contBB, trapBB);

                cg_.builder_->SetInsertPoint(trapBB);
                {
                    auto ptrTyTrap = llvm::PointerType::getUnqual(*cg_.context_);
                    auto *printfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*cg_.context_), {ptrTyTrap}, true);
                    auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
                    llvm::Value *fmtStr = cg_.builder_->CreateGlobalStringPtr(
                        Sad::Compiler::kAdtWrongVariantFieldMsg, "adt.wrongvar.fmt");
                    llvm::Value *enumNameStr = cg_.builder_->CreateGlobalStringPtr(
                        inst->operands[2].name, "adt.wrongvar.enum");
                    cg_.builder_->CreateCall(printfFunc, {fmtStr, enumNameStr});
                    if (cg_.freestanding_)
                    {
                        // (AR) وضع حرّ: __sad_panic بدل exit (weak، النواة تتجاوزه)
                        // (EN) Freestanding: __sad_panic instead of exit (weak)
                        auto *panicType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt64Ty(*cg_.context_)}, false);
                        auto panicFunc = cg_.module_->getOrInsertFunction("__sad_panic", panicType);
                        cg_.builder_->CreateCall(panicFunc,
                                                 {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1)});
                    }
                    else
                    {
                        auto *exitType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt32Ty(*cg_.context_)}, false);
                        auto exitFunc = cg_.module_->getOrInsertFunction("exit", exitType);
                        cg_.builder_->CreateCall(exitFunc,
                                                 {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 1)});
                    }
                    cg_.builder_->CreateUnreachable();
                }
                cg_.builder_->SetInsertPoint(contBB);

                fieldVal = selected ? selected
                                    : llvm::Constant::getNullValue(getSadDynType(*cg_.context_));
            }

            if (!fieldVal && structType && structFieldIdx < structType->getNumElements())
            {
                // (AR) GEP + load للحقل المطلوب
                // (EN) GEP + load for requested field
                llvm::Value *fieldGEP = cg_.builder_->CreateStructGEP(
                    structType, enumPtr, structFieldIdx,
                    "payload." + std::to_string(fieldIndex) + ".gep");
                llvm::Type *fieldType = structType->getElementType(structFieldIdx);
                fieldVal = cg_.builder_->CreateLoad(fieldType, fieldGEP,
                                                "payload." + std::to_string(fieldIndex) + ".val");
            }
            else if (!fieldVal)
            {
                // (AR) نوع غير معروف — نفترض ptr
                //      نحسب الإزاحة يدوياً: offset = (fieldIndex + 1) * 8
                // (EN) Type unknown — assume ptr
                //      Calculate offset manually: offset = (fieldIndex + 1) * 8
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                llvm::Value *offset = llvm::ConstantInt::get(cg_.getInt64Type(), (fieldIndex + 1) * 8);
                llvm::Value *rawGEP = cg_.builder_->CreateGEP(
                    llvm::Type::getInt8Ty(*cg_.context_), enumPtr, offset,
                    "payload." + std::to_string(fieldIndex) + ".raw");
                fieldVal = cg_.builder_->CreateLoad(ptrTy, rawGEP,
                                                "payload." + std::to_string(fieldIndex) + ".val");
            }

            if (inst->result.has_value())
            {
                // (AR) === ISSUE-076 (حلّ %SadDyn الجذريّ): خانة الحمولة نوعُها %SadDyn ===
                //      إن كانت القيمة المحمَّلة %SadDyn (الحال المعتاد لحمولة ADT بعد الترحيل):
                //        - نوع النتيجة محدَّد (Float/Integer/…) ⇒ فكٌّ أصيل (مسار سريع نفس-النطاق؛
                //          كلّ مستهلكي النوع المحدَّد القائمين يعملون بلا تغيير)؛
                //        - نوع النتيجة Any/مجهول ⇒ تبقى %SadDyn (تعبر الدوال والأذرع كقيمةٍ من
                //          الدرجة الأولى، ويوزّع عليها المستهلكون عبر الوسم — لا فكَّ يدويًّا صامتًا).
                // (EN) === ISSUE-076 (%SadDyn root fix): the payload slot is %SadDyn ===
                //      If the loaded value is %SadDyn (the usual case for a migrated ADT payload):
                //        - concrete result type (Float/Integer/…) ⇒ native unpack (same-scope fast
                //          path; every existing concrete-typed consumer works unchanged);
                //        - Any/unknown result type ⇒ stays %SadDyn (crosses functions/arms as a
                //          first-class value; consumers dispatch on the tag — no silent manual unwrap).
                if (isSadDyn(fieldVal))
                {
                    llvm::Value *out = fieldVal;
                    switch (inst->result->dataType)
                    {
                    case SadTypeKind::Float:
                        out = unpackDouble(cg_, fieldVal);
                        break;
                    case SadTypeKind::Integer:
                        out = dynPayloadI64(cg_, fieldVal);
                        break;
                    case SadTypeKind::Boolean:
                        out = cg_.builder_->CreateTrunc(
                            dynPayloadI64(cg_, fieldVal), cg_.getInt1Type(), inst->result->name + ".bool");
                        break;
                    case SadTypeKind::String:
                    case SadTypeKind::Pointer:
                        out = unpackPtr(cg_, fieldVal);
                        break;
                    default:
                        out = fieldVal; // (AR) Any ⇒ يبقى %SadDyn / (EN) Any ⇒ stays %SadDyn
                        break;
                    }
                    cg_.context_info_.namedValues[inst->result->name] = out;
                    return out;
                }

                // (AR) === مسار احتياطيّ قديم (خانة غير %SadDyn — بنية غير مُسجَّلة، نادر) ===
                // (EN) === legacy fallback path (non-%SadDyn slot — unregistered struct, rare) ===
                // (AR) تحويل القيمة إلى i64 إذا كانت ptr
                // (EN) Convert value to i64 if ptr
                if (fieldVal->getType()->isPointerTy())
                {
                    fieldVal = cg_.builder_->CreatePtrToInt(fieldVal, cg_.getInt64Type(),
                                                        "payload." + std::to_string(fieldIndex) + ".toi64");
                }

                auto *i64Ty = cg_.getInt64Type();
                const std::string &resName = inst->result->name;

                // (AR) === ISSUE-076/082/084 (ب″): مسار سريع ساكن للحمولة العشريّة المعروفة ===
                //      النوع مُستنتَج Float (من موقع إنشاءٍ سُجِّل، نفس النطاق) ⇒ نفكّ التعليب:
                //      نمسح البتّة 62 لاستعادة مؤشّر الصندوق، ثمّ نحمّل الـdouble الحقيقيّ. هكذا
                //      تعمل كلّ مستهلكي double القائمين (حساب/مقارنة/طباعة) على قيمة أصيلة.
                // (EN) === ISSUE-076/082/084 (ب″): static fast-path for a type-known float payload ===
                //      Type inferred Float (from a registered same-scope construction site) ⇒ unbox:
                //      clear bit62 to recover the box pointer, then load the real double. Every
                //      existing double consumer (arith/compare/print) then works on a native value.
                if (inst->result->dataType == SadTypeKind::Float)
                {
                    llvm::Value *boxI64 = cg_.builder_->CreateAnd(
                        fieldVal, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit62),
                        "payload." + std::to_string(fieldIndex) + ".float.box.clear");
                    llvm::Value *boxPtr = cg_.builder_->CreateIntToPtr(
                        boxI64, llvm::PointerType::getUnqual(*cg_.context_),
                        "payload." + std::to_string(fieldIndex) + ".float.box.ptr");
                    llvm::Value *asDouble = cg_.builder_->CreateLoad(
                        llvm::Type::getDoubleTy(*cg_.context_), boxPtr,
                        "payload." + std::to_string(fieldIndex) + ".float.box.load");
                    cg_.context_info_.namedValues[resName] = asDouble;
                    // (AR) أعلام: قيمة عشريّة أصيلة (ليست مؤشّرًا)
                    // (EN) Flags: a native decimal value (not a pointer)
                    cg_.context_info_.namedValues[resName + ".__is_ptr"] =
                        llvm::ConstantInt::getFalse(*cg_.context_);
                    cg_.context_info_.namedValues[resName + ".__is_float"] =
                        llvm::ConstantInt::getTrue(*cg_.context_);
                    return asDouble;
                }

                // (AR) === تصنيف زمن-التشغيل رباعيّ الاتّجاه على بتّتي الوسم (63،62) ===
                //      يُستخدم للأنواع غير-العشريّة الساكنة (صحيح/نصّ/منطقيّ) وللنوع الديناميّ
                //      Any (تراجُع Unknown — إحالة أماميّة/تعارُض). نحسب bit63/bit62 ونضبط
                //      العلَمين اللذين يعتمد عليهما المستهلكون داخل النطاق:
                //        __is_ptr   = (00) نصّ/مؤشّر    __is_float = (01) صندوق عشريّ
                // (EN) === 4-way runtime classification on the two tag bits (63,62) ===
                //      Used for static non-float types (int/string/bool) and for the dynamic Any
                //      type (Unknown fallback — forward-ref/conflict). Compute bit63/bit62 and set
                //      the runtime flags in-scope consumers rely on:
                //        __is_ptr   = (00) string/pointer   __is_float = (01) boxed float
                llvm::Value *bit63 = cg_.builder_->CreateAnd(
                    fieldVal, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit63), "tag.bit63");
                llvm::Value *bit62 = cg_.builder_->CreateAnd(
                    fieldVal, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62), "tag.bit62");
                llvm::Value *isInt = cg_.builder_->CreateICmpNE(
                    bit63, llvm::ConstantInt::get(i64Ty, 0), "is.tagged.hi"); // bit63 set ⇒ int(10)/bool(11)
                llvm::Value *bit62Set = cg_.builder_->CreateICmpNE(
                    bit62, llvm::ConstantInt::get(i64Ty, 0), "is.tagged.lo");
                llvm::Value *isFloatBox = cg_.builder_->CreateAnd(
                    cg_.builder_->CreateNot(isInt, "not.hi"), bit62Set, "is.float.box"); // 01
                llvm::Value *isPtr = cg_.builder_->CreateAnd(
                    cg_.builder_->CreateNot(isInt, "not.hi2"),
                    cg_.builder_->CreateNot(bit62Set, "not.lo"), "is.ptr"); // 00

                if (inst->result->dataType == SadTypeKind::Any)
                {
                    // (AR) الديناميّ (تراجُع Unknown): نُبقي القيمة موسومةً كما حُمِّلت — فهي
                    //      التمثيل الأساسيّ الذي يعبر حدود الدوال/الإرجاع حيث تضيع الأعلام،
                    //      فيفكّ المستهلك الوسم زمنَ التشغيل. الأعلام تخدم المستهلكين داخل النطاق.
                    // (EN) Dynamic (Unknown fallback): keep the value tagged as loaded — it is the
                    //      canonical representation that crosses function/return boundaries (where
                    //      flags are lost) so the consumer decodes the tag at runtime. The flags
                    //      serve in-scope consumers.
                    cg_.context_info_.namedValues[resName] = fieldVal;
                    cg_.context_info_.namedValues[resName + ".__is_ptr"] = isPtr;
                    cg_.context_info_.namedValues[resName + ".__is_float"] = isFloatBox;
                    return fieldVal;
                }

                // (AR) الأنواع الساكنة غير-العشريّة: صحيح ⇒ فكّ وسم bit63 (قيمة نظيفة للحساب)؛
                //      نصّ/مؤشّر ⇒ يبقى كما هو. منطقيّ يبقي bit62 ليميّزه المستهلك. (المسار القديم)
                // (EN) Static non-float types: integer ⇒ clear bit63 (clean value for arithmetic);
                //      string/pointer ⇒ stays as-is. Bool keeps bit62 for the consumer. (legacy path)
                llvm::Value *untagged = cg_.builder_->CreateAnd(
                    fieldVal, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit63), "untagged");
                llvm::Value *result = cg_.builder_->CreateSelect(
                    isInt, untagged, fieldVal, "payload.untagged");

                cg_.context_info_.namedValues[resName] = result;
                cg_.context_info_.namedValues[resName + ".__is_ptr"] = isPtr;
                // (AR) لا صندوق عشريّ في المسار الساكن غير-العشريّ ⇒ العلَم false صراحةً
                // (EN) No float box on the static non-float path ⇒ flag explicitly false
                cg_.context_info_.namedValues[resName + ".__is_float"] =
                    llvm::ConstantInt::getFalse(*cg_.context_);
            }
            return fieldVal;
        }

        llvm::Value *EnumOpsCodeGen::emitEnumIsVariant(std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) فحص ما إذا كان التعداد في حالة معيّنة
            //      المُعاملات:
            //        [0] = قيمة التعداد (مؤشر أو i64)
            //        [1] = المميّز المتوقع (i64)
            //        [2] = اسم التعداد (string) — اختياري، للبحث عن structType
            //        [3] = هل الحالة المُفحوصة وحدية (1=unit, 0=data) — اختياري
            //      النتيجة: i1 (true إذا تطابق المميّز)
            //
            //      الاستراتيجية:
            //      - إذا كانت الحالة المُفحوصة unit variant:
            //        القيمة قد تكون data variant (مؤشر) أو unit variant (tag مباشر)
            //        → نحاول قراءة tag من المؤشر، وإذا كان tag != expected → نقارن مباشرة
            //        ⚠ لكن القيمة قد تكون tag مباشر (unit) → IntToPtr يعطي عنوان غير صالح!
            //        → الحل: دائماً نفحص المؤشر أولاً (IntToPtr + load tag) ونقارن.
            //          لأن ADT الحقيقي (حتى unit variants مع بانيات) يُخزن كمؤشر لبنية.
            //
            //      - المشكلة الأساسية: القيمة المُمررة كمعامل لدالة هي i64.
            //        objectClassMap لا ينتقل عبر حدود الدوال.
            //        → نستخدم operands[2] (اسم التعداد) و operands[3] (isUnit) كمعلومات بديلة.
            //
            // (EN) Check if an ADT enum is a specific variant
            //      Operands:
            //        [0] = enum value (pointer or i64)
            //        [1] = expected tag (i64)
            //        [2] = enum name (string) — optional, for structType lookup
            //        [3] = is checked variant a unit? (1=unit, 0=data) — optional
            //      Result: i1 (true if tag matches)
            //
            //      Strategy:
            //      - If ADT has a constructor that allocates structs, ALL values
            //        (including unit variants) are stored as heap pointers.
            //      - We use operands[2] and [3] for metadata instead of objectClassMap
            //        because objectClassMap doesn't propagate across function boundaries.
            // ================================================================

            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_IS_VARIANT"}});
                return nullptr;
            }

            llvm::Value *enumVal = cg_.resolveOperand(inst->operands[0]);
            if (!enumVal)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_IS_VARIANT"}});
                return nullptr;
            }

            int64_t expectedTag = inst->operands[1].intValue;

            // (AR) استخراج المعلومات الإضافية من SIR (إذا توفرت)
            // (EN) Extract extra metadata from SIR (if available)
            std::string enumName;
            bool checkedVariantIsUnit = false;
            if (inst->operands.size() >= 3)
            {
                enumName = inst->operands[2].name;
            }
            if (inst->operands.size() >= 4)
            {
                checkedVariantIsUnit = (inst->operands[3].intValue != 0);
            }

            // ================================================================
            // (AR) تحديد ما إذا كانت القيمة مؤشراً لبنية ADT أم قيمة tag مباشرة
            //
            //      الأولوية:
            //      1. objectClassMap (إذا كان المتغير مسجلاً)
            //      2. اسم التعداد من SIR metadata → البحث في classStructTypes
            //      3. SIR type (STRUCT أو PTR)
            //      4. افتراضي: إذا كان التعداد معروفاً → data variant (لأن حتى unit
            //         variants يتم إنشاؤها عبر ENUM_CONSTRUCT كبنى مخصصة)
            //
            // (EN) Determine if value is an ADT struct pointer or a direct tag value
            //      Priority:
            //      1. objectClassMap (if variable is registered)
            //      2. enum name from SIR metadata → search classStructTypes
            //      3. SIR type (STRUCT or PTR)
            //      4. default: if enum is known → data variant (because even unit
            //         variants are created via ENUM_CONSTRUCT as heap structs)
            // ================================================================
            bool isDataVariant = false;
            llvm::StructType *structType = nullptr;

            // (AR) المحاولة 1: objectClassMap
            // (EN) Attempt 1: objectClassMap
            auto classIt = cg_.context_info_.objectClassMap.find(inst->operands[0].name);
            if (classIt != cg_.context_info_.objectClassMap.end())
            {
                isDataVariant = true;
                auto typeIt = cg_.context_info_.classStructTypes.find(classIt->second);
                if (typeIt != cg_.context_info_.classStructTypes.end())
                {
                    structType = typeIt->second;
                }
            }

            // (AR) المحاولة 2: اسم التعداد من SIR metadata
            // (EN) Attempt 2: enum name from SIR metadata
            if (!isDataVariant && !enumName.empty())
            {
                auto typeIt = cg_.context_info_.classStructTypes.find(enumName);
                if (typeIt != cg_.context_info_.classStructTypes.end())
                {
                    structType = typeIt->second;
                    isDataVariant = true;
                }
            }

            // (AR) المحاولة 3: نوع SIR
            // (EN) Attempt 3: SIR type
            if (!isDataVariant && (inst->operands[0].dataType == SadTypeKind::Struct ||
                                   inst->operands[0].dataType == SadTypeKind::Pointer))
            {
                isDataVariant = true;
            }

            // (AR) المحاولة 4: إذا كان التعداد معروفاً بأي شكل → افترض data variant
            //      لأن unit variants في ADT يتم إنشاؤها أيضاً كبنى مخصصة
            //      (ENUM_CONSTRUCT بدون حقول ← بنية بـ tag فقط)
            // (EN) Attempt 4: if enum is known at all → assume data variant
            //      because unit variants in ADT are also created as heap structs
            //      (ENUM_CONSTRUCT with no fields ← struct with just tag)
            if (!isDataVariant && !enumName.empty())
            {
                isDataVariant = true;
            }

            // ================================================================
            // (AR) تنفيذ المقارنة بناءً على نوع القيمة
            // (EN) Execute comparison based on value type
            // ================================================================
            if (!enumVal->getType()->isPointerTy() && !isDataVariant)
            {
                // (AR) unit variant حقيقي: القيمة هي المميّز نفسه — مقارنة مباشرة
                //      هذا يحدث فقط مع تعدادات بسيطة (C-style) بدون أي data variant
                // (EN) True unit variant: value IS the tag — compare directly
                //      Only happens with simple (C-style) enums without any data variants
                llvm::Value *cmpResult = cg_.builder_->CreateICmpEQ(
                    enumVal,
                    llvm::ConstantInt::get(cg_.getInt64Type(), expectedTag),
                    "isvar.cmp.unit");
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = cmpResult;
                }
                return cmpResult;
            }

            // (AR) data variant أو ADT بشكل عام: نحتاج تحويل i64 → ptr ثم قراءة tag
            // (EN) data variant or ADT in general: need i64 → ptr conversion then read tag
            llvm::Value *enumPtr = enumVal;
            if (!enumPtr->getType()->isPointerTy())
            {
                enumPtr = cg_.builder_->CreateIntToPtr(enumPtr,
                                                   llvm::PointerType::getUnqual(*cg_.context_), "isvar.toptr");
            }

            // (AR) قراءة المميّز من البنية
            // (EN) Read tag from struct
            llvm::Value *tagVal;
            if (structType)
            {
                llvm::Value *tagGEP = cg_.builder_->CreateStructGEP(structType, enumPtr, 0, "isvar.tag.gep");
                tagVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), tagGEP, "isvar.tag");
            }
            else
            {
                // (AR) بدون structType: نحمّل مباشرة كـ i64 (أول عنصر في البنية)
                // (EN) Without structType: load directly as i64 (first element in struct)
                tagVal = cg_.builder_->CreateLoad(cg_.getInt64Type(), enumPtr, "isvar.tag.raw");
            }

            // (AR) المقارنة: tag == expected_tag
            // (EN) Comparison: tag == expected_tag
            llvm::Value *cmpResult = cg_.builder_->CreateICmpEQ(
                tagVal,
                llvm::ConstantInt::get(cg_.getInt64Type(), expectedTag),
                "isvar.cmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = cmpResult;
            }
            return cmpResult;
        }

        // ============================================================================
        // emitEnumFree - تحرير ذاكرة قيمة تعداد جبري
        // ============================================================================
        // (AR) تحرير الذاكرة المخصصة لقيمة ADT عبر free()
        //      المُعاملات: [0]=مؤشر البنية
        //      لا يوجد نتيجة — عملية void
        //
        //      ملاحظة: يتحقق أن المؤشر ليس null وأنه ptr (وليس i64)
        //      لتجنب تحرير حالات Unit التي قد تكون ثوابت i64
        //
        // (EN) Free heap-allocated ADT value via free()
        //      Operands: [0]=struct pointer
        //      No result — void operation
        //
        //      Note: checks for null and that it's a ptr (not i64)
        //      to avoid freeing Unit variants that may be i64 constants
        // ============================================================================
        llvm::Value *EnumOpsCodeGen::emitEnumFree(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_FREE"}});
                return nullptr;
            }

            llvm::Value *enumPtr = cg_.resolveOperand(inst->operands[0]);
            if (!enumPtr)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ENUM_FREE"}});
                return nullptr;
            }

            // (AR) التأكد أن القيمة مؤشر — إذا كانت i64 فهي Unit variant ولا حاجة لتحريرها
            // (EN) Ensure value is a pointer — if i64 it's a Unit variant and doesn't need freeing
            if (!enumPtr->getType()->isPointerTy())
            {
                return nullptr;
            }

            // (AR) فحص null قبل التحرير
            // (EN) Null check before free
            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                enumPtr,
                llvm::ConstantPointerNull::get(
                    llvm::PointerType::getUnqual(*cg_.context_)),
                "enum.isnull");

            llvm::Function *currentFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *freeBlock = llvm::BasicBlock::Create(
                *cg_.context_, "enum.free", currentFunc);
            llvm::BasicBlock *contBlock = llvm::BasicBlock::Create(
                *cg_.context_, "enum.free.cont", currentFunc);

            cg_.builder_->CreateCondBr(isNull, contBlock, freeBlock);

            // (AR) كتلة التحرير
            // (EN) Free block
            cg_.builder_->SetInsertPoint(freeBlock);
            auto freeType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto freeFunc = cg_.module_->getOrInsertFunction("free", freeType);
            cg_.builder_->CreateCall(freeFunc, {enumPtr});
            cg_.builder_->CreateBr(contBlock);

            // (AR) الاستمرار بعد التحرير
            // (EN) Continue after free
            cg_.builder_->SetInsertPoint(contBlock);

            return nullptr;
        }

    } // namespace LLVM
} // namespace Sad
