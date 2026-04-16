/**
 * =============================================================================
 * ملف: sir_optimizer.cpp
 * الوصف: تحسينات SIR
 * المهمة: T266 - SIR optimization passes
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 * 
 * ⚡ دليل المبتدئ لتحسينات SIR
 * ═════════════════════════════════
 * 
 * ما هي التحسينات؟
 * ────────────────
 * نُحسّن الكود ليعمل أسرع أو يستخدم ذاكرة أقل.
 * 
 * أنواع التحسينات:
 * ────────────────
 * 
 * 1. إزالة الاستعارات الزائدة
 *    ```
 *    قبل:  %1 = Borrow(%0)
 *          %2 = Reborrow(%1)
 *          use(%2)
 *    
 *    بعد:  %1 = Borrow(%0)
 *          use(%1)
 *    ```
 * 
 * 2. تحويل Copy إلى Move
 *    ```
 *    قبل:  %1 = Copy(%0)
 *          Drop(%0)  // %0 لم يُستخدم بعد
 *    
 *    بعد:  %1 = Move(%0)
 *    ```
 * 
 * 3. حذف Drop الميت
 *    ```
 *    قبل:  %0 = Move(...)
 *          Drop(%0)  // %0 منقول، لا حاجة للإسقاط
 *    
 *    بعد:  %0 = Move(...)
 *    ```
 * 
 * 4. طي الثوابت
 *    ```
 *    قبل:  %0 = 10
 *          %1 = 20
 *          %2 = Add(%0, %1)
 *    
 *    بعد:  %2 = 30
 *    ```
 * 
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>

namespace sad::sir {

// =============================================================================
// إحصائيات التحسين
// =============================================================================

/**
 * إحصائيات التحسين
 */
struct OptimizationStats {
    int removedInstructions = 0;
    int copyToMoveConversions = 0;
    int removedReborrows = 0;
    int removedDeadDrops = 0;
    int foldedConstants = 0;
    int removedUnusedValues = 0;
    
    std::string toString() const {
        std::string result = "إحصائيات التحسين:\n";
        result += "  - تعليمات محذوفة: " + std::to_string(removedInstructions) + "\n";
        result += "  - تحويل Copy→Move: " + std::to_string(copyToMoveConversions) + "\n";
        result += "  - إعادة استعارة محذوفة: " + std::to_string(removedReborrows) + "\n";
        result += "  - إسقاط ميت محذوف: " + std::to_string(removedDeadDrops) + "\n";
        result += "  - ثوابت مطوية: " + std::to_string(foldedConstants) + "\n";
        result += "  - قيم غير مستخدمة: " + std::to_string(removedUnusedValues) + "\n";
        return result;
    }
};

// =============================================================================
// تمريرة تحسين أساسية
// =============================================================================

/**
 * واجهة تمريرة التحسين
 */
class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;
    
    virtual std::string name() const = 0;
    virtual bool run(SirFunction& func, OptimizationStats& stats) = 0;
};

// =============================================================================
// إزالة إعادة الاستعارة الزائدة
// =============================================================================

/**
 * إزالة Reborrow الزائدة
 */
class RemoveRedundantReborrows : public OptimizationPass {
public:
    std::string name() const override {
        return "إزالة إعادة الاستعارة الزائدة";
    }
    
    bool run(SirFunction& func, OptimizationStats& stats) override {
        bool changed = false;
        
        for (auto& block : func.blocks) {
            std::vector<SirInstruction> newInsts;
            std::map<ValueId, ValueId> reborrowMap;
            
            for (auto& inst : block.instructions) {
                if (inst.opcode == Opcode::Reborrow && inst.result) {
                    // تتبع إعادة الاستعارة
                    auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
                    reborrowMap[*inst.result] = srcOp.id;
                    stats.removedReborrows++;
                    stats.removedInstructions++;
                    changed = true;
                    continue;
                }
                
                // استبدال المراجع إلى Reborrow
                for (auto& op : inst.operands) {
                    if (auto* valOp = std::get_if<ValueOperand>(&op)) {
                        auto it = reborrowMap.find(valOp->id);
                        if (it != reborrowMap.end()) {
                            valOp->id = it->second;
                        }
                    }
                }
                
                newInsts.push_back(inst);
            }
            
            block.instructions = std::move(newInsts);
        }
        
        return changed;
    }
};

// =============================================================================
// تحويل Copy إلى Move
// =============================================================================

/**
 * تحويل Copy إلى Move عندما يكون المصدر غير مستخدم بعدها
 */
class CopyToMoveConversion : public OptimizationPass {
public:
    std::string name() const override {
        return "تحويل Copy إلى Move";
    }
    
