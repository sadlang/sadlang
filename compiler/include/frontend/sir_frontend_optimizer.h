// ======================================================================
// sir_frontend_optimizer.h - محسّن SIR للواجهة الأمامية
//                           Frontend SIR Optimizer
// ======================================================================
// الوصف بالعربية:
//   محسّن يعمل على مستوى SIR الأمامي (Sad::Compiler::SIR)
//   يطبق 5 ممرات تحسين: طيّ الثوابت، حذف الكود الميت،
//   التضمين، إزالة التعبيرات المتكررة، تبسيط الحلقات
//
// English Description:
//   Optimizer operating on Frontend SIR (Sad::Compiler::SIR)
//   Applies 5 optimization passes: constant folding, dead code elimination,
//   function inlining, common subexpression elimination, loop simplification
//
// المطور / Developer: خالد (Khalid) — Compiler & Core
// التاريخ / Date: July 2026
// الإصدار / Version: 1.0
// ======================================================================

#pragma once

#include "frontend/sir_types.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_module.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace Sad {
namespace Compiler {
namespace SIR {

// ======================================================================
// (AR) إحصائيات التحسين / (EN) Optimization Statistics
// ======================================================================
struct FrontendOptStats {
    size_t constantsFolded = 0;
    size_t deadInstructionsRemoved = 0;
    size_t functionsInlined = 0;
    size_t commonSubexpressionsEliminated = 0;
    size_t loopsSimplified = 0;
    size_t totalPassesRun = 0;
    size_t totalIterations = 0;

    size_t totalOptimizations() const {
        return constantsFolded + deadInstructionsRemoved +
               functionsInlined + commonSubexpressionsEliminated +
               loopsSimplified;
    }

    std::string toString() const;
};

// ======================================================================
// (AR) واجهة ممر التحسين / (EN) Optimization Pass Interface
// ======================================================================
class SIRFrontendPass {
public:
    virtual ~SIRFrontendPass() = default;
    virtual std::string getName() const = 0;
    virtual bool runOnFunction(SIRFunction& func) = 0;
    virtual bool runOnModule(SIRModule& module);
};

// ======================================================================
// ممر 1: طيّ الثوابت / Pass 1: Constant Folding
// ======================================================================
// (AR) يطوي العمليات الحسابية على ثوابت في وقت الترجمة
// (EN) Folds arithmetic operations on constants at compile time
//
// مثال / Example:
//   %t1 = add.i64 5, 3  →  (يُحذف ويُستبدل %t1 بالثابت 8)
//   %t2 = mul.i64 %t1, 2  →  %t2 = mul.i64 8, 2  →  (ثابت 16)
class ConstantFoldingFrontendPass : public SIRFrontendPass {
public:
    std::string getName() const override { return "ConstantFolding"; }
    bool runOnFunction(SIRFunction& func) override;

private:
    bool foldInstruction(SIRInstruction& inst,
                        std::unordered_map<std::string, SIROperand>& constants);
    bool isConstant(const SIROperand& op,
                   const std::unordered_map<std::string, SIROperand>& constants) const;
    SIROperand resolveConstant(const SIROperand& op,
                              const std::unordered_map<std::string, SIROperand>& constants) const;
};

// ======================================================================
// ممر 2: حذف الكود الميت / Pass 2: Dead Code Elimination
// ======================================================================
// (AR) يحذف التعليمات التي لا تُستخدم نتائجها
// (EN) Removes instructions whose results are never used
//
// مثال / Example:
//   %t1 = add.i64 5, 3    ← لا يُستخدم %t1 → يُحذف
//   %t2 = mul.i64 %a, %b
//   ret %t2
class DeadCodeEliminationFrontendPass : public SIRFrontendPass {
public:
    std::string getName() const override { return "DeadCodeElimination"; }
    bool runOnFunction(SIRFunction& func) override;

private:
    std::unordered_set<std::string> collectUsedRegisters(const SIRFunction& func) const;
    bool hasSideEffects(const SIRInstruction& inst) const;
};

// ======================================================================
// ممر 3: تضمين الدوال / Pass 3: Function Inlining
// ======================================================================
// (AR) يضمّن الدوال الصغيرة في مواقع استدعائها
// (EN) Inlines small functions at their call sites
//
// مثال / Example:
//   دالة مربع(س) → ارجع س * س
//   %r = call @مربع(%x)  →  %r = mul.i64 %x, %x
class FunctionInliningFrontendPass : public SIRFrontendPass {
public:
    std::string getName() const override { return "FunctionInlining"; }
    bool runOnFunction(SIRFunction& func) override { return false; }
    bool runOnModule(SIRModule& module) override;

