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
#include "error_manager.h"  // (AR) EM-CPP-7: buildBilingualMessage من الكتالوج
#include "error_catalog.h"  // (AR) RenderContext
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

        // (AR) EM-CPP-7: نسخة من الكتالوج (ErrorCode + placeholders) — كل الرسائل من YAML.
        // (EN) EM-CPP-7: catalog-based variant (ErrorCode + placeholders) — all messages from YAML.
        void LLVMCodeGenContext::reportError(Sad::Errors::ErrorCode code,
                                             std::map<std::string, std::string> placeholders)
        {
            // (AR) سجّل الرمز أوّلًا — يُمكّن البوّابات المُنمَّطة بالرمز في السائق
            // (EN) Record the code first — enables code-scoped driver gates
            errorCodes_.push_back(code);
            Sad::Errors::RenderContext ctx;
            ctx.placeholders = std::move(placeholders);
            reportError(Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(code, ctx));
        }

    } // namespace LLVM
} // namespace Sad
