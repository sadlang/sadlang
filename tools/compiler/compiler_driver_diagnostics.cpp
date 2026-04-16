// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_diagnostics.cpp - التشخيصات والألوان وTargetTriple
// Diagnostics, Colors, and TargetTriple
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يوفر البنية التحتية للتشخيصات (أخطاء، تحذيرات، ملاحظات)
// والألوان وتحليل target triple.
//
// المحتويات:
//   1. ألوان ANSI (colors namespace) — تُستخدم من كل ملفات compiler_driver
//   2. path_to_utf8() — تحويل مسارات الملفات لـ UTF-8
//   3. TargetTriple — تحليل وتوليد سلاسل الهدف
//   4. Diagnostic — طباعة رسالة تشخيص واحدة
//   5. DiagnosticEngine — محرك التشخيصات (report_note/warning/error/fatal)
//
// Description (English):
// ----------------------
// Infrastructure for diagnostics (errors, warnings, notes), ANSI colors,
// and target triple parsing.
//
// Contents:
//   1. ANSI colors (colors namespace) — used by all compiler_driver files
//   2. path_to_utf8() — safe filesystem path to UTF-8 conversion
//   3. TargetTriple — target triple parsing and generation
//   4. Diagnostic — printing a single diagnostic message
//   5. DiagnosticEngine — diagnostic engine (report_note/warning/error/fatal)
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"

// UTF-8 utilities for Arabic filename support
#include "../../shared/utils/include/utf8_utils.h"

#ifdef _WIN32
#include <windows.h>
#undef ERROR
#undef FATAL
#endif

#include <iostream>
#include <sstream>
#include <vector>

namespace sad
{
    namespace driver
    {

        // ============================================================================
        // (AR) تحويل مسار filesystem::path إلى نص UTF-8 بشكل آمن
        // (EN) Safely convert a filesystem::path to a UTF-8 string
        //      On Windows, path::string() uses ANSI codepage which corrupts Arabic chars.
        //      This helper uses wstring() → from_wstring() for correct UTF-8 output.
        // ============================================================================
        inline std::string path_to_utf8(const std::filesystem::path &p)
        {
#ifdef _WIN32
            return sad::utf8::from_wstring(p.wstring());
#else
            return p.string();
#endif
        }

        // ============================================================================
        // ANSI Color Codes / أكواد الألوان
        // ============================================================================

        namespace colors
        {
            const char *RESET = "\033[0m";
            const char *BOLD = "\033[1m";
            const char *RED = "\033[31m";
            const char *GREEN = "\033[32m";
            const char *YELLOW = "\033[33m";
            const char *BLUE = "\033[34m";
            const char *MAGENTA = "\033[35m";
            const char *CYAN = "\033[36m";
            const char *WHITE = "\033[37m";

            // Disable colors if not supported
            void disable_colors()
            {
                RESET = "";
                BOLD = "";
                RED = "";
                GREEN = "";
                YELLOW = "";
                BLUE = "";
                MAGENTA = "";
                CYAN = "";
                WHITE = "";
            }
        }

        // ============================================================================
        // TargetTriple Implementation / تنفيذ TargetTriple
        // ============================================================================

        std::optional<TargetTriple> TargetTriple::parse(const std::string &triple)
        {
            // Parse format: <arch>-<vendor>-<os>-<env>
            // Examples: x86_64-pc-linux-gnu, aarch64-apple-macos

            TargetTriple result;
            std::istringstream ss(triple);
            std::string part;
            std::vector<std::string> parts;

            while (std::getline(ss, part, '-'))
            {
                parts.push_back(part);
            }

            if (parts.size() < 2)
            {
                return std::nullopt; // Invalid format
            }

            result.architecture = parts[0];

            if (parts.size() == 2)
            {
                // <arch>-<os>
                result.vendor = "unknown";
                result.os = parts[1];
                result.environment = "";
            }
            else if (parts.size() == 3)
            {
                // <arch>-<vendor>-<os>
                result.vendor = parts[1];
                result.os = parts[2];
                result.environment = "";
            }
            else if (parts.size() >= 4)
            {
                // <arch>-<vendor>-<os>-<env>
                result.vendor = parts[1];
                result.os = parts[2];
                result.environment = parts[3];
            }

            return result;
        }

        std::string TargetTriple::to_string() const
        {
            std::string result = architecture;

            if (!vendor.empty())
            {
                result += "-" + vendor;
            }

            if (!os.empty())
            {
                result += "-" + os;
            }

            if (!environment.empty())
            {
                result += "-" + environment;
            }

            return result;
        }

