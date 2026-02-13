/**
 * =============================================================================
 * ملف: property_flag.cpp
 * الوصف: علم CLI لتشغيل اختبارات الخصائص
 * المهمة: T250 - `ص اختبار خصائص` CLI flag
 * المرحلة: Phase 25 - User Story 22 (Property Testing)
 * =============================================================================
 * 
 * 🚩 دليل المبتدئ لأوامر اختبار الخصائص
 * ════════════════════════════════════════
 * 
 * ما هو هذا الملف؟
 * ────────────────
 * يضيف أوامر خاصة لتشغيل اختبارات الخصائص من سطر الأوامر.
 * 
 * الأوامر المتاحة:
 * ────────────────
 * ```bash
 * # تشغيل كل اختبارات الخصائص
 * ص اختبار خصائص
 * 
 * # اختبار ملف معين
 * ص اختبار خصائص ملفي.ص
 * 
 * # اختبار دالة معينة
 * ص اختبار خصائص --دالة اختبار_الجمع
 * 
 * # تحديد عدد التكرارات
 * ص اختبار خصائص --تكرارات 1000
 * 
 * # استخدام بذرة معينة (للإعادة)
 * ص اختبار خصائص --بذرة 123456
 * 
 * # تعطيل التقليص
 * ص اختبار خصائص --بدون-تقليص
 * 
 * # الإخراج المفصل
 * ص اختبار خصائص --مفصل
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <functional>
#include <iostream>
#include <filesystem>

namespace sad::testing {

// =============================================================================
// خيارات الأمر
// =============================================================================

/**
 * خيارات تشغيل اختبار الخصائص
 */
struct PropertyTestOptions {
    // الملفات والدوال
    std::vector<std::string> files;
    std::optional<std::string> function;
    std::string pattern;  // نمط للمطابقة
    
    // إعدادات التنفيذ
    int iterations = 100;
    std::optional<uint64_t> seed;
    int maxSize = 100;
    bool shrinkEnabled = true;
    int shrinkAttempts = 100;
    int timeout = 30;  // ثوانٍ
    
    // الإخراج
    bool verbose = false;
    bool quiet = false;
    bool json = false;
    std::string outputFile;
    
    // الإعادة
    std::string reproduceFile;
    bool listFailures = false;
    bool cleanupFailures = false;
    
    // متفرقات
    bool help = false;
    bool version = false;
    int workers = 1;  // عدد العمال للتوازي
};

// =============================================================================
// محلل الأوامر
// =============================================================================

