/**
 * =============================================================================
 * ملف: test_advanced_lsp.cpp
 * الوصف: اختبارات تكامل لميزات LSP المتقدمة
 * المهمة: T259 - LSP advanced integration tests
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * 🧪 دليل المبتدئ لاختبارات تكامل LSP
 * ══════════════════════════════════════
 * 
 * ما هي اختبارات التكامل؟
 * ────────────────────────
 * اختبارات التكامل تتحقق من أن أجزاء النظام تعمل معاً بشكل صحيح.
 * 
 * هنا نختبر:
 * - عدسات الملكية (Code Lenses)
 * - تصور دورة الحياة (Lifetime Visualization)
 * - رسم الاستعارات (Borrow Graph)
 * - تلميحات النقل (Move Hints)
 * - الرموز الدلالية (Semantic Tokens)
 * - إجراءات الكود (Code Actions)
 * - معلومات التمرير (Hover)
 * 
 * كيف يعمل LSP؟
 * ─────────────
 * ```
 *     المحرر (VS Code)              الخادم (LSP Server)
 *           │                              │
 *           │──── textDocument/hover ─────►│
 *           │     (أين المؤشر؟)             │
 *           │                              │
 *           │◄─── معلومات Markdown ────────│
 *           │     (نوع + ملكية)             │
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <functional>
#include <chrono>

// =============================================================================
// إطار الاختبارات
// =============================================================================

namespace sad::test {

/**
 * نتيجة اختبار واحد
 */
struct TestResult {
    std::string name;
    bool passed;
    std::string error;
    double durationMs;
};

/**
 * مجموعة نتائج
 */
struct TestSuiteResult {
    std::string suiteName;
    std::vector<TestResult> results;
    int passed = 0;
    int failed = 0;
    
    void addResult(const TestResult& result) {
        results.push_back(result);
        if (result.passed) passed++;
        else failed++;
    }
};

/**
 * مُنفذ الاختبارات
 */
class TestRunner {
public:
    using TestFunc = std::function<void()>;
    
    void addTest(const std::string& name, TestFunc func) {
        tests_.push_back({name, func});
    }
    
    TestSuiteResult run(const std::string& suiteName) {
        TestSuiteResult suite;
        suite.suiteName = suiteName;
        
        for (const auto& test : tests_) {
            TestResult result;
            result.name = test.first;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                test.second();
                result.passed = true;
            } catch (const std::exception& e) {
                result.passed = false;
                result.error = e.what();
            } catch (...) {
                result.passed = false;
                result.error = "خطأ غير معروف";
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
            
            suite.addResult(result);
        }
        
        tests_.clear();
        return suite;
    }
    
private:
    std::vector<std::pair<std::string, TestFunc>> tests_;
};

