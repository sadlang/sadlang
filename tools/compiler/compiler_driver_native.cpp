// بسم الله الرحمن الرحيم
// ============================================================================
// Compiler Driver — Native (no-LLVM) Backend
// مشغّل المترجم — الخلفيّة الأصليّة السياديّة (بلا LLVM)
// ============================================================================
// (AR) يصِل هذا الملفُّ الخلفيّةَ الأصليّةَ بمسارِ الترجمة الحيّ. حتّى الآن كانت
//      مخفّضاتُ SIR ⇒ شيفرةِ الآلة (x86-64 وARM64) مُبرهَنةً بالاختبارات وحدَها،
//      لا يبلغها أيُّ مستخدِمٍ من سطر الأوامر. بهذا الملفّ يصير «--خلفية-أصلية»
//      مساراً كاملاً: مصدرُ ص ⇒ SIR ⇒ شيفرةُ آلة ⇒ ثنائيُّ ELF64 ساكن، بلا
//      LLVM ولا رابطٍ أجنبيّ (لا ld ولا lld) ولا سلسلةِ أدواتٍ خارجيّة البتّة.
//
//      المعماريّةُ تُشتقّ من حقلِ architecture في ثالوثِ «--هدف»: aarch64/arm64 ⇒ ARM64،
//      وإلّا x86-64 (الافتراض). لا علمَ معماريّةٍ مستقلّ: الهدفُ مصدرٌ واحد.
//
// (EN) Wires the sovereign native backend into the live compilation path.
//      Until now the SIR ⇒ machine-code lowerings (x86-64, ARM64) were proven
//      by tests only and unreachable from the CLI. With this file
//      "--خلفية-أصلية" becomes a complete path: ص source ⇒ SIR ⇒ machine code
//      ⇒ static ELF64, with no LLVM, no foreign linker, no external toolchain.
// ============================================================================

#include "compiler_driver.h"
#include "error_codes.h"
#include "../../compiler/include/frontend/sir_module.h"
#include "../../compiler/include/backend/native/sir_native_lowering.h"
#include "../../compiler/include/backend/native/arm64_sir_lowering.h"

#include <filesystem>
#include <fstream>
#include <system_error>
#include <iostream>
#include <string>
#include <vector>

