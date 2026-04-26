/**
 * @file optimizer_o2.h
 * @brief تحسينات المستوى -O2 للمترجم sadc
 * @brief (EN) O2 optimization passes for sadc compiler
 * 
 * @author سعيد
 * @date نوفمبر 2026
 */

#ifndef SADC_OPTIMIZER_O2_H
#define SADC_OPTIMIZER_O2_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;memory&gt;
#include &lt;map&gt;
#include &lt;set&gt;
#include &lt;functional&gt;

namespace Sad {
namespace Compiler {

// Forward declarations
class SIRFunction;
class SIRBasicBlock;
class SIRInstruction;
class SIRModule;

//==============================================================================
// إطار التحسين
//==============================================================================

/**
 * @brief نوع مرور التحسين
 */
enum class PassType {
    ANALYSIS,       // تحليل فقط
    TRANSFORM,      // تحويل
    CLEANUP         // تنظيف
};

/**
 * @brief مستوى التحسين
 */
enum class OptLevel {
    O0,     // بدون تحسين
    O1,     // تحسينات أساسية
    O2,     // تحسينات متوسطة (افتراضي)
    O3,     // تحسينات قصوى
    Os,     // تحسين للحجم
    Oz      // أقصى تحسين للحجم
};

/**
 * @brief نتيجة المرور
 */
struct PassResult {
    bool modified;          // هل تم تعديل الكود
    int instructionsRemoved;
    int instructionsAdded;
    int blocksRemoved;
    std::string message;
};

/**
 * @brief واجهة مرور التحسين
 */
class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;
    
    virtual std::string getName() const = 0;
    virtual PassType getType() const = 0;
    virtual PassResult run(SIRModule&amp; module) = 0;
    
    // تبعيات
    virtual std::vector&lt;std::string&gt; getDependencies() const { return {}; }
    virtual std::vector&lt;std::string&gt; getInvalidates() const { return {}; }
};

//==============================================================================
// تحسينات -O2
//==============================================================================

/**
 * @brief طي الثوابت (Constant Folding)
 * حساب التعبيرات الثابتة في وقت الترجمة
 */
class ConstantFoldingPass : public OptimizationPass {
public:
    std::string getName() const override { return "constant-folding"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    bool foldInstruction(SIRInstruction* inst);
    bool isConstant(SIRInstruction* inst);
    int64_t evaluateConstant(SIRInstruction* inst);
};

/**
 * @brief نشر الثوابت (Constant Propagation)
 * استبدال المتغيرات بقيمها الثابتة
 */
class ConstantPropagationPass : public OptimizationPass {
public:
    std::string getName() const override { return "constant-propagation"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    std::map&lt;std::string, int64_t&gt; constants_;
    void propagateInBlock(SIRBasicBlock* block);
};

/**
 * @brief إزالة الكود الميت (Dead Code Elimination)
 * حذف التعليمات غير المستخدمة
 */
class DeadCodeEliminationPass : public OptimizationPass {
public:
    std::string getName() const override { return "dce"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;
    
    std::vector&lt;std::string&gt; getDependencies() const override {
        return {"liveness-analysis"};
    }

private:
    std::set&lt;SIRInstruction*&gt; liveInstructions_;
    void markLive(SIRInstruction* inst);
    void sweep(SIRFunction&amp; func);
};

/**
 * @brief إزالة الحذف المشترك (Common Subexpression Elimination)
 * حذف التعبيرات المكررة
 */
class CSEPass : public OptimizationPass {
public:
    std::string getName() const override { return "cse"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    struct ExprHash {
        size_t operator()(const SIRInstruction* inst) const;
    };
    struct ExprEqual {
        bool operator()(const SIRInstruction* a, const SIRInstruction* b) const;
    };
    
    std::unordered_map&lt;SIRInstruction*, SIRInstruction*, ExprHash, ExprEqual&gt; expressions_;
};

/**
 * @brief حركة الكود الثابت في الحلقات (Loop Invariant Code Motion)
 * نقل الكود الذي لا يتغير خارج الحلقة
 */
class LICMPass : public OptimizationPass {
public:
    std::string getName() const override { return "licm"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;
    
    std::vector&lt;std::string&gt; getDependencies() const override {
        return {"loop-analysis", "dominator-tree"};
    }

private:
    bool isLoopInvariant(SIRInstruction* inst, const std::set&lt;SIRBasicBlock*&gt;&amp; loopBlocks);
    bool canHoist(SIRInstruction* inst);
};

/**
 * @brief تضمين الدوال (Function Inlining)
 * استبدال استدعاء الدالة بجسمها
 */
class InliningPass : public OptimizationPass {
public:
    std::string getName() const override { return "inline"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    int maxInlineSize_ = 50;  // أقصى عدد تعليمات للتضمين
    int maxInlineDepth_ = 3;  // أقصى عمق تضمين
    
    bool shouldInline(SIRFunction* callee, int depth);
    void inlineCall(SIRInstruction* call, SIRFunction* callee);
};

/**
 * @brief فتح الحلقات (Loop Unrolling)
 * تكرار جسم الحلقة لتقليل الحمل
 */
class LoopUnrollingPass : public OptimizationPass {
public:
    std::string getName() const override { return "loop-unroll"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;
    
    std::vector&lt;std::string&gt; getDependencies() const override {
        return {"loop-analysis"};
    }

private:
    int unrollFactor_ = 4;
    int maxUnrollIterations_ = 16;
    
    bool canUnroll(const struct LoopInfo&amp; loop);
    void unrollLoop(SIRFunction&amp; func, const struct LoopInfo&amp; loop);
};

/**
 * @brief إزالة ذيل الاستدعاء (Tail Call Optimization)
 * تحويل الاستدعاء الذيلي إلى قفزة
 */
class TailCallOptPass : public OptimizationPass {
public:
    std::string getName() const override { return "tailcall"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    bool isTailCall(SIRInstruction* call, SIRFunction* func);
    void optimizeTailCall(SIRInstruction* call);
};

/**
 * @brief قوة الاختزال (Strength Reduction)
 * استبدال العمليات المكلفة بأرخص
 */
class StrengthReductionPass : public OptimizationPass {
public:
    std::string getName() const override { return "strength-reduction"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    // x * 2 -&gt; x &lt;&lt; 1
    // x / 2 -&gt; x &gt;&gt; 1
    // x * constant -&gt; shifts and adds
    bool reduceMultiply(SIRInstruction* inst);
    bool reduceDivide(SIRInstruction* inst);
    bool reduceModulo(SIRInstruction* inst);
};

/**
 * @brief تبسيط CFG
 * دمج الكتل وإزالة القفزات الزائدة
 */
class CFGSimplifyPass : public OptimizationPass {
public:
    std::string getName() const override { return "cfg-simplify"; }
    PassType getType() const override { return PassType::CLEANUP; }
    PassResult run(SIRModule&amp; module) override;

private:
    bool mergeBlocks(SIRFunction&amp; func);
    bool eliminateUnreachable(SIRFunction&amp; func);
    bool threadJumps(SIRFunction&amp; func);
};

/**
 * @brief إعادة ترتيب التعليمات (Instruction Scheduling)
 * ترتيب التعليمات لتحسين استخدام المعالج
 */
class InstructionSchedulingPass : public OptimizationPass {
public:
    std::string getName() const override { return "instruction-scheduling"; }
    PassType getType() const override { return PassType::TRANSFORM; }
    PassResult run(SIRModule&amp; module) override;

private:
    void scheduleBlock(SIRBasicBlock* block);
    int getLatency(SIRInstruction* inst);
};

//==============================================================================
// مدير التحسين
//==============================================================================

/**
 * @brief مدير مرور التحسين
 */
class OptimizationManager {
public:
    /**
     * @brief تشغيل التحسينات
     */
    void run(SIRModule&amp; module, OptLevel level);
    
