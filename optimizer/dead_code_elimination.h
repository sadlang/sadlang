// بسم الله الرحمن الرحيم
// ===================================================================
// Dead Code Elimination Optimizer - محسّن إزالة الكود الميت
// Phase 6.1: Dead Code Elimination
// ===================================================================
// الحمد لله رب العالمين

#ifndef SAD_OPTIMIZER_DEAD_CODE_ELIMINATION_H
#define SAD_OPTIMIZER_DEAD_CODE_ELIMINATION_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>

namespace sad {
namespace optimizer {

// ===================================================================
// Forward Declarations
// ===================================================================

class BasicBlock;
class Function;
class Instruction;
class Value;
class CFG;  // Control Flow Graph

// ===================================================================
// Dead Code Elimination Pass - ممر إزالة الكود الميت
// ===================================================================

/**
 * Dead Code Elimination Optimizer
 * محسّن إزالة الكود الميت
 * 
 * يقوم بإزالة:
 * 1. Unreachable code - الكود غير القابل للوصول
 * 2. Unused variables - المتغيرات غير المستخدمة
 * 3. Dead stores - التخزينات الميتة
 * 4. Unnecessary computations - الحسابات غير الضرورية
 * 
 * Removes:
 * 1. Unreachable code
 * 2. Unused variables
 * 3. Dead stores
 * 4. Unnecessary computations
 */
class DeadCodeElimination {
public:
    DeadCodeElimination() = default;
    ~DeadCodeElimination() = default;

    // Main optimization entry point - نقطة الدخول الرئيسية
    bool optimize(Function* function);
    bool optimize_module(std::vector<Function*>& functions);

    // Statistics - الإحصائيات
    struct Stats {
        size_t unreachable_blocks_removed = 0;    // كتل غير قابلة للوصول
        size_t dead_instructions_removed = 0;     // تعليمات ميتة
        size_t unused_variables_removed = 0;      // متغيرات غير مستخدمة
        size_t dead_stores_removed = 0;           // تخزينات ميتة
        size_t total_optimizations = 0;           // مجموع التحسينات

        void reset();
        std::string to_string() const;
    };

    const Stats& get_stats() const { return stats_; }
    void reset_stats() { stats_.reset(); }

    // Configuration - الإعدادات
    struct Config {
        bool remove_unreachable = true;      // إزالة الكود غير القابل للوصول
        bool remove_dead_stores = true;      // إزالة التخزينات الميتة
        bool remove_unused_vars = true;      // إزالة المتغيرات غير المستخدمة
        bool aggressive_mode = false;        // الوضع العدواني
        size_t max_iterations = 10;         // أقصى عدد من التكرارات
    };

    void set_config(const Config& config) { config_ = config; }
    const Config& get_config() const { return config_; }

private:
    // ===============================================================
    // Phase 1: Unreachable Code Removal - إزالة الكود غير القابل للوصول
    // ===============================================================

    /**
     * Remove unreachable basic blocks
     * إزالة الكتل الأساسية غير القابلة للوصول
     */
    bool remove_unreachable_blocks(Function* function);

    /**
     * Mark reachable blocks using DFS
     * تحديد الكتل القابلة للوصول باستخدام DFS
     */
    void mark_reachable_blocks(
        BasicBlock* start,
        std::unordered_set<BasicBlock*>& reachable
    );

    /**
     * Check if block is reachable
     * فحص إذا كانت الكتلة قابلة للوصول
     */
    bool is_reachable(BasicBlock* block, const std::unordered_set<BasicBlock*>& reachable);

    // ===============================================================
    // Phase 2: Dead Instruction Elimination - إزالة التعليمات الميتة
    // ===============================================================

    /**
     * Remove dead instructions (instructions with no uses)
     * إزالة التعليمات الميتة (تعليمات بدون استخدامات)
     */
    bool remove_dead_instructions(Function* function);

    /**
     * Check if instruction is dead
     * فحص إذا كانت التعليمة ميتة
     */
    bool is_dead_instruction(Instruction* inst);

    /**
     * Check if instruction has side effects
     * فحص إذا كانت للتعليمة آثار جانبية
     */
    bool has_side_effects(Instruction* inst);

    /**
     * Mark live instructions (backward analysis)
     * تحديد التعليمات الحية (تحليل عكسي)
     */
    void mark_live_instructions(
        Function* function,
        std::unordered_set<Instruction*>& live
    );

    // ===============================================================
    // Phase 3: Unused Variable Elimination - إزالة المتغيرات غير المستخدمة
    // ===============================================================

    /**
     * Remove unused local variables
     * إزالة المتغيرات المحلية غير المستخدمة
     */
    bool remove_unused_variables(Function* function);

    /**
     * Find all variable uses
     * إيجاد جميع استخدامات المتغير
     */
    size_t count_variable_uses(Value* variable, Function* function);

    /**
     * Check if variable is used
     * فحص إذا كان المتغير مستخدماً
     */
    bool is_variable_used(Value* variable, Function* function);

    // ===============================================================
    // Phase 4: Dead Store Elimination - إزالة التخزينات الميتة
    // ===============================================================

    /**
     * Remove dead stores (stores that are overwritten before use)
     * إزالة التخزينات الميتة (تخزينات يتم الكتابة عليها قبل الاستخدام)
     */
    bool remove_dead_stores(Function* function);

