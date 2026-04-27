// ======================================================================
// method_call_builder.h - بنّاء استدعاءات الطرق / Method Call Builder
// ======================================================================
// (AR) Phase 6 — Step 2: صنف منفصل يجمع 8 methods لاستدعاءات الطرق
//      على الأنواع المدمجة (قنوات، أقفال، مستقبلات، مجموعات انتظار،
//      مصفوفات، نصوص، خرائط). يحمل مرجعاً إلى SIRBuilder للوصول إلى
//      الحقول المشتركة (السياق) والـ helpers.
// (EN) Phase 6 — Step 2: standalone class grouping 8 method-call builders
//      for built-in types (channels, mutexes, futures, waitgroups,
//      arrays, strings, maps). Holds a reference to SIRBuilder to access
//      shared fields (context) and helpers.
// ======================================================================

#pragma once

#include "../sir_types.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Sad { namespace AST { class MethodCallExpr; } }

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) إعلانات مسبقة / (EN) Forward declarations
            class SIRBuilder;
            struct BuildResult;

            /**
             * @brief (AR) بنّاء استدعاءات الطرق على الأنواع المدمجة
             * @brief (EN) Builder for method calls on built-in types
             */
            class MethodCallBuilder
            {
            public:
                /**
                 * @brief (AR) منشئ — يستقبل مرجعاً إلى SIRBuilder المالك
                 * @brief (EN) Constructor — receives reference to owning SIRBuilder
                 */
                explicit MethodCallBuilder(SIRBuilder &b) : b_(b) {}

                // ──────────────────────────────────────────────────────
                // (AR) طرق التزامن / (EN) Concurrency methods
                // ──────────────────────────────────────────────────────

                std::optional<BuildResult> buildChannelMethodCall(
                    Sad::AST::MethodCallExpr *expr, const BuildResult &objResult);

                std::optional<BuildResult> buildMutexMethodCall(
                    Sad::AST::MethodCallExpr *expr, const BuildResult &objResult);

                std::optional<BuildResult> buildFutureMethodCall(
                    Sad::AST::MethodCallExpr *expr, const BuildResult &objResult);

                std::optional<BuildResult> buildWaitGroupMethodCall(
                    Sad::AST::MethodCallExpr *expr, const BuildResult &objResult);

                // ──────────────────────────────────────────────────────
                // (AR) طرق المصفوفات / (EN) Array methods
                // ──────────────────────────────────────────────────────

                std::optional<BuildResult> buildArrayBasicMethodCall(
                    const BuildResult &objResult,
                    const std::string &methodName,
                    const std::vector<SIROperand> &args);

                std::optional<BuildResult> buildArrayHigherOrderMethodCall(
                    const BuildResult &objResult,
                    const std::string &methodName,
                    const std::vector<SIROperand> &args,
                    const std::string &closureLambdaName,
                    SadTypeKind closureRetType);

                // ──────────────────────────────────────────────────────
                // (AR) طرق النصوص والخرائط / (EN) String & map methods
                // ──────────────────────────────────────────────────────

                std::optional<BuildResult> buildStringBuiltinMethodCall(
                    const BuildResult &objResult,
                    const std::string &methodName,
                    const std::vector<SIROperand> &args);

                std::optional<BuildResult> buildMapBuiltinMethodCall(
                    const BuildResult &objResult,
                    const std::string &methodName,
                    const std::vector<SIROperand> &args);

            private:
                // (AR) مرجع إلى SIRBuilder المالك — يُستخدم للوصول إلى
                //      الحقول الموروثة من SIRBuilderContext و helpers
                //      مثل buildExpression, newTempRegister, إلخ.
                // (EN) Reference to owning SIRBuilder — used to access
                //      fields inherited from SIRBuilderContext and helpers
                //      like buildExpression, newTempRegister, etc.
                SIRBuilder &b_;
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
