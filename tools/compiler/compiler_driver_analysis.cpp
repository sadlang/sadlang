// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_analysis.cpp - مراحل التحليل الأمامي
// Frontend Analysis Phases (Lexing, Parsing, SIR Building, Borrow/Type Check)
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يحتوي على دوال التحليل الأمامية التي تتطلب كل مكتبات التحليل:
//   1. run_frontend() — خط أنابيب التحليل الكامل (المعجمي + النحوي + SIR)
//   2. run_borrow_check() — فحص الملكية والاستعارة
//   3. run_type_check() — فحص الأنواع المتقدم
//
// Description (English):
// ----------------------
// This file contains frontend analysis functions requiring ALL analysis libs:
//   1. run_frontend() — full analysis pipeline (lexing + parsing + SIR)
//   2. run_borrow_check() — ownership/borrow checking
//   3. run_type_check() — advanced type checking
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"

// UTF-8 utilities for Arabic filename support
#include "../../shared/utils/include/utf8_utils.h"

// Lexer and Parser
#include "../../shared/lexer/include/lexer_core.h"
#include "../../shared/parser/include/parser_core.h"
#include "../../shared/ast/include/ast_printer.h"

// SIR Builder
#include "../../compiler_new/include/frontend/sir_builder.h"
#include "../../compiler_new/include/frontend/sir_module.h"

// LLVM Backend
#include "../../compiler_new/include/backend/llvm/llvm_codegen.h"

// Arabic Optimizer / المحسّن العربي
#include "../../compiler_new/include/backend/llvm/arabic_optimizer.h"

// SIR Optimizer / محسّن التمثيل الوسيط
// (AR) يحتوي على تمريرات التحسين: طي الثوابت، إزالة الكود الميت، CSE، نشر النسخ
// (EN) Contains optimization passes: constant folding, DCE, CSE, copy propagation
#include "../../compiler_new/include/middle/optimizer.h"

// Frontend SIR Optimizer / محسّن SIR الأمامي (5 ممرات)
#include "../../compiler_new/include/frontend/sir_frontend_optimizer.h"

// Borrow Checker / فاحص الاستعارة
#include "../../compiler_new/include/semantic/borrow_checker.h"
#include "../../shared/ast/include/statements.h"

// (AR) عقد الوحدات — ImportStmt و FromImportStmt لحل التبعيات تلقائياً
// (EN) Module AST nodes — ImportStmt & FromImportStmt for auto dependency resolution
#include "../../shared/ast/include/module_nodes.h"

// (AR) محلل الوحدات — يبحث عن ملفات .ص المستوردة على القرص
// (EN) Module Resolver — finds imported .ص files on disk
#include "../../shared/modules/include/module_resolver.h"

// LLVM headers for file output
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
// (AR) مكتبة كتابة Bitcode - لإخراج ملفات .bc
// (EN) Bitcode writer - for .bc file output
#include <llvm/Bitcode/BitcodeWriter.h>

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
#include <unordered_set>

namespace sad
{
    namespace driver
    {

        // ============================================================================
        // (AR) الباني والهادم — يحتاجان تعريف LLVMCodeGen الكامل
        // (EN) Constructor/Destructor — need complete LLVMCodeGen definition
        // ============================================================================

        CompilerDriver::CompilerDriver()
        {
            // LLVM is always available, no conditional initialization needed
        }

        // Destructor - needs to be defined in cpp where LLVMCodeGen is complete
        CompilerDriver::~CompilerDriver() = default;

