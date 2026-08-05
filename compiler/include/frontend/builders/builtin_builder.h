// ======================================================================
// builtin_builder.h - بنّاء الدوال المدمجة / Built-in Function Builder
// ======================================================================
// (AR) Phase 6 — Step 3: صنف منفصل يجمع 17 method لبناء الدوال المدمجة
//      المُقسّمة على 11 فئة: core, math, io_utils, strings_arrays, simd,
//      system, network (×2), os_core, os_hardware, os_system, security,
//      ffi, async, embedded, ui, uefi.
//      يحمل مرجعاً إلى SIRBuilder للوصول للحقول المشتركة والـhelpers.
// (EN) Phase 6 — Step 3: standalone class grouping 17 builders for built-in
//      functions across 11 categories. Holds a reference to SIRBuilder.
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

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
             * @brief (AR) بنّاء استدعاءات الدوال المدمجة (17 method)
             * @brief (EN) Builder for built-in function calls (17 methods)
             */
            class BuiltinBuilder
            {
            public:
                /**
                 * @brief (AR) منشئ — يستقبل مرجعاً إلى SIRBuilder المالك
                 * @brief (EN) Constructor — receives reference to owning SIRBuilder
                 */
                explicit BuiltinBuilder(SIRBuilder &b) : b_(b) {}

                // ──────────────────────────────────────────────────────
                // (AR) 17 method موزعة على فئات الدوال المدمجة
                // (EN) 17 methods across built-in function categories
                // ──────────────────────────────────────────────────────

                std::optional<BuildResult> buildBuiltinSystem_Async(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                // (AR) isSyntaxDesugared: نداءٌ وَلَّدَته صياغةُ اللغةِ (`|س|` ⇒ `مطلق(س)`)
                //      فتتخطّاه بوّابةُ الاستيراد — صياغةُ اللغةِ لا تُحجَب خلف وحدة.
                // (EN) isSyntaxDesugared: a call synthesized by the syntax; the import
                //      gate skips it — core syntax is never hidden behind a module.
                std::optional<BuildResult> buildBuiltinCallCore(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands, bool isSyntaxDesugared = false);

                std::optional<BuildResult> buildBuiltinSystem_Embedded(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_FFI(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinIOUtilsCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinMathCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                // (AR) الواجهةُ المسمّاةُ لوحدةِ «خرائط» (م-٠٠١ ق١+ق٢) — builtins_maps.cpp
                // (EN) The Maps module's named interface (card م-٠٠١, ق١+ق٢)
                std::optional<BuildResult> buildBuiltinMapsCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                BuildResult buildNetworkBuiltinInstruction(SIROpcode opcode, std::vector<SIROperand> &argOperands, SadTypeKind returnType, const char *comment);

                std::optional<BuildResult> buildBuiltinCallNetwork(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_OsCore(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_OsHardware(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_OsSystem(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_Security(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);
                std::optional<BuildResult> buildBuiltinSystem_Crypto(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinCallSimd(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinStringArrayCall(const std::string &funcName, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinCallSystem(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_Uefi(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

                std::optional<BuildResult> buildBuiltinSystem_UI(const std::string &funcName, bool isUserDefinedFunction, std::vector<BuildResult> &argResults, std::vector<SIROperand> &argOperands);

            private:
                // (AR) مرجع إلى SIRBuilder المالك للوصول إلى الحقول والـhelpers
                // (EN) Reference to owning SIRBuilder for fields/helpers access
                SIRBuilder &b_;
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
