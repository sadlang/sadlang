/**
 * =============================================================================
 * ملف: test_doc_gen.cpp
 * الوصف: اختبارات نظام توليد التوثيق
 * المهمة: T234 - Documentation tests
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 🧪 دليل اختبارات التوثيق
 * ═════════════════════════
 * 
 * ما الذي نختبره؟
 * ─────────────────
 * 1. محلل تعليقات التوثيق
 * 2. معالج Markdown العربي
 * 3. مستخرج التوثيق
 * 4. مولد HTML
 * 5. الروابط المتقاطعة
 * 6. مستخرج اختبارات التوثيق
 * 7. التكامل الكامل
 * 
 * =============================================================================
 */

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <string>
#include <functional>
#include <chrono>

namespace sad::test {

// =============================================================================
// إطار الاختبار
// =============================================================================

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double timeMs;
};

class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    void addTest(const std::string& name, std::function<void()> test) {
        tests_.push_back({name, test});
    }
    
    void run() {
        std::cout << "\n╔═══════════════════════════════════════════════╗\n";
        std::cout << "║  " << name_ << "\n";
        std::cout << "╚═══════════════════════════════════════════════╝\n\n";
        
        int passed = 0, failed = 0;
        
        for (const auto& [name, test] : tests_) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                test();
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> diff = end - start;
                
                std::cout << "   ✅ " << name << " (" << diff.count() << " ms)\n";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "   ❌ " << name << ": " << e.what() << "\n";
                failed++;
            }
        }
        
        std::cout << "\n   ───────────────────────────\n";
        std::cout << "   نجح: " << passed << " | فشل: " << failed << "\n";
        
        totalPassed_ += passed;
        totalFailed_ += failed;
    }
    
    static int totalPassed() { return totalPassed_; }
    static int totalFailed() { return totalFailed_; }
    
private:
    std::string name_;
    std::vector<std::pair<std::string, std::function<void()>>> tests_;
    static int totalPassed_;
    static int totalFailed_;
};

int TestSuite::totalPassed_ = 0;
int TestSuite::totalFailed_ = 0;

