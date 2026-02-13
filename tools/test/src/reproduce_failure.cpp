/**
 * =============================================================================
 * ملف: reproduce_failure.cpp
 * الوصف: إعادة إنتاج حالات الفشل في اختبار الخصائص
 * المهمة: T249 - Failure reproduction
 * المرحلة: Phase 25 - User Story 22 (Property Testing)
 * =============================================================================
 * 
 * 🔄 دليل المبتدئ لإعادة إنتاج الفشل
 * ═══════════════════════════════════════
 * 
 * لماذا نحتاج إعادة الإنتاج؟
 * ────────────────────────────
 * اختبار الخصائص يستخدم قيم عشوائية. عند الفشل، نحتاج:
 * 
 * 1. حفظ القيم التي سببت الفشل
 * 2. إعادة تشغيل الاختبار بنفس القيم
 * 3. التأكد من أن الإصلاح يعمل
 * 
 * كيف يعمل؟
 * ──────────
 * ```
 * الاختبار فشل ← حفظ البذرة/القيم ← ملف .failure
 *                     ↓
 * المطور يصلح ← إعادة مع البذرة ← تأكيد الإصلاح
 * ```
 * 
 * ملف الفشل:
 * ──────────
 * ```json
 * {
 *   "name": "خاصية_الجمع",
 *   "seed": 123456789,
 *   "args": [42, -15],
 *   "shrunk": [1, -1],
 *   "error": "assertion failed"
 * }
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace sad::testing {

// =============================================================================
// بيانات الفشل
// =============================================================================

/**
 * معلومات الفشل المحفوظة
 */
struct FailureRecord {
    std::string testName;
    uint64_t seed;
    int iteration;
    std::string errorMessage;
    
    // القيم كنصوص JSON
    std::vector<std::string> originalArgs;
    std::vector<std::string> shrunkArgs;
    
    // بيانات وصفية
    std::string timestamp;
    std::string sourceFile;
    int sourceLine;
    std::string gitCommit;
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"اسم_الاختبار\": \"" << testName << "\",\n";
        ss << "  \"البذرة\": " << seed << ",\n";
        ss << "  \"التكرار\": " << iteration << ",\n";
        ss << "  \"الخطأ\": \"" << escapeJson(errorMessage) << "\",\n";
        
        ss << "  \"القيم_الأصلية\": [";
        for (size_t i = 0; i < originalArgs.size(); i++) {
            if (i > 0) ss << ", ";
            ss << originalArgs[i];
        }
        ss << "],\n";
        
        ss << "  \"القيم_المُقلصة\": [";
        for (size_t i = 0; i < shrunkArgs.size(); i++) {
            if (i > 0) ss << ", ";
            ss << shrunkArgs[i];
        }
        ss << "],\n";
        
        ss << "  \"الوقت\": \"" << timestamp << "\",\n";
        ss << "  \"الملف\": \"" << sourceFile << "\",\n";
        ss << "  \"السطر\": " << sourceLine << ",\n";
        ss << "  \"الإيداع\": \"" << gitCommit << "\"\n";
        ss << "}";
        
        return ss.str();
    }
    
    /**
     * قراءة من JSON (مبسط)
     */
    static FailureRecord fromJson(const std::string& json) {
        FailureRecord record;
        
        // استخراج البذرة
        auto seedPos = json.find("\"البذرة\":");
        if (seedPos != std::string::npos) {
            auto numStart = json.find_first_of("0123456789", seedPos);
            auto numEnd = json.find_first_not_of("0123456789", numStart);
            record.seed = std::stoull(json.substr(numStart, numEnd - numStart));
        }
        
        // استخراج اسم الاختبار
        auto namePos = json.find("\"اسم_الاختبار\":");
        if (namePos != std::string::npos) {
            auto strStart = json.find('"', namePos + 20) + 1;
            auto strEnd = json.find('"', strStart);
            record.testName = json.substr(strStart, strEnd - strStart);
        }
        
        // استخراج التكرار
        auto iterPos = json.find("\"التكرار\":");
        if (iterPos != std::string::npos) {
            auto numStart = json.find_first_of("0123456789", iterPos);
            auto numEnd = json.find_first_not_of("0123456789", numStart);
            record.iteration = std::stoi(json.substr(numStart, numEnd - numStart));
        }
        
        return record;
    }
    
