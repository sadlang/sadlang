/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 *
 * هذا الملف يحتوي على تنفيذ مولد كود LLVM IR الذي يترجم SIR إلى LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 *
 * التزام ب STRICT_CODING_RULES.md:
 * - تم قراءة جميع الملفات الرأسية بالكامل (1746 سطر)
 * - تم توثيق كل استخدام API بموقع المصدر
 * - لا توجد دوال أو متغيرات مخترعة
 *
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2024
 * الإصدار (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/collections/strings_codegen.h"
#include "sir_constants.h"   // (AR) kSadNullSentinel لوعي نوع() بـعدم زمن التشغيل
#include "sad_dyn_repr.h"     // (AR) ISSUE-076: نوع(%SadDyn) عبر الموزِّع dynTypeName
#include "adt_payload_tags.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h بدلاً من llvm/Support/TargetRegistry.h
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

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        // Helper function for array struct type
        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            static llvm::StructType *arrTy = nullptr;
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(ctx, {
                                                          llvm::Type::getInt64Ty(ctx),       // length
                                                          llvm::Type::getInt64Ty(ctx),       // capacity
                                                          llvm::PointerType::getUnqual(ctx), // data pointer
                                                          llvm::PointerType::getUnqual(ctx), // tags (i8*) or null [option A]
                                                          llvm::Type::getInt8Ty(ctx)         // homogKind (option A2): DynKind of a homogeneous array; read only when tags==null
                                                      },
                                                 "SadArray");
            }
            return arrTy;
        }

        llvm::Value *StringsCodeGen::emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                return cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Unknown), "typeof_unknown");
            }

            // (AR) اسم النوع من المصدر الموحَّد (types.yaml) عبر الدالة المولَّدة
            //      sadTypeKindArabicName — نفس مصدر المفسّر، فيتطابق نوع() بين المحرّكين
            //      (لا «عدد_صحيح» في المترجم مقابل «رقم» في المفسّر). [توحيد أسماء نوع()]
            // (EN) Type name from the unified SoT (types.yaml) via the generated
            //      sadTypeKindArabicName — same source as the interpreter, so نوع() matches
            //      across engines (no «عدد_صحيح» vs «رقم» divergence).
            // (AR) [طبقة طبيعي64] طبيعي64/بايت نوعان **سطحيّان** يُخزَّنان int64 زمن التشغيل
            //      (Option B، kind=رقم)؛ والمفسّر يعيد لهما «رقم» في نوع(). فنُطبّع الاسم إلى
            //      رقم (Integer) هنا كي يتطابق المساران — بعد أن صيّرت الطبقةُ النوعَ الساكن
            //      UInt64 يصل معامل نوع() (كان Integer قبلها فيطابق تلقائيًّا).
            // (EN) [طبيعي64 layer] طبيعي64/Byte are SURFACE types stored as int64 at runtime
            //      (Option B, kind=رقم); the interpreter's نوع() returns «رقم» for them. Normalize
            //      the name to Integer here so both tracks match — now that the layer propagates
            //      the UInt64/Byte static type to نوع()'s operand (it was Integer before, matching
            //      by default).
            SadTypeKind typeNameKind = inst->operands[0].dataType;
            if (typeNameKind == SadTypeKind::UInt64 || typeNameKind == SadTypeKind::Byte)
                typeNameKind = SadTypeKind::Integer;
            // (AR) ومثلُه التعدادُ الجبريُّ (ISSUE-153): صارت قيمتُه تحملُ `Enum`
            //      ساكنًا ليصدُقَ وسمُها في `%SadDyn`، فلو تُرِكَ لأجابَ `نوع()` «تعداد»
            //      على المسارِ الساكنِ و«خريطة» على الموسوم — جوابانِ لسؤالٍ واحد.
            //      ⚠️ والتعدادُ **البسيط** لا يتأثّر: قُِيسَ أنّ قيمتَه تصلُ `Integer`
            //      ويُجيبُ عنها المحرّكانِ «رقم» — فلا يمرُّ بهذه الذراعِ أصلًا.
            // (EN) Same for an ADT (ISSUE-153): its value now carries a static `Enum` so its
            //      %SadDyn tag can tell the truth; left alone, نوع() would answer «enum» on the
            //      static path and «map» on the tagged one — two answers to one question.
            //      A PLAIN enum is unaffected: measured, its value arrives as Integer and both
            //      engines answer «number», so it never reaches this arm.
            if (typeNameKind == SadTypeKind::Enum)
                typeNameKind = SadTypeKind::Map;
            const char *typeName = ::Sad::Types::sadTypeKindArabicName(typeNameKind);

            llvm::Value *staticStr = cg_.builder_->CreateGlobalStringPtr(typeName, "typeof_str");

            // ================================================================
            // (AR) [S-TS-P4 codegen] وعي زمن-التشغيل بـعدم (Null):
            //      قيمة i64 قد تحمل حارس Null (kSadNullSentinel) حتى لو كان نوعها
            //      الساكن «عدد_صحيح» — مثل متغير اختياري `رقم؟` أُسنِد إليه `لاشيء`.
            //      نُصدر اختيارًا زمن-التشغيل: (القيمة == الحارس) ? «عدم» : الاسم الساكن.
            //      الأنواع غير i64 (عشري/نص/مصفوفة/منطقي) لا يمكن أن تحمل الحارس → تتخطّى.
            // (EN) [S-TS-P4 codegen] Runtime null awareness: an i64 value may carry the
            //      Null sentinel even when its static type is «عدد_صحيح» (e.g. an optional
            //      `رقم؟` assigned `لاشيء`). Emit a runtime select: (val == sentinel)
            //      ? «عدم» : static-name. Non-i64 types can't hold the sentinel → skipped.
            // ================================================================
            llvm::Value *result = staticStr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);

            // (AR) === ISSUE-076 (حلّ %SadDyn الجذريّ): قيمةٌ ديناميّة ===
            //      نوعُها الساكن Any ⇒ "أي"، لكنّ القيمة تحمل وسمَ نوعها الحقيقيّ زمنَ التشغيل.
            //      نوزّع عبر dynTypeName فيعيد الاسم الصحيح (رقم/عشري/منطقي/نص) مطابقًا للمفسّر.
            // (EN) === ISSUE-076 (%SadDyn root fix): a dynamic value ===
            //      Its static type is Any ⇒ "أي", but the value carries its real kind tag at
            //      runtime. Dispatch via dynTypeName to return the correct name matching نوع().
            if (isSadDyn(val))
            {
                result = dynTypeName(cg_, val);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }
            auto *i64Ty = cg_.getInt64Type();
            // (AR) القيمة قد تكون i64 مباشرة، أو مؤشّرًا يحمل بِتّات الحارس (اختياريّ
            //      ذو نوع داخليّ مرجعيّ مثل `نص؟` أُسنِد إليه `لاشيء` — NS-06 موجة 3).
            //      في الحالتين نحوّلها إلى i64 ونقارنها بالحارس.
            // (EN) The value may be a raw i64, or a pointer carrying the sentinel bit
            //      pattern (optional with a reference inner type like `نص؟` set to `لاشيء`).
            //      In both cases coerce to i64 and compare against the sentinel.
            llvm::Value *asI64 = nullptr;
            if (val && val->getType()->isIntegerTy(64))
                asI64 = val;
            else if (val && val->getType()->isPointerTy())
                asI64 = cg_.builder_->CreatePtrToInt(val, i64Ty, "typeof.p2i");
            // (AR) [إصلاح تصادم kSadNullSentinel] طبيعي64/بايت لا يكونان نوعَ العدم أبدًا
            //      (العدمُ يُخزَّن ثابتَ i64 نوعُه Integer)؛ فلا نطبّق فحص الحارس عليهما هنا:
            //      طبيعي64 شرعيّ = 2^63+1 يساوي الحارس ⇒ كان `نوع()` يُخرج «عدم» بدل «رقم».
            //      Integer مُستثنى (يتصادم جوهريًّا: `رقم؟=لاشيء` عدمُه Integer، لا يُميَّز).
            // (EN) [kSadNullSentinel collision fix] طبيعي64/Byte are never the null type (null is
            //      stored as an Integer-typed i64 constant), so the sentinel check is not applied
            //      to them here: a legitimate طبيعي64 = 2^63+1 equals the sentinel ⇒ `نوع()` used
            //      to return «عدم» instead of «رقم». Integer is excluded (it collides
            //      intrinsically: `رقم؟=لاشيء`'s null is Integer-typed, indistinguishable).
            const bool typeofNonNullableNum =
                inst->operands[0].dataType == SadTypeKind::UInt64 ||
                inst->operands[0].dataType == SadTypeKind::Byte;
            if (asI64 && !typeofNonNullableNum)
            {
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    asI64, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), "typeof.isnull");
                llvm::Value *nullStr = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Null), "typeof_null");
                result = cg_.builder_->CreateSelect(isNull, nullStr, staticStr, "typeof.sel");
            }

            // ================================================================
            // (AR) ISSUE-076/084: حمولة ADT عشريّة مجهولة النوع سكونيًّا (Any، إحالة أماميّة)
            //      تحمل علَم __is_float من الاستخراج (نفس النطاق). عند وجوده نختار زمنَ التشغيل
            //      اسم «عشري» بدل الاسم الساكن (Any) ⇒ يطابق نوع() في المفسّر. غياب العلَم ⇒
            //      لا تغيير (القيم غير-ADT لا تحمله).
            // (EN) ISSUE-076/084: a statically-unknown (Any, forward-ref) float ADT payload
            //      carries the __is_float flag from extraction (same scope). When present, pick
            //      the «عشري» name at runtime instead of the static (Any) name ⇒ matches the
            //      interpreter's نوع(). Flag absent ⇒ no change (non-ADT values don't carry it).
            auto isFloatIt = cg_.context_info_.namedValues.find(inst->operands[0].name + ".__is_float");
            if (isFloatIt != cg_.context_info_.namedValues.end())
            {
                llvm::Value *floatName = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Float), "typeof_float");
                result = cg_.builder_->CreateSelect(isFloatIt->second, floatName, result, "typeof.float.sel");
            }

            // ================================================================
            // (AR) ISSUE-076/082/084 (Amelia #6): معامِلٌ ديناميّ Any (حمولةُ ADT عابرةٌ للدوال،
            //      لا علَمَ __is_float لها هنا) ⇒ نفكّ الوسم زمنَ التشغيل لاختيار الاسم: 01 عشريّ
            //      · 10 رقم · 11 منطقيّ · 00 نصّ (أسماء SoT عبر sadTypeKindArabicName). يُصلح
            //      نوع(ص) على رابطٍ مُمرَّرٍ لدالة. نُبقي «عدم» للحارس.
            // (EN) ISSUE-076/082/084 (Amelia #6): a dynamic Any operand (a payload passed across a
            //      function, with no __is_float flag here) ⇒ decode the tag at runtime to pick the
            //      name: 01 float · 10 int · 11 bool · 00 string (SoT names via sadTypeKindArabicName).
            //      Fixes نوع(ص) on a binding passed to a function. Keep «عدم» for the sentinel.
            if (inst->operands[0].dataType == SadTypeKind::Any && asI64)
            {
                llvm::Value *b63 = cg_.builder_->CreateAnd(
                    asI64, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit63), "typeof.b63");
                llvm::Value *b62 = cg_.builder_->CreateAnd(
                    asI64, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62), "typeof.b62");
                llvm::Value *isHi = cg_.builder_->CreateICmpNE(b63, llvm::ConstantInt::get(i64Ty, 0), "typeof.hi");
                llvm::Value *isLo = cg_.builder_->CreateICmpNE(b62, llvm::ConstantInt::get(i64Ty, 0), "typeof.lo");
                llvm::Value *intName = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Integer), "typeof.int");
                llvm::Value *floatName2 = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Float), "typeof.flt");
                llvm::Value *boolName = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Boolean), "typeof.bool");
                llvm::Value *strName = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::String), "typeof.str");
                llvm::Value *nameHi = cg_.builder_->CreateSelect(isLo, boolName, intName, "typeof.hi.sel");
                llvm::Value *nameLo = cg_.builder_->CreateSelect(isLo, floatName2, strName, "typeof.lo.sel");
                llvm::Value *dynName = cg_.builder_->CreateSelect(isHi, nameHi, nameLo, "typeof.dyn");
                llvm::Value *isNull2 = cg_.builder_->CreateICmpEQ(
                    asI64, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), "typeof.isnull2");
                llvm::Value *nullStr2 = cg_.builder_->CreateGlobalStringPtr(
                    ::Sad::Types::sadTypeKindArabicName(SadTypeKind::Null), "typeof_null2");
                result = cg_.builder_->CreateSelect(isNull2, nullStr2, dynName, "typeof.any.sel");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

    } // namespace LLVM
} // namespace Sad
