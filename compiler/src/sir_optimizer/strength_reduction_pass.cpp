/**
 * @file strength_reduction_pass.cpp
 * @brief تنفيذ تمرير تقليل القوة
 * @brief Strength Reduction Pass Implementation
 * 
 * @details
 * (AR) يستبدل العمليات الحسابية المكلفة بعمليات أخف.
 * (EN) Replaces expensive arithmetic operations with cheaper alternatives.
 * 
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "sir_optimizer/strength_reduction_pass.h"
#include <iostream>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// Constructor / البناء
// ============================================================================

StrengthReductionPass::StrengthReductionPass()
    : OptimizationPass("Strength Reduction", PassType::TRANSFORMATION)
    , reductionCount_(0)
{
}

StrengthReductionPass::~StrengthReductionPass() = default;

// ============================================================================
// Main Entry Point
// ============================================================================

bool StrengthReductionPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) return false;
    
    reductionCount_ = 0;
    bool modified = false;
    
    for (auto& blockPtr : function->basicBlocks) {
        if (processBlock(blockPtr.get())) {
            modified = true;
        }
    }
    
    return modified;
}

// ============================================================================
// Block Processing / معالجة الكتل
// ============================================================================

bool StrengthReductionPass::processBlock(SIR::SIRBasicBlock* block) {
    if (!block) return false;
    
    bool modified = false;
    
    for (auto& inst : block->instructions) {
        // (AR) محاولة التبسيط الجبري أولاً
        // (EN) Try algebraic simplification first
        if (tryAlgebraicSimplification(inst)) {
            modified = true;
            continue;
        }
        
        // (AR) محاولة تقليل القوة حسب نوع العملية
        // (EN) Try strength reduction by operation type
        switch (inst.opcode) {
            case SIR::SIROpcode::MUL_I64:
                if (tryReduceMultiply(inst)) modified = true;
                break;
                
            case SIR::SIROpcode::DIV_I64:
                if (tryReduceDivision(inst)) modified = true;
                break;
                
            case SIR::SIROpcode::FLOOR_DIV_I64:
                if (tryReduceDivision(inst)) modified = true;
                break;
                
            case SIR::SIROpcode::MOD_I64:
                if (tryReduceModulo(inst)) modified = true;
                break;
                
            default:
                break;
        }
    }
    
    return modified;
}

// ============================================================================
// Strength Reduction Transforms / تحويلات تقليل القوة
// ============================================================================

// (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): معاملٌ قد يكون ديناميًّا زمنَ التوليد (%SadDyn، حمولةُ
//      ADT) ⇒ لا تُحوّله إلى إزاحة/قناعٍ بتّيّ. نوعه Any أو Unknown هنا (فقدُ نوعٍ في تعبيرٍ ديناميّ)،
//      و`shl/shr/and` على بنية %SadDyn = IR باطل ⇒ انهيار الخلفيّة. خلفيّة LLVM تُعيد تقليل القوّة
//      للأعداد الصحيحة الحقيقيّة، فتخطّي التحويل هنا بلا خسارة أداء.
// (EN) ISSUE-076 (%SadDyn root fix): an operand that may be dynamic at codegen (%SadDyn, an ADT
//      payload) ⇒ do NOT reduce it to a shift/bit-mask. Its type is Any or Unknown here (type lost in
//      a dynamic expression), and shl/shr/and on a %SadDyn struct = invalid IR ⇒ backend crash.
//      LLVM's backend re-applies strength reduction for real integers, so skipping here costs nothing.
static inline bool srMaybeDynamic(const SIR::SIROperand& op) {
    return op.dataType == SIR::SadTypeKind::Any || op.dataType == SIR::SadTypeKind::Unknown;
}

// (AR) Amelia (ISSUE-063): معاملٌ أو نتيجةٌ عشريّة ⇒ لا تقليلَ قوّةٍ بتّيًّا — الإزاحة/القناع
//      على double (أو على بِتّاته المحمولة i64) يُنتج قمامة (7.5//2 كانت تصير lshr على البِتّات).
// (EN) Amelia (ISSUE-063): a float operand or result ⇒ no bitwise strength reduction — a
//      shift/mask on a double (or its raw i64 bits) yields garbage (7.5//2 became an lshr on
//      the bits).
static inline bool srHasFloat(const SIR::SIRInstruction& inst) {
    if (inst.hasResult() && inst.result->dataType == SIR::SadTypeKind::Float)
        return true;
    for (const auto& op : inst.operands)
        if (op.dataType == SIR::SadTypeKind::Float)
            return true;
    return false;
}

bool StrengthReductionPass::tryReduceMultiply(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    if (srHasFloat(inst)) return false; // Amelia (ISSUE-063)
    if (inst.hasResult() && srMaybeDynamic(*inst.result)) return false; // Amelia (ISSUE-063)

    // (AR) محاولة إيجاد المعامل الثابت (قوة 2)
    // (EN) Try to find the constant operand (power of 2)
    for (size_t i = 0; i < 2; ++i) {
        auto constVal = getIntConstant(inst.operands[i]);
        if (constVal && isPowerOfTwo(*constVal) && *constVal > 1) {
            size_t otherIdx = 1 - i;
            if (srMaybeDynamic(inst.operands[otherIdx])) return false; // ISSUE-076
            int shiftAmount = log2(*constVal);

            // (AR) تحويل: x * (2^n) → x << n
            // (EN) Transform: x * (2^n) → x << n
            inst.opcode = SIR::SIROpcode::SHL;
            inst.operands[0] = inst.operands[otherIdx];
            inst.operands[1] = SIR::SIROperand::ConstantI64(shiftAmount);
            
            reductionCount_++;
            recordModification();
            return true;
        }
    }
    
    return false;
}

bool StrengthReductionPass::tryReduceDivision(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    if (srHasFloat(inst)) return false;                                 // Amelia (ISSUE-063)
    if (inst.hasResult() && srMaybeDynamic(*inst.result)) return false; // Amelia (ISSUE-063)

    // (AR) Amelia (ISSUE-063): DIV_I64 (دلالة اقتطاعٍ نحو الصفر) لا تُكافئ أيَّ إزاحة على
    //      السوالب: lshr يعطي قمامة (-7/2 كانت تطبع 9223372036854775804) وashr يعطي أرضيّة
    //      (-4). نتقيّد بـFLOOR_DIV_I64 التي تكافئ ashr تمامًا (إزاحةٌ حسابيّة = أرضيّة)،
    //      وخلفيّة LLVM تعيد تقليل قوّة sdiv الحقيقيّة بنفسها.
    // (EN) Amelia (ISSUE-063): DIV_I64 (truncate-toward-zero semantics) matches NO shift for
    //      negatives: lshr yields garbage (-7/2 printed 9223372036854775804) and ashr yields
    //      floor (-4). Restrict to FLOOR_DIV_I64, which is exactly ashr (arithmetic shift =
    //      floor); LLVM's backend re-derives the sdiv strength reduction itself.
    if (inst.opcode != SIR::SIROpcode::FLOOR_DIV_I64) return false;

    // (AR) [طبقة طبيعي — الخطوة ٧] لا تخفيض لطبيعي: `x // 2^n` الموقَّعُ = SAR (إزاحة
    //      حسابيّة = أرضيّة)، لكنّ طبيعي لا-موقَّع فأرضيّتُه = LSHR (إزاحة منطقيّة). SAR على
    //      طبيعي يمدّ الإشارة (MAX = ‎-1‏ بتّيًّا ⇒ SAR(‎-1‏،1) = ‎-1‏ = MAX بدل MAX//2). نتركها
    //      FLOOR_DIV_I64 لمسار emitDiv اللا-موقَّع (UDiv)، وخلفيّةُ LLVM تخفّض udiv/2^n إلى lshr
    //      بنفسها. (النتيجةُ مثبَّتةٌ UInt64 في الواجهة الأماميّة لطبيعي //).
    // (EN) [طبيعي layer — Step 7] No reduction for طبيعي: signed `x // 2^n` = SAR (arithmetic
    //      shift = floor), but طبيعي is unsigned so its floor = LSHR (logical shift). SAR on a
    //      طبيعي sign-extends (MAX = -1 bits ⇒ SAR(-1,1) = -1 = MAX instead of MAX//2). Leave it
    //      as FLOOR_DIV_I64 for emitDiv's unsigned UDiv path; LLVM's backend strength-reduces
    //      udiv/2^n to lshr itself. (The result is pinned UInt64 in the frontend for طبيعي //.)
    if (inst.hasResult() && inst.result->dataType == SIR::SadTypeKind::UInt64)
        return false;

    // (AR) التحقق من أن القاسم ثابت وقوة 2
    // (EN) Check if divisor is constant and power of 2
    auto constVal = getIntConstant(inst.operands[1]);
    if (constVal && isPowerOfTwo(*constVal) && *constVal > 1) {
        if (srMaybeDynamic(inst.operands[0])) return false; // ISSUE-076 (%SadDyn)
        int shiftAmount = log2(*constVal);

        // (AR) تحويل: x // (2^n) → x >>> n (إزاحة حسابيّة = أرضيّة للسوالب أيضًا)
        // (EN) Transform: x // (2^n) → x >>> n (arithmetic shift = floor, negatives included)
        inst.opcode = SIR::SIROpcode::SAR;
        inst.operands[1] = SIR::SIROperand::ConstantI64(shiftAmount);

        reductionCount_++;
        recordModification();
        return true;
    }

    return false;
}

bool StrengthReductionPass::tryReduceModulo(SIR::SIRInstruction& inst) {
    // (AR) Amelia (ISSUE-063): «x % (2^n) → x & (2^n-1)» صحيحة لغير السالب فقط — srem يحمل
    //      إشارة المقسوم (-7 % 4 = -3) بينما القناع يعطي 1. لا سبيل لإثبات عدم السالبيّة
    //      هنا، وخلفيّة LLVM تقلّل قوّة srem الحقيقيّة بنفسها ⇒ نعطّل التحويل.
    // (EN) Amelia (ISSUE-063): "x % (2^n) → x & (2^n-1)" holds for non-negatives only — srem
    //      carries the dividend's sign (-7 % 4 = -3) while the mask yields 1. Non-negativity
    //      cannot be proven here, and LLVM's backend strength-reduces true srem itself ⇒
    //      disable the transform.
    (void)inst;
    return false;
}

// ============================================================================
// Algebraic Simplification / التبسيط الجبري
// ============================================================================

bool StrengthReductionPass::tryAlgebraicSimplification(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    if (!inst.hasResult()) return false;

    // (AR) ISSUE-076/084 (ب″): نتيجةٌ ديناميّة (Any = حمولةُ ADT مجهولةُ النوع) ⇒ لا تبسيط
    //      جبريّ صحيحيّ الدلالة. `س - س` لعشريٍّ = 0.0 (لا 0 الصحيح)، و`س * 0`=0.0، إلخ؛
    //      هذه المطابقات تُنتج ثابتًا صحيحًا غير موسوم يُقرأ خطأً (مؤشّر null ⇒ "void").
    //      نترك المسار الديناميّ في الخلف يحسبها بوسمها الصحيح زمنَ التشغيل.
    // (EN) ISSUE-076/084 (ب″): a dynamic (Any = statically-unknown ADT payload) result ⇒ no
    //      integer-semantics algebraic simplification. `x - x` for a float = 0.0 (not int 0),
    //      `x * 0` = 0.0, etc.; these identities emit an untagged integer constant that decodes
    //      wrongly (null pointer ⇒ "void"). Leave it to the backend's dynamic path to compute the
    //      correctly-tagged result at runtime.
    if (inst.result->dataType == SIR::SadTypeKind::Any) return false;

    // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): معاملٌ ديناميّ (Any = %SadDyn، حمولةُ ADT) ⇒ لا
    //      تقليلَ قوّةٍ صحيحيًّا. المعامل ليس i64 بل بنيةٌ واصفةٌ لذاتها، فتحويلُ «*2→<<1» أو
    //      «+0→move» يُصدر عمليّة صحيحة على بنية = IR باطل ⇒ انهيار الخلفيّة. النتيجة قد تُستنتَج
    //      محدَّدةً بينما المعامل ديناميّ، فلا يكفي فحصُ النتيجة وحده.
    // (EN) ISSUE-076 (%SadDyn root fix): a dynamic operand (Any = %SadDyn, an ADT payload) ⇒ no
    //      integer strength reduction. The operand is a self-describing struct, not i64, so
    //      "*2→<<1" or "+0→move" emit an integer op on a struct = invalid IR ⇒ backend crash. The
    //      result may be inferred concrete while an operand is dynamic, so guarding the result alone
    //      is insufficient.
    for (const auto &op : inst.operands)
        if (srMaybeDynamic(op)) return false;

    auto c0 = getIntConstant(inst.operands[0]);
    auto c1 = getIntConstant(inst.operands[1]);
    
    switch (inst.opcode) {
        // (AR) x + 0 → x, 0 + x → x
        case SIR::SIROpcode::ADD_I64: {
            if (c1 && *c1 == 0) {
                // (AR) تحويل إلى نسخ / Convert to move
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            if (c0 && *c0 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[1]};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x - 0 → x
        case SIR::SIROpcode::SUB_I64: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            // (AR) x - x → 0
            if (inst.operands[0].type == SIR::SIROperandType::REGISTER &&
                inst.operands[1].type == SIR::SIROperandType::REGISTER &&
                inst.operands[0].name == inst.operands[1].name) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {SIR::SIROperand::ConstantI64(0)};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x * 0 → 0, x * 1 → x, 0 * x → 0, 1 * x → x
        case SIR::SIROpcode::MUL_I64: {
            for (size_t i = 0; i < 2; ++i) {
                auto cv = (i == 0) ? c0 : c1;
                if (cv) {
                    if (*cv == 0) {
                        inst.opcode = SIR::SIROpcode::MOVE;
                        inst.operands = {SIR::SIROperand::ConstantI64(0)};
                        recordModification();
                        return true;
                    }
                    if (*cv == 1) {
                        size_t otherIdx = 1 - i;
                        inst.opcode = SIR::SIROpcode::MOVE;
                        inst.operands = {inst.operands[otherIdx]};
                        recordModification();
                        return true;
                    }
                }
            }
            break;
        }
        
        // (AR) x / 1 → x
        case SIR::SIROpcode::DIV_I64:
        case SIR::SIROpcode::FLOOR_DIV_I64: {
            // (AR) Amelia (ISSUE-063): معاملٌ عشريّ ⇒ ليست هويّة: 7.5//1 = floor = 7.0 لا 7.5،
            //      وقسمة() تقتطع. اتركها للخلف.
            // (EN) Amelia (ISSUE-063): a float operand ⇒ not an identity: 7.5//1 = floor = 7.0,
            //      not 7.5, and قسمة() truncates. Leave to the backend.
            if (srHasFloat(inst)) break;
            if (c1 && *c1 == 1) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x & 0 → 0, x & -1 → x, x | 0 → x
        case SIR::SIROpcode::AND: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {SIR::SIROperand::ConstantI64(0)};
                recordModification();
                return true;
            }
            if (c1 && *c1 == -1) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            break;
        }
        
        case SIR::SIROpcode::OR: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x ^ 0 → x
        case SIR::SIROpcode::XOR: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            // (AR) x ^ x → 0
            if (inst.operands[0].type == SIR::SIROperandType::REGISTER &&
                inst.operands[1].type == SIR::SIROperandType::REGISTER &&
                inst.operands[0].name == inst.operands[1].name) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {SIR::SIROperand::ConstantI64(0)};
                recordModification();
                return true;
            }
            break;
        }
        
        default:
            break;
    }
    
    return false;
}

// ============================================================================
// Helper Methods / دوال مساعدة
// ============================================================================

bool StrengthReductionPass::isPowerOfTwo(int64_t value) const {
    if (value <= 0) return false;
    return (value & (value - 1)) == 0;
}

int StrengthReductionPass::log2(int64_t value) const {
    int result = 0;
    while (value > 1) {
        value >>= 1;
        result++;
    }
    return result;
}

std::optional<int64_t> StrengthReductionPass::getIntConstant(
    const SIR::SIROperand& operand) const
{
    // (AR) Amelia (ISSUE-063): ثابتٌ صحيحُ النوع فقط — قراءة intValue من ثابت Float تُرجع
    //      بِتّات double خامًا (بِتّات 2.0 = 2^62 «قوّة اثنين»! ⇒ إزاحة بمقدار 62).
    // (EN) Amelia (ISSUE-063): Integer-typed constants only — reading intValue off a Float
    //      constant returns raw double bits (bits(2.0) = 2^62, a "power of two"! ⇒ shift by 62).
    if (operand.type == SIR::SIROperandType::CONSTANT &&
        operand.dataType == SIR::SadTypeKind::Integer) {
        return operand.intValue;
    }
    return std::nullopt;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