        bool CompilerDriver::run_frontend(const std::string &file)
        {
            // Read file
            auto source_opt = read_file(file);
            if (!source_opt)
            {
                diagnostics_.report_fatal("failed to read file: " + file +
                                          " / فشل قراءة الملف");
                return false;
            }

            std::string source = *source_opt;

            // ════════════════════════════════════════════════════════════════════════
            // (AR) المرحلة 0: فحص سمات Freestanding قبل التحليل المعجمي
            // ════════════════════════════════════════════════════════════════════════
            // نبحث في النص المصدري عن سمة #![بلا_مكتبة_قياسية] قبل أي مرحلة تحليل.
            // هذا يسمح للمبرمج باستخدام:
            //
            //   #![بلا_مكتبة_قياسية]      ← السمة العربية (المتوقع في لغة ص)
            //   #![no_std]                ← السمة الإنجليزية (للتوافق)
            //   #![بلا_رئيسية]            ← تعطيل main() بشكل صريح
            //   #![نقطة_دخول="_start"]    ← تحديد نقطة الدخول
            //
            // هذا الفحص المبكر يضمن أن كل مراحل التحليل اللاحقة (المعجمي، النحوي،
            // الدلالي، بناء SIR) تعرف أنها في وضع freestanding وتُطبّق القواعد
            // المناسبة (عدم استخدام malloc, printf, أنواع STL، ...)
            //
            // الخوارزمية:
            //   1. البحث السريع بـ string::find (O(n) مرة واحدة فقط)
            //   2. إذا وُجدت السمة: تفعيل options_.freestanding
            //   3. إعادة تشغيل validate_freestanding_options لضبط باقي الخيارات
            //   4. طباعة رسالة تشخيصية في الوضع المفصل
            // ════════════════════════════════════════════════════════════════════════
            // (EN) Phase 0: Detect freestanding attributes before lexing
            //
            // Search source text for #![بلا_مكتبة_قياسية] before any parsing.
            // This allows programmers to use Arabic and English no_std annotations.
            //
            // Early detection ensures all analysis phases operate in freestanding
            // mode: no malloc, printf, STL types, etc.
            //
            // Algorithm:
            //   1. Fast O(n) string search (done once only)
            //   2. If attribute found: activate options_.freestanding
            //   3. Re-run freestanding defaults setup
            //   4. Print diagnostic in verbose mode
            // ════════════════════════════════════════════════════════════════════════
            {
                // (AR) البحث عن السمات بالعربية والإنجليزية
                // (EN) Search for attributes in both Arabic and English
                const bool has_no_std_arabic = (source.find(u8"#![\u0628\u0644\u0627_\u0645\u0643\u062a\u0628\u0629_\u0642\u064a\u0627\u0633\u064a\u0629]") != std::string::npos);
                const bool has_no_std_english = (source.find("#![no_std]") != std::string::npos);
                const bool has_no_main_arabic = (source.find(u8"#![\u0628\u0644\u0627_\u0631\u0626\u064a\u0633\u064a\u0629]") != std::string::npos);
                const bool has_no_std = has_no_std_arabic || has_no_std_english;

                if (has_no_std && !options_.freestanding)
                {
                    // (AR) تفعيل وضع freestanding تلقائياً
                    // (EN) Auto-activate freestanding mode
                    options_.freestanding = true;

                    // (AR) تفعيل no_main — بلا مكتبة قياسية = بلا نقطة بدء C
                    // (EN) Enable no_main — no stdlib means no C startup entry
                    if (!options_.no_main)
                    {
                        options_.no_main = true;
                    }

                    // (AR) تعيين نقطة الدخول الافتراضية
                    // (EN) Set default entry point
                    if (options_.freestanding_entry.empty())
                    {
                        options_.freestanding_entry = "_start";
                    }

                    // (AR) تفعيل abort_on_panic — لا استثناءات بدون مكتبة
                    // (EN) Enable abort on panic — no exceptions without stdlib
                    options_.abort_on_panic = true;

                    if (options_.verbose)
                    {
                        std::cout << u8"  \u26a1 [مرحلة 0] اكتُشف ";
                        if (has_no_std_arabic)
                        {
                            std::cout << u8"#![\u0628\u0644\u0627_\u0645\u0643\u062a\u0628\u0629_\u0642\u064a\u0627\u0633\u064a\u0629]";
                        }
                        else
                        {
                            std::cout << "#![no_std]";
                        }
                        std::cout << u8" — وضع Freestanding مُفعَّل تلقائياً\n";
                        std::cout << u8"     \u2022 \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: " << options_.freestanding_entry << "\n";
                        std::cout << u8"     \u2022 no_main: \u0645\u0641\u0639\u064e\u0651\u0644 | abort_on_panic: \u0645\u0641\u0639\u064e\u0651\u0644\n";
                    }
                }

                // (AR) تفعيل no_main إذا وُجدت السمة #![بلا_رئيسية] بشكل مستقل
                // (EN) Enable no_main if #![بلا_رئيسية] attribute found independently
                if (has_no_main_arabic && !options_.no_main)
                {
                    options_.no_main = true;
                    if (options_.verbose)
                    {
                        std::cout << u8"  \u26a1 [مرحلة 0] اكتُشف #![\u0628\u0644\u0627_\u0631\u0626\u064a\u0633\u064a\u0629] — no_main \u0645\u0641\u0639\u064e\u0651\u0644\n";
                    }
                }

                // (AR) البحث عن سمة نقطة الدخول المُخصَّصة: #![نقطة_دخول="..."]
                //      مثال: #![نقطة_دخول="kernel_main"] أو #![entry="my_start"]
                // (EN) Search for custom entry point attribute: #![نقطة_دخول="..."]
                {
                    // (AR) نمط السمة العربية
                    const std::string ar_entry_prefix = u8"#![\u0646\u0642\u0637\u0629_\u062f\u062e\u0648\u0644=\"";
                    auto ar_pos = source.find(ar_entry_prefix);
                    if (ar_pos != std::string::npos)
                    {
                        auto start = ar_pos + ar_entry_prefix.size();
                        auto end = source.find('"', start);
                        if (end != std::string::npos && end > start)
                        {
                            options_.freestanding_entry = source.substr(start, end - start);
                            if (options_.verbose)
                            {
                                std::cout << u8"  \u26a1 [مرحلة 0] نقطة دخول مُخصَّصة: "
                                          << options_.freestanding_entry << "\n";
                            }
                        }
                    }

                    // (AR) نمط السمة الإنجليزية للتوافق
                    const std::string en_entry_prefix = "#![entry=\"";
                    auto en_pos = source.find(en_entry_prefix);
                    if (en_pos != std::string::npos && options_.freestanding_entry.empty())
                    {
                        auto start = en_pos + en_entry_prefix.size();
                        auto end = source.find('"', start);
                        if (end != std::string::npos && end > start)
                        {
                            options_.freestanding_entry = source.substr(start, end - start);
                            if (options_.verbose)
                            {
                                std::cout << u8"  \u26a1 [مرحلة 0] entry point (EN): "
                                          << options_.freestanding_entry << "\n";
                            }
                        }
                    }
                }

                // ════════════════════════════════════════════════════════════════════
                // (AR) المرحلة 0.1: اكتشاف سمات الدوال الخاصة بـ freestanding
                // ════════════════════════════════════════════════════════════════════
                // نبحث عن السمات التالية على الدوال (function attributes) لتسجيلها:
                //
                //   #[معالج_ذعر]       → يجب وجودها في كل وحدة freestanding
                //                         تُعالج الذعر بدلاً من throw/std::terminate
                //
                //   #[معالج_تخصيص]    → تُوجَد عند استخدام التخصيص الديناميكي
                //                         مطلوبة مع --allow-alloc
                //
                //   #[معالج_مقاطعة]   → معالجات المقاطعات (ISR)
                //                         تُولَّد مع اتفاقية interrupt تلقائياً
                //
                //   #[نقطة_دخول]      → دالة بدء التشغيل (بدلاً من main)
                //
                // هذه المعلومات تُستخدم لاحقاً في:
                //   - التحقق من اكتمال الوحدة (FreestandingCodeGen::أنهِ_وحدة)
                //   - توليد الاتفاقيات الصحيحة للدوال
                //   - الإبلاغ عن السمات المفقودة
                // ════════════════════════════════════════════════════════════════════
                if (options_.freestanding)
                {
                    // (AR) فحص وجود #[معالج_ذعر] أو #[panic_handler]
                    const bool له_معالج_ذعر_عربي = (source.find(u8"#[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]") != std::string::npos);
                    const bool له_معالج_ذعر_إنجليزي = (source.find("#[panic_handler]") != std::string::npos);

                    // (AR) فحص وجود #[معالج_تخصيص] أو #[alloc_error_handler]
                    const bool له_معالج_تخصيص_عربي = (source.find(u8"#[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635]") != std::string::npos);
                    const bool له_معالج_تخصيص_إنجليزي = (source.find("#[alloc_error_handler]") != std::string::npos);

                    // (AR) فحص وجود #[معالج_مقاطعة] أو #[interrupt_handler]
                    const bool له_معالج_مقاطعة_عربي = (source.find(u8"#[\u0645\u0639\u0627\u0644\u062c_\u0645\u0642\u0627\u0637\u0639\u0629") != std::string::npos);
                    const bool له_معالج_مقاطعة_إنجليزي = (source.find("#[interrupt_handler") != std::string::npos);

                    // (AR) فحص وجود #[نقطة_دخول] أو #[entry_point]
                    const bool له_نقطة_دخول_عربي = (source.find(u8"#[\u0646\u0642\u0637\u0629_\u062f\u062e\u0648\u0644]") != std::string::npos);
                    const bool له_نقطة_دخول_إنجليزي = (source.find("#[entry_point]") != std::string::npos);

                    if (options_.verbose)
                    {
                        // (AR) طباعة ملخص السمات المكتشفة
                        std::cout << u8"\n  \u2550\u2550 \u0633\u0645\u0627\u062a freestanding \u0627\u0644\u0645\u0643\u062a\u0634\u0641\u0629 \u2550\u2550\n";
                        // نقطة الدخول
                        std::cout << (له_نقطة_دخول_عربي || له_نقطة_دخول_إنجليزي
                                          ? u8"  \u2705 "
                                          : u8"  \u274c ")
                                  << u8"#[\u0646\u0642\u0637\u0629_\u062f\u062e\u0648\u0644]\n";
                        // معالج الذعر
                        std::cout << (له_معالج_ذعر_عربي || له_معالج_ذعر_إنجليزي
                                          ? u8"  \u2705 "
                                          : u8"  \u26a0  ")
                                  << u8"#[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]\n";
                        // معالج التخصيص
                        if (options_.allow_freestanding_alloc)
                        {
                            std::cout << (له_معالج_تخصيص_عربي || له_معالج_تخصيص_إنجليزي
                                              ? u8"  \u2705 "
                                              : u8"  \u274c ")
                                      << u8"#[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635] (\u0645\u0637\u0644\u0648\u0628 \u0645\u0639 --allow-alloc)\n";
                        }
                        // معالج المقاطعة
                        if (له_معالج_مقاطعة_عربي || له_معالج_مقاطعة_إنجليزي)
                        {
                            std::cout << u8"  \u2705 #[\u0645\u0639\u0627\u0644\u062c_\u0645\u0642\u0627\u0637\u0639\u0629] (\u0633\u064a\u062a\u0645 \u062a\u0648\u0644\u064a\u062f \u0627\u062a\u0641\u0627\u0642\u064a\u0629 interrupt)\n";
                        }

                        // (AR) تحذير: وضع freestanding بدون معالج ذعر
                        if (!له_معالج_ذعر_عربي && !له_معالج_ذعر_إنجليزي)
                        {
                            std::cout << u8"\n  \u26a0 [freestanding] \u062a\u062d\u0630\u064a\u0631: \u0644\u0627 \u064a\u0648\u062c\u062f #[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]\n"
                                      << u8"     \u0633\u064a\u0633\u062a\u062e\u062f\u0645 \u0627\u0644\u0645\u0639\u0627\u0644\u062c \u0627\u0644\u0627\u0641\u062a\u0631\u0627\u0636\u064a (happy loop + hlt).\n"
                                      << u8"     \u0623\u0636\u0641:\n"
                                      << u8"       #[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]\n"
                                      << u8"       \u062f\u0627\u0644\u0629 \u0639\u0646\u062f_\u0627\u0644\u0630\u0639\u0631() \u062a \u062d\u0644\u0642\u0629 {}\n\n";
                        }

                        // (AR) تحذير: --allow-alloc بدون معالج تخصيص
                        if (options_.allow_freestanding_alloc &&
                            !له_معالج_تخصيص_عربي && !له_معالج_تخصيص_إنجليزي)
                        {
                            std::cout << u8"  \u26a0 [freestanding] \u062a\u062d\u0630\u064a\u0631: --allow-alloc \u0645\u0641\u0639\u0651\u0644 \u0644\u0643\u0646 \u0644\u0627 \u064a\u0648\u062c\u062f #[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635]\n"
                                      << u8"     \u0627\u0644\u0644\u062d\u0627\u0638\u0629: \u0627\u0635\u0641 \u062f\u0627\u0644\u0629 \u0628\u0640 #[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635] \u0644\u0645\u0639\u0627\u0644\u062c\u0629 \u0641\u0634\u0644 \u0627\u0644\u062a\u062e\u0635\u064a\u0635.\n";
                        }

                        std::cout << u8"\n";
                    }
                }
            }
            // ════════════════════════════════════════════════════════════════════════

            // Lex source
            if (options_.verbose)
            {
                std::cout << "  [1/5] Lexing... / التحليل المعجمي...\n";
            }

            lexer_ = std::make_unique<Lexer>(source);

            // Parse tokens
            if (options_.verbose)
            {
                std::cout << "  [2/5] Parsing... / التحليل النحوي...\n";
            }

            parser_ = std::make_unique<Parser>(*lexer_);
            current_ast_ = parser_->parseProgram();

            // Check for parser errors
            if (parser_->hasErrors())
            {
                for (const auto &error : parser_->getErrors())
                {
                    diagnostics_.report_error(error, file);
                }
                return false;
            }

            // Print AST if requested
            if (options_.emit_ast)
            {
                std::cout << "\n========== AST ==========\n";
                if (!current_ast_.empty())
                {
                    // (AR) طباعة ملخص AST
                    // (EN) Print AST summary
                    std::cout << "  Statements: " << current_ast_.size() << "\n";
                    for (size_t i = 0; i < current_ast_.size(); ++i)
                    {
                        if (current_ast_[i])
                        {
                            std::cout << "  [" << i << "] "
                                      << typeid(*current_ast_[i]).name() << "\n";
                        }
                    }
                }
                else
                {
                    std::cout << "(empty AST)\n";
                }
                std::cout << "=========================\n\n";
            }

            // ================================================================
            // (AR) فحص الملكية والاستعارة - بين التحليل وبناء SIR
            // (EN) Borrow check - between parsing and SIR building
            // ================================================================
            if (options_.enable_borrow_check)
            {
                if (options_.verbose)
                {
                    std::cout << "  [3/5] Borrow checking... / فحص الاستعارة...\n";
                }

                if (!run_borrow_check(file))
                {
                    return false;
                }
            }

            // ================================================================
            // (AR) فحص الأنواع المتقدم - بعد فحص الاستعارة وقبل بناء SIR
            // (EN) Advanced type check - after borrow check, before SIR building
            // ================================================================
            if (options_.enable_type_check)
            {
                if (options_.verbose)
                {
                    std::cout << "  [3.5/5] Type checking... / فحص الأنواع...\n";
                }

                if (!run_type_check(file))
                {
                    return false;
                }
            }

            // Build SIR
            if (options_.verbose)
            {
                std::cout << "  [4/5] Building SIR... / بناء SIR...\n";
            }

            sir_builder_ = std::make_unique<SIRBuilder>();
            sir_builder_->setCurrentFilePath(file);
            sir_builder_->setModuleMode(options_.module_mode);
            sir_module_ = sir_builder_->buildModule(&current_ast_);

            // Check for semantic errors
            if (!sir_module_)
            {
                diagnostics_.report_error("Failed to build SIR module", file);
                return false;
            }

            // (AR) تحسين SIR — 5 ممرات: طيّ ثوابت، حذف كود ميت، تضمين، CSE، تبسيط حلقات
            // (EN) Optimize SIR — 5 passes: const fold, DCE, inlining, CSE, loop simplify
            if (options_.opt_level != OptimizationLevel::O0)
            {
                if (options_.verbose)
                {
                    std::cout << "  [4.5/5] Optimizing SIR... / تحسين SIR...\n";
                }
                Sad::Compiler::SIR::SIRFrontendOptimizer sirOptimizer;
                sirOptimizer.optimizeModule(*sir_module_);
                if (options_.verbose)
                {
                    std::cout << sirOptimizer.getStats().toString();
                }
            }

            return true;
        }

