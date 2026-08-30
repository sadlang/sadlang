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
#include "cli_flags_generated.h"
// (AR) لتصنيف الهدف بنفس دلالات الخلفيّة (معدن عارٍ مقابل نظام تشغيل).
//      الترويسةُ بلا LLVM عمدًا: موضعُ القرارِ واحدٌ يشترك فيه السائقُ ومولّدُ
//      LLVM معًا، فلا يجرُّ السائقُ LLVM لأجلِ تشخيصٍ — وهو شرطُ بناءِ المترجمِ
//      النحيلِ `sad-build-native` بلا LLVM البتّة.
// (EN) To classify the target with the backend's own semantics. The header is
//      deliberately LLVM-free: one decision site shared by the driver and the
//      LLVM generator, so the driver does not drag LLVM in for a diagnostic —
//      the precondition for building the thin, LLVM-free `sad-build-native`.
#include "hw_bridge_profile.h"
#include "error_manager.h"
#include "explanation_level.h"
// (AR) محلل أعلام سياسة الذاكرة (--gc/--learn/--prod) لتوحيد سلوك الأخطاء
// (EN) Memory policy flag parser to unify error behavior with interpreter
#include "memory/policy/memory_mode_flag.h"
// (AR) عقد الوحدات — ImportStmt و FromImportStmt لاكتشاف التبعيات
// (EN) Module AST nodes — ImportStmt & FromImportStmt for dependency discovery
#include "module_nodes.h"

