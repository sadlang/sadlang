// بسم الله الرحمن الرحيم
// ============================================================================
// Compiler Driver — CLI & Utilities
// المحرك — واجهة سطر الأوامر والدوال المساعدة
// ============================================================================
// هذا الملف يحتوي على:
//   - print_version()     : طباعة إصدار المترجم
//   - print_help()        : طباعة رسالة المساعدة ثنائية اللغة
//   - CommandLineParser   : تحليل وسائط سطر الأوامر (parse + parse_option)
//   - Utility functions   : دوال مساعدة (امتدادات الملفات، وجود الملف، اسم الإخراج...)
// ============================================================================

#include "compiler_driver.h"
#include "utf8_utils.h"
#include "cli_flags_generated.h" // (AR) المصدر الوحيد لأعلام المترجم الطويلة

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace sad
{
    namespace driver
    {
        // (AR) اسم مستعار: جدول الأعلام المولَّد يعيش في sad::compiler::cli_gen.
        namespace cli_gen = ::sad::cli;

        // (AR) اختصارات للأعلام القصيرة القياسيّة — كلّها من المصدر الوحيد المولَّد.
        namespace
        {
            constexpr const char *SF_O0 = cli_gen::short_flags::OptO0;
            constexpr const char *SF_O1 = cli_gen::short_flags::OptO1;
            constexpr const char *SF_O2 = cli_gen::short_flags::OptO2;
            constexpr const char *SF_O3 = cli_gen::short_flags::OptO3;
            constexpr const char *SF_OS = cli_gen::short_flags::OptOs;
            constexpr const char *SF_OZ = cli_gen::short_flags::OptOz;
            constexpr const char *SF_C = cli_gen::short_flags::CompileOnly;
            constexpr const char *SF_S = cli_gen::short_flags::EmitAsm;
            constexpr const char *SF_G = cli_gen::short_flags::DebugInfo;
            constexpr const char *SF_V = cli_gen::short_flags::VerboseShort;
            constexpr const char *SF_WERROR = cli_gen::short_flags::WarningsAsErrors;
            constexpr const char *SF_LIBPATH = cli_gen::short_flags::LibPathPrefix;
            constexpr const char *SF_LIBNAME = cli_gen::short_flags::LibNamePrefix;
            constexpr const char *SF_OUT = cli_gen::short_flags::OutputFile;
            constexpr const char *SF_TARGET = cli_gen::short_flags::TargetShort;
            constexpr const char *SF_HELP = cli_gen::short_flags::HelpShort;

            // (AR) مطابقة بادئة بدلالة ثابت مولَّد (لا أطوال سحريّة في الكود).
            bool starts_with(const std::string &arg, const char *prefix)
            {
                const std::size_t n = std::strlen(prefix);
                return arg.size() >= n && arg.compare(0, n, prefix) == 0;
            }
        } // namespace

        void CompilerDriver::print_version(std::ostream &os)
        {
            os << colors::BOLD << get_compiler_name() << " " << get_version() << colors::RESET << "\n";
            os << "Copyright (C) 2024 Sad Language Project\n";
            os << "This is free software; see the source for copying conditions.\n";

#if defined(ENABLE_LLVM) || defined(ENABLE_LLVM_BACKEND)
            os << "\nLLVM support: " << colors::GREEN << "enabled" << colors::RESET << "\n";
#else
            os << "\nLLVM support: " << colors::YELLOW << "disabled" << colors::RESET << "\n";
#endif

            os << "\nTarget: " << TargetTriple::get_host_target().to_string() << "\n";
        }

        void CompilerDriver::print_help(std::ostream &os)
        {
            os << colors::BOLD << get_compiler_name() << " - Sad Language Compiler\n"
               << colors::RESET;
            os << "مترجم لغة Sad / Sad Language Compiler\n\n";

            os << "Usage / الاستخدام:\n";
            os << "  sadc [options] <input files...>\n\n";

            os << "Examples / أمثلة:\n";
            os << "  sadc program.ص -o program           "
               << colors::CYAN << "# Compile to executable\n"
               << colors::RESET;
            os << "  sadc program.ص -O3 -o program       "
               << colors::CYAN << "# With optimizations\n"
               << colors::RESET;
            os << "  sadc program.ص -c -o program.o      "
               << colors::CYAN << "# Compile to object file\n"
               << colors::RESET;
            os << "  sadc program.ص --أظهر-llvm -o prog.ll "
               << colors::CYAN << "# Emit LLVM IR\n"
               << colors::RESET;
            os << "  sadc *.o -o program                 "
               << colors::CYAN << "# Link object files\n\n"
               << colors::RESET;

            os << "Options / الخيارات:\n";
            os << "  (AR) أعلام قصيرة قياسيّة / standard short flags:\n";
            for (std::size_t i = 0; i < cli_gen::kShortFlagCount; ++i)
            {
                const auto &sf = cli_gen::kShortFlags[i];
                os << "    " << sf.name << "   " << sf.desc_ar << " / " << sf.desc_en << "\n";
            }
            os << "\n";
            os << "  (AR) أعلام طويلة — أسماء عربيّة قانونيّة وحيدة (بلا مرادفات):\n";
            os << "  (EN) long flags — single canonical Arabic names (no aliases):\n";
            for (std::size_t i = 0; i < cli_gen::kFlagCount; ++i)
            {
                const auto &spec = cli_gen::kFlags[i];
                // (AR) اعرض ما يقبله المترجم فقط: أعلامه + أعلام الذاكرة (يبتلعها
                //      الماسح المسبق فتعمل مع sad-build فعليًّا).
                if (!spec.for_compiler && !spec.for_memory)
                {
                    continue;
                }
                std::string name = spec.canonical;
                if (spec.kind == cli_gen::FlagKind::Value && spec.value_hint[0] != '\0')
                {
                    name += "=<";
                    name += spec.value_hint;
                    name += ">";
                }
                os << "    " << name << "\n";
                os << "        " << spec.desc_ar << " / " << spec.desc_en << "\n";
            }
        }

        // ============================================================================
        // CommandLineParser Implementation / تنفيذ محلل سطر الأوامر
        // ============================================================================

        CommandLineParser::CommandLineParser(int argc, char *argv[])
            : argc_(argc), argv_(argv) {}

        bool CommandLineParser::parse(CompilerOptions &options, DiagnosticEngine &diag)
        {
            // Parse each argument
            for (int i = 1; i < argc_; ++i)
            {
                std::string arg = argv_[i];

                // (AR) -h القصير القياسيّ يبقى؛ الأسماء الطويلة (--مساعدة/--إصدار)
                //      تُدار عبر المصدر الوحيد في parse_option ثمّ نتوقّف هنا.
                // (EN) Short -h stays; long --مساعدة/--إصدار flow through the SoT table.
                if (arg == SF_HELP)
                {
                    help_requested_ = true;
                    return true;
                }

                // Parse option or add as input file
                if (arg[0] == '-')
                {
                    if (!parse_option(arg, options, diag))
                    {
                        return false;
                    }

                    // (AR) توقّف فور طلب المساعدة/الإصدار عبر الجدول (--مساعدة/--إصدار)
                    if (help_requested_ || version_requested_)
                    {
                        return true;
                    }

                    // Some options take a value (next argument)
                    if (arg == SF_OUT || arg == SF_TARGET)
                    {
                        if (i + 1 < argc_)
                        {
                            if (arg == SF_OUT)
                            {
                                options.output_file = argv_[++i];
                            }
                            else if (arg == SF_TARGET)
                            {
                                // (AR) -T تحدد الهدف (target triple) — مثل --target=
                                // (EN) -T sets target triple — same as --target=
                                std::string triple_str = argv_[++i];
                                auto triple_opt = TargetTriple::parse(triple_str);
                                if (triple_opt)
                                {
                                    options.target = *triple_opt;
                                    options.target_explicit = true;
                                }
                                else
                                {
                                    diag.report_error(
                                        std::string(cli_gen::messages::InvalidTargetTriple) + triple_str);
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            diag.report_error(arg + cli_gen::messages::RequiresArgument);
                            return false;
                        }
                    }
                }
                else
                {
                    // Input file
                    options.input_files.push_back(arg);
                }
            }

            // ────────────────────────────────────────────────────────────────
            // (AR) «--حجم-الكومة» بلا وضعٍ حرّ خطأٌ — لكنّه **لا يُفحَص هنا**.
            //      الوضعُ الحرُّ يُفعَّل من طريقين: العلمُ «--حرّ»، وسمةُ المصدر
            //      «#![بلا_مكتبة_قياسية]»/«#![no_std]». والثانيةُ لا تُقرأ إلّا
            //      بعد فتحِ الملفّ، فالفحصُ زمنَ الوزم كان يرفض نواةً حرّةً فعلًا
            //      ويصفُها بالمستضافة. موضعُه الصحيح: بعد استقرارِ العَلَم في
            //      المرحلة 0 من التحليل (compiler_driver_analysis.cpp).
            // (EN) «--حجم-الكومة» without freestanding is an error — but it is
            //      NOT checked here. Freestanding can also be activated by a
            //      source attribute, which is only read after the file is
            //      opened; checking at parse time rejected genuinely
            //      freestanding kernels. See phase 0 in the analysis stage.
            // ────────────────────────────────────────────────────────────────

            return true;
        }

        bool CommandLineParser::parse_option(const std::string &arg, CompilerOptions &options,
                                             DiagnosticEngine &diag)
        {
            // ─── (AR) أعلام قصيرة قياسيّة موروثة من سلسلة الأدوات (ليست في المصدر الوحيد) ───
            // (EN) Standard short toolchain flags — kept verbatim, not part of the SoT.
            if (arg == SF_O0) { options.opt_level = OptimizationLevel::O0; return true; }
            if (arg == SF_O1) { options.opt_level = OptimizationLevel::O1; return true; }
            if (arg == SF_O2) { options.opt_level = OptimizationLevel::O2; return true; }
            if (arg == SF_O3) { options.opt_level = OptimizationLevel::O3; return true; }
            if (arg == SF_OS) { options.opt_level = OptimizationLevel::Os; return true; }
            if (arg == SF_OZ) { options.opt_level = OptimizationLevel::Oz; return true; }
            if (arg == SF_C) { options.output_type = OutputType::OBJECT_FILE; return true; }
            if (arg == SF_S) { options.output_type = OutputType::ASSEMBLY; return true; }
            if (arg == SF_G) { options.debug_info = true; return true; }
            if (arg == SF_V) { options.verbose = true; return true; }
            if (arg == SF_WERROR) { options.warnings_as_errors = true; return true; }
            if (starts_with(arg, SF_LIBPATH))
            {
                options.library_paths.push_back(arg.substr(std::strlen(SF_LIBPATH)));
                return true;
            }
            if (starts_with(arg, SF_LIBNAME))
            {
                options.libraries.push_back(arg.substr(std::strlen(SF_LIBNAME)));
                return true;
            }

            // ─── (AR) الأعلام الطويلة من المصدر الوحيد (بلا مرادفات ولا توافق خلفيّ) ───
            // (EN) Long flags from the single source of truth — no aliases, no back-compat.
            for (std::size_t k = 0; k < cli_gen::kFlagCount; ++k)
            {
                const auto &spec = cli_gen::kFlags[k];
                // (AR) أعلام المفسّر/الذاكرة ليست للمترجم: أعلام الذاكرة يبتلعها
                //      الماسح المسبق، وأعلام المفسّر خيارٌ غير معروف هنا (بحقّ).
                // (EN) Interpreter/memory flags aren't the compiler's: memory ones are
                //      consumed by the pre-scanner; interpreter ones are unknown here.
                if (!spec.for_compiler)
                {
                    continue;
                }
                const std::string name = spec.canonical;
                const auto act = static_cast<std::uint16_t>(spec.action);
                if (arg == name)
                {
                    return dispatch_flag(act, name, "", options, diag);
                }
                if (spec.kind == cli_gen::FlagKind::Value)
                {
                    const std::string prefix = name + "=";
                    if (arg.size() > prefix.size() && arg.compare(0, prefix.size(), prefix) == 0)
                    {
                        return dispatch_flag(act, name, arg.substr(prefix.size()), options, diag);
                    }
                }
            }

            // ─── (AR) خيار غير معروف (خلا -o/-T اللذين يبتلعان الوسيط التالي في parse) ───
            // (AR) خطأ لا تحذير: بعد إلغاء كلّ المرادفات والتوافق الخلفيّ، لا بدّ أن
            //      يفشل الاستعمال القديم بصوتٍ عالٍ — وإلّا صار كلّ علم بائت فشلًا
            //      صامتًا (يُنتج مخرجًا خاطئًا بدل أن يتوقّف).
            // (EN) Error, not warning: with all aliases/back-compat removed, stale
            //      flags must fail loudly instead of silently producing wrong output.
            if (arg != SF_OUT && arg != SF_TARGET)
            {
                diag.report_error(std::string(cli_gen::messages::UnknownOptionPrefix) + arg);
                return false;
            }
            return true;
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) مُوزِّع الأعلام: يُنفّذ الإجراء الموسوم في المصدر الوحيد
        //      (cli_flags_generated.h). كلّ الأسماء تأتي من الجدول؛ رسائل الخطأ
        //      تستعمل الاسم القانونيّ المُمرَّر (لا سلاسل أسماء حرّة في الكود).
        // (EN) Flag dispatcher: runs the tagged action from the generated SoT table.
        // ════════════════════════════════════════════════════════════════════
        bool CommandLineParser::dispatch_flag(std::uint16_t action, const std::string &canonical,
                                              const std::string &value, CompilerOptions &options,
                                              DiagnosticEngine &diag)
        {
            using A = cli_gen::FlagAction;
            const auto requireValue = [&]() -> bool {
                diag.report_error(canonical + cli_gen::messages::RequiresValue);
                return false;
            };

            switch (static_cast<A>(action))
            {
            // ─── الوضع الحرّ ───
            // (AR) «--حرّ» يُدار في ماسح الذاكرة المسبق ويُعاد ربطه في الواجهة
            //      الأماميّة؛ لذا لا حالة له هنا. الباقي أعلام توليديّة.
            case A::NoMain: options.no_main = true; return true;
            case A::AbortOnPanic: options.abort_on_panic = true; return true;
            case A::AllowAlloc: options.allow_freestanding_alloc = true; return true;
            case A::HeapSize:
            {
                // (AR) الصيغةُ وحدَها، بلا سقف: السقفُ يعتمد عرضَ الهدف وهو مجهولٌ
                //      في هذه الطبقة (قد يأتي «--هدف» بعد هذا العلم). التفصيلُ في
                //      ‎emitFreestandingMalloc‎ (‎SEM_FREESTANDING_HEAP_TOO_LARGE‎).
                //
                //      والتحقّقُ يدويٌّ لا بـ‎stoul‎: ‎stoul("64ك")‎ تعيد 64 صامتةً
                //      فتصير الوحدةُ المكتوبةُ خطأً حجمًا مقبولًا. لا تُبسِّطه.
                // (EN) Syntax only; the real bound needs the target width (see
                //      emitFreestandingMalloc). Hand-rolled parse on purpose:
                //      stoul("64ك") silently returns 64.
                if (value.empty()) return requireValue();
                std::uint64_t mib = 0;
                for (char ch : value)
                {
                    if (ch < '0' || ch > '9')
                    {
                        diag.report_error(canonical + cli_gen::messages::HeapSizeInvalid + value);
                        return false;
                    }
                    mib = mib * 10 + static_cast<std::uint64_t>(ch - '0');
                    // (AR) حارسُ فيضانِ الحساب نفسِه (لا سقفُ سياسة): رقمٌ من
                    //      عشرين خانةً يلتفّ في ‎uint64‎ قبل أن يبلغ التوليد.
                    if (mib > (std::numeric_limits<std::uint32_t>::max)())
                    {
                        diag.report_error(canonical + cli_gen::messages::HeapSizeInvalid + value);
                        return false;
                    }
                }
                if (mib == 0)
                {
                    diag.report_error(canonical + cli_gen::messages::HeapSizeInvalid + value);
                    return false;
                }
                options.freestanding_heap_mib = static_cast<std::uint32_t>(mib);
                return true;
            }
            case A::LinkerScript:
                if (value.empty()) return requireValue();
                options.linker_script = value; return true;
            case A::EntryPoint:
                if (value.empty()) return requireValue();
                options.freestanding_entry = value; return true;

            // ─── الوحدة والربط ───
            case A::ModuleMode: options.module_mode = true; return true;
            case A::LinkStatic: options.link_static = true; return true;
            case A::OutShared: options.output_type = OutputType::SHARED_LIBRARY; return true;
            case A::Lto:
                if (value.empty() || value == cli_gen::values::LtoFull)
                {
                    options.enable_lto_full = true;
                    options.enable_lto_thin = false;
                }
                else if (value == cli_gen::values::LtoThin)
                {
                    options.enable_lto_thin = true;
                    options.enable_lto_full = false;
                }
                else
                {
                    diag.report_error(canonical + cli_gen::messages::UnknownValue + value);
                    return false;
                }
                return true;
            case A::LtoNone:
                options.enable_lto_full = false;
                options.enable_lto_thin = false;
                return true;

            // ─── الهدف ───
            case A::Target:
            {
                if (value.empty()) return requireValue();
                auto triple_opt = TargetTriple::parse(value);
                if (!triple_opt)
                {
                    diag.report_error(std::string(cli_gen::messages::InvalidTargetTriple) + value);
                    return false;
                }
                options.target = *triple_opt;
                options.target_explicit = true;
                return true;
            }

            // ─── مخرجات وسيطة ───
            case A::EmitLlvm: options.output_type = OutputType::LLVM_IR; return true;
            case A::EmitBc: options.output_type = OutputType::LLVM_BC; return true;
            case A::EmitAst: options.emit_ast = true; return true;
            case A::EmitAstJson: options.emit_ast_json = true; return true;
            case A::EmitSir: options.emit_sir = true; return true;
            case A::TimePasses: options.time_passes = true; return true;

            // ─── الشرح واللغة ───
            case A::Explain:
                options.explain_level =
                    value.empty() ? std::string(cli_gen::values::ExplainDefault) : value;
                return true;
            case A::OutputLanguage:
                if (value.empty()) return requireValue();
                options.output_language = value; return true;

            // ─── الألوان والإسهاب ───
            case A::ColorOn: options.color_diagnostics = true; return true;
            case A::ColorOff:
                options.color_diagnostics = false;
                colors::disable_colors();
                return true;
            case A::Verbose: options.verbose = true; return true;

            // ─── فحص الاستعارة ───
            case A::BorrowCheckOn: options.enable_borrow_check = true; return true;
            case A::BorrowCheckOff: options.enable_borrow_check = false; return true;
            case A::DebugBorrow: options.debug_borrow_check = true; return true;
            case A::BorrowMsgArabic: options.arabic_borrow_messages = true; return true;
            case A::BorrowMsgEnglish: options.arabic_borrow_messages = false; return true;

            // ─── فحص الأنواع ───
            case A::TypeCheckOn: options.enable_type_check = true; return true;
            case A::TypeCheckOff: options.enable_type_check = false; return true;
            case A::DebugTypes: options.debug_type_check = true; return true;
            case A::StrictTypes: options.strict_type_check = true; return true;

            // ─── الواجهات الرسومية ───
            case A::EmitUi: options.emit_ui = true; return true;
            case A::UiPlatform:
                if (value.empty()) return requireValue();
                options.emit_ui = true;
                options.ui_platform = value;
                return true;
            case A::PlatformDesktop:
                options.emit_ui = true;
                options.ui_platform = cli_gen::values::PlatformDesktop;
                return true;
            case A::PlatformAndroid:
                options.emit_ui = true;
                options.ui_platform = cli_gen::values::PlatformAndroid;
                // (AR) عند تفعيل أندرويد نضبط الهدف تلقائيًّا إن لم يُحدَّد هدف أندرويد.
                if (!options.target.is_android())
                {
                    auto android_triple = TargetTriple::parse(cli_gen::values::AndroidTargetTriple);
                    if (android_triple)
                    {
                        options.target = *android_triple;
                        // (AR) هدف مضبوط عمدًا — لا يُطلَق عليه تحذير «افتُرض ثالوث
                        //      المضيف» (وإلّا كذَب التشخيص على المستخدم بثالوث لم يُفترَض).
                        // (EN) An intentionally set target — must not trigger the
                        //      «host triple assumed» warning (else the diagnostic lies).
                        options.target_explicit = true;
                    }
                }
                return true;
            case A::PlatformIos:
                options.emit_ui = true;
                options.ui_platform = cli_gen::values::PlatformIos;
                return true;
            case A::PlatformWeb:
                options.emit_ui = true;
                options.ui_platform = cli_gen::values::PlatformWeb;
                return true;

            // ─── التوثيق ───
            case A::EmitDocs: options.emit_docs = true; return true;
            case A::DocsOut:
                if (value.empty()) return requireValue();
                options.emit_docs = true;
                options.docs_output_path = value;
                return true;
            case A::DocsProject:
                if (value.empty()) return requireValue();
                options.emit_docs = true;
                options.docs_project_dir = value;
                return true;
            case A::DocsProjectName:
                if (value.empty()) return requireValue();
                options.docs_project_name = value;
                return true;
            case A::DocsFormat:
                if (value.empty()) return requireValue();
                options.emit_docs = true;
                options.docs_format = value;
                return true;
            case A::DocsExclude:
                if (value.empty()) return requireValue();
                options.docs_excludes.push_back(value);
                return true;

            // ─── المساعدة والإصدار ───
            case A::HelpRequested: help_requested_ = true; return true;
            case A::VersionRequested: version_requested_ = true; return true;

            // (AR) حارس: إجراء في المصدر الوحيد بلا حالة هنا ⇒ فشل صريح، لا قبول
            //      صامت لعلمٍ لا يفعل شيئًا.
            // (EN) Guard: an SoT action with no case here fails loudly instead of
            //      silently accepting a no-op flag.
            default:
                diag.report_error(canonical + cli_gen::messages::UnimplementedAction);
                return false;
            }
        }

        const char *canonical_flag(std::uint16_t action)
        {
            const auto act = static_cast<cli_gen::FlagAction>(action);
            for (std::size_t i = 0; i < cli_gen::kFlagCount; ++i)
            {
                if (cli_gen::kFlags[i].action == act)
                {
                    return cli_gen::kFlags[i].canonical;
                }
            }
            return "";
        }

        // ============================================================================
        // Utility Functions / دوال مساعدة
        // ============================================================================

        std::string get_file_extension(const std::string &path)
        {
            return sad::utf8::get_extension(path);
        }

        std::string change_extension(const std::string &path, const std::string &new_ext)
        {
            return sad::utf8::change_extension(path, new_ext);
        }

        bool file_exists(const std::string &path)
        {
            return sad::utf8::file_exists(path);
        }

        std::optional<std::string> read_file(const std::string &path)
        {
            return sad::utf8::read_file(path);
        }

        std::filesystem::path get_executable_dir()
        {
            return sad::utf8::get_executable_dir();
        }

        std::optional<std::string> find_system_linker()
        {
            // Try to find system linker
            const char *linkers[] = {
                "ld",      // GNU ld
                "lld",     // LLVM linker
                "ld.lld",  // LLVM linker (alternative name)
                "link.exe" // MSVC linker (Windows)
            };

            for (const char *linker : linkers)
            {
                // Check if linker exists in PATH
                std::string command = std::string("which ") + linker + " > /dev/null 2>&1";

#ifdef _WIN32
                command = std::string("where ") + linker + " > nul 2>&1";
#endif

                if (std::system(command.c_str()) == 0)
                {
                    return std::string(linker);
                }
            }

            return std::nullopt;
        }

        std::string get_default_output_name(const std::string &input_file,
                                            OutputType output_type)
        {
            std::string base = sad::utf8::get_stem(input_file);

            switch (output_type)
            {
            case OutputType::EXECUTABLE:
#ifdef _WIN32
                return base + ".exe";
#else
                return base;
#endif

            case OutputType::BYTECODE:
                return base + ".sbc";

            case OutputType::LLVM_IR:
                return base + ".ll";

            case OutputType::LLVM_BC:
                return base + ".bc";

            case OutputType::OBJECT_FILE:
                return base + ".o";

            case OutputType::ASSEMBLY:
                return base + ".asm";

            case OutputType::SHARED_LIBRARY:
#ifdef _WIN32
                return base + ".dll";
#elif defined(__APPLE__)
                return "lib" + base + ".dylib";
#else
                return "lib" + base + ".so";
#endif

            case OutputType::STATIC_LIBRARY:
#ifdef _WIN32
                return base + ".lib";
#else
                return "lib" + base + ".a";
#endif

            default:
                return base;
            }
        }

    } // namespace driver
} // namespace sad
