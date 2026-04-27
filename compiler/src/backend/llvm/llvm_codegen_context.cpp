/*
 * ============================================================================
 * LLVMCodeGenContext — تنفيذ الدوال المساعدة
 * LLVMCodeGenContext — Helper methods implementation
 * ============================================================================
 *
 * Phase 7 Step 0 — استخراج قاعدة الحالة المشتركة من LLVMCodeGen.
 * Phase 7 Step 0 — Extracted shared state base from LLVMCodeGen.
 * ============================================================================
 */

#include "llvm_codegen_context.h"
#include <iostream>

namespace Sad
{
    namespace LLVM
    {
        // ============================================================================
        // (AR) reportError — تسجيل خطأ وطباعته على stderr
        // (EN) reportError — Record an error and print to stderr
        // ============================================================================
        void LLVMCodeGenContext::reportError(const std::string &message)
        {
            // (AR) رفع علم الأخطاء حتى تتمكن طبقات أعلى من الإحباط
            // (EN) Raise the error flag so upper layers can abort
            hasErrors_ = true;

            // (AR) إضافة الرسالة لسجل التسلسل الزمني
            // (EN) Append message to chronological log
            errors_.push_back(message);

            // (AR) طباعة فورية على stderr
            // (EN) Immediate stderr print
            std::cerr << "LLVM CodeGen Error: " << message << std::endl;
        }

    } // namespace LLVM
} // namespace Sad