// (AR) دعم استخراج التوثيق المدمج (--docs / --وثّق / --docs-out=)
// (EN) Built-in documentation extraction support
#include "lexer_core.h"
#include "parser_core.h"
#include "docs_extractor.h"
#include "pdf_exporter.h"

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
#include <filesystem>
#include <system_error>
// (AR) 🔑 ضمٌّ صريحٌ لكلِّ مورِّد. حذفُ ترويسةٍ «ميّتةٍ» بمقياسِ الاستعمالِ
//      المباشرِ يحذفُ معها ما كانت تُورِّدُه نقلًا؛ فيُصرَّحُ بالمورِّدِ لا يُورَّث.
// (EN) Explicit include for every supplier: removing a header that is "dead" by
//      direct-use count also removes what it transitively supplied.
#include <cstdint>
#include <utility>
#include <string>
#include <vector>

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

            // (AR) وضع AST-JSON: أبقِ قناة stdout نظيفة للـJSON — اكبح verbose.
            // (EN) AST-JSON mode: keep stdout clean for JSON — suppress verbose chatter.
            if (options_.emit_ast_json)
            {
                options_.verbose = false;
            }

            // ════════════════════════════════════════════════════════════════════════
            // (AR) Phase E-3 — تطبيق إعدادات شرح الخطأ ولغة الإخراج على ErrorManager
            // (EN) Phase E-3 — apply explanation level + output language to EM
            // ════════════════════════════════════════════════════════════════════════
            {
                auto &em = Sad::Errors::ErrorManager::getInstance();
                em.initializeDefaults();
                if (!options_.explain_level.empty())
                {
                    em.setExplanationLevel(Sad::Errors::parseExplanationLevel(options_.explain_level));
                }
                if (!options_.output_language.empty())
                {
                    em.setLanguage(Sad::Errors::parseLanguage(options_.output_language));
                }
            }

            // ════════════════════════════════════════════════════════════════════════
            // (AR) وضع توثيق المشروع متعدد الملفات — لا يحتاج input_files صريحة
            // (EN) Multi-file project docs mode — no explicit input_files required
            // ════════════════════════════════════════════════════════════════════════
            if (options_.emit_docs && !options_.docs_project_dir.empty())
            {
                if (!emit_project_docs())
                {
                    diagnostics_.print_diagnostics(std::cerr, options_.color_diagnostics);
                    return 1;
                }
                return 0;
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
            // (AR) Phase E-3: طباعة التحذيرات حتى في حال النجاح (مثل --learn).
            //      لو لم نطبعها هنا، فإن وضع --learn يُترجم بصمت تام بلا أي
            //      ملاحظة تعليمية رغم وجود انتهاكات ملكية تستوجب التنبيه.
            // (EN) Phase E-3: Print warnings even on success (e.g. --learn mode).
            //      Otherwise --learn would compile silently despite ownership
            //      violations that we explicitly want to surface to the user.
            if (diagnostics_.has_diagnostics())
            {
                diagnostics_.print_diagnostics(std::cerr, options_.color_diagnostics);
            }

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

        // ════════════════════════════════════════════════════════════════════════
        // (AR) emit_project_docs — توثيق Markdown لمشروع متعدد الملفات
        // (EN) emit_project_docs — multi-file project Markdown documentation
        // ════════════════════════════════════════════════════════════════════════
        bool CompilerDriver::emit_project_docs()
        {
            using DocsExtractor = Sad::AST::DocsExtractor;

            const std::string &root = options_.docs_project_dir;

            // (AR) جمع جميع ملفات لغة ص تحت المجلد الجذر مع مراعاة الاستبعادات
            // (EN) Collect all Sad source files honoring exclude patterns
            auto file_paths = options_.docs_excludes.empty()
                                  ? DocsExtractor::collectSadFiles(root)
                                  : DocsExtractor::collectSadFiles(root, options_.docs_excludes);
            if (file_paths.empty())
            {
                diagnostics_.report_fatal(
                    "no .ص or .sad files found under: " + root +
                    " / لم يُعثر على ملفات لغة ص في المسار");
                return false;
            }

            if (options_.verbose)
            {
                std::cout << colors::CYAN
                          << "Documenting project: " << root
                          << " (" << file_paths.size() << " files)\n"
                          << colors::RESET;
            }

            // (AR) تحليل كل ملف وحفظ AST لكل واحد
            // (EN) Parse each file and retain its AST
            //
            // ملاحظة CW-29: نُمرّر مؤشرات إلى StmtList لتجنّب النسخ العميق
            // Note CW-29: pass pointers to StmtList to avoid deep copies
            std::vector<Sad::AST::StmtList> programs;
            programs.reserve(file_paths.size());
            std::vector<DocsExtractor::FileEntry> entries;
            entries.reserve(file_paths.size());

            size_t failures = 0;
            for (const auto &path : file_paths)
            {
                auto src = read_file(path);
                if (!src)
                {
                    std::cerr << "  warning: cannot read " << path << "\n";
                    ++failures;
                    continue;
                }

                Sad::Lexer::LexerCore lexer(*src);
                Sad::Parser::ParserCore parser(lexer);
                auto prog = parser.parseProgram();

                if (parser.hasErrors())
                {
                    std::cerr << "  warning: parse errors in " << path << "\n";
                    for (const auto &err : parser.getErrors())
                        std::cerr << "    " << err << "\n";
                    ++failures;
                    continue;
                }

                programs.push_back(std::move(prog));
                entries.push_back({path, &programs.back()});
            }

            if (entries.empty())
            {
                diagnostics_.report_fatal(
                    "no parseable Sad files in project / لا توجد ملفات قابلة للتحليل");
                return false;
            }

            // (AR) اسم المشروع: من الخيار، وإلا اسم المجلد الجذر
            // (EN) Project name: from option, else root directory name
            std::string project_name = options_.docs_project_name;
            if (project_name.empty())
            {
                std::error_code ec;
                std::filesystem::path p(root);
                auto canon = std::filesystem::weakly_canonical(p, ec);
                if (!ec)
                    project_name = canon.filename().u8string();
                if (project_name.empty())
                    project_name = p.filename().u8string();
                if (project_name.empty())
                    project_name = root;
            }

            std::string md = DocsExtractor::extractProjectMarkdown(
                project_name, entries);

            // (AR) دعم تصدير المشروع إلى PDF عبر markdownToPrintableHtml + PdfExporter
            // (EN) Project PDF export via markdownToPrintableHtml + PdfExporter
            bool isProjPdf = (options_.docs_format == "pdf");
            if (isProjPdf && options_.docs_output_path.empty())
            {
                diagnostics_.report_fatal(
                    std::string("PDF export requires / تصدير PDF يتطلب ") +
                    canonical_flag(static_cast<std::uint16_t>(::sad::cli::FlagAction::DocsOut)) + "=<file.pdf>");
                return false;
            }

            if (isProjPdf)
            {
                std::string mer = DocsExtractor::buildProjectClassDiagram(entries);
                std::string html = DocsExtractor::markdownToPrintableHtml(md, project_name, mer);
                std::string tmpHtml = options_.docs_output_path + ".tmp.html";
                {
                    std::ofstream tf(tmpHtml, std::ios::binary | std::ios::trunc);
                    if (!tf)
                    {
                        diagnostics_.report_fatal("cannot write temp HTML / تعذّرت كتابة HTML المؤقت");
                        return false;
                    }
                    tf << html;
                }
                std::string err;
                bool ok = Sad::AST::PdfExporter::convert(tmpHtml, options_.docs_output_path, err);
                std::error_code ec_rm;
                std::filesystem::remove(tmpHtml, ec_rm);
                if (!ok)
                {
                    diagnostics_.report_fatal("PDF export failed: " + err);
                    return false;
                }
                std::cout << u8"✓ Project PDF saved to: "
                          << options_.docs_output_path
                          << " (" << entries.size() << " files)\n";
                return true;
            }

            if (options_.docs_output_path.empty())
            {
                std::cout << md;
            }
            else
            {
                std::ofstream ofs(options_.docs_output_path,
                                  std::ios::binary | std::ios::trunc);
                if (!ofs)
                {
                    diagnostics_.report_fatal(
                        "cannot write docs to: " + options_.docs_output_path +
                        " / تعذّرت كتابة التوثيق");
                    return false;
                }
                ofs << md;
                ofs.close();
                std::cout << u8"✓ Project docs saved to: "
                          << options_.docs_output_path
                          << " (" << entries.size() << " files";
                if (failures)
                    std::cout << ", " << failures << " skipped";
                std::cout << ")\n";
            }

            return true;
        }

        bool CompilerDriver::compile_file(const std::string &input_file)
        {
            // ════════════════════════════════════════════════════════════════════════
            // (AR) وضع استخراج التوثيق — يُشغّل Lexer + Parser فقط ثم يُنتج Markdown
            //      ويتجاوز جميع مراحل middle-end / backend (LLVM، الربط، إلخ).
            //      وفق CW-01 (SRP): مهمة منفصلة عن التحويل إلى ملف تنفيذي.
            // (EN) Documentation extraction mode — runs Lexer + Parser only,
            //      emits Markdown, and skips all middle-end / backend stages.
            //      Per CW-01 (SRP): a distinct concern from native compilation.
            // ════════════════════════════════════════════════════════════════════════
            if (options_.emit_docs)
            {
                auto source_opt = read_file(input_file);
                if (!source_opt)
                {
                    diagnostics_.report_fatal("failed to read file: " + input_file +
                                              " / فشل قراءة الملف");
                    return false;
                }

                Sad::Lexer::LexerCore lexer(*source_opt);
                Sad::Parser::ParserCore parser(lexer);
                auto program = parser.parseProgram();

                if (parser.hasErrors())
                {
                    for (const auto &err : parser.getErrors())
                    {
                        std::cerr << err << "\n";
                    }
                    return false;
                }

                std::string md;
                bool isPdf = (options_.docs_format == "pdf");
                if (options_.docs_format == "json")
                    md = Sad::AST::DocsExtractor::extractJson(program, input_file);
                else if (options_.docs_format == "html")
                    md = Sad::AST::DocsExtractor::extractHtml(program, input_file);
                else if (isPdf)
                    md = Sad::AST::DocsExtractor::extractPrintableHtml(program, input_file);
                else
                    md = Sad::AST::DocsExtractor::extractMarkdown(
                        program, input_file);

                if (options_.docs_output_path.empty())
                {
                    if (isPdf)
                    {
                        diagnostics_.report_fatal(
                            std::string(canonical_flag(
                                static_cast<std::uint16_t>(::sad::cli::FlagAction::DocsFormat))) +
                            "=pdf requires / يتطلب " +
                            canonical_flag(
                                static_cast<std::uint16_t>(::sad::cli::FlagAction::DocsOut)));
                        return false;
                    }
                    std::cout << md;
                }
                else
                {
                    // (AR) لـ PDF: نكتب HTML مؤقت ثم نحوّله
                    std::string htmlPath = isPdf
                                               ? (options_.docs_output_path + ".tmp.html")
                                               : options_.docs_output_path;
                    std::ofstream ofs(htmlPath,
                                      std::ios::binary | std::ios::trunc);
                    if (!ofs)
                    {
                        diagnostics_.report_fatal(
                            "cannot write docs to: " + htmlPath +
                            " / تعذّرت كتابة التوثيق");
                        return false;
                    }
                    ofs << md;
                    ofs.close();
                    if (isPdf)
                    {
                        std::string err;
                        bool ok = Sad::AST::PdfExporter::convert(
                            htmlPath, options_.docs_output_path, err);
                        std::error_code ec;
                        std::filesystem::remove(htmlPath, ec);
                        if (!ok)
                        {
                            diagnostics_.report_fatal(
                                "PDF export failed: " + err);
                            return false;
                        }
                        std::cout << u8"✓ PDF saved to: "
                                  << options_.docs_output_path << "\n";
                    }
                    else
                    {
                        std::cout << u8"✓ Docs saved to: "
                                  << options_.docs_output_path << "\n";
                    }
                }

                return true;
            }

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

            // (AR) وضع إخراج AST بصيغة JSON — يتوقّف بعد الواجهة الأماميّة (لا توليد/ربط).
            // (EN) AST-JSON mode — halt after frontend (no codegen/link).
            if (options_.emit_ast_json)
            {
                return true;
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

            // ============================================================
            // (AR) رفض صريح: صيغ الإخراج أحاديّة الوحدة (--emit-llvm/--emit-bc/-S)
            //      تكتب كلّ ملفّ فوق نفس المخرج فتطمس سابقه بلا تشخيص.
            //      كان هذا «نجاحًا كاذبًا» (خروج 0 مع فقدان N-1 وحدة).
            //      الحلّ لمشروع متعدّد الملفّات: --module لكلّ ملفّ ثمّ اربط،
            //      أو ادمج المصادر في ملفّ واحد قبل --emit-llvm.
            // (EN) Explicit rejection: single-module output formats silently
            //      overwrote the same output per file (N-1 modules lost, exit 0).
            // ============================================================
            {
                size_t source_file_count = 0;
                for (const auto &f : input_files)
                {
                    std::string ext = get_file_extension(f);
                    if (ext != ".o" && ext != ".obj")
                        source_file_count++;
                }
                bool single_module_format =
                    options_.output_type == OutputType::LLVM_IR ||
                    options_.output_type == OutputType::LLVM_BC ||
                    options_.output_type == OutputType::ASSEMBLY;
                if (source_file_count > 1 && single_module_format && !options_.module_mode)
                {
                    diagnostics_.report_error(
                        std::string("صيغة الإخراج أحاديّة الوحدة (") +
                        canonical_flag(static_cast<std::uint16_t>(::sad::cli::FlagAction::EmitLlvm)) + "/" +
                        canonical_flag(static_cast<std::uint16_t>(::sad::cli::FlagAction::EmitBc)) + "/" +
                        ::sad::cli::short_flags::EmitAsm +
                        ") تقبل ملفًّا مصدريًّا واحدًا فقط؛ مُرِّر " +
                        std::to_string(source_file_count) + " ملفّات. استخدم " +
                        canonical_flag(static_cast<std::uint16_t>(::sad::cli::FlagAction::ModuleMode)) +
                        " لترجمة كلّ ملفّ مستقلًّا ثمّ اربط، أو ادمج المصادر في ملفّ واحد. "
                        "(single-module output format accepts only one source file)");
                    return false;
                }
            }

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

            // (AR) وضع AST-JSON: توقّف بعد إخراج شجرة كلّ ملفّ — لا ربط (يمنع فشل الرابط
            //      على «كائنات» غير مولَّدة في حالة تعدّد الملفّات).
            // (EN) AST-JSON mode: halt after emitting each file's tree — no linking.
            if (options_.emit_ast_json)
            {
                cleanup_temp_files();
                return true;
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
            // ═══════════════════════════════════════════════════════════════════════
            // (AR) المرحلة E-3: تحليل أعلام سياسة الذاكرة (--gc/--learn/--prod) أولاً
            //      حتى يستهلكها المحلل ولا تظهر كـ "أعلام غير معروفة" لـ CommandLineParser.
            //      السياسة المُحلَّلة تُمرَّر إلى Sad::Errors::dispatch() عند كل خطأ ملكية
            //      لتوحيد السلوك بين المُفسِّر (sad) والمترجم (sadc).
            // (EN) Phase E-3: Parse memory policy flags first so they are consumed
            //      before the regular CLI parser. The parsed policy is later passed
            //      to Sad::Errors::dispatch() for every ownership error, unifying
            //      behavior between the interpreter (sad) and compiler (sadc).
            // ═══════════════════════════════════════════════════════════════════════
            std::vector<std::string> mem_remaining_storage; // (AR) تبقى حية طوال الدالة
            std::vector<char *> rebuilt_argv_storage;
            {
                ::Sad::Memory::MemoryModeFlag memFlagParser;
                auto memResult = memFlagParser.parse(argc, argv);
                if (!memResult.success)
                {
                    for (const auto &err : memResult.errors)
                        diagnostics_.report_error(
                            "memory policy / سياسة الذاكرة: " + err);
                    // (AR) Phase E-3: اطبع التشخيصات قبل الخروج لكي يرى المستخدم
                    //      رسالة الرفض الواضحة (مثلاً: '--dev' أُزيل، استخدم '--gc').
                    // (EN) Phase E-3: print diagnostics before exit so user sees
                    //      the clear rejection message (e.g. '--dev' removed, use '--gc').
                    diagnostics_.print_diagnostics(std::cerr, options_.color_diagnostics);
                    return false;
                }
                for (const auto &warn : memResult.warnings)
                    diagnostics_.report_warning(
                        "memory policy / سياسة الذاكرة: " + warn);

                // ═══════════════════════════════════════════════════════════════════
                // (AR) إصلاح جذريّ: العَلَم --freestanding/--no-std/--kernel مزدوج الغرض
                //      — يضبط سياسة الذاكرة (بلا GC) ويجب أيضًا أن يفعّل وضع الترجمة
                //      الحرّة في codegen. لكنّ معالج سياسة الذاكرة أعلاه يبتلع العَلَم
                //      قبل المحلّل الرئيسيّ (CommandLineParser) فلا يصل
                //      options_.freestanding أبدًا. نُعيد ربطه هنا من إعدادات السياسة.
                //      (كان هذا يُبقي الوضع الحرّ معطّلًا صامتًا رغم تمرير العَلَم.)
                // (EN) Root fix: --freestanding is dual-purpose (memory policy AND
                //      codegen freestanding). The memory pre-parser consumes it before
                //      the main CLI parser, so options_.freestanding never got set.
                //      Re-propagate it here from the parsed policy.
                // ═══════════════════════════════════════════════════════════════════
                if (memResult.noStdRequested)
                {
                    options_.freestanding = true;
                }

                // (AR) parse() يتجاوز argv[0]، إذا اختلف العدد فقد استُهلك علم.
                // (EN) parse() skips argv[0]; if size differs, at least one consumed.
                const size_t originalUserArgs =
                    static_cast<size_t>(argc > 0 ? argc - 1 : 0);
                if (memResult.remainingArgs.size() != originalUserArgs)
                {
                    options_.memory_policy = memResult.settings;
                    options_.memory_policy_set = true;

                    // (AR) أعد بناء argc/argv: argv[0] + الأعلام المتبقية فقط.
                    // (EN) Rebuild argc/argv: argv[0] + remaining flags only.
                    mem_remaining_storage = std::move(memResult.remainingArgs);
                    rebuilt_argv_storage.reserve(mem_remaining_storage.size() + 1);
                    rebuilt_argv_storage.push_back(argv[0]);
                    for (auto &s : mem_remaining_storage)
                        rebuilt_argv_storage.push_back(s.data());
                    argc = static_cast<int>(rebuilt_argv_storage.size());
                    argv = rebuilt_argv_storage.data();
                }
            }

            CommandLineParser parser(argc, argv);

            if (!parser.parse(options_, diagnostics_))
            {
                // (AR) اطبع سبب الفشل: تشخيصات المحلِّل تُراكَم ولا تُطبع تلقائيًّا،
                //      فكان فشل سطر الأوامر يخرج بـ1 صامتًا بلا رسالة.
                // (EN) Print why: CLI diagnostics accumulate and are not auto-printed,
                //      so a bad flag used to exit(1) silently with no message.
                diagnostics_.print_diagnostics(std::cerr, options_.color_diagnostics);
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
                // (AR) وضع توثيق المشروع لا يحتاج ملفات إدخال صريحة
                // (EN) Project docs mode does not require explicit input files
                if (options_.emit_docs && !options_.docs_project_dir.empty())
                    return true;
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
                // ════════════════════════════════════════════════════════════════
                // (AR) تشخيص الهدف المُستنتَج: راية الوضع الحرّ تصف **غياب المكتبة
                //      القياسيّة**، لا حلقة الامتياز. حين لا يُمرَّر «--هدف» يرث
                //      المترجم ثالوث المضيف. نصنّفه بنفس دالّة الخلفيّة تمامًا
                //      (classifyHwBridgeProfile) كي لا يناقض التشخيصُ التوليد، ثمّ:
                //        • HostedLibc (ويندوز/ماك): لا جسور ولا نداءات ⇒ **تحذير**:
                //          الناتج لا يصلح للمعدن ولا لـ-nostdlib.
                //        • LinuxSyscall (مضيف لينكس، الحالة الغالبة في CI): تُبثّ
                //          نداءات النظام ويُربط -nostdlib فعلًا ⇒ **ملاحظة** ألطف:
                //          افتُرض نطاق مستخدم؛ للنواة مرّر --هدف. (ليست تحذيرًا كي
                //          لا تنقلب خطأً تحت -Werror فتكسر بناء المضيف الحرّ.)
                //      لا شيء إن كان الهدف صريحًا أو معدنًا مُستنتَجًا (لا التباس).
                // (EN) Inferred-target diagnostic: the freestanding flag describes
                //      the *absence of a standard library*, not the privilege ring.
                //      With no «--هدف» the compiler inherits the host triple. We
                //      classify it with the backend's own function so the diagnostic
                //      cannot contradict codegen: HostedLibc (Windows/macOS) ⇒ a
                //      *warning* (neither bare-metal nor -nostdlib capable);
                //      LinuxSyscall (a Linux host, the common CI case) ⇒ a milder
                //      *note* (syscalls are emitted, -nostdlib links — for a kernel
                //      pass --هدف). A note, not a warning, so -Werror does not turn
                //      it into an error that breaks a valid hosted freestanding build.
                // ════════════════════════════════════════════════════════════════
                const ::sad::target::HwBridgeProfile profile =
                    ::sad::target::classifyHwBridgeProfileFromTripleText(
                        options_.target.to_string());
                if (!options_.target_explicit)
                {
                    if (profile == ::sad::target::HwBridgeProfile::HostedLibc)
                    {
                        diagnostics_.report_warning(
                            std::string(::sad::cli::messages::FreestandingHostTargetHosted) +
                            options_.target.to_string());
                    }
                    else if (profile == ::sad::target::HwBridgeProfile::LinuxSyscall)
                    {
                        diagnostics_.report_note(
                            std::string(::sad::cli::messages::FreestandingHostTargetLinux) +
                            options_.target.to_string());
                    }
                }

                // (AR) معدن عارٍ بمعمارية بلا جسر (aarch64/riscv64 وأمثالها): جسرا
                //      الإخراج والوقت كعبان صامتان. قبل هذه الحملة كان يُبثّ تجميع
                //      x86 لا يُترجَم — فشلٌ صاخب. الآن يرتبط بصمت، فنُصرّح بالحاجة
                //      إلى تعريف BSP بدل ترك المطوّر أمام نواة صامتة بلا خيط. يُصرَّح
                //      ولو كان الهدف صريحًا (المطوّر اختار المعمارية عن قصد).
                // (EN) Bare metal with no bridge (aarch64/riscv64 and the like): the
                //      output and time bridges are silent stubs. This campaign
                //      replaced non-building x86 asm (a loud failure) with a silent
                //      link, so state the BSP requirement rather than leave a silent
                //      kernel with no thread to pull. Emitted even for an explicit
                //      target — the developer chose the architecture deliberately.
                if (profile == ::sad::target::HwBridgeProfile::BareMetalStub)
                {
                    diagnostics_.report_note(
                        std::string(::sad::cli::messages::FreestandingBareMetalStubBridges) +
                        options_.target.to_string());
                }

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
                    std::cout << u8"     \u2022 \u0644\u0627 \u062f\u0627\u0644\u0629 main() \u0642\u064a\u0627\u0633\u064a\u0629\n";
                    // (AR) \u0627\u0644\u0643\u0648\u0645\u0629 \u0627\u0644\u0633\u0627\u0643\u0646\u0629: \u0627\u0644\u062d\u0642\u064a\u0642\u0629\u064f \u0627\u0644\u0648\u062d\u064a\u062f\u0629\u064f \u0641\u064a \u0647\u0630\u0647 \u0627\u0644\u0644\u0648\u062d\u0629 \u0627\u0644\u062a\u064a
                    //      **\u062a\u064f\u0633\u0642\u0637 \u0627\u0644\u0625\u0642\u0644\u0627\u0639** \u0625\u0646 \u0623\u062e\u0637\u0623\u062a. \u0627\u0641\u062a\u0631\u0627\u0636\u064f\u0647\u0627 \u0645\u0634\u0631\u0648\u0637\u064c \u0628\u0627\u0644\u0647\u062f\u0641\u060c
                    //      \u0641\u064a\u062a\u0642\u0644\u0651\u0628 128 \u0636\u0639\u0641\u064b\u0627 \u0639\u0644\u0649 \u062d\u0631\u0641\u064d \u0641\u064a \u0627\u0644\u062b\u0627\u0644\u0648\u062b \u2014 \u0648\u0628\u0644\u0627 \u0647\u0630\u0627 \u0627\u0644\u0633\u0637\u0631
                    //      \u064a\u062a\u0642\u0644\u0651\u0628 \u0635\u0627\u0645\u062a\u064b\u0627.
                    //      \u26a0\ufe0f \u0644\u0627 \u064a\u064f\u0635\u0646\u064e\u0651\u0641 \u0627\u0644\u0647\u062f\u0641\u064f \u0647\u0646\u0627: \u0627\u0644\u062a\u0635\u0646\u064a\u0641\u064f \u064a\u0639\u062a\u0645\u062f \u062a\u0637\u0628\u064a\u0639\u064e
                    //      \u200ellvm::Triple\u200e \u0641\u064a \u0627\u0644\u0645\u0648\u0644\u0651\u062f (\u0648\u0641\u064a\u0647 \u0641\u062e\u0651 \u00ab\u0627\u0644\u0628\u0627\u0626\u0639 \u064a\u064f\u0642\u0631\u0623 \u0646\u0638\u0627\u0645\u064b\u0627\u00bb)\u060c
                    //      \u0641\u062a\u0643\u0631\u0627\u0631\u064f\u0647 \u0641\u064a \u0647\u0630\u0647 \u0627\u0644\u0637\u0628\u0642\u0629 \u064a\u062e\u0644\u0642 \u0645\u0635\u062f\u0631\u064e \u062d\u0642\u064a\u0642\u0629\u064d \u062b\u0627\u0646\u064a\u064b\u0627. \u0646\u0637\u0628\u0639
                    //      \u0627\u0644\u0645\u0635\u062f\u0631\u064e \u0648\u0627\u0644\u0642\u064a\u0645\u0629\u064e \u0645\u062a\u0649 \u0639\u064f\u0631\u0641\u062a\u060c \u0648\u0627\u0644\u0642\u064a\u0645\u062a\u064a\u0646 \u0627\u0644\u0645\u0645\u0643\u0646\u062a\u064a\u0646 \u0645\u062a\u0649 \u0644\u0645 \u062a\u064f\u0639\u0631\u0641.
                    // (EN) Heap line: the one fact here that breaks the boot if wrong.
                    //      We do not re-classify the target \u2014 that lives in codegen.
                    if (options_.freestanding_heap_mib != 0)
                    {
                        std::cout << u8"     \u2022 \u0627\u0644\u0643\u0648\u0645\u0629 \u0627\u0644\u0633\u0627\u0643\u0646\u0629: "
                                  << options_.freestanding_heap_mib
                                  << u8" \u0645.\u0628 (\u0645\u0646 \u00ab--\u062d\u062c\u0645-\u0627\u0644\u0643\u0648\u0645\u0629\u00bb)\n";
                    }
                    else
                    {
                        std::cout << u8"     \u2022 \u0627\u0644\u0643\u0648\u0645\u0629 \u0627\u0644\u0633\u0627\u0643\u0646\u0629: "
                                  << u8"\u0627\u0641\u062a\u0631\u0627\u0636\u064a\u0651 \u0627\u0644\u0647\u062f\u0641 "
                                  << u8"(4 \u0645.\u0628 \u0639\u0644\u0649 \u0645\u0639\u062f\u0646\u064d \u0639\u0627\u0631\u064d\u060c "
                                  << u8"512 \u0645.\u0628 \u0641\u064a \u0646\u0637\u0627\u0642 \u0627\u0644\u0645\u0633\u062a\u062e\u062f\u0645) "
                                  << u8"\u2014 \u062d\u062f\u0651\u062f\u0647 \u0628\u0640\u00ab--\u062d\u062c\u0645-\u0627\u0644\u0643\u0648\u0645\u0629=\u0646\u00bb\n";
                    }
                    std::cout << "\n";
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
