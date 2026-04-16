/**
 * @file inlining_pass.h
 * @brief (AR) تمرير إدراج الدوال — يستبدل استدعاء الدوال الصغيرة بجسمها مباشرة
 * @brief (EN) Function Inlining Pass — replaces calls to small functions with their body
 *
 * @details
 * (AR) يفحص كل استدعاء CALL في الوحدة:
 *      - إذا كانت الدالة المستدعاة صغيرة (≤ عتبة محددة من التعليمات)
 *      - وليست تعاودية (لا تستدعي نفسها)
 *      - يتم نسخ جسم الدالة في مكان الاستدعاء مع إعادة تسمية السجلات
 *      عتبة الإدراج الافتراضية: 20 تعليمة
 *
 * (EN) Examines each CALL instruction in the module:
 *      - If the callee is small (≤ threshold instructions)
 *      - And not recursive
 *      - Copy function body at call site with register renaming
 *      Default threshold: 20 instructions
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#pragma once

#include "pass.h"
#include <unordered_map>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            class InliningPass : public OptimizationPass
            {
            public:
                // (AR) عتبة الإدراج: الحد الأقصى لعدد تعليمات الدالة المراد إدراجها
                static constexpr int DEFAULT_INLINE_THRESHOLD = 20;

                explicit InliningPass(int threshold = DEFAULT_INLINE_THRESHOLD);
                ~InliningPass();

                bool runOnFunction(SIR::SIRFunction *function) override;

                // (AR) تعيين الوحدة الكاملة (يحتاجها للبحث عن الدوال المستدعاة)
                // (EN) Set the full module (needed to look up callees)
                void setModule(SIR::SIRModule *module) { module_ = module; }

            private:
                // (AR) هل يمكن إدراج هذه الدالة؟
                bool canInline(SIR::SIRFunction *callee) const;

                // (AR) حساب عدد التعليمات في دالة
                int countInstructions(SIR::SIRFunction *function) const;

                // (AR) هل الدالة تعاودية؟
                bool isRecursive(SIR::SIRFunction *function) const;

                // (AR) تنفيذ الإدراج لاستدعاء واحد
                bool inlineCallSite(SIR::SIRFunction *caller, SIR::SIRBasicBlock *block,
                                    size_t callIndex, SIR::SIRFunction *callee);

                // (AR) إنشاء اسم سجل فريد للنسخة المُدرجة
                std::string makeUniqueRegName(const std::string &original);

                // (AR) إعادة تسمية السجلات في تعليمة
                SIR::SIRInstruction renameInstruction(const SIR::SIRInstruction &inst,
                                                      const std::unordered_map<std::string, std::string> &nameMap);

                // (AR) البحث عن دالة بالاسم في الوحدة
                SIR::SIRFunction *findFunction(const std::string &name);

                int inlineThreshold_;
                int inlinedCount_;
                int uniqueCounter_;
                SIR::SIRModule *module_;
            };

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