class PropertyFlagParser {
public:
    PropertyTestOptions parse(int argc, char* argv[]) {
        PropertyTestOptions opts;
        
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            // خيارات طويلة (عربية)
            if (arg == "--تكرارات" || arg == "--iterations") {
                if (i + 1 < argc) {
                    opts.iterations = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--بذرة" || arg == "--seed") {
                if (i + 1 < argc) {
                    opts.seed = std::stoull(argv[++i]);
                }
            }
            else if (arg == "--دالة" || arg == "--function") {
                if (i + 1 < argc) {
                    opts.function = argv[++i];
                }
            }
            else if (arg == "--نمط" || arg == "--pattern") {
                if (i + 1 < argc) {
                    opts.pattern = argv[++i];
                }
            }
            else if (arg == "--حجم-أقصى" || arg == "--max-size") {
                if (i + 1 < argc) {
                    opts.maxSize = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--محاولات-تقليص" || arg == "--shrink-attempts") {
                if (i + 1 < argc) {
                    opts.shrinkAttempts = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--مهلة" || arg == "--timeout") {
                if (i + 1 < argc) {
                    opts.timeout = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--عمال" || arg == "--workers") {
                if (i + 1 < argc) {
                    opts.workers = std::stoi(argv[++i]);
                }
            }
            else if (arg == "--إخراج" || arg == "--output") {
                if (i + 1 < argc) {
                    opts.outputFile = argv[++i];
                }
            }
            else if (arg == "--إعادة" || arg == "--reproduce") {
                if (i + 1 < argc) {
                    opts.reproduceFile = argv[++i];
                }
            }
            // أعلام بسيطة
            else if (arg == "--بدون-تقليص" || arg == "--no-shrink") {
                opts.shrinkEnabled = false;
            }
            else if (arg == "--مفصل" || arg == "-v" || arg == "--verbose") {
                opts.verbose = true;
            }
            else if (arg == "--صامت" || arg == "-q" || arg == "--quiet") {
                opts.quiet = true;
            }
            else if (arg == "--json") {
                opts.json = true;
            }
            else if (arg == "--قائمة-فشل" || arg == "--list-failures") {
                opts.listFailures = true;
            }
            else if (arg == "--تنظيف" || arg == "--cleanup") {
                opts.cleanupFailures = true;
            }
            else if (arg == "--مساعدة" || arg == "-h" || arg == "--help") {
                opts.help = true;
            }
            else if (arg == "--إصدار" || arg == "--version") {
                opts.version = true;
            }
            // ملفات
            else if (!arg.empty() && arg[0] != '-') {
                opts.files.push_back(arg);
            }
        }
        
        return opts;
    }
};

// =============================================================================
// مساعد الأوامر
// =============================================================================

class PropertyTestHelp {
public:
    void printHelp() {
        std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                    اختبار الخصائص - المساعدة                                 ║
╚══════════════════════════════════════════════════════════════════════════════╝

الاستخدام:
    ص اختبار خصائص [ملفات...] [خيارات]

الوصف:
    تشغيل اختبارات الخصائص المعرّفة بـ #[اختبار_خاصية]

أمثلة:
    ص اختبار خصائص                      # كل الاختبارات
    ص اختبار خصائص src/                 # اختبارات في مجلد
    ص اختبار خصائص ملف.ص               # اختبارات ملف معين
    ص اختبار خصائص --دالة اختبار_جمع  # دالة معينة

═══════════════════════════════════════════════════════════════════════════════
                              الخيارات
═══════════════════════════════════════════════════════════════════════════════

الملفات والدوال:
    --دالة, --function <اسم>      اختبار دالة معينة فقط
    --نمط, --pattern <نص>         مطابقة أسماء الاختبارات بنمط

إعدادات التنفيذ:
    --تكرارات, --iterations <ن>   عدد التكرارات (افتراضي: 100)
    --بذرة, --seed <ن>            بذرة العشوائية (للإعادة)
    --حجم-أقصى, --max-size <ن>    الحجم الأقصى للقيم المولدة (افتراضي: 100)
    --مهلة, --timeout <ث>          مهلة لكل اختبار بالثواني (افتراضي: 30)
    --عمال, --workers <ن>         عدد العمال للتوازي (افتراضي: 1)

التقليص:
    --بدون-تقليص, --no-shrink     تعطيل تقليص حالات الفشل
    --محاولات-تقليص <ن>           عدد محاولات التقليص (افتراضي: 100)

الإخراج:
    --مفصل, -v, --verbose         إخراج تفصيلي
    --صامت, -q, --quiet           إخراج مختصر
    --json                         إخراج بصيغة JSON
    --إخراج, --output <ملف>       حفظ التقرير في ملف

إعادة الإنتاج:
    --إعادة, --reproduce <ملف>    إعادة إنتاج فشل محفوظ
    --قائمة-فشل, --list-failures  عرض جميع حالات الفشل المحفوظة
    --تنظيف, --cleanup            حذف حالات الفشل القديمة

عام:
    --مساعدة, -h, --help          عرض هذه المساعدة
    --إصدار, --version            عرض رقم الإصدار

═══════════════════════════════════════════════════════════════════════════════
                           أمثلة متقدمة
═══════════════════════════════════════════════════════════════════════════════

# تشغيل 1000 تكرار مع بذرة محددة
ص اختبار خصائص --تكرارات 1000 --بذرة 12345

# تشغيل متوازي مع 4 عمال
ص اختبار خصائص --عمال 4

# إعادة إنتاج فشل سابق
ص اختبار خصائص --إعادة .sad_failures/20240115_103000_test.failure

# حفظ التقرير كـ JSON
ص اختبار خصائص --json --إخراج تقرير.json

# اختبارات تطابق نمط معين
ص اختبار خصائص --نمط "جمع|طرح"

)";
    }
    
    void printVersion() {
        std::cout << "اختبار الخصائص - إصدار 1.0.0\n";
        std::cout << "جزء من لغة ص البرمجية\n";
    }
};

// =============================================================================
// تقرير JSON
// =============================================================================

class JsonReporter {
public:
    struct TestResult {
        std::string name;
        bool passed;
        int iterations;
        std::optional<int> failedAt;
        std::optional<uint64_t> seed;
        double duration;
        std::string error;
    };
    
    std::string generateReport(const std::vector<TestResult>& results) {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"نتائج\": [\n";
        
        for (size_t i = 0; i < results.size(); i++) {
            const auto& r = results[i];
            ss << "    {\n";
            ss << "      \"اسم\": \"" << r.name << "\",\n";
            ss << "      \"نجح\": " << (r.passed ? "true" : "false") << ",\n";
            ss << "      \"تكرارات\": " << r.iterations << ",\n";
            if (r.failedAt) {
                ss << "      \"فشل_في\": " << *r.failedAt << ",\n";
            }
            if (r.seed) {
                ss << "      \"بذرة\": " << *r.seed << ",\n";
            }
            ss << "      \"مدة\": " << r.duration << ",\n";
            ss << "      \"خطأ\": \"" << r.error << "\"\n";
            ss << "    }";
            if (i < results.size() - 1) ss << ",";
            ss << "\n";
        }
        
        ss << "  ],\n";
        
        // إحصائيات
        int passed = 0, failed = 0;
        double total = 0;
        for (const auto& r : results) {
            if (r.passed) passed++;
            else failed++;
            total += r.duration;
        }
        
        ss << "  \"ملخص\": {\n";
        ss << "    \"مجموع\": " << results.size() << ",\n";
        ss << "    \"نجح\": " << passed << ",\n";
        ss << "    \"فشل\": " << failed << ",\n";
        ss << "    \"مدة_إجمالية\": " << total << "\n";
        ss << "  }\n";
        ss << "}\n";
        
        return ss.str();
    }
};

// =============================================================================
// مكتشف الاختبارات
// =============================================================================

class PropertyTestDiscoverer {
public:
    struct DiscoveredTest {
        std::string name;
        std::string file;
        int line;
        std::map<std::string, std::string> config;  // من السمة
    };
    
    /**
     * اكتشاف اختبارات في ملفات
     */
    std::vector<DiscoveredTest> discover(const std::vector<std::string>& files) {
        std::vector<DiscoveredTest> tests;
        
        for (const auto& file : files) {
            auto fileTests = discoverInFile(file);
            tests.insert(tests.end(), fileTests.begin(), fileTests.end());
        }
        
        return tests;
    }
    
    /**
     * اكتشاف في مجلد
     */
    std::vector<DiscoveredTest> discoverInDirectory(const std::string& dir) {
        std::vector<std::string> sadFiles;
        
        for (const auto& entry : 
             std::filesystem::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file() && 
                entry.path().extension() == ".ص") {
                sadFiles.push_back(entry.path().string());
            }
        }
        
        return discover(sadFiles);
    }
    
private:
    std::vector<DiscoveredTest> discoverInFile(const std::string& file) {
        std::vector<DiscoveredTest> tests;
        
        // TODO: تحليل الملف والبحث عن #[اختبار_خاصية]
        // هذا تنفيذ مبسط - سيتم ربطه مع المحلل الفعلي
        
        return tests;
    }
};

// =============================================================================
// أمر اختبار الخصائص
// =============================================================================

class PropertyTestCommand {
public:
    int execute(int argc, char* argv[]) {
        // تحليل الأوامر
        PropertyFlagParser parser;
        auto opts = parser.parse(argc, argv);
        
        // مساعدة
        if (opts.help) {
            PropertyTestHelp help;
            help.printHelp();
            return 0;
        }
        
        // إصدار
        if (opts.version) {
            PropertyTestHelp help;
            help.printVersion();
            return 0;
        }
        
        // قائمة الفشل
        if (opts.listFailures) {
            return listFailures();
        }
        
        // تنظيف
        if (opts.cleanupFailures) {
            return cleanupFailures();
        }
        
        // إعادة إنتاج
        if (!opts.reproduceFile.empty()) {
            return reproduce(opts.reproduceFile);
        }
        
        // تشغيل الاختبارات
        return runTests(opts);
    }
    
private:
    int runTests(const PropertyTestOptions& opts) {
        if (!opts.quiet) {
            printHeader();
        }
        
        // اكتشاف الاختبارات
        PropertyTestDiscoverer discoverer;
        std::vector<PropertyTestDiscoverer::DiscoveredTest> tests;
        
        if (opts.files.empty()) {
            // البحث في المجلد الحالي
            tests = discoverer.discoverInDirectory(".");
        } else {
            for (const auto& f : opts.files) {
                if (std::filesystem::is_directory(f)) {
                    auto dirTests = discoverer.discoverInDirectory(f);
                    tests.insert(tests.end(), dirTests.begin(), dirTests.end());
                } else {
                    auto fileTests = discoverer.discover({f});
                    tests.insert(tests.end(), fileTests.begin(), fileTests.end());
                }
            }
        }
        
        // تصفية بالدالة أو النمط
        if (opts.function) {
            tests.erase(
                std::remove_if(tests.begin(), tests.end(),
                    [&](const auto& t) { return t.name != *opts.function; }),
                tests.end());
        }
        
        if (!opts.pattern.empty()) {
            // TODO: دعم التعبيرات النمطية
        }
        
        if (tests.empty()) {
            std::cout << "لم يتم العثور على اختبارات خصائص.\n";
            return 0;
        }
        
        if (!opts.quiet) {
            std::cout << "تم العثور على " << tests.size() << " اختبار(ات).\n\n";
        }
        
        // TODO: تنفيذ الاختبارات الفعلية
        // هذا سيتم ربطه مع property_runner.cpp
        
        return 0;
    }
    
