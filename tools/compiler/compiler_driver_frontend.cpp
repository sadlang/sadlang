// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_frontend.cpp - الواجهة الرئيسية لمحرك المترجم
// CompilerDriver Main Interface (run, compile, link, parse_command_line)
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// الواجهة الرئيسية لمحرك المترجم: نقطة الدخول (run)، ترجمة الملفات،
// ربط ملفات الكائن، تحليل سطر الأوامر، والتحقق من الخيارات.
//
// الملفات المنفصلة:
//   - compiler_driver_diagnostics.cpp: الألوان + TargetTriple + التشخيصات
//   - compiler_driver_analysis.cpp: run_frontend + فحص الاستعارة/الأنواع
//   - compiler_driver_backend.cpp: run_middleend + run_backend
//   - compiler_driver_linker.cpp: invoke_linker + link_object_to_executable
//
// Description (English):
// ----------------------
// Main compiler driver interface: entry point (run), file compilation,
// object file linking, command-line parsing, and option validation.
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"

// (AR) عقد الوحدات — ImportStmt و FromImportStmt لاكتشاف التبعيات
// (EN) Module AST nodes — ImportStmt & FromImportStmt for dependency discovery
#include "../../shared/ast/include/module_nodes.h"

#ifdef _WIN32
#include <windows.h>
#undef ERROR
#undef FATAL
#undef CYAN
#undef YELLOW
#undef RED
#undef GREEN
#undef BLUE
#undef MAGENTA
#undef WHITE
#endif

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <cstdlib>

namespace sad
{
    namespace driver
    {

        // ============================================================================
        // CompilerDriver Implementation / تنفيذ CompilerDriver
        // ============================================================================

        int CompilerDriver::run(int argc, char *argv[])
        {
            // Parse command line
            if (!parse_command_line(argc, argv))
            {
                return 1;
            }

            // Validate options
            if (!validate_options())
            {
                return 1;
            }

            // Compile files
            if (!compile_files(options_.input_files))
            {
                // Print diagnostics
                diagnostics_.print_diagnostics(std::cerr, options_.color_diagnostics);

                // Print error summary
                std::cerr << colors::RED << colors::BOLD
                          << diagnostics_.get_error_count() << " error(s) generated.\n"
                          << colors::RESET;

                return 1;
            }

            // Success!
            if (options_.verbose)
            {
                std::cout << colors::GREEN << colors::BOLD
                          << "✓ Compilation successful! / الترجمة نجحت!\n"
                          << colors::RESET;

                // Print timing info
                if (options_.time_passes)
                {
                    std::cout << "\nTiming Statistics:\n";
                    std::cout << std::fixed << std::setprecision(3);
                    std::cout << "  Frontend:   " << frontend_time_ << "s\n";
                    std::cout << "  Middle-end: " << middleend_time_ << "s\n";
                    std::cout << "  Backend:    " << backend_time_ << "s\n";
                    if (link_time_ > 0.0)
                    {
                        std::cout << "  Linking:    " << link_time_ << "s\n";
                    }
                    std::cout << "  Total:      "
                              << (frontend_time_ + middleend_time_ + backend_time_ + link_time_)
                              << "s\n";
                }
            }

            return 0;
        }

        bool CompilerDriver::compile_file(const std::string &input_file)
        {
            // ════════════════════════════════════════════════════════════════════════
            // (AR) وضع توليد الواجهات — يُستخدم خط أنابيب UI بدلاً من LLVM
            // (EN) UI generation mode — uses UI pipeline instead of LLVM
            // ════════════════════════════════════════════════════════════════════════
            if (options_.emit_ui)
            {
                return run_ui_pipeline(input_file, options_.ui_platform);
            }

            if (options_.verbose)
            {
                std::cout << colors::CYAN << "Compiling " << input_file << "...\n"
                          << colors::RESET;
            }

            // Time frontend
            auto start_time = std::chrono::high_resolution_clock::now();

            // Run frontend
            if (!run_frontend(input_file))
            {
                return false;
            }

            auto end_time = std::chrono::high_resolution_clock::now();
            frontend_time_ += std::chrono::duration<double>(end_time - start_time).count();

            // Time middle-end
            start_time = std::chrono::high_resolution_clock::now();

            // Run middle-end
            if (!run_middleend())
            {
                return false;
            }

            end_time = std::chrono::high_resolution_clock::now();
            middleend_time_ += std::chrono::duration<double>(end_time - start_time).count();

            // Print IR if requested
            print_ir_if_requested();

            // Time backend
            start_time = std::chrono::high_resolution_clock::now();

            // Run backend
            if (!run_backend())
            {
                return false;
            }

            end_time = std::chrono::high_resolution_clock::now();
            backend_time_ += std::chrono::duration<double>(end_time - start_time).count();

            return true;
        }

