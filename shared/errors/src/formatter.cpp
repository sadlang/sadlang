/**
 * @file formatter.cpp
 * @brief (AR) تنفيذ Formatter — يستخدم Diagnostic::format ويضيف ملخصاً
 *        (EN) Formatter impl — leverages Diagnostic::format and adds a summary
 */

#include "formatter.h"

#include <ostream>

namespace Sad
{
    namespace Errors
    {

        namespace
        {
            const char *RESET = "\033[0m";
            const char *BOLD = "\033[1m";
            const char *RED = "\033[91m";
            const char *YELLOW = "\033[93m";
            const char *GREEN = "\033[92m";
        } // anonymous namespace

        void Formatter::printDiagnostic(std::ostream &os,
                                        const Diagnostic &diag,
                                        const FormatterOptions &opts)
        {
            // (AR) Diagnostic::format يبني السلسلة الكاملة (caret + colors + fix-its + notes)
            // (EN) Diagnostic::format builds the full string (caret + colors + fix-its + notes)
            os << diag.format(opts.language,
                              opts.colorize,
                              opts.showSource ? opts.sourceCode : std::string{});
            os << "\n";
        }

        void Formatter::printAll(std::ostream &os,
                                 const std::vector<Diagnostic> &diags,
                                 const FormatterOptions &opts)
        {
            size_t errors = 0, warnings = 0;
            for (const auto &d : diags)
            {
                printDiagnostic(os, d, opts);
                switch (d.getSeverity())
                {
                case DiagnosticSeverity::ERROR:
                    ++errors;
                    break;
                case DiagnosticSeverity::WARNING:
                    ++warnings;
                    break;
                default:
                    break;
                }
            }
            if (opts.showSummary && !diags.empty())
            {
                printSummary(os, errors, warnings, opts);
            }
        }

        void Formatter::printSummary(std::ostream &os,
                                     size_t errorCount,
                                     size_t warningCount,
                                     const FormatterOptions &opts)
        {
            const bool color = opts.colorize;
            const bool ar = (opts.language == Language::ARABIC || opts.language == Language::BOTH);
            const bool en = (opts.language == Language::ENGLISH || opts.language == Language::BOTH);

            os << "\n";
            if (color)
                os << BOLD;
            os << "─────────────────────────────────────\n";

            auto colorFor = [&](size_t n, bool isError) -> const char *
            {
                if (!color)
                    return "";
                if (n == 0)
                    return GREEN;
                return isError ? RED : YELLOW;
            };

            if (ar)
            {
                os << colorFor(errorCount, true) << "أخطاء: " << errorCount << "  ";
                if (color)
                    os << RESET;
                os << colorFor(warningCount, false) << "تحذيرات: " << warningCount;
                if (color)
                    os << RESET;
                os << "\n";
            }
            if (en)
            {
                os << colorFor(errorCount, true) << "Errors: " << errorCount << "  ";
                if (color)
                    os << RESET;
                os << colorFor(warningCount, false) << "Warnings: " << warningCount;
                if (color)
                    os << RESET;
                os << "\n";
            }
            if (color)
                os << RESET;
        }

    } // namespace Errors
} // namespace Sad