    /**
     * @brief إضافة مرور مخصص
     */
    void addPass(std::unique_ptr&lt;OptimizationPass&gt; pass);
    
    /**
     * @brief الحصول على الإحصائيات
     */
    struct Stats {
        int totalPasses;
        int instructionsRemoved;
        int instructionsAdded;
        int blocksRemoved;
        double totalTimeMs;
    };
    Stats getStats() const { return stats_; }
    
    /**
     * @brief تفعيل/تعطيل التصحيح
     */
    void setDebug(bool enabled) { debug_ = enabled; }
    
    /**
     * @brief تفعيل مرور معين
     */
    void enablePass(const std::string&amp; name);
    void disablePass(const std::string&amp; name);

private:
    std::vector&lt;std::unique_ptr&lt;OptimizationPass&gt;&gt; passes_;
    std::set&lt;std::string&gt; disabledPasses_;
    Stats stats_;
    bool debug_ = false;
    
    void setupPassesForLevel(OptLevel level);
    std::vector&lt;OptimizationPass*&gt; sortPasses();
};

/**
 * @brief إنشاء مدير تحسين بتحسينات O2
 */
inline std::unique_ptr&lt;OptimizationManager&gt; createO2Optimizer() {
    auto manager = std::make_unique&lt;OptimizationManager&gt;();
    
    // إضافة جميع تحسينات O2
    manager-&gt;addPass(std::make_unique&lt;ConstantFoldingPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;ConstantPropagationPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;DeadCodeEliminationPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;CSEPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;LICMPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;InliningPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;LoopUnrollingPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;TailCallOptPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;StrengthReductionPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;CFGSimplifyPass&gt;());
    manager-&gt;addPass(std::make_unique&lt;InstructionSchedulingPass&gt;());
    
    return manager;
}

} // namespace Compiler
} // namespace Sad

#endif // SADC_OPTIMIZER_O2_H