        // ============================================================================
        // (AR) استخراج تبعيات الوحدات من AST — يمسح الجمل المستوردة ويرجع مسارات الوحدات
        // (EN) Extract module dependencies from AST — scans import statements and returns module paths
        // ============================================================================
        static std::vector<std::vector<std::string>> extractModuleDependencies(
            const Sad::AST::StmtList &ast)
        {
            std::vector<std::vector<std::string>> deps;
            for (const auto &stmt : ast)
            {
                if (!stmt)
                    continue;
                // (AR) جملة استورد — استورد وحدة
                if (auto *importStmt = dynamic_cast<Sad::AST::ImportStmt *>(stmt.get()))
                {
                    deps.push_back(importStmt->modulePath);
                }
                // (AR) جملة من...استورد — من وحدة استورد رمز
                else if (auto *fromImportStmt = dynamic_cast<Sad::AST::FromImportStmt *>(stmt.get()))
                {
                    deps.push_back(fromImportStmt->modulePath);
                }
            }
            return deps;
        }

        bool CompilerDriver::compile_files(const std::vector<std::string> &input_files)
        {
            // Check if we have input files
            if (input_files.empty())
            {
                diagnostics_.report_fatal("no input files / لا يوجد ملفات إدخال");
                return false;
            }

            // (AR) تحديد ما إذا كنا نحتاج لمرحلة ربط منفصلة
            // ============================================================
            // (AR) عند وجود عدة ملفات مصدرية وليس في وضع -c (كائن فقط)،
            // نحتاج لمرحلة ربط لجمع ملفات الكائن المُنتجة.
            // أيضاً عند وجود ملفات .o/.obj مباشرة كمدخلات.
            // ============================================================
            // (EN) When multiple source files exist and not in -c (object-only) mode,
            // we need a linking phase to combine produced object files.
            // Also when .o/.obj files are given directly as inputs.
            // ============================================================
            bool has_object_inputs = false;
            for (const auto &f : input_files)
            {
                std::string ext = get_file_extension(f);
                if (ext == ".o" || ext == ".obj")
                {
                    has_object_inputs = true;
                    break;
                }
            }
            bool need_linking = has_object_inputs ||
                                (input_files.size() > 1 &&
                                 options_.output_type != OutputType::OBJECT_FILE &&
                                 options_.output_type != OutputType::LLVM_IR &&
                                 options_.output_type != OutputType::LLVM_BC &&
                                 options_.output_type != OutputType::ASSEMBLY);

            // ====================================================================
            // (AR) مرحلة اكتشاف تبعيات الوحدات تلقائياً
            // ====================================================================
            // (AR) عند ترجمة ملف واحد يحتوي على جمل «استورد»، نحتاج لاكتشاف
            //      ملفات .ص المستوردة وإضافتها لقائمة الترجمة تلقائياً.
            //      الخوارزمية:
            //        1. تحليل معجمي+نحوي سريع لكل ملف مصدري
            //        2. استخراج ImportStmt / FromImportStmt من AST
            //        3. استخدام ModuleResolver لحل المسارات إلى ملفات فعلية
            //        4. إضافة الملفات الجديدة المكتشفة وتكرار حتى الاستقرار
            // ====================================================================
            // (EN) Module dependency auto-discovery phase
            //      When a single file contains import statements, we automatically
            //      discover the .ص files and add them to the compilation list.
            //      Algorithm: parse → extract imports → resolve to files → repeat
            //
            // (AR) ملاحظة: يُتخطى الاكتشاف التلقائي في وضع الوحدة (--module)
            //      لأن كل ملف يُترجم مستقلاً ويُعالج الاستيراد لاحقاً عبر الرابط.
            // (EN) Note: Auto-discovery is skipped in module mode (--module)
            //      because each file is compiled independently and imports are
            //      resolved later by the linker.
            // ====================================================================
            std::vector<std::string> all_source_files;
            // ====================================================================
            // (AR) لا نكتشف التبعيات تلقائياً — SIR Builder يعالج الاستيراد داخلياً
            // ====================================================================
            // SIR Builder في buildImportStmt/buildFromImportStmt يحل الوحدات المستوردة،
            // يحلل AST لها، ويدمج دوالها/متغيراتها في وحدة SIR الحالية.
            // لذلك لا نحتاج لتجميع الملفات المستوردة كوحدات مستقلة.
            // الاكتشاف التلقائي كان يسبب:
            //   1. ازدواجية الرموز (الدالة موجودة في كِلا الملفين)
            //   2. كتابة ملف الإخراج مرتين (الأخير يطغى)
            //   3. فقدان __sad_main ونقطة الدخول main
            //
            // (EN) Don't auto-discover imports — SIR Builder handles them inline
            // SIR Builder in buildImportStmt/buildFromImportStmt resolves imported modules,
            // parses their AST, and merges their functions/variables into the current SIR module.
            // So we don't need to compile imported files as separate compilation units.
            // Auto-discovery was causing:
            //   1. Duplicate symbols (function exists in both files)
            //   2. Output file overwritten twice (last wins)
            //   3. Loss of __sad_main and main entry point
            // ====================================================================
            {
                for (const auto &f : input_files)
                {
                    std::string ext = get_file_extension(f);
                    if (ext == ".\xd8\xb5" || ext == ".sad")
                    {
                        all_source_files.push_back(f);
                    }
                }
            }

            std::vector<std::string> object_files;

            // (AR) بناء القائمة النهائية: ملفات المصدر المكتشفة + ملفات الكائن الأصلية
            // (EN) Build final list: discovered source files + original object files
            std::vector<std::string> final_files = all_source_files;
            for (const auto &f : input_files)
            {
                std::string ext = get_file_extension(f);
                if (ext == ".o" || ext == ".obj")
                {
                    final_files.push_back(f);
                }
            }

            // Compile each file
            for (const auto &input_file : final_files)
            {
                // Check if file exists
                if (!file_exists(input_file))
                {
                    diagnostics_.report_error("file not found: " + input_file +
                                              " / الملف غير موجود");
                    return false;
                }

                // Get file extension
                std::string ext = get_file_extension(input_file);

                if (ext == ".\xd8\xb5" || ext == ".sad")
                {
                    // Source file - compile it (supports .ص and .sad)

                    // (AR) عند الحاجة للربط، نجمع كل ملف مصدري إلى ملف كائن (.o) أولاً
                    // (EN) When linking is needed, compile each source to object file first
                    OutputType saved_output_type = options_.output_type;
                    std::string saved_output_file = options_.output_file;
                    std::string obj_file;

                    if (need_linking)
                    {
                        options_.output_type = OutputType::OBJECT_FILE;
                        obj_file = change_extension(input_file, ".o");
                        // (AR) إذا كان مسار ملف الكائن يحتوي أحرف غير ASCII، نستخدم مسار مؤقت
                        //      لتجنب خلل LLVM مع المسارات العربية على Windows
                        // (EN) If object file path has non-ASCII chars, use temp path
                        //      to avoid LLVM crash with Arabic paths on Windows
#ifdef _WIN32
                        {
                            bool hasUnicode = false;
                            for (unsigned char c : obj_file)
                            {
                                if (c > 127)
                                {
                                    hasUnicode = true;
                                    break;
                                }
                            }
                            if (hasUnicode)
                            {
                                auto tmpPath = get_temp_file(".o");
                                temp_files_.push_back(tmpPath);
                                obj_file = tmpPath.string();
                            }
                        }
#endif
                        options_.output_file = obj_file;
                    }

                    if (!compile_file(input_file))
                    {
                        if (need_linking)
                        {
                            options_.output_type = saved_output_type;
                            options_.output_file = saved_output_file;
                        }
                        return false;
                    }

                    if (need_linking)
                    {
                        options_.output_type = saved_output_type;
                        options_.output_file = saved_output_file;
                        object_files.push_back(obj_file);
                        temp_files_.push_back(obj_file);
                    }
                }
                else if (ext == ".o" || ext == ".obj")
                {
                    // Object file - add to linking list
                    if (need_linking)
                    {
                        object_files.push_back(input_file);
                    }
                    else
                    {
                        diagnostics_.report_error("cannot process object file without linking: " +
                                                  input_file);
                        return false;
                    }
                }
                else
                {
                    diagnostics_.report_error("unknown file type: " + input_file +
                                              " / نوع ملف غير معروف");
                    return false;
                }
            }

            // Link if needed
            if (need_linking && !object_files.empty())
            {
                auto start_time = std::chrono::high_resolution_clock::now();

                if (!link_files(object_files, options_.output_file))
                {
                    return false;
                }

                auto end_time = std::chrono::high_resolution_clock::now();
                link_time_ = std::chrono::duration<double>(end_time - start_time).count();
            }

            // Cleanup temporary files
            cleanup_temp_files();

            return true;
        }

