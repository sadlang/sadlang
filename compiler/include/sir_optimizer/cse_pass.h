/**
 * @file cse_pass.h
 * @brief تمرير حذف التعابير الفرعية المشتركة
 * @brief Common Subexpression Elimination Pass
 * 
 * @details
 * (AR) يقوم بإيجاد التعابير المتكررة وإزالتها لتحسين الأداء وتقليل الكود.
 * (EN) Finds and eliminates redundant expressions to improve performance and reduce code.
 * 
 * الخوارزمية / Algorithm:
 * 1. بناء جدول hash للتعابير / Build expression hash table
 * 2. تحديد التعابير المكررة / Identify duplicate expressions
 * 3. استبدال المكررات بالنتيجة المحفوظة / Replace duplicates with saved result
 * 4. حذف التعليمات الزائدة / Remove redundant instructions
 * 
 * مثال / Example:
 * Before:
 *   %1 = add %a, %b
 *   %2 = mul %1, %c
 *   %3 = add %a, %b    <- نفس التعبير / Same expression
 *   %4 = mul %3, %d
 * 
 * After:
 *   %1 = add %a, %b
 *   %2 = mul %1, %c
 *   %4 = mul %1, %d    <- استخدام %1 بدلاً من %3 / Use %1 instead of %3
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#pragma once

#include "pass.h"
#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <vector>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// تمثيل التعبير / Expression Representation
// ============================================================================

/**
 * @brief (AR) معلومات التعبير للمقارنة والتخزين
 * @brief (EN) Expression information for comparison and storage
 * 
 * @details
 * (AR) يُستخدم لتمثيل تعبير بطريقة يمكن مقارنتها وتخزينها في جدول hash
 * (EN) Used to represent an expression in a comparable and hashable way
 */
struct ExpressionKey {
    SIR::SIROpcode opcode;              ///< رمز العملية / Operation code
    // (AR) نوعُ **النتيجة** جزءٌ من الهُويّة: `AND`/`OR` (وهما في القائمة المؤهَّلة)
    //      يحملان دلالتَين — منطقيَّين بنتيجةِ Boolean وبتّيَّين بنتيجةِ Integer —
    //      ولا يفرّق بينهما إلّا هي. ومفتاحٌ بلا نوعِ نتيجةٍ يدمج `س & ص` مع
    //      `ب١ و ب٢` فيرث أحدُهما قيمةَ الآخر. نظيرُ الإصلاحِ في `ExprKey` الأماميّ.
    SIR::SadTypeKind resultType = SIR::SadTypeKind::Unknown;
    std::vector<std::string> operands;   ///< المعاملات / Operands
    
    /**
     * @brief (AR) منشئ
     * @brief (EN) Constructor
     */
    ExpressionKey(SIR::SIROpcode op, const std::vector<std::string>& ops,
                  SIR::SadTypeKind rt = SIR::SadTypeKind::Unknown)
        : opcode(op), resultType(rt), operands(ops) {}
    
    /**
     * @brief (AR) مقارنة المساواة
     * @brief (EN) Equality comparison
     */
    bool operator==(const ExpressionKey& other) const {
        return opcode == other.opcode && resultType == other.resultType &&
               operands == other.operands;
    }
    
    /**
     * @brief (AR) حساب قيمة hash
     * @brief (EN) Compute hash value
     */
    size_t hash() const;
};

/**
 * @brief (AR) معلومات التعبير المحفوظ
 * @brief (EN) Saved expression information
 */
struct ExpressionInfo {
    std::string resultRegister;     ///< السجل الناتج / Result register
    SIR::SIRInstruction* instruction; ///< التعليمة الأصلية / Original instruction
    int blockIndex;                 ///< رقم الكتلة / Block index
    