    void setMaxInlineSize(size_t size) { maxInlineSize_ = size; }

private:
    size_t maxInlineSize_ = 10;
    // (AR) أسماء المتغيرات العالمية — لا تُعاد تسميتها عند التضمين
    // (EN) Global variable names — NOT renamed during inlining
    std::unordered_set<std::string> globalVarNames_;
    bool shouldInline(const SIRFunction& callee) const;
    bool inlineCallSite(SIRFunction& caller, SIRBasicBlock& block,
                       size_t instIdx, const SIRFunction& callee,
                       size_t& nextReg);
};

// ======================================================================
// ممر 4: إزالة التعبيرات المتكررة / Pass 4: CSE
// ======================================================================
// (AR) يزيل التعبيرات المتكررة ويعيد استخدام النتائج السابقة
// (EN) Eliminates common subexpressions and reuses previous results
//
// مثال / Example:
//   %t1 = add.i64 %a, %b
//   %t2 = add.i64 %a, %b  →  يُستبدل %t2 بـ %t1
class CommonSubexpressionEliminationPass : public SIRFrontendPass {
public:
    std::string getName() const override { return "CSE"; }
    bool runOnFunction(SIRFunction& func) override;

private:
    struct ExprKey {
        SIROpcode opcode;
        // (AR) 🐞 نوعُ **النتيجة** جزءٌ من الهُويّة لا زينة: `SIROpcode::NOT` يحمل
        //      دلالتَين — النفيَ المنطقيَّ (`ليس`، نتيجتُه Boolean) والقلبَ البتّيَّ
        //      (`~`، نتيجتُه Integer) — ولا يفرّق بينهما إلّا نوعُ النتيجة. فمفتاحٌ
        //      بـ(الأوپكود، المعامِلات) وحدَها يدمج `ليس ٧` مع `~٧` فيرث الثاني قيمةَ
        //      الأوّل. المقيس: `ليس ٧` ثمّ `~٧` في برنامجٍ واحدٍ ⇒ «‑8» ثمّ «0» في
        //      الهدفَين الأصليَّين، والمفسّرُ «خطأ» ثمّ «‑8». والدمجُ سابقٌ للتمييزِ في
        //      المخفّض، لكنّه كان **مستورًا**: حين كان المخفّضُ يقلب البتّاتِ في
        //      الحالتين خرج المدموجُ مساويًا للمدموجِ فيه فبدا سليمًا.
        //      والشقيقُ `compiler/include/sir_optimizer/cse_pass.h` (`ExpressionKey`)
        //      كان يحمل الثغرةَ عينَها وأُغلق معه في الدفعةِ نفسِها؛ وقائمتُه
        //      المؤهَّلةُ تضمّ `AND`/`OR` — وهما يحملان الازدواجَ نفسَه (منطقيّان
        //      بنتيجةِ Boolean، وبتّيّان بنتيجةِ Integer). إغلاقُه **دفاعيٌّ لا مقيسَ
        //      الأثر**: لم أجد بذرةً تسلك مسارَه اليومَ بجوابٍ مختلف.
        SadTypeKind resultType = SadTypeKind::Unknown;
        std::vector<std::string> operandNames;
        bool operator==(const ExprKey& other) const;
    };
    struct ExprKeyHash {
        size_t operator()(const ExprKey& key) const;
    };
    bool isPureOperation(SIROpcode opcode) const;
};

// ======================================================================
// ممر 5: تبسيط الحلقات / Pass 5: Loop Simplification
// ======================================================================
// (AR) تبسيط الحلقات: حذف فروع بشروط ثابتة، إزالة كتل فارغة
// (EN) Loop simplification: remove branches with constant conditions,
//      eliminate empty blocks, simplify trivial loops
class LoopSimplificationFrontendPass : public SIRFrontendPass {
public:
    std::string getName() const override { return "LoopSimplification"; }
    bool runOnFunction(SIRFunction& func) override;

private:
    bool simplifyBranches(SIRFunction& func);
    bool removeEmptyBlocks(SIRFunction& func);
    bool mergeLinearBlocks(SIRFunction& func);
};

// ======================================================================
// (AR) المحسّن الرئيسي / (EN) Main Optimizer
// ======================================================================
class SIRFrontendOptimizer {
public:
    SIRFrontendOptimizer();

    void addPass(std::unique_ptr<SIRFrontendPass> pass);
    void enablePass(const std::string& name, bool enabled = true);

    bool optimizeFunction(SIRFunction& func);
    bool optimizeModule(SIRModule& module);

    const FrontendOptStats& getStats() const { return stats_; }
    void resetStats() { stats_ = FrontendOptStats{}; }

    void setMaxIterations(size_t n) { maxIterations_ = n; }

private:
    std::vector<std::unique_ptr<SIRFrontendPass>> passes_;
    std::unordered_map<std::string, bool> enabledPasses_;
    FrontendOptStats stats_;
    size_t maxIterations_ = 10;
};

} // namespace SIR
} // namespace Compiler
} // namespace Sad
