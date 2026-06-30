// بسم الله الرحمن الرحيم
/**
 * @file repl_engine.cpp
 * @brief تنفيذ محرك REPL للغة ص / REPL Engine Implementation
 *
 * @author Sad Language Team
 * @date January 2026
 */

#include "repl_engine.h"
#include <iostream>
#include <fstream>
#include <sstream>

// (AR) DEF-002: جسر تطبيق سياسة الذاكرة الموحَّد. الـ REPL لا يحلّل أعلام CLI
//      لسياسة الذاكرة بعد، لذا الاستدعاء أدناه `policySet=false` ⇒ no-op.
//      يبقى حاضراً كنقطة استعداد: عند إضافة أعلام للـ REPL لاحقاً يكفي تمرير
//      الإعدادات الفعلية + true بدون إعادة هندسة.
// (EN) DEF-002: unified policy bridge. REPL does not parse CLI flags yet, so
//      we invoke with policySet=false (no-op) — placeholder for future wiring.
#include "memory/gc/policy_bridge.h"
#include "memory/policy/gc_mode.h"
#include "ui/sad_graphics_bridge.h" // (AR) م2-أ: تثبيت جسر الواجهات (REPL مضيف مفسّر تفاعليّ)

// Windows VOID macro conflicts with ::Sad::Types::SadTypeKind::Void
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#ifdef VOID
#undef VOID
#endif

namespace Sad
{
    namespace REPL
    {

        // ============================================================================
        // Helper Functions / دوال مساعدة
        // ============================================================================

        // C++17 compatible starts_with
        inline bool starts_with(const std::string &str, const std::string &prefix)
        {
            if (prefix.length() > str.length())
                return false;
            return str.compare(0, prefix.length(), prefix) == 0;
        }

        // ============================================================================
        // Color Codes / أكواد الألوان
        // ============================================================================

        static const char *RESET = "\033[0m";
        static const char *BOLD = "\033[1m";
        static const char *RED = "\033[31m";
        static const char *GREEN = "\033[32m";
        static const char *YELLOW = "\033[33m";
        static const char *BLUE = "\033[34m";
        static const char *CYAN = "\033[36m";
        static const char *MAGENTA = "\033[35m";

        // ============================================================================
        // REPLEngine Implementation / تنفيذ REPLEngine
        // ============================================================================

