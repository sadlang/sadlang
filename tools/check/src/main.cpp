// ============================================================================
// (AR) sad-check: فاحص الملكية والأنواع الثابت — يفحص ملف/مجلد .ص بدون تنفيذه
// (EN) sad-check: static ownership and type checker — analyzes .ص files
//
// (AR) يستخدم نفس Lexer/Parser/Ownership الموحَّد في الطبقة المشتركة (shared/)
//      لذلك النتائج مطابقة تماماً لما يُنتجه sad-run و sad-build.
// (EN) Uses the same unified Lexer/Parser/Ownership in shared layer (shared/)
//      so results are byte-identical to what sad-run and sad-build produce.
//
// (AR) المزايا المتقدمة في هذا الإصدار 1.1.0:
//      --watch       : إعادة الفحص تلقائياً عند تغيير الملف/المجلد
//      --recursive   : فحص جميع ملفات .ص في مجلد بشكل تكراري
//      --summary     : إخراج إحصائي مختصر (مفيد لـ CI)
//      --explain     : ملاحظة سياق إضافية حول كل خطأ
//      --no-color    : تعطيل ألوان ANSI
// ============================================================================

#include "lexer_core.h"
#include "parser_core.h"
#include "ownership/borrow_checker.h"
#include "error_manager.h"
#include "utf8_args.h" // (AR) الأداة الموحَّدة في shared/utils/include

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
// (AR) إعلان نطاق-ملف بربط C لـ isatty: في هذه الوحدة لا يُصرّح <unistd.h> عنها
//      (تعارض سلسلة التضمينات مع -std=c++17 الصارم → __STRICT_ANSI__). الإعلان هنا
//      (نطاق عام) يطابق توقيع POSIX/libc فيُحَلّ الرمز عند الربط. extern "C" يجب أن
//      يكون في نطاق ملف لا داخل دالة.
// (EN) File-scope C-linkage declaration for isatty: <unistd.h> doesn't expose it
//      in this TU (include chain vs strict -std=c++17). This global declaration
//      matches the POSIX/libc signature and resolves at link time. extern "C"
//      must be at namespace scope, not inside a function.
extern "C" int isatty(int) noexcept;
#endif

namespace fs = std::filesystem;

namespace
{

    constexpr const char *kCheckVersion = "1.1.0";

    // ============================================================================
    // (AR) دعم ألوان ANSI / (EN) ANSI color support
    // ============================================================================
    struct ColorPalette
    {
        bool enabled = false;
        const char *reset() const { return enabled ? "\033[0m" : ""; }
        const char *red() const { return enabled ? "\033[31m" : ""; }
        const char *green() const { return enabled ? "\033[32m" : ""; }
        const char *yellow() const { return enabled ? "\033[33m" : ""; }
        const char *cyan() const { return enabled ? "\033[36m" : ""; }
        const char *dim() const { return enabled ? "\033[2m" : ""; }
        const char *bold() const { return enabled ? "\033[1m" : ""; }
    };

    void enableVirtualTerminal()
    {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE)
        {
            DWORD mode = 0;
            if (GetConsoleMode(hOut, &mode))
            {
                SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            }
        }
        HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
        if (hErr != INVALID_HANDLE_VALUE)
        {
            DWORD mode = 0;
            if (GetConsoleMode(hErr, &mode))
            {
                SetConsoleMode(hErr, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            }
        }
#endif
    }

    // ============================================================================
    // (AR) خيارات التشغيل / (EN) Runtime options
    // ============================================================================
    struct Options
    {
        std::vector<std::string> inputs;
        bool quiet = false;
        bool useJson = false;
        bool watch = false;
        bool recursive = false;
        bool summary = false;
        bool explain = false;
        ColorPalette color;
    };

    // ============================================================================
    // (AR) نتائج فحص ملف واحد / (EN) Per-file result
    // ============================================================================
    struct FileResult
    {
        std::string path;
        bool readOk = false;
        bool parseOk = false;
        bool ownershipOk = false;
        size_t variables = 0;
        size_t borrows = 0;
        size_t moves = 0;
        std::vector<Sad::Semantic::OwnershipError> errors;

        bool isClean() const { return readOk && parseOk && ownershipOk; }
    };

