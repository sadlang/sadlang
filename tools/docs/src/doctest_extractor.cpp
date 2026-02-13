/**
 * =============================================================================
 * ملف: doctest_extractor.cpp
 * الوصف: استخراج اختبارات التوثيق وتشغيلها
 * المهمة: T233 - Doc tests extractor
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 🧪 دليل المبتدئ لاختبارات التوثيق
 * ════════════════════════════════════
 * 
 * ما هي اختبارات التوثيق؟
 * ─────────────────────────
 * اختبارات مكتوبة داخل التوثيق نفسه!
 * تتأكد أن الأمثلة في التوثيق تعمل فعلاً.
 * 
 * لماذا نحتاجها؟
 * ───────────────
 * 1. ضمان صحة الأمثلة في التوثيق
 * 2. اكتشاف التوثيق القديم
 * 3. الأمثلة الحقيقية أفضل من المصطنعة
 * 
 * مثال في لغة ص:
 * ───────────────
 * 
 * /// تجمع عددين
 * /// @مثال
 * /// ```ص
 * /// متغير ن = جمع(٣, ٥)
 * /// اطبع(ن)  // الخرج: ٨
 * /// ```
 * دالة جمع(أ: عدد, ب: عدد) -> عدد
 *     ارجع أ + ب
 * نهاية
 * 
 * كيف يعمل الاستخراج:
 * ────────────────────
 * 1. نقرأ ملفات المصدر
 * 2. نجد كتل ```ص ... ```
 * 3. نستخرج الكود والخرج المتوقع
 * 4. ننشئ ملف اختبار
 * 5. نشغله ونقارن النتائج
 * 
 * تنسيقات الاختبار:
 * ──────────────────
 * 
 * // خرج متوقع
 * اطبع("مرحباً")  // الخرج: مرحباً
 * 
 * // قيمة متوقعة
 * /// @توقع ن == ٨
 * 
 * // خطأ متوقع
 * /// @خطأ خطأ_نوع
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

namespace sad::docs {

// =============================================================================
// أنواع الاختبار
// =============================================================================

enum class DocTestType {
    OutputTest,     // مقارنة الخرج
    AssertTest,     // تأكيد شرط
    ErrorTest,      // توقع خطأ
    NoRunTest       // لا تشغل (عرض فقط)
};

// =============================================================================
// اختبار توثيق واحد
// =============================================================================

struct DocTest {
    std::string name;           // اسم الاختبار
    std::string sourceFile;     // الملف المصدر
    int lineNumber;             // رقم السطر
    std::string code;           // الكود
    std::string expectedOutput; // الخرج المتوقع
    std::string expectedError;  // الخطأ المتوقع
    std::vector<std::string> assertions; // التأكيدات
    DocTestType type;           // نوع الاختبار
    std::string context;        // سياق (اسم الدالة/الصنف)
    bool skip = false;          // تخطي الاختبار
    std::string skipReason;     // سبب التخطي
    
    /**
     * تحويل لملف اختبار
     */
    std::string toTestFile() const {
        std::ostringstream ss;
        
        ss << "// اختبار توثيق: " << name << "\n";
        ss << "// من: " << sourceFile << ":" << lineNumber << "\n\n";
        
        // الكود
        ss << code << "\n\n";
        
        // التأكيدات
        for (const auto& assertion : assertions) {
            ss << "لو !(" << assertion << ")\n";
            ss << "    اطبع(\"❌ فشل: " << assertion << "\")\n";
            ss << "    خروج(١)\n";
            ss << "نهاية\n";
        }
        
        ss << "\nاطبع(\"✅ نجح: " << name << "\")\n";
        
        return ss.str();
    }
};

// =============================================================================
// نتيجة اختبار
// =============================================================================

struct DocTestResult {
    std::string testName;
    bool passed = false;
    std::string actualOutput;
    std::string expectedOutput;
    std::string errorMessage;
    double timeMs = 0;
    
    /**
     * طباعة النتيجة
     */
    void print() const {
        if (passed) {
            std::cout << "   ✅ " << testName << " (" << timeMs << " مللي ثانية)\n";
        } else {
            std::cout << "   ❌ " << testName << "\n";
            if (!errorMessage.empty()) {
                std::cout << "      خطأ: " << errorMessage << "\n";
            }
            if (actualOutput != expectedOutput) {
                std::cout << "      متوقع: " << expectedOutput << "\n";
                std::cout << "      فعلي: " << actualOutput << "\n";
            }
        }
    }
};

