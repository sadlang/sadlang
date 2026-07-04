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

#include <iostream>
#include <cstdlib>

namespace sad
{
    namespace driver
    {

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
            os << "  sadc program.ص --emit-llvm -o prog.ll "
               << colors::CYAN << "# Emit LLVM IR\n"
               << colors::RESET;
            os << "  sadc *.o -o program                 "
               << colors::CYAN << "# Link object files\n\n"
               << colors::RESET;

            os << "Options / الخيارات:\n";
            os << "  -o <file>              " << "Output file / ملف الإخراج\n";
            os << "  -c                     " << "Compile only (no linking) / ترجمة فقط\n";
            os << "  -S                     " << "Generate assembly / إنتاج assembly\n";
            os << "  --emit-llvm            " << "Emit LLVM IR / إخراج LLVM IR\n";
            os << "  --emit-bc              " << "Emit LLVM bitcode / إخراج LLVM bitcode\n";
            os << "\n";

            os << "Optimization / التحسين:\n";
            os << "  -O0                    " << "No optimization / بدون تحسين\n";
            os << "  -O1                    " << "Basic optimization / تحسين أساسي\n";
            os << "  -O2                    " << "Standard optimization (default) / تحسين عادي\n";
            os << "  -O3                    " << "Aggressive optimization / تحسين قصوى\n";
            os << "  -Os                    " << "Size optimization / تحسين الحجم\n";
            os << "  --lto, --lto=full      " << "Full Link-Time Optimization / تحسين كامل وقت الربط\n";
            os << "  --lto=thin             " << "ThinLTO (faster builds) / ThinLTO (بناء أسرع)\n";
            os << "\n";

            os << "Debug / التنقيح:\n";
            os << "  -g                     " << "Generate debug info / معلومات التنقيح\n";
            os << "  --emit-ast             " << "Print AST / طباعة AST\n";
            os << "  --emit-ast-json        " << "Print AST as JSON, halt before codegen / طباعة AST بصيغة JSON آليّة (توقّف قبل التوليد)\n";
            os << "  --emit-sir             " << "Print SIR / طباعة SIR\n";
            os << "  --time-passes          " << "Time each compilation pass / توقيت المراحل\n";
            os << "\n";

            os << "Documentation / التوثيق:\n";
            os << "  --docs                 " << "Extract Markdown docs to stdout / استخراج التوثيق إلى stdout\n";
            os << "  --وثّق                  " << "Arabic alias of --docs / مرادف عربي\n";
            os << "  --docs-out=<file>      " << "Write extracted docs to file / كتابة التوثيق إلى ملف\n";
            os << "  --docs-project=<dir>   " << "Recursively document all .ص files in dir / توثيق مشروع كامل\n";
            os << "  --docs-project-name=<name>  " << "Project title for project docs / اسم المشروع في التوثيق\n";
            os << "  --docs-format=<fmt>    " << "Output format: markdown|json|html / صيغة الإخراج\n";
            os << "  --docs-exclude=<sub>   " << "Exclude files containing substring (repeatable) / استبعاد ملفات\n";
            os << "\n";

            os << "Error explanation / شرح الأخطاء (Phase E-3):\n";
            os << "  --explain[=<level>]    " << "brief|normal|detailed|teacher (default: detailed)\n";
            os << "  --lang=<lang>          " << "ar|en|both — message language / لغة الرسائل\n";
            os << "\n";

            os << "Target / الهدف:\n";
            os << "  --target=<triple>      " << "Target platform / المنصة المستهدفة\n";
            os << "  --freestanding         " << "Freestanding (no OS) / مستقل بلا نظام تشغيل\n";
            os << "\n";

            os << "Freestanding / وضع بلا مكتبة قياسية:\n";
            os << "  --freestanding         " << "Full bare-metal mode / وضع bare-metal كامل\n";
            os << "                         " << "(مكافئ لـ --no-std)\n";
            os << "  --no-std               " << "Same as --freestanding / نفس --freestanding\n";
            os << "  --no-main              " << "No default main() entry / تعطيل main الافتراضية\n";
            os << "  --abort-on-panic       " << "Abort instead of unwind / إيقاف عند الذعر\n";
            os << "  --linker-script=<f>    " << "Use linker script (.ld) / سكريبت رابط\n";
            os << "  --entry-point=<name>   " << "Custom entry point name / اسم نقطة الدخول\n";
            os << "  --allow-alloc          " << "Allow heap allocation / السماح بالتخصيص الديناميكي\n";
            os << "\n";
            os << "  مثال نواة بسيطة / Minimal kernel example:\n";
            os << "    sadc --freestanding --no-main --abort-on-panic \\\n";
            os << "         --linker-script=kernel.ld kernel.ص -o kernel.elf\n";
            os << "\n";