        // ============================================================================
        // (AR) فحص الملكية والاستعارة / Borrow Check Phase
        // ============================================================================

        bool CompilerDriver::run_borrow_check(const std::string &file)
        {
            // (AR) إنشاء فاحص الاستعارة
            // (EN) Create borrow checker
            borrow_checker_ = std::make_unique<Sad::Semantic::BorrowChecker>();

            // (AR) إعداد الخيارات
            // (EN) Configure options
            borrow_checker_->setArabicMessages(options_.arabic_borrow_messages);
            borrow_checker_->setDebugMode(options_.debug_borrow_check);
            borrow_checker_->setNLLMode(true); // (AR) تفعيل NLL دائماً / (EN) Always enable NLL

            // (AR) لف العبارات في BlockStmt لتمريرها للفاحص
            // (EN) Wrap statements in BlockStmt to pass to checker
            Sad::AST::BlockStmt programBlock(std::move(current_ast_));

            // (AR) تشغيل الفحص
            // (EN) Run the check
            auto result = borrow_checker_->check(&programBlock);

            // (AR) استعادة AST بعد الفحص (نحتاجها لبناء SIR)
            // (EN) Restore AST after check (needed for SIR building)
            current_ast_ = std::move(programBlock.statements);

            // (AR) عرض ملخص الفحص في الوضع المفصل
            // (EN) Show check summary in verbose mode
            if (options_.verbose)
            {
                if (options_.arabic_borrow_messages)
                {
                    std::cout << result.getArabicSummary();
                }
                else
                {
                    std::cout << result.getSummary();
                }
            }

            // (AR) الإبلاغ عن التحذيرات
            // (EN) Report warnings
            for (const auto &warning : result.warnings)
            {
                diagnostics_.report_warning(warning, file);
            }

            // (AR) الإبلاغ عن الأخطاء
            // (EN) Report errors
            if (!result.success)
            {
                for (const auto &error : result.errors)
                {
                    std::string msg;
                    if (options_.arabic_borrow_messages)
                    {
                        msg = error.toArabicString();
                    }
                    else
                    {
                        msg = error.toEnglishString();
                    }
                    diagnostics_.report_error(msg, file, error.errorLocation.line, error.errorLocation.column);
                }

                // (AR) ملخص الأخطاء
                // (EN) Error summary
                std::cerr << "\n";
                if (options_.arabic_borrow_messages)
                {
                    std::cerr << "✗ فحص الاستعارة فشل: " << result.errors.size()
                              << " خطأ في الملكية\n";
                }
                else
                {
                    std::cerr << "✗ Borrow check failed: " << result.errors.size()
                              << " ownership error(s)\n";
                }

                return false;
            }

            // (AR) نجاح الفحص
            // (EN) Check passed
            if (options_.verbose)
            {
                if (options_.arabic_borrow_messages)
                {
                    std::cout << "  ✓ فحص الاستعارة نجح\n";
                }
                else
                {
                    std::cout << "  ✓ Borrow check passed\n";
                }
            }

            return true;
        }