    // ============================================================================
    // (AR) أدوات مساعدة / (EN) Utilities
    // ============================================================================
    void printUsage(std::ostream &os)
    {
        os << "sad-check v" << kCheckVersion
           << " - فاحص الملكية والأنواع الثابت\n";
        os << "                  static ownership and type checker\n\n";
        os << "الاستخدام / Usage:\n";
        os << "  sad-check <file.\xD8\xB5 | directory> [خيارات / options]\n\n";
        os << "خيارات / Options:\n";
        os << "  -q, --quiet         (AR) لا تطبع رسائل عند النجاح\n";
        os << "                      (EN) suppress success messages\n";
        os << "  --json              (AR) إخراج النتائج بصيغة JSON\n";
        os << "                      (EN) output results as JSON\n";
        os << "  -w, --watch         (AR) إعادة الفحص عند التغيير\n";
        os << "                      (EN) re-check on file change\n";
        os << "  -r, --recursive     (AR) فحص جميع ملفات .ص في مجلد\n";
        os << "                      (EN) recurse into directory for .\xD8\xB5 files\n";
        os << "  --summary           (AR) إخراج إحصائي مختصر (CI-friendly)\n";
        os << "                      (EN) compact stats only (CI-friendly)\n";
        os << "  --explain           (AR) ملاحظة سياق إضافية حول كل خطأ\n";
        os << "                      (EN) add extra context note around each error\n";
        os << "  --no-color          (AR) تعطيل ألوان ANSI\n";
        os << "                      (EN) disable ANSI colors\n";
        os << "  -h, --help          (AR) عرض هذه المساعدة\n";
        os << "  -v, --version       (AR) إصدار الأداة / version info\n\n";
        os << "رمز الخروج / Exit codes:\n";
        os << "  0 = clean   1 = ownership/parse errors   2 = read fail   3 = bad CLI\n";
    }