            os << "Linking / الربط:\n";
            os << "  -L<path>               " << "Add library search path / مسار المكتبات\n";
            os << "  -l<lib>                " << "Link with library / ربط مكتبة\n";
            os << "  --static               " << "Static linking / ربط ثابت\n";
            os << "  -T<script>             " << "Use linker script / استخدام linker script\n";
            os << "\n";

            os << "Other / أخرى:\n";
            os << "  -v, --verbose          " << "Verbose output / إخراج مفصل\n";
            os << "  -h, --help             " << "Show this help / عرض المساعدة\n";
            os << "  --version              " << "Show version / عرض الإصدار\n";
            os << "  -Werror                " << "Treat warnings as errors / التحذيرات كأخطاء\n";
            os << "  --color                " << "Use colored output / استخدام ألوان\n";
            os << "  --no-color             " << "Disable colored output / تعطيل الألوان\n";
            os << "\n";

            os << "Ownership & Borrow Checking / نظام الملكية والاستعارة:\n";
            os << "  --borrow-check         " << "Enable borrow checking (default) / تفعيل فحص الاستعارة\n";
            os << "  --فحص-الاستعارة         " << "فحص الاستعارة (عربي)\n";
            os << "  --no-borrow-check      " << "Disable borrow checking / تعطيل فحص الاستعارة\n";
            os << "  --debug-borrow         " << "Debug borrow checker output / تنقيح فحص الاستعارة\n";
            os << "  --arabic-borrow        " << "Arabic borrow error messages / رسائل عربية\n";
            os << "  --english-borrow       " << "English borrow error messages / رسائل إنجليزية\n";
            os << "\n";

            os << "Type Checking / فحص الأنواع المتقدم:\n";
            os << "  --type-check           " << "Enable type checking (default) / تفعيل فحص الأنواع\n";
            os << "  --فحص-الأنواع           " << "فحص الأنواع (عربي)\n";
            os << "  --no-type-check        " << "Disable type checking / تعطيل فحص الأنواع\n";
            os << "  --debug-types          " << "Debug type checker output / تنقيح فحص الأنواع\n";
            os << "  --strict-types         " << "Strict type checking mode / وضع صارم للأنواع\n";
            os << "  --أنواع-صارمة          " << "وضع صارم للأنواع (عربي)\n";
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

                // Check for help/version
                if (arg == "-h" || arg == "--help")
                {
                    help_requested_ = true;
                    return true;
                }

                if (arg == "--version")
                {
                    version_requested_ = true;
                    return true;
                }

