/**
 * @file main.cpp
 * @brief (AR) نقطة الدخول الرئيسية لأداة مصحح الأداء — sad-profiler
 * @brief (EN) Main entry point for the profiler tool — sad-profiler
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * الوصف بالعربية:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا هو الملف الرئيسي لأداة مصحح الأداء (sad-profiler). هذه الأداة تُشغّل
 * برنامج بلغة ص مع جمع بيانات الأداء — تقيس زمن كل دالة، وعدد مرات 
 * استدعائها، وتكتشف نقاط الاختناق.
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * طريقة الاستخدام:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *   sad-profiler <ملف.ص> [خيارات]
 * 
 * الخيارات:
 *   --صيغة=نص|json|html|لهب|csv    صيغة التقرير (الافتراضي: نص)
 *   --format=text|json|html|flame|csv  Report format (default: text)
 *   --ملف=<مسار>                     حفظ التقرير في ملف
 *   --output=<path>                    Save report to file
 *   --عدد=<ن>                         عدد الدوال الأعلى (الافتراضي: 20)
 *   --top=<n>                          Top functions count (default: 20)
 *   --عتبة=<مللي>                     عتبة العرض بالمللي ثانية
 *   --threshold=<ms>                   Display threshold in ms
 *   --بدون-ألوان                      إخراج بدون ألوان
 *   --no-color                        No color output
 *   --بدون-شجرة                       بدون شجرة الاستدعاءات
 *   --no-tree                         No call tree
 *   --بدون-ذاكرة                      بدون تتبع الذاكرة
 *   --no-memory                       No memory tracking
 *   --مساعدة                          عرض المساعدة
 *   --help                            Show help
 * 
 * أمثلة:
 *   sad-profiler برنامج.ص
 *   sad-profiler برنامج.ص --صيغة=html --ملف=تقرير.html
 *   sad-profiler برنامج.ص --صيغة=json --عدد=50
 *   sad-profiler برنامج.ص --عتبة=0.1 --بدون-ذاكرة
 * 
 * @author فريق تطوير لغة ص
 * @date 2025
 */

#include "profiler_core.h"
#include "profiler_hooks.h"

// (AR) مكونات المفسر / (EN) Interpreter components
#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"
#include "value.h"
#include "../../interpreter_new/include/exception.h"
#include "../../shared/utils/include/utf8_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <clocale>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) طباعة شعار الأداة / (EN) Print tool banner
// ═══════════════════════════════════════════════════════════════════════════════
static void printBanner() {
    std::cout << "\033[96m\033[1m"
              << "╔═══════════════════════════════════════════════════╗\n"
              << "║  📊 مصحح أداء لغة ص — sad-profiler v1.2.0      ║\n"
              << "║     أداة تحليل أداء برامج لغة ص البرمجية        ║\n"
              << "╚═══════════════════════════════════════════════════╝\n"
              << "\033[0m" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) طباعة المساعدة / (EN) Print help
// ═══════════════════════════════════════════════════════════════════════════════
static void printHelp(const char* progName) {
    printBanner();
    std::cout << "الاستخدام / Usage:\n"
              << "  " << progName << " <ملف.ص> [خيارات]\n\n"
              << "الخيارات / Options:\n"
              << "  --صيغة=<نوع>     صيغة التقرير: نص, json, html, لهب, csv\n"
              << "  --format=<type>   Report format: text, json, html, flame, csv\n"
              << "  --ملف=<مسار>     حفظ التقرير في ملف\n"
              << "  --output=<path>   Save report to file\n"
              << "  --عدد=<ن>        عدد الدوال المعروضة (افتراضي: 20)\n"
              << "  --top=<n>        Top functions to show (default: 20)\n"
              << "  --عتبة=<مللي>    عتبة العرض بالمللي ثانية\n"
              << "  --threshold=<ms>  Display threshold in ms\n"
              << "  --بدون-ألوان     إخراج بدون ألوان ANSI\n"
              << "  --no-color       No ANSI color output\n"
              << "  --بدون-شجرة      بدون شجرة الاستدعاءات\n"
              << "  --no-tree        No call tree\n"
              << "  --بدون-ذاكرة     بدون تتبع الذاكرة\n"
              << "  --no-memory      No memory tracking\n"
              << "  --بدون-مدمجة     استبعاد الدوال المدمجة\n"
              << "  --exclude-builtins  Exclude built-in functions\n"
              << "  --تصفية=<نمط>    تصفية بنمط اسم الدالة\n"
              << "  --filter=<pattern>  Filter by function name pattern\n"
              << "  --عمق-شجرة=<ن>   أقصى عمق لشجرة الاستدعاءات\n"
              << "  --max-depth=<n>   Max call tree display depth\n"
              << "  --bom             إضافة UTF-8 BOM لملفات الإخراج\n"
              << "  --عينات=<ن>      معدل أخذ العينات (1=كل استدعاء)\n"
              << "  --sampling=<n>    Sampling rate (1=every call)\n"
              << "  --توزيع          عرض تقرير التوزيع الزمني\n"
              << "  --distribution    Show time distribution report\n"
              << "  --ساخن-بارد       عرض تقرير الدوال الساخنة/الباردة\n"
              << "  --hot-cold        Show hot/cold functions report\n"
              << "  --تفاصيل=<دالة>  عرض تفاصيل دالة محددة\n"
              << "  --detail=<func>   Show details for specific function\n"
              << "  --مقارنة=<د1,د2> مقارنة بين دالتين\n"
              << "  --compare=<f1,f2> Compare two functions\n"
              << "  --ملخص           ملخص مختصر (سطر واحد)\n"
              << "  --summary         One-line summary\n"
              << "  --نسخة           عرض رقم الإصدار\n"
              << "  --version         Show version number\n"
              << "  --مساعدة         عرض هذه المساعدة\n"
              << "  --help           Show this help\n"
              << std::endl;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) قراءة ملف مصدري / (EN) Read source file