        TargetTriple TargetTriple::get_host_target()
        {
            TargetTriple result;

#if defined(_WIN32) || defined(_WIN64)
            result.architecture = "x86_64";
            result.vendor = "pc";
            result.os = "windows";
            result.environment = "msvc";
#elif defined(__linux__)
            result.architecture = "x86_64";
            result.vendor = "pc";
            result.os = "linux";
            result.environment = "gnu";
#elif defined(__APPLE__)
            result.architecture = "x86_64";
            result.vendor = "apple";
            result.os = "macos";
            result.environment = "";
#else
            result.architecture = "unknown";
            result.vendor = "unknown";
            result.os = "unknown";
            result.environment = "";
#endif

            return result;
        }

        // ============================================================================
        // Diagnostic Implementation / تنفيذ التشخيص
        // ============================================================================

        void Diagnostic::print(std::ostream &os, bool use_colors) const
        {
            // Color codes
            const char *color_start = "";
            const char *color_end = use_colors ? colors::RESET : "";
            const char *bold_start = use_colors ? colors::BOLD : "";

            // Select color based on severity
            switch (severity)
            {
            case DiagnosticSeverity::NOTE:
                color_start = use_colors ? colors::CYAN : "";
                break;
            case DiagnosticSeverity::WARNING:
                color_start = use_colors ? colors::YELLOW : "";
                break;
            case DiagnosticSeverity::ERROR:
            case DiagnosticSeverity::FATAL:
                color_start = use_colors ? colors::RED : "";
                break;
            }

            // Severity label
            std::string severity_label;
            std::string severity_label_ar;

            switch (severity)
            {
            case DiagnosticSeverity::NOTE:
                severity_label = "note";
                severity_label_ar = "ملاحظة";
                break;
            case DiagnosticSeverity::WARNING:
                severity_label = "warning";
                severity_label_ar = "تحذير";
                break;
            case DiagnosticSeverity::ERROR:
                severity_label = "error";
                severity_label_ar = "خطأ";
                break;
            case DiagnosticSeverity::FATAL:
                severity_label = "fatal error";
                severity_label_ar = "خطأ فادح";
                break;
            }

            // Print location if available
            if (!file.empty())
            {
                os << bold_start << file;
                if (line > 0)
                {
                    os << ":" << line;
                    if (column > 0)
                    {
                        os << ":" << column;
                    }
                }
                os << ": ";
            }

            // Print severity
            os << color_start << bold_start << severity_label << ": " << color_end;

            // Print message (bilingual)
            os << bold_start << message << color_end;

            if (!message_ar.empty())
            {
                os << " / " << color_start << message_ar << color_end;
            }

            os << "\n";

            // Print source line with caret
            if (!source_line.empty() && line > 0 && column > 0)
            {
                os << source_line << "\n";

                // Print caret (^) at error position
                for (int i = 1; i < column; ++i)
                {
                    os << " ";
                }
                os << color_start << "^" << color_end << "\n";
            }

            // Print notes
            for (const auto &note : notes)
            {
                os << colors::CYAN << "note: " << colors::RESET << note << "\n";
            }
        }

        void DiagnosticEngine::report_note(const std::string &msg, const std::string &file,
                                           int line, int column)
        {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::NOTE;
            diag.message = msg;
            diag.file = file;
            diag.line = line;
            diag.column = column;
            add_diagnostic(diag);
        }

        void DiagnosticEngine::report_warning(const std::string &msg, const std::string &file,
                                              int line, int column)
        {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::WARNING;
            diag.message = msg;
            diag.file = file;
            diag.line = line;
            diag.column = column;
            add_diagnostic(diag);
            ++warning_count_;

            if (warnings_as_errors_)
            {
                ++error_count_;
            }
        }

        void DiagnosticEngine::report_error(const std::string &msg, const std::string &file,
                                            int line, int column)
        {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::ERROR;
            diag.message = msg;
            diag.file = file;
            diag.line = line;
            diag.column = column;
            add_diagnostic(diag);
            ++error_count_;
        }

        void DiagnosticEngine::report_fatal(const std::string &msg, const std::string &file,
                                            int line, int column)
        {
            Diagnostic diag;
            diag.severity = DiagnosticSeverity::FATAL;
            diag.message = msg;
            diag.file = file;
            diag.line = line;
            diag.column = column;
            add_diagnostic(diag);
            ++error_count_;
        }

        void DiagnosticEngine::print_diagnostics(std::ostream &os, bool use_colors) const
        {
            for (const auto &diag : diagnostics_)
            {
                diag.print(os, use_colors);
            }
        }

        void DiagnosticEngine::add_diagnostic(Diagnostic diag)
        {
            diagnostics_.push_back(std::move(diag));
        }

    } // namespace driver
} // namespace sad