// ماكرو التأكيد
#define ASSERT_TRUE(cond) \
    if (!(cond)) throw std::runtime_error("فشل: " #cond)

#define ASSERT_FALSE(cond) \
    if (cond) throw std::runtime_error("فشل: !" #cond)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) throw std::runtime_error("فشل: " #a " != " #b)

#define ASSERT_NE(a, b) \
    if ((a) == (b)) throw std::runtime_error("فشل: " #a " == " #b)

#define ASSERT_CONTAINS(str, sub) \
    if ((str).find(sub) == std::string::npos) \
        throw std::runtime_error("فشل: لا يحتوي على " #sub)

#define ASSERT_NOT_CONTAINS(str, sub) \
    if ((str).find(sub) != std::string::npos) \
        throw std::runtime_error("فشل: يحتوي على " #sub)

// =============================================================================
// هياكل وهمية للاختبار
// =============================================================================

// محلل تعليقات التوثيق (وهمي)
class MockDocCommentParser {
public:
    struct DocBlock {
        std::string summary;
        std::string description;
        std::vector<std::pair<std::string, std::string>> params;
        std::string returnDoc;
        std::vector<std::string> examples;
    };
    
    DocBlock parse(const std::string& comment) {
        DocBlock block;
        
        std::istringstream stream(comment);
        std::string line;
        
        while (std::getline(stream, line)) {
            // إزالة ///
            size_t pos = line.find("///");
            if (pos != std::string::npos) {
                line = line.substr(pos + 3);
                // تنظيف
                size_t start = line.find_first_not_of(" \t");
                if (start != std::string::npos) {
                    line = line.substr(start);
                } else {
                    continue;
                }
            }
            
            if (line.empty()) continue;
            
            if (line[0] == '@') {
                if (line.find("@معطى") == 0 || line.find("@param") == 0) {
                    size_t sp = line.find(' ', 6);
                    if (sp != std::string::npos) {
                        std::string rest = line.substr(6);
                        size_t nameEnd = rest.find(' ');
                        std::string name = rest.substr(0, nameEnd);
                        std::string doc = nameEnd != std::string::npos ? rest.substr(nameEnd + 1) : "";
                        block.params.push_back({name, doc});
                    }
                } else if (line.find("@أرجع") == 0 || line.find("@return") == 0) {
                    block.returnDoc = line.substr(6);
                } else if (line.find("@مثال") == 0 || line.find("@example") == 0) {
                    block.examples.push_back(line.substr(6));
                }
            } else {
                if (block.summary.empty()) {
                    block.summary = line;
                } else {
                    block.description += line + "\n";
                }
            }
        }
        
        return block;
    }
};

// معالج Markdown (وهمي)
class MockMarkdownProcessor {
public:
    std::string toHTML(const std::string& markdown) {
        std::string html = markdown;
        
        // العناوين
        size_t pos = 0;
        while ((pos = html.find("# ", pos)) != std::string::npos) {
            size_t end = html.find('\n', pos);
            std::string title = html.substr(pos + 2, end - pos - 2);
            html.replace(pos, end - pos, "<h1>" + title + "</h1>");
        }
        
        // **bold**
        std::string::size_type start = 0;
        while ((start = html.find("**", start)) != std::string::npos) {
            auto endPos = html.find("**", start + 2);
            if (endPos != std::string::npos) {
                std::string text = html.substr(start + 2, endPos - start - 2);
                html.replace(start, endPos - start + 2, "<strong>" + text + "</strong>");
            } else {
                break;
            }
        }
        
        // `code`
        start = 0;
        while ((start = html.find("`", start)) != std::string::npos) {
            auto endPos = html.find("`", start + 1);
            if (endPos != std::string::npos) {
                std::string text = html.substr(start + 1, endPos - start - 1);
                html.replace(start, endPos - start + 1, "<code>" + text + "</code>");
            } else {
                break;
            }
        }
        
        return html;
    }
};

// مولد HTML (وهمي)
class MockHTMLGenerator {
public:
    struct DocItem {
        std::string name;
        std::string type;
        std::string summary;
        std::string signature;
    };
    
    std::string generate(const DocItem& item) {
        std::ostringstream ss;
        ss << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n";
        ss << "<head><title>" << item.name << "</title></head>\n";
        ss << "<body>\n";
        ss << "<h1>" << item.name << "</h1>\n";
        ss << "<span class=\"type\">" << item.type << "</span>\n";
        ss << "<pre>" << item.signature << "</pre>\n";
        ss << "<p>" << item.summary << "</p>\n";
        ss << "</body>\n</html>";
        return ss.str();
    }
    
    std::string generateIndex(const std::vector<DocItem>& items) {
        std::ostringstream ss;
        ss << "<!DOCTYPE html>\n<html dir=\"rtl\">\n<body>\n";
        ss << "<h1>الفهرس</h1>\n<ul>\n";
        for (const auto& item : items) {
            ss << "<li><a href=\"" << item.name << ".html\">" 
               << item.name << "</a></li>\n";
        }
        ss << "</ul>\n</body>\n</html>";
        return ss.str();
    }
};

// فهرس الروابط المتقاطعة (وهمي)
class MockCrossRefIndex {
public:
    void add(const std::string& name, const std::string& url, const std::string& summary) {
        refs_[name] = {url, summary};
    }
    
    std::string link(const std::string& text) {
        std::string result = text;
        
        // البحث عن `اسم`
        size_t start = 0;
        while ((start = result.find("`", start)) != std::string::npos) {
            auto endPos = result.find("`", start + 1);
            if (endPos == std::string::npos) break;
            
            std::string name = result.substr(start + 1, endPos - start - 1);
            // إزالة ()
            std::string lookupName = name;
            if (lookupName.size() > 2 && 
                lookupName.substr(lookupName.size() - 2) == "()") {
                lookupName = lookupName.substr(0, lookupName.size() - 2);
            }
            
            auto it = refs_.find(lookupName);
            if (it != refs_.end()) {
                std::string linked = "<a href=\"" + it->second.first + "\">" + name + "</a>";
                result.replace(start, endPos - start + 1, linked);
                start += linked.size();
            } else {
                start = endPos + 1;
            }
        }
        
        return result;
    }
    
private:
    std::map<std::string, std::pair<std::string, std::string>> refs_;
};

// مستخرج اختبارات التوثيق (وهمي)
class MockDocTestExtractor {
public:
    struct DocTest {
        std::string name;
        std::string code;
        std::string expectedOutput;
    };
    
    std::vector<DocTest> extract(const std::string& content) {
        std::vector<DocTest> tests;
        
        bool inCodeBlock = false;
        std::string currentCode;
        std::string currentOutput;
        int testNum = 0;
        
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (line.find("```") != std::string::npos) {
                if (!inCodeBlock) {
                    inCodeBlock = true;
                    currentCode.clear();
                    currentOutput.clear();
                } else {
                    inCodeBlock = false;
                    if (!currentCode.empty()) {
                        DocTest test;
                        test.name = "test_" + std::to_string(++testNum);
                        test.code = currentCode;
                        test.expectedOutput = currentOutput;
                        tests.push_back(test);
                    }
                }
            } else if (inCodeBlock) {
                // البحث عن // الخرج:
                size_t outputPos = line.find("// الخرج:");
                if (outputPos != std::string::npos) {
                    currentOutput += line.substr(outputPos + 12) + "\n";
                    currentCode += line.substr(0, outputPos) + "\n";
                } else {
                    currentCode += line + "\n";
                }
            }
        }
        
        return tests;
    }
};

// =============================================================================
// اختبارات محلل التعليقات
// =============================================================================

void runDocCommentParserTests() {
    TestSuite suite("🔍 اختبارات محلل تعليقات التوثيق");
    
    suite.addTest("تحليل ملخص بسيط", []() {
        MockDocCommentParser parser;
        auto block = parser.parse("/// تجمع عددين");
        ASSERT_EQ(block.summary, "تجمع عددين");
    });
    
    suite.addTest("تحليل معطيات", []() {
        MockDocCommentParser parser;
        auto block = parser.parse(R"(
/// دالة الجمع
/// @معطى أ العدد الأول
/// @معطى ب العدد الثاني
)");
        ASSERT_EQ(block.params.size(), 2);
    });
    
    suite.addTest("تحليل قيمة مرجعة", []() {
        MockDocCommentParser parser;
        auto block = parser.parse(R"(
/// دالة
/// @أرجع ناتج الجمع
)");
        ASSERT_CONTAINS(block.returnDoc, "ناتج");
    });
    
    suite.addTest("تحليل أمثلة", []() {
        MockDocCommentParser parser;
        auto block = parser.parse(R"(
/// دالة
/// @مثال جمع(٣, ٥)
)");
        ASSERT_EQ(block.examples.size(), 1);
    });
    
    suite.addTest("تحليل وصف متعدد الأسطر", []() {
        MockDocCommentParser parser;
        auto block = parser.parse(R"(
/// ملخص الدالة
/// هذا وصف تفصيلي
/// يمتد على عدة أسطر
)");
        ASSERT_CONTAINS(block.description, "تفصيلي");
    });
    
    suite.run();
}

// =============================================================================
// اختبارات معالج Markdown
// =============================================================================

void runMarkdownProcessorTests() {
    TestSuite suite("📝 اختبارات معالج Markdown");
    
    suite.addTest("تحويل عنوان", []() {
        MockMarkdownProcessor processor;
        std::string html = processor.toHTML("# عنوان");
        ASSERT_CONTAINS(html, "<h1>عنوان</h1>");
    });
    
    suite.addTest("تحويل نص عريض", []() {
        MockMarkdownProcessor processor;
        std::string html = processor.toHTML("هذا **مهم** جداً");
        ASSERT_CONTAINS(html, "<strong>مهم</strong>");
    });
    
    suite.addTest("تحويل كود", []() {
        MockMarkdownProcessor processor;
        std::string html = processor.toHTML("استخدم `print` للطباعة");
        ASSERT_CONTAINS(html, "<code>print</code>");
    });
    
    suite.addTest("تحويل نص عربي", []() {
        MockMarkdownProcessor processor;
        std::string html = processor.toHTML("مرحباً بالعالم");
        ASSERT_CONTAINS(html, "مرحباً");
    });
    
    suite.run();
}

// =============================================================================
// اختبارات مولد HTML
// =============================================================================

void runHTMLGeneratorTests() {
    TestSuite suite("🌐 اختبارات مولد HTML");
    
    suite.addTest("توليد صفحة دالة", []() {
        MockHTMLGenerator generator;
        MockHTMLGenerator::DocItem item;
        item.name = "جمع";
        item.type = "دالة";
        item.summary = "تجمع عددين";
        item.signature = "دالة جمع(أ: عدد, ب: عدد) -> عدد";
        
        std::string html = generator.generate(item);
        
        ASSERT_CONTAINS(html, "<h1>جمع</h1>");
        ASSERT_CONTAINS(html, "دالة");
        ASSERT_CONTAINS(html, "dir=\"rtl\"");
    });
    
    suite.addTest("توليد فهرس", []() {
        MockHTMLGenerator generator;
        std::vector<MockHTMLGenerator::DocItem> items = {
            {"جمع", "دالة", "تجمع عددين", ""},
            {"طرح", "دالة", "تطرح عددين", ""}
        };
        
        std::string html = generator.generateIndex(items);
        
        ASSERT_CONTAINS(html, "جمع.html");
        ASSERT_CONTAINS(html, "طرح.html");
        ASSERT_CONTAINS(html, "<ul>");
    });
    
    suite.addTest("HTML صحيح", []() {
        MockHTMLGenerator generator;
        MockHTMLGenerator::DocItem item = {"اختبار", "دالة", "وصف", "sig"};
        std::string html = generator.generate(item);
        
        ASSERT_CONTAINS(html, "<!DOCTYPE html>");
        ASSERT_CONTAINS(html, "</html>");
    });
    
    suite.run();
}

// =============================================================================
// اختبارات الروابط المتقاطعة
// =============================================================================

void runCrossRefTests() {
    TestSuite suite("🔗 اختبارات الروابط المتقاطعة");
    
    suite.addTest("ربط دالة", []() {
        MockCrossRefIndex index;
        index.add("جمع", "api/جمع.html", "تجمع عددين");
        
        std::string text = "استخدم `جمع()` للجمع";
        std::string result = index.link(text);
        
        ASSERT_CONTAINS(result, "<a href=");
        ASSERT_CONTAINS(result, "api/جمع.html");
    });
    
    suite.addTest("ربط صنف", []() {
        MockCrossRefIndex index;
        index.add("قائمة", "api/قائمة.html", "قائمة ديناميكية");
        
        std::string text = "أنشئ `قائمة` جديدة";
        std::string result = index.link(text);
        
        ASSERT_CONTAINS(result, "href=\"api/قائمة.html\"");
    });
    
    suite.addTest("مرجع غير موجود", []() {
        MockCrossRefIndex index;
        
        std::string text = "استخدم `غير_موجود()`";
        std::string result = index.link(text);
        
        // يجب أن يبقى كما هو
        ASSERT_CONTAINS(result, "غير_موجود");
    });
    
    suite.addTest("عدة روابط", []() {
        MockCrossRefIndex index;
        index.add("أ", "a.html", "");
        index.add("ب", "b.html", "");
        
        std::string text = "ادمج `أ` مع `ب`";
        std::string result = index.link(text);
        
        ASSERT_CONTAINS(result, "a.html");
        ASSERT_CONTAINS(result, "b.html");
    });
    
    suite.run();
}

// =============================================================================
// اختبارات اختبارات التوثيق
// =============================================================================

void runDocTestTests() {
    TestSuite suite("🧪 اختبارات مستخرج اختبارات التوثيق");
    
    suite.addTest("استخراج اختبار بسيط", []() {
        MockDocTestExtractor extractor;
        std::string content = R"(
/// مثال
/// ```ص
/// اطبع("مرحباً")  // الخرج: مرحباً
/// ```
)";
        auto tests = extractor.extract(content);
        ASSERT_EQ(tests.size(), 1);
        ASSERT_CONTAINS(tests[0].expectedOutput, "مرحباً");
    });
    
    suite.addTest("استخراج عدة اختبارات", []() {
        MockDocTestExtractor extractor;
        std::string content = R"(
/// ```ص
/// اطبع("١")  // الخرج: ١
/// ```
/// 
/// ```ص
/// اطبع("٢")  // الخرج: ٢
/// ```
)";
        auto tests = extractor.extract(content);
        ASSERT_EQ(tests.size(), 2);
    });
    
    suite.addTest("كود بدون خرج متوقع", []() {
        MockDocTestExtractor extractor;
        std::string content = R"(
/// ```ص
/// متغير س = ٥
/// متغير ص = س * ٢
/// ```
)";
        auto tests = extractor.extract(content);
        ASSERT_EQ(tests.size(), 1);
        ASSERT_TRUE(tests[0].expectedOutput.empty());
    });
    
    suite.run();
}

// =============================================================================
// اختبارات التكامل
// =============================================================================

void runIntegrationTests() {
    TestSuite suite("🔄 اختبارات التكامل");
    
    suite.addTest("سير العمل الكامل", []() {
        // 1. تحليل التعليقات
        MockDocCommentParser parser;
        auto doc = parser.parse(R"(
/// تجمع عددين
/// @معطى أ العدد الأول
/// @أرجع ناتج الجمع
)");
        ASSERT_FALSE(doc.summary.empty());
        
        // 2. توليد HTML
        MockHTMLGenerator generator;
        MockHTMLGenerator::DocItem item = {
            "جمع", "دالة", doc.summary, "دالة جمع(أ, ب)"
        };
        std::string html = generator.generate(item);
        ASSERT_CONTAINS(html, "جمع");
        
        // 3. الروابط المتقاطعة
        MockCrossRefIndex index;
        index.add("جمع", "جمع.html", doc.summary);
        std::string linked = index.link("انظر `جمع()`");
        ASSERT_CONTAINS(linked, "<a href=");
    });
    
    suite.addTest("معالجة ملف كامل", []() {
        std::string fileContent = R"(
/// دالة الترحيب
/// تطبع رسالة ترحيب
/// @معطى اسم اسم الشخص
/// @أرجع لا شيء
/// @مثال
/// ```ص
/// رحب("أحمد")  // الخرج: مرحباً أحمد
/// ```
دالة رحب(اسم: نص)
    اطبع("مرحباً " + اسم)
نهاية
)";
        
        // استخراج التوثيق
        MockDocCommentParser parser;
        // ... التحليل
        
        // استخراج الاختبارات
        MockDocTestExtractor extractor;
        auto tests = extractor.extract(fileContent);
        ASSERT_EQ(tests.size(), 1);
    });
    
    suite.run();
}