private:
    static std::string escapeJson(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// =============================================================================
// مخزن الفشل
// =============================================================================

/**
 * مدير ملفات الفشل
 */
class FailureStore {
public:
    explicit FailureStore(const std::string& dir = ".sad_failures")
        : directory_(dir) {
        std::filesystem::create_directories(directory_);
    }
    
    /**
     * حفظ حالة فشل
     */
    std::string save(const FailureRecord& record) {
        // إنشاء اسم الملف
        auto filename = generateFilename(record.testName);
        auto path = directory_ / filename;
        
        // كتابة الملف
        std::ofstream file(path);
        if (file.is_open()) {
            file << record.toJson();
            file.close();
        }
        
        return path.string();
    }
    
    /**
     * تحميل حالة فشل
     */
    std::optional<FailureRecord> load(const std::string& filename) {
        std::filesystem::path path;
        
        if (std::filesystem::exists(filename)) {
            path = filename;
        } else {
            path = directory_ / filename;
        }
        
        if (!std::filesystem::exists(path)) {
            return std::nullopt;
        }
        
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return FailureRecord::fromJson(buffer.str());
    }
    
    /**
     * قائمة جميع حالات الفشل
     */
    std::vector<std::string> list() {
        std::vector<std::string> files;
        
        if (!std::filesystem::exists(directory_)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
            if (entry.is_regular_file() && 
                entry.path().extension() == ".failure") {
                files.push_back(entry.path().filename().string());
            }
        }
        
        return files;
    }
    
    /**
     * حذف حالة فشل (بعد إصلاحها)
     */
    bool remove(const std::string& filename) {
        std::filesystem::path path;
        
        if (std::filesystem::exists(filename)) {
            path = filename;
        } else {
            path = directory_ / filename;
        }
        
        return std::filesystem::remove(path);
    }
    
    /**
     * تنظيف الحالات القديمة
     */
    int cleanup(int daysOld = 30) {
        int removed = 0;
        auto now = std::chrono::system_clock::now();
        
        for (const auto& entry : std::filesystem::directory_iterator(directory_)) {
            if (entry.is_regular_file()) {
                auto fileTime = entry.last_write_time();
                auto age = std::chrono::duration_cast<std::chrono::hours>(
                    std::filesystem::file_time_type::clock::now() - fileTime
                ).count() / 24;
                
                if (age > daysOld) {
                    std::filesystem::remove(entry.path());
                    removed++;
                }
            }
        }
        
        return removed;
    }
    
private:
    std::filesystem::path directory_;
    
    std::string generateFilename(const std::string& testName) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        ss << "_" << sanitize(testName) << ".failure";
        
        return ss.str();
    }
    
    std::string sanitize(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
                result += c;
            } else if (c == ' ') {
                result += '_';
            }
        }
        return result.empty() ? "test" : result;
    }
};

// =============================================================================
// مُعيد الإنتاج
// =============================================================================

/**
 * مُعيد إنتاج الاختبارات الفاشلة
 */
class FailureReproducer {
public:
    struct ReproduceResult {
        bool stillFails;
        std::string message;
        int attempts;
    };
    
    /**
     * إعادة إنتاج من ملف
     */
    ReproduceResult reproduceFromFile(const std::string& filename,
                                      std::function<bool(uint64_t, int)> testRunner) {
        FailureStore store;
        auto record = store.load(filename);
        
        if (!record) {
            return {false, "لم يتم العثور على ملف الفشل", 0};
        }
        
        return reproduceFromRecord(*record, testRunner);
    }
    
    /**
     * إعادة إنتاج من سجل
     */
    ReproduceResult reproduceFromRecord(const FailureRecord& record,
                                        std::function<bool(uint64_t, int)> testRunner) {
        ReproduceResult result;
        result.attempts = 1;
        
        // تشغيل الاختبار مع البذرة المحفوظة
        bool passes = testRunner(record.seed, record.iteration);
        
        result.stillFails = !passes;
        
        if (result.stillFails) {
            result.message = "الفشل لا يزال قابلاً للإعادة";
        } else {
            result.message = "الاختبار ينجح الآن - الخطأ ربما تم إصلاحه";
        }
        
        return result;
    }
    
