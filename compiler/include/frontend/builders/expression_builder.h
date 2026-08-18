// ======================================================================
// expression_builder.h - بنّاء التعابير / Expression Builder
// ======================================================================
// (AR) Phase 6 — Step 7: صنف منفصل يجمع 30 method لبناء التعابير:
//      buildExpression (الموزع) + 4 أساسية (Literal/Variable/ShortCircuit/Unary)
//      + buildBinaryOp + 4 collections + 4 comprehensions + 2 functional
//      + 3 index/ternary + 3 lowlevel + 2 members + 4 nullsafety + 2 objects.
// (EN) Phase 6 — Step 7: standalone class grouping 30 expression builders.
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Sad { namespace AST {
    class ArrayExpr;
    class AtomicExpr;
    class BinaryExpr;
    class DictComprehensionExpr;
    class ErrorPropagateExpr;
    class Expression;
    class GeneratorExpr;
    class IndexAssignExpr;
    class IndexExpr;
    class InlineAsmExpr;
    class LambdaExpr;
    class ListComprehensionExpr;
    class LiteralExpr;
    class MapExpr;
    class MemberAccessExpr;
    class MemberAssignExpr;
    class MemberExpr;
    class NewExpr;
    class NullCoalesceExpr;
    class OptionalChainExpr;
    class RangeExpr;
    class SetComprehensionExpr;
    class SizeofExpr;
    class SliceExpr;
    class TernaryExpr;
    class TupleExpr;
    class UnaryExpr;
    class VariableExpr;
    class WalrusExpr;
}}

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            class SIRBuilder;
            struct BuildResult;
            struct ADTEnumInfo; // (AR) [ISSUE-080] لتوقيع buildAdtFieldDispatch / (EN) for buildAdtFieldDispatch signature

            /**
             * @brief (AR) بنّاء التعابير (30 method)
             * @brief (EN) Expression builder (30 methods)
             */
            class ExpressionBuilder
            {
            public:
                explicit ExpressionBuilder(SIRBuilder &b) : b_(b) {}

                BuildResult buildExpression(Sad::AST::Expression *expr);

                BuildResult buildLiteral(Sad::AST::LiteralExpr *literal);

                BuildResult buildVariableAccess(Sad::AST::VariableExpr *varNode);

                BuildResult buildShortCircuitLogical(Sad::AST::BinaryExpr *binOp);

                BuildResult buildUnaryOp(Sad::AST::UnaryExpr *unOp);

                BuildResult buildBinaryOp(Sad::AST::BinaryExpr *binOp);

                // (AR) [طبقة طبيعي64 — الخطوة ٥] مُحلِّل نوع سطحيّ **ضحل** يُطابق حرفيًّا
                //      resolveStaticType بالمفسّر: متغيّر→نوعه المُصرَّح صراحةً، حرفيّ→رمزه،
                //      نداء→نوع إرجاع الدالّة المُصرَّح صراحةً (astDecl)، ثنائيّ→هيمنة
                //      Float>UInt64>Byte>Integer. لا يستنتج عبر الإسناد/الجسم (بخلاف انتشار
                //      SIR العميق) كي يقرّر المساران إشارةَ المقارنة من نفس المعلومة تمامًا.
                // (EN) [طبيعي64 layer — Step 5] A SHALLOW surface-type resolver that literally
                //      mirrors the interpreter's resolveStaticType: variable→its explicitly-declared
                //      type, literal→its token, call→the callee's explicitly-declared return type
                //      (astDecl), binary→Float>UInt64>Byte>Integer dominance. It does NOT infer
                //      through assignment/body (unlike deep SIR propagation) so both tracks decide
                //      comparison signedness from exactly the same information.
                Sad::Types::SadTypeKind resolveSurfaceType(const Sad::AST::Expression *expr);

                BuildResult buildExprArray(Sad::AST::ArrayExpr *expr);

                BuildResult buildExprTuple(Sad::AST::TupleExpr *expr);

                BuildResult buildExprMap(Sad::AST::MapExpr *expr);

                BuildResult buildExprWalrus(Sad::AST::WalrusExpr *expr);

                BuildResult buildExprListComp(Sad::AST::ListComprehensionExpr *expr);

                BuildResult buildExprDictComp(Sad::AST::DictComprehensionExpr *expr);

                BuildResult buildExprSetComp(Sad::AST::SetComprehensionExpr *expr);


                BuildResult buildExprLambda(Sad::AST::LambdaExpr *expr);

                BuildResult buildExprRange(Sad::AST::RangeExpr *expr);

                BuildResult buildExprTernary(Sad::AST::TernaryExpr *expr);

                BuildResult buildExprIndex(Sad::AST::IndexExpr *expr);

                BuildResult buildExprIndexAssign(Sad::AST::IndexAssignExpr *expr);

                BuildResult buildExprInlineAsm(Sad::AST::InlineAsmExpr *expr);

                BuildResult buildExprSizeof(Sad::AST::SizeofExpr *expr);

                BuildResult buildExprAtomic(Sad::AST::AtomicExpr *expr);

                BuildResult buildExprMember(Sad::AST::MemberExpr *expr);

                BuildResult buildExprMemberAssign(Sad::AST::MemberAssignExpr *expr);

                BuildResult buildExprOptionalChain(Sad::AST::OptionalChainExpr *expr);

                // (AR) النداءُ الآمن — حدُّه المُعلَنُ (SEM043) وتعليلُه عند التعريف.
                // (EN) Safe method call — declared limit (SEM043); rationale at the definition.
                BuildResult buildExprOptionalMethodCall(Sad::AST::MethodCallExpr *expr);

                BuildResult buildExprNullCoalesce(Sad::AST::NullCoalesceExpr *expr);

                BuildResult buildExprSlice(Sad::AST::SliceExpr *expr);

                BuildResult buildExprErrorPropagate(Sad::AST::ErrorPropagateExpr *expr);

                BuildResult buildNewObject(Sad::AST::NewExpr *expr);

                BuildResult buildMemberAccess(Sad::AST::MemberAccessExpr *expr);

            private:
                // (AR) يُهيّئ تكرار خريطة في الاستيعابات الثلاثة: إن كان المصدر خريطةً يستبدله
                //      بمصفوفة مفاتيحها (عناصرها نصوص) عبر __sad_map_keys، ويُصدِر مصفوفة القيم
                //      عبر __sad_map_values إن طُلب متغيّر قيمة (فكّ زوج «لكل مفتاح، قيمة»). يوحّد
                //      المنطق بين buildExprListComp/DictComp/SetComp ويطابق مسار حلقة «لكل».
                // (EN) Prepares map iteration for the three comprehensions: if the source is a map it
                //      replaces it with its keys array (string elements) via __sad_map_keys, and emits
                //      the values array via __sad_map_values when a value variable is requested
                //      (pair-unpack «for key, value»). Shared by buildExprListComp/DictComp/SetComp and
                //      mirrors the «for» loop path.
                // @param iterResult   (in/out) نتيجة المصدر — تُستبدَل بمصفوفة المفاتيح إن كان خريطة
                // @param valueVar     اسم متغيّر القيمة (فارغ ⇒ لا تُجمَع القيم)
                // @param outValuesReg سجلّ مصفوفة القيم (يبقى فارغًا إن لا قيمة أو ليس خريطة)
                // @param outKeyType   نوع عنصر الحلقة المحسوم (String للخريطة، أو نوع عنصر المصفوفة)
                // @param outValueType نوع متغيّر القيمة المحسوم (صالح فقط إن outValuesReg غير فارغ)
                void lowerMapComprehensionIterable(BuildResult &iterResult,
                                                   const std::string &valueVar,
                                                   std::string &outValuesReg,
                                                   SadTypeKind &outKeyType,
                                                   SadTypeKind &outValueType);

                // (AR) مصدر تكرار استيعابٍ محسوم: إمّا مدى (يُخفَّض حسابيًّا: بداية+عدّاد×خطوة)
                //      أو مصفوفة/خريطة (ARRAY_LEN/ARRAY_GET). يوحّد كشف المدى ودعمه بين
                //      استيعابات القوائم/الخرائط/المجموعات كي لا تتباعد.
                // (EN) A resolved comprehension iteration source: either a range (lowered
                //      arithmetically: start + counter × step) or an array/map
                //      (ARRAY_LEN/ARRAY_GET). Unifies range detection/support across the
                //      list/dict/set comprehensions so they cannot diverge.
                struct ComprehensionSource
                {
                    bool isRange = false;
                    // (AR) مسار المدى / (EN) range path
                    std::string startReg;
                    std::string stepReg;
                    std::string lenReg;
                    // (AR) مسار المصفوفة/الخريطة / (EN) array/map path
                    std::string iterRegName;
                    SadTypeKind iterType = SadTypeKind::Array;
                    std::string mapValuesReg;
                    SadTypeKind keyElemType = SadTypeKind::Integer;
                    SadTypeKind valueVarType = SadTypeKind::Integer;
                };

                // (AR) يُهيّئ مصدر تكرار الاستيعاب في كتلة ما-قبل-الحلقة الحاليّة: يكشف
                //      RangeExpr فيبثّ حدوده وخطوته وطوله لابتفريعيًّا؛ وإلّا يبني المصدر
                //      ويُهيّئ تكرار الخريطة. يُستدعى قبل كتل الحلقة.
                // (EN) Prepares the comprehension iteration source in the current loop
                //      preheader: detects a RangeExpr and emits its bounds/step/length
                //      branchlessly; otherwise builds the source and prepares map
                //      iteration. Call before the loop blocks.
                ComprehensionSource prepareComprehensionSource(Sad::AST::Expression *iterable,
                                                               const std::string &valueVar);

                // (AR) يعيد سجلّ طول المصدر (المدى: محسوب مسبقًا؛ وإلّا ARRAY_LEN). يبثّ في
                //      الكتلة الحاليّة (كتلة الشرط عادةً).
                // (EN) Returns the source length register (range: precomputed; else
                //      ARRAY_LEN). Emits into the current block (usually the cond block).
                std::string comprehensionSourceLength(const ComprehensionSource &src);

                // (AR) يعيد سجلّ العنصر عند العدّاد (المدى: بداية+عدّاد×خطوة؛ وإلّا ARRAY_GET).
                //      يبثّ في الكتلة الحاليّة (كتلة الجسم).
                // (EN) Returns the element register at the counter (range: start +
                //      counter × step; else ARRAY_GET). Emits into the current block (body).
                std::string comprehensionSourceElement(const ComprehensionSource &src,
                                                       const std::string &curIdxReg);

                // (AR) [ISSUE-080] يبني الوصول النقطيّ المباشر لحقل ADT بتوزيعٍ حسب وسم
                //      الحالة زمن التشغيل: يجمع (وسم، فهرس) لكلّ حالةٍ تحوي الحقل ويبثّ
                //      ENUM_GET_PAYLOAD في «وضع التوزيع» (كاشفٌ kAdtFieldDispatchSentinel).
                //      يُغلق تصادم الأسماء عبر الحالات (يقرأ الخانة الصحيحة) والوصول لحالةٍ
                //      خاطئة (trap). مشترَكٌ بين المسارَين التوأمَين (buildExprMember /
                //      buildMemberAccess) منعًا لتباعدهما. يفترض أنّ التعداد والحقل موجودان.
                // (EN) [ISSUE-080] Builds direct ADT field access with runtime variant-tag
                //      dispatch: collects (tag, index) for each variant containing the field
                //      and emits ENUM_GET_PAYLOAD in «dispatch mode» (kAdtFieldDispatchSentinel).
                //      Closes cross-variant name collision (reads the right slot) and wrong-
                //      variant access (trap). Shared by the twin paths (buildExprMember /
                //      buildMemberAccess) to prevent divergence. Assumes enum+field exist.
                BuildResult buildAdtFieldDispatch(const BuildResult &objResult,
                                                  const std::string &fieldName,
                                                  const ADTEnumInfo &adtInfo);

                SIRBuilder &b_;
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