    int listFailures() {
        std::cout << "قائمة حالات الفشل المحفوظة:\n\n";
        
        std::filesystem::path failuresDir(".sad_failures");
        if (!std::filesystem::exists(failuresDir)) {
            std::cout << "  (لا توجد حالات فشل)\n";
            return 0;
        }
        
        int count = 0;
        for (const auto& entry : std::filesystem::directory_iterator(failuresDir)) {
            if (entry.path().extension() == ".failure") {
                std::cout << "  - " << entry.path().filename().string() << "\n";
                count++;
            }
        }
        
        if (count == 0) {
            std::cout << "  (لا توجد حالات فشل)\n";
        } else {
            std::cout << "\nالمجموع: " << count << " حالة(ات)\n";
        }
        
        return 0;
    }
    
    int cleanupFailures() {
        std::filesystem::path failuresDir(".sad_failures");
        if (!std::filesystem::exists(failuresDir)) {
            std::cout << "لا توجد حالات فشل للحذف.\n";
            return 0;
        }
        
        int removed = 0;
        for (const auto& entry : std::filesystem::directory_iterator(failuresDir)) {
            if (entry.path().extension() == ".failure") {
                std::filesystem::remove(entry.path());
                removed++;
            }
        }
        
        std::cout << "تم حذف " << removed << " حالة(ات) فشل.\n";
        return 0;
    }
    