                // Parse option or add as input file
                if (arg[0] == '-')
                {
                    if (!parse_option(arg, options, diag))
                    {
                        return false;
                    }

                    // Some options take a value (next argument)
                    if (arg == "-o" || arg == "-T")
                    {
                        if (i + 1 < argc_)
                        {
                            if (arg == "-o")
                            {
                                options.output_file = argv_[++i];
                            }
                            else if (arg == "-T")
                            {
                                // (AR) -T تحدد الهدف (target triple) — مثل --target=
                                // (EN) -T sets target triple — same as --target=
                                std::string triple_str = argv_[++i];
                                auto triple_opt = TargetTriple::parse(triple_str);
                                if (triple_opt)
                                {
                                    options.target = *triple_opt;
                                }
                                else
                                {
                                    diag.report_error("invalid target triple: " + triple_str);
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            diag.report_error("option '" + arg + "' requires an argument");
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

            return true;
        }

        bool CommandLineParser::parse_option(const std::string &arg, CompilerOptions &options,
                                             DiagnosticEngine &diag)
        {
            // Optimization levels
            if (arg == "-O0")
            {
                options.opt_level = OptimizationLevel::O0;
            }
            else if (arg == "-O1")
            {
                options.opt_level = OptimizationLevel::O1;
            }
            else if (arg == "-O2")
            {
                options.opt_level = OptimizationLevel::O2;
            }
            else if (arg == "-O3")
            {
                options.opt_level = OptimizationLevel::O3;
            }
            else if (arg == "-Os")
            {
                options.opt_level = OptimizationLevel::Os;
            }
            else if (arg == "-Oz")
            {
                options.opt_level = OptimizationLevel::Oz;
            }

            // ─── Link-Time Optimization ───
            // (AR) --lto أو --lto=full → FullLTO
            //      --lto=thin → ThinLTO (بناء أسرع)
            // (EN) --lto / --lto=full → FullLTO; --lto=thin → ThinLTO
            else if (arg == "--lto" || arg == "--lto=full")
            {
                options.enable_lto_full = true;
                options.enable_lto_thin = false;
            }
            else if (arg == "--lto=thin")
            {
                options.enable_lto_thin = true;
                options.enable_lto_full = false;
            }
            else if (arg == "--no-lto")
            {
                options.enable_lto_full = false;
                options.enable_lto_thin = false;
            }

            // Output types
            else if (arg == "-c")
            {
                options.output_type = OutputType::OBJECT_FILE;
            }
            else if (arg == "-S")
            {
                options.output_type = OutputType::ASSEMBLY;
            }
            else if (arg == "--emit-llvm")
            {
                options.output_type = OutputType::LLVM_IR;
            }
            else if (arg == "--emit-bc")
            {
                options.output_type = OutputType::LLVM_BC;
            }
            else if (arg == "--shared")
            {
                options.output_type = OutputType::SHARED_LIBRARY;
            }

            // Debug
            else if (arg == "-g")
            {
                options.debug_info = true;
            }
            else if (arg == "--emit-ast")
            {
                options.emit_ast = true;
            }
            else if (arg == "--emit-ast-json")
            {
                options.emit_ast_json = true;
            }
            else if (arg == "--emit-sir")
            {
                options.emit_sir = true;
            }
            else if (arg == "--time-passes")
            {
                options.time_passes = true;
            }
            // ────────────────────────────────────────────────────────────────────
            // (AR) استخراج التوثيق Markdown من شجرة AST
            //      --docs                 → طباعة على stdout
            //      --وثّق                  → مرادف عربي
            //      --docs-out=<ملف>       → كتابة إلى ملف
            // (EN) Extract Markdown documentation from AST
            //      --docs                 → print to stdout
            //      --وثّق                  → Arabic alias
            //      --docs-out=<file>      → write to file
            // ────────────────────────────────────────────────────────────────────
            else if (arg == "--docs" || arg == u8"--وثّق")
            {
                options.emit_docs = true;
            }
            else if (arg.size() >= 11 && arg.substr(0, 11) == "--docs-out=")
            {
                options.emit_docs = true;
                options.docs_output_path = arg.substr(11);
            }
            // (AR) --docs-project=<dir> توثيق مشروع متعدد الملفات
            // (EN) --docs-project=<dir> multi-file project documentation
            else if (arg.size() >= 15 && arg.substr(0, 15) == "--docs-project=")
            {
                options.emit_docs = true;
                options.docs_project_dir = arg.substr(15);
            }
            // (AR) --docs-project-name=<name> اسم المشروع المعروض في العنوان
            else if (arg.size() >= 20 && arg.substr(0, 20) == "--docs-project-name=")
            {
                options.docs_project_name = arg.substr(20);
            }
            // (AR) --docs-format=markdown|json|html
            else if (arg.size() >= 14 && arg.substr(0, 14) == "--docs-format=")
            {
                options.emit_docs = true;
                options.docs_format = arg.substr(14);
            }
            // (AR) --docs-exclude=<sub> — يمكن تكراره
            else if (arg.size() >= 15 && arg.substr(0, 15) == "--docs-exclude=")
            {
                options.docs_excludes.push_back(arg.substr(15));
            }
            // (AR) Phase E-3: --explain[=<level>] و --lang=<lang>
            // (EN) Phase E-3: --explain[=<level>] and --lang=<lang>
            else if (arg.size() >= 10 && arg.substr(0, 10) == "--explain=")
            {
                options.explain_level = arg.substr(10);
            }
            else if (arg == "--explain")
            {
                options.explain_level = "detailed";
            }
            else if (arg.size() >= 7 && arg.substr(0, 7) == "--lang=")
            {
                options.output_language = arg.substr(7);
            }

            // Target
            else if (arg.size() >= 9 && arg.substr(0, 9) == "--target=")
            {
                std::string triple_str = arg.substr(9);
                auto triple_opt = TargetTriple::parse(triple_str);
                if (triple_opt)
                {
                    options.target = *triple_opt;
                }
                else
                {
                    diag.report_error("invalid target triple: " + triple_str);
                    return false;
                }
            }
            else if (arg == "--freestanding")
            {
                // ────────────────────────────────────────────────────────────────────
                // (AR) تفعيل وضع bare-metal الكامل:
                //   - يُعطِّل جميع مكتبات C/C++ القياسية
                //   - يُلزم وجود معالج ذعر مخصص
                //   - يُلزم وجود نقطة دخول مخصصة
                //   - يُضيف -nostdlib -nostartfiles -nodefaultlibs للرابط
                //
                // (EN) Enable full bare-metal mode:
                //   - Disables all C/C++ standard libraries
                //   - Requires custom panic handler
                //   - Requires custom entry point
                //   - Adds -nostdlib -nostartfiles -nodefaultlibs to linker
                // ────────────────────────────────────────────────────────────────────
                options.freestanding = true;
            }
            else if (arg == "--no-std" || arg == "--nostd")
            {
                // (AR) مرادف لـ --freestanding
                // (EN) Alias for --freestanding
                options.freestanding = true;
            }
            else if (arg == "--no-main" || arg == "--nomain")
            {
                // (AR) تعطيل نقطة الدخول الافتراضية main()
                //      يُستخدم مع --freestanding لتعريف _start المخصص
                // (EN) Disable default main() entry point
                options.no_main = true;
            }
            else if (arg == "--abort-on-panic")
            {
                // (AR) إيقاف عند الذعر بدلاً من stack unwinding
                //      مناسب لأنظمة لا تدعم C++ exceptions
                // (EN) Abort on panic instead of stack unwinding
                options.abort_on_panic = true;
            }
            else if (arg.size() >= 16 && arg.substr(0, 16) == "--linker-script=")
            {
                // (AR) مسار سكريبت الرابط (.ld) لتعريف تخطيط الذاكرة
                //      مثال: --linker-script=kernel.ld
                // (EN) Linker script path (.ld) to define memory layout
                options.linker_script = arg.substr(16);
            }
            else if (arg.size() >= 14 && arg.substr(0, 14) == "--entry-point=")
            {
                // (AR) اسم دالة نقطة الدخول المخصصة
                //      مثال: --entry-point=kernel_main
                // (EN) Custom entry point function name
                options.freestanding_entry = arg.substr(14);
            }
            else if (arg == "--allow-alloc")
            {
                // (AR) السماح بالتخصيص الديناميكي في وضع freestanding
                //      يتطلب تسجيل مُخصّص مخصص (#[معالج_تخصيص])
                // (EN) Allow dynamic allocation in freestanding (requires custom allocator)
                options.allow_freestanding_alloc = true;
            }
            else if (arg == "--module" || arg == "--وحدة")
            {
                // (AR) وضع الوحدة: يُعامل الملف كمكتبة بدون نقطة دخول
                //      يتخطى إنشاء __sad_main ودالة main wrapper
                //      مناسب لتجميع ملفات مكتبة في مشاريع متعددة الملفات
                // (EN) Module mode: treat file as library, skip __sad_main and main wrapper
                options.module_mode = true;

                // ─── ربط المكتبات ───────────────────────────────────────────────────────
                // (AR) خيارات الرابط (Linker): مسارات المكتبات وأسماؤها ونوع الربط
                // (EN) Linker options: library paths, library names, and link type
            }
            else if (arg.size() >= 2 && arg.substr(0, 2) == "-L")
            {
                options.library_paths.push_back(arg.substr(2));
            }
            else if (arg.size() >= 2 && arg.substr(0, 2) == "-l")
            {
                options.libraries.push_back(arg.substr(2));
            }
            else if (arg == "--static")
            {
                options.link_static = true;

                // ─── خيارات متنوعة أخرى ─────────────────────────────────────────────────
                // (AR) خيارات عامة: الإسهاب، التحذيرات كأخطاء، والألوان
                // (EN) General options: verbosity, warnings-as-errors, color output
            }
            else if (arg == "-v" || arg == "--verbose")
            {
                options.verbose = true;
            }
            else if (arg == "-Werror")
            {
                options.warnings_as_errors = true;
            }
            else if (arg == "--color")
            {
                options.color_diagnostics = true;
            }
            else if (arg == "--no-color")
            {
                options.color_diagnostics = false;
                colors::disable_colors();

                // ─── خيارات نظام الملكية والاستعارة ────────────────────────────────────
                // (AR) التحكم في فحص الملكية والاستعارة (Ownership & Borrow Checker)
                // (EN) Ownership & Borrow Check options
            }
            else if (arg == "--borrow-check" || arg == "--فحص-الاستعارة" || arg == "--ملكية")
            {
                options.enable_borrow_check = true;
            }
            else if (arg == "--no-borrow-check" || arg == "--بدون-فحص-استعارة")
            {
                options.enable_borrow_check = false;
            }
            else if (arg == "--debug-borrow" || arg == "--تنقيح-الاستعارة")
            {
                options.debug_borrow_check = true;
            }
            else if (arg == "--arabic-borrow" || arg == "--رسائل-عربية")
            {
                options.arabic_borrow_messages = true;
            }
            else if (arg == "--english-borrow")
            {
                options.arabic_borrow_messages = false;
            }

            // (AR) خيارات فحص الأنواع المتقدم / Type Check options
            else if (arg == "--type-check" || arg == "--فحص-الأنواع")
            {
                options.enable_type_check = true;
            }
            else if (arg == "--no-type-check" || arg == "--بدون-فحص-أنواع")
            {
                options.enable_type_check = false;
            }
            else if (arg == "--debug-types" || arg == "--تنقيح-الأنواع")
            {
                options.debug_type_check = true;
            }
            else if (arg == "--strict-types" || arg == "--أنواع-صارمة")
            {
                options.strict_type_check = true;
            }

            // ─── خيارات خط أنابيب الواجهات الرسومية / UI Pipeline options ───────
            // (AR) --ui/--واجهات: تفعيل وضع توليد الواجهات (سطح مكتب، اندرويد، iOS، ويب)
            // (EN) --ui: Enable UI code generation mode
            else if (arg == "--ui" || arg == "--واجهات")
            {
                options.emit_ui = true;
            }
            // --platform=<name> / --منصة=<name>
            else if (arg.size() >= 11 && arg.substr(0, 11) == "--platform=")
            {
                options.ui_platform = arg.substr(11);
                options.emit_ui = true;
            }
            else if (arg.size() > 2 && arg.find("--منصة=") == 0)
            {
                auto eqPos = arg.find('=');
                if (eqPos != std::string::npos)
                {
                    options.ui_platform = arg.substr(eqPos + 1);
                    options.emit_ui = true;
                }
            }
            // Shorthand platform flags
            else if (arg == "--desktop" || arg == "--سطح-المكتب")
            {
                options.emit_ui = true;
                options.ui_platform = "desktop";
            }
            else if (arg == "--android" || arg == "--اندرويد")
            {
                options.emit_ui = true;
                options.ui_platform = "android";
                // ────────────────────────────────────────────────────────────────
                // (AR) عند تفعيل وضع أندرويد، نعيّن الهدف تلقائياً إلى ARM64
                //      إذا لم يُحدَّد هدف آخر يدوياً بـ --target=
                // (EN) When Android mode is activated, auto-set target to ARM64
                //      if no other target was manually specified via --target=
                // ────────────────────────────────────────────────────────────────
                if (!options.target.is_android())
                {
                    auto android_triple = TargetTriple::parse("aarch64-linux-android" + std::to_string(24));
                    if (android_triple)
                    {
                        options.target = *android_triple;
                    }
                }
            }
            else if (arg == "--ios" || arg == "--ايفون")
            {
                options.emit_ui = true;
                options.ui_platform = "ios";
            }
            else if (arg == "--web" || arg == "--ويب")
            {
                options.emit_ui = true;
                options.ui_platform = "web";
            }

            // Unknown option
            else if (arg != "-o" && arg != "-T")
            {
                diag.report_warning("unknown option: " + arg + " / خيار غير معروف");
            }

            return true;
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