        bool CompilerDriver::link_files(const std::vector<std::string> &object_files,
                                        const std::string &output_file)
        {
            if (options_.verbose)
            {
                std::cout << colors::CYAN << "Linking " << output_file << "...\n"
                          << colors::RESET;
            }

            // Invoke system linker
            return invoke_linker(object_files, output_file);
        }

        bool CompilerDriver::parse_command_line(int argc, char *argv[])
        {
            CommandLineParser parser(argc, argv);

            if (!parser.parse(options_, diagnostics_))
            {
                return false;
            }

            // Check if help/version requested
            if (parser.is_help_requested())
            {
                print_help(std::cout);
                std::exit(0);
            }

            if (parser.is_version_requested())
            {
                print_version(std::cout);
                std::exit(0);
            }

            return true;
        }

        bool CompilerDriver::validate_options()
        {
            // ════════════════════════════════════════════════════════════════════════
            // (AR) التحقق من خيارات المترجم قبل بدء الترجمة
            // ════════════════════════════════════════════════════════════════════════
            // هذه الدالة تُجري سلسلة من التحقق على خيارات المترجم لضمان:
            //   1. وجود ملفات إدخال صالحة
            //   2. إعداد ملف الإخراج الافتراضي إذا لم يُحدَّد
            //   3. ضبط خيارات وضع Freestanding تلقائياً إذا كانت مُفعَّلة
            //   4. التعامل مع خصوصيات نظام ويندوز (مثل دعم الألوان)
            // ════════════════════════════════════════════════════════════════════════
            // (EN) Validate compiler options before starting compilation
            // ════════════════════════════════════════════════════════════════════════

            // Check if we have input files
            if (options_.input_files.empty())
            {
                diagnostics_.report_fatal("no input files / لا يوجد ملفات إدخال");
                return false;
            }

            // Set default output file if not specified
            if (options_.output_file.empty())
            {
                options_.output_file = get_default_output_name(
                    options_.input_files[0],
                    options_.output_type);
            }

            // ════════════════════════════════════════════════════════════════════════
            // (AR) ضبط خيارات وضع Freestanding التلقائية
            // ════════════════════════════════════════════════════════════════════════
            // وضع Freestanding يعني البرمجة بدون مكتبة قياسية — يُستخدم في:
            //   - تطوير أنوية أنظمة التشغيل (kernel)
            //   - البرمجة المدمجة (embedded systems)
            //   - البرامج ذاتية الاكتفاء (self-contained executables)
            //   - بيئات بدء التشغيل (boot loaders)
            //   - البرمجة المعدنية (bare-metal programming)
            //
            // عندما يُفعَّل هذا الوضع (سواء عبر --freestanding أو #![بلا_مكتبة_قياسية]):
            //   1. no_main = true   → لا يوجد دالة main() القياسية
            //   2. abort_on_panic   → لا استثناءات C++ في بيئة بدون مكتبة
            //   3. freestanding_entry = "_start" → نقطة الدخول الافتراضية للنواة
            //
            // ملاحظة: يمكن تجاوز اسم نقطة الدخول بـ --entry=kernel_main
            // ════════════════════════════════════════════════════════════════════════
            // (EN) Configure freestanding mode defaults
            //
            // Freestanding mode = programming without standard library, used in:
            //   - OS kernel development
            //   - Embedded systems
            //   - Self-contained executables
            //   - Boot loaders
            //   - Bare-metal programming
            //
            // When activated (via --freestanding or #![بلا_مكتبة_قياسية]):
            //   1. no_main = true   → no standard main() entry point
            //   2. abort_on_panic   → no C++ exceptions without runtime
            //   3. freestanding_entry = "_start" → default kernel entry point
            // ════════════════════════════════════════════════════════════════════════
            if (options_.freestanding)
            {
                // (AR) تفعيل no_main تلقائياً — في بيئة freestanding لا مكتبة C
                //      لذا لا توجد دالة __start المسؤولة عن استدعاء main()
                // (EN) Auto-enable no_main — freestanding has no C lib startup
                if (!options_.no_main)
                {
                    options_.no_main = true;
                    if (options_.verbose)
                    {
                        std::cout << u8"  \u2699 [freestanding] no_main \u0645\u0641\u0639\u0651\u0644 \u062a\u0644\u0642\u0627\u0626\u064a\u0627\u064b\n";
                    }
                }

                // (AR) تعيين نقطة الدخول الافتراضية _start إذا لم تُحدَّد
                //      _start هي نقطة الدخول القياسية لأنوية لينكس/نظم التشغيل
                //      يمكن تغييرها لـ kernel_main أو أي اسم آخر بـ --entry=...
                // (EN) Set default entry point _start if not specified
                //      _start is the standard entry for Linux kernels/OS
                if (options_.freestanding_entry.empty())
                {
                    options_.freestanding_entry = "_start";
                    if (options_.verbose)
                    {
                        std::cout << u8"  \u2699 [freestanding] \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: _start\n";
                    }
                }

                // (AR) تفعيل abort_on_panic — لا استثناءات C++ في بيئة freestanding
                //      الذعر (panic) يجب أن يوقف النواة مباشرة بدلاً من رمي استثناء
                // (EN) Enable abort_on_panic — no C++ exceptions in freestanding
                //      Panic must halt the kernel directly, not throw an exception
                if (!options_.abort_on_panic)
                {
                    options_.abort_on_panic = true;
                    if (options_.verbose)
                    {
                        std::cout << u8"  \u2699 [freestanding] abort_on_panic \u0645\u0641\u0639\u0651\u0644 \u062a\u0644\u0642\u0627\u0626\u064a\u0627\u064b\n";
                    }
                }

                // (AR) طباعة ملخص وضع freestanding في الوضع المفصل
                // (EN) Print freestanding mode summary in verbose mode
                if (options_.verbose)
                {
                    std::cout << u8"\n  \u2550\u2550 \u0648\u0636\u0639 Freestanding \u0645\u064f\u0641\u0639\u064e\u0651\u0644 \u2550\u2550\n";
                    std::cout << u8"     \u2022 \u0628\u062f\u0648\u0646 \u0645\u0643\u062a\u0628\u0629 \u0642\u064a\u0627\u0633\u064a\u0629 (no stdlib)\n";
                    std::cout << u8"     \u2022 \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: " << options_.freestanding_entry << "\n";
                    std::cout << u8"     \u2022 abort_on_panic: \u0645\u0641\u0639\u0651\u0644\n";
                    std::cout << u8"     \u2022 \u0644\u0627 \u062f\u0627\u0644\u0629 main() \u0642\u064a\u0627\u0633\u064a\u0629\n\n";
                }
            }

// Disable colors on Windows if not supported
#ifdef _WIN32
            // Check if ANSI colors are supported
            // For now, disable colors on Windows by default
            if (options_.color_diagnostics)
            {
                // You can add Windows console color support here
                // For now, disable:
                options_.color_diagnostics = false;
                colors::disable_colors();
            }
#endif

            return true;
        }

    } // namespace driver
} // namespace sad
