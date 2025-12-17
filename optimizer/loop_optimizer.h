// بسم الله الرحمن الرحيم
// Loop Optimizer - محسّن الحلقات
// Phase 6.3: Loop Optimizations
// الحمد لله رب العالمين

#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <optional>
#include <functional>
#include "../ir/ir.h"

namespace sad {
namespace optimizer {

// Loop Information - معلومات الحلقة
// Represents a natural loop in the control flow graph
struct LoopInfo {
    // Loop header - رأس الحلقة (entry point)
    BasicBlock* header = nullptr;
    
    // Loop latch - نهاية الحلقة (back edge source)
    BasicBlock* latch = nullptr;
    
    // Loop exit blocks - كتل الخروج
    std::vector<BasicBlock*> exits;
    
    // Loop body blocks - كتل جسم الحلقة
    std::unordered_set<BasicBlock*> body;
    
    // Parent loop - الحلقة الأم (for nested loops)
    LoopInfo* parent = nullptr;
    
    // Nested loops - الحلقات المتداخلة
    std::vector<std::unique_ptr<LoopInfo>> nested_loops;
    
    // Loop depth - عمق التداخل
    int depth = 0;
    
    // Trip count - عدد التكرارات (if known)
    std::optional<int64_t> trip_count;
    
    // Induction variable - متغير التحريض
    Value* induction_var = nullptr;
    
    // Loop invariants - القيم الثابتة داخل الحلقة
    std::unordered_set<Value*> invariants;
    
    // Is loop innermost? - هل الحلقة الأعمق؟
    bool is_innermost() const {
        return nested_loops.empty();
    }
    
    // Contains block? - هل تحتوي الكتلة؟
    bool contains(BasicBlock* block) const {
        return body.find(block) != body.end();
    }
    
    // Get loop size - حجم الحلقة
    size_t size() const {
        return body.size();
    }
};

// Loop Detection - كشف الحلقات
// Detects natural loops using dominance information
class LoopDetector {
public:
    LoopDetector() = default;
    
    // Detect all loops in function - كشف جميع الحلقات
    std::vector<std::unique_ptr<LoopInfo>> detect_loops(Function* function);
    
private:
    // Find back edges - إيجاد الحواف الراجعة
    std::vector<std::pair<BasicBlock*, BasicBlock*>> find_back_edges(Function* function);
    
    // Build loop from back edge - بناء حلقة من حافة راجعة
    std::unique_ptr<LoopInfo> build_loop(BasicBlock* header, BasicBlock* latch);
    
    // Compute dominators - حساب المسيطرات
    void compute_dominators(Function* function);
    
    // Does A dominate B? - هل A يسيطر على B؟
    bool dominates(BasicBlock* a, BasicBlock* b) const;
    
    // Find loop body using BFS - إيجاد جسم الحلقة
    void find_loop_body(LoopInfo* loop);
    
    // Dominators map - خريطة المسيطرات
    std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>> dominators_;
};

// Loop Unroller - موسّع الحلقات
// Unrolls loops by replicating the loop body
class LoopUnroller {
public:
    struct Config {
        bool enable_unrolling = true;
        int max_unroll_factor = 8;        // أقصى عامل توسيع
        int max_unroll_body_size = 50;    // أقصى حجم جسم للتوسيع
        bool allow_partial_unroll = true; // السماح بالتوسيع الجزئي
    };
    
    explicit LoopUnroller(Config config = {}) : config_(config) {}
    
    // Unroll loop - توسيع الحلقة
    bool unroll(LoopInfo* loop);
    
    // Can unroll? - هل يمكن التوسيع؟
    bool can_unroll(LoopInfo* loop) const;
    
    // Get unroll factor - الحصول على عامل التوسيع
    int get_unroll_factor(LoopInfo* loop) const;
    
private:
    Config config_;
    
    // Clone loop body - نسخ جسم الحلقة
    void clone_loop_body(LoopInfo* loop, int factor);
    
    // Update loop bounds - تحديث حدود الحلقة
    void update_loop_bounds(LoopInfo* loop, int factor);
};

// Loop Fusion - دمج الحلقات
// Merges adjacent loops with compatible iteration spaces
class LoopFusion {
public:
    struct Config {
        bool enable_fusion = true;
        bool check_dependencies = true; // فحص التبعيات
    };
    
