/**
 * ==========================================================
 * ملف: shared_tests.cpp
 * الوصف: تنفيذ الاختبارات المشتركة
 * المهمة: T313 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🧪 ما هي الاختبارات المشتركة؟
 * ----------------------------
 * عندما تعمل مع فريق، يمكنكم تشغيل الاختبارات معاً
 * وترى نتائج الجميع في مكان واحد!
 * 
 * 📝 مثال:
 * --------
 * أحمد يشغّل اختبارات الوحدة ← الجميع يرى النتيجة
 * سارة تشغّل اختبارات التكامل ← الجميع يرى النتيجة
 */

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <mutex>
#include <sstream>
#include <cstring>
#include <chrono>

namespace sad {
namespace collab {

// ==========================================================
// 📌 نتيجة اختبار
// ==========================================================

/**
 * ✅ حالة الاختبار
 */
enum class TestStatus {
    PENDING,    // قيد الانتظار
    RUNNING,    // يعمل
    PASSED,     // نجح
    FAILED,     // فشل
    SKIPPED     // تم تخطيه
};

const char* testStatusName(TestStatus status) {
    switch (status) {
        case TestStatus::PENDING: return "قيد الانتظار";
        case TestStatus::RUNNING: return "يعمل";
        case TestStatus::PASSED: return "نجح";
        case TestStatus::FAILED: return "فشل";
        case TestStatus::SKIPPED: return "تم تخطيه";
        default: return "غير معروف";
    }
}

const char* testStatusEmoji(TestStatus status) {
    switch (status) {
        case TestStatus::PENDING: return "⏳";
        case TestStatus::RUNNING: return "🔄";
        case TestStatus::PASSED: return "✅";
        case TestStatus::FAILED: return "❌";
        case TestStatus::SKIPPED: return "⏭️";
        default: return "❓";
    }
}

/**
 * 📋 نتيجة اختبار
 */
struct TestResult {
    std::string testName;
    std::string testFile;
    TestStatus status = TestStatus::PENDING;
    uint32_t runnerId;              // من شغّله
    std::string runnerName;
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    std::string output;
    std::string errorMessage;
    int assertions = 0;
    int failedAssertions = 0;
    
    /**
     * المدة بالمللي ثانية
     */
    uint64_t duration() const {
        return endTime > startTime ? endTime - startTime : 0;
    }
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"testName\": \"" << testName << "\",\n";
        oss << "  \"testFile\": \"" << testFile << "\",\n";
        oss << "  \"status\": \"" << testStatusName(status) << "\",\n";
        oss << "  \"runnerId\": " << runnerId << ",\n";
        oss << "  \"runnerName\": \"" << runnerName << "\",\n";
        oss << "  \"duration\": " << duration() << ",\n";
        oss << "  \"assertions\": " << assertions << ",\n";
        oss << "  \"failedAssertions\": " << failedAssertions << "\n";
        oss << "}";
        return oss.str();
    }
    
    /**
     * تحويل لـ string قابل للقراءة
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << testStatusEmoji(status) << " " << testName;
        oss << " (" << runnerName << ")";
        oss << " - " << duration() << "ms";
        if (status == TestStatus::FAILED && !errorMessage.empty()) {
            oss << "\n   ❌ " << errorMessage;
        }
        return oss.str();
    }
};

// ==========================================================
// 📌 طلب اختبار
// ==========================================================

/**
 * 📝 طلب تشغيل اختبار
 */
struct TestRequest {
    std::string testPattern;        // نمط الاختبار (اسم أو wildcard)
    std::string testFile;           // ملف الاختبار
    uint32_t requesterId;           // من طلب
    std::string requesterName;
    uint64_t timestamp;
    bool runAll = false;            // تشغيل الكل
    
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"testPattern\": \"" << testPattern << "\",\n";
        oss << "  \"testFile\": \"" << testFile << "\",\n";
        oss << "  \"requesterId\": " << requesterId << ",\n";
        oss << "  \"requesterName\": \"" << requesterName << "\",\n";
        oss << "  \"runAll\": " << (runAll ? "true" : "false") << "\n";
        oss << "}";
        return oss.str();
    }
};

// ==========================================================
// 📌 ملخص الاختبارات
// ==========================================================

/**
 * 📊 ملخص الاختبارات
 */
struct TestSummary {
    int total = 0;
    int passed = 0;
    int failed = 0;
    int skipped = 0;
    int running = 0;
    uint64_t totalDuration = 0;
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "═══════════════════════════════════════════════════\n";
        oss << "              📊 ملخص الاختبارات\n";
        oss << "═══════════════════════════════════════════════════\n\n";
        oss << "  📋 الإجمالي: " << total << "\n";
        oss << "  ✅ نجح: " << passed << "\n";
        oss << "  ❌ فشل: " << failed << "\n";
        oss << "  ⏭️ تم تخطيه: " << skipped << "\n";
        oss << "  🔄 يعمل: " << running << "\n";
        oss << "  ⏱️ المدة: " << totalDuration << "ms\n";
        
