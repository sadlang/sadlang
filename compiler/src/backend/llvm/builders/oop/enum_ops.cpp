/*
 * ============================================================================
 * LLVM IR Code Generator - Enum/ADT Operations
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/oop/enum_ops_codegen.h"
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
                //      حقل 0 = i64 (tag)، الباقي = ptr
                // (EN) If struct not registered — create anonymous type based on field count
                //      field 0 = i64 (tag), rest = ptr
                std::vector<llvm::Type *> fieldTypes;
                fieldTypes.push_back(cg_.getInt64Type()); // __tag
                for (size_t i = 2; i < inst->operands.size(); ++i)
                {
                    fieldTypes.push_back(llvm::PointerType::getUnqual(*cg_.context_));
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
                        if (fieldVal->getType() != expectedType)
                        {
                            if (fieldVal->getType()->isIntegerTy() && expectedType->isPointerTy())
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

            llvm::Value *fieldVal;
            if (structType && structFieldIdx < structType->getNumElements())
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
            else
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
                // (AR) تحويل القيمة إلى i64 إذا كانت ptr
                // (EN) Convert value to i64 if ptr
                if (fieldVal->getType()->isPointerTy())
                {
                    fieldVal = cg_.builder_->CreatePtrToInt(fieldVal, cg_.getInt64Type(),
                                                        "payload." + std::to_string(fieldIndex) + ".toi64");
                }

                // (AR) === MSB pointer tagging: فك التعليم ===
                //      الأرقام المُخزنة في ADT تُعلّم بـ bit 63=1 عند ENUM_CONSTRUCT
                //      المؤشرات (نصوص) bit 63=0 (في userspace دائمًا)
                //      هنا نفحص bit 63 ونعمل untag للأرقام ونحفظ flag للنصوص
                // (EN) === MSB pointer tagging: untag ===
                //      Integers stored in ADT are tagged with bit 63=1 at ENUM_CONSTRUCT
                //      Pointers (strings) have bit 63=0 (always in userspace)
                //      Here we check bit 63, untag integers, and save flag for strings
                llvm::Value *msbMask = llvm::ConstantInt::get(cg_.getInt64Type(), static_cast<uint64_t>(1) << 63);
                llvm::Value *msbBit = cg_.builder_->CreateAnd(fieldVal, msbMask, "tag.msb.bit");
                llvm::Value *isInt = cg_.builder_->CreateICmpNE(
                    msbBit, llvm::ConstantInt::get(cg_.getInt64Type(), 0), "is.tagged.int");
                // (AR) فك التعليم: إذا رقم (MSB=1) → val & ~(1<<63) = إزالة العلامة
                //      إذا مؤشر (MSB=0) → يبقى كما هو
                // (EN) Untag: if integer (MSB=1) → val & ~(1<<63) = clear tag
                //      if pointer (MSB=0) → stays as-is
                llvm::Value *untagged = cg_.builder_->CreateAnd(
                    fieldVal,
                    llvm::ConstantInt::get(cg_.getInt64Type(), ~(static_cast<uint64_t>(1) << 63)),
                    "untagged");
                llvm::Value *result = cg_.builder_->CreateSelect(
                    isInt, untagged, fieldVal, "payload.untagged");

                cg_.context_info_.namedValues[inst->result->name] = result;

                // (AR) حفظ flag: هل القيمة مؤشر (نص/كائن)؟
                // (EN) Save flag: is value a pointer (string/object)?
                llvm::Value *isPtr = cg_.builder_->CreateNot(isInt, "is.ptr");
                cg_.context_info_.namedValues[inst->result->name + ".__is_ptr"] = isPtr;
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