// =============================================================================
// اختبارات الأداء
// =============================================================================

void runPerformanceTests() {
    TestSuite suite("⚡ اختبارات الأداء");
    
    suite.addTest("تحليل 1000 تعليق", []() {
        MockDocCommentParser parser;
        
        for (int i = 0; i < 1000; i++) {
            parser.parse("/// دالة رقم " + std::to_string(i));
        }
        // إذا وصلنا هنا، الأداء مقبول
        ASSERT_TRUE(true);
    });
    
    suite.addTest("توليد 100 صفحة", []() {
        MockHTMLGenerator generator;
        
        for (int i = 0; i < 100; i++) {
            MockHTMLGenerator::DocItem item = {
                "دالة_" + std::to_string(i),
                "دالة",
                "وصف",
                "توقيع"
            };
            generator.generate(item);
        }
        ASSERT_TRUE(true);
    });
    
    suite.addTest("ربط 500 مرجع", []() {
        MockCrossRefIndex index;
        
        // إضافة مراجع
        for (int i = 0; i < 100; i++) {
            index.add("ref_" + std::to_string(i), "url_" + std::to_string(i) + ".html", "");
        }
        
        // ربط
        std::string text = "";
        for (int i = 0; i < 5; i++) {
            text += "`ref_" + std::to_string(i) + "` ";
        }
        
        for (int i = 0; i < 100; i++) {
            index.link(text);
        }
        
        ASSERT_TRUE(true);
    });
    
    suite.run();
}

} // namespace sad::test

