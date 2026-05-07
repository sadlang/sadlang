/**
 * @file licm_pass.h
 * @brief تمرير نقل الكود الثابت خارج الحلقة (LICM)
 * @brief Loop Invariant Code Motion Pass
 * 
 * @details
 * (AR) ينقل التعليمات التي لا تعتمد على متغيرات الحلقة إلى خارجها.
 *      هذا يقلل عدد التعليمات المنفذة في كل تكرار.
 *      مثال: في حلقة تحسب x = a + b (حيث a وb لا تتغيران)
 *            يُنقل الحساب إلى ما قبل الحلقة.
 * 
 * (EN) Moves instructions that don't depend on loop variables outside.
 *      Reduces instruction count per iteration.
 *      Example: In a loop computing x = a + b (where a,b are invariant)
 *               the computation is moved to the loop preheader.
 * 
 * @author SadLanguage Compiler Team
 * @date January 2026
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief معلومات الحلقة المكتشفة
 * @brief Detected loop information
 */
struct LoopInfo {
    SIR::SIRBasicBlock* header;              ///< رأس الحلقة / Loop header
    SIR::SIRBasicBlock* preheader;           ///< كتلة ما قبل الحلقة / Loop preheader
    std::vector<SIR::SIRBasicBlock*> body;   ///< جسم الحلقة / Loop body blocks
    std::vector<SIR::SIRBasicBlock*> exits;  ///< مخارج الحلقة / Loop exit blocks
    SIR::SIRBasicBlock* latch;               ///< كتلة العودة / Loop latch (back-edge)
};

/**
 * @brief تمرير نقل الكود الثابت خارج الحلقة
 * @brief Loop Invariant Code Motion Pass
 * 
 * @details
 * (AR) يكتشف الحلقات في CFG ثم يحدد التعليمات الثابتة (التي لا تعتمد
 *      على متغيرات تتغير داخل الحلقة) وينقلها إلى خارج الحلقة.
 * 
 * (EN) Detects loops in CFG, identifies invariant instructions (not depending
 *      on values modified inside the loop), and hoists them out.
 * 
 * @example
 * Before:
 * ```
 * loop:
 *   %t1 = add.i64 %a, %b     # invariant!
 *   %t2 = mul.i64 %t1, %i    # NOT invariant (uses %i)
 *   ...
 * ```
 * After:
 * ```
 * preheader:
 *   %t1 = add.i64 %a, %b     # hoisted out
 * loop:
 *   %t2 = mul.i64 %t1, %i    # stays in loop
 *   ...
 * ```
 */
class LICMPass : public OptimizationPass {
public:
    LICMPass();
    ~LICMPass();
    
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    /**
     * @brief اكتشاف الحلقات في CFG
     * @brief Detect loops in CFG
     */
    std::vector<LoopInfo> detectLoops(SIR::SIRFunction* function);
    
    /**
     * @brief البحث عن حافات الرجوع (back edges) باستخدام DFS
     * @brief Find back edges using DFS
     */
    void findBackEdges(SIR::SIRBasicBlock* block,
                      std::unordered_set<SIR::SIRBasicBlock*>& visited,
                      std::unordered_set<SIR::SIRBasicBlock*>& onStack,
                      std::vector<std::pair<SIR::SIRBasicBlock*, SIR::SIRBasicBlock*>>& backEdges);
    
    /**
     * @brief بناء جسم الحلقة من حافة الرجوع
     * @brief Build loop body from back edge
     */
    LoopInfo buildLoopFromBackEdge(SIR::SIRBasicBlock* latch,
                                  SIR::SIRBasicBlock* header,
                                  SIR::SIRFunction* function);
    
    /**
     * @brief التحقق مما إذا كانت التعليمة ثابتة بالنسبة للحلقة
     * @brief Check if instruction is loop-invariant
     */
    bool isLoopInvariant(const SIR::SIRInstruction& inst,
                        const LoopInfo& loop,
                        const std::unordered_set<std::string>& loopDefs);
    
    /**
     * @brief جمع جميع التعريفات داخل الحلقة
     * @brief Collect all definitions inside the loop
     */
    std::unordered_set<std::string> collectLoopDefs(const LoopInfo& loop);
    
    /**
     * @brief نقل التعليمة إلى ما قبل الحلقة
     * @brief Hoist instruction to preheader
     */
    bool hoistInstruction(SIR::SIRInstruction& inst,
                         LoopInfo& loop);
    
    /**
     * @brief التحقق من أن النقل آمن
     * @brief Check if hoisting is safe
     */
    bool isSafeToHoist(const SIR::SIRInstruction& inst) const;
    
    /**
     * @brief إيجاد أو إنشاء كتلة ما قبل الحلقة
     * @brief Find or create loop preheader
     */
    SIR::SIRBasicBlock* findOrCreatePreheader(LoopInfo& loop,
                                              SIR::SIRFunction* function);
    
    int hoistedCount_;   ///< عدد التعليمات المنقولة / Hoisted instruction count
    int loopsFound_;     ///< عدد الحلقات المكتشفة / Loops found count
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
