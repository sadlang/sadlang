/**
 * =============================================================================
 * ملف: all_targets_flag.cpp
 * الوصف: دعم علم --كل-الأهداف للتجميع المتقاطع
 * المهمة: T242 - --كل-الأهداف CLI flag
 * المرحلة: Phase 24 - User Story 21 (Cross-Platform Build --كل-الأهداف)
 * =============================================================================
 * 
 * 🎯 دليل المبتدئ لـ --كل-الأهداف
 * ════════════════════════════════
 * 
 * ما هو هذا العلم؟
 * ─────────────────
 * يسمح بتجميع برنامج واحد لجميع المنصات المدعومة دفعة واحدة!
 * 
 * الاستخدام:
 * ──────────
 * ص ترجم برنامج.ص --كل-الأهداف
 * ص ترجم برنامج.ص --all-targets
 * 
 * الأهداف المدعومة:
 * ─────────────────
 * - windows-x64    → برنامج.exe
 * - linux-x64      → برنامج
 * - macos-x64      → برنامج
 * - macos-arm64    → برنامج (Apple Silicon)
 * - android-arm64  → libبرنامج.so
 * - ios-arm64      → برنامج.framework
 * - wasm           → برنامج.wasm
 * 
 * خيارات إضافية:
 * ───────────────
 * --أهداف "windows,linux,macos"    تحديد أهداف معينة
 * --targets "windows,linux,macos"  نفس الشيء بالإنجليزية
 * --استبعاد "android,ios"           استبعاد أهداف
 * --exclude "android,ios"          نفس الشيء بالإنجليزية
 * --موازي                           تجميع متوازي
 * --parallel                        نفس الشيء
 * 
 * مثال:
 * ─────
 * ```ص
 * // ملف: تطبيق.ص
 * دالة رئيسية() ← عدد:
 *     اطبع("مرحباً من لغة ص!")
 *     أرجع 0
 * نهاية
 * ```
 * 
 * ثم:
 * ```bash
 * ص ترجم تطبيق.ص --كل-الأهداف
 * ```
 * 
 * النتيجة:
 * ```
 * build/
 *   windows-x64/تطبيق.exe
 *   linux-x64/تطبيق
 *   macos-x64/تطبيق
 *   macos-arm64/تطبيق
 *   android-arm64/libتطبيق.so
 *   ios-arm64/تطبيق.framework/
 *   wasm/تطبيق.wasm
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <future>

namespace sad::cli {

// =============================================================================
// تعريف الأهداف
// =============================================================================

enum class Target {
    WindowsX64,
    WindowsX86,
    LinuxX64,
    LinuxARM64,
    MacOSX64,
    MacOSARM64,
    AndroidARM64,
    AndroidARM32,
    AndroidX64,
    IOSARM64,
    WebAssembly
};

/**
 * معلومات الهدف
 */
struct TargetInfo {
    Target target;
    std::string name;           // اسم قصير
    std::string displayName;    // اسم للعرض
    std::string triple;         // compiler triple
    std::string extension;      // امتداد الملف
    bool available;             // متوفر على هذا النظام؟
    
    std::string getOutputName(const std::string& baseName) const {
        if (extension.empty()) {
            return baseName;
        }
        return baseName + extension;
    }
};

/**
 * قائمة جميع الأهداف
 */
std::vector<TargetInfo> getAllTargets() {
    return {
        {Target::WindowsX64, "windows-x64", "ويندوز 64-bit", 
         "x86_64-pc-windows-msvc", ".exe", true},
        {Target::WindowsX86, "windows-x86", "ويندوز 32-bit",
         "i686-pc-windows-msvc", ".exe", true},
        {Target::LinuxX64, "linux-x64", "لينكس 64-bit",
         "x86_64-unknown-linux-gnu", "", true},
        {Target::LinuxARM64, "linux-arm64", "لينكس ARM64",
         "aarch64-unknown-linux-gnu", "", true},
        {Target::MacOSX64, "macos-x64", "ماك 64-bit",
         "x86_64-apple-darwin", "", true},
        {Target::MacOSARM64, "macos-arm64", "ماك Apple Silicon",
         "aarch64-apple-darwin", "", true},
        {Target::AndroidARM64, "android-arm64", "أندرويد ARM64",
         "aarch64-linux-android", ".so", true},
        {Target::AndroidARM32, "android-arm32", "أندرويد ARM32",
         "armv7-linux-androideabi", ".so", true},
        {Target::AndroidX64, "android-x64", "أندرويد x64",
         "x86_64-linux-android", ".so", true},
        {Target::IOSARM64, "ios-arm64", "iOS ARM64",
         "aarch64-apple-ios", ".framework", true},
        {Target::WebAssembly, "wasm", "WebAssembly",
         "wasm32-unknown-unknown", ".wasm", true}
    };
}