    bool run(SirFunction& func, OptimizationStats& stats) override {
        bool changed = false;
        
        // تحليل استخدام القيم
        std::map<ValueId, int> useCount;
        
        for (const auto& block : func.blocks) {
            for (const auto& inst : block.instructions) {
                for (const auto& op : inst.operands) {
                    if (auto* valOp = std::get_if<ValueOperand>(&op)) {
                        useCount[valOp->id]++;
                    }
                }
            }
        }
        
        // تحويل Copy إلى Move
        for (auto& block : func.blocks) {
            for (auto& inst : block.instructions) {
                if (inst.opcode == Opcode::Copy && !inst.operands.empty()) {
                    auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
                    
                    // إذا كان المصدر يُستخدم مرة واحدة فقط (هنا)
                    if (useCount[srcOp.id] == 1) {
                        inst.opcode = Opcode::Move;
                        inst.comment = "تحويل من Copy";
                        stats.copyToMoveConversions++;
                        changed = true;
                    }
                }
            }
        }
        
        return changed;
    }
};

// =============================================================================
// حذف Drop الميت
// =============================================================================

/**
 * حذف Drop للقيم المنقولة
 */
class RemoveDeadDrops : public OptimizationPass {
public:
    std::string name() const override {
        return "حذف الإسقاط الميت";
    }
    
    bool run(SirFunction& func, OptimizationStats& stats) override {
        bool changed = false;
        
        // تتبع القيم المنقولة
        std::set<ValueId> movedValues;
        
        for (auto& block : func.blocks) {
            for (const auto& inst : block.instructions) {
                if (inst.opcode == Opcode::Move && !inst.operands.empty()) {
                    auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
                    movedValues.insert(srcOp.id);
                }
            }
        }
        
        // حذف Drop للقيم المنقولة
        for (auto& block : func.blocks) {
            std::vector<SirInstruction> newInsts;
            
            for (auto& inst : block.instructions) {
                if (inst.opcode == Opcode::Drop && !inst.operands.empty()) {
                    auto& valOp = std::get<ValueOperand>(inst.operands[0]);
                    
                    if (movedValues.count(valOp.id)) {
                        // القيمة منقولة، لا حاجة للإسقاط
                        stats.removedDeadDrops++;
                        stats.removedInstructions++;
                        changed = true;
                        continue;
                    }
                }
                
                newInsts.push_back(inst);
            }
            
            block.instructions = std::move(newInsts);
        }
        
        return changed;
    }
};

// =============================================================================
// حذف القيم غير المستخدمة
// =============================================================================

/**
 * حذف القيم التي لا تُستخدم أبداً
 */
class RemoveUnusedValues : public OptimizationPass {
public:
    std::string name() const override {
        return "حذف القيم غير المستخدمة";
    }
    
    bool run(SirFunction& func, OptimizationStats& stats) override {
        bool changed = false;
        
        // تحليل استخدام القيم
        std::set<ValueId> usedValues;
        
        for (const auto& block : func.blocks) {
            for (const auto& inst : block.instructions) {
                for (const auto& op : inst.operands) {
                    if (auto* valOp = std::get_if<ValueOperand>(&op)) {
                        usedValues.insert(valOp->id);
                    }
                }
            }
        }
        
        // حذف التعليمات التي تُنتج قيماً غير مستخدمة
        for (auto& block : func.blocks) {
            std::vector<SirInstruction> newInsts;
            
            for (auto& inst : block.instructions) {
                // لا نحذف التعليمات ذات الآثار الجانبية
                if (hasSideEffects(inst.opcode)) {
                    newInsts.push_back(inst);
                    continue;
                }
                
                if (inst.result && !usedValues.count(*inst.result)) {
                    stats.removedUnusedValues++;
                    stats.removedInstructions++;
                    changed = true;
                    continue;
                }
                
                newInsts.push_back(inst);
            }
            
            block.instructions = std::move(newInsts);
        }
        
        return changed;
    }
    
private:
    bool hasSideEffects(Opcode op) {
        switch (op) {
            case Opcode::Store:
            case Opcode::Drop:
            case Opcode::Call:
            case Opcode::Return:
            case Opcode::Jump:
            case Opcode::Branch:
                return true;
            default:
                return false;
        }
    }
};

// =============================================================================
// طي الثوابت
// =============================================================================

/**
 * طي العمليات على الثوابت
 */
class ConstantFolding : public OptimizationPass {
public:
    std::string name() const override {
        return "طي الثوابت";
    }
    
    bool run(SirFunction& func, OptimizationStats& stats) override {
        bool changed = false;
        
        // تتبع القيم الثابتة
        std::map<ValueId, int64_t> constants;
        
        for (auto& block : func.blocks) {
            for (auto& inst : block.instructions) {
                // تحقق من العمليات الحسابية
                if (isArithmetic(inst.opcode) && inst.result && 
                    inst.operands.size() >= 2) {
                    
                    auto& lhsOp = std::get<ValueOperand>(inst.operands[0]);
                    auto& rhsOp = std::get<ValueOperand>(inst.operands[1]);
                    
                    auto lhsIt = constants.find(lhsOp.id);
                    auto rhsIt = constants.find(rhsOp.id);
                    
                    if (lhsIt != constants.end() && rhsIt != constants.end()) {
                        // كلا المعاملين ثابتان
                        int64_t result = evaluateOp(inst.opcode, 
                                                     lhsIt->second, 
                                                     rhsIt->second);
                        constants[*inst.result] = result;
                        stats.foldedConstants++;
                        changed = true;
                    }
                }
            }
        }
        
        return changed;
    }
    
private:
    bool isArithmetic(Opcode op) {
        return op == Opcode::Add || op == Opcode::Sub ||
               op == Opcode::Mul || op == Opcode::Div ||
               op == Opcode::FloorDiv || op == Opcode::Mod;
    }
    