    /**
     * إعادة إنتاج جميع الفشل المحفوظ
     */
    std::vector<ReproduceResult> reproduceAll(
        FailureStore& store,
        std::function<bool(const std::string&, uint64_t)> testLookup)
    {
        std::vector<ReproduceResult> results;
        
        for (const auto& filename : store.list()) {
            auto record = store.load(filename);
            if (!record) continue;
            
            auto result = reproduceFromRecord(*record, 
                [&](uint64_t seed, int iter) {
                    return testLookup(record->testName, seed);
                });
            
            results.push_back(result);
        }
        
        return results;
    }
};

// =============================================================================
// مُولد تقرير الفشل
// =============================================================================

class FailureReporter {
public:
    /**
     * طباعة تقرير تفصيلي
     */
    void printDetailedReport(const FailureRecord& record) {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║            تقرير فشل اختبار الخصائص                          ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        
        std::cout << "║ الاختبار: " << std::left << std::setw(50) 
                  << record.testName << "║\n";
        std::cout << "║ الوقت: " << std::left << std::setw(53) 
                  << record.timestamp << "║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        
        std::cout << "║ البذرة: " << std::left << std::setw(52) 
                  << record.seed << "║\n";
        std::cout << "║ التكرار: " << std::left << std::setw(51) 
                  << record.iteration << "║\n";
        
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ الخطأ:                                                       ║\n";
        std::cout << "║   " << std::left << std::setw(59) 
                  << record.errorMessage << "║\n";
        
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ القيم الأصلية:                                               ║\n";
        for (const auto& arg : record.originalArgs) {
            std::cout << "║   " << std::left << std::setw(59) << arg << "║\n";
        }
        
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ القيم المُقلصة:                                              ║\n";
        for (const auto& arg : record.shrunkArgs) {
            std::cout << "║   " << std::left << std::setw(59) << arg << "║\n";
        }
        
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ لإعادة الإنتاج:                                              ║\n";
        std::cout << "║   ص اختبار --بذرة " << std::left << std::setw(43) 
                  << record.seed << "║\n";
        
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }
    
    /**
     * طباعة ملخص
     */
    void printSummary(const std::vector<FailureRecord>& records) {
        std::cout << "\n═══ ملخص الفشل ═══\n\n";
        
        int i = 1;
        for (const auto& r : records) {
            std::cout << i++ << ". " << r.testName 
                      << " (بذرة: " << r.seed << ")\n";
        }
        
        std::cout << "\n";
    }
    
    /**
     * تصدير لـ Markdown
     */
    std::string exportMarkdown(const FailureRecord& record) {
        std::ostringstream ss;
        
        ss << "# فشل اختبار: " << record.testName << "\n\n";
        ss << "## المعلومات\n\n";
        ss << "| الحقل | القيمة |\n";
        ss << "|-------|--------|\n";
        ss << "| البذرة | `" << record.seed << "` |\n";
        ss << "| التكرار | " << record.iteration << " |\n";
        ss << "| الوقت | " << record.timestamp << " |\n";
        ss << "\n## الخطأ\n\n";
        ss << "```\n" << record.errorMessage << "\n```\n\n";
        ss << "## القيم\n\n";
        ss << "### الأصلية\n\n";
        for (const auto& arg : record.originalArgs) {
            ss << "- `" << arg << "`\n";
        }
        ss << "\n### المُقلصة\n\n";
        for (const auto& arg : record.shrunkArgs) {
            ss << "- `" << arg << "`\n";
        }
        ss << "\n## الإعادة\n\n";
        ss << "```bash\nص اختبار --بذرة " << record.seed << "\n```\n";
        
        return ss.str();
    }
};

// =============================================================================
// أمر CLI للإعادة
// =============================================================================

class ReproduceCommand {
public:
    struct Options {
        std::string failureFile;
        bool listAll = false;
        bool cleanup = false;
        int cleanupDays = 30;
        bool verbose = false;
    };
    
    int run(const Options& opts) {
        FailureStore store;
        
        if (opts.listAll) {
            return listFailures(store);
        }
        
        if (opts.cleanup) {
            return cleanupFailures(store, opts.cleanupDays);
        }
        
        if (!opts.failureFile.empty()) {
            return reproduceFailure(store, opts.failureFile, opts.verbose);
        }
        
        printUsage();
        return 1;
    }
    
private:
    int listFailures(FailureStore& store) {
        auto files = store.list();
        
        if (files.empty()) {
            std::cout << "لا توجد حالات فشل محفوظة.\n";
            return 0;
        }
        
        std::cout << "حالات الفشل المحفوظة:\n\n";
        
        FailureReporter reporter;
        std::vector<FailureRecord> records;
        
        for (const auto& f : files) {
            auto record = store.load(f);
            if (record) {
                records.push_back(*record);
            }
        }
        
        reporter.printSummary(records);
        return 0;
    }
    
