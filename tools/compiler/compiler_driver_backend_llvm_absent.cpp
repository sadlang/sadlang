// بسم الله الرحمن الرحيم
// ============================================================================
// Compiler Driver — Backend, LLVM-absent build
// مشغّل المترجم — الخلفيّة في بناءٍ بلا LLVM
// ============================================================================
// (AR) هذا الملفُّ بديلُ `compiler_driver_backend.cpp` في هدفِ `sad-build-native`
//      وحدَه. وهو **ملفٌّ كاملٌ يُصرَّفُ دائمًا في هدفِه**، لا فرعُ `#ifdef` داخلَ
//      ملفٍّ آخر: الفرعُ الميّتُ الذي لا يُصرَّفُ في أكثرِ الخاناتِ ينجرفُ صامتًا
//      عن أخيه، وهو عطبٌ مقيسٌ في هذا المستودعِ من قبل. فكلُّ سطرٍ هنا يُصرَّفُ
//      في المنصّاتِ الثلاثِ وفي التكوينَين.
//
//      **الوظيفة:** مفترقُ الخلفيّةِ نفسُه الموجودُ في الملفِّ الأصليّ — عند
//      «--خلفية-أصلية» يُسلَك المسارُ السياديّ — وإلّا **رفضٌ صريحٌ مشخَّص**
//      برمزِ الكتالوج `INT_LLVM_PATH_ABSENT`. والصمتُ هو العطبُ الذي يحرسُه هذا
//      الملفّ: أداةٌ تقبلُ عَلَمًا لا تنفّذُه أسوأُ من أداةٍ ترفضُه.
//
//      **ما ليسَ هنا:** المرحلةُ الوسطى وطباعةُ SIR — انتقلتا إلى
//      `compiler_driver_middleend.cpp` ويشترك فيهما المترجمانِ حرفًا بحرف، كي لا
//      يختلفَ ناتجُ الخلفيّةِ الأصليّةِ بينَ `sad-build` و`sad-build-native`.
//
// (EN) This file replaces compiler_driver_backend.cpp in the `sad-build-native`
//      target only. It is a *whole TU that is always compiled in its target*,
//      not an `#ifdef` branch inside another file: a branch that is not compiled
//      in most cells drifts silently from its sibling — a defect already
//      measured in this repo. Every line here is compiled on all three platforms
//      in both configurations.
//
//      Function: the same backend fork as the original file — with
//      "--خلفية-أصلية" take the sovereign path — otherwise an explicit,
//      diagnosed rejection carrying the catalog code INT_LLVM_PATH_ABSENT.
//      Silence is the defect this file guards against: a tool that accepts a
//      flag it does not honour is worse than one that rejects it.
// ============================================================================

#include "compiler_driver.h"
#include "error_codes.h"
#include "error_messages_generated.h"

#include <string>

namespace sad
{
    namespace driver
    {
        namespace
        {
            // (AR) اسمُ نوعِ المخرَجِ لبيانات {detail} — من تعدادِ المُشغِّلِ لا من نصٍّ حرّ،
            //      فلا يمكنُ أن يصفَ الرفضُ مخرَجًا غيرَ الذي رُفض.
            // (EN) The output-type name for {detail}, taken from the driver enum
            //      rather than free text, so the rejection cannot name the wrong output.
            const char *outputTypeName(OutputType type)
            {
                switch (type)
                {
                case OutputType::EXECUTABLE: return "executable";
                case OutputType::BYTECODE: return "bytecode";
                case OutputType::LLVM_IR: return "llvm-ir";
                case OutputType::LLVM_BC: return "llvm-bc";
                case OutputType::OBJECT_FILE: return "object";
                case OutputType::ASSEMBLY: return "assembly";
                case OutputType::SHARED_LIBRARY: return "shared-library";
                case OutputType::STATIC_LIBRARY: return "static-library";
                }
                return "unknown";
            }

            // (AR) رسالةُ الرفضِ من كتالوجِ SoT مع إحلالِ {detail} — لا سلسلةَ نصٍّ
            //      مباشرةً في الشيفرة (قاعدةُ المستودعِ الجارية).
            // (EN) The rejection message from the SoT catalog with {detail}
            //      substituted — no raw string literal in code (repo rule).
            std::string catalogMessage(::Sad::Errors::ErrorCode code,
                                       const std::string &detail)
            {
                const auto *entry = ::Sad::Errors::Generated::findByCode(code);
                if (!entry || !entry->briefAr)
                {
                    return {};
                }
                std::string message = entry->briefAr;
                static const std::string kPlaceholder = "{detail}";
                const auto position = message.find(kPlaceholder);
                if (position != std::string::npos)
                {
                    message.replace(position, kPlaceholder.size(), detail);
                }
                return message;
            }
        } // namespace

        bool CompilerDriver::run_backend()
        {
            // (AR) الحارسُ نفسُه في النسخةِ المبنيّةِ بـLLVM: لا خلفيّةَ بلا وحدةِ SIR.
            // (EN) The same guard as the LLVM-built version: no backend without SIR.
            if (!sir_module_)
            {
                diagnostics_.report_fatal(catalogMessage(
                    ::Sad::Errors::ErrorCode::INT_NATIVE_NO_MODULE, {}));
                return false;
            }

            // (AR) المسارُ الوحيدُ الذي يبلغُه هذا الثنائيّ: ص ⇒ SIR ⇒ شيفرةُ آلةٍ ⇒ ELF64.
            // (EN) The only path this binary can reach: ص ⇒ SIR ⇒ machine code ⇒ ELF64.
            if (options_.native_backend)
            {
                return run_native_backend();
            }

            // (AR) كلُّ ما عداه يمرُّ بـLLVM، وهي ليست في هذا الثنائيّ أصلًا.
            //      لا سقوطَ صامتٌ إلى الخلفيّةِ الأصليّة: مَن طلبَ `-c` أو `--emit-llvm`
            //      طلبَ مخرَجًا بعينِه، وإعطاؤه تنفيذيًّا مكانَه كذبٌ لا تسامح.
            // (EN) Everything else goes through LLVM, which is not in this binary.
            //      No silent fallback to the native backend: whoever asked for -c or
            //      --emit-llvm asked for a specific artefact, and handing them an
            //      executable instead would be a lie.
            diagnostics_.report_fatal(
                catalogMessage(::Sad::Errors::ErrorCode::INT_LLVM_PATH_ABSENT,
                               outputTypeName(options_.output_type)));
            return false;
        }

    } // namespace driver
} // namespace sad
// بسم الله الرحمن الرحيم