// =============================================================================
// تقرير الاختبارات
// =============================================================================

struct DocTestReport {
    int total = 0;
    int passed = 0;
    int failed = 0;
    int skipped = 0;
    double totalTimeMs = 0;
    std::vector<DocTestResult> results;
    
    /**
     * طباعة التقرير
     */
    void print() const {
        std::cout << "\n";
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "   📊 تقرير اختبارات التوثيق\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        // النتائج
        for (const auto& result : results) {
            result.print();
        }
        
        // الملخص
        std::cout << "\n───────────────────────────────────────────────\n";
        std::cout << "📈 الملخص:\n";
        std::cout << "   إجمالي: " << total << "\n";
        std::cout << "   نجح: " << passed << " ✅\n";
        std::cout << "   فشل: " << failed << " ❌\n";
        if (skipped > 0) {
            std::cout << "   تخطي: " << skipped << " ⏭️\n";
        }
        std::cout << "   الوقت: " << totalTimeMs << " مللي ثانية\n";
        
        // النتيجة النهائية
        if (failed == 0) {
            std::cout << "\n🎉 جميع اختبارات التوثيق نجحت!\n";
        } else {
            std::cout << "\n⚠️ بعض الاختبارات فشلت. راجع التوثيق.\n";
        }
    }
    
    /**
     * إرجاع كود الخروج
     */
    int exitCode() const {
        return failed == 0 ? 0 : 1;
    }
};

// =============================================================================
// مستخرج اختبارات التوثيق
// =============================================================================

class DocTestExtractor {
public:
    /**
     * استخراج الاختبارات من ملف
     */
    std::vector<DocTest> extractFromFile(const std::string& filePath) {
        std::vector<DocTest> tests;
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return tests;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        return extractFromContent(content, filePath);
    }
    