        // ============================================================================
        // (AR) فحص الأنواع المتقدم / Advanced Type Check Phase
        // ============================================================================

        bool CompilerDriver::run_type_check(const std::string &file)
        {
            // (AR) إنشاء فاحص الأنواع
            // (EN) Create type checker
            type_checker_ = std::make_unique<Sad::Semantic::TypeChecker>();

            // (AR) إعداد الخيارات
            // (EN) Configure options
            type_checker_->setArabicMessages(options_.arabic_borrow_messages);
            type_checker_->setDebugMode(options_.debug_type_check);
            type_checker_->setStrictMode(options_.strict_type_check);

            // (AR) لف العبارات في BlockStmt لتمريرها للفاحص
            // (EN) Wrap statements in BlockStmt to pass to checker
            Sad::AST::BlockStmt programBlock(std::move(current_ast_));

            // (AR) تشغيل الفحص
            // (EN) Run the check
            auto result = type_checker_->check(&programBlock);

            // (AR) استعادة AST بعد الفحص (نحتاجها لبناء SIR)
            // (EN) Restore AST after check (needed for SIR building)
            current_ast_ = std::move(programBlock.statements);

            // (AR) عرض ملخص الفحص في الوضع المفصل
            // (EN) Show check summary in verbose mode
            if (options_.verbose)
            {
                if (options_.arabic_borrow_messages)
                {
                    std::cout << result.getArabicSummary();
                }
                else
                {
                    std::cout << result.getSummary();
                }
            }

            // (AR) الإبلاغ عن التحذيرات
            // (EN) Report warnings
            for (const auto &warning : result.warnings)
            {
                diagnostics_.report_warning(warning, file);
            }

            // (AR) الإبلاغ عن الأخطاء
            // (EN) Report errors
            if (!result.success)
            {
                for (const auto &error : result.errors)
                {
                    std::string msg;
                    if (options_.arabic_borrow_messages)
                    {
                        msg = error.toArabicString();
                    }
                    else
                    {
                        msg = error.toEnglishString();
                    }
                    diagnostics_.report_error(msg, file,
                                              static_cast<int>(error.line),
                                              static_cast<int>(error.column));
                }

                // (AR) ملخص الأخطاء
                // (EN) Error summary
                std::cerr << "\n";
                if (options_.arabic_borrow_messages)
                {
                    std::cerr << "✗ فحص الأنواع فشل: " << result.errors.size()
                              << " خطأ في الأنواع\n";
                }
                else
                {
                    std::cerr << "✗ Type check failed: " << result.errors.size()
                              << " type error(s)\n";
                }

                return false;
            }

            // (AR) نجاح الفحص
            // (EN) Check passed
            if (options_.verbose)
            {
                if (options_.arabic_borrow_messages)
                {
                    std::cout << "  ✓ فحص الأنواع نجح\n";
                }
                else
                {
                    std::cout << "  ✓ Type check passed\n";
                }
            }

            return true;
        }

    } // namespace driver
} // namespace sad