    /**
     * Check if store is dead
     * فحص إذا كان التخزين ميتاً
     */
    bool is_dead_store(Instruction* store, Function* function);

    /**
     * Find next use or store of variable
     * إيجاد الاستخدام أو التخزين التالي للمتغير
     */
    Instruction* find_next_use_or_store(
        Value* variable,
        BasicBlock* block,
        Instruction* after
    );

    // ===============================================================
    // Control Flow Analysis - تحليل تدفق التحكم
    // ===============================================================

    /**
     * Build Control Flow Graph
     * بناء رسم بياني لتدفق التحكم
     */
    std::unique_ptr<CFG> build_cfg(Function* function);

    /**
     * Get successors of a block
     * الحصول على الكتل التالية
     */
    std::vector<BasicBlock*> get_successors(BasicBlock* block);

    /**
     * Get predecessors of a block
     * الحصول على الكتل السابقة
     */
    std::vector<BasicBlock*> get_predecessors(BasicBlock* block);

    // ===============================================================
    // Use-Def Analysis - تحليل الاستخدام-التعريف
    // ===============================================================

    /**
     * Build use-def chains
     * بناء سلاسل الاستخدام-التعريف
     */
    void build_use_def_chains(Function* function);

    /**
     * Get all uses of a value
     * الحصول على جميع استخدامات القيمة
     */
    std::vector<Instruction*> get_uses(Value* value);

    /**
     * Get definition of a value
     * الحصول على تعريف القيمة
     */
    Instruction* get_definition(Value* value);

    // ===============================================================
    // Utility Functions - دوال مساعدة
    // ===============================================================

    /**
     * Remove instruction from block
     * إزالة تعليمة من الكتلة
     */
    void remove_instruction(Instruction* inst, BasicBlock* block);

    /**
     * Remove block from function
     * إزالة كتلة من الدالة
     */
    void remove_block(BasicBlock* block, Function* function);

    /**
     * Update CFG after removal
     * تحديث CFG بعد الإزالة
     */
    void update_cfg_after_removal(BasicBlock* removed_block);

    /**
     * Print optimization info
     * طباعة معلومات التحسين
     */
    void print_optimization(const std::string& message);

    /**
     * Run iterative optimization
     * تشغيل التحسين التكراري
     */
    bool run_iterative_optimization(Function* function);

private:
    // Configuration
    Config config_;

    // Statistics
    Stats stats_;

    // Use-Def chains
    std::unordered_map<Value*, std::vector<Instruction*>> use_chains_;
    std::unordered_map<Value*, Instruction*> def_map_;

    // CFG cache
    std::unique_ptr<CFG> cfg_;

    // Debug/logging
    bool verbose_ = false;
};

// ===================================================================
// Control Flow Graph - رسم بياني لتدفق التحكم
// ===================================================================

class CFG {
public:
    CFG() = default;

    // Add edge
    void add_edge(BasicBlock* from, BasicBlock* to);

    // Remove edge
    void remove_edge(BasicBlock* from, BasicBlock* to);

    // Query
    std::vector<BasicBlock*> get_successors(BasicBlock* block) const;
    std::vector<BasicBlock*> get_predecessors(BasicBlock* block) const;
    bool has_edge(BasicBlock* from, BasicBlock* to) const;

    // Entry/Exit blocks
    void set_entry(BasicBlock* entry) { entry_ = entry; }
    void set_exit(BasicBlock* exit) { exit_ = exit; }
    BasicBlock* get_entry() const { return entry_; }
    BasicBlock* get_exit() const { return exit_; }

    // Traversal
    std::vector<BasicBlock*> get_all_blocks() const;
    std::vector<BasicBlock*> get_reachable_blocks() const;

private:
    std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> successors_;
    std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> predecessors_;
    BasicBlock* entry_ = nullptr;
    BasicBlock* exit_ = nullptr;
};

// ===================================================================
// Optimization Pass Manager - مدير ممرات التحسين
// ===================================================================

class DCEPassManager {
public:
    DCEPassManager() = default;

    // Add pass
    void add_pass(std::unique_ptr<DeadCodeElimination> pass);

    // Run all passes
    bool run_on_function(Function* function);
    bool run_on_module(std::vector<Function*>& functions);

    // Get combined statistics
    DeadCodeElimination::Stats get_combined_stats() const;

private:
    std::vector<std::unique_ptr<DeadCodeElimination>> passes_;
};

// ===================================================================
// Utility Functions - دوال مساعدة عامة
// ===================================================================

/**
 * Create standard DCE optimizer
 * إنشاء محسّن DCE قياسي
 */
std::unique_ptr<DeadCodeElimination> create_standard_dce();

/**
 * Create aggressive DCE optimizer
 * إنشاء محسّن DCE عدواني
 */
std::unique_ptr<DeadCodeElimination> create_aggressive_dce();

/**
 * Quick check if function needs DCE
 * فحص سريع إذا كانت الدالة تحتاج DCE
 */
bool needs_dead_code_elimination(Function* function);

} // namespace optimizer
} // namespace sad

#endif // SAD_OPTIMIZER_DEAD_CODE_ELIMINATION_H