    std::string readFile(const std::string &path)
    {
        fs::path p = fs::u8path(path);
        std::ifstream f(p, std::ios::binary);
        if (!f)
            return std::string();
        std::ostringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    bool isSadSource(const fs::path &p)
    {
        std::error_code ec;
        if (!fs::is_regular_file(p, ec))
            return false;
        auto name = p.u8string();
        auto pos = name.find_last_of('.');
        if (pos == std::string::npos)
            return false;
        auto suffix = name.substr(pos);
        // (AR) ".ص" بـUTF-8 = 0x2E 0xD8 0xB5
        return suffix == ".\xD8\xB5";
    }

    // (AR) جمع كل ملفات .ص من مدخل (ملف مفرد أو مجلد + recursive)
    std::vector<std::string> gatherInputs(const Options &opts)
    {
        std::vector<std::string> out;
        for (const auto &raw : opts.inputs)
        {
            fs::path p = fs::u8path(raw);
            std::error_code ec;
            if (!fs::exists(p, ec))
            {
                out.push_back(raw); // (AR) سيُبلَّغ عنه كـ read fail
                continue;
            }
            if (fs::is_regular_file(p, ec))
            {
                out.push_back(raw);
                continue;
            }
            if (fs::is_directory(p, ec))
            {
                if (opts.recursive)
                {
                    for (auto it = fs::recursive_directory_iterator(p, ec);
                         it != fs::recursive_directory_iterator() && !ec;
                         it.increment(ec))
                    {
                        if (isSadSource(it->path()))
                        {
                            out.push_back(it->path().u8string());
                        }
                    }
                }
                else
                {
                    for (auto it = fs::directory_iterator(p, ec);
                         it != fs::directory_iterator() && !ec;
                         it.increment(ec))
                    {
                        if (isSadSource(it->path()))
                        {
                            out.push_back(it->path().u8string());
                        }
                    }
                }
            }
        }
        return out;
    }

    // ============================================================================
    // (AR) كاتم مؤقت لـstderr — يمنع تسرب رسائل تشخيص الـparser في أوضاع
    //      summary/json حيث يحتاج المخرج أن يكون نظيفاً للاستهلاك الآلي.
    // (EN) Temporary stderr silencer — prevents parser diagnostic leakage
    //      in summary/json modes where output must be machine-clean.
    // ============================================================================
    class StderrSilencer
    {
    public:
        explicit StderrSilencer(bool active) : active_(active)
        {
            if (!active_)
                return;
            oldBuf_ = std::cerr.rdbuf();
            std::cerr.rdbuf(sink_.rdbuf());
        }
        ~StderrSilencer()
        {
            if (active_)
                std::cerr.rdbuf(oldBuf_);
        }
        StderrSilencer(const StderrSilencer &) = delete;
        StderrSilencer &operator=(const StderrSilencer &) = delete;

    private:
        bool active_;
        std::streambuf *oldBuf_ = nullptr;
        std::ostringstream sink_;
    };

    // ============================================================================
    // (AR) فحص ملف واحد / (EN) Check a single file
    // ============================================================================
    FileResult checkOne(const std::string &path, bool silenceStderr = false)
    {
        FileResult r;
        r.path = path;

        fs::path p = fs::u8path(path);
        std::error_code ec;
        if (!fs::exists(p, ec))
        {
            r.readOk = false;
            return r;
        }
        std::string source = readFile(path);
        r.readOk = true; // (AR) موجود وقابل للفتح

        Sad::Errors::ErrorManager::getInstance().clear();
        Sad::Errors::ErrorManager::getInstance().setSourceCode(source, path);

        StderrSilencer guard(silenceStderr);

        Sad::Lexer::LexerCore lexer(source);
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        if (parser.hasErrors())
        {
            r.parseOk = false;
            return r;
        }
        r.parseOk = true;

        Sad::Semantic::BorrowChecker checker;
        bool allOk = true;
        for (auto &stmt : program)
        {
            if (!stmt)
                continue;
            auto sub = checker.check(stmt.get());
            for (const auto &e : sub.errors)
                r.errors.push_back(e);
            r.variables += sub.totalVariables;
            r.borrows += sub.totalBorrows;
            r.moves += sub.totalMoves;
            if (!sub.success)
                allOk = false;
        }
        r.ownershipOk = allOk;
        return r;
    }

    // ============================================================================
    // (AR) عرض ملف واحد بالنص / (EN) Render single file (text mode)
    // ============================================================================
    void renderText(std::ostream &os, const FileResult &r, const Options &opts)
    {
        const auto &c = opts.color;

        if (!r.readOk)
        {
            os << c.red() << "✗" << c.reset() << " " << r.path
               << ": " << c.red() << "(AR) لا يمكن قراءة الملف / "
               << "(EN) cannot read file" << c.reset() << "\n";
            return;
        }
        if (!r.parseOk)
        {
            os << c.red() << "✗" << c.reset() << " " << r.path
               << ": " << c.red() << "(AR) خطأ نحوي / (EN) parse error"
               << c.reset() << "\n";
            return;
        }
        if (!r.ownershipOk)
        {
            os << c.red() << "✗" << c.reset() << " " << r.path << ": "
               << c.bold() << r.errors.size() << c.reset()
               << " خطأ ملكية / ownership error(s)\n";
            for (const auto &e : r.errors)
            {
                os << "  " << c.red() << "✗" << c.reset()
                   << " متغير '" << c.bold() << e.variableName << c.reset()
                   << "': " << e.arabicMessage << "\n";
                os << "    " << c.dim() << "variable '" << e.variableName
                   << "': " << e.message << c.reset() << "\n";
                if (!e.suggestion.empty())
                {
                    os << "    " << c.cyan() << "💡 " << e.suggestion
                       << c.reset() << "\n";
                }
                if (opts.explain)
                {
                    os << "    " << c.dim()
                       << "(AR) لمعرفة موضع الخطأ بدقة شغّل sad-run على نفس الملف\n"
                       << "    (EN) for precise location, run sad-run on the same file"
                       << c.reset() << "\n";
                }
            }
            return;
        }

        if (opts.quiet)
            return;
        os << c.green() << "✓" << c.reset() << " " << r.path
           << ": " << c.green() << "نظيف / clean" << c.reset()
           << c.dim() << "  (vars=" << r.variables
           << " borrows=" << r.borrows
           << " moves=" << r.moves << ")" << c.reset() << "\n";
    }

    // ============================================================================
    // (AR) إخراج JSON لمصفوفة من النتائج / (EN) JSON array output
    // ============================================================================
    void renderJson(std::ostream &os, const std::vector<FileResult> &results)
    {
        auto esc = [](const std::string &s)
        {
            std::string out;
            out.reserve(s.size() + 4);
            for (char ch : s)
            {
                switch (ch)
                {
                case '"':
                    out += "\\\"";
                    break;
                case '\\':
                    out += "\\\\";
                    break;
                case '\n':
                    out += "\\n";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                default:
                    out += ch;
                }
            }
            return out;
        };

        os << "{\n  \"results\": [\n";
        for (size_t i = 0; i < results.size(); ++i)
        {
            const auto &r = results[i];
            os << "    {\n";
            os << "      \"file\": \"" << esc(r.path) << "\",\n";
            os << "      \"clean\": " << (r.isClean() ? "true" : "false") << ",\n";
            os << "      \"readOk\": " << (r.readOk ? "true" : "false") << ",\n";
            os << "      \"parseOk\": " << (r.parseOk ? "true" : "false") << ",\n";
            os << "      \"ownershipOk\": " << (r.ownershipOk ? "true" : "false") << ",\n";
            os << "      \"variables\": " << r.variables << ",\n";
            os << "      \"borrows\": " << r.borrows << ",\n";
            os << "      \"moves\": " << r.moves << ",\n";
            os << "      \"errors\": [";
            for (size_t j = 0; j < r.errors.size(); ++j)
            {
                const auto &e = r.errors[j];
                os << (j == 0 ? "\n" : ",\n");
                os << "        {\n";
                os << "          \"variable\": \"" << esc(e.variableName) << "\",\n";
                os << "          \"message\": \"" << esc(e.message) << "\",\n";
                os << "          \"messageAr\": \"" << esc(e.arabicMessage) << "\",\n";
                os << "          \"suggestion\": \"" << esc(e.suggestion) << "\"\n";
                os << "        }";
            }
            if (!r.errors.empty())
                os << "\n      ";
            os << "]\n";
            os << "    }" << (i + 1 < results.size() ? "," : "") << "\n";
        }
        os << "  ]\n}\n";
    }

    // ============================================================================
    // (AR) إخراج موجز إحصائي / (EN) Compact summary
    // ============================================================================
    void renderSummary(std::ostream &os,
                       const std::vector<FileResult> &results,
                       const Options &opts)
    {
        const auto &c = opts.color;
        size_t total = results.size();
        size_t clean = 0, parseFail = 0, ownFail = 0, readFail = 0;
        size_t totalVars = 0, totalBorrows = 0, totalMoves = 0, totalErrs = 0;
        for (const auto &r : results)
        {
            if (!r.readOk)
                ++readFail;
            else if (!r.parseOk)
                ++parseFail;
            else if (!r.ownershipOk)
                ++ownFail;
            else
                ++clean;
            totalVars += r.variables;
            totalBorrows += r.borrows;
            totalMoves += r.moves;
            totalErrs += r.errors.size();
        }
        bool allClean = (clean == total);
        os << (allClean ? c.green() : c.yellow())
           << "── ملخص / Summary ──" << c.reset() << "\n";
        os << "  files=" << total
           << "  " << c.green() << "clean=" << clean << c.reset()
           << "  " << c.red() << "ownership-fail=" << ownFail << c.reset()
           << "  " << c.red() << "parse-fail=" << parseFail << c.reset()
           << "  " << c.red() << "read-fail=" << readFail << c.reset()
           << "\n";
        os << "  vars=" << totalVars
           << "  borrows=" << totalBorrows
           << "  moves=" << totalMoves
           << "  errors=" << totalErrs << "\n";
    }

    // ============================================================================
    // (AR) دورة فحص واحدة لكل المدخلات / (EN) One pass over all inputs
    // ============================================================================
    int runOnePass(const Options &opts)
    {
        auto files = gatherInputs(opts);
        if (files.empty())
        {
            std::cerr << "(AR) لا توجد ملفات .\xD8\xB5 للفحص\n";
            std::cerr << "(EN) no .\xD8\xB5 files to check\n";
            return 2;
        }

        std::vector<FileResult> results;
        results.reserve(files.size());
        // (AR) في وضع summary/json نكتم تشخيص الـparser للحصول على مخرج نظيف
        const bool silence = opts.summary || opts.useJson;
        for (const auto &f : files)
        {
            results.push_back(checkOne(f, silence));
        }

        if (opts.useJson)
        {
            renderJson(std::cout, results);
        }
        else if (opts.summary)
        {
            renderSummary(std::cout, results, opts);
        }
        else
        {
            for (const auto &r : results)
            {
                renderText(std::cout, r, opts);
            }
            if (results.size() > 1)
            {
                std::cout << "\n";
                renderSummary(std::cout, results, opts);
            }
        }

        int exit = 0;
        for (const auto &r : results)
        {
            if (!r.readOk)
                exit = std::max(exit, 2);
            else if (!r.parseOk)
                exit = std::max(exit, 1);
            else if (!r.ownershipOk)
                exit = std::max(exit, 1);
        }
        return exit;
    }

    // ============================================================================
    // (AR) وضع المراقبة / (EN) Watch mode (poll mtime every 500ms)
    // ============================================================================
    int runWatch(const Options &opts)
    {
        using Clock = std::chrono::system_clock;
        std::unordered_map<std::string, fs::file_time_type> lastSeen;

        auto snapshotMtimes = [&]()
        {
            std::unordered_map<std::string, fs::file_time_type> snap;
            auto files = gatherInputs(opts);
            for (const auto &f : files)
            {
                std::error_code ec;
                auto t = fs::last_write_time(fs::u8path(f), ec);
                if (!ec)
                    snap[f] = t;
            }
            return snap;
        };

        const auto &c = opts.color;
        std::cout << c.cyan()
                  << "(AR) وضع المراقبة نشط — اضغط Ctrl+C للخروج\n"
                  << "(EN) watch mode active — press Ctrl+C to exit"
                  << c.reset() << "\n\n";

        int lastExit = runOnePass(opts);
        lastSeen = snapshotMtimes();

        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto current = snapshotMtimes();
            bool changed = (current.size() != lastSeen.size());
            if (!changed)
            {
                for (const auto &kv : current)
                {
                    auto it = lastSeen.find(kv.first);
                    if (it == lastSeen.end() || it->second != kv.second)
                    {
                        changed = true;
                        break;
                    }
                }
            }
            if (!changed)
                continue;
            lastSeen = current;

            auto now = Clock::now();
            auto t = Clock::to_time_t(now);
            std::tm tmv{};
#ifdef _WIN32
            localtime_s(&tmv, &t);
#else
            localtime_r(&t, &tmv);
#endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%H:%M:%S", &tmv);

            std::cout << "\n"
                      << c.yellow()
                      << "── إعادة فحص / re-check @ " << buf
                      << " ──" << c.reset() << "\n";
            lastExit = runOnePass(opts);
        }
        return lastExit;
    }

} // anonymous