        REPLEngine::REPLEngine(const REPLConfig &config)
            : config_(config), state_(REPLState::Ready), bracketDepth_(0), lastWasEmpty_(false)
        {
            // Initialize color support on Windows / تفعيل دعم الألوان على ويندوز
#ifdef _WIN32
            if (config_.enableColor)
            {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                DWORD dwMode = 0;
                GetConsoleMode(hOut, &dwMode);
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
#endif

            // Initialize components / تهيئة المكونات
            if (config_.enableHistory)
            {
                history_ = std::make_unique<HistoryManager>(
                    config_.maxHistorySize,
                    config_.historyFile);
                history_->load();
            }

            commands_ = std::make_unique<REPLCommands>(this);

            // Initialize interpreter / تهيئة المفسر
            // (AR) م2-أ: تثبيت جسر الواجهات قبل المفسّر (عديم التكرار، يشمل مفسّر
            //      إعادة التعيين لأنّ التثبيت حالة عالميّة) كي يدعم REPL الرسومات.
            // (EN) Phase 2-A: install the UI bridge before the interpreter (idempotent,
            //      global — also covers the reset interpreter) so the REPL supports UI.
            Sad::Interpreter::installSadGraphicsBridge();

            Interpreter::InterpreterOptions interpOpts;
            interpOpts.enableDebugMode = false;
            interpOpts.printResults = false;
            interpreter_ = std::make_unique<Interpreter::Interpreter>(interpOpts);

            // (AR) DEF-002: نقطة استعداد لجسر سياسة الذاكرة (الآن no-op لعدم
            //      وجود أعلام CLI في REPL). راجع التعليق أعلى الملف.
            // (EN) DEF-002: ready-point for the policy bridge (no-op today).
            ::Sad::Memory::MemoryModeSettings replPolicy{};
            ::Sad::Memory::GC::applyMemoryPolicyGlobal(replPolicy,
                                                       /*policySet=*/false,
                                                       /*debugMode=*/false);

            // ErrorManager is a singleton, no need to initialize
            // errorManager_ = std::make_unique<Errors::ErrorManager>();
        }

        REPLEngine::~REPLEngine()
        {
            // Save history / حفظ التاريخ
            if (history_ && config_.enableHistory)
            {
                history_->save();
            }
        }

        int REPLEngine::run()
        {
            printHeader();

            while (state_ != REPLState::Exiting)
            {
                try
                {
                    printPrompt();
                    std::string line = readLine();

                    if (!processLine(line))
                    {
                        break;
                    }
                }
                catch (const std::exception &e)
                {
                    printError(std::string("خطأ داخلي / Internal error: ") + e.what());
                    state_ = REPLState::Ready;
                }
            }

            if (config_.enableColor)
            {
                std::cout << GREEN << "وداعاً! Goodbye!" << RESET << std::endl;
            }
            else
            {
                std::cout << "وداعاً! Goodbye!" << std::endl;
            }

            return 0;
        }

        std::string REPLEngine::evaluate(const std::string &line)
        {
            try
            {
                // Tokenize and Parse / التحليل المعجمي والنحوي
                Lexer::LexerCore lexer(line);
                Parser::ParserCore parser(lexer);
                auto ast = parser.parseProgram();

                if (ast.empty())
                {
                    return "";
                }

                // Check for parse errors / التحقق من أخطاء التحليل
                if (parser.hasErrors())
                {
                    std::string errors;
                    for (const auto &err : parser.getErrors())
                    {
                        if (!errors.empty())
                            errors += "\n";
                        errors += err;
                    }
                    return std::string("خطأ نحوي / Syntax Error: ") + errors;
                }

                // Execute via the persistent interpreter / تنفيذ عبر المفسر الدائم
                auto result = interpreter_->execute(ast);

                if (!result.success)
                {
                    return std::string("خطأ / Error: ") + result.errorMessage;
                }

                // Return result value if not void / إرجاع القيمة إن لم تكن فارغة
                if (result.result.getKind() != Types::SadTypeKind::Void)
                {
                    return result.result.toString();
                }

                return "";
            }
            catch (const std::exception &e)
            {
                return std::string("خطأ / Error: ") + e.what();
            }
        }

        std::string REPLEngine::evaluateMultiline(const std::vector<std::string> &lines)
        {
            // Combine lines / دمج الأسطر
            std::string combined;
            for (const auto &line : lines)
            {
                combined += line + "\n";
            }

            return evaluate(combined);
        }

        bool REPLEngine::loadFile(const std::string &filename)
        {
            std::ifstream file(filename);
            if (!file.is_open())
            {
                printError("فشل فتح الملف / Failed to open file: " + filename);
                return false;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            file.close();

            std::string result = evaluate(content);
            std::cout << result << std::endl;

            return true;
        }

        void REPLEngine::reset()
        {
            // Reset interpreter state / إعادة تعيين حالة المفسر
            Interpreter::InterpreterOptions interpOpts;
            interpOpts.enableDebugMode = false;
            interpOpts.printResults = false;
            interpreter_ = std::make_unique<Interpreter::Interpreter>(interpOpts);

            // (AR) DEF-002: نفس نقطة الاستعداد عند إعادة التهيئة.
            // (EN) DEF-002: same ready-point on reset.
            ::Sad::Memory::MemoryModeSettings replPolicy{};
            ::Sad::Memory::GC::applyMemoryPolicyGlobal(replPolicy,
                                                       /*policySet=*/false,
                                                       /*debugMode=*/false);

            // Clear multiline buffer / مسح المخزن متعدد الأسطر
            multilineBuffer_.clear();
            bracketDepth_ = 0;
            lastWasEmpty_ = false;
            state_ = REPLState::Ready;

            std::cout << "تم إعادة تعيين حالة REPL / REPL state reset." << std::endl;
        }

        bool REPLEngine::processLine(const std::string &line)
        {
            // Add to history / إضافة للتاريخ
            if (history_ && !line.empty() && state_ == REPLState::Ready)
            {
                history_->add(line);
            }

            // Check for special command / فحص الأوامر الخاصة
            if (commands_->isCommand(line))
            {
                return commands_->process(line);
            }

            // Handle multiline / معالجة متعدد الأسطر
            if (state_ == REPLState::MultiLine || isIncomplete(line))
            {
                multilineBuffer_.push_back(line);
                state_ = REPLState::MultiLine;

                // Check if complete / فحص إذا اكتمل
                if (line.empty() && lastWasEmpty_)
                {
                    // Two empty lines = execute / سطران فارغان = تنفيذ
                    std::string result = evaluateMultiline(multilineBuffer_);
                    if (config_.printResults)
                    {
                        std::cout << result << std::endl;
                    }
                    multilineBuffer_.clear();
                    state_ = REPLState::Ready;
                    lastWasEmpty_ = false;
                }
                else
                {
                    lastWasEmpty_ = line.empty();
                }

                return true;
            }

            // Normal code execution / تنفيذ عادي
            processCode(line);
            return true;
        }

        bool REPLEngine::processCommand(const std::string &command)
        {
            return commands_->process(command);
        }

        void REPLEngine::processCode(const std::string &code)
        {
            if (code.empty())
            {
                return;
            }

            state_ = REPLState::Executing;
            std::string result = evaluate(code);
            state_ = REPLState::Ready;

            if (config_.printResults && !result.empty())
            {
                std::cout << result << std::endl;
            }
        }

        bool REPLEngine::isIncomplete(const std::string &line)
        {
            // Count brackets / عد الأقواس
            int openBraces = 0;
            int openBrackets = 0;
            int openParens = 0;

            for (char c : line)
            {
                if (c == '{')
                    openBraces++;
                else if (c == '}')
                    openBraces--;
                else if (c == '[')
                    openBrackets++;
                else if (c == ']')
                    openBrackets--;
                else if (c == '(')
                    openParens++;
                else if (c == ')')
                    openParens--;
            }

            bracketDepth_ += openBraces + openBrackets + openParens;

            // Check for incomplete keywords / فحص الكلمات المفتاحية غير المكتملة
            std::string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t"));

            if (starts_with(trimmed, "دالة") || starts_with(trimmed, "function") ||
                starts_with(trimmed, "إذا") || starts_with(trimmed, "if") ||
                starts_with(trimmed, "لكل") || starts_with(trimmed, "for") ||
                starts_with(trimmed, "بينما") || starts_with(trimmed, "while") ||
                starts_with(trimmed, "صنف") || starts_with(trimmed, "class"))
            {
                return true;
            }

            return bracketDepth_ > 0;
        }

        void REPLEngine::printPrompt()
        {
            if (!config_.showPrompt)
                return;

            if (state_ == REPLState::MultiLine)
            {
                if (config_.enableColor)
                {
                    std::cout << YELLOW << config_.multilinePrompt << RESET;
                }
                else
                {
                    std::cout << config_.multilinePrompt;
                }
            }
            else
            {
                if (config_.enableColor)
                {
                    std::cout << CYAN << config_.promptSymbol << RESET;
                }
                else
                {
                    std::cout << config_.promptSymbol;
                }
            }
            std::cout.flush();
        }

        void REPLEngine::printResult(const Data::Value &result)
        {
            if (config_.enableColor)
            {
                std::cout << GREEN << result.toString() << RESET << std::endl;
            }
            else
            {
                std::cout << result.toString() << std::endl;
            }
        }

        void REPLEngine::printError(const std::string &message)
        {
            if (config_.enableColor)
            {
                std::cerr << RED << "✗ " << message << RESET << std::endl;
            }
            else
            {
                std::cerr << "✗ " << message << std::endl;
            }
        }

        void REPLEngine::printHeader()
        {
            if (config_.enableColor)
            {
                std::cout << BOLD << CYAN;
                std::cout << "========================================\n";
                std::cout << "لغة ص - REPL\n";
                std::cout << "Sad Language REPL\n";
                std::cout << "الإصدار / Version: 1.0.0\n";
                std::cout << "========================================\n";
                std::cout << RESET << std::endl;

                std::cout << "اكتب " << YELLOW << "':help'" << RESET
                          << " للمساعدة أو " << YELLOW << "':exit'" << RESET
                          << " للخروج\n";
                std::cout << "Type " << YELLOW << "':help'" << RESET
                          << " for help or " << YELLOW << "':exit'" << RESET
                          << " to quit\n"
                          << std::endl;
            }
            else
            {
                std::cout << "========================================\n";
                std::cout << "لغة ص - REPL\n";
                std::cout << "Sad Language REPL\n";
                std::cout << "الإصدار / Version: 1.0.0\n";
                std::cout << "========================================\n\n";

                std::cout << "اكتب ':help' للمساعدة أو ':exit' للخروج\n";
                std::cout << "Type ':help' for help or ':exit' to quit\n\n";
            }
        }

        std::string REPLEngine::readLine()
        {
            std::string line;
            std::getline(std::cin, line);

            // Check for EOF / فحص نهاية الملف
            if (std::cin.eof())
            {
                state_ = REPLState::Exiting;
                return "";
            }

            return line;
        }

    } // namespace REPL
} // namespace Sad