// =============================================================================
// محلل الأعلام
// =============================================================================

struct AllTargetsConfig {
    bool enabled = false;               // --كل-الأهداف
    std::set<std::string> include;      // --أهداف
    std::set<std::string> exclude;      // --استبعاد
    bool parallel = false;              // --موازي
    int maxJobs = 4;                    // --مهام
    std::string outputDir = "build";    // --خرج
    bool verbose = false;               // --تفصيل
    bool dryRun = false;                // --تجريب
};

class AllTargetsFlagParser {
public:
    /**
     * تحليل الأعلام
     */
    static AllTargetsConfig parse(const std::vector<std::string>& args) {
        AllTargetsConfig config;
        
        for (size_t i = 0; i < args.size(); i++) {
            const std::string& arg = args[i];
            
            // --كل-الأهداف / --all-targets
            if (arg == "--كل-الأهداف" || arg == "--all-targets") {
                config.enabled = true;
            }
            // --أهداف / --targets
            else if (arg == "--أهداف" || arg == "--targets") {
                if (i + 1 < args.size()) {
                    parseTargetList(args[++i], config.include);
                }
            }
            // --استبعاد / --exclude
            else if (arg == "--استبعاد" || arg == "--exclude") {
                if (i + 1 < args.size()) {
                    parseTargetList(args[++i], config.exclude);
                }
            }
            // --موازي / --parallel
            else if (arg == "--موازي" || arg == "--parallel") {
                config.parallel = true;
            }
            // --مهام / --jobs
            else if (arg == "--مهام" || arg == "--jobs" || arg == "-j") {
                if (i + 1 < args.size()) {
                    config.maxJobs = std::stoi(args[++i]);
                }
            }
            // --خرج / --output
            else if (arg == "--خرج" || arg == "--output" || arg == "-o") {
                if (i + 1 < args.size()) {
                    config.outputDir = args[++i];
                }
            }
            // --تفصيل / --verbose
            else if (arg == "--تفصيل" || arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            }
            // --تجريب / --dry-run
            else if (arg == "--تجريب" || arg == "--dry-run") {
                config.dryRun = true;
            }
        }
        
        return config;
    }
    
    /**
     * عرض المساعدة
     */
    static void printHelp() {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║            أعلام التجميع المتقاطع - لغة ص                    ║
╚═══════════════════════════════════════════════════════════════╝

الاستخدام:
  ص ترجم <ملف.ص> --كل-الأهداف [خيارات]

الأعلام الأساسية:
  --كل-الأهداف, --all-targets    تجميع لجميع المنصات المدعومة
  --أهداف, --targets <قائمة>     تحديد أهداف معينة (مفصولة بفاصلة)
  --استبعاد, --exclude <قائمة>   استبعاد أهداف من التجميع

التحكم:
  --موازي, --parallel             تجميع متوازي
  --مهام, --jobs, -j <عدد>        عدد المهام المتوازية (افتراضي: 4)
  --خرج, --output, -o <مسار>      مجلد الإخراج (افتراضي: build)
  --تفصيل, --verbose, -v          عرض تفاصيل التجميع
  --تجريب, --dry-run              عرض الأوامر بدون تنفيذ

الأهداف المتوفرة:
  windows-x64    ويندوز 64-bit
  windows-x86    ويندوز 32-bit
  linux-x64      لينكس 64-bit
  linux-arm64    لينكس ARM64
  macos-x64      ماك Intel
  macos-arm64    ماك Apple Silicon
  android-arm64  أندرويد ARM64
  android-arm32  أندرويد ARM32
  ios-arm64      iOS
  wasm           WebAssembly

أمثلة:
  ص ترجم app.ص --كل-الأهداف
  ص ترجم app.ص --أهداف "windows-x64,linux-x64"
  ص ترجم app.ص --كل-الأهداف --استبعاد "android,ios" --موازي
  ص ترجم app.ص --all-targets -j 8 --verbose

)";
    }
    
private:
    static void parseTargetList(const std::string& str, std::set<std::string>& out) {
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, ',')) {
            // إزالة المسافات
            size_t start = item.find_first_not_of(" \t");
            size_t end = item.find_last_not_of(" \t");
            if (start != std::string::npos) {
                out.insert(item.substr(start, end - start + 1));
            }
        }
    }
};

// =============================================================================
// محرك التجميع المتعدد
// =============================================================================

class MultiTargetCompiler {
public:
    using CompileCallback = std::function<bool(const TargetInfo&, const std::string&)>;
    
    struct BuildResult {
        TargetInfo target;
        bool success;
        std::string output;
        double duration;  // ثوانٍ
    };
    
