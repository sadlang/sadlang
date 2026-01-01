/*
 * محسّن اللغة العربية / Arabic Language Optimizer
 * =============================================
 * 
 * تحسينات مخصصة للغة Sad - تركز على:
 * Custom optimizations for Sad language - focusing on:
 * 
 * 1. تحسين النصوص العربية UTF-8 / Arabic UTF-8 string optimization
 * 2. معالجة RTL text بكفاءة / Efficient RTL text handling  
 * 3. تحسين pattern matching / Pattern matching optimization
 * 4. تحسين عمليات Unicode / Unicode operations optimization
 * 5. String concatenation optimization للنصوص العربية
 */

#pragma once

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/DenseMap.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {

/**
 * إحصائيات التحسين العربي / Arabic optimization statistics
 */
struct ArabicOptimizationStats {
    // String optimizations
    size_t strings_pooled = 0;           // النصوص المُدمجة / Pooled strings
    size_t rtl_operations_optimized = 0; // عمليات RTL المُحسّنة / Optimized RTL ops
    size_t unicode_ops_optimized = 0;    // عمليات Unicode المُحسّنة / Optimized Unicode ops
    
    // Pattern matching optimizations
    size_t patterns_analyzed = 0;        // الأنماط المُحللة / Analyzed patterns
    size_t jump_tables_created = 0;      // جداول القفز المُنشأة / Created jump tables
    size_t guards_optimized = 0;         // الحراس المُحسّنة / Optimized guards
    
    // General optimizations
    size_t concatenations_merged = 0;    // الدمج المُحسّن / Merged concatenations
    size_t constants_folded = 0;         // الثوابت المطوية / Folded constants
    
    // Performance metrics
    double optimization_time_ms = 0.0;   // وقت التحسين بالملي ثانية / Optimization time in ms
    
    // طباعة الإحصائيات / Print statistics
    void print() const;
    
    // دمج إحصائيات / Merge statistics
    void merge(const ArabicOptimizationStats& other);
};

/**
 * محسّن النصوص العربية / Arabic String Optimizer
 * ============================================
 * 
 * يقوم بـ:
 * - دمج النصوص الثابتة المتكررة (String pooling)
 * - تحسين UTF-8 encoding/decoding
 * - تحسين string concatenation
 * - تحسين RTL text operations
 */
class ArabicStringOptimizer {
public:
    ArabicStringOptimizer() = default;
    
    /**
     * تحسين النصوص في module / Optimize strings in module
     */
    bool optimizeModule(llvm::Module& module, ArabicOptimizationStats& stats);
    
    /**
     * تحسين النصوص في دالة / Optimize strings in function
     */
    bool optimizeFunction(llvm::Function& function, ArabicOptimizationStats& stats);
    
private:
    // خريطة النصوص المُدمجة / String pool map
    std::unordered_map<std::string, llvm::GlobalVariable*> string_pool_;
    
    /**
     * دمج النصوص الثابتة المتكررة / Pool duplicate constant strings
     */
    bool poolConstantStrings(llvm::Module& module, ArabicOptimizationStats& stats);
    
    /**
     * تحسين string concatenation / Optimize string concatenation
     */
    bool optimizeConcatenation(llvm::Function& function, ArabicOptimizationStats& stats);
    
    /**
     * تحسين UTF-8 encoding / Optimize UTF-8 encoding
     */
    bool optimizeUTF8Operations(llvm::Function& function, ArabicOptimizationStats& stats);
    
    /**
     * التحقق من أن النص عربي / Check if string is Arabic
     */
    bool isArabicString(llvm::StringRef str) const;
};

/**
 * محسّن Pattern Matching / Pattern Matching Optimizer
 * ==================================================
 * 
 * يقوم بـ:
 * - تحليل patterns في compile-time
 * - توليد jump tables للـ patterns المعقدة
 * - تحسين guards
 * - التحقق من exhaustiveness
 */
class PatternMatchingOptimizer {
public:
    PatternMatchingOptimizer() = default;
    
    /**
     * تحسين pattern matching في module / Optimize pattern matching in module
     */
    bool optimizeModule(llvm::Module& module, ArabicOptimizationStats& stats);
    
    /**
     * تحسين pattern matching في دالة / Optimize pattern matching in function
     */
    bool optimizeFunction(llvm::Function& function, ArabicOptimizationStats& stats);
    
private:
    /**
     * تحليل pattern matching statement / Analyze pattern matching statement
     */
    bool analyzePattern(llvm::Instruction* inst, ArabicOptimizationStats& stats);
    
    /**
     * توليد jump table / Generate jump table
     */
    bool generateJumpTable(llvm::SwitchInst* switchInst, ArabicOptimizationStats& stats);
    
    /**
     * تحسين guards / Optimize guards
     */
    bool optimizeGuards(llvm::Function& function, ArabicOptimizationStats& stats);
};

/**
 * محسّن عمليات Unicode / Unicode Operations Optimizer
 * ==================================================
 * 
 * يقوم بـ:
 * - تحسين character classification
 * - تحسين string comparison
 * - تحسين normalization
 * - استخدام SIMD عند الإمكان
 */
class UnicodeOptimizer {
public:
    UnicodeOptimizer() = default;
    
    /**
     * تحسين عمليات Unicode في module / Optimize Unicode operations in module
     */
    bool optimizeModule(llvm::Module& module, ArabicOptimizationStats& stats);
    
    /**
     * تحسين عمليات Unicode في دالة / Optimize Unicode operations in function
     */
    bool optimizeFunction(llvm::Function& function, ArabicOptimizationStats& stats);
    
private:
    /**
     * تحسين character classification / Optimize character classification
     */
    bool optimizeCharClassification(llvm::Function& function, ArabicOptimizationStats& stats);
    
    /**
     * تحسين string comparison / Optimize string comparison
     */
    bool optimizeStringComparison(llvm::Function& function, ArabicOptimizationStats& stats);
    
    /**
     * تحسين normalization / Optimize normalization
     */
    bool optimizeNormalization(llvm::Function& function, ArabicOptimizationStats& stats);
};

/**
 * LLVM Pass للتحسينات العربية / LLVM Pass for Arabic optimizations
 * ===============================================================
 * 
 * Pass رئيسي يجمع جميع التحسينات المخصصة للغة العربية
 * Main pass that combines all custom Arabic optimizations
 */
class ArabicOptimizationPass : public llvm::PassInfoMixin<ArabicOptimizationPass> {
public:
    /**
     * المُنشئ / Constructor
     */
    ArabicOptimizationPass() = default;
    
    /**
     * تشغيل التحسين على module / Run optimization on module
     */
    llvm::PreservedAnalyses run(llvm::Module& module, llvm::ModuleAnalysisManager& mam);
    
    /**
     * الحصول على الإحصائيات / Get statistics
     */
    const ArabicOptimizationStats& getStats() const { return stats_; }
    
    /**
     * طباعة الإحصائيات / Print statistics
     */
    void printStats() const;
    
    /**
     * إعادة تعيين الإحصائيات / Reset statistics
     */
    void resetStats() { stats_ = ArabicOptimizationStats(); }
    
private:
    // المُحسّنات الفرعية / Sub-optimizers
    ArabicStringOptimizer string_optimizer_;           // محسّن النصوص / String optimizer
    PatternMatchingOptimizer pattern_optimizer_;       // محسّن الأنماط / Pattern optimizer
    UnicodeOptimizer unicode_optimizer_;               // محسّن Unicode / Unicode optimizer
    
    // الإحصائيات / Statistics
    ArabicOptimizationStats stats_;
    
    /**
     * تحسين دالة واحدة / Optimize a single function
     */
    bool optimizeFunction(llvm::Function& function);
};

} // namespace sad