        if (failed == 0 && running == 0) {
            oss << "\n  🎉 جميع الاختبارات نجحت!\n";
        } else if (failed > 0) {
            oss << "\n  ⚠️ هناك " << failed << " اختبار فاشل\n";
        }
        
        return oss.str();
    }
};

// ==========================================================
// 📌 مدير الاختبارات المشتركة
// ==========================================================

/**
 * 🧪 مدير الاختبارات المشتركة
 */
class SharedTestManager {
private:
    std::string sessionId_;
    uint32_t localUserId_;
    
    std::map<std::string, TestResult> results_;
    std::queue<TestRequest> pendingRequests_;
    std::mutex mutex_;
    
    // Callbacks
    std::function<void(const TestRequest&)> onTestRequested_;
    std::function<void(const TestResult&)> onTestCompleted_;
    
public:
    /**
     * إنشاء المدير
     */
    SharedTestManager(const std::string& sessionId, uint32_t localUserId)
        : sessionId_(sessionId), localUserId_(localUserId) {}
    
    // ═══════════════════════════════════════════════════════
    // طلب الاختبارات
    // ═══════════════════════════════════════════════════════
    
    /**
     * طلب تشغيل اختبار
     */
    void requestTest(const std::string& testPattern,
                     const std::string& testFile = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        TestRequest req;
        req.testPattern = testPattern;
        req.testFile = testFile;
        req.requesterId = localUserId_;
        req.requesterName = "أنت";
        req.timestamp = currentTime();
        req.runAll = testPattern == "*" || testPattern.empty();
        
        pendingRequests_.push(req);
        
        // إشعار الآخرين (عبر callback)
        if (onTestRequested_) {
            onTestRequested_(req);
        }
    }
    
    /**
     * استقبال طلب اختبار من آخر
     */
    void receiveTestRequest(const TestRequest& request) {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingRequests_.push(request);
    }
    
    // ═══════════════════════════════════════════════════════
    // تحديث النتائج
    // ═══════════════════════════════════════════════════════
    
    /**
     * بدء اختبار
     */
    void startTest(const std::string& testName, const std::string& testFile) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        TestResult result;
        result.testName = testName;
        result.testFile = testFile;
        result.status = TestStatus::RUNNING;
        result.runnerId = localUserId_;
        result.runnerName = "أنت";
        result.startTime = currentTime();
        