    ExpressionInfo() : instruction(nullptr), blockIndex(-1) {}
    ExpressionInfo(const std::string& reg, SIR::SIRInstruction* inst, int block)
        : resultRegister(reg), instruction(inst), blockIndex(block) {}
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad

// Hash function for ExpressionKey
namespace std {
    template<>
    struct hash<Sad::Compiler::Optimizer::ExpressionKey> {
        size_t operator()(const Sad::Compiler::Optimizer::ExpressionKey& key) const {
            return key.hash();
        }
    };
}

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// فئة تمرير CSE / CSE Pass Class
// ============================================================================

/**
 * @brief (AR) تمرير حذف التعابير الفرعية المشتركة
 * @brief (EN) Common Subexpression Elimination Pass
 * 
 * @details
 * (AR) يحلل التعابير في الكود ويزيل المكررات منها
 * 
 * (EN) Analyzes expressions in code and removes duplicates
 * 
 * الميزات / Features:
 * - تحديد التعابير المكررة / Identify duplicate expressions
 * - استبدال آمن للمراجع / Safe replacement of references
 * - دعم جميع أنواع العمليات / Support for all operation types
 * - تتبع نطاق الصلاحية / Scope tracking
 * 
 * @example
 * @code{.cpp}
 * CSEPass cse;
 * auto module = createModule();
 * bool modified = cse.run(module);
 * @endcode
 */
class CSEPass : public OptimizationPass {
public:
    /**
     * @brief (AR) منشئ
     * @brief (EN) Constructor
     */
    CSEPass();
    
    /**
     * @brief (AR) مدمر
     * @brief (EN) Destructor
     */
    ~CSEPass() override;
    
    /**
     * @brief (AR) تشغيل التمرير على دالة
     * @brief (EN) Run pass on function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم التعديل / if modified
     */
    bool runOnFunction(SIR::SIRFunction* function) override;
    
    /**
     * @brief (AR) الحصول على اسم التمرير
     * @brief (EN) Get pass name
     */
    std::string getPassName() const {
        return "Common Subexpression Elimination";
    }
    
    /**
     * @brief (AR) الحصول على نوع التمرير
     * @brief (EN) Get pass type
     */
    PassType getPassType() const {
        return PassType::TRANSFORMATION;
    }

private:
    // ========================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ========================================================================
    
    /**
     * @brief (AR) تحليل تعليمة وإنشاء مفتاح التعبير
     * @brief (EN) Analyze instruction and create expression key
     * 
     * @param inst التعليمة / Instruction
     * @return مفتاح التعبير أو nullptr / Expression key or nullptr
     */
    std::unique_ptr<ExpressionKey> analyzeExpression(const SIR::SIRInstruction* inst);
    
    /**
     * @brief (AR) فحص إذا كانت التعليمة قابلة للحذف
     * @brief (EN) Check if instruction is eliminable
     * 
     * @param inst التعليمة / Instruction
     * @return true إذا كانت قابلة للحذف / if eliminable
     */
    bool isEliminable(const SIR::SIRInstruction* inst);
    
    /**
     * @brief (AR) فحص إذا كان التعبير له تأثيرات جانبية
     * @brief (EN) Check if expression has side effects
     * 
     * @param opcode رمز العملية / Operation code
     * @return true إذا كان له تأثيرات جانبية / if has side effects
     */
    bool hasSideEffects(SIR::SIROpcode opcode);
    
    /**
     * @brief (AR) استبدال استخدامات سجل بآخر
     * @brief (EN) Replace uses of one register with another
     * 
     * @param function الدالة / Function
     * @param oldReg السجل القديم / Old register
     * @param newReg السجل الجديد / New register
     * @param startBlock الكتلة البداية / Starting block
     */
    void replaceRegisterUses(
        SIR::SIRFunction* function,
        const std::string& oldReg,
        const std::string& newReg,
        int startBlock
    );
    
    /**
     * @brief (AR) حذف تعليمة ميتة
     * @brief (EN) Remove dead instruction
     * 
     * @param function الدالة / Function
     * @param blockIndex رقم الكتلة / Block index
     * @param instIndex رقم التعليمة / Instruction index
     */
    void removeInstruction(
        SIR::SIRFunction* function,
        int blockIndex,
        int instIndex
    );
    
    /**
     * @brief (AR) مسح جداول التتبع
     * @brief (EN) Clear tracking tables
     */
    void clearTables();
    
    // ========================================================================
    // بيانات خاصة / Private Data
    // ========================================================================
    
    /// جدول التعابير المحفوظة / Saved expressions table
    std::unordered_map<ExpressionKey, ExpressionInfo> expressionTable_;
    
    /// التعليمات المحذوفة / Removed instructions
    std::unordered_set<SIR::SIRInstruction*> removedInstructions_;
    
    /// عداد الاستبدالات / Replacement counter
    int replacementCount_;
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