#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace sad
{
    namespace driver
    {
        namespace
        {
            // (AR) أسماءُ معماريّة AArch64 في ثالوثِ الهدف. الثالوثُ مفكّكٌ أصلًا
            //      (TargetTriple::parse)، فالمقارنةُ على حقلِ architecture تطابُقٌ
            //      تامٌّ لا مطابقةُ بادئةٍ على نصٍّ خام.
            // (EN) AArch64 architecture names in the target triple. The triple is
            //      already parsed (TargetTriple::parse), so matching the architecture
            //      field is an exact test, not a prefix guess over a raw string.
            constexpr const char *kArchAarch64 = "aarch64";
            constexpr const char *kArchArm64 = "arm64";
            constexpr const char *kArchX8664 = "x86_64";

            // (AR) هل الهدفُ AArch64؟ دونَ «--هدف» يكون الثالوثُ ثالوثَ المضيف،
            //      فيُخفّض لمعماريّةِ المضيف نفسِها.
            // (EN) Is the target AArch64? With no --هدف the triple is the host's,
            //      so lowering targets the host architecture.
            bool targetIsArm64(const TargetTriple &triple)
            {
                return triple.architecture == kArchAarch64 ||
                       triple.architecture == kArchArm64;
            }

            // (AR) أنظمةُ التشغيل التي يصلح لها كاتبُ ELF64: لينكس، و«لا نظام» (معدنٌ
            //      عارٍ/الوضع الحرّ — ثالوثٌ كـx86_64-unknown-none-elf). ويندوز (PE)
            //      وماك (Mach-O) حاويتانِ مختلفتان لا يكتبهما هذا المسار.
            // (EN) OSes the ELF64 writer is valid for: linux and "none" (bare metal /
            //      freestanding). Windows (PE) and macOS (Mach-O) are different
            //      containers this path does not write.
            constexpr const char *kOsLinux = "linux";
            constexpr const char *kOsNone = "none";

            bool osIsElf(const std::string &os)
            {
                // (AR) ثالوثٌ بلا نظامٍ مذكور (مثل «aarch64») يُعامَل معاملةَ «none»:
                //      لا يدّعي منصّةً غيرَ ELF، والوضعُ الحرّ هو الحالةُ الطبيعيّة هنا.
                return os.empty() || os == kOsLinux || os == kOsNone;
            }

            // (AR) هل يُخرِج هذا الهدفُ ELF64 بمخفّضٍ موجود؟ الشرطانِ معًا: معماريّةٌ
            //      لها مخفّض، ونظامٌ حاويتُه ELF. الرفضُ الصريحُ عند غيرِهما يمنع
            //      إخراجَ ELF x86-64 لهدفِ wasm أو ويندوز صامتًا.
            // (EN) Does this target yield ELF64 with an existing lowerer? Both must
            //      hold: an architecture we lower, and an ELF-container OS.
            bool targetIsSupported(const TargetTriple &triple)
            {
                const bool arch = triple.architecture == kArchX8664 ||
                                  targetIsArm64(triple);
                return arch && osIsElf(triple.os);
            }

            // (AR) اسمُ نوعِ المخرَج لبيانات {detail} — من تعداد المُشغِّل لا من نصٍّ حرّ.
            // (EN) Output-type name for the {detail} data, from the driver enum.
            const char *outputTypeName(OutputType t)
            {
                switch (t)
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

            // (AR) رسالةُ خطأٍ من كتالوج SoT مع إحلال {detail}. تُبنى عبر LoweringResult
            //      نفسِه لا بمُنسِّقٍ ثانٍ: صيغةُ الرسالة واحدةٌ في الخلفيّة الأصليّة كلِّها.
            // (EN) A SoT-catalog message with {detail} substituted, built through
            //      LoweringResult itself rather than a second formatter, so the whole
            //      native backend renders its diagnostics one way.
            std::string sotMessage(::Sad::Errors::ErrorCode code, const std::string &detail)
            {
                ::sad::native::LoweringResult r;
                r.errorCode = code;
                r.detail = detail;
                return r.message();
            }
        } // namespace

        bool CompilerDriver::run_native_backend()
        {
            // (AR) الحارسُ نفسُه في run_backend، ويُعاد هنا لأنّ الدالّة نقطةُ دخولٍ
            //      قائمةٌ بذاتها: لا تخفيضَ بلا وحدةِ SIR.
            // (EN) Same guard as run_backend; repeated because this is a standalone
            //      entry point — no lowering without a SIR module.
            if (!sir_module_)
            {
                diagnostics_.report_fatal(
                    sotMessage(::Sad::Errors::ErrorCode::INT_NATIVE_NO_MODULE, {}));
                return false;
            }

            // (AR) حارسُ الهدف قبل التخفيض: أرخصُ من توليدِ بايتاتٍ ثمّ اكتشافِ أنّها
            //      لحاويةٍ خاطئة — وأصدقُ من كتابةِ ELF لهدفٍ لا يقرؤه.
            // (EN) Target guard before lowering: cheaper than emitting bytes then
            //      discovering the wrong container, and honest about what we support.
            if (!targetIsSupported(options_.target))
            {
                diagnostics_.report_fatal(
                    sotMessage(::Sad::Errors::ErrorCode::INT_NATIVE_TARGET_UNSUPPORTED,
                               options_.target.to_string()));
                return false;
            }

            // (AR) هذا المسارُ يُخرِج تنفيذيًّا ساكناً وحدَه: لا مُجمِّعَ ولا رابطَ ولا
            //      مُصدِرَ IR تحته. قبولُ «-c» أو «--مشترك» صامتًا كان يكتب تنفيذيًّا
            //      باسمِ ملفِّ كائنٍ أو مكتبة، فتُضلَّل كلُّ أداةٍ تاليةٍ في السلسلة.
            // (EN) This path emits only a static executable — no assembler, linker, or
            //      IR emitter beneath it. Silently accepting -c or --shared wrote an
            //      executable under an object/library name, misleading everything after.
            if (options_.output_type != OutputType::EXECUTABLE)
            {
                diagnostics_.report_fatal(
                    sotMessage(::Sad::Errors::ErrorCode::INT_NATIVE_OUTPUT_TYPE,
                               outputTypeName(options_.output_type)));
                return false;
            }

            const bool arm64 = targetIsArm64(options_.target);

            if (options_.verbose)
            {
                std::cout << "  [الخلفية الأصلية] SIR ⇒ "
                          << (arm64 ? "ARM64" : "x86-64") << " ⇒ ELF64\n";
            }

            // (AR) مفترقُ المعماريّة: مخفّضانِ مستقلّان، والمخرَجُ من كليهما بايتاتُ
            //      ELF64 كاملةٌ جاهزةٌ للكتابة (كلٌّ يلفُّ شيفرتَه بـe_machine هدفِه).
            // (EN) Architecture fork: two independent lowerings, each returning a
            //      complete ELF64 byte image (each wraps its code with its e_machine).
            const ::sad::native::LoweringResult result =
                arm64 ? ::sad::native::lowerModuleToElfArm64(*sir_module_)
                      : ::sad::native::lowerModuleToElf(*sir_module_);

            if (!result.ok)
            {
                // (AR) الرسالةُ من كتالوج أخطاء SoT مع إحلال {detail} — لا نصَّ مباشراً.
                // (EN) Message from the SoT error catalog with {detail} substituted.
                diagnostics_.report_fatal(result.message());
                return false;
            }

            // (AR) الكتابةُ بنمطٍ ثنائيّ: أيُّ ترجمةٍ لنهايات الأسطر تفسد الثنائيّ.
            //      وعند أيّ إخفاقٍ يُحذَف المخرَج: التقصيرُ (trunc) وقع فعلًا، فتركُ
            //      ما كُتب يخلّف ثنائيًّا مبتورًا يبدو صالحًا ويُشغَّل فينهار.
            // (EN) Binary mode: any newline translation would corrupt the image. On any
            //      failure the output is removed — trunc already happened, so leaving a
            //      partial write behind would strand a truncated binary that looks valid.
            {
                bool wrote = false;
                {
                    std::ofstream out(options_.output_file, std::ios::binary | std::ios::trunc);
                    if (out)
                    {
                        out.write(reinterpret_cast<const char *>(result.code.data()),
                                  static_cast<std::streamsize>(result.code.size()));
                        out.flush();
                        wrote = static_cast<bool>(out);
                    }
                }
                if (!wrote)
                {
                    std::error_code ignored;
                    std::filesystem::remove(options_.output_file, ignored);
                    diagnostics_.report_fatal(
                        sotMessage(::Sad::Errors::ErrorCode::INT_NATIVE_OUTPUT_WRITE,
                                   options_.output_file));
                    return false;
                }
            }

#ifndef _WIN32
            // (AR) ELF لا يُنفَّذ ببتِّ تنفيذٍ مفقود، وفشلُ chmod الصامت يُعلن نجاحًا
            //      عن ثنائيّ لا يعمل — فيُعامَل معاملةَ فشلِ كتابة. على ويندوز لا
            //      مقابلَ لهذا البتّ، والمخرَجُ ثنائيُّ Linux يأخذ صلاحيّتَه على مضيفه.
            // (EN) An ELF without the exec bit will not run, and a silently failed chmod
            //      would report success for a binary that cannot execute — so it is
            //      treated as a write failure. Windows has no such bit.
            if (::chmod(options_.output_file.c_str(),
                        S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
            {
                std::error_code ignored;
                std::filesystem::remove(options_.output_file, ignored);
                diagnostics_.report_fatal(
                    sotMessage(::Sad::Errors::ErrorCode::INT_NATIVE_OUTPUT_WRITE,
                               options_.output_file));
                return false;
            }
#endif

            if (options_.verbose)
            {
                std::cout << "  [الخلفية الأصلية] " << options_.output_file
                          << " — " << result.code.size() << " بايت\n";
            }

            return true;
        }

    } // namespace driver
} // namespace sad
// بسم الله الرحمن الرحيم
