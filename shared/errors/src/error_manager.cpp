/**
 * @file error_manager.cpp
 * @brief (AR) تطبيق المدير المركزي للأخطاء
 *        (EN) Implementation of central error manager
 *
 * (AR) يحتوي على تطبيق ErrorManager (Singleton) وجميع دوال الإدارة
 * (EN) Contains ErrorManager (Singleton) implementation and all management functions
 *
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 */

#include "error_manager.h"
#include "formatter.h"
#include <mutex>
#include "smart_errors.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Sad
{
    namespace Errors
    {

        // ====================================================================
        // (AR) تطبيق DiagnosticSink / (EN) DiagnosticSink Implementation
        // ====================================================================

        /**
         * @brief (AR) يضيف تشخيصاً جديداً
         *        (EN) Adds new diagnostic
         */
        void DiagnosticSink::add(const Diagnostic &diag)
        {
            diagnostics_.push_back(diag);

            // (AR) تحديث العدادات
            // (EN) Update counters
            if (diag.getSeverity() == DiagnosticSeverity::ERROR)
            {
                errorCount_++;
            }
            else if (diag.getSeverity() == DiagnosticSeverity::WARNING)
            {
                warningCount_++;
            }
        }

        /**
         * @brief (AR) يمسح جميع التشخيصات
         *        (EN) Clears all diagnostics
         */
        void DiagnosticSink::clear()
        {
            diagnostics_.clear();
            errorCount_ = 0;
            warningCount_ = 0;
        }

        // (AR) يقتطع التشخيصات إلى أوّل n ويُعيد حساب عدّادات الأخطاء/التحذيرات.
        // (EN) Truncates diagnostics to the first n and recomputes counts.
        void DiagnosticSink::truncateTo(size_t n)
        {
            if (n >= diagnostics_.size())
            {
                return; // (AR) لا شيء لإزالته / (EN) nothing to remove
            }
            diagnostics_.resize(n);
            // (AR) إعادة حساب العدّادات من البقيّة / (EN) recompute counters from the remainder
            errorCount_ = 0;
            warningCount_ = 0;
            for (const auto &diag : diagnostics_)
            {
                if (diag.getSeverity() == DiagnosticSeverity::ERROR)
                {
                    errorCount_++;
                }
                else if (diag.getSeverity() == DiagnosticSeverity::WARNING)
                {
                    warningCount_++;
                }
            }
        }

        // ====================================================================
        // (AR) تطبيق ErrorManager / (EN) ErrorManager Implementation
        // ====================================================================

        /**
         * @brief (AR) يرجع النسخة الوحيدة (Singleton)
         *        (EN) Returns singleton instance
         */
        ErrorManager &ErrorManager::getInstance()
        {
            static ErrorManager instance;
            return instance;
        }

        /**
         * @brief (AR) يبلغ عن خطأ
         *        (EN) Reports an error
         */
        void ErrorManager::reportError(ErrorCode code,
                                       const SourceLocation &location,
                                       const std::string &message_ar,
                                       const std::string &message_en)
        {
            std::lock_guard<std::mutex> lock(mutex_);

            // (AR) التحقق من الحد الأقصى
            // (EN) Check max errors
            if (sink_.getErrorCount() >= maxErrors_)
            {
                return;
            }

            Diagnostic diag(code, DiagnosticSeverity::ERROR, location,
                            message_ar, message_en);

            // Phase 15: Smart Error enrichment
            if (smartErrorsEnabled_)
            {
                diag = SmartErrorSystem::getInstance().enrichDiagnostic(diag);
            }

            sink_.add(diag);
        }

        /**
         * @brief (AR) يبلغ عن تحذير
         *        (EN) Reports a warning
         */
        void ErrorManager::reportWarning(ErrorCode code,
                                         const SourceLocation &location,
                                         const std::string &message_ar,
                                         const std::string &message_en)
        {
            std::lock_guard<std::mutex> lock(mutex_);

            Diagnostic diag(code, DiagnosticSeverity::WARNING, location,
                            message_ar, message_en);
            sink_.add(diag);
        }

        /**
         * @brief (AR) يضيف تشخيص كامل
         *        (EN) Adds complete diagnostic
         */
        void ErrorManager::report(const Diagnostic &diag)
        {
            std::lock_guard<std::mutex> lock(mutex_);

            // (AR) التحقق من الحد الأقصى للأخطاء فقط
            // (EN) Check max errors only for ERROR severity
            if (diag.getSeverity() == DiagnosticSeverity::ERROR &&
                sink_.getErrorCount() >= maxErrors_)
            {
                return;
            }

            sink_.add(diag);
        }

        /**
         * @brief (AR) يرجع عدد الأخطاء
         *        (EN) Returns error count
         */
        size_t ErrorManager::getErrorCount() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return sink_.getErrorCount();
        }

        /**
         * @brief (AR) يرجع عدد التحذيرات
         *        (EN) Returns warning count
         */
        size_t ErrorManager::getWarningCount() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return sink_.getWarningCount();
        }

        /**
         * @brief (AR) يتحقق من وجود أخطاء
         *        (EN) Checks if errors exist
         */
        bool ErrorManager::hasErrors() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return sink_.hasErrors();
        }

        /**
         * @brief (AR) يرجع جميع التشخيصات
         *        (EN) Returns all diagnostics
         */
        const std::vector<Diagnostic> &ErrorManager::getAllDiagnostics() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return sink_.getAll();
        }

        // (AR) عدد كل التشخيصات — لأخذ لقطة قبل كتلة «حاول».
        // (EN) Total diagnostics count — to snapshot before a 'try' block.
        size_t ErrorManager::getDiagnosticCount() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return sink_.size();
        }

        // (AR) يتراجع عن التشخيصات إلى لقطة سابقة بعد التقاط استثناء بـ«امسك».
        // (EN) Rolls diagnostics back to a prior snapshot after a 'catch'.
        void ErrorManager::truncateDiagnosticsTo(size_t n)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            sink_.truncateTo(n);
        }

        /**
         * @brief (AR) يطبع جميع التشخيصات
         *        (EN) Prints all diagnostics
         */
        void ErrorManager::printAll(Language lang, bool colorize) const
        {
            std::lock_guard<std::mutex> lock(mutex_);

            const auto &diagnostics = sink_.getAll();

            if (diagnostics.empty())
            {
                return;
            }

            // (AR) عنوان التقرير
            // (EN) Report header
            const std::string BOLD = colorize ? "\033[1m" : "";
            const std::string RESET = colorize ? "\033[0m" : "";

            std::cerr << BOLD << "═══════════════════════════════════════════════\n";
            std::cerr << "  ";

            if (lang == Language::ARABIC)
            {
                std::cerr << "تقرير التشخيص";
            }
            else
            {
                std::cerr << "Diagnostic Report";
            }

            std::cerr << "\n";
            std::cerr << "═══════════════════════════════════════════════\n";
            std::cerr << RESET;

            // (AR) عرض كل تشخيص
            // (EN) Display each diagnostic
            for (size_t i = 0; i < diagnostics.size(); i++)
            {
                std::cerr << diagnostics[i].format(lang, colorize, sourceCode_);

                if (i < diagnostics.size() - 1)
                {
                    std::cerr << "\n";
                    std::cerr << "───────────────────────────────────────────────\n";
                    std::cerr << "\n";
                }
            }

            // (AR) الملخص النهائي
            // (EN) Final summary
            std::cerr << "\n";
            std::cerr << BOLD << "═══════════════════════════════════════════════\n";

            size_t errorCount = sink_.getErrorCount();
            size_t warningCount = sink_.getWarningCount();

            if (errorCount > 0)
            {
                const std::string RED = colorize ? "\033[91m" : "";
                std::cerr << RED << "❌ " << errorCount;
                if (lang == Language::ARABIC)
                {
                    std::cerr << " خطأ";
                }
                else
                {
                    std::cerr << " error(s)";
                }
                std::cerr << RESET + BOLD;
            }

            if (warningCount > 0)
            {
                if (errorCount > 0)
                    std::cerr << "  ";

                const std::string YELLOW = colorize ? "\033[93m" : "";
                std::cerr << YELLOW << "⚠️  " << warningCount;
                if (lang == Language::ARABIC)
                {
                    std::cerr << " تحذير";
                }
                else
                {
                    std::cerr << " warning(s)";
                }
                std::cerr << RESET + BOLD;
            }

            std::cerr << "\n";
            std::cerr << "═══════════════════════════════════════════════\n";
            std::cerr << RESET;
        }

        /**
         * @brief (AR) يحول جميع التشخيصات إلى JSON
         *        (EN) Converts all diagnostics to JSON
         */
        std::string ErrorManager::toJSON() const
        {
            std::lock_guard<std::mutex> lock(mutex_);

            const auto &diagnostics = sink_.getAll();

            std::ostringstream oss;

            oss << "{\n";
            oss << "  \"version\": \"1.0\",\n";
            oss << "  \"language\": \"sad\",\n";

            // Statistics
            oss << "  \"statistics\": {\n";
            oss << "    \"total\": " << diagnostics.size() << ",\n";
            oss << "    \"errors\": " << sink_.getErrorCount() << ",\n";
            oss << "    \"warnings\": " << sink_.getWarningCount() << "\n";
            oss << "  },\n";

            // Diagnostics array
            oss << "  \"diagnostics\": [\n";

            for (size_t i = 0; i < diagnostics.size(); i++)
            {
                std::string diagJSON = diagnostics[i].toJSON();

                // Indent
                std::istringstream stream(diagJSON);
                std::string line;
                bool first = true;

                while (std::getline(stream, line))
                {
                    if (!first)
                        oss << "\n";
                    oss << "    " << line;
                    first = false;
                }

                if (i < diagnostics.size() - 1)
                {
                    oss << ",";
                }
                oss << "\n";
            }

            oss << "  ]\n";
            oss << "}\n";

            return oss.str();
        }

        /**
         * @brief (AR) يحفظ التشخيصات في ملف
         *        (EN) Saves diagnostics to file
         */
        void ErrorManager::saveToFile(const std::string &filename) const
        {
            std::lock_guard<std::mutex> lock(mutex_);

            std::ofstream file(filename);
            if (!file.is_open())
            {
                return;
            }

            // (AR) حفظ كـ JSON
            // (EN) Save as JSON
            std::string json = toJSON();
            file << json;
            file.close();
        }

        /**
         * @brief (AR) يحدد اللغة الافتراضية
         *        (EN) Sets default language
         */
        void ErrorManager::setLanguage(Language lang)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            defaultLang_ = lang;
        }

        /**
         * @brief (AR) يحدد استخدام الألوان
         *        (EN) Sets colorize option
         */
        void ErrorManager::setColorize(bool enable)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            colorizeOutput_ = enable;
        }

        /**
         * @brief (AR) يحدد الحد الأقصى للأخطاء
         *        (EN) Sets maximum errors
         */
        void ErrorManager::setMaxErrors(size_t max)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            maxErrors_ = max;
        }

        /**
         * @brief (AR) يحدد ملف الإخراج
         *        (EN) Sets output file
         */
        void ErrorManager::setOutputFile(const std::string &filename)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            outputFile_ = filename;
        }

        /**
         * @brief (AR) يمسح جميع التشخيصات
         *        (EN) Clears all diagnostics
         */
        void ErrorManager::clear()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            sink_.clear();
        }

        // ====================================================================
        // (AR) المرحلة 1 — تكامل ErrorCatalog + ExplanationLevel
        // (EN) Phase 1 — ErrorCatalog + ExplanationLevel integration
        // ====================================================================

        void ErrorManager::initializeDefaults()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (catalogInitialized_)
                return;
            ErrorCatalog::instance().registerDefaults();
            catalogInitialized_ = true;
        }

        void ErrorManager::setExplanationLevel(ExplanationLevel level)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            explanationLevel_ = level;
        }

        ExplanationLevel ErrorManager::getExplanationLevel() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return explanationLevel_;
        }

        Language ErrorManager::getLanguage() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return defaultLang_;
        }

        std::string ErrorManager::buildBilingualMessage(ErrorCode code,
                                                        const RenderContext &ctx) const
        {
            // (AR) نلتقط الإعدادات داخل القفل، ثم نُحرِّر القفل قبل استدعاء Catalog
            //      لتجنّب القفل المتشابك إن استدعى Catalog لاحقاً ErrorManager.
            // (EN) Snapshot settings under lock, release before calling Catalog to
            //      avoid potential deadlock if Catalog ever calls back into ErrorManager.
            ExplanationLevel lvl;
            Language lang;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                lvl = explanationLevel_;
                lang = defaultLang_;
            }

            const auto rendered = ErrorCatalog::instance().render(code, lvl, lang, ctx);

            // ================================================================
            // (AR) 🔑 الرسالةُ تحملُ رمزَها. كان مسارُ المحلّلِ يطبعُ
            //      «⛔ [SYN001] …» ومسارُ الكتالوجِ هذا يطبعُ النصَّ **بلا رمز**،
            //      فكانت بذرةٌ تطلبُ رمزًا بعينِه تُخفِقُ مع أنّ الرفضَ صحيح:
            //      قِيسَ (٢٠٢٦-٠٩-٠٣) أنّ **ستَّ بذورٍ** في `sections` صنفُها
            //      «رُفِضَ لكنْ بلا الرمزِ المطلوب» — جذرُها هذا السطرُ لا ستّةُ
            //      أعطاب. ومعها بذرةُ SEM049 الجديدةُ التي عجزت عن توكيدِ رمزِها.
            //
            //      والموضعُ هنا لا في كلِّ باعث: `buildBilingualMessage` هي
            //      **المصبُّ الواحدُ** لكلِّ رسالةٍ من الكتالوج، أماميّةً كانت أو
            //      خلفيّة. فالرمزُ يُضافُ مرّةً ويظهرُ في كلِّ مكان.
            // (EN) Every catalog message carries its code. The parser path printed
            //      «⛔ [SYN001] …» while this path printed the text WITHOUT a code,
            //      so a seed asserting a specific code failed even though the
            //      rejection was correct — measured: six seeds in `sections` share
            //      this one line as their root, not six separate defects. This is
            //      the single sink for every catalog message, frontend or backend.
            // ================================================================
            // (AR) صياغة موحَّدة: "[CODE] (AR) ... / (EN) ..." حسب اللغة المختارة
            // (EN) Unified format: "[CODE] (AR) ... / (EN) ..." per selected language
            std::string out;
            out += "[";
            out += getErrorCodeString(code);
            out += "] ";
            if (!rendered.messageAr.empty())
            {
                out += "(AR) ";
                out += rendered.messageAr;
            }
            if (!rendered.messageEn.empty())
            {
                if (!out.empty())
                    out += "\n";
                out += "(EN) ";
                out += rendered.messageEn;
            }
            if (rendered.fixHintAr || rendered.fixHintEn)
            {
                out += "\n";
                if (rendered.fixHintAr)
                {
                    out += "  → ";
                    out += *rendered.fixHintAr;
                }
                if (rendered.fixHintAr && rendered.fixHintEn)
                    out += "\n";
                if (rendered.fixHintEn)
                {
                    out += "  → ";
                    out += *rendered.fixHintEn;
                }
            }
            if (rendered.codeExample)
            {
                out += "\n";
                out += *rendered.codeExample;
            }
            return out;
        }

        void ErrorManager::reportFromCatalog(ErrorCode code,
                                             const SourceLocation &location,
                                             const RenderContext &ctx)
        {
            ExplanationLevel lvl;
            Language lang;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                lvl = explanationLevel_;
                lang = defaultLang_;
            }
            const auto r = ErrorCatalog::instance().render(code, lvl, lang, ctx);
            reportError(code, location, r.messageAr, r.messageEn);
        }

        void ErrorManager::reportWarningFromCatalog(ErrorCode code,
                                                    const SourceLocation &location,
                                                    const RenderContext &ctx)
        {
            ExplanationLevel lvl;
            Language lang;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                lvl = explanationLevel_;
                lang = defaultLang_;
            }
            const auto r = ErrorCatalog::instance().render(code, lvl, lang, ctx);
            reportWarning(code, location, r.messageAr, r.messageEn);
        }

        void ErrorManager::flush(std::ostream &os)
        {
            // (AR) لقطة آمنة للإعدادات والتشخيصات (نسخة) قبل تحرير القفل
            // (EN) Thread-safe snapshot of settings + diagnostics (copy) before releasing lock
            FormatterOptions opts;
            std::vector<Diagnostic> diagsCopy;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                opts.language = defaultLang_;
                opts.colorize = colorizeOutput_;
                opts.sourceCode = sourceCode_;
                diagsCopy = sink_.getAll();
            }
            Formatter::printAll(os, diagsCopy, opts);
        }

        // ====================================================================
        // (AR) تطبيق DiagnosticBuilder / (EN) DiagnosticBuilder Implementation
        // ====================================================================

        /**
         * @brief (AR) البناء
         *        (EN) Constructor
         */
        DiagnosticBuilder::DiagnosticBuilder(ErrorCode code,
                                             DiagnosticSeverity severity,
                                             const SourceLocation &location)
            : diag_(code, severity, location, "", "") {}

        /**
         * @brief (AR) يضيف الرسالة
         *        (EN) Adds message
         */
        DiagnosticBuilder &DiagnosticBuilder::withMessage(const std::string &ar,
                                                          const std::string &en)
        {
            diag_ = Diagnostic(diag_.getCode(), diag_.getSeverity(),
                               diag_.getLocation(), ar, en);
            return *this;
        }

        /**
         * @brief (AR) يضيف اقتراح إصلاح
         *        (EN) Adds fix-it hint
         */
        DiagnosticBuilder &DiagnosticBuilder::withFixIt(const FixItHint &hint)
        {
            diag_.addFixIt(hint);
            return *this;
        }

        /**
         * @brief (AR) يضيف اقتراح إدراج
         *        (EN) Adds insert fix-it
         */
        DiagnosticBuilder &DiagnosticBuilder::withFixItInsert(const SourceLocation &loc,
                                                              const std::string &text)
        {
            FixItHint hint(FixItHint::Type::INSERT, loc, text,
                           "أضف '" + text + "'",
                           "Add '" + text + "'");
            return withFixIt(hint);
        }

        /**
         * @brief (AR) يضيف اقتراح استبدال
         *        (EN) Adds replace fix-it
         */
        DiagnosticBuilder &DiagnosticBuilder::withFixItReplace(const SourceLocation &loc,
                                                               const std::string &text)
        {
            FixItHint hint(FixItHint::Type::REPLACE, loc, text,
                           "استبدل بـ '" + text + "'",
                           "Replace with '" + text + "'");
            return withFixIt(hint);
        }

        /**
         * @brief (AR) يضيف ملاحظة
         *        (EN) Adds note
         */
        DiagnosticBuilder &DiagnosticBuilder::withNote(const Diagnostic &note)
        {
            diag_.addNote(note);
            return *this;
        }

        /**
         * @brief (AR) يبني التشخيص
         *        (EN) Builds diagnostic
         */
        Diagnostic DiagnosticBuilder::build()
        {
            return diag_;
        }

        /**
         * @brief (AR) يبني ويبلغ فوراً
         *        (EN) Builds and reports immediately
         */
        void DiagnosticBuilder::report()
        {
            ErrorManager::getInstance().report(diag_);
        }

    } // namespace Errors
} // namespace Sad