    int cleanupFailures(FailureStore& store, int days) {
        int removed = store.cleanup(days);
        std::cout << "تم حذف " << removed << " ملف(ات) فشل قديمة.\n";
        return 0;
    }
    
    int reproduceFailure(FailureStore& store, const std::string& file, bool verbose) {
        auto record = store.load(file);
        
        if (!record) {
            std::cout << "خطأ: لم يتم العثور على ملف الفشل.\n";
            return 1;
        }
        
        FailureReporter reporter;
        reporter.printDetailedReport(*record);
        
        std::cout << "\nلإعادة التشغيل، استخدم:\n";
        std::cout << "  ص اختبار " << record->testName 
                  << " --بذرة " << record->seed << "\n\n";
        
        return 0;
    }
    
    void printUsage() {
        std::cout << "استخدام: ص إعادة-فشل [خيارات]\n\n";
        std::cout << "الخيارات:\n";
        std::cout << "  --ملف <اسم>    إعادة إنتاج فشل معين\n";
        std::cout << "  --قائمة        عرض جميع حالات الفشل\n";
        std::cout << "  --تنظيف        حذف الحالات القديمة\n";
        std::cout << "  --أيام <ن>     عمر الحالات للحذف (افتراضي: 30)\n";
    }
};

} // namespace sad::testing

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadFailureStore {
    sad::testing::FailureStore* store;
};

SadFailureStore* sad_failure_store_new(const char* dir) {
    auto* ctx = new SadFailureStore();
    ctx->store = new sad::testing::FailureStore(dir ? dir : ".sad_failures");
    return ctx;
}

const char* sad_failure_store_save(SadFailureStore* ctx, 
                                   const char* testName,
                                   uint64_t seed,
                                   int iteration,
                                   const char* error) {
    sad::testing::FailureRecord record;
    record.testName = testName;
    record.seed = seed;
    record.iteration = iteration;
    record.errorMessage = error;
    
    // الوقت الحالي
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    record.timestamp = ss.str();
    
    static std::string savedPath;
    savedPath = ctx->store->save(record);
    return savedPath.c_str();
}

void sad_failure_store_free(SadFailureStore* ctx) {
    if (ctx) {
        delete ctx->store;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef REPRODUCE_FAILURE_TEST

void testFailureRecord() {
    std::cout << "📝 اختبار سجل الفشل...\n";
    
    sad::testing::FailureRecord record;
    record.testName = "خاصية_الجمع";
    record.seed = 123456789;
    record.iteration = 42;
    record.errorMessage = "فشل التأكيد";
    record.originalArgs = {"100", "-50"};
    record.shrunkArgs = {"1", "-1"};
    record.timestamp = "2024-01-15 10:30:00";
    
    auto json = record.toJson();
    assert(json.find("خاصية_الجمع") != std::string::npos);
    assert(json.find("123456789") != std::string::npos);
    
    auto loaded = sad::testing::FailureRecord::fromJson(json);
    assert(loaded.seed == 123456789);
    assert(loaded.testName == "خاصية_الجمع");
    
    std::cout << "   ✅ نجح\n";
}

void testFailureStore() {
    std::cout << "💾 اختبار مخزن الفشل...\n";
    
    sad::testing::FailureStore store(".test_failures");
    
    sad::testing::FailureRecord record;
    record.testName = "اختبار_تجريبي";
    record.seed = 999;
    record.iteration = 1;
    record.timestamp = "2024-01-15 10:30:00";
    
    auto path = store.save(record);
    assert(!path.empty());
    
    auto files = store.list();
    assert(!files.empty());
    
    auto loaded = store.load(files[0]);
    assert(loaded.has_value());
    assert(loaded->seed == 999);
    
    // تنظيف
    for (const auto& f : files) {
        store.remove(f);
    }
    std::filesystem::remove_all(".test_failures");
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات إعادة إنتاج الفشل\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testFailureRecord();
    testFailureStore();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // REPRODUCE_FAILURE_TEST