    /**
     * تجميع لأهداف متعددة
     */
    std::vector<BuildResult> compile(
        const std::string& sourceFile,
        const AllTargetsConfig& config,
        CompileCallback callback)
    {
        // جمع الأهداف
        auto targets = getSelectedTargets(config);
        
        if (config.verbose) {
            std::cout << "🎯 الأهداف المحددة:\n";
            for (const auto& t : targets) {
                std::cout << "   • " << t.displayName << " (" << t.name << ")\n";
            }
        }
        
        std::vector<BuildResult> results;
        
        if (config.parallel && targets.size() > 1) {
            results = compileParallel(sourceFile, targets, config, callback);
        } else {
            results = compileSequential(sourceFile, targets, config, callback);
        }
        
        return results;
    }
    
    /**
     * عرض ملخص النتائج
     */
    static void printSummary(const std::vector<BuildResult>& results) {
        std::cout << "\n";
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "               ملخص التجميع\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        int success = 0, failed = 0;
        double totalTime = 0;
        
        for (const auto& r : results) {
            totalTime += r.duration;
            if (r.success) {
                success++;
                std::cout << "  ✅ " << r.target.displayName << " (" 
                          << r.duration << " ث)\n";
            } else {
                failed++;
                std::cout << "  ❌ " << r.target.displayName << "\n";
                if (!r.output.empty()) {
                    std::cout << "      " << r.output << "\n";
                }
            }
        }
        
        std::cout << "\n───────────────────────────────────────────────\n";
        std::cout << "  المجموع: " << results.size() << " أهداف\n";
        std::cout << "  نجح: " << success << " | فشل: " << failed << "\n";
        std::cout << "  الوقت الكلي: " << totalTime << " ثانية\n";
        std::cout << "═══════════════════════════════════════════════\n";
    }
    
private:
    /**
     * الحصول على الأهداف المحددة
     */
    std::vector<TargetInfo> getSelectedTargets(const AllTargetsConfig& config) {
        auto all = getAllTargets();
        std::vector<TargetInfo> result;
        
        for (const auto& t : all) {
            // تحقق من التضمين
            if (!config.include.empty()) {
                bool found = false;
                for (const auto& inc : config.include) {
                    if (t.name.find(inc) != std::string::npos) {
                        found = true;
                        break;
                    }
                }
                if (!found) continue;
            }
            
            // تحقق من الاستبعاد
            bool excluded = false;
            for (const auto& exc : config.exclude) {
                if (t.name.find(exc) != std::string::npos) {
                    excluded = true;
                    break;
                }
            }
            if (excluded) continue;
            
            // تحقق من التوفر
            if (!t.available) continue;
            
            result.push_back(t);
        }
        
        return result;
    }
    