// ═══════════════════════════════════════════════════════════════════════════════
static std::string readFile(const std::string& filename) {
    auto file = sad::utf8::open_ifstream(filename);
    if (!file.is_open()) {
        throw std::runtime_error("لم يتم العثور على الملف: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تحليل صيغة التقرير / (EN) Parse report format
// ═══════════════════════════════════════════════════════════════════════════════
static Sad::Tools::ReportFormat parseFormat(const std::string& fmt) {
    if (fmt == "نص" || fmt == "text" || fmt == "txt") return Sad::Tools::ReportFormat::TEXT;
    if (fmt == "json") return Sad::Tools::ReportFormat::JSON;
    if (fmt == "html") return Sad::Tools::ReportFormat::HTML;
    if (fmt == "لهب" || fmt == "flame" || fmt == "flamegraph") return Sad::Tools::ReportFormat::FLAMEGRAPH;
    if (fmt == "csv") return Sad::Tools::ReportFormat::CSV;
    
    std::cerr << "⚠️ صيغة غير معروفة: " << fmt << " — استخدام الصيغة النصية\n";
    return Sad::Tools::ReportFormat::TEXT;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) استخراج قيمة من خيار (--key=value)
// (EN) Extract value from option (--key=value)
// ═══════════════════════════════════════════════════════════════════════════════
static std::string extractOptionValue(const std::string& arg) {
    auto pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) الدالة الرئيسية / (EN) Main function
// ═══════════════════════════════════════════════════════════════════════════════
int main(int argc, char* argv[]) {
    // (AR) إعداد UTF-8 / (EN) Setup UTF-8
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");
    auto utf8_args = sad::utf8::get_utf8_args();
    std::vector<char*> new_argv;
    for (auto& s : utf8_args) new_argv.push_back(s.data());
    argc = static_cast<int>(new_argv.size());
    argv = new_argv.data();
#else
    std::setlocale(LC_ALL, "");
#endif
    
    // (AR) فحص المعاملات / (EN) Check arguments
    if (argc < 2) {
        printHelp(argv[0]);
        return 1;
    }
    
    // (AR) تحليل الخيارات / (EN) Parse options
    Sad::Tools::ProfilerOptions profOptions;
    profOptions.enabled = true;
    profOptions.colorOutput = true;
    profOptions.arabicOutput = true;
    profOptions.trackCallTree = true;
    profOptions.trackMemory = true;
    profOptions.topFunctionsCount = 20;
    
    std::string filename;
    std::string outputFile;
    std::string detailFunc;          // (AR) دالة لعرض تفاصيلها / (EN) Function to show details for
    std::string compareArg;          // (AR) مقارنة بين دالتين / (EN) Compare two functions
    bool showDistribution = false;   // (AR) عرض تقرير التوزيع / (EN) Show distribution report
    bool showHotCold = false;        // (AR) عرض ساخن/بارد / (EN) Show hot/cold report
    bool showSummary = false;        // (AR) ملخص مختصر / (EN) One-line summary
    Sad::Tools::ReportFormat format = Sad::Tools::ReportFormat::TEXT;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // (AR) مساعدة / (EN) Help
        if (arg == "--help" || arg == "-h" || arg == "--مساعدة") {
            printHelp(argv[0]);
            return 0;
        }
        
        // (AR) صيغة التقرير / (EN) Report format
        if (arg.find("--صيغة=") == 0 || arg.find("--format=") == 0) {
            format = parseFormat(extractOptionValue(arg));
            continue;
        }
        
        // (AR) ملف الإخراج / (EN) Output file
        if (arg.find("--ملف=") == 0 || arg.find("--output=") == 0) {
            outputFile = extractOptionValue(arg);
            continue;
        }
        
        // (AR) عدد الدوال / (EN) Top functions count
        if (arg.find("--عدد=") == 0 || arg.find("--top=") == 0) {
            try {
                profOptions.topFunctionsCount = std::stoul(extractOptionValue(arg));
            } catch (...) {}
            continue;
        }
        
        // (AR) عتبة العرض / (EN) Display threshold
        if (arg.find("--عتبة=") == 0 || arg.find("--threshold=") == 0) {
            try {
                profOptions.thresholdMs = std::stod(extractOptionValue(arg));
            } catch (...) {}
            continue;
        }
        
        // (AR) بدون ألوان / (EN) No color
        if (arg == "--بدون-ألوان" || arg == "--no-color") {
            profOptions.colorOutput = false;
            continue;
        }
        
        // (AR) بدون شجرة / (EN) No tree
        if (arg == "--بدون-شجرة" || arg == "--no-tree") {
            profOptions.trackCallTree = false;
            continue;
        }
        
        // (AR) بدون ذاكرة / (EN) No memory
        if (arg == "--بدون-ذاكرة" || arg == "--no-memory") {
            profOptions.trackMemory = false;
            continue;
        }
        
        // (AR) استبعاد المدمجة / (EN) Exclude builtins
        if (arg == "--بدون-مدمجة" || arg == "--exclude-builtins") {
            profOptions.excludeBuiltins = true;
            continue;
        }
        
        // (AR) BOM / (EN) UTF-8 BOM
        if (arg == "--bom") {
            profOptions.utfBom = true;
            continue;
        }
        
        // (AR) تصفية / (EN) Filter
        if (arg.find("--تصفية=") == 0 || arg.find("--filter=") == 0) {
            profOptions.filterPattern = extractOptionValue(arg);
            continue;
        }
        
        // (AR) عمق الشجرة / (EN) Max tree depth
        if (arg.find("--عمق-شجرة=") == 0 || arg.find("--max-depth=") == 0) {
            try {
                profOptions.maxTreeDepth = std::stoul(extractOptionValue(arg));
            } catch (...) {}
            continue;
        }
        
        // (AR) معدل أخذ العينات / (EN) Sampling rate
        if (arg.find("--عينات=") == 0 || arg.find("--sampling=") == 0) {
            try {
                profOptions.samplingRate = std::stoul(extractOptionValue(arg));
                if (profOptions.samplingRate < 1) profOptions.samplingRate = 1;
            } catch (...) {}
            continue;
        }
        
        // (AR) تقرير التوزيع / (EN) Distribution report
        if (arg == "--توزيع" || arg == "--distribution") {
            showDistribution = true;
            continue;
        }
        
        // (AR) تقرير ساخن/بارد / (EN) Hot/cold report
        if (arg == "--ساخن-بارد" || arg == "--hot-cold") {
            showHotCold = true;
            continue;
        }
        
        // (AR) تفاصيل دالة / (EN) Function detail
        if (arg.find("--تفاصيل=") == 0 || arg.find("--detail=") == 0) {
            detailFunc = extractOptionValue(arg);
            continue;
        }
        
        // (AR) مقارنة بين دالتين / (EN) Compare two functions
        if (arg.find("--مقارنة=") == 0 || arg.find("--compare=") == 0) {
            compareArg = extractOptionValue(arg);
            continue;
        }
        
        // (AR) ملخص مختصر / (EN) One-line summary
        if (arg == "--ملخص" || arg == "--summary") {
            showSummary = true;
            continue;
        }
        
        // (AR) الإصدار / (EN) Version
        if (arg == "--نسخة" || arg == "--version") {
            std::cout << "sad-profiler v1.2.0\n";
            return 0;
        }
        
        // (AR) اسم الملف / (EN) Filename
        if (arg[0] != '-') {
            filename = arg;
        }
    }
    
    if (filename.empty()) {
        std::cerr << "❌ خطأ: لم يتم تحديد ملف .ص\n";
        printHelp(argv[0]);
        return 1;
    }
    
    profOptions.format = format;
    profOptions.outputFile = outputFile;
    
    try {
        // ═══════════════════════════════════════════════════════════════
        // (AR) المرحلة 1: قراءة وتحليل الملف المصدري
        // (EN) Phase 1: Read and parse source file
        // ═══════════════════════════════════════════════════════════════
        printBanner();
        std::cout << "📂 قراءة الملف: " << filename << "\n";
        
        std::string source = readFile(filename);
        
        Sad::Errors::ErrorManager::getInstance().clear();
        Sad::Errors::ErrorManager::getInstance().setSourceCode(source, filename);
        
        // (AR) التحليل المعجمي والنحوي / (EN) Lexing and parsing
        Sad::Lexer::LexerCore lexer(source);
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        
        if (parser.hasErrors()) {
            std::cerr << "❌ أخطاء في التحليل النحوي:\n";
            parser.printErrors();
            return 1;
        }
        
        std::cout << "✓ تم تحليل " << program.size() << " جملة بنجاح\n";
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) المرحلة 2: إعداد المفسر والمصحح
        // (EN) Phase 2: Setup interpreter and profiler
        // ═══════════════════════════════════════════════════════════════
        
        Sad::Tools::ProfilerCore profiler(profOptions);
        Sad::Tools::setGlobalProfiler(&profiler);
        
        Sad::Interpreter::InterpreterOptions interpOptions;
        interpOptions.enableDebugMode = false;
        interpOptions.printResults = false;
        interpOptions.currentFilePath = filename;
        
        Sad::Interpreter::Interpreter interpreter(interpOptions);
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) المرحلة 3: التنفيذ مع جمع البيانات
        // (EN) Phase 3: Execute with data collection
        // ═══════════════════════════════════════════════════════════════
        std::cout << "🚀 بدء التنفيذ مع جمع بيانات الأداء...\n\n";
        
        profiler.start();
        auto result = interpreter.execute(program);
        profiler.stop();
        
        std::cout << "\n";
        
        if (!result.success) {
            std::cerr << "⚠️ انتهى البرنامج بخطأ: " << result.errorMessage << "\n";
            std::cerr << "   (سيتم عرض بيانات الأداء المُجمعة حتى نقطة الخطأ)\n\n";
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) المرحلة 4: إنتاج التقرير
        // (EN) Phase 4: Generate report
        // ═══════════════════════════════════════════════════════════════
        std::string report = profiler.generateReport(format);
        
        if (!outputFile.empty()) {
            // (AR) حفظ في ملف / (EN) Save to file
            if (profiler.saveReport(outputFile, format)) {
                std::cout << "✓ تم حفظ التقرير في: " << outputFile << "\n";
            } else {
                std::cerr << "❌ فشل حفظ التقرير في: " << outputFile << "\n";
                // (AR) طباعة على الشاشة كبديل / (EN) Print to screen as fallback
                std::cout << report;
            }
        } else {
            // (AR) طباعة على الشاشة / (EN) Print to screen
            if (showSummary) {
                std::cout << profiler.generateOneLinerSummary() << "\n";
            } else {
                std::cout << report;
            }
        }
        
        // (AR) تقارير إضافية / (EN) Additional reports
        if (showDistribution) {
            std::cout << "\n" << profiler.generateTimeDistributionReport();
        }
        if (showHotCold) {
            std::cout << "\n" << profiler.generateHotColdReport();
        }
        if (!detailFunc.empty()) {
            std::cout << "\n" << profiler.generateFunctionDetail(detailFunc);
        }
        if (!compareArg.empty()) {
            auto commaPos = compareArg.find(',');
            if (commaPos != std::string::npos) {
                std::string f1 = compareArg.substr(0, commaPos);
                std::string f2 = compareArg.substr(commaPos + 1);
                std::cout << "\n" << profiler.generateComparisonReport(f1, f2);
            } else {
                std::cerr << "⚠️ صيغة المقارنة: --مقارنة=دالة1,دالة2\n";
            }
        }
        
        // (AR) تنظيف / (EN) Cleanup
        Sad::Tools::setGlobalProfiler(nullptr);
        
        return result.success ? 0 : 1;
        
    } catch (const Sad::Interpreter::ExitException& exitEx) {
        return exitEx.getExitCode();
    } catch (const std::exception& e) {
        std::cerr << "❌ خطأ غير متوقع: " << e.what() << "\n";
        return 1;
    }
}