// ============================================================================
// (AR) نقطة الدخول / (EN) Entry point
// ============================================================================
int main(int argc, char **argv)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    try
    {
        auto args = Sad::HubUtil::getUtf8Args(argc, argv);
        const int n = static_cast<int>(args.size());

        Options opts;
        bool noColor = false;

        for (int i = 1; i < n; ++i)
        {
            const std::string &a = args[i];
            if (a == "--help" || a == "-h")
            {
                printUsage(std::cout);
                return 0;
            }
            if (a == "--version" || a == "-v")
            {
                std::cout << "sad-check " << kCheckVersion << "\n";
                return 0;
            }
            if (a == "--quiet" || a == "-q")
            {
                opts.quiet = true;
                continue;
            }
            if (a == "--json")
            {
                opts.useJson = true;
                continue;
            }
            if (a == "--watch" || a == "-w")
            {
                opts.watch = true;
                continue;
            }
            if (a == "--recursive" || a == "-r")
            {
                opts.recursive = true;
                continue;
            }
            if (a == "--summary")
            {
                opts.summary = true;
                continue;
            }
            if (a == "--explain")
            {
                opts.explain = true;
                continue;
            }
            if (a == "--no-color")
            {
                noColor = true;
                continue;
            }
            if (!a.empty() && a[0] == '-')
            {
                std::cerr << "(AR) معامل غير معروف: " << a << "\n";
                std::cerr << "(EN) unknown option: " << a << "\n";
                return 3;
            }
            opts.inputs.push_back(a);
        }

        if (opts.inputs.empty())
        {
            printUsage(std::cerr);
            return 3;
        }

        // (AR) ألوان: مفعّلة افتراضياً إن كانت stdout طرفية وليست JSON
        if (!noColor && !opts.useJson)
        {
#ifdef _WIN32
            opts.color.enabled = (_isatty(_fileno(stdout)) != 0);
#else
            // (AR) ::isatty مُعلَنة بربط C في نطاق الملف (انظر أعلاه) — في هذه الوحدة
            //      لا يُصرّح <unistd.h> عنها (تعارض سلسلة التضمينات مع -std=c++17).
            // (EN) ::isatty is declared with C linkage at file scope (see above) —
            //      <unistd.h> doesn't expose it in this TU under strict c++17.
            opts.color.enabled = (::isatty(fileno(stdout)) != 0);
#endif
            if (opts.color.enabled)
                enableVirtualTerminal();
        }

        if (opts.watch)
        {
            return runWatch(opts);
        }
        return runOnePass(opts);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "(AR) خطأ غير متوقع في sad-check: " << ex.what() << "\n";
        std::cerr << "(EN) unexpected error in sad-check: " << ex.what() << "\n";
        return 2;
    }
    catch (...)
    {
        std::cerr << "(AR) خطأ غير معروف في sad-check\n";
        std::cerr << "(EN) unknown error in sad-check\n";
        return 2;
    }
}
