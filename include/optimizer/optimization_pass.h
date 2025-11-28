#ifndef OPTIMIZATION_PASS_H
#define OPTIMIZATION_PASS_H

#include <memory>
#include <string>
#include "ast.h"

namespace sad {

/**
 * واجهة ممر التحسين
 * Optimization Pass Interface
 * 
 * تمثل ممر تحسين واحد يمكن تطبيقه على الـ AST
 */
class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;

    /**
     * تطبيق التحسين
     * Apply optimization
     */
    virtual std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) = 0;

    /**
     * الحصول على اسم التحسين
     * Get optimization name
     */
    virtual std::string getName() const = 0;

    /**
     * الحصول على وصف التحسين
     * Get optimization description
     */
    virtual std::string getDescription() const = 0;

    /**
     * هل التحسين آمن؟
     * Is the optimization safe?
     */
    virtual bool isSafe() const { return true; }

    /**
     * مستوى التحسين (0-3)
     * Optimization level (0-3)
     * 0: basic, 1: moderate, 2: aggressive, 3: experimental
     */
    virtual int getLevel() const { return 1; }
};

/**
 * ممر حذف الكود الميت
 * Dead Code Elimination Pass
 */
class DeadCodeEliminationPass : public OptimizationPass {
public:
    std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) override;
    std::string getName() const override { return "DeadCodeElimination"; }
    std::string getDescription() const override { 
        return "حذف الكود غير المستخدم / Remove unused code"; 
    }
    int getLevel() const override { return 1; }
};

/**
 * ممر طي الثوابت
 * Constant Folding Pass
 */
class ConstantFoldingPass : public OptimizationPass {
public:
    std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) override;
    std::string getName() const override { return "ConstantFolding"; }
    std::string getDescription() const override { 
        return "حساب التعبيرات الثابتة في وقت الترجمة / Evaluate constant expressions at compile time"; 
    }
    int getLevel() const override { return 0; }
};

/**
 * ممر تبسيط التعبيرات
 * Expression Simplification Pass
 */
class ExpressionSimplificationPass : public OptimizationPass {
public:
    std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) override;
    std::string getName() const override { return "ExpressionSimplification"; }
    std::string getDescription() const override { 
        return "تبسيط التعبيرات الحسابية / Simplify arithmetic expressions"; 
    }
    int getLevel() const override { return 1; }
};

/**
 * ممر تحسين الحلقات
 * Loop Optimization Pass
 */
class LoopOptimizationPass : public OptimizationPass {
public:
    std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) override;
    std::string getName() const override { return "LoopOptimization"; }
    std::string getDescription() const override { 
        return "تحسين أداء الحلقات / Optimize loop performance"; 
    }
    int getLevel() const override { return 2; }
};

/**
 * ممر دمج الدوال
 * Function Inlining Pass
 */
class FunctionInliningPass : public OptimizationPass {
public:
    std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) override;
    std::string getName() const override { return "FunctionInlining"; }
    std::string getDescription() const override { 
        return "دمج الدوال الصغيرة في نقطة الاستدعاء / Inline small functions"; 
    }
    int getLevel() const override { return 2; }
    bool isSafe() const override { return false; } // قد يزيد حجم الكود
};

/**
 * ممر إزالة الإسناد الزائد
 * Redundant Assignment Elimination Pass
 */
class RedundantAssignmentEliminationPass : public OptimizationPass {
public:
    std::shared_ptr<ASTNode> apply(std::shared_ptr<ASTNode> ast) override;
    std::string getName() const override { return "RedundantAssignmentElimination"; }
    std::string getDescription() const override { 
        return "إزالة الإسناد المكرر / Remove redundant assignments"; 
    }
    int getLevel() const override { return 1; }
};

} // namespace sad

#endif // OPTIMIZATION_PASS_H