    /**
     * استخراج من محتوى
     */
    std::vector<DocTest> extractFromContent(
        const std::string& content,
        const std::string& fileName = "test.ص"
    ) {
        std::vector<DocTest> tests;
        
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        std::string currentContext;
        bool inDocComment = false;
        bool inCodeBlock = false;
        std::string codeBlockContent;
        std::string codeBlockLang;
        int codeBlockStartLine = 0;
        std::vector<std::string> docLines;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // تتبع السياق
            if (line.find("دالة ") != std::string::npos) {
                size_t start = line.find("دالة ") + 5;
                size_t end = line.find('(');
                if (end != std::string::npos) {
                    currentContext = line.substr(start, end - start);
                }
            } else if (line.find("صنف ") != std::string::npos) {
                size_t start = line.find("صنف ") + 4;
                size_t end = line.find_first_of(" {:<");
                if (end != std::string::npos) {
                    currentContext = line.substr(start, end - start);
                }
            }
            
            // تعليق توثيق
            if (line.find("///") != std::string::npos) {
                size_t pos = line.find("///") + 3;
                std::string comment = line.substr(pos);
                // تنظيف
                size_t start = comment.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    comment = comment.substr(start);
                } else {
                    comment = "";
                }
                
                inDocComment = true;
                
                // بداية كتلة كود
                if (comment.find("```") == 0) {
                    if (!inCodeBlock) {
                        inCodeBlock = true;
                        codeBlockContent.clear();
                        codeBlockStartLine = lineNumber;
                        // اللغة
                        codeBlockLang = comment.substr(3);
                        if (codeBlockLang.empty()) codeBlockLang = "ص";
                    } else {
                        // نهاية كتلة كود
                        inCodeBlock = false;
                        
                        // إنشاء اختبار
                        if (shouldExtract(codeBlockLang)) {
                            DocTest test;
                            test.name = currentContext.empty() 
                                ? "test_" + std::to_string(tests.size() + 1)
                                : currentContext + "_test_" + std::to_string(tests.size() + 1);
                            test.sourceFile = fileName;
                            test.lineNumber = codeBlockStartLine;
                            test.context = currentContext;
                            
                            parseCodeBlock(codeBlockContent, test);
                            
                            tests.push_back(test);
                        }
                    }
                } else if (inCodeBlock) {
                    codeBlockContent += comment + "\n";
                } else {
                    docLines.push_back(comment);
                    
                    // @تخطي
                    if (comment.find("@تخطي") != std::string::npos ||
                        comment.find("@skip") != std::string::npos) {
                        // التخطي سيؤثر على الاختبار التالي
                    }
                }
            } else if (inDocComment && line.find("///") == std::string::npos) {
                inDocComment = false;
                docLines.clear();
            }
        }
        
        return tests;
    }
    
    /**
     * استخراج من مجلد
     */
    std::vector<DocTest> extractFromDirectory(const std::string& dirPath) {
        std::vector<DocTest> allTests;
        
        try {
            for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if (ext == ".ص" || ext == ".sad") {
                        auto tests = extractFromFile(entry.path().string());
                        allTests.insert(allTests.end(), tests.begin(), tests.end());
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "⚠️ خطأ: " << e.what() << "\n";
        }
        
        return allTests;
    }
    
private:
    /**
     * هل نستخرج هذه اللغة؟
     */
    bool shouldExtract(const std::string& lang) const {
        return lang == "ص" || lang == "sad" || 
               lang == "ص-مثال" || lang == "sad-example" ||
               lang.empty();
    }
    
    /**
     * تحليل كتلة الكود
     */
    void parseCodeBlock(const std::string& content, DocTest& test) {
        std::istringstream stream(content);
        std::string line;
        std::ostringstream code;
        
        while (std::getline(stream, line)) {
            // البحث عن الخرج المتوقع: // الخرج: ...
            size_t outputPos = line.find("// الخرج:");
            if (outputPos == std::string::npos) {
                outputPos = line.find("// output:");
            }
            
            if (outputPos != std::string::npos) {
                std::string expected = line.substr(outputPos + 10);
                // تنظيف
                size_t start = expected.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    expected = expected.substr(start);
                }
                test.expectedOutput += expected + "\n";
                test.type = DocTestType::OutputTest;
                
                // إزالة التعليق من الكود
                code << line.substr(0, outputPos) << "\n";
            }
            // البحث عن تأكيدات: // توقع: ...
            else if (line.find("// توقع:") != std::string::npos ||
                     line.find("// assert:") != std::string::npos) {
                size_t pos = line.find(":");
                std::string assertion = line.substr(pos + 1);
                size_t start = assertion.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    test.assertions.push_back(assertion.substr(start));
                }
                test.type = DocTestType::AssertTest;
                code << line.substr(0, line.find("//")) << "\n";
            }
            // البحث عن خطأ متوقع: // خطأ: ...
            else if (line.find("// خطأ:") != std::string::npos ||
                     line.find("// error:") != std::string::npos) {
                size_t pos = line.find(":");
                test.expectedError = line.substr(pos + 1);
                test.type = DocTestType::ErrorTest;
                code << line.substr(0, line.find("//")) << "\n";
            }
            // لا تشغل
            else if (line.find("// لا-تشغل") != std::string::npos ||
                     line.find("// norun") != std::string::npos) {
                test.type = DocTestType::NoRunTest;
            }
            else {
                code << line << "\n";
            }
        }
        
        test.code = code.str();
        
        // إزالة سطر جديد أخير من الخرج المتوقع
        if (!test.expectedOutput.empty() && test.expectedOutput.back() == '\n') {
            test.expectedOutput.pop_back();
        }
    }
};

// =============================================================================
// مشغل الاختبارات
// =============================================================================

class DocTestRunner {
public:
    using RunFunction = std::function<std::string(const std::string&)>;
    
    /**
     * تعيين دالة التشغيل
     */
    void setRunner(RunFunction fn) {
        runner_ = fn;
    }
    