    int64_t evaluateOp(Opcode op, int64_t lhs, int64_t rhs) {
        switch (op) {
            case Opcode::Add: return lhs + rhs;
            case Opcode::Sub: return lhs - rhs;
            case Opcode::Mul: return lhs * rhs;
            case Opcode::Div: return rhs != 0 ? lhs / rhs : 0;
            case Opcode::FloorDiv: {
                if (rhs == 0) return 0;
                int64_t q = lhs / rhs;
                if ((lhs ^ rhs) < 0 && lhs % rhs != 0) q -= 1;
                return q;
            }
            case Opcode::Mod: return rhs != 0 ? lhs % rhs : 0;
            default:
                // (AR) عملية غير حسابية — تحذير + إرجاع 0
                // (EN) Non-arithmetic opcode — warn + return 0
                std::cerr << "[sadc تحذير] evaluateOp مع عملية غير حسابية: "
                          << static_cast<int>(op) << std::endl;
                return 0;
        }
    }
};

// =============================================================================
// مدير التحسين
// =============================================================================

/**
 * مدير تمريرات التحسين
 */
class SirOptimizer {
public:
    SirOptimizer() {
        // إضافة التمريرات الافتراضية
        addPass(std::make_unique<RemoveRedundantReborrows>());
        addPass(std::make_unique<CopyToMoveConversion>());
        addPass(std::make_unique<RemoveDeadDrops>());
        addPass(std::make_unique<RemoveUnusedValues>());
        addPass(std::make_unique<ConstantFolding>());
    }
    
    void addPass(std::unique_ptr<OptimizationPass> pass) {
        passes_.push_back(std::move(pass));
    }
    
    /**
     * تحسين دالة
     */
    OptimizationStats optimize(SirFunction& func, int maxIterations = 10) {
        OptimizationStats totalStats;
        
        for (int i = 0; i < maxIterations; i++) {
            bool changed = false;
            
            for (auto& pass : passes_) {
                if (pass->run(func, totalStats)) {
                    changed = true;
                }
            }
            
            if (!changed) break;
        }
        
        return totalStats;
    }
    
    /**
     * تحسين وحدة كاملة
     */
    OptimizationStats optimize(SirModule& module, int maxIterations = 10) {
        OptimizationStats totalStats;
        
        for (auto& func : module.functions) {
            auto funcStats = optimize(func, maxIterations);
            
            totalStats.removedInstructions += funcStats.removedInstructions;
            totalStats.copyToMoveConversions += funcStats.copyToMoveConversions;
            totalStats.removedReborrows += funcStats.removedReborrows;
            totalStats.removedDeadDrops += funcStats.removedDeadDrops;
            totalStats.foldedConstants += funcStats.foldedConstants;
            totalStats.removedUnusedValues += funcStats.removedUnusedValues;
        }
        
        return totalStats;
    }
    
private:
    std::vector<std::unique_ptr<OptimizationPass>> passes_;
};

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadSirOptimizer {
    sad::sir::SirOptimizer* optimizer;
    sad::sir::OptimizationStats lastStats;
};

SadSirOptimizer* sad_sir_optimizer_new() {
    auto* ctx = new SadSirOptimizer();
    ctx->optimizer = new sad::sir::SirOptimizer();
    return ctx;
}

int sad_sir_optimize_function(SadSirOptimizer* ctx, void* function) {
    if (!ctx || !function) return -1;  // (AR) -1 يعني خطأ، 0 يعني لا تحسينات / (EN) -1 means error, 0 means no optimizations
    
    auto* func = static_cast<sad::sir::SirFunction*>(function);
    ctx->lastStats = ctx->optimizer->optimize(*func);
    
    return ctx->lastStats.removedInstructions;
}

int sad_sir_optimize_module(SadSirOptimizer* ctx, void* module) {
    if (!ctx || !module) return -1;  // (AR) -1 يعني خطأ / (EN) -1 means error
    
    auto* mod = static_cast<sad::sir::SirModule*>(module);
    ctx->lastStats = ctx->optimizer->optimize(*mod);
    
    return ctx->lastStats.removedInstructions;
}

const char* sad_sir_optimizer_stats(SadSirOptimizer* ctx) {
    // (AR) استخدام thread_local بدلاً من static لتجنب سباق البيانات
    // (EN) Use thread_local instead of static to avoid data races
    thread_local std::string stats;
    if (!ctx) return "";
    stats = ctx->lastStats.toString();
    return stats.c_str();
}

void sad_sir_optimizer_free(SadSirOptimizer* ctx) {
    if (ctx) {
        delete ctx->optimizer;
        delete ctx;
    }
}

} // extern "C"
