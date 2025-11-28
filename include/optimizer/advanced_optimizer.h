#ifndef ADVANCED_OPTIMIZER_H
#define ADVANCED_OPTIMIZER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "ast.h"
#include "optimization_pass.h"

namespace sad {

/**
 * محسن الكود المتقدم
 * Advanced Code Optimizer
 * 
 * المرحلة 10: تحسين الأداء المتقدم
 * يقوم بتطبيق مجموعة من التحسينات على شجرة AST
 */
class AdvancedOptimizer {
public:
    AdvancedOptimizer();
    ~AdvancedOptimizer();

    /**
     * تحسين شجرة AST
     * Optimize the AST
     */
    std::shared_ptr<ASTNode> optimize(std::shared_ptr<ASTNode> ast);

    /**
     * إضافة ممر تحسين جديد
     * Add a new optimization pass
     */
    void addOptimizationPass(std::unique_ptr<OptimizationPass> pass);

    /**
     * تفعيل/تعطيل تحسين معين
     * Enable/disable specific optimization
     */
    void enableOptimization(const std::string& name, bool enabled);

    /**
     * الحصول على إحصائيات التحسين
     * Get optimization statistics
     */
    struct OptimizationStats {
        int totalPasses = 0;              // عدد الممرات المنفذة
        int nodesEliminated = 0;          // العقد المحذوفة
        int expressionsSimplified = 0;    // التعبيرات المبسطة
        int loopsOptimized = 0;           // الحلقات المحسنة
        int functionsInlined = 0;         // الدوال المدمجة
        double optimizationTime = 0.0;    // وقت التحسين (بالثواني)
    };
    
    OptimizationStats getStatistics() const { return stats_; }

    /**
     * إعادة تعيين الإحصائيات
     * Reset statistics
     */
    void resetStatistics();

private:
    // ممرات التحسين
    std::vector<std::unique_ptr<OptimizationPass>> passes_;
    
    // التحسينات المفعلة/المعطلة
    std::unordered_map<std::string, bool> enabledOptimizations_;
    
    // إحصائيات التحسين
    OptimizationStats stats_;

    // تطبيق جميع الممرات
    std::shared_ptr<ASTNode> applyAllPasses(std::shared_ptr<ASTNode> ast);
    
    // التحقق من صحة الـ AST بعد التحسين
    bool validateAST(std::shared_ptr<ASTNode> ast);
};

} // namespace sad

#endif // ADVANCED_OPTIMIZER_H