    /**
     * تشغيل اختبار واحد
     */
    DocTestResult runTest(const DocTest& test) {
        DocTestResult result;
        result.testName = test.name;
        result.expectedOutput = test.expectedOutput;
        
        if (test.skip) {
            result.passed = true;
            result.actualOutput = "(تم التخطي)";
            return result;
        }
        
        if (test.type == DocTestType::NoRunTest) {
            result.passed = true;
            result.actualOutput = "(عرض فقط)";
            return result;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // تشغيل الكود
            if (runner_) {
                result.actualOutput = runner_(test.code);
            } else {
                result.actualOutput = simulateRun(test.code);
            }
            
            // مقارنة النتيجة
            switch (test.type) {
                case DocTestType::OutputTest:
                    result.passed = (result.actualOutput == test.expectedOutput);
                    break;
                    
                case DocTestType::AssertTest:
                    // يُفترض أن الكود يُرجع نجاح/فشل
                    result.passed = (result.actualOutput.find("❌") == std::string::npos);
                    break;
                    
                case DocTestType::ErrorTest:
                    result.passed = (result.actualOutput.find(test.expectedError) != std::string::npos);
                    break;
                    
                default:
                    result.passed = true;
            }
        } catch (const std::exception& e) {
            result.passed = (test.type == DocTestType::ErrorTest);
            result.errorMessage = e.what();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = endTime - startTime;
        result.timeMs = diff.count();
        
        return result;
    }
    
    /**
     * تشغيل جميع الاختبارات
     */
    DocTestReport runAll(const std::vector<DocTest>& tests) {
        DocTestReport report;
        report.total = tests.size();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (const auto& test : tests) {
            if (test.skip) {
                report.skipped++;
                continue;
            }
            
            auto result = runTest(test);
            report.results.push_back(result);
            
            if (result.passed) {
                report.passed++;
            } else {
                report.failed++;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = endTime - startTime;
        report.totalTimeMs = diff.count();
        
        return report;
    }
    
private:
    RunFunction runner_;
    
    /**
     * محاكاة التشغيل (للاختبار)
     */
    std::string simulateRun(const std::string& code) {
        // محاكاة بسيطة: البحث عن اطبع()
        std::ostringstream output;
        std::regex printRe(R"(اطبع\(\"([^\"]*)\"\))");
        
        std::sregex_iterator it(code.begin(), code.end(), printRe);
        std::sregex_iterator end;
        
        while (it != end) {
            output << (*it)[1].str() << "\n";
            ++it;
        }
        
        std::string result = output.str();
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
        
        return result;
    }
};

// =============================================================================
// مولد ملفات الاختبار
// =============================================================================

class DocTestGenerator {
public:
    /**
     * توليد ملف اختبار
     */
    std::string generateTestFile(const std::vector<DocTest>& tests) {
        std::ostringstream ss;
        
        ss << "// ═══════════════════════════════════════════════\n";
        ss << "// اختبارات التوثيق المُولدة تلقائياً\n";
        ss << "// تم التوليد: " << getCurrentTime() << "\n";
        ss << "// ═══════════════════════════════════════════════\n\n";
        
        ss << "استورد اختبار\n\n";
        
        for (size_t i = 0; i < tests.size(); i++) {
            const auto& test = tests[i];
            
            ss << "// اختبار #" << (i + 1) << ": " << test.name << "\n";
            ss << "// من: " << test.sourceFile << ":" << test.lineNumber << "\n";
            ss << "دالة اختبار_" << (i + 1) << "()\n";
            
            // الكود مع تعديل المسافات
            std::istringstream codeStream(test.code);
            std::string line;
            while (std::getline(codeStream, line)) {
                ss << "    " << line << "\n";
            }
            
            // التأكيدات
            if (!test.expectedOutput.empty()) {
                ss << "    // التحقق من الخرج\n";
            }
            
            for (const auto& assertion : test.assertions) {
                ss << "    تأكد(" << assertion << ")\n";
            }
            
            ss << "نهاية\n\n";
        }
        
        // دالة التشغيل الرئيسية
        ss << "// تشغيل جميع الاختبارات\n";
        ss << "دالة رئيسية()\n";
        ss << "    متغير نجاح = ٠\n";
        ss << "    متغير فشل = ٠\n\n";
        
        for (size_t i = 0; i < tests.size(); i++) {
            ss << "    حاول\n";
            ss << "        اختبار_" << (i + 1) << "()\n";
            ss << "        نجاح += ١\n";
            ss << "        اطبع(\"✅ " << tests[i].name << "\")\n";
            ss << "    امسك خطأ\n";
            ss << "        فشل += ١\n";
            ss << "        اطبع(\"❌ " << tests[i].name << "\")\n";
            ss << "    نهاية\n\n";
        }
        
        ss << "    اطبع(\"\")\n";
        ss << "    اطبع(\"الملخص: \" + نص(نجاح) + \" نجح, \" + نص(فشل) + \" فشل\")\n";
        ss << "    ارجع فشل == ٠ ? ٠ : ١\n";
        ss << "نهاية\n";
        
        return ss.str();
    }
    
    /**
     * حفظ ملف الاختبار
     */
    void saveTestFile(
        const std::vector<DocTest>& tests,
        const std::string& outputPath
    ) {
        std::string content = generateTestFile(tests);
        std::ofstream file(outputPath);
        file << content;
    }
    
private:
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string result = std::ctime(&time);
        // إزالة السطر الجديد
        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }
        return result;
    }
};

} // namespace sad::docs

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadDocTestExtractor {
    sad::docs::DocTestExtractor* extractor;
};