    explicit LoopFusion(Config config = {}) : config_(config) {}
    
    // Fuse adjacent loops - دمج حلقات متجاورة
    bool fuse(LoopInfo* loop1, LoopInfo* loop2);
    
    // Can fuse? - هل يمكن الدمج؟
    bool can_fuse(LoopInfo* loop1, LoopInfo* loop2) const;
    
private:
    Config config_;
    
    // Check loop compatibility - فحص التوافق
    bool are_compatible(LoopInfo* loop1, LoopInfo* loop2) const;
    
    // Merge loop bodies - دمج أجسام الحلقات
    void merge_bodies(LoopInfo* loop1, LoopInfo* loop2);
};

// Loop Fission - تقسيم الحلقات
// Splits a loop into multiple loops for better cache locality
class LoopFission {
public:
    struct Config {
        bool enable_fission = true;
        int min_split_size = 5; // الحد الأدنى لحجم القسم
    };
    
    explicit LoopFission(Config config = {}) : config_(config) {}
    
    // Split loop - تقسيم الحلقة
    std::vector<std::unique_ptr<LoopInfo>> split(LoopInfo* loop);
    
    // Can split? - هل يمكن التقسيم؟
    bool can_split(LoopInfo* loop) const;
    
private:
    Config config_;
    
    // Find split points - إيجاد نقاط التقسيم
    std::vector<int> find_split_points(LoopInfo* loop) const;
    
    // Create split loops - إنشاء حلقات منقسمة
    std::unique_ptr<LoopInfo> create_split_loop(
        LoopInfo* original,
        int start_idx,
        int end_idx
    );
};

// Loop-Invariant Code Motion (LICM) - نقل الكود الثابت خارج الحلقة
// Moves loop-invariant computations outside the loop
class LICM {
public:
    struct Config {
        bool enable_licm = true;
        bool aggressive = false; // وضع متقدم
    };
    
    explicit LICM(Config config = {}) : config_(config) {}
    
    // Apply LICM - تطبيق نقل الكود
    bool optimize(LoopInfo* loop);
    
    // Find invariants - إيجاد الثوابت
    std::unordered_set<Value*> find_invariants(LoopInfo* loop);
    
    // Is invariant? - هل ثابت؟
    bool is_invariant(Value* value, LoopInfo* loop) const;
    
private:
    Config config_;
    
    // Hoist instruction - رفع التعليمة
    bool hoist(Instruction* inst, LoopInfo* loop);
    
    // Can hoist? - هل يمكن الرفع؟
    bool can_hoist(Instruction* inst, LoopInfo* loop) const;
    
    // Create preheader - إنشاء كتلة ما قبل الرأس
    BasicBlock* create_preheader(LoopInfo* loop);
};

// Induction Variable Analysis - تحليل متغيرات التحريض
// Analyzes and simplifies induction variables
class InductionVariableAnalysis {
public:
    struct InductionVariable {
        Value* variable;           // المتغير
        Value* start_value;        // القيمة الابتدائية
        Value* step_value;         // قيمة الخطوة
        std::string op;            // العملية ("+", "-", etc.)
        bool is_basic;             // متغير أساسي؟
        
        // Is simple increment? - هل زيادة بسيطة؟
        bool is_simple_increment() const {
            return op == "+" || op == "-";
        }
    };
    
    InductionVariableAnalysis() = default;
    
    // Analyze loop - تحليل الحلقة
    std::vector<InductionVariable> analyze(LoopInfo* loop);
    
    // Find basic induction variables - إيجاد متغيرات التحريض الأساسية
    std::vector<InductionVariable> find_basic_ivs(LoopInfo* loop);
    
    // Find derived induction variables - إيجاد متغيرات التحريض المشتقة
    std::vector<InductionVariable> find_derived_ivs(
        LoopInfo* loop,
        const std::vector<InductionVariable>& basic_ivs
    );
    
    // Simplify induction variables - تبسيط متغيرات التحريض
    bool simplify_ivs(LoopInfo* loop, const std::vector<InductionVariable>& ivs);
    
private:
    // Is induction variable? - هل متغير تحريض؟
    bool is_induction_variable(Value* value, LoopInfo* loop) const;
    