        results_[testName] = result;
    }
    
    /**
     * إنهاء اختبار بنجاح
     */
    void passTest(const std::string& testName, int assertions = 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = results_.find(testName);
        if (it != results_.end()) {
            it->second.status = TestStatus::PASSED;
            it->second.endTime = currentTime();
            it->second.assertions = assertions;
            
            if (onTestCompleted_) {
                onTestCompleted_(it->second);
            }
        }
    }
    
    /**
     * إنهاء اختبار بفشل
     */
    void failTest(const std::string& testName, const std::string& error,
                  int assertions = 0, int failedAssertions = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = results_.find(testName);
        if (it != results_.end()) {
            it->second.status = TestStatus::FAILED;
            it->second.endTime = currentTime();
            it->second.errorMessage = error;
            it->second.assertions = assertions;
            it->second.failedAssertions = failedAssertions;
            
            if (onTestCompleted_) {
                onTestCompleted_(it->second);
            }
        }
    }
    
    /**
     * استقبال نتيجة من آخر
     */
    void receiveResult(const TestResult& result) {
        std::lock_guard<std::mutex> lock(mutex_);
        results_[result.testName] = result;
    }
    
    // ═══════════════════════════════════════════════════════
    // الاستعلام
    // ═══════════════════════════════════════════════════════
    
    /**
     * الحصول على ملخص
     */
    TestSummary getSummary() const {
        TestSummary summary;
        
        for (const auto& [name, result] : results_) {
            summary.total++;
            switch (result.status) {
                case TestStatus::PASSED:
                    summary.passed++;
                    break;
                case TestStatus::FAILED:
                    summary.failed++;
                    break;
                case TestStatus::SKIPPED:
                    summary.skipped++;
                    break;
                case TestStatus::RUNNING:
                    summary.running++;
                    break;
                default:
                    break;
            }
            summary.totalDuration += result.duration();
        }
        
        return summary;
    }
    
    /**
     * الحصول على جميع النتائج
     */
    std::vector<TestResult> getAllResults() const {
        std::vector<TestResult> list;
        for (const auto& [name, result] : results_) {
            list.push_back(result);
        }
        return list;
    }
    
    /**
     * الحصول على الاختبارات الفاشلة
     */
    std::vector<TestResult> getFailedTests() const {
        std::vector<TestResult> list;
        for (const auto& [name, result] : results_) {
            if (result.status == TestStatus::FAILED) {
                list.push_back(result);
            }
        }
        return list;
    }
    
    // ═══════════════════════════════════════════════════════
    // التقرير
    // ═══════════════════════════════════════════════════════
    
    /**
     * تقرير كامل
     */
    std::string generateReport() const {
        std::ostringstream oss;
        
        auto summary = getSummary();
        oss << summary.toString() << "\n";
        
        oss << "📋 تفاصيل الاختبارات:\n";
        oss << "───────────────────────────────────────────────────\n";
        
        for (const auto& [name, result] : results_) {
            oss << "  " << result.toString() << "\n";
        }
        
        return oss.str();
    }
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"sessionId\": \"" << sessionId_ << "\",\n";
        oss << "  \"summary\": " << summaryToJson() << ",\n";
        oss << "  \"results\": [\n";
        
        bool first = true;
        for (const auto& [name, result] : results_) {
            if (!first) oss << ",\n";
            oss << "    " << result.toJson();
            first = false;
        }
        
        oss << "\n  ]\n}";
        return oss.str();
    }
    
    // ═══════════════════════════════════════════════════════
    // Callbacks
    // ═══════════════════════════════════════════════════════
    
    void setOnTestRequested(std::function<void(const TestRequest&)> callback) {
        onTestRequested_ = callback;
    }
    
    void setOnTestCompleted(std::function<void(const TestResult&)> callback) {
        onTestCompleted_ = callback;
    }
    
private:
    uint64_t currentTime() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    std::string summaryToJson() const {
        auto s = getSummary();
        std::ostringstream oss;
        oss << "{"
            << "\"total\": " << s.total << ", "
            << "\"passed\": " << s.passed << ", "
            << "\"failed\": " << s.failed << ", "
            << "\"skipped\": " << s.skipped << ", "
            << "\"running\": " << s.running << ", "
            << "\"duration\": " << s.totalDuration
            << "}";
        return oss.str();
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مدير الاختبارات
 */
void* sad_shared_tests_new(const char* sessionId, uint32_t userId) {
    return new sad::collab::SharedTestManager(sessionId, userId);
}

/**
 * تحرير المدير
 */
void sad_shared_tests_free(void* manager) {
    delete static_cast<sad::collab::SharedTestManager*>(manager);
}

/**
 * طلب تشغيل اختبار
 */
void sad_shared_tests_request(void* manager, const char* pattern,
                               const char* file) {
    auto* mgr = static_cast<sad::collab::SharedTestManager*>(manager);
    mgr->requestTest(pattern, file ? file : "");
}

/**
 * بدء اختبار
 */
void sad_shared_tests_start(void* manager, const char* name, const char* file) {
    auto* mgr = static_cast<sad::collab::SharedTestManager*>(manager);
    mgr->startTest(name, file);
}

/**
 * نجاح اختبار
 */
void sad_shared_tests_pass(void* manager, const char* name, int assertions) {
    auto* mgr = static_cast<sad::collab::SharedTestManager*>(manager);
    mgr->passTest(name, assertions);
}

/**
 * فشل اختبار
 */
void sad_shared_tests_fail(void* manager, const char* name, const char* error) {
    auto* mgr = static_cast<sad::collab::SharedTestManager*>(manager);
    mgr->failTest(name, error);
}

/**
 * الحصول على التقرير
 */
char* sad_shared_tests_report(void* manager) {
    auto* mgr = static_cast<sad::collab::SharedTestManager*>(manager);
    std::string report = mgr->generateReport();
    
    char* result = new char[report.size() + 1];
    std::strcpy(result, report.c_str());
    return result;
}

/**
 * الحصول على JSON
 */
char* sad_shared_tests_to_json(void* manager) {
    auto* mgr = static_cast<sad::collab::SharedTestManager*>(manager);
    std::string json = mgr->toJson();
    
    char* result = new char[json.size() + 1];
    std::strcpy(result, json.c_str());
    return result;
}

/**
 * تحرير نص
 */
void sad_shared_tests_free_string(char* str) {
    delete[] str;
}

} // extern "C"
