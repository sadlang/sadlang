// بسم الله الرحمن الرحيم
// Function Inliner - موسّع الدوال المباشر
// Phase 6.4: Inlining & CSE
// الحمد لله رب العالمين

#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <functional>
#include "../ir/ir.h"

namespace sad {
namespace optimizer {

// Call Site Information - معلومات موقع الاستدعاء
struct CallSite {
    Instruction* call_inst;           // تعليمة الاستدعاء
    Function* caller;                 // الدالة المستدعية
    Function* callee;                 // الدالة المستدعاة
    BasicBlock* call_block;           // الكتلة التي تحتوي الاستدعاء
    std::vector<Value*> arguments;    // المعاملات
    
    // Is call in loop? - هل الاستدعاء في حلقة؟
    bool in_loop = false;
    
    // Call depth - عمق الاستدعاء
    int depth = 0;
};

// Inlining Heuristics - معايير التوسيع
class InliningHeuristics {
public:
    struct Metrics {
        size_t instruction_count = 0;    // عدد التعليمات
        size_t basic_block_count = 0;    // عدد الكتل الأساسية
        size_t call_count = 0;           // عدد الاستدعاءات
        bool has_recursion = false;      // يحتوي تكرار؟
        bool has_loops = false;          // يحتوي حلقات؟
        int nesting_depth = 0;           // عمق التداخل
    };
    
    // Calculate metrics for function - حساب المقاييس
    static Metrics calculate_metrics(Function* func);
    
    // Should inline? - هل يجب التوسيع؟
    static bool should_inline(
        const CallSite& site,
        const Metrics& callee_metrics,
        int max_inline_size,
        bool aggressive
    );
    
    // Calculate inline cost - حساب تكلفة التوسيع
    static int calculate_inline_cost(
        const CallSite& site,
        const Metrics& callee_metrics
    );
    
    // Calculate inline benefit - حساب فائدة التوسيع
    static int calculate_inline_benefit(
        const CallSite& site,
        const Metrics& callee_metrics
    );
};

// Function Inliner - موسّع الدوال
class FunctionInliner {
public:
    struct Config {
        bool enable_inlining = true;
        int max_inline_size = 50;         // أقصى حجم للدالة القابلة للتوسيع
        int max_inline_depth = 3;         // أقصى عمق للتوسيع
        bool inline_hot_functions = true; // توسيع الدوال الساخنة
        bool inline_small_functions = true; // توسيع الدوال الصغيرة
        bool aggressive = false;          // وضع متقدم
        
        // Size thresholds - حدود الحجم
        int small_function_size = 10;     // حجم الدالة الصغيرة
        int hot_function_size = 30;       // حجم الدالة الساخنة
    };
    
    explicit FunctionInliner(Config config = {}) : config_(config) {}
    
    // Inline function at call site - توسيع الدالة في موقع الاستدعاء
    bool inline_call(const CallSite& site);
    
    // Inline all eligible calls in function - توسيع كل الاستدعاءات المؤهلة
    bool inline_calls(Function* function);
    
    // Can inline? - هل يمكن التوسيع؟
    bool can_inline(const CallSite& site) const;
    
    // Get configuration - الحصول على الإعدادات
    const Config& get_config() const { return config_; }
    void set_config(const Config& config) { config_ = config; }
    
private:
    Config config_;
    
    // Clone function body - نسخ جسم الدالة
    void clone_function_body(const CallSite& site);
    
    // Map arguments to parameters - ربط المعاملات بالمعاملات الشكلية
    void map_arguments(
        const CallSite& site,
        std::unordered_map<Value*, Value*>& value_map
    );
    
    // Replace return with result - استبدال الإرجاع بالنتيجة
    void replace_returns(
        const CallSite& site,
        const std::vector<Instruction*>& cloned_instructions
    );
    
    // Check for recursion - فحص التكرار
    bool is_recursive(Function* func) const;
};

// Common Subexpression Elimination - إزالة التعبيرات المشتركة
class CSE {
public:
    struct Config {
        bool enable_cse = true;
        bool enable_global_cse = true;    // CSE عبر الكتل
        bool enable_load_cse = true;      // CSE للتحميلات
    };
    
    explicit CSE(Config config = {}) : config_(config) {}
    
    // Apply CSE to function - تطبيق CSE على الدالة
    bool optimize(Function* function);
    
    // Apply CSE to basic block - تطبيق CSE على كتلة
    bool optimize_block(BasicBlock* block);
    
    // Get configuration - الحصول على الإعدادات
    const Config& get_config() const { return config_; }
    void set_config(const Config& config) { config_ = config; }
    
private:
    Config config_;
    
    // Expression hash - هاش التعبير
    struct ExpressionHash {
        size_t operator()(const Instruction* inst) const;
    };
    
    // Expression equality - تساوي التعبيرات
    struct ExpressionEqual {
        bool operator()(const Instruction* a, const Instruction* b) const;
    };
    
    // Available expressions - التعبيرات المتاحة
    std::unordered_map<
        const Instruction*,
        Value*,
        ExpressionHash,
        ExpressionEqual
    > available_expressions_;
    
    // Find equivalent expression - إيجاد تعبير مكافئ
    Value* find_equivalent(Instruction* inst);
    
    // Is expression available? - هل التعبير متاح؟
    bool is_available(Instruction* inst) const;
    
    // Can eliminate? - هل يمكن الإزالة؟
    bool can_eliminate(Instruction* inst) const;
};

// Copy Propagation - نشر النسخ
class CopyPropagation {
public:
    struct Config {
        bool enable_copy_propagation = true;
        bool enable_forward_propagation = true;  // النشر الأمامي
        bool enable_backward_propagation = false; // النشر الخلفي
    };
    
