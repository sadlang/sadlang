// ============================================================================
// llvm_codegen_arithmetic_cmp.cpp — عمليات الإزاحة والمقارنة
// LLVM IR: Shift operations and comparison operators
// ============================================================================
// تم استخراج هذا الملف من llvm_codegen_arithmetic.cpp وفقاً لقاعدة CW-05
// ============================================================================
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
#include "sad_dyn_repr.h" // (AR) ISSUE-076: موزِّع dynCompare للمعامِلات الديناميّة %SadDyn
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
#include "builders/arithmetic/arithmetic_codegen.h" // (Phase 7 Step 1)
#include "llvm_codegen.h"

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        llvm::Value *ArithmeticCodeGen::emitShl(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Shl"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معامل %SadDyn (خانة رقّاها المسحُ المسبق) ⇒ فكّ i64 دقيق
            //      (عشريّ⇒fptosi، صحيح⇒الحمولة) — كان CreateShl على بنية ⇒ IR فاسد.
            // (EN) ISSUE-063: a %SadDyn operand (pre-scan-promoted slot) ⇒ precise i64
            //      unpack (Float⇒fptosi, Int⇒payload) — CreateShl on a struct was invalid IR.
            if (isSadDyn(left))
                left = unpackI64(cg_, left);
            if (isSadDyn(right))
                right = unpackI64(cg_, right);

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "shl.l.ptoi");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "shl.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "shl.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "shl.r.zext");
            if (left->getType()->isDoubleTy())
                left = emitF64ToI64Sat(left, "shl.l.f2i.sat");
            if (right->getType()->isDoubleTy())
                right = emitF64ToI64Sat(right, "shl.r.f2i.sat");

            llvm::Value *result = cg_.builder_->CreateShl(left, right, "shltmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة إزاحة يمين
         * Emit shift right instruction
         *
         * Source: llvm_codegen.h:427
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitShr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Shr"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معامل %SadDyn ⇒ فكّ i64 دقيق (مرآة emitShl)
            // (EN) ISSUE-063: a %SadDyn operand ⇒ precise i64 unpack (mirror of emitShl)
            if (isSadDyn(left))
                left = unpackI64(cg_, left);
            if (isSadDyn(right))
                right = unpackI64(cg_, right);

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "shr.l.ptoi");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "shr.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "shr.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "shr.r.zext");
            if (left->getType()->isDoubleTy())
                left = emitF64ToI64Sat(left, "shr.l.f2i.sat");
            if (right->getType()->isDoubleTy())
                right = emitF64ToI64Sat(right, "shr.r.f2i.sat");

            // (AR) [طبقة طبيعي64 — الخطوة ٨] إشارةُ الإزاحة اليمنى من النوع السطحيّ للمعامل
            //      الأيسر (القيمة المُزاحة): طبيعي64 ⇒ منطقيّة LShr (تُدخِل أصفارًا: MAX>>1=2^63-1)
            //      مطابقةً للمفسّر اللا-موقَّع؛ غير ذلك ⇒ حسابيّة AShr تحفظ الإشارة (‎-8>>1=-4‏)
            //      مطابقةً للمفسّر الموقَّع `int64_t >>`. الواجهةُ الأماميّة تُصالِح operands[0]
            //      .dataType مع السطح الضحل (رفعٌ عند الطمس بإعادة الإسناد، خفضٌ للمُستنتَج).
            // (EN) [طبيعي64 layer — Step 8] Right-shift signedness from the LEFT operand's (the
            //      shifted value's) surface type: طبيعي64 ⇒ logical LShr (shifts in zeros:
            //      MAX>>1=2^63-1) matching the unsigned interpreter; otherwise ⇒ arithmetic AShr
            //      (sign-preserving, -8>>1=-4) matching the signed interpreter `int64_t >>`. The
            //      frontend reconciles operands[0].dataType with the shallow surface (upgrade on a
            //      reassign clobber, downgrade for inferred).
            llvm::Value *result =
                (inst->operands[0].dataType == SadTypeKind::UInt64)
                    ? cg_.builder_->CreateLShr(left, right, "lshrtmp")
                    : cg_.builder_->CreateAShr(left, right, "shrtmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // المرحلة 5: تعليمات المقارنة والذاكرة
        // Phase 5: Comparison & Memory Instructions
        // ============================================================================

        /**
         * إصدار تعليمة مقارنة متساوي
         * Emit equal comparison instruction
         *
         * Source: llvm_codegen.h:433
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        namespace
        {
            // (AR) مقارنةُ «عدم» في المساواةِ واللامساواة — قبلَ أيِّ تفرّعٍ على شكلِ التمثيل.
            //
            //      «لاشيء» تمثيلُه حارسٌ عدديٌّ (kSadNullSentinel)، فمقارنتُه حكمٌ عدديٌّ على
            //      القيمةِ لا مقارنةُ نصوص. وكانَ المسارُ يتفرّعُ أوّلًا على شكلِ الطرفَين،
            //      فمتى طُبِّعا مؤشرَينِ في مرحلةٍ سابقةٍ ذهبَ إلى strcmp فاستدعى:
            //          strcmp(ptr، inttoptr(-9223372036854775807))
            //      وعاقبتُه SIGSEGV.
            //
            //      ولا يجوزُ أن يكونَ الجوابُ ثابتًا («عدمٌ لا يساوي نصًّا ⇒ خطأ»): خانةٌ
            //      نوعُها الساكنُ نصٌّ أو عددٌ قد تحملُ الحارسَ **وقتَ التشغيل**، وهو ما
            //      يجعلُ «متغيّرٌ == لاشيء» صحيحًا. فالحكمُ الثابتُ يكسِرُ العدمَ الحقيقيّ.
            //      والصوابُ تطبيعُ الطرفِ الآخرِ إلى ٦٤ بتًّا ومقارنتُه بالحارس.
            // (EN) Null comparison in equality/inequality — above any dispatch on the shape of
            //      the representation.
            //
            //      `null` is represented by a numeric sentinel (kSadNullSentinel), so comparing
            //      it is a numeric judgement about the value, not a string comparison. The path
            //      used to dispatch on the operand shapes first, so once an earlier stage had
            //      normalised both to pointers it went to strcmp and called:
            //          strcmp(ptr, inttoptr(-9223372036854775807))
            //      which segfaults.
            //
            //      The answer must not be a constant ("null never equals a string ⇒ false"): a
            //      slot whose static type is string or integer may hold the sentinel AT RUNTIME,
            //      which is exactly what makes `variable == null` true. A constant verdict breaks
            //      genuine null. The correct rule is to normalise the other side to 64 bits and
            //      compare it against the sentinel.
            bool operandIsNullLiteral(const SIROperand &operand)
            {
                return operand.dataType == SadTypeKind::Null;
            }

            // (AR) هل النوعُ الساكنُ عدديٌّ؟ — لا يشمل المنطقيَّ عمدًا.
            // (EN) Is the static type numeric? — deliberately excludes bool.
            bool isStaticNumericKind(SadTypeKind kind)
            {
                switch (kind)
                {
                case SadTypeKind::Integer:
                case SadTypeKind::Float:
                case SadTypeKind::Byte:
                case SadTypeKind::Int8:
                case SadTypeKind::Int16:
                case SadTypeKind::Int32:
                case SadTypeKind::Int64:
                case SadTypeKind::UInt8:
                case SadTypeKind::UInt16:
                case SadTypeKind::UInt32:
                case SadTypeKind::UInt64:
                case SadTypeKind::Float32:
                case SadTypeKind::Float64:
                    return true;
                default:
                    return false;
                }
            }

            // (AR) منطقيٌّ في مواجهةِ عدد — نوعان مختلفان في عقدِ المفسّر.
            //
            //      المفسّرُ (evaluateComparisonOp) يفصل: `isNumeric()` لا يشمل المنطقيّ،
            //      فيسقط الطرفان إلى فرعِ «وسمان مختلفان» ويكون الجوابُ «خطأ» للمساواةِ
            //      و«صحيح» للّامساواة. أمّا المصرِّفُ فكان يوسّع المنطقيَّ إلى ٦٤ بتًّا
            //      (`zext_l`/`zext_r` أدناه) ثمّ يقارن عدديًّا، فيقول `1 == صحيح` **صحيح**.
            //
            //      وليس هذا فرقًا نظريًّا: `متغير رقم أ = 1` ثمّ `أ == ب` حيث `ب` منطقيّ
            //      يعطي جوابَين متضادَّين على المحرّكَين لبرنامجٍ واحد. والحكمُ ثابتٌ هنا
            //      لا زمنَ تشغيلٍ لأنّ النوعَين ساكنان: لا حالةَ تشغيلٍ تجعلهما متساوِيَين.
            // (EN) Bool versus number — two different types under the interpreter's contract.
            //
            //      The interpreter (evaluateComparisonOp) separates them: `isNumeric()` does not
            //      include bool, so both sides fall to the "differing tags" branch and the answer
            //      is false for equality and true for inequality. The compiler, by contrast, was
            //      widening the bool to 64 bits (`zext_l`/`zext_r` below) and then comparing
            //      numerically, so it said `1 == true` is **true**.
            //
            //      This is not a theoretical difference: `var int a = 1` then `a == b` with `b`
            //      boolean gives opposite answers on the two engines for one program. The verdict
            //      is constant here rather than runtime because both types are static: no runtime
            //      state can make them equal.
            bool boolVersusNumber(const SIROperand &left, const SIROperand &right)
            {
                return (left.dataType == SadTypeKind::Boolean &&
                        isStaticNumericKind(right.dataType)) ||
                       (right.dataType == SadTypeKind::Boolean &&
                        isStaticNumericKind(left.dataType));
            }

            // (AR) تطبيعُ الطرفِ غيرِ العدمِ إلى عرضِ الحارسِ (٦٤ بتًّا) بلا تغييرِ بتّاتِه:
            //      المؤشرُ بـptrtoint، والعشريُّ ببتّاتِه، والعددُ الأضيقُ بتمديدِ إشارة.
            //      وما لا يُطبَّعُ يُعادُ فيه عدمٌ ليسقطَ النداءُ إلى المسارِ العامِّ بدلَ
            //      أن يُخترَعَ له جوابٌ — فالسكوتُ عن المجهولِ أسلمُ من تخمينِه.
            // (EN) Normalise the non-null side to the sentinel's width (64 bits) without changing
            //      its bits: pointers via ptrtoint, doubles via their bit pattern, narrower
            //      integers via sign extension. Anything else returns null so the caller falls
            //      through to the general path rather than inventing an answer — staying silent
            //      about the unknown is safer than guessing at it.
            llvm::Value *normaliseToSentinelWidth(LLVMCodeGen &cg, llvm::Value *value)
            {
                if (!value)
                    return nullptr;

                llvm::Type *valueType = value->getType();
                llvm::Type *int64Type = llvm::Type::getInt64Ty(*cg.context_);

                if (valueType->isPointerTy())
                    return cg.builder_->CreatePtrToInt(value, int64Type, "null.ptr2int");
                if (valueType->isIntegerTy(64))
                    return value;
                if (valueType->isIntegerTy())
                    return cg.builder_->CreateSExt(value, int64Type, "null.sext");
                if (valueType->isDoubleTy())
                    return cg.builder_->CreateBitCast(value, int64Type, "null.bits");

                return nullptr;
            }

            // (AR) مقارنةُ نصَّينِ آمنةٌ من العدم.
            //
            //      الوسمُ الساكنُ لا يكفي هنا ولا يمكنُ أن يكفي: خانةٌ نوعُها المُصرَّحُ «نصّ»
            //      تقبلُ نصًّا في موقعِ نداءٍ وعدمًا في آخر، فلا يوجَدُ وسمٌ واحدٌ يصدُقُ على
            //      الاثنَين. فحملُ الوسمِ عبرَ رَبطِ المعامِلِ — وهو ما كانَ نصُّ العقدِ يصِفُه
            //      علاجًا — لا يُغني عن فحصٍ **وقتَ التشغيل**، لأنّ السؤالَ نفسَه زمنُ تشغيل.
            //
            //      ومن غيرِ هذا الفحصِ يُمرَّرُ حارسُ العدمِ إلى strcmp فيسقطُ البرنامجُ
            //      بـSIGSEGV بلا تشخيص، بينما المفسّرُ يُصيبُ ويُنذِر.
            //
            //      والصياغةُ بلا تفرّعٍ عمدًا: يُستبدَلُ بالمؤشرِ العدميِّ نصٌّ فارغٌ قبلَ
            //      النداء، فلا تُقرَأُ ذاكرةٌ غيرُ صالحةٍ أصلًا، ثمّ يُنتقى الجوابُ. فالعدمُ
            //      يساوي العدمَ ولا يساوي نصًّا، وهو حكمُ المفسّرِ حرفًا.
            // (EN) Null-safe string comparison.
            //
            //      A static tag is not enough here and cannot be: a slot declared `string` may
            //      receive a string at one call site and null at another, so no single static tag
            //      is true of both. Carrying the tag across parameter binding — which the contract
            //      text prescribed as the cure — does not remove the need for a RUNTIME check,
            //      because the question itself is a runtime one.
            //
            //      Without this check the null sentinel reaches strcmp and the program dies with
            //      SIGSEGV and no diagnostic, while the interpreter is correct and warns.
            //
            //      The formulation is deliberately branchless: a null pointer is replaced by an
            //      empty string before the call, so invalid memory is never read at all, and then
            //      the answer is selected. Null equals null and never equals a string — exactly
            //      the interpreter's verdict.
            llvm::Value *emitNullSafeStringEquality(LLVMCodeGen &cg,
                                                    llvm::Value *left,
                                                    llvm::Value *right,
                                                    bool wantEqual)
            {
                llvm::Type *int64Type = llvm::Type::getInt64Ty(*cg.context_);
                llvm::Type *pointerType = llvm::PointerType::getUnqual(*cg.context_);
                llvm::Constant *sentinel =
                    llvm::ConstantInt::get(int64Type, Sad::Compiler::kSadNullSentinel);

                llvm::Value *leftBits = cg.builder_->CreatePtrToInt(left, int64Type, "str.l.bits");
                llvm::Value *rightBits = cg.builder_->CreatePtrToInt(right, int64Type, "str.r.bits");
                llvm::Value *leftIsNull = cg.builder_->CreateICmpEQ(leftBits, sentinel, "str.l.isnull");
                llvm::Value *rightIsNull = cg.builder_->CreateICmpEQ(rightBits, sentinel, "str.r.isnull");
                llvm::Value *eitherIsNull = cg.builder_->CreateOr(leftIsNull, rightIsNull, "str.either.isnull");
                llvm::Value *bothAreNull = cg.builder_->CreateAnd(leftIsNull, rightIsNull, "str.both.isnull");

                llvm::Value *emptyText = cg.builder_->CreateGlobalStringPtr("", "str.empty");
                llvm::Value *safeLeft = cg.builder_->CreateSelect(leftIsNull, emptyText, left, "str.l.safe");
                llvm::Value *safeRight = cg.builder_->CreateSelect(rightIsNull, emptyText, right, "str.r.safe");

                llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg.context_), {pointerType, pointerType}, false);
                llvm::FunctionCallee strcmpFn = cg.module_->getOrInsertFunction("strcmp", strcmpType);
                llvm::Value *strcmpResult =
                    cg.builder_->CreateCall(strcmpFn, {safeLeft, safeRight}, "strcmp.ret");
                llvm::Value *textsMatch = cg.builder_->CreateICmpEQ(
                    strcmpResult, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg.context_), 0), "streq");

                llvm::Value *areEqual =
                    cg.builder_->CreateSelect(eitherIsNull, bothAreNull, textsMatch, "str.eq.nullsafe");
                if (wantEqual)
                    return areEqual;
                return cg.builder_->CreateNot(areEqual, "str.ne.nullsafe");
            }
        } // namespace

        llvm::Value *ArithmeticCodeGen::emitCmpEq(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CmpEq"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", std::string("Operands not found for cmpeq:") + inst->operands[0].name + ", " + inst->operands[1].name}});
                return nullptr;
            }

            llvm::Value *result = nullptr;

            // (AR) مقارنةُ «عدم» أوّلًا — راجِعْ تعليقَ operandIsNullLiteral أعلاه.
            // (EN) Null comparison first — see the operandIsNullLiteral comment above.
            if (operandIsNullLiteral(inst->operands[0]) != operandIsNullLiteral(inst->operands[1]))
            {
                llvm::Value *other = operandIsNullLiteral(inst->operands[0]) ? right : left;
                if (llvm::Value *widened = normaliseToSentinelWidth(cg_, other))
                {
                    result = cg_.builder_->CreateICmpEQ(
                        widened,
                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_),
                                               Sad::Compiler::kSadNullSentinel),
                        "nulleq");
                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
            }

            // (AR) منطقيٌّ مقابل عدد ⇒ «خطأ» — راجِعْ تعليقَ boolVersusNumber أعلاه.
            // (EN) Bool vs number ⇒ false — see the boolVersusNumber comment above.
            if (boolVersusNumber(inst->operands[0], inst->operands[1]))
            {
                result = llvm::ConstantInt::getFalse(*cg_.context_);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة مقارنة المؤشرات/النصوص أولاً (حتى لو الأنواع متطابقة)
            // (EN) Handle pointer/string comparison first (even if types match)
            if (leftTy->isPointerTy() && rightTy->isPointerTy())
            {
                // (AR) كلاهما مؤشران - استخدام strcmp لمقارنة النصوص
                // (EN) Both pointers - use strcmp for string comparison
                bool isStringCmp = (inst->operands[0].dataType == SadTypeKind::String ||
                                    inst->operands[1].dataType == SadTypeKind::String);
                if (isStringCmp)
                {
                    result = emitNullSafeStringEquality(cg_, left, right, true);
                }
                else
                {
                    result = cg_.builder_->CreateICmpEQ(left, right, "cmpeqtmp");
                }
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                // ================================================================
                // (AR) فحص مقارنة نصوص عندما أحد الطرفين i64 والآخر ptr
                //      حقول الكائنات تُخزن كـ i64 لكنها فعلياً مؤشرات نصوص
                //      نحوّل i64 إلى ptr ونستدعي strcmp
                // (EN) String comparison when one side is i64 and other is ptr
                //      Object fields stored as i64 but actually string pointers
                //      Convert i64 to ptr and call strcmp
                // ================================================================
                {
                    bool isStringCmp = (inst->operands[0].dataType == SadTypeKind::String ||
                                        inst->operands[1].dataType == SadTypeKind::String);
                    if (isStringCmp &&
                        ((leftTy->isIntegerTy(64) && rightTy->isPointerTy()) ||
                         (leftTy->isPointerTy() && rightTy->isIntegerTy(64))))
                    {
                        if (leftTy->isIntegerTy(64))
                            left = cg_.builder_->CreateIntToPtr(left, llvm::PointerType::getUnqual(*cg_.context_), "i642ptr.l");
                        if (rightTy->isIntegerTy(64))
                            right = cg_.builder_->CreateIntToPtr(right, llvm::PointerType::getUnqual(*cg_.context_), "i642ptr.r");

                        result = emitNullSafeStringEquality(cg_, left, right, true);
                        if (inst->result.has_value())
                        {
                            cg_.context_info_.namedValues[inst->result->name] = result;
                        }
                        return result;
                    }
                }

                // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = cg_.builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = cg_.builder_->CreateZExt(right, leftTy, "zext_r");
                }
                else if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = coerceFloatOperandToDouble(inst->operands[0], left); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = coerceFloatOperandToDouble(inst->operands[1], right); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                // (AR) تحديث الأنواع بعد التحويل
                // (EN) Update types after conversion
                leftTy = left->getType();
                rightTy = right->getType();
            }

            // (AR) ISSUE-076/082/084 (ب″، Amelia #9): مقارنة == واعيةٌ بالوسم. لمعامِلٍ Any:
            //      إن كان أيّ طرفٍ صندوقًا عشريًّا (01) ⇒ فكّ التعليب وقارِن FCMP (يُصلح == بين
            //      صندوقين عشريّين مختلفين)؛ وإلّا فكّ وسم الصحيح/المنطقيّ (10/11) وقارِن ICMP.
            //      صندوقٌ عشريّ مقابل حرفيٍّ double يُفكَّ في مسار coerce أعلاه (كلاهما double هنا).
            // (EN) ISSUE-076/082/084 (ب″, Amelia #9): tag-aware ==. For an Any operand: if either
            //      side is a boxed float (01) ⇒ unbox and FCMP (fixes == between two distinct boxed
            //      floats); else untag int/bool (10/11) and ICMP. A boxed float vs a double literal
            //      is unboxed in the coerce path above (both double here).
            result = emitDynamicEqNe(inst->operands[0], inst->operands[1], left, right, /*isEq=*/true);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة غير متساوي
         * Emit not equal comparison instruction
         *
         * Source: llvm_codegen.h:434
         */
        llvm::Value *ArithmeticCodeGen::emitCmpNe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CmpNe"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) والمقارنةُ نفسُها هنا: العطبُ مُطابِقٌ في مسارِ اللامساواة، إذ يستدعي
            //      strcmp على الحارسِ المُحوَّلِ مؤشرًا.
            // (EN) The same comparison here: the identical defect exists on the inequality path,
            //      which strcmps the sentinel-as-pointer.
            if (operandIsNullLiteral(inst->operands[0]) != operandIsNullLiteral(inst->operands[1]))
            {
                llvm::Value *other = operandIsNullLiteral(inst->operands[0]) ? right : left;
                if (llvm::Value *widened = normaliseToSentinelWidth(cg_, other))
                {
                    llvm::Value *nullResult = cg_.builder_->CreateICmpNE(
                        widened,
                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_),
                                               Sad::Compiler::kSadNullSentinel),
                        "nullne");
                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = nullResult;
                    return nullResult;
                }
            }

            // (AR) منطقيٌّ مقابل عدد ⇒ «صحيح» — راجِعْ تعليقَ boolVersusNumber أعلاه.
            // (EN) Bool vs number ⇒ true — see the boolVersusNumber comment above.
            if (boolVersusNumber(inst->operands[0], inst->operands[1]))
            {
                llvm::Value *mismatch = llvm::ConstantInt::getTrue(*cg_.context_);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = mismatch;
                return mismatch;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة مقارنة النصوص
            // (EN) Handle string comparison
            if (leftTy->isPointerTy() && rightTy->isPointerTy())
            {
                bool isStringCmp = (inst->operands[0].dataType == SadTypeKind::String ||
                                    inst->operands[1].dataType == SadTypeKind::String);
                if (isStringCmp)
                {
                    llvm::Value *result = emitNullSafeStringEquality(cg_, left, right, false);
                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
            }

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = cg_.builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = cg_.builder_->CreateZExt(right, leftTy, "zext_r");
                }
                else if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = coerceFloatOperandToDouble(inst->operands[0], left); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = coerceFloatOperandToDouble(inst->operands[1], right); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isPointerTy())
                {
                    left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
                }
                else if (rightTy->isPointerTy())
                {
                    right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            // (AR) ISSUE-076/082/084 (ب″، Amelia #9): != واعيةٌ بالوسم — نظير == (صندوق عشريّ ⇒
            //      فكّ+FCMP؛ صحيح/منطقيّ ⇒ فكّ وسم+ICMP). يُصلح != بين صندوقين عشريّين مختلفين.
            // (EN) ISSUE-076/082/084 (ب″, Amelia #9): tag-aware != — mirrors == (boxed float ⇒
            //      unbox+FCMP; int/bool ⇒ untag+ICMP). Fixes != between two distinct boxed floats.
            llvm::Value *result = emitDynamicEqNe(inst->operands[0], inst->operands[1], left, right, /*isEq=*/false);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أقل من
         * Emit less than comparison instruction
         *
         * Source: llvm_codegen.h:435
         */
        llvm::Value *ArithmeticCodeGen::emitCmpLt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CmpLt"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) === ISSUE-076 (حلّ %SadDyn الجذريّ): معامل ديناميّ ⇒ الموزِّع dynCompare ===
            //      أيّ طرفٍ %SadDyn ⇒ وحّد الطرفين وقارِن عبر الموزِّع (عشريّ⇒fcmp، صحيح⇒icmp موقَّع).
            //      يُغلق «الترتيب بين صندوقين عشريّين» بنيويًّا (لا مقارنة عناوين malloc).
            // (EN) === ISSUE-076 (%SadDyn root fix): a dynamic operand ⇒ the dynCompare dispatcher ===
            if (isSadDyn(left) || isSadDyn(right))
            {
                llvm::Value *dl = toDyn(cg_, left, inst->operands[0].dataType);
                llvm::Value *dr = toDyn(cg_, right, inst->operands[1].dataType);
                llvm::Value *dres = dynCompare(cg_, DynCmp::LT, dl, dr);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = dres;
                return dres;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            { // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = coerceFloatOperandToDouble(inst->operands[0], left); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = coerceFloatOperandToDouble(inst->operands[1], right); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = cg_.builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = cg_.builder_->CreateZExt(right, leftTy, "zext_r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = cg_.builder_->CreateFCmpOLT(left, right, "cmplttmp");
            }
            else
            {
                // (AR) [طبقة طبيعي64 — الخطوة ٥] ترتيب لا-موقَّع حين كلا المعامِلين طبيعي64
                //      (يُطابق المفسّر). النوع من dataType الساكن للمعامل. لا أوپكود جديد.
                // (EN) [طبيعي64 layer — Step 5] Unsigned ordering when both operands are طبيعي64
                //      (mirrors the interpreter). Type from the operand's static dataType. No new opcode.
                result = (inst->operands[0].dataType == SadTypeKind::UInt64 &&
                          inst->operands[1].dataType == SadTypeKind::UInt64)
                             ? cg_.builder_->CreateICmpULT(left, right, "cmpulttmp")
                             : cg_.builder_->CreateICmpSLT(left, right, "cmplttmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أقل من أو يساوي
         * Emit less than or equal comparison instruction
         *
         * Source: llvm_codegen.h:436
         */
        llvm::Value *ArithmeticCodeGen::emitCmpLe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CmpLe"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-076 (%SadDyn): معامل ديناميّ ⇒ الموزِّع dynCompare (عشريّ⇒fcmp، صحيح⇒icmp).
            // (EN) ISSUE-076 (%SadDyn): a dynamic operand ⇒ the dynCompare dispatcher.
            if (isSadDyn(left) || isSadDyn(right))
            {
                llvm::Value *dl = toDyn(cg_, left, inst->operands[0].dataType);
                llvm::Value *dr = toDyn(cg_, right, inst->operands[1].dataType);
                llvm::Value *dres = dynCompare(cg_, DynCmp::LE, dl, dr);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = dres;
                return dres;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = coerceFloatOperandToDouble(inst->operands[0], left); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = coerceFloatOperandToDouble(inst->operands[1], right); // ISSUE-076/084 (ب″): فكّ تعليب Any / unbox Any
                }
                else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = cg_.builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = cg_.builder_->CreateZExt(right, leftTy, "zext_r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = cg_.builder_->CreateFCmpOLE(left, right, "cmpletmp");
            }
            else
            {
                // (AR) [طبقة طبيعي64 — الخطوة ٥] ترتيب لا-موقَّع حين كلا المعامِلين طبيعي64.
                // (EN) [طبيعي64 layer — Step 5] Unsigned ordering when both operands are طبيعي64.
                result = (inst->operands[0].dataType == SadTypeKind::UInt64 &&
                          inst->operands[1].dataType == SadTypeKind::UInt64)
                             ? cg_.builder_->CreateICmpULE(left, right, "cmpuletmp")
                             : cg_.builder_->CreateICmpSLE(left, right, "cmpletmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أكبر من
         * Emit greater than comparison instruction
         *
         * Source: llvm_codegen.h:437
         */

    } // namespace LLVM
} // namespace Sad