    // Compute trip count - حساب عدد التكرارات
    std::optional<int64_t> compute_trip_count(
        const InductionVariable& iv,
        LoopInfo* loop
    ) const;
};

// Loop Optimizer - محسّن الحلقات الرئيسي
// Main loop optimization pass combining all techniques
class LoopOptimizer {
public:
    struct Config {
        bool enable_unrolling = true;
        bool enable_fusion = true;
        bool enable_fission = false;    // مُعطّل افتراضياً
        bool enable_licm = true;
        bool enable_iv_simplification = true;
        
        int max_unroll_factor = 8;
        int max_iterations = 3;         // الحد الأقصى للتكرارات
        
        LoopUnroller::Config unroll_config;
        LoopFusion::Config fusion_config;
        LoopFission::Config fission_config;
        LICM::Config licm_config;
    };
    
    struct Statistics {
        int loops_detected = 0;
        int loops_unrolled = 0;
        int loops_fused = 0;
        int loops_split = 0;
        int invariants_hoisted = 0;
        int ivs_simplified = 0;
        
        int total_optimizations() const {
            return loops_unrolled + loops_fused + loops_split +
                   invariants_hoisted + ivs_simplified;
        }
        
        std::string to_string() const;
    };
    
    explicit LoopOptimizer(Config config = {}) : config_(config) {}
    
    // Main optimization entry point - نقطة الدخول الرئيسية
    bool optimize(Function* function);
    
    // Get configuration - الحصول على الإعدادات
    const Config& get_config() const { return config_; }
    void set_config(const Config& config) { config_ = config; }
    
    // Get statistics - الحصول على الإحصائيات
    const Statistics& get_stats() const { return stats_; }
    void reset_stats() { stats_ = Statistics{}; }
    
private:
    Config config_;
    Statistics stats_;
    
    // Optimization phases - مراحل التحسين
    bool detect_loops(Function* function);
    bool apply_licm();
    bool apply_unrolling();
    bool apply_fusion();
    bool apply_fission();
    bool simplify_induction_variables();
    
    // Loop information - معلومات الحلقات
    std::vector<std::unique_ptr<LoopInfo>> loops_;
    
    // Sub-optimizers - المحسّنات الفرعية
    std::unique_ptr<LoopDetector> detector_;
    std::unique_ptr<LoopUnroller> unroller_;
    std::unique_ptr<LoopFusion> fusion_;
    std::unique_ptr<LoopFission> fission_;
    std::unique_ptr<LICM> licm_;
    std::unique_ptr<InductionVariableAnalysis> iv_analysis_;
};

// Factory functions - دوال المصنع

// Create standard loop optimizer - إنشاء محسّن قياسي
inline std::unique_ptr<LoopOptimizer> create_standard_loop_optimizer() {
    LoopOptimizer::Config config;
    config.enable_unrolling = true;
    config.enable_fusion = true;
    config.enable_fission = false;
    config.enable_licm = true;
    config.enable_iv_simplification = true;
    config.max_unroll_factor = 4;
    
    return std::make_unique<LoopOptimizer>(config);
}

// Create aggressive loop optimizer - إنشاء محسّن متقدم
inline std::unique_ptr<LoopOptimizer> create_aggressive_loop_optimizer() {
    LoopOptimizer::Config config;
    config.enable_unrolling = true;
    config.enable_fusion = true;
    config.enable_fission = true;
    config.enable_licm = true;
    config.enable_iv_simplification = true;
    config.max_unroll_factor = 8;
    config.max_iterations = 5;
    
    config.unroll_config.allow_partial_unroll = true;
    config.fusion_config.check_dependencies = true;
    config.licm_config.aggressive = true;
    
    return std::make_unique<LoopOptimizer>(config);
}

// Create conservative loop optimizer - إنشاء محسّن محافظ
inline std::unique_ptr<LoopOptimizer> create_conservative_loop_optimizer() {
    LoopOptimizer::Config config;
    config.enable_unrolling = true;
    config.enable_fusion = false;
    config.enable_fission = false;
    config.enable_licm = true;
    config.enable_iv_simplification = false;
    config.max_unroll_factor = 2;
    
    return std::make_unique<LoopOptimizer>(config);
}

} // namespace optimizer
} // namespace sad