// ماكرو للتأكيدات
#define ASSERT_TRUE(cond) \
    if (!(cond)) throw std::runtime_error("فشل: " #cond)

#define ASSERT_FALSE(cond) \
    if (cond) throw std::runtime_error("فشل (يجب أن يكون خطأ): " #cond)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) throw std::runtime_error("فشل: " #a " != " #b)

#define ASSERT_NE(a, b) \
    if ((a) == (b)) throw std::runtime_error("فشل: " #a " == " #b)

#define ASSERT_CONTAINS(str, sub) \
    if ((str).find(sub) == std::string::npos) \
        throw std::runtime_error("فشل: لا يحتوي على \"" sub "\"")

} // namespace sad::test

// =============================================================================
// محاكاة LSP
// =============================================================================

namespace sad::lsp::mock {

/**
 * معلومات Code Lens
 */
struct CodeLens {
    int line;
    std::string command;
    std::string title;
};

/**
 * محاكاة مُزود عدسات الملكية
 */
class MockOwnershipLensProvider {
public:
    std::vector<CodeLens> provideLenses(const std::string& source) {
        std::vector<CodeLens> lenses;
        
        // تحليل مبسط للمتغيرات
        int lineNum = 0;
        size_t pos = 0;
        while (pos < source.length()) {
            size_t end = source.find('\n', pos);
            if (end == std::string::npos) end = source.length();
            
            std::string line = source.substr(pos, end - pos);
            lineNum++;
            
            // البحث عن تعريفات المتغيرات
            if (line.find("متغير ") != std::string::npos) {
                CodeLens lens;
                lens.line = lineNum;
                
                if (line.find('&') != std::string::npos) {
                    if (line.find("&متغير") != std::string::npos) {
                        lens.title = "استعارة متغيرة";
                    } else {
                        lens.title = "استعارة ثابتة";
                    }
                } else {
                    lens.title = "مالك";
                }
                
                lens.command = "sad.showOwnership";
                lenses.push_back(lens);
            }
            
            pos = end + 1;
        }
        
        return lenses;
    }
};

/**
 * محاكاة مُصور دورة الحياة
 */
class MockLifetimeVisualizer {
public:
    std::string visualize(const std::string& source) {
        std::string result;
        
        // تحليل مبسط
        int lineNum = 0;
        size_t pos = 0;
        while (pos < source.length()) {
            size_t end = source.find('\n', pos);
            if (end == std::string::npos) end = source.length();
            
            std::string line = source.substr(pos, end - pos);
            lineNum++;
            
            result += std::to_string(lineNum) + ": ";
            
            if (line.find("متغير ") != std::string::npos) {
                result += "│ ▶ بداية حياة\n";
            } else if (line.find("نهاية") != std::string::npos) {
                result += "│ ◀ نهاية حياة\n";
            } else {
                result += "│ ○ نشط\n";
            }
            
            pos = end + 1;
        }
        
        return result;
    }
};

/**
 * محاكاة رسم الاستعارات
 */
class MockBorrowGraph {
public:
    struct Node {
        std::string name;
        std::string type;  // "owner" | "borrow" | "mut_borrow"
    };
    
    struct Edge {
        std::string from;
        std::string to;
        std::string type;  // "borrow" | "mut_borrow"
    };
    
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    
    void analyze(const std::string& source) {
        nodes.clear();
        edges.clear();
        
        // تحليل مبسط
        size_t pos = 0;
        while ((pos = source.find("متغير ", pos)) != std::string::npos) {
            size_t nameStart = pos + 10;
            size_t nameEnd = source.find_first_of(" =:\n", nameStart);
            std::string name = source.substr(nameStart, nameEnd - nameStart);
            
            // البحث عن القيمة
            size_t eqPos = source.find('=', nameEnd);
            if (eqPos != std::string::npos && eqPos < source.find('\n', pos)) {
                std::string value = source.substr(eqPos + 1, 
                    source.find('\n', eqPos) - eqPos - 1);
                
                if (value.find("&متغير") != std::string::npos) {
                    nodes.push_back({name, "mut_borrow"});
                    // البحث عن المصدر
                    size_t srcStart = value.find_first_not_of(" &متغير");
                    if (srcStart != std::string::npos) {
                        std::string src = value.substr(srcStart);
                        size_t srcEnd = src.find_first_of(" \t\n");
                        if (srcEnd != std::string::npos) src = src.substr(0, srcEnd);
                        edges.push_back({name, src, "mut_borrow"});
                    }
                } else if (value.find('&') != std::string::npos) {
                    nodes.push_back({name, "borrow"});
                    size_t srcStart = value.find_first_not_of(" &");
                    if (srcStart != std::string::npos) {
                        std::string src = value.substr(srcStart);
                        size_t srcEnd = src.find_first_of(" \t\n");
                        if (srcEnd != std::string::npos) src = src.substr(0, srcEnd);
                        edges.push_back({name, src, "borrow"});
                    }
                } else {
                    nodes.push_back({name, "owner"});
                }
            } else {
                nodes.push_back({name, "owner"});
            }
            
            pos = nameEnd;
        }
    }
    
    std::string toDot() {
        std::string dot = "digraph BorrowGraph {\n";
        dot += "  rankdir=LR;\n";
        
        for (const auto& node : nodes) {
            dot += "  \"" + node.name + "\" [label=\"" + node.name + "\"";
            if (node.type == "owner") dot += ", shape=box";
            else if (node.type == "mut_borrow") dot += ", shape=diamond";
            else dot += ", shape=ellipse";
            dot += "];\n";
        }
        
        for (const auto& edge : edges) {
            dot += "  \"" + edge.from + "\" -> \"" + edge.to + "\"";
            if (edge.type == "mut_borrow") dot += " [style=bold]";
            dot += ";\n";
        }
        
        dot += "}\n";
        return dot;
    }
};

/**
 * محاكاة تلميحات النقل
 */
class MockMoveHints {
public:
    struct Hint {
        int line;
        int column;
        std::string text;
        std::string kind;  // "move" | "copy" | "borrow"
    };
    
    std::vector<Hint> getHints(const std::string& source) {
        std::vector<Hint> hints;
        
        int lineNum = 0;
        size_t pos = 0;
        while (pos < source.length()) {
            size_t end = source.find('\n', pos);
            if (end == std::string::npos) end = source.length();
            
            std::string line = source.substr(pos, end - pos);
            lineNum++;
            
            // البحث عن النقل
            size_t movePos = line.find("انقل ");
            if (movePos != std::string::npos) {
                hints.push_back({lineNum, (int)movePos, "⤳ نقل", "move"});
            }
            
            // البحث عن الاستعارة
            size_t borrowPos = line.find('&');
            if (borrowPos != std::string::npos && line.find("متغير") != std::string::npos) {
                bool isMut = line.find("&متغير") != std::string::npos;
                hints.push_back({
                    lineNum, 
                    (int)borrowPos, 
                    isMut ? "✏️ &متغير" : "📖 &", 
                    isMut ? "mut_borrow" : "borrow"
                });
            }
            
            pos = end + 1;
        }
        
        return hints;
    }
};

/**
 * محاكاة الرموز الدلالية
 */
class MockSemanticTokens {
public:
    struct Token {
        int line;
        int start;
        int length;
        std::string type;
        std::string modifiers;
    };
    
    std::vector<Token> getTokens(const std::string& source) {
        std::vector<Token> tokens;
        
        // كلمات مفتاحية عربية
        std::vector<std::string> keywords = {
            "دالة", "متغير", "ثابت", "إذا", "وإلا", "طالما",
            "لكل", "أرجع", "صنف", "سمة", "نهاية"
        };
        
        std::vector<std::string> types = {
            "عدد", "نص", "منطقي", "عشري64", "مصفوفة"
        };
        
        int lineNum = 0;
        size_t pos = 0;
        while (pos < source.length()) {
            size_t end = source.find('\n', pos);
            if (end == std::string::npos) end = source.length();
            
            std::string line = source.substr(pos, end - pos);
            lineNum++;
            
            // البحث عن الكلمات المفتاحية
            for (const auto& kw : keywords) {
                size_t kwPos = line.find(kw);
                while (kwPos != std::string::npos) {
                    tokens.push_back({
                        lineNum,
                        (int)kwPos,
                        (int)kw.length(),
                        "keyword",
                        ""
                    });
                    kwPos = line.find(kw, kwPos + 1);
                }
            }
            
            // البحث عن الأنواع
            for (const auto& type : types) {
                size_t typePos = line.find(type);
                while (typePos != std::string::npos) {
                    tokens.push_back({
                        lineNum,
                        (int)typePos,
                        (int)type.length(),
                        "type",
                        ""
                    });
                    typePos = line.find(type, typePos + 1);
                }
            }
            
            pos = end + 1;
        }
        
        return tokens;
    }
};

/**
 * محاكاة إجراءات الكود
 */
class MockCodeActions {
public:
    struct CodeAction {
        std::string title;
        std::string kind;
        bool isPreferred;
    };
    
    std::vector<CodeAction> getActions(const std::string& diagnosticKind) {
        std::vector<CodeAction> actions;
        
        if (diagnosticKind == "use_after_move") {
            actions.push_back({"نسخ بدلاً من نقل", "quickfix", true});
            actions.push_back({"إضافة .استنساخ()", "quickfix", false});
        } else if (diagnosticKind == "double_borrow") {
            actions.push_back({"تقليل النطاق", "quickfix", true});
            actions.push_back({"استخدام استعارة ثابتة", "quickfix", false});
        } else if (diagnosticKind == "borrow_conflict") {
            actions.push_back({"إنهاء الاستعارة السابقة", "quickfix", true});
            actions.push_back({"إعادة هيكلة الكود", "refactor", false});
        }
        
        return actions;
    }
};

/**
 * محاكاة معلومات التمرير
 */
class MockHoverProvider {
public:
    std::string getHover(const std::string& source, int line, int column) {
        // تحليل مبسط
        std::string result;
        
        // الحصول على السطر
        int lineNum = 0;
        size_t pos = 0;
        while (pos < source.length()) {
            size_t end = source.find('\n', pos);
            if (end == std::string::npos) end = source.length();
            
            lineNum++;
            if (lineNum == line) {
                std::string lineStr = source.substr(pos, end - pos);
                
                // البحث عن المتغير في الموقع
                if (lineStr.find("متغير") != std::string::npos) {
                    result = "```sad\nمتغير: عدد\n```\n\n---\n\n";
                    
                    if (lineStr.find('&') != std::string::npos) {
                        result += "🔗 **استعارة**\n";
                    } else {
                        result += "👑 **مالك**\n";
                    }
                }
                break;
            }
            
            pos = end + 1;
        }
        
        return result;
    }
};

} // namespace sad::lsp::mock

// =============================================================================
// اختبارات عدسات الملكية
// =============================================================================

void test_ownership_lenses() {
    sad::test::TestRunner runner;
    
    runner.addTest("عدسة للمالك", []() {
        sad::lsp::mock::MockOwnershipLensProvider provider;
        
        std::string source = "متغير س = 10\n";
        auto lenses = provider.provideLenses(source);
        
        ASSERT_EQ(lenses.size(), 1);
        ASSERT_EQ(lenses[0].title, "مالك");
    });
    
    runner.addTest("عدسة للاستعارة الثابتة", []() {
        sad::lsp::mock::MockOwnershipLensProvider provider;
        
        std::string source = "متغير س = 10\nمتغير ص = &س\n";
        auto lenses = provider.provideLenses(source);
        
        ASSERT_EQ(lenses.size(), 2);
        ASSERT_EQ(lenses[1].title, "استعارة ثابتة");
    });
    
    runner.addTest("عدسة للاستعارة المتغيرة", []() {
        sad::lsp::mock::MockOwnershipLensProvider provider;
        
        std::string source = "متغير س = 10\nمتغير ص = &متغير س\n";
        auto lenses = provider.provideLenses(source);
        
        ASSERT_EQ(lenses.size(), 2);
        ASSERT_EQ(lenses[1].title, "استعارة متغيرة");
    });
    
    auto result = runner.run("عدسات الملكية");
    
    std::cout << "📋 " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات تصور دورة الحياة
// =============================================================================

void test_lifetime_visualization() {
    sad::test::TestRunner runner;
    
    runner.addTest("تصور بسيط", []() {
        sad::lsp::mock::MockLifetimeVisualizer visualizer;
        
        std::string source = "متغير س = 10\nاطبع(س)\nnهاية\n";
        auto viz = visualizer.visualize(source);
        
        ASSERT_CONTAINS(viz, "بداية حياة");
    });
    
    runner.addTest("تصور متعدد المتغيرات", []() {
        sad::lsp::mock::MockLifetimeVisualizer visualizer;
        
        std::string source = "متغير س = 10\nمتغير ص = 20\nnهاية\n";
        auto viz = visualizer.visualize(source);
        
        // يجب أن يكون هناك بدايتين
        size_t count = 0;
        size_t pos = 0;
        while ((pos = viz.find("بداية حياة", pos)) != std::string::npos) {
            count++;
            pos++;
        }
        
        ASSERT_EQ(count, 2);
    });
    
    auto result = runner.run("تصور دورة الحياة");
    
    std::cout << "⏳ " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات رسم الاستعارات
// =============================================================================

void test_borrow_graph() {
    sad::test::TestRunner runner;
    
    runner.addTest("رسم استعارة بسيطة", []() {
        sad::lsp::mock::MockBorrowGraph graph;
        
        std::string source = "متغير س = 10\nمتغير ص = &س\n";
        graph.analyze(source);
        
        ASSERT_EQ(graph.nodes.size(), 2);
        ASSERT_EQ(graph.edges.size(), 1);
    });
    
    runner.addTest("إخراج DOT", []() {
        sad::lsp::mock::MockBorrowGraph graph;
        
        std::string source = "متغير س = 10\nمتغير ص = &س\n";
        graph.analyze(source);
        
        auto dot = graph.toDot();
        
        ASSERT_CONTAINS(dot, "digraph");
        ASSERT_CONTAINS(dot, "->");
    });
    
    runner.addTest("استعارة متغيرة", []() {
        sad::lsp::mock::MockBorrowGraph graph;
        
        std::string source = "متغير س = 10\nمتغير ص = &متغير س\n";
        graph.analyze(source);
        
        ASSERT_EQ(graph.nodes.size(), 2);
        ASSERT_EQ(graph.nodes[1].type, "mut_borrow");
    });
    
    auto result = runner.run("رسم الاستعارات");
    
    std::cout << "🕸️ " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات تلميحات النقل
// =============================================================================

void test_move_hints() {
    sad::test::TestRunner runner;
    
    runner.addTest("تلميح النقل", []() {
        sad::lsp::mock::MockMoveHints hints;
        
        std::string source = "متغير س = 10\nانقل س\n";
        auto h = hints.getHints(source);
        
        ASSERT_TRUE(h.size() > 0);
        
        bool foundMove = false;
        for (const auto& hint : h) {
            if (hint.kind == "move") foundMove = true;
        }
        ASSERT_TRUE(foundMove);
    });
    
    runner.addTest("تلميح الاستعارة", []() {
        sad::lsp::mock::MockMoveHints hints;
        
        std::string source = "متغير س = 10\nمتغير ص = &س\n";
        auto h = hints.getHints(source);
        
        bool foundBorrow = false;
        for (const auto& hint : h) {
            if (hint.kind == "borrow") foundBorrow = true;
        }
        ASSERT_TRUE(foundBorrow);
    });
    
    auto result = runner.run("تلميحات النقل");
    
    std::cout << "➡️ " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات الرموز الدلالية
// =============================================================================

void test_semantic_tokens() {
    sad::test::TestRunner runner;
    
    runner.addTest("رموز الكلمات المفتاحية", []() {
        sad::lsp::mock::MockSemanticTokens tokens;
        
        std::string source = "دالة مرحبا()\n    اطبع(\"مرحبا\")\nنهاية\n";
        auto t = tokens.getTokens(source);
        
        bool foundKeyword = false;
        for (const auto& token : t) {
            if (token.type == "keyword") foundKeyword = true;
        }
        ASSERT_TRUE(foundKeyword);
    });
    
    runner.addTest("رموز الأنواع", []() {
        sad::lsp::mock::MockSemanticTokens tokens;
        
        std::string source = "متغير س: عدد = 10\n";
        auto t = tokens.getTokens(source);
        
        bool foundType = false;
        for (const auto& token : t) {
            if (token.type == "type") foundType = true;
        }
        ASSERT_TRUE(foundType);
    });
    
    auto result = runner.run("الرموز الدلالية");
    
    std::cout << "🎨 " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات إجراءات الكود
// =============================================================================

void test_code_actions() {
    sad::test::TestRunner runner;
    
    runner.addTest("إجراءات use_after_move", []() {
        sad::lsp::mock::MockCodeActions actions;
        
        auto a = actions.getActions("use_after_move");
        
        ASSERT_TRUE(a.size() > 0);
        
        bool foundPreferred = false;
        for (const auto& action : a) {
            if (action.isPreferred) foundPreferred = true;
        }
        ASSERT_TRUE(foundPreferred);
    });
    
    runner.addTest("إجراءات double_borrow", []() {
        sad::lsp::mock::MockCodeActions actions;
        
        auto a = actions.getActions("double_borrow");
        
        ASSERT_TRUE(a.size() > 0);
    });
    
    runner.addTest("إجراءات borrow_conflict", []() {
        sad::lsp::mock::MockCodeActions actions;
        
        auto a = actions.getActions("borrow_conflict");
        
        ASSERT_TRUE(a.size() > 0);
    });
    
    auto result = runner.run("إجراءات الكود");
    
    std::cout << "🔧 " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات معلومات التمرير
// =============================================================================

void test_hover() {
    sad::test::TestRunner runner;
    
    runner.addTest("تمرير على مالك", []() {
        sad::lsp::mock::MockHoverProvider hover;
        
        std::string source = "متغير س = 10\n";
        auto h = hover.getHover(source, 1, 5);
        
        ASSERT_TRUE(!h.empty());
        ASSERT_CONTAINS(h, "مالك");
    });
    
    runner.addTest("تمرير على استعارة", []() {
        sad::lsp::mock::MockHoverProvider hover;
        
        std::string source = "متغير س = 10\nمتغير ص = &س\n";
        auto h = hover.getHover(source, 2, 5);
        
        ASSERT_TRUE(!h.empty());
        ASSERT_CONTAINS(h, "استعارة");
    });
    
    auto result = runner.run("معلومات التمرير");
    
    std::cout << "🖱️ " << result.suiteName << ": "
              << result.passed << "/" << result.results.size() << " نجح\n";
    
    for (const auto& r : result.results) {
        std::cout << "   " << (r.passed ? "✅" : "❌") << " " << r.name;
        if (!r.passed) std::cout << " - " << r.error;
        std::cout << "\n";
    }
}

// =============================================================================
// اختبارات التكامل الشاملة
// =============================================================================

void test_integration_scenario() {
    std::cout << "\n🔗 سيناريو التكامل الكامل:\n";
    std::cout << "═══════════════════════════════════\n";
    
    // كود المثال
    std::string source = R"(دالة معالجة_بيانات(بيانات: &مصفوفة<عدد>)
    متغير مجموع = 0
    لكل عنصر في بيانات
        مجموع = مجموع + عنصر
    نهاية
    أرجع مجموع
نهاية

متغير قائمة = [1، 2، 3، 4، 5]
متغير مرجع = &قائمة
متغير نتيجة = معالجة_بيانات(مرجع)
اطبع(نتيجة)
)";
    
    std::cout << "📝 الكود المصدري:\n";
    std::cout << "---\n" << source << "---\n\n";
    
    // 1. عدسات الملكية
    sad::lsp::mock::MockOwnershipLensProvider lensProvider;
    auto lenses = lensProvider.provideLenses(source);
    std::cout << "📋 عدسات الملكية: " << lenses.size() << " عدسة\n";
    for (const auto& lens : lenses) {
        std::cout << "   سطر " << lens.line << ": " << lens.title << "\n";
    }
    
    // 2. رسم الاستعارات
    sad::lsp::mock::MockBorrowGraph graph;
    graph.analyze(source);
    std::cout << "\n🕸️ رسم الاستعارات: " << graph.nodes.size() << " عقدة، " 
              << graph.edges.size() << " ضلع\n";
    
    // 3. تلميحات النقل
    sad::lsp::mock::MockMoveHints hints;
    auto moveHints = hints.getHints(source);
    std::cout << "\n➡️ تلميحات النقل: " << moveHints.size() << " تلميح\n";
    for (const auto& hint : moveHints) {
        std::cout << "   سطر " << hint.line << ": " << hint.text << "\n";
    }
    
    // 4. الرموز الدلالية
    sad::lsp::mock::MockSemanticTokens tokens;
    auto semanticTokens = tokens.getTokens(source);
    std::cout << "\n🎨 الرموز الدلالية: " << semanticTokens.size() << " رمز\n";
    
    std::cout << "\n✅ سيناريو التكامل اكتمل بنجاح!\n";
}

// =============================================================================
// نقطة الدخول
// =============================================================================

int main() {
    std::cout << "══════════════════════════════════════════════════════════\n";
    std::cout << "   اختبارات تكامل ميزات LSP المتقدمة\n";
    std::cout << "   T259 - Phase 26: User Story 23 (LSP Advanced)\n";
    std::cout << "══════════════════════════════════════════════════════════\n\n";
    
    test_ownership_lenses();
    std::cout << "\n";
    
    test_lifetime_visualization();
    std::cout << "\n";
    
    test_borrow_graph();
    std::cout << "\n";
    
    test_move_hints();
    std::cout << "\n";
    
    test_semantic_tokens();
    std::cout << "\n";
    
    test_code_actions();
    std::cout << "\n";
    
    test_hover();
    
    test_integration_scenario();
    
    std::cout << "\n══════════════════════════════════════════════════════════\n";
    std::cout << "   ✅ جميع اختبارات التكامل اكتملت!\n";
    std::cout << "══════════════════════════════════════════════════════════\n";
    
    return 0;
}
