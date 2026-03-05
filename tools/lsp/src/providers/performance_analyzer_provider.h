// بسم الله الرحمن الرحيم
/**
 * ══════════════════════════════════════════════════════════════════════════════
 * ملف: performance_analyzer_provider.h
 * الوصف: محلل الأداء الفوري للغة ص
 * ══════════════════════════════════════════════════════════════════════════════
 * 
 * يوفر تحليلاً فورياً للأداء أثناء الكتابة:
 * - اكتشاف التعقيد الزمني
 * - تحديد تسربات الذاكرة المحتملة
 * - اقتراحات تحسين الخوارزميات
 * - تحليل استخدام الموارد
 * 
 * ══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "../../include/lsp_protocol_types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <chrono>

namespace sad {
namespace lsp {
namespace analysis {

// ══════════════════════════════════════════════════════════════════════════════
//  تعدادات التعقيد
// ══════════════════════════════════════════════════════════════════════════════

/**
 * التعقيد الزمني
 */
enum class TimeComplexity {
    Constant,       // O(1)
    Logarithmic,    // O(log n)
    Linear,         // O(n)
    Linearithmic,   // O(n log n)
    Quadratic,      // O(n²)
    Cubic,          // O(n³)
    Exponential,    // O(2^n)
    Factorial,      // O(n!)
    Unknown
};

/**
 * التعقيد المكاني
 */
enum class SpaceComplexity {
    Constant,       // O(1)
    Logarithmic,    // O(log n)
    Linear,         // O(n)
    Quadratic,      // O(n²)
    Unknown
};

/**
 * مستوى خطورة مشكلة الأداء
 */
enum class PerformanceSeverity {
    Info,           // معلومة
    Warning,        // تحذير
    Critical        // حرج
};

/**
 * نوع مشكلة الأداء
 */
enum class PerformanceIssueType {
    // تعقيد زمني
    NestedLoops,                // حلقات متداخلة
    IneffientSearch,            // بحث غير فعال
    RedundantComputation,       // حساب مكرر
    UnoptimizedRecursion,       // تكرار غير محسن
    
    // استخدام الذاكرة
    MemoryLeak,                 // تسرب ذاكرة
    LargeAllocation,            // تخصيص كبير
    UnreleasedResources,        // موارد غير محررة
    CyclicReference,            // مرجع دائري
    
    // سلاسل النصوص
    StringConcatInLoop,         // دمج نصوص في حلقة
    UnneededStringCopy,         // نسخ نص غير ضروري
    
    // الإدخال/الإخراج
    SynchronousIO,              // إدخال/إخراج متزامن
    UnbufferedIO,               // إدخال/إخراج غير مخزن
    
    // أنماط عامة
    PrematureOptimization,      // تحسين مبكر
    DeadCode,                   // كود ميت
    UnusedVariable              // متغير غير مستخدم
};

// ══════════════════════════════════════════════════════════════════════════════
//  الهياكل الأساسية
// ══════════════════════════════════════════════════════════════════════════════

/**
 * نطاق في الكود
 */
struct CodeRange {
    int start_line;
    int start_column;
    int end_line;
    int end_column;
};

/**
 * مشكلة أداء
 */
struct PerformanceIssue {
    std::string id;
    PerformanceIssueType type;
    PerformanceSeverity severity;
    std::string title;
    std::string description;
    std::string suggestion;
    CodeRange range;
    TimeComplexity time_complexity;
    SpaceComplexity space_complexity;
    std::optional<std::string> fix_code;
};

/**
 * تحليل دالة
 */
struct FunctionAnalysis {
    std::string name;
    CodeRange range;
    TimeComplexity time_complexity;
    SpaceComplexity space_complexity;
    int loop_depth;                     // عمق الحلقات
    int recursion_depth;                // عمق التكرار
    int branch_count;                   // عدد الفروع
    int variable_count;                 // عدد المتغيرات
    int line_count;                     // عدد الأسطر
    bool uses_recursion;
    bool has_tail_recursion;
    std::vector<PerformanceIssue> issues;
};

/**
 * تحليل وحدة (ملف)
 */
struct ModuleAnalysis {
    std::string path;
    int total_lines;
    int code_lines;
    int comment_lines;
    int blank_lines;
    int function_count;
    int class_count;
    double average_complexity;
    int max_loop_depth;
    std::vector<FunctionAnalysis> functions;
    std::vector<PerformanceIssue> issues;
};

/**
 * مخطط استخدام الذاكرة
 */
struct MemoryProfile {
    int allocation_count;               // عدد التخصيصات
    int deallocation_count;             // عدد التحريرات
    std::vector<std::string> potential_leaks;
    std::vector<std::string> large_allocations;
};

/**
 * نتيجة تحليل الأداء
 */
struct PerformanceReport {
    std::string document_uri;
    std::chrono::milliseconds analysis_time;
    ModuleAnalysis module;
    MemoryProfile memory;
    std::vector<PerformanceIssue> all_issues;
    
    // ملخص
    int info_count = 0;
    int warning_count = 0;
    int critical_count = 0;
    
