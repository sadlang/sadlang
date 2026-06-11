/*
 * ============================================================================
 * LLVMCodeGenContext — قاعدة الحالة المشتركة لمولد كود LLVM
 * LLVMCodeGenContext — Shared State Base for LLVM Code Generator
 * ============================================================================
 *
 * الوصف (Description):
 *   هذه القاعدة (base struct) تحوي كل الحالة المشتركة بين LLVMCodeGen
 *   ومُكوّناته الفرعية (sub-codegens) في إطار Phase 7. الهدف هو فصل
 *   "ما يحمله المولّد" عن "ما يفعله المولّد" بحيث يستطيع كل sub-codegen
 *   الوصول للحقول عبر الوراثة دون تكرار التعريفات.
 *
 *   This base struct holds all state shared between LLVMCodeGen and its
 *   sub-codegens (Phase 7 refactor). Goal: separate state ownership from
 *   behavior so sub-codegens can access fields via inheritance.
 *
 * المحتويات (Contents):
 *   1. أساسيات LLVM: context, module, builder, targetMachine
 *   2. سياق التوليد: CodeGenContext (دوال/متغيرات/أصناف/vtable)
 *   3. مساعدات مركّبة: typeMapper, optimizer
 *   4. مرجع SIR: sirModule
 *   5. الإعدادات: optLevel, autoOpt, moduleMode, freestanding
 *   6. تتبع الأخطاء: hasErrors, errors
 *   7. دوال مساعدة عامة: reportError + accessors
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): Phase 7 Step 0 — 2025-12
 * ============================================================================
 */

#ifndef SAD_LLVM_CODEGEN_CONTEXT_H
#define SAD_LLVM_CODEGEN_CONTEXT_H

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "error_codes.h" // (AR) EM-CPP-7: Sad::Errors::ErrorCode للكتالوج

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Target/TargetMachine.h>

#include "llvm_type_mapper.h"
#include "llvm_optimizer.h"
#include "sir_module.h"

namespace Sad
{
    namespace LLVM
    {
        // ============================================================================
        // CodeGenContext — حالة التنفيذ أثناء توليد دالة واحدة
        // CodeGenContext — Per-function execution state
        // (موجود مسبقاً، يُمرر هنا كمرجع للتوضيح فقط — التعريف الفعلي في llvm_codegen.h)
        // ============================================================================
        struct CodeGenContext;

        // ============================================================================
        // LLVMCodeGenContext — قاعدة الحالة المشتركة (Phase 7 base class)
        // LLVMCodeGenContext — Shared state base (Phase 7 base class)
        // ============================================================================
        struct LLVMCodeGenContext
        {
            // ------------------------------------------------------------------------
            // (1) أساسيات LLVM / LLVM core
            // ------------------------------------------------------------------------

            /// (AR) سياق LLVM الرئيسي — الحاوية الجذر لكل أنواع وقيم LLVM
            /// (EN) LLVM root context — owner of all LLVM types/values
            std::unique_ptr<llvm::LLVMContext> context_;

            /// (AR) وحدة LLVM الحالية (الناتج النهائي للترجمة)
            /// (EN) Current LLVM module (final compilation output)
            std::unique_ptr<llvm::Module> module_;

            /// (AR) باني تعليمات LLVM
            /// (EN) LLVM IR instruction builder
            std::unique_ptr<llvm::IRBuilder<>> builder_;

            /// (AR) الآلة الهدف (مملوكة من قِبَل LLVM)
            /// (EN) Target machine (LLVM-owned)
            llvm::TargetMachine *targetMachine_ = nullptr;

            // ------------------------------------------------------------------------
            // (3) المساعدات المركّبة / Composed helpers
            // ------------------------------------------------------------------------

            /// (AR) محوّل الأنواع SIR → LLVM
            /// (EN) SIR → LLVM type mapper
            std::unique_ptr<LLVMTypeMapper> typeMapper_;

            /// (AR) محسّن LLVM (passes pipeline)
            /// (EN) LLVM optimizer (passes pipeline)
            std::unique_ptr<sad::LLVMOptimizer> optimizer_;

            /// (AR) مرجع وحدة SIR المصدر
            /// (EN) Source SIR module reference
            std::shared_ptr<Compiler::SIR::SIRModule> sirModule_;

            // ------------------------------------------------------------------------
            // (4) إعدادات التحسين والوضع / Optimization & mode settings
            // ------------------------------------------------------------------------

            /// (AR) مستوى التحسين (O0..O3)
            /// (EN) Optimization level (O0..O3)
            sad::OptimizationLevel optimizationLevel_ = sad::OptimizationLevel::O0;

            /// (AR) تشغيل التحسين تلقائياً بعد generate()
            /// (EN) Auto-run optimization after generate()
            bool autoOptimize_ = false;

            /// (AR) وضع الوحدة: تخطّي main wrapper
            /// (EN) Module mode: skip main wrapper
            bool moduleMode_ = false;

            /// (AR) وضع مستقل: لا اعتماد على libc
            /// (EN) Freestanding mode: no libc dependency
            bool freestanding_ = false;

            // ------------------------------------------------------------------------
            // (5) تتبع الأخطاء / Error tracking
            // ------------------------------------------------------------------------

            /// (AR) هل وقع خطأ منذ بدء التوليد؟
            /// (EN) Did any error occur during generation?
            bool hasErrors_ = false;

            /// (AR) قائمة رسائل الأخطاء بالترتيب الزمني
            /// (EN) Chronological list of error messages
            std::vector<std::string> errors_;

            // ------------------------------------------------------------------------
            // (7) دوال مساعدة عامة / Public helpers (متاحة لكل sub-codegen)
            // ------------------------------------------------------------------------

            /**
             * (AR) تسجيل خطأ وطباعته على stderr
             * (EN) Record an error and print to stderr
             */
            void reportError(const std::string &message);

            /// (AR) EM-CPP-7: تسجيل خطأ من الكتالوج (ErrorCode + placeholders).
            /// (EN) EM-CPP-7: record an error from the catalog (ErrorCode + placeholders).
            void reportError(Sad::Errors::ErrorCode code,
                             std::map<std::string, std::string> placeholders = {});

            /**
             * (AR) هل هناك أخطاء مسجّلة؟
             * (EN) Are there any recorded errors?
             */
            bool hasErrors() const { return hasErrors_; }

            /**
             * (AR) قائمة الأخطاء (للقراءة فقط)
             * (EN) Errors list (read-only)
             */
            const std::vector<std::string> &getErrors() const { return errors_; }

        protected:
            /// (AR) مُنشئ افتراضي — كل الحقول لها قيم ابتدائية
            /// (EN) Default constructor — all fields have inline initializers
            LLVMCodeGenContext() = default;

            /// (AR) منع النسخ — يحتوي unique_ptr
            /// (EN) Non-copyable — contains unique_ptr
            LLVMCodeGenContext(const LLVMCodeGenContext &) = delete;
            LLVMCodeGenContext &operator=(const LLVMCodeGenContext &) = delete;

            /// (AR) السماح بالنقل (للمرونة المستقبلية)
            /// (EN) Movable (for future flexibility)
            LLVMCodeGenContext(LLVMCodeGenContext &&) = default;
            LLVMCodeGenContext &operator=(LLVMCodeGenContext &&) = default;

            ~LLVMCodeGenContext() = default;
        };

    } // namespace LLVM
} // namespace Sad

#endif // SAD_LLVM_CODEGEN_CONTEXT_H