    int reproduce(const std::string& file) {
        std::cout << "إعادة إنتاج الفشل من: " << file << "\n\n";
        
        // TODO: ربط مع reproduce_failure.cpp
        
        return 0;
    }
    
    void printHeader() {
        std::cout << "\n";
        std::cout << "════════════════════════════════════════════════════════════\n";
        std::cout << "              اختبار الخصائص - لغة ص\n";
        std::cout << "════════════════════════════════════════════════════════════\n\n";
    }
};

} // namespace sad::testing

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

/**
 * نقطة الدخول الرئيسية
 */
int sad_property_test_main(int argc, char* argv[]) {
    sad::testing::PropertyTestCommand cmd;
    return cmd.execute(argc, argv);
}

/**
 * طباعة المساعدة
 */
void sad_property_test_help() {
    sad::testing::PropertyTestHelp help;
    help.printHelp();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef PROPERTY_FLAG_TEST

void testFlagParsing() {
    std::cout << "🚩 اختبار تحليل الأعلام...\n";
    
    sad::testing::PropertyFlagParser parser;
    
    // اختبار التكرارات
    {
        const char* argv[] = {"test", "--تكرارات", "500"};
        auto opts = parser.parse(3, const_cast<char**>(argv));
        assert(opts.iterations == 500);
    }
    
    // اختبار البذرة
    {
        const char* argv[] = {"test", "--بذرة", "123456"};
        auto opts = parser.parse(3, const_cast<char**>(argv));
        assert(opts.seed.has_value());
        assert(*opts.seed == 123456);
    }
    
    // اختبار تعطيل التقليص
    {
        const char* argv[] = {"test", "--بدون-تقليص"};
        auto opts = parser.parse(2, const_cast<char**>(argv));
        assert(!opts.shrinkEnabled);
    }
    
    // اختبار الملفات
    {
        const char* argv[] = {"test", "file1.ص", "file2.ص", "--مفصل"};
        auto opts = parser.parse(4, const_cast<char**>(argv));
        assert(opts.files.size() == 2);
        assert(opts.verbose);
    }
    
    std::cout << "   ✅ نجح\n";
}

void testJsonReporter() {
    std::cout << "📊 اختبار تقرير JSON...\n";
    
    sad::testing::JsonReporter reporter;
    
    std::vector<sad::testing::JsonReporter::TestResult> results = {
        {"اختبار_1", true, 100, std::nullopt, std::nullopt, 0.5, ""},
        {"اختبار_2", false, 50, 42, 123456ULL, 1.2, "فشل"}
    };
    
    auto json = reporter.generateReport(results);
    
    assert(json.find("اختبار_1") != std::string::npos);
    assert(json.find("true") != std::string::npos);
    assert(json.find("123456") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات علم CLI للخصائص\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testFlagParsing();
    testJsonReporter();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // PROPERTY_FLAG_TEST