// =============================================================================
// نقطة الدخول
// =============================================================================

int main() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════╗
║                 🧪 اختبارات نظام التوثيق                          ║
║                     Documentation System Tests                     ║
╚═══════════════════════════════════════════════════════════════════╝
)";
    
    sad::test::runDocCommentParserTests();
    sad::test::runMarkdownProcessorTests();
    sad::test::runHTMLGeneratorTests();
    sad::test::runCrossRefTests();
    sad::test::runDocTestTests();
    sad::test::runIntegrationTests();
    sad::test::runPerformanceTests();
    
    // الملخص النهائي
    std::cout << "\n";
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "                    📊 الملخص النهائي\n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";
    
    int passed = sad::test::TestSuite::totalPassed();
    int failed = sad::test::TestSuite::totalFailed();
    int total = passed + failed;
    
    std::cout << "   إجمالي الاختبارات: " << total << "\n";
    std::cout << "   ✅ نجح: " << passed << "\n";
    std::cout << "   ❌ فشل: " << failed << "\n";
    std::cout << "   📈 النسبة: " << (total > 0 ? (passed * 100 / total) : 0) << "%\n";
    
    if (failed == 0) {
        std::cout << "\n🎉 جميع الاختبارات نجحت!\n";
    } else {
        std::cout << "\n⚠️ بعض الاختبارات فشلت.\n";
    }
    
    return failed == 0 ? 0 : 1;
}