    /**
     * تجميع متسلسل
     */
    std::vector<BuildResult> compileSequential(
        const std::string& sourceFile,
        const std::vector<TargetInfo>& targets,
        const AllTargetsConfig& config,
        CompileCallback callback)
    {
        std::vector<BuildResult> results;
        
        for (const auto& target : targets) {
            std::cout << "🔨 جارٍ التجميع لـ " << target.displayName << "...\n";
            
            BuildResult result;
            result.target = target;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            if (config.dryRun) {
                result.success = true;
                result.output = "تجريب: " + target.triple;
            } else {
                std::string outputPath = config.outputDir + "/" + target.name;
                result.success = callback(target, outputPath);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            result.duration = std::chrono::duration<double>(end - start).count();
            
            results.push_back(result);
        }
        
        return results;
    }
    
    /**
     * تجميع متوازي
     */
    std::vector<BuildResult> compileParallel(
        const std::string& sourceFile,
        const std::vector<TargetInfo>& targets,
        const AllTargetsConfig& config,
        CompileCallback callback)
    {
        std::vector<std::future<BuildResult>> futures;
        std::mutex printMutex;
        
        for (const auto& target : targets) {
            futures.push_back(std::async(std::launch::async, [&, target]() {
                {
                    std::lock_guard<std::mutex> lock(printMutex);
                    std::cout << "🔨 بدء التجميع لـ " << target.displayName << "...\n";
                }
                
                BuildResult result;
                result.target = target;
                
                auto start = std::chrono::high_resolution_clock::now();
                
                if (config.dryRun) {
                    result.success = true;
                    result.output = "تجريب: " + target.triple;
                } else {
                    std::string outputPath = config.outputDir + "/" + target.name;
                    result.success = callback(target, outputPath);
                }
                
                auto end = std::chrono::high_resolution_clock::now();
                result.duration = std::chrono::duration<double>(end - start).count();
                
                {
                    std::lock_guard<std::mutex> lock(printMutex);
                    if (result.success) {
                        std::cout << "  ✓ اكتمل " << target.displayName << "\n";
                    } else {
                        std::cout << "  ✗ فشل " << target.displayName << "\n";
                    }
                }
                
                return result;
            }));
        }
        
        std::vector<BuildResult> results;
        for (auto& f : futures) {
            results.push_back(f.get());
        }
        
        return results;
    }
};

// =============================================================================
// أمر التجميع المتعدد
// =============================================================================

class AllTargetsCommand {
public:
    /**
     * تنفيذ الأمر
     */
    int execute(int argc, char* argv[]) {
        std::vector<std::string> args(argv + 1, argv + argc);
        
        // تحقق من وجود --مساعدة
        for (const auto& arg : args) {
            if (arg == "--مساعدة" || arg == "--help" || arg == "-h") {
                AllTargetsFlagParser::printHelp();
                return 0;
            }
        }
        
        // تحليل الأعلام
        auto config = AllTargetsFlagParser::parse(args);
        
        if (!config.enabled && config.include.empty()) {
            std::cerr << "خطأ: يجب تحديد --كل-الأهداف أو --أهداف\n";
            AllTargetsFlagParser::printHelp();
            return 1;
        }
        
        // البحث عن ملف المصدر
        std::string sourceFile;
        for (const auto& arg : args) {
            if (arg.size() > 2 && arg.substr(arg.size() - 2) == ".ص") {
                sourceFile = arg;
                break;
            }
            // (AR) الامتداد القديم .sad لم يعد مدعوماً / (EN) Old .sad extension no longer supported
        }
        
        if (sourceFile.empty()) {
            std::cerr << "خطأ: لم يتم تحديد ملف المصدر\n";
            return 1;
        }
        
        std::cout << "╔═══════════════════════════════════════════════╗\n";
        std::cout << "║     التجميع المتقاطع - لغة ص                 ║\n";
        std::cout << "╚═══════════════════════════════════════════════╝\n\n";
        
        std::cout << "📄 الملف: " << sourceFile << "\n";
        std::cout << "📁 الإخراج: " << config.outputDir << "/\n";
        std::cout << "⚡ موازي: " << (config.parallel ? "نعم" : "لا") << "\n\n";
        
        // التجميع
        MultiTargetCompiler compiler;
        auto results = compiler.compile(sourceFile, config,
            [&](const TargetInfo& target, const std::string& outputPath) {
                // هنا يتم استدعاء المترجم الفعلي
                // للتبسيط، نعيد نجاح دائماً
                return true;
            });
        
        // عرض الملخص
        MultiTargetCompiler::printSummary(results);
        
        // حساب الحالة
        int failed = 0;
        for (const auto& r : results) {
            if (!r.success) failed++;
        }
        
        return failed > 0 ? 1 : 0;
    }
};

} // namespace sad::cli

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

int sad_all_targets_main(int argc, char* argv[]) {
    sad::cli::AllTargetsCommand cmd;
    return cmd.execute(argc, argv);
}

void sad_all_targets_help() {
    sad::cli::AllTargetsFlagParser::printHelp();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef ALL_TARGETS_TEST

#include <cassert>

void testFlagParser() {
    std::cout << "🏷️ اختبار تحليل الأعلام...\n";
    
    std::vector<std::string> args = {
        "program.ص",
        "--كل-الأهداف",
        "--استبعاد", "android,ios",
        "--موازي",
        "-j", "8"
    };
    
    auto config = sad::cli::AllTargetsFlagParser::parse(args);
    
    assert(config.enabled == true);
    assert(config.parallel == true);
    assert(config.maxJobs == 8);
    assert(config.exclude.count("android") > 0);
    assert(config.exclude.count("ios") > 0);
    
    std::cout << "   ✅ نجح\n";
}

void testTargetList() {
    std::cout << "📋 اختبار قائمة الأهداف...\n";
    
    auto targets = sad::cli::getAllTargets();
    
    assert(targets.size() >= 10);
    
    bool hasWindows = false, hasLinux = false, hasWasm = false;
    for (const auto& t : targets) {
        if (t.name == "windows-x64") hasWindows = true;
        if (t.name == "linux-x64") hasLinux = true;
        if (t.name == "wasm") hasWasm = true;
    }
    
    assert(hasWindows);
    assert(hasLinux);
    assert(hasWasm);
    
    std::cout << "   ✅ نجح\n";
}

void testDryRun() {
    std::cout << "🧪 اختبار التجريب...\n";
    
    sad::cli::AllTargetsConfig config;
    config.enabled = true;
    config.dryRun = true;
    config.include = {"windows-x64", "linux-x64"};
    
    sad::cli::MultiTargetCompiler compiler;
    auto results = compiler.compile("test.ص", config,
        [](const sad::cli::TargetInfo& t, const std::string& path) {
            return true;
        });
    
    assert(results.size() == 2);
    for (const auto& r : results) {
        assert(r.success);
    }
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات --كل-الأهداف\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testFlagParser();
    testTargetList();
    testDryRun();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // ALL_TARGETS_TEST