    // تصنيف عام
    char grade = 'A';  // A, B, C, D, F
    std::string summary;
};

// ══════════════════════════════════════════════════════════════════════════════
//  محلل الأداء
// ══════════════════════════════════════════════════════════════════════════════

class PerformanceAnalyzerProvider {
public:
    PerformanceAnalyzerProvider();
    ~PerformanceAnalyzerProvider();
    
    // ════════════════════════════════════════════════════════════════════════
    //  التحليل الرئيسي
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * تحليل كامل لملف
     */
    PerformanceReport analyze(
        const std::string& document,
        const std::string& uri
    );
    
    /**
     * تحليل سريع للمنطقة المرئية فقط
     */
    std::vector<PerformanceIssue> analyze_visible_range(
        const std::string& document,
        int start_line,
        int end_line
    );
    
    /**
     * تحليل دالة محددة
     */
    FunctionAnalysis analyze_function(
        const std::string& function_code,
        const std::string& function_name
    );
    
    // ════════════════════════════════════════════════════════════════════════
    //  التحليل التفصيلي
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * حساب التعقيد الزمني
     */
    TimeComplexity calculate_time_complexity(const std::string& code);
    
    /**
     * حساب التعقيد المكاني
     */
    SpaceComplexity calculate_space_complexity(const std::string& code);
    
    /**
     * تحليل الحلقات
     */
    std::vector<PerformanceIssue> analyze_loops(const std::string& code);
    
    /**
     * تحليل الاستدعاءات التكرارية
     */
    std::vector<PerformanceIssue> analyze_recursion(const std::string& code);
    
    /**
     * تحليل استخدام الذاكرة
     */
    MemoryProfile analyze_memory_usage(const std::string& code);
    
    /**
     * البحث عن كود ميت
     */
    std::vector<PerformanceIssue> find_dead_code(const std::string& code);
    
    // ════════════════════════════════════════════════════════════════════════
    //  اكتشاف الأنماط السيئة
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * اكتشاف دمج النصوص في حلقات
     */
    std::vector<PerformanceIssue> detect_string_concat_in_loops(
        const std::string& code
    );
    
    /**
     * اكتشاف الحسابات المكررة
     */
    std::vector<PerformanceIssue> detect_redundant_computations(
        const std::string& code
    );
    
    /**
     * اكتشاف البحث غير الفعال
     */
    std::vector<PerformanceIssue> detect_inefficient_search(
        const std::string& code
    );
    
    /**
     * اكتشاف التكرار غير المحسن (بدون ذاكرة)
     */
    std::vector<PerformanceIssue> detect_unoptimized_recursion(
        const std::string& code
    );
    
    // ════════════════════════════════════════════════════════════════════════
    //  التحسينات المقترحة
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * اقتراح تحسينات لمشكلة معينة
     */
    std::string suggest_improvement(const PerformanceIssue& issue);
    
    /**
     * توليد كود محسن
     */
    std::optional<std::string> generate_optimized_code(
        const std::string& original_code,
        const PerformanceIssue& issue
    );
    
    /**
     * شرح مشكلة الأداء
     */
    std::string explain_issue(const PerformanceIssue& issue);
    
    // ════════════════════════════════════════════════════════════════════════
    //  الإعدادات
    // ════════════════════════════════════════════════════════════════════════
    
    void set_max_acceptable_complexity(TimeComplexity complexity) {
        max_acceptable_complexity_ = complexity;
    }
    
    void set_max_loop_depth(int depth) {
        max_loop_depth_ = depth;
    }
    
    void set_analyze_memory(bool enabled) {
        analyze_memory_ = enabled;
    }
    
    void set_strict_mode(bool strict) {
        strict_mode_ = strict;
    }
    
private:
    // الإعدادات
    TimeComplexity max_acceptable_complexity_ = TimeComplexity::Quadratic;
    int max_loop_depth_ = 3;
    int max_function_lines_ = 50;
    int max_parameters_ = 5;
    bool analyze_memory_ = true;
    bool strict_mode_ = false;
    
    // أدوات داخلية
    int count_loop_depth(const std::string& code);
    bool is_tail_recursive(const std::string& code);
    std::vector<std::string> extract_functions(const std::string& code);
    std::map<std::string, int> count_variable_usage(const std::string& code);
};

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ══════════════════════════════════════════════════════════════════════════════

/**
 * تحويل التعقيد الزمني لنص
 */
std::string time_complexity_to_string(TimeComplexity complexity);

/**
 * تحويل التعقيد المكاني لنص
 */
std::string space_complexity_to_string(SpaceComplexity complexity);

/**
 * تحويل خطورة المشكلة لنص
 */
std::string severity_to_string(PerformanceSeverity severity);

/**
 * تحويل نوع المشكلة لنص
 */
std::string issue_type_to_string(PerformanceIssueType type);

/**
 * الحصول على إيموجي للتصنيف
 */
std::string grade_emoji(char grade);

} // namespace analysis
} // namespace lsp
} // namespace sad