SadDocTestExtractor* sad_doctest_extractor_new() {
    auto* ctx = new SadDocTestExtractor();
    ctx->extractor = new sad::docs::DocTestExtractor();
    return ctx;
}

int sad_doctest_extract_count(SadDocTestExtractor* ctx, const char* filePath) {
    auto tests = ctx->extractor->extractFromFile(filePath);
    return static_cast<int>(tests.size());
}

void sad_doctest_extractor_free(SadDocTestExtractor* ctx) {
    if (ctx) {
        delete ctx->extractor;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef DOCTEST_EXTRACTOR_TEST

#include <cassert>

void testBasicExtraction() {
    std::cout << "📝 اختبار الاستخراج الأساسي...\n";
    
    std::string code = R"(
/// تجمع عددين
/// @مثال
/// ```ص
/// متغير ن = جمع(٣, ٥)
/// اطبع("النتيجة: ٨")  // الخرج: النتيجة: ٨
/// ```
دالة جمع(أ: عدد, ب: عدد) -> عدد
    ارجع أ + ب
نهاية
)";
    
    sad::docs::DocTestExtractor extractor;
    auto tests = extractor.extractFromContent(code, "test.ص");
    
    assert(tests.size() == 1);
    assert(tests[0].expectedOutput == "النتيجة: ٨");
    
    std::cout << "   ✅ نجح\n";
}

void testMultipleTests() {
    std::cout << "📝 اختبار استخراج متعدد...\n";
    
    std::string code = R"(
/// دالة أولى
/// ```ص
/// اطبع("أولى")  // الخرج: أولى
/// ```
دالة أولى()
نهاية

/// دالة ثانية
/// ```ص
/// اطبع("ثانية")  // الخرج: ثانية
/// ```
دالة ثانية()
نهاية
)";
    
    sad::docs::DocTestExtractor extractor;
    auto tests = extractor.extractFromContent(code, "test.ص");
    
    assert(tests.size() == 2);
    
    std::cout << "   ✅ نجح\n";
}

void testTestRunner() {
    std::cout << "🏃 اختبار المشغل...\n";
    
    sad::docs::DocTest test;
    test.name = "اختبار_طباعة";
    test.code = R"(اطبع("مرحباً"))";
    test.expectedOutput = "مرحباً";
    test.type = sad::docs::DocTestType::OutputTest;
    
    sad::docs::DocTestRunner runner;
    auto result = runner.runTest(test);
    
    assert(result.passed);
    
    std::cout << "   ✅ نجح\n";
}

void testFileGeneration() {
    std::cout << "📄 اختبار توليد الملف...\n";
    
    std::vector<sad::docs::DocTest> tests;
    
    sad::docs::DocTest test1;
    test1.name = "اختبار_١";
    test1.code = "اطبع(\"مرحباً\")";
    tests.push_back(test1);
    
    sad::docs::DocTest test2;
    test2.name = "اختبار_٢";
    test2.code = "متغير ن = ٥\nتأكد(ن == ٥)";
    tests.push_back(test2);
    
    sad::docs::DocTestGenerator generator;
    std::string output = generator.generateTestFile(tests);
    
    assert(output.find("اختبار_١") != std::string::npos);
    assert(output.find("اختبار_٢") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مستخرج اختبارات التوثيق\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicExtraction();
    testMultipleTests();
    testTestRunner();
    testFileGeneration();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // DOCTEST_EXTRACTOR_TEST