    explicit CopyPropagation(Config config = {}) : config_(config) {}
    
    // Apply copy propagation - تطبيق نشر النسخ
    bool optimize(Function* function);
    
    // Get configuration - الحصول على الإعدادات
    const Config& get_config() const { return config_; }
    void set_config(const Config& config) { config_ = config; }
    
private:
    Config config_;
    
    // Copy map - خريطة النسخ
    std::unordered_map<Value*, Value*> copy_map_;
    
    // Build copy map - بناء خريطة النسخ
    void build_copy_map(Function* function);
    
    // Is copy instruction? - هل تعليمة نسخ؟
    bool is_copy(Instruction* inst) const;
    
    // Propagate copies - نشر النسخ
    bool propagate_copies(Function* function);
    
    // Get copy source - الحصول على مصدر النسخ
    Value* get_copy_source(Value* value) const;
};

// Dead Store Elimination - إزالة التخزين الميت
class DeadStoreElimination {
public:
    struct Config {
        bool enable_dse = true;
        bool aggressive = false;
    };
    
    explicit DeadStoreElimination(Config config = {}) : config_(config) {}
    
    // Apply DSE - تطبيق DSE
    bool optimize(Function* function);
    
private:
    Config config_;
    
    // Is dead store? - هل تخزين ميت؟
    bool is_dead_store(Instruction* store) const;
    
    // Find next use - إيجاد الاستخدام التالي
    Instruction* find_next_use(Value* value, Instruction* after) const;
};

// Inlining & CSE Optimizer - المحسّن الشامل
class InliningCSEOptimizer {
public:
    struct Config {
        bool enable_inlining = true;
        bool enable_cse = true;
        bool enable_copy_propagation = true;
        bool enable_dse = true;
        
        int max_iterations = 3;
        
        FunctionInliner::Config inliner_config;
        CSE::Config cse_config;
        CopyPropagation::Config copy_prop_config;
        DeadStoreElimination::Config dse_config;
    };
    
    struct Statistics {
        int functions_inlined = 0;
        int call_sites_inlined = 0;
        int expressions_eliminated = 0;
        int copies_propagated = 0;
        int dead_stores_eliminated = 0;
        
        int total_optimizations() const {
            return call_sites_inlined + expressions_eliminated +
                   copies_propagated + dead_stores_eliminated;
        }
        
        std::string to_string() const;
    };
    
    explicit InliningCSEOptimizer(Config config = {}) : config_(config) {}
    
    // Main optimization entry point - نقطة الدخول الرئيسية
    bool optimize(Function* function);
    
    // Optimize entire module - تحسين الوحدة كاملة
    bool optimize_module(Module* module);
    
    // Get configuration - الحصول على الإعدادات
    const Config& get_config() const { return config_; }
    void set_config(const Config& config) { config_ = config; }
    
    // Get statistics - الحصول على الإحصائيات
    const Statistics& get_stats() const { return stats_; }
    void reset_stats() { stats_ = Statistics{}; }
    
private:
    Config config_;
    Statistics stats_;
    
    // Sub-optimizers - المحسّنات الفرعية
    std::unique_ptr<FunctionInliner> inliner_;
    std::unique_ptr<CSE> cse_;
    std::unique_ptr<CopyPropagation> copy_prop_;
    std::unique_ptr<DeadStoreElimination> dse_;
    
    // Optimization phases - مراحل التحسين
    bool apply_inlining(Function* function);
    bool apply_cse(Function* function);
    bool apply_copy_propagation(Function* function);
    bool apply_dse(Function* function);
    
    // Find call sites - إيجاد مواقع الاستدعاء
    std::vector<CallSite> find_call_sites(Function* function);
    
    // Build call graph - بناء رسم الاستدعاء
    void build_call_graph(Module* module);
};

// Factory functions - دوال المصنع

// Create standard inlining optimizer - إنشاء محسّن قياسي
inline std::unique_ptr<InliningCSEOptimizer> create_standard_inlining_optimizer() {
    InliningCSEOptimizer::Config config;
    config.enable_inlining = true;
    config.enable_cse = true;
    config.enable_copy_propagation = true;
    config.enable_dse = true;
    
    config.inliner_config.max_inline_size = 50;
    config.inliner_config.inline_small_functions = true;
    
    return std::make_unique<InliningCSEOptimizer>(config);
}

// Create aggressive inlining optimizer - إنشاء محسّن متقدم
inline std::unique_ptr<InliningCSEOptimizer> create_aggressive_inlining_optimizer() {
    InliningCSEOptimizer::Config config;
    config.enable_inlining = true;
    config.enable_cse = true;
    config.enable_copy_propagation = true;
    config.enable_dse = true;
    config.max_iterations = 5;
    
    config.inliner_config.max_inline_size = 100;
    config.inliner_config.aggressive = true;
    config.inliner_config.inline_hot_functions = true;
    
    config.cse_config.enable_global_cse = true;
    config.cse_config.enable_load_cse = true;
    
    return std::make_unique<InliningCSEOptimizer>(config);
}

// Create conservative inlining optimizer - إنشاء محسّن محافظ
inline std::unique_ptr<InliningCSEOptimizer> create_conservative_inlining_optimizer() {
    InliningCSEOptimizer::Config config;
    config.enable_inlining = true;
    config.enable_cse = true;
    config.enable_copy_propagation = false;
    config.enable_dse = false;
    
    config.inliner_config.max_inline_size = 20;
    config.inliner_config.inline_small_functions = true;
    config.inliner_config.inline_hot_functions = false;
    
    return std::make_unique<InliningCSEOptimizer>(config);
}

} // namespace optimizer
} // namespace sad
